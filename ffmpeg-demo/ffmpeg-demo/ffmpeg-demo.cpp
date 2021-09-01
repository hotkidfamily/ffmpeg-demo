// ffmpeg-demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <memory>

#pragma warning(disable: 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loader/FFMpeg_loader.h"

#include "decoding.h"

#define INBUF_SIZE 18000

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
    char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename, "w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}


static int decode_write_frame(const char *outfilename, AVCodecContext *avctx,
    AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;
    char buf[1024];

    len = FFMpeg::avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", *frame_count);
        return len;
    }
    if (got_frame) {
        printf("Saving %sframe %3d\n", last ? "last " : "", *frame_count);
        fflush(stdout);

        /* the picture is allocated by the decoder, no need to free it */
        snprintf(buf, sizeof(buf), "%s-%d.pgm", outfilename, *frame_count);

        pgm_save(frame->data[0], frame->linesize[0],
            frame->width, frame->height, buf);
        (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}

int ffmpeg_decoder_main(char * filename, char *outfilename, char * codecname)
{
    const AVCodec *codec;
    AVCodecContext *c = NULL;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;
#if 0
    FFMpeg::avcodec_register_all();
#endif
    FFMpeg::av_init_packet(&avpkt);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the MPEG-1 video decoder */
    //codec = FFMpeg::avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    codec = FFMpeg::avcodec_find_decoder_by_name(codecname);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = FFMpeg::avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED)
        c->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames

                                                /* For some codecs, such as msmpeg4 and mpeg4, width and height
                                                MUST be initialized there because this information is not
                                                available in the bitstream. */

                                                /* open it */
    if (FFMpeg::avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = FFMpeg::av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    frame_count = 0;
    for (;;) {
        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0)
            break;

        /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
        and this is the only method to use them because you cannot
        know the compressed data size before analysing it.

        BUT some other codecs (msmpeg4, mpeg4) are inherently frame
        based, so you must call them with all the data for one
        frame exactly. You must also initialize 'width' and
        'height' before initializing them. */

        /* NOTE2: some codecs allow the raw parameters (frame size,
        sample rate) to be changed at any frame. We handle this, so
        you should also take care of it */

        /* here, we use a stream based decoder (mpeg1video), so we
        feed decoder and see if it could decode a frame */
        avpkt.data = inbuf;
        while (avpkt.size > 0)
            if (decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 0) < 0)
            {
                printf("end");
                break;
            }
    }

    /* Some codecs, such as MPEG, transmit the I- and P-frame with a
    latency of one frame. You must do the following to have a
    chance to get the last frame of the video. */
    avpkt.data = NULL;
    avpkt.size = 0;
    decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 1);

    fclose(f);

    FFMpeg::avcodec_free_context(&c);
    FFMpeg::av_frame_free(&frame);

    return 0;
}

static void fn_log_cb(void *ptr, int level, const char *fmt, va_list vl)
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

    printf("%s, %s", moduleName, msg);

    FFMpeg::av_freep(&msg);
}

