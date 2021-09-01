/*
 * Copyright (c) 2017 Jun Zhao
 * Copyright (c) 2017 Kaixuan Liu
 *
 * HW Acceleration API (video decoding) decode sample
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 /**
  * @file
  * HW-Accelerated decoding example.
  *
  * @example hw_decode.c
  * This example shows how to do HW-accelerated decoding with output
  * frames from the HW video surfaces.
  */
#include "stdafx.h"
#include <stdio.h>

#include "loader/FFMpeg_loader.h"

extern long long inframe, outframe, duration;
static AVBufferRef* hw_device_ctx = NULL;
static enum AVPixelFormat hw_pix_fmt = AV_PIX_FMT_NONE;
static FILE* output_file = NULL;

static int hw_decoder_init(AVCodecContext* ctx, const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = FFMpeg::av_hwdevice_ctx_create(&hw_device_ctx, type,
        NULL, NULL, 0)) < 0) {
        fprintf(stderr, "Failed to create specified HW device.\n");
        return err;
    }
    ctx->hw_device_ctx = FFMpeg::av_buffer_ref(hw_device_ctx);

    return err;
}

static enum AVPixelFormat get_hw_format(AVCodecContext* ctx,
    const enum AVPixelFormat* pix_fmts)
{
    const enum AVPixelFormat* p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }

    fprintf(stderr, "Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}

static int decode_write(AVCodecContext* avctx, AVPacket* packet)
{
    AVFrame* frame = NULL, * sw_frame = NULL;
    AVFrame* tmp_frame = NULL;
    uint8_t* buffer = NULL;
    int size;
    int ret = 0;

    ret = FFMpeg::avcodec_send_packet(avctx, packet);
    if (ret < 0) {
        fprintf(stderr, "Error during decoding\n");
        return ret;
    }

    while (1) {
        if (!(frame = FFMpeg::av_frame_alloc()) || !(sw_frame = FFMpeg::av_frame_alloc())) {
            fprintf(stderr, "Can not alloc frame\n");
            ret = AVERROR(ENOMEM);
            goto fail;
        }

        ret = FFMpeg::avcodec_receive_frame(avctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            FFMpeg::av_frame_free(&frame);
            FFMpeg::av_frame_free(&sw_frame);
            return 0;
        }
        else if (ret < 0) {
            fprintf(stderr, "Error while decoding\n");
            goto fail;
        }

        outframe++;

        if (frame->format == hw_pix_fmt) {
            /* retrieve data from GPU to CPU */
            ret = FFMpeg::av_hwframe_map(sw_frame, frame, AV_HWFRAME_MAP_READ);
            if(ret == AVERROR(ENOSYS))
                ret = FFMpeg::av_hwframe_transfer_data(sw_frame, frame, 0);
            if (ret < 0) {
                fprintf(stderr, "Error transferring the data to system memory\n");
                goto fail;
            }
            sw_frame->width = frame->width;
            sw_frame->height = frame->height;
            tmp_frame = sw_frame;
        }
        else
            tmp_frame = frame;
#if 0
        size = FFMpeg::av_image_get_buffer_size((AVPixelFormat)tmp_frame->format, tmp_frame->width,
            tmp_frame->height, 1);
        buffer = (uint8_t*)FFMpeg::av_malloc(size);
        if (!buffer) {
            fprintf(stderr, "Can not alloc buffer\n");
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        ret = FFMpeg::av_image_copy_to_buffer(buffer, size,
            (const uint8_t* const*)tmp_frame->data,
            (const int*)tmp_frame->linesize, (AVPixelFormat)tmp_frame->format,
            tmp_frame->width, tmp_frame->height, 1);
        if (ret < 0) {
            fprintf(stderr, "Can not copy image to buffer\n");
            goto fail;
        }

        if ((ret = fwrite(buffer, 1, size, output_file)) < 0) {
            fprintf(stderr, "Failed to dump raw data.\n");
            goto fail;
        }
#endif
    fail:
        FFMpeg::av_frame_free(&frame);
        FFMpeg::av_frame_free(&sw_frame);
        FFMpeg::av_freep(&buffer);
        if (ret < 0)
            return ret;
    }
}

int hw_dec(const char *dec_name, const char* input, const char* outfile)
{
    AVFormatContext* input_ctx = NULL;
    int video_stream, ret;
    AVStream* video = NULL;
    AVCodecContext* decoder_ctx = NULL;
    AVCodec* decoder = NULL;
    AVPacket packet;
    enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
    int i;

    /*
    "cuda",
    "drm",
    "dxva2",
    "d3d11va",
    "opencl",
    "qsv",
    "vaapi",
    "vdpau",
    "videotoolbox",
    "mediacodec",
    "vulkan"
    */

    type = FFMpeg::av_hwdevice_find_type_by_name(dec_name);
    if (type == AV_HWDEVICE_TYPE_NONE) {
        fprintf(stderr, "Device type %s is not supported.\n", dec_name);
        fprintf(stderr, "Available device types:");
        while ((type = FFMpeg::av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
            fprintf(stderr, " %s", FFMpeg::av_hwdevice_get_type_name(type));
        fprintf(stderr, "\n");
        return -1;
    }

    /* open the input file */
    if (FFMpeg::avformat_open_input(&input_ctx, input, NULL, NULL) != 0) {
        fprintf(stderr, "Cannot open input file '%s'\n", input);
        return -1;
    }

    if (FFMpeg::avformat_find_stream_info(input_ctx, NULL) < 0) {
        fprintf(stderr, "Cannot find input stream information.\n");
        return -1;
    }

    /* find the video stream information */
    ret = FFMpeg::av_find_best_stream(input_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (ret < 0) {
        fprintf(stderr, "Cannot find a video stream in the input file\n");
        return -1;
    }
    video_stream = ret;

    for (i = 0;; i++) {
        const AVCodecHWConfig* config = FFMpeg::avcodec_get_hw_config(decoder, i);
        if (!config) {
            fprintf(stderr, "Decoder %s does not support device type %s.\n",
                decoder->name, FFMpeg::av_hwdevice_get_type_name(type));
            return -1;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
            config->device_type == type) {
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }

    if (!(decoder_ctx = FFMpeg::avcodec_alloc_context3(decoder)))
        return AVERROR(ENOMEM);

    video = input_ctx->streams[video_stream];
    if (FFMpeg::avcodec_parameters_to_context(decoder_ctx, video->codecpar) < 0)
        return -1;

    decoder_ctx->get_format = get_hw_format;

    if (hw_decoder_init(decoder_ctx, type) < 0)
        return -1;

    if ((ret = FFMpeg::avcodec_open2(decoder_ctx, decoder, NULL)) < 0) {
        fprintf(stderr, "Failed to open codec for stream #%u\n", video_stream);
        return -1;
    }

    /* open the file to dump raw data */
    ret = fopen_s(&output_file, outfile, "w+");

    clock_t du_start = clock();
    /* actual decoding and dump the raw data */
    while (ret >= 0) {
        clock_t start = clock();
        inframe++;
        if ((ret = FFMpeg::av_read_frame(input_ctx, &packet)) < 0)
            break;

        if (video_stream == packet.stream_index)
            ret = decode_write(decoder_ctx, &packet);

        clock_t end = clock();

        clock_t duration = end - du_start;

        double second = duration * 1.0f / CLOCKS_PER_SEC;
        fprintf(stderr, "\r[qsv] in (%lld) %.2f - (%lld)%.2f", inframe, inframe / second, outframe, outframe / second);

        FFMpeg::av_packet_unref(&packet);
    }

    /* flush the decoder */
    packet.data = NULL;
    packet.size = 0;
    ret = decode_write(decoder_ctx, &packet);
    FFMpeg::av_packet_unref(&packet);

    if (output_file)
        fclose(output_file);
    FFMpeg::avcodec_free_context(&decoder_ctx);
    FFMpeg::avformat_close_input(&input_ctx);
    FFMpeg::av_buffer_unref(&hw_device_ctx);

    return 0;
}
