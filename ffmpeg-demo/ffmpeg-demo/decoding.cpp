#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "loader/FFMpeg_loader.h"

#pragma warning(disable: 4996)

const char *decoders[] = {
    "aac",
    "h264_cuvid",
    "mjpeg",
    "ac3",
    "h264_qsv",
    "mp2",
    "bmp",
    "hevc",
    "mp3",
    "eac3",
    "hevc_cuvid",
    "mpeg2video",
    "flv",
    "hevc_qsv",
    "mpeg4",
    "h263",
    "libfdk_aac",
    "rawvideo",
    "h264",
    "libittiam265",
    "vorbis"
};


#include "decoding.h"

 #undef av_err2str
// #define av_err2str(x) #x

#include <functional>

std::function<char*(int)> av_err2str = [&](int errnum) {
    static char info[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    FFMpeg::av_strerror(errnum, info, AV_ERROR_MAX_STRING_SIZE);
    return info;
};

extern long long inframe, outframe, duration;

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
static int width, height;
static enum AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
static AVStream *video_stream = NULL, *audio_stream = NULL;
static const char *src_filename = NULL;
static const char *video_dst_filename = NULL;
static const char *audio_dst_filename = NULL;
static FILE *video_dst_file = NULL;
static FILE *audio_dst_file = NULL;

static uint8_t *video_dst_data[4] = { NULL };
static int      video_dst_linesize[4];
static int video_dst_bufsize;

static int video_stream_idx = -1, audio_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;
static int video_frame_count = 0;
static int audio_frame_count = 0;

/* Enable or disable frame reference counting. You are not supposed to support
* both paths in your application but pick the one most appropriate to your
* needs. Look for the use of refcount in this example to see what are the
* differences of API usage between them. */
static int refcount = 0;

static int decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;

    *got_frame = 0;

    if (pkt.stream_index == video_stream_idx) {
        /* decode video frame */
        ret = FFMpeg::avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(ret));
            return ret;
        }

        if (*got_frame) {

            if (frame->width != width || frame->height != height || frame->format != pix_fmt) {
                if (video_dst_data[0]) {
                    FFMpeg::av_free(video_dst_data[0]);
                }

                {
                    ret = FFMpeg::av_image_alloc(video_dst_data, video_dst_linesize,
                        frame->width, frame->height, (AVPixelFormat)frame->format, 1);
                    if (ret < 0) {
                        fprintf(stderr, "Could not allocate raw video buffer\n");
                        exit(-1);
                    }
                    video_dst_bufsize = ret;
                }

                pix_fmt = (AVPixelFormat)frame->format;

                /* To handle this change, one could call FFMpeg::av_image_alloc again and
                * decode the following frames into another rawvideo file. */
                fprintf(stderr, "Error: Width, height and pixel format have to be "
                    "constant in a rawvideo file, but the width, height or "
                    "pixel format of the input video changed:\n"
                    "old: width = %d, height = %d, format = %s\n"
                    "new: width = %d, height = %d, format = %s\n",
                    width, height, FFMpeg::av_get_pix_fmt_name(pix_fmt),
                    frame->width, frame->height,
                    FFMpeg::av_get_pix_fmt_name((AVPixelFormat)frame->format));
                //return -1;
            }

//             printf("video_frame%s n:%d coded_n:%d\n",
//                 cached ? "(cached)" : "",
//                 video_frame_count++, frame->coded_picture_number);

            /* copy decoded frame to destination buffer:
            * this is required since rawvideo expects non aligned data */
            FFMpeg::av_image_copy(video_dst_data, video_dst_linesize,
                (const uint8_t **)(frame->data), frame->linesize,
                pix_fmt, width, height);

            outframe++;
            /* write to rawvideo file */
            fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
        }
    }
    else if (pkt.stream_index == audio_stream_idx) {
        /* decode audio frame */
        ret = FFMpeg::avcodec_decode_audio4(audio_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding audio frame (%s)\n", av_err2str(ret));
            return ret;
        }
        /* Some audio decoders decode only part of the packet, and have to be
        * called again with the remainder of the packet data.
        * Sample: fate-suite/lossless-audio/luckynight-partial.shn
        * Also, some decoders might over-read the packet. */
        decoded = FFMIN(ret, pkt.size);

        if (*got_frame) {
            size_t unpadded_linesize = frame->nb_samples * FFMpeg::av_get_bytes_per_sample((AVSampleFormat)frame->format);
/*
            printf("audio_frame%s n:%d nb_samples:%d pts:%s\n",
                cached ? "(cached)" : "",
                audio_frame_count++, frame->nb_samples,
                av_ts2timestr(frame->pts, &audio_dec_ctx->time_base));*/

            /* Write the raw audio data samples of the first plane. This works
            * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
            * most audio decoders output planar audio, which uses a separate
            * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
            * In other words, this code will write only the first audio channel
            * in these cases.
            * You should use libswresample or libavfilter to convert the frame
            * to packed data. */
            fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
        }
    }

    /* If we use frame reference counting, we own the data and need
    * to de-reference it when we don't use it anymore */
    if (*got_frame && refcount)
        FFMpeg::av_frame_unref(frame);

    return decoded;
}

