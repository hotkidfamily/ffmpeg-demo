#include "stdafx.h"

#include <stdio.h>

#include "loader/FFMpeg_loader.h"


typedef struct DecodeContext {
    AVBufferRef* hw_device_ref;
} DecodeContext;

long long inframe = 0, outframe = 0, duration = 0;

static AVPixelFormat get_format(AVCodecContext* avctx, const enum AVPixelFormat* pix_fmts)
{
    while (*pix_fmts != AV_PIX_FMT_NONE) {
        if (*pix_fmts == AV_PIX_FMT_QSV) {
            DecodeContext* decode = (DecodeContext*)avctx->opaque;
            AVHWFramesContext* frames_ctx;
            AVQSVFramesContext* frames_hwctx;
            int ret;

            /* create a pool of surfaces to be used by the decoder */
            avctx->hw_frames_ctx = FFMpeg::av_hwframe_ctx_alloc(decode->hw_device_ref);
            if (!avctx->hw_frames_ctx)
                return AV_PIX_FMT_NONE;
            frames_ctx = (AVHWFramesContext*)avctx->hw_frames_ctx->data;
            frames_hwctx = (AVQSVFramesContext *)frames_ctx->hwctx;

            frames_ctx->format = AV_PIX_FMT_QSV;
            frames_ctx->sw_format = avctx->sw_pix_fmt;
            frames_ctx->width = FFALIGN(avctx->coded_width, 32);
            frames_ctx->height = FFALIGN(avctx->coded_height, 32);
            frames_ctx->initial_pool_size = 32;

            frames_hwctx->frame_type = MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;

            ret = FFMpeg::av_hwframe_ctx_init(avctx->hw_frames_ctx);
            if (ret < 0)
                return AV_PIX_FMT_NONE;

            return AV_PIX_FMT_QSV;
        }

        pix_fmts++;
    }

    fprintf(stderr, "The QSV pixel format not offered in get_format()\n");

    return AV_PIX_FMT_NONE;
}

#undef av_err2str
#include <functional>
extern std::function<char* (int)> av_err2str;

static int decode_packet(DecodeContext* decode, AVCodecContext* decoder_ctx,
    AVFrame* frame, AVFrame* sw_frame,
    AVPacket* pkt, AVIOContext* output_ctx)
{
    int ret = 0;

//     AVPacket pktheadr = { 0 };
//     pktheadr.data = pkt->data;
//     pktheadr.size = 0x53;
//     ret = FFMpeg::avcodec_send_packet(decoder_ctx, pkt);
// 
//     pkt->data = pkt->data + 0x53;
//     pkt->size -= 0x53;

    ret = FFMpeg::avcodec_send_packet(decoder_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error during decoding\n");
        return ret;
    }

    while (ret >= 0) {
        int i, j;

        ret = FFMpeg::avcodec_receive_frame(decoder_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            return ret;
        }

        /* A real program would do something useful with the decoded frame here.
         * We just retrieve the raw data and write it to a file, which is rather
         * useless but pedagogic. */
        //ret = FFMpeg::av_hwframe_transfer_data(sw_frame, frame, 0);
        ret = FFMpeg::av_hwframe_map(sw_frame, frame, AV_HWFRAME_MAP_READ);
        sw_frame->width = frame->width;
        sw_frame->height = frame->height;
        if (ret < 0) {
            fprintf(stderr, "Error transferring the data to system memory, %x - %s\n", ret, av_err2str(ret));
            goto fail;
        }

        outframe++;
 
 		for (i = 0; i < FF_ARRAY_ELEMS(sw_frame->data) && sw_frame->data[i]; i++)
        	for (j = 0; j < (sw_frame->height >> (i > 0)); j++)
                FFMpeg::avio_write(output_ctx, sw_frame->data[i] + j * sw_frame->linesize[i], sw_frame->width);

    fail:
        FFMpeg::av_frame_unref(sw_frame);
        FFMpeg::av_frame_unref(frame);

        if (ret < 0)
            return ret;
    }

    return 0;
}