int ffmpeg_main(char * outname, char *codecname)
{
	const char *filename, *codec_name;
	const AVCodec *codec;
	AVCodecContext *c = NULL;
	int i, ret, x, y, got_output;
	FILE *f;
	AVFrame *frame;
	AVPacket pkt;
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };

	filename = outname;
	codec_name = codecname;

	FFMpeg::avcodec_register_all();

	/* find the mpeg1video encoder */
	codec = FFMpeg::avcodec_find_encoder_by_name(codec_name);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}

	c = FFMpeg::avcodec_alloc_context3(codec);
	if (!c) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}

	/* put sample parameters */
	c->bit_rate = 1200000;
	/* resolution must be a multiple of two */
	c->width = 800;
	c->height = 600;
	/* frames per second */
	AVRational time = { 1, 25 };
	AVRational fps = { 25, 1 };
	c->time_base = time;
	c->framerate = fps;

	/* emit one intra frame every ten frames
	* check frame pict_type before passing frame
	* to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
	* then gop_size is ignored and the output of encoder
	* will always be I frame irrespective to gop_size
	*/
	c->gop_size = 10;
	c->max_b_frames = 1;
	c->pix_fmt = AV_PIX_FMT_YUV420P;

	if (codec->id == AV_CODEC_ID_H264)
		FFMpeg::av_opt_set(c->priv_data, "preset", "slow", 0);

	const char *config = "preset=yy:temporalnum=2:spatialid=1:annexg=1:keyint=48:min-keyint=48:scenecut=0:bframes=0:b-adapt=0:b-pyramid=none:threads=1:sliced-threads=0:ref=1:subme=6:me=hex:analyse=i4x4,i8x8,p8x8,p4x4,b8x8:direct=spatial:weightp=1:weightb=1:8x8dct=1:cabac=1:deblock=0,0:psy=0:trellis=0:aq-mode=1:rc-lookahead=0:sync-lookahead=0:mbtree=0:force-cfr=0";
	FFMpeg::av_opt_set(c->priv_data, "x264-params", config, NULL);

    FFMpeg::av_log_set_callback(fn_log_cb);

	/* open it */
    ret = FFMpeg::avcodec_open2(c, codec, NULL);
	if (ret < 0) {
		fprintf(stderr, "Could not open codec %d\n", ret);
		exit(1);
	}

	f = fopen(filename, "wb");
	if (!f) {
		fprintf(stderr, "Could not open %s\n", filename);
		exit(1);
	}

	frame = FFMpeg::av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}
	frame->format = c->pix_fmt;
	frame->width = c->width;
	frame->height = c->height;

	ret = FFMpeg::av_frame_get_buffer(frame, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate the video frame data\n");
		exit(1);
	}

	/* encode 1 second of video */
	for (i = 0; i < 1000; i++) {
		FFMpeg::av_init_packet(&pkt);
		pkt.data = NULL;    // packet data will be allocated by the encoder
		pkt.size = 0;

		fflush(stdout);

		/* make sure the frame data is writable */
		ret = FFMpeg::av_frame_make_writable(frame);
		if (ret < 0)
			exit(1);

		/* prepare a dummy image */
		/* Y */
		for (y = 0; y < c->height; y++) {
			for (x = 0; x < c->width; x++) {
				frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
			}
		}

		/* Cb and Cr */
		for (y = 0; y < c->height / 2; y++) {
			for (x = 0; x < c->width / 2; x++) {
				frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
				frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
			}
		}

		frame->pts = i;

		/* encode the image */
		ret = FFMpeg::avcodec_encode_video2(c, &pkt, frame, &got_output);
		if (ret < 0) {
			fprintf(stderr, "Error encoding frame\n");
			exit(1);
		}

		if (got_output) {
			printf("Write frame %3d (size=%5d)\n", i, pkt.size);
			fwrite(pkt.data, 1, pkt.size, f);
			FFMpeg::av_packet_unref(&pkt);
		}
	}

	/* get the delayed frames */
	for (got_output = 1; got_output; i++) {
		fflush(stdout);

		ret = FFMpeg::avcodec_encode_video2(c, &pkt, NULL, &got_output);
		if (ret < 0) {
			fprintf(stderr, "Error encoding frame\n");
			exit(1);
		}

		if (got_output) {
			printf("Write frame %3d (size=%5d)\n", i, pkt.size);
			fwrite(pkt.data, 1, pkt.size, f);
			FFMpeg::av_packet_unref(&pkt);
		}
	}

	/* add sequence end code to have a real MPEG file */
	fwrite(endcode, 1, sizeof(endcode), f);
	fclose(f);

	FFMpeg::avcodec_free_context(&c);
	FFMpeg::av_frame_free(&frame);

	return 0;
}

extern int qsv_dec_2(const char * argv, const char* argv2);
extern int hw_dec(const char* hw_name, const char* input, const char* output);

#include <windows.h>
#include <Shlwapi.h>
#include <string>

#pragma comment(lib, "Shlwapi.lib")

inline std::wstring GetFileDirFromPath(LPCTSTR filepath)
{
    size_t orgLen = _tcslen(filepath);
    TCHAR end = filepath[orgLen - 1];
    if (end != _T('\\') && end != _T(':'))
    {
        LPCTSTR FN = PathFindFileName(filepath);
        return std::wstring(filepath, orgLen - _tcslen(FN));
    }
    else
    {
        return std::wstring(filepath);
    }
}

inline std::wstring GetCurrentPathDir()
{
    TCHAR buffer[MAX_PATH];
    ZeroMemory(buffer, sizeof(TCHAR) * MAX_PATH);
    ::GetModuleFileName(NULL, buffer, MAX_PATH);
    return GetFileDirFromPath(buffer);
}

int main(int argc, char *argv[])
{
	auto w = GetCurrentPathDir();
	FFMpeg::InitModules(w.c_str(), nullptr);
    FFMpeg::avcodec_register_all();
    FFMpeg::avformat_network_init();


	//ffmpeg_main("a.mp4", "libx264");
	/*
	const char* input = "D:\\NTTS\\resources\\movie\\Story - of - Earth - 4k.mp4";
	const char* input = "D:\\NTTS\\resources\\movie\\gt-music.mp4";
	const char* input = "D:\\temps\\pubg\\1080p-pubg-crf\\pubg-sand-cloud-1920x1080p-60fps.crf-20.h264";
	const char* input = "D:\\NTTS\\resources\\movie\\NBA Season 2012 2013 Best Plays (Full HD 1080p).mp4";
	const char* input = "D:\\NTTS\\resources\\movie\\Interstellar.2014.星际穿越.720p.mp4";
	const char* input = "D:\\temps\\720p_3p_dance\\265_crf_limited_2500\\dance_3p_1280x720_25_c950.crf-17.h265";
	const char* input = "D:\\NTTS\\resources\\HEVC-MOVIE\\flv1453.h265";
	*/
	FFMpeg::av_log_set_level(AV_LOG_VERBOSE);
	const char* info = FFMpeg::avformat_configuration();
	printf("%s\n", info);
	const char* input = "D:\\NTTS\\resources\\HEVC-MOVIE\\flv1453.h265";
	const char* output = "c:\\temps\\out.yuv";
	const char* aoutput = "c:\\temps\\out.pcm";
    //decoder(input, output, aoutput);
	//qsv_dec_2(input, output);

    hw_dec("dxva2", input, output);

	FFMpeg::UnInitModules();
    return 0;
}