static int open_codec_context(int *stream_idx,
    AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = FFMpeg::av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
            FFMpeg::av_get_media_type_string(type), src_filename);
        return ret;
    }
    else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];
#if 0
        for each (auto str in decoders)
        {
            dec = FFMpeg::avcodec_find_decoder_by_name(str);
            fprintf(stderr, "%s to find %s codec\n", dec ? "success" : "failed", str);
        }
#endif
        /* find decoder for the stream */

        if (st->codecpar->codec_id == AV_CODEC_ID_H265){
            dec = FFMpeg::avcodec_find_decoder_by_name("hevc_cuvid");
            if (!dec) {
                FFMpeg::avcodec_find_decoder_by_name("hevc_qsv");
            }
            if (!dec) {
                FFMpeg::avcodec_find_decoder_by_name("hevc_dxva2");
            }
        }
        else if (st->codecpar->codec_id == AV_CODEC_ID_H264) {
            dec = FFMpeg::avcodec_find_decoder_by_name("h264_cuvid");
            if (!dec) {
                FFMpeg::avcodec_find_decoder_by_name("h264_qsv");
            }
            if (!dec) {
                FFMpeg::avcodec_find_decoder_by_name("h264_dxva2");
            }
        }
        else
            dec = FFMpeg::avcodec_find_decoder(st->codecpar->codec_id);

        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                FFMpeg::av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = FFMpeg::avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                FFMpeg::av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = FFMpeg::avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                FFMpeg::av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders, with or without reference counting */
        FFMpeg::av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
        if ((ret = FFMpeg::avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                FFMpeg::av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

static int get_format_from_sample_fmt(const char **fmt,
    enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8" },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    fprintf(stderr,
        "sample format %s is not supported as output format\n",
        FFMpeg::av_get_sample_fmt_name(sample_fmt));
    return -1;
}
extern "C"{
#include "libavutil/bprint.h"
}
void _func_av_log_callback(void *ptr, int level, const char *fmt, va_list vl) 
{
    char* msg = nullptr;

    AVBPrint msgBuf;
    FFMpeg::av_bprint_init(&msgBuf, 0, 1);
    FFMpeg::av_vbprintf(&msgBuf, fmt, vl);
    FFMpeg::av_bprint_finalize(&msgBuf, &msg);

    const char* moduleName = nullptr;
    AVClass* avc = ptr ? *(AVClass **)ptr : NULL;
    if (avc) {
        moduleName = avc->item_name(ptr);
    }
    if (!moduleName) moduleName = "ffmpeg";


    if (level <= AV_LOG_ERROR) {
        fprintf(stderr, "[%s] %s", moduleName, msg);
    }
    else if (level == AV_LOG_WARNING) {
        fprintf(stderr, "[%s] %s", moduleName, msg);
    }
    else if (level == AV_LOG_INFO) {
        fprintf(stderr, "[%s] %s", moduleName, msg);
    }
    else if (level >= AV_LOG_VERBOSE) {
        fprintf(stderr, "[%s] %s", moduleName, msg);
    }

    FFMpeg::av_freep(&msg);
}

int decoder(const char* filename, const char* vDstName, const char* aDstName)
{
    int ret = 0, got_frame;

    src_filename = filename;
    video_dst_filename = vDstName;
    audio_dst_filename = aDstName;

    //FFMpeg::av_log_set_callback(_func_av_log_callback);
#if 0
    FFMpeg::avcodec_register_all();
#endif
    FFMpeg::av_register_all();

    /* open input file, and allocate format context */
    if ( (ret = FFMpeg::avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0) {
        fprintf(stderr, "Could not open source file %s for %d - %s\n", src_filename, ret , av_err2str(ret));
        exit(1);
    }

    /* retrieve stream information */
    if (FFMpeg::avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];

        video_dst_file = fopen(video_dst_filename, "wb");
        if (!video_dst_file) {
            fprintf(stderr, "Could not open destination file %s\n", video_dst_filename);
            ret = 1;
            goto end;
        }

        /* allocate image where the decoded image will be put */
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;

        {
            ret = FFMpeg::av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);
            if (ret < 0) {
                fprintf(stderr, "Could not allocate raw video buffer\n");
                exit(-1);
            }
            video_dst_bufsize = ret;
        }

    }

    if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
        audio_dst_file = fopen(audio_dst_filename, "wb");
        if (!audio_dst_file) {
            fprintf(stderr, "Could not open destination file %s\n", audio_dst_filename);
            ret = 1;
            goto end;
        }
    }

    /* dump input information to stderr */
    FFMpeg::av_dump_format(fmt_ctx, 0, src_filename, 0);

    if (!audio_stream && !video_stream) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }

    frame = FFMpeg::av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* initialize packet, set data to NULL, let the demuxer fill it */
    FFMpeg::av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    if (video_stream)
        printf("Demuxing video from file '%s' into '%s'\n", src_filename, video_dst_filename);
    if (audio_stream)
        printf("Demuxing audio from file '%s' into '%s'\n", src_filename, audio_dst_filename);


    clock_t du_start = clock();
    /* read frames from the file */
    while (FFMpeg::av_read_frame(fmt_ctx, &pkt) >= 0) {
        clock_t start = clock();
        inframe++;
        AVPacket orig_pkt = pkt;
        do {
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        FFMpeg::av_packet_unref(&orig_pkt);
        clock_t end = clock();

        clock_t duration = end - du_start;

        double second = duration * 1.0f / CLOCKS_PER_SEC;
        fprintf(stderr, "\r[qsv] in (%lld) %.2f - (%lld)%.2f", inframe, inframe / second, outframe, outframe / second);
    }

    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&got_frame, 1);
    } while (got_frame);

    printf("Demuxing succeeded.\n");

    if (video_stream) {
        printf("Play the output video file with the command:\n"
            "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
            FFMpeg::av_get_pix_fmt_name(pix_fmt), width, height,
            video_dst_filename);
    }

    if (audio_stream) {
        enum AVSampleFormat sfmt = audio_dec_ctx->sample_fmt;
        int n_channels = audio_dec_ctx->channels;
        const char *fmt;

        if (FFMpeg::av_sample_fmt_is_planar(sfmt)) {
            const char *packed = FFMpeg::av_get_sample_fmt_name(sfmt);
            printf("Warning: the sample format the decoder produced is planar "
                "(%s). This example will output the first channel only.\n",
                packed ? packed : "?");
            sfmt = FFMpeg::av_get_packed_sample_fmt(sfmt);
            n_channels = 1;
        }

        if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
            goto end;

        printf("Play the output audio file with the command:\n"
            "ffplay -f %s -ac %d -ar %d %s\n",
            fmt, n_channels, audio_dec_ctx->sample_rate,
            audio_dst_filename);
    }

end:
    FFMpeg::avcodec_free_context(&video_dec_ctx);
    FFMpeg::avcodec_free_context(&audio_dec_ctx);
    FFMpeg::avformat_close_input(&fmt_ctx);
    if (video_dst_file)
        fclose(video_dst_file);
    if (audio_dst_file)
        fclose(audio_dst_file);
    FFMpeg::av_frame_free(&frame);
    FFMpeg::av_free(video_dst_data[0]);

    return ret < 0;
}