int qsv_dec_2(const char* argv, const char* argv2)
{
    AVFormatContext* input_ctx = NULL;
    AVStream* video_st = NULL;
    AVCodecContext* decoder_ctx = NULL;
    const AVCodec* decoder;

    AVPacket pkt = { 0 };
    AVFrame* frame = NULL, * sw_frame = NULL;

    DecodeContext decode = { NULL };

    AVIOContext* output_ctx = NULL;

    //FFMpeg::av_log_set_level(AV_LOG_VERBOSE);

    fprintf(stderr, "input %s -> output %s ", argv, argv2);

    int ret, i;

    /* open the input file */
    ret = FFMpeg::avformat_open_input(&input_ctx, argv, NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "Cannot open input file '%s': ", argv);
        goto finish;
    }

    /* find the first H.264 video stream */
    for (i = 0; i < input_ctx->nb_streams; i++) {
        AVStream* st = input_ctx->streams[i];

        if (st->codecpar->codec_id == AV_CODEC_ID_H264 && !video_st)
            video_st = st;
        else if (st->codecpar->codec_id == AV_CODEC_ID_HEVC && !video_st)
            video_st = st;
        else
            st->discard = AVDISCARD_ALL;
    }
    if (!video_st) {
        fprintf(stderr, "No H.264 video stream in the input file\n");
        goto finish;
    }

    AVDictionary* dict = NULL;
    int err = FFMpeg::av_dict_set(&dict, "child_device", "1", 0);
    err = FFMpeg::av_dict_set(&dict, "gpu_copy", "on", 0);
    /* open the hardware device */
    ret = FFMpeg::av_hwdevice_ctx_create(&decode.hw_device_ref, AV_HWDEVICE_TYPE_QSV,
        "hw_any", dict, 0);
    if (ret < 0) {
        fprintf(stderr, "Cannot open the hardware device\n");
        goto finish;
    } 

    /* initialize the decoder */
    if(video_st->codecpar->codec_id == AV_CODEC_ID_H264)
        decoder = FFMpeg::avcodec_find_decoder_by_name("h264_qsv");
    else if (video_st->codecpar->codec_id == AV_CODEC_ID_HEVC)
        decoder = FFMpeg::avcodec_find_decoder_by_name("hevc_qsv");
    if (!decoder) {
        fprintf(stderr, "The QSV decoder is not present in libavcodec\n");
        goto finish;
    }

    decoder_ctx = FFMpeg::avcodec_alloc_context3(decoder);
    if (!decoder_ctx) {
        ret = AVERROR(ENOMEM);
        goto finish;
    }
    decoder_ctx->codec_id = video_st->codecpar->codec_id;
    if (video_st->codecpar->extradata_size) {
        decoder_ctx->extradata = (uint8_t*)FFMpeg::av_mallocz(video_st->codecpar->extradata_size +
            AV_INPUT_BUFFER_PADDING_SIZE);
        if (!decoder_ctx->extradata) {
            ret = AVERROR(ENOMEM);
            goto finish;
        }
        memcpy(decoder_ctx->extradata, video_st->codecpar->extradata,
            video_st->codecpar->extradata_size);
        decoder_ctx->extradata_size = video_st->codecpar->extradata_size;
    }

    decoder_ctx->opaque = &decode;
    decoder_ctx->get_format = get_format;

    ret = FFMpeg::avcodec_open2(decoder_ctx, NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error opening the decoder: ");
        goto finish;
    }

    /* open the output stream */
    ret = FFMpeg::avio_open(&output_ctx, argv2, AVIO_FLAG_WRITE);
    if (ret < 0) {
        fprintf(stderr, "Error opening the output context: ");
        goto finish;
    }

    frame = FFMpeg::av_frame_alloc();
    sw_frame = FFMpeg::av_frame_alloc();
    if (!frame || !sw_frame) {
        ret = AVERROR(ENOMEM);
        goto finish;
    }

    //clock_t clock(void);
    clock_t du_start = clock();
    /* actual decoding */
    while (ret >= 0) {
        clock_t start = clock();
        inframe++;
        ret = FFMpeg::av_read_frame(input_ctx, &pkt);
        if (ret < 0)
            break;

        if (pkt.stream_index == video_st->index)
            ret = decode_packet(&decode, decoder_ctx, frame, sw_frame, &pkt, output_ctx);

        clock_t end = clock();

        clock_t duration = end - du_start;

        double second = duration * 1.0f /CLOCKS_PER_SEC;
        fprintf(stderr, "\r[qsv] in (%lld) %.2f - (%lld)%.2f", inframe, inframe / second, outframe, outframe / second);

        FFMpeg::av_packet_unref(&pkt);
    }

    /* flush the decoder */
    pkt.data = NULL;
    pkt.size = 0;
    ret = decode_packet(&decode, decoder_ctx, frame, sw_frame, &pkt, output_ctx);

finish:
    if (ret < 0) {
        char buf[1024];
        FFMpeg::av_strerror(ret, buf, sizeof(buf));
        fprintf(stderr, "%s\n", buf);
    }

    FFMpeg::avformat_close_input(&input_ctx);

    FFMpeg::av_frame_free(&frame);
    FFMpeg::av_frame_free(&sw_frame);

    FFMpeg::avcodec_free_context(&decoder_ctx);

    FFMpeg::av_buffer_unref(&decode.hw_device_ref);

    FFMpeg::avio_close(output_ctx);

    return ret;
}
