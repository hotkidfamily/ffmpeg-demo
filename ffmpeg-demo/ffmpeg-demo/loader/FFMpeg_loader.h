#ifndef __FFMPEG_LOADER_H__
#define __FFMPEG_LOADER_H__

#pragma once

#pragma warning(push)
#pragma warning(disable : 4244 4819)
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/codec.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/bprint.h"
#include "libavutil/buffer.h"
#include "libavutil/avstring.h"
#include "libswresample/swresample.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/fifo.h"
#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/hwcontext.h"
#include "libavutil/hwcontext_qsv.h"
#include "libavutil/mem.h"
}
#pragma warning(pop) 

#define MODULE_EXTERN_FUN_DECLARE(x) extern P##x x

namespace FFMpeg
{
// avcodec
typedef const AVBitStreamFilter *(*fn_av_bsf_get_by_name)(const char *name);
typedef int (*fn_av_bsf_alloc)(const AVBitStreamFilter *filter, AVBSFContext **ctx);
typedef int (*fn_av_bsf_init)(AVBSFContext *ctx);
typedef int (*fn_av_bsf_send_packet)(AVBSFContext *ctx, AVPacket *pkt);
typedef int (*fn_av_bsf_receive_packet)(AVBSFContext *ctx, AVPacket *pkt);
typedef void (*fn_av_init_packet)(AVPacket *pkt);
typedef void (*fn_av_packet_unref)(AVPacket *pkt);
typedef AVCodecContext *(*fn_avcodec_alloc_context3)(const AVCodec *codec);
typedef void (*fn_avcodec_free_context)(AVCodecContext **avctx);
typedef int (*fn_avcodec_open2)(AVCodecContext *avctx,
                                const AVCodec *codec,
                                AVDictionary **options);
typedef int (*fn_avcodec_close)(AVCodecContext *avctx);
typedef int (*fn_avcodec_send_packet)(AVCodecContext *avctx, const AVPacket *avpkt);
typedef int (*fn_avcodec_receive_frame)(AVCodecContext *avctx, AVFrame *frame);
typedef void (*fn_avcodec_flush_buffers)(AVCodecContext *avctx);
typedef AVCodec *(*fn_avcodec_find_encoder)(enum AVCodecID id);
typedef AVCodec *(*fn_avcodec_find_encoder_by_name)(const char *name);
typedef AVCodec *(*fn_avcodec_find_decoder)(enum AVCodecID id);
typedef AVCodec *(*fn_avcodec_find_decoder_by_name)(const char *name);
typedef const AVCodecHWConfig *(*fn_avcodec_get_hw_config)(const AVCodec *codec, int index);
typedef int (*fn_avcodec_parameters_to_context)(AVCodecContext *codec,
                                                const AVCodecParameters *par);
typedef int (*fn_avcodec_parameters_from_context)(AVCodecParameters *par, const AVCodecContext *codec);
typedef AVPacket *(*fn_av_packet_alloc)(void);
typedef void (*fn_av_packet_free)(AVPacket **pkt);
typedef void (*fn_av_packet_rescale_ts)(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);
typedef void (*fn_avcodec_register_all)(void);
typedef int (*fn_avcodec_decode_audio4)(AVCodecContext* avctx, AVFrame* frame, int* got_frame_ptr, const AVPacket* avpkt);
typedef int (*fn_avcodec_decode_video2)(AVCodecContext* avctx, AVFrame* picture, int* got_picture_ptr, const AVPacket* avpkt);
typedef int (*fn_avcodec_encode_video2)(AVCodecContext* avctx, AVPacket* avpkt, const AVFrame* frame, int* got_packet_ptr);


// avformat
typedef const char* (*fn_avformat_configuration)(void);
typedef void (*fn_av_register_all)(void);
typedef int (*fn_avformat_network_init)(void);
typedef int (*fn_avformat_open_input)(AVFormatContext **ps,
                                      const char *url,
                                      ff_const59 AVInputFormat *fmt,
                                      AVDictionary **options);
typedef int (*fn_avformat_find_stream_info)(AVFormatContext *ic, AVDictionary **options);
typedef int (*fn_av_find_best_stream)(AVFormatContext *ic,
                                      enum AVMediaType type,
                                      int wanted_stream_nb,
                                      int related_stream,
                                      AVCodec **decoder_ret,
                                      int flags);
typedef int (*fn_av_read_frame)(AVFormatContext *s, AVPacket *pkt);
typedef void (*fn_avformat_close_input)(AVFormatContext **s);
typedef void (*fn_avio_write)(AVIOContext *s, const unsigned char *buf, int size);
typedef void (*fn_avio_wb32)(AVIOContext *s, unsigned int val);
typedef void (*fn_avio_write_marker)(AVIOContext *s, int64_t time, enum AVIODataMarkerType type);
typedef int64_t (*fn_avio_skip)(AVIOContext *s, int64_t offset);
typedef int          (*fn_avio_r8  )(AVIOContext *s);
typedef unsigned int (*fn_avio_rl32)(AVIOContext *s);
typedef unsigned int (*fn_avio_rb24)(AVIOContext *s);
typedef int (*fn_avio_closep)(AVIOContext **s);
typedef int (*fn_avio_close)(AVIOContext* s);
typedef int (*fn_av_write_trailer)(AVFormatContext *s);
typedef void (*fn_avformat_free_context)(AVFormatContext *s);
typedef int (*fn_avformat_alloc_output_context2)(AVFormatContext **ctx,
                                           ff_const59 AVOutputFormat *oformat,
                                           const char *format_name,
                                           const char *filename);
typedef AVStream *(*fn_avformat_new_stream)(AVFormatContext *s, const AVCodec *c);
typedef void (*fn_av_dump_format)(AVFormatContext *ic, int index, const char *url, int is_output);
typedef int (*fn_avio_open)(AVIOContext **s, const char *url, int flags);
typedef int (*fn_avformat_write_header)(AVFormatContext *s, AVDictionary **options);
typedef int (*fn_av_interleaved_write_frame)(AVFormatContext *s, AVPacket *pkt);
typedef AVIOContext *(*fn_avio_alloc_context)(unsigned char *buffer,
                                int buffer_size,
                                int write_flag,
                                void *opaque,
                                int (*read_packet)(void *opaque, uint8_t *buf, int buf_size),
                                int (*write_packet)(void *opaque, uint8_t *buf, int buf_size),
                                int64_t (*seek)(void *opaque, int64_t offset, int whence));


//avutil
typedef void (*fn_av_log_set_level)(int level);
typedef void (*fn_av_log_set_callback)(void (*callback)(void *, int, const char *, va_list));
typedef void (*fn_av_log_default_callback)(void *avcl, int level, const char *fmt, va_list vl);
typedef void (*fn_av_dict_free)(AVDictionary **m);
typedef AVDictionaryEntry *(*fn_av_dict_get)(const AVDictionary *m,
                               const char *key,
                               const AVDictionaryEntry *prev,
                               int flags);
typedef int (*fn_av_dict_set)(AVDictionary **pm, const char *key, const char *value, int flags);
typedef void (*fn_av_bprint_init)(AVBPrint *buf, unsigned size_init, unsigned size_max);
typedef void (*fn_av_bprint_init_for_buffer)(AVBPrint *buf, char *buffer, unsigned size);
typedef void (*fn_av_vbprintf)(AVBPrint *buf, const char *fmt, va_list vl_arg);
typedef int (*fn_av_bprint_finalize)(AVBPrint *buf, char **ret_str);
typedef AVFrame *(*fn_av_frame_alloc)(void);
typedef void (*fn_av_frame_free)(AVFrame **frame);
typedef AVFrame *(*fn_av_frame_clone)(const AVFrame *src);
typedef void (*fn_av_frame_unref)(AVFrame *frame);
typedef int (*fn_av_frame_get_buffer)(AVFrame *frame, int align);
typedef int (*fn_av_frame_copy)(AVFrame *dst, const AVFrame *src);
typedef int (*fn_av_frame_copy_props)(AVFrame *dst, const AVFrame *src);
typedef AVFrameSideData *(*fn_av_frame_new_side_data)(AVFrame *frame,
                                                      enum AVFrameSideDataType type,
                                                      int size);
typedef AVFrameSideData *(*fn_av_frame_new_side_data_from_buf)(AVFrame *frame,
                                                               enum AVFrameSideDataType type,
                                                               AVBufferRef *buf);
typedef AVFrameSideData *(*fn_av_frame_get_side_data)(const AVFrame *frame,
                                                      enum AVFrameSideDataType type);
typedef void (*fn_av_frame_remove_side_data)(AVFrame *frame, enum AVFrameSideDataType type);
typedef void *(*fn_av_malloc)(size_t size);
typedef void *(*fn_av_mallocz)(size_t size);
typedef void (*fn_av_free)(void *ptr);
typedef void (*fn_av_freep)(void *ptr);
typedef const AVPixFmtDescriptor *(*fn_av_pix_fmt_desc_get)(enum AVPixelFormat pix_fmt);
typedef const char *(*fn_av_get_pix_fmt_name)(enum AVPixelFormat pix_fmt);
typedef void (*fn_av_image_fill_max_pixsteps)(int max_pixsteps[4],
                                              int max_pixstep_comps[4],
                                              const AVPixFmtDescriptor *pixdesc);
typedef int (*fn_av_image_fill_linesizes)(int linesizes[4], enum AVPixelFormat pix_fmt, int width);
typedef int (*fn_av_image_fill_pointers)(uint8_t *data[4],
                                         enum AVPixelFormat pix_fmt,
                                         int height,
                                         uint8_t *ptr,
                                         const int linesizes[4]);
typedef void (*fn_av_image_copy_plane)(uint8_t *dst,
                                       int dst_linesize,
                                       const uint8_t *src,
                                       int src_linesize,
                                       int bytewidth,
                                       int height);
typedef void (*fn_av_image_copy)(uint8_t *dst_data[4],
                                 int dst_linesizes[4],
                                 const uint8_t *src_data[4],
                                 const int src_linesizes[4],
                                 enum AVPixelFormat pix_fmt,
                                 int width,
                                 int height);
typedef void (*fn_av_image_copy_uc_from)(uint8_t *dst_data[4],
                                         const ptrdiff_t dst_linesizes[4],
                                         const uint8_t *src_data[4],
                                         const ptrdiff_t src_linesizes[4],
                                         enum AVPixelFormat pix_fmt,
                                         int width,
                                         int height);
typedef int (*fn_av_image_copy_to_buffer)(uint8_t *dst,
                                          int dst_size,
                                          const uint8_t *const src_data[4],
                                          const int src_linesize[4],
                                          enum AVPixelFormat pix_fmt,
                                          int width,
                                          int height,
                                          int align);
typedef int (*fn_av_image_get_buffer_size)(enum AVPixelFormat pix_fmt,
                                           int width,
                                           int height,
                                           int align);
typedef int (*fn_av_get_cpu_flags)(void);
typedef int64_t (*fn_av_rescale_rnd)(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const;
typedef int (*fn_av_strerror)(int errnum, char *errbuf, size_t errbuf_size);
typedef int (*fn_av_stristart)(const char *str, const char *pfx, const char **ptr);
typedef void (*fn_av_log)(void *avcl, int level, const char *fmt, ...);
typedef AVBufferRef *(*fn_av_buffer_ref)(AVBufferRef *buf);
typedef void (*fn_av_buffer_unref)(AVBufferRef **buf);
typedef enum AVHWDeviceType (*fn_av_hwdevice_find_type_by_name)(const char *name);
typedef int (*fn_av_hwdevice_ctx_create)(AVBufferRef **device_ctx,
                           enum AVHWDeviceType type,
                           const char *device,
                           AVDictionary *opts,
                           int flags);
typedef AVBufferRef *(*fn_av_hwframe_ctx_alloc)(AVBufferRef *device_ctx);
typedef int (*fn_av_hwframe_transfer_data)(AVFrame *dst, const AVFrame *src, int flags);
typedef int (*fn_av_hwframe_map)(AVFrame *dst, const AVFrame *src, int flags);
typedef const char* (*fn_av_get_media_type_string)(enum AVMediaType media_type);
typedef int (*fn_av_frame_make_writable)(AVFrame* frame);
typedef const char* (*fn_av_hwdevice_get_type_name)(enum AVHWDeviceType type);
typedef int (*fn_av_hwframe_ctx_init)(AVBufferRef* ref);
typedef int (*fn_av_image_alloc)(uint8_t* pointers[4], int linesizes[4], int w, int h, enum AVPixelFormat pix_fmt, int align);
typedef void (*fn_av_opt_set_defaults)(void* s);
typedef void (*fn_av_opt_set_defaults2)(void* s, int mask, int flags);
typedef int (*fn_av_opt_set_from_string)(void* ctx, const char* opts, const char* const* shorthand, const char* key_val_sep, const char* pairs_sep);
typedef int (*fn_av_opt_set_dict)(void* obj, struct AVDictionary** options);
typedef int (*fn_av_opt_set_dict2)(void* obj, struct AVDictionary** options, int search_flags);
typedef int (*fn_av_opt_set)(void* obj, const char* name, const char* val, int search_flags);
typedef int (*fn_av_opt_set_int)(void* obj, const char* name, int64_t     val, int search_flags);
typedef int (*fn_av_opt_set_double)(void* obj, const char* name, double      val, int search_flags);
typedef int (*fn_av_opt_set_q)(void* obj, const char* name, AVRational  val, int search_flags);
typedef int (*fn_av_opt_set_bin)(void* obj, const char* name, const uint8_t* val, int size, int search_flags);
typedef int (*fn_av_opt_set_image_size)(void* obj, const char* name, int w, int h, int search_flags);
typedef int (*fn_av_opt_set_pixel_fmt)(void* obj, const char* name, enum AVPixelFormat fmt, int search_flags);
typedef int (*fn_av_opt_set_sample_fmt)(void* obj, const char* name, enum AVSampleFormat fmt, int search_flags);
typedef int (*fn_av_opt_set_video_rate)(void* obj, const char* name, AVRational val, int search_flags);
typedef int (*fn_av_opt_set_channel_layout)(void* obj, const char* name, int64_t ch_layout, int search_flags);
typedef int (*fn_av_opt_set_dict_val)(void* obj, const char* name, const AVDictionary* val, int search_flags);
typedef const char* (*fn_av_get_sample_fmt_name)(enum AVSampleFormat sample_fmt);
typedef int (*fn_av_get_bytes_per_sample)(enum AVSampleFormat sample_fmt);
typedef int (*fn_av_sample_fmt_is_planar)(enum AVSampleFormat sample_fmt);
typedef enum AVHWDeviceType (*fn_av_hwdevice_iterate_types)(enum AVHWDeviceType prev);
typedef enum AVSampleFormat (*fn_av_get_packed_sample_fmt)(enum AVSampleFormat sample_fmt);

    //swscale
typedef const int *(*fn_sws_getCoefficients)(int colorspace);
typedef int (*fn_sws_isSupportedInput)(enum AVPixelFormat pix_fmt);
typedef int (*fn_sws_isSupportedOutput)(enum AVPixelFormat pix_fmt);
typedef int (*fn_sws_isSupportedEndiannessConversion)(enum AVPixelFormat pix_fmt);
typedef struct SwsContext *(*fn_sws_alloc_context)(void);
typedef int (*fn_sws_init_context)(struct SwsContext *sws_context, SwsFilter *srcFilter, SwsFilter *dstFilter);
typedef void (*fn_sws_freeContext)(struct SwsContext *swsContext);
typedef struct SwsContext *(*fn_sws_getContext)(int srcW,
                                                int srcH,
                                                enum AVPixelFormat srcFormat,
                                                int dstW,
                                                int dstH,
                                                enum AVPixelFormat dstFormat,
                                                int flags,
                                                SwsFilter *srcFilter,
                                                SwsFilter *dstFilter,
                                                const double *param);
typedef int (*fn_sws_scale)(struct SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);
typedef int (*fn_sws_setColorspaceDetails)(struct SwsContext *c, const int inv_table[4],
                             int srcRange, const int table[4], int dstRange,
                             int brightness, int contrast, int saturation);
typedef int (*fn_sws_getColorspaceDetails)(struct SwsContext *c, int **inv_table,
                             int *srcRange, int **table, int *dstRange,
                             int *brightness, int *contrast, int *saturation);
typedef SwsVector *(*fn_sws_allocVec)(int length);
typedef SwsVector *(*fn_sws_getGaussianVec)(double variance, double quality);
typedef void (*fn_sws_scaleVec)(SwsVector *a, double scalar);
typedef void (*fn_sws_normalizeVec)(SwsVector *a, double height);
typedef void (*fn_sws_freeVec)(SwsVector *a);
typedef void (*fn_sws_freeFilter)(SwsFilter *filter);
typedef struct SwsContext *(*fn_sws_getCachedContext)(struct SwsContext *context,
                                        int srcW, int srcH, enum AVPixelFormat srcFormat,
                                        int dstW, int dstH, enum AVPixelFormat dstFormat,
                                        int flags, SwsFilter *srcFilter,
                                        SwsFilter *dstFilter, const double *param);
typedef void (*fn_sws_convertPalette8ToPacked32)(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);
typedef void (*fn_sws_convertPalette8ToPacked24)(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);


extern fn_av_bsf_get_by_name av_bsf_get_by_name;
extern fn_av_bsf_alloc av_bsf_alloc;
extern fn_av_bsf_init av_bsf_init;
extern fn_av_bsf_send_packet av_bsf_send_packet;
extern fn_av_bsf_receive_packet av_bsf_receive_packet;
extern fn_av_init_packet av_init_packet;
extern fn_av_packet_unref av_packet_unref;
extern fn_avcodec_alloc_context3 avcodec_alloc_context3;
extern fn_avcodec_free_context avcodec_free_context;
extern fn_avcodec_open2 avcodec_open2;
extern fn_avcodec_close avcodec_close;
extern fn_avcodec_send_packet avcodec_send_packet;
extern fn_avcodec_receive_frame avcodec_receive_frame;
extern fn_avcodec_flush_buffers avcodec_flush_buffers;
extern fn_avcodec_find_encoder avcodec_find_encoder;
extern fn_avcodec_find_encoder_by_name avcodec_find_encoder_by_name;
extern fn_avcodec_find_decoder avcodec_find_decoder;
extern fn_avcodec_find_decoder_by_name avcodec_find_decoder_by_name;
extern fn_avcodec_get_hw_config avcodec_get_hw_config;
extern fn_avcodec_parameters_to_context avcodec_parameters_to_context;
extern fn_avcodec_parameters_from_context avcodec_parameters_from_context;
extern fn_av_packet_alloc av_packet_alloc;
extern fn_av_packet_free av_packet_free;
extern fn_av_packet_rescale_ts av_packet_rescale_ts;
extern fn_avcodec_register_all avcodec_register_all;
extern fn_avcodec_decode_audio4 avcodec_decode_audio4;
extern fn_avcodec_decode_video2 avcodec_decode_video2;
extern fn_avcodec_encode_video2 avcodec_encode_video2; 


extern fn_avformat_configuration avformat_configuration;
extern fn_av_register_all av_register_all;
extern fn_avformat_network_init avformat_network_init;
extern fn_avformat_open_input avformat_open_input;
extern fn_avformat_find_stream_info avformat_find_stream_info;
extern fn_av_find_best_stream av_find_best_stream;
extern fn_av_read_frame av_read_frame;
extern fn_avformat_close_input avformat_close_input;
extern fn_avio_write avio_write;
extern fn_avio_wb32 avio_wb32;
extern fn_avio_write_marker avio_write_marker;
extern fn_avio_skip avio_skip;
extern fn_avio_r8 avio_r8;
extern fn_avio_rl32 avio_rl32;
extern fn_avio_rb24 avio_rb24;
extern fn_avio_closep avio_closep;
extern fn_avio_close avio_close;
extern fn_av_write_trailer av_write_trailer;
extern fn_avformat_free_context avformat_free_context;
extern fn_avformat_alloc_output_context2 avformat_alloc_output_context2;
extern fn_avformat_new_stream avformat_new_stream;
extern fn_av_dump_format av_dump_format;
extern fn_avio_open avio_open;
extern fn_avformat_write_header avformat_write_header;
extern fn_av_interleaved_write_frame av_interleaved_write_frame;
extern fn_avio_alloc_context avio_alloc_context;


extern fn_av_log_set_level av_log_set_level;
extern fn_av_log_set_callback av_log_set_callback;
extern fn_av_log_default_callback av_log_default_callback;
extern fn_av_dict_free av_dict_free;
extern fn_av_dict_get av_dict_get;
extern fn_av_dict_set av_dict_set;
extern fn_av_bprint_init av_bprint_init;
extern fn_av_bprint_init_for_buffer av_bprint_init_for_buffer;
extern fn_av_vbprintf av_vbprintf;
extern fn_av_bprint_finalize av_bprint_finalize;
extern fn_av_frame_alloc av_frame_alloc;
extern fn_av_frame_free av_frame_free;
extern fn_av_frame_clone av_frame_clone;
extern fn_av_frame_unref av_frame_unref;
extern fn_av_frame_get_buffer av_frame_get_buffer;
extern fn_av_frame_copy av_frame_copy;
extern fn_av_frame_copy_props av_frame_copy_props;
extern fn_av_frame_new_side_data av_frame_new_side_data;
extern fn_av_frame_new_side_data_from_buf av_frame_new_side_data_from_buf;
extern fn_av_frame_get_side_data av_frame_get_side_data;
extern fn_av_frame_remove_side_data av_frame_remove_side_data;
extern fn_av_malloc av_malloc;
extern fn_av_mallocz av_mallocz;
extern fn_av_free av_free;
extern fn_av_freep av_freep;
extern fn_av_pix_fmt_desc_get av_pix_fmt_desc_get;
extern fn_av_get_pix_fmt_name av_get_pix_fmt_name;
extern fn_av_image_fill_max_pixsteps av_image_fill_max_pixsteps;
extern fn_av_image_fill_linesizes av_image_fill_linesizes;
extern fn_av_image_fill_pointers av_image_fill_pointers;
extern fn_av_image_copy_plane av_image_copy_plane;
extern fn_av_image_copy av_image_copy;
extern fn_av_image_copy_uc_from av_image_copy_uc_from;
extern fn_av_image_copy_to_buffer av_image_copy_to_buffer;
extern fn_av_image_get_buffer_size av_image_get_buffer_size;
extern fn_av_get_cpu_flags av_get_cpu_flags;
extern fn_av_rescale_rnd av_rescale_rnd;
extern fn_av_strerror av_strerror;
extern fn_av_stristart av_stristart;
extern fn_av_log av_log;
extern fn_av_buffer_ref av_buffer_ref;
extern fn_av_buffer_unref av_buffer_unref;
extern fn_av_hwdevice_find_type_by_name av_hwdevice_find_type_by_name;
extern fn_av_hwdevice_ctx_create av_hwdevice_ctx_create;
extern fn_av_hwframe_ctx_alloc av_hwframe_ctx_alloc;
extern fn_av_hwframe_transfer_data av_hwframe_transfer_data;
extern fn_av_hwframe_map av_hwframe_map;
extern fn_av_get_media_type_string av_get_media_type_string;
extern fn_av_frame_make_writable av_frame_make_writable;
extern fn_av_hwdevice_get_type_name av_hwdevice_get_type_name;
extern fn_av_hwframe_ctx_init av_hwframe_ctx_init;
extern fn_av_image_alloc av_image_alloc;
extern fn_av_opt_set_defaults av_opt_set_defaults;
extern fn_av_opt_set_defaults2 av_opt_set_defaults2;
extern fn_av_opt_set_from_string av_opt_set_from_string;
extern fn_av_opt_set_dict av_opt_set_dict;
extern fn_av_opt_set_dict2 av_opt_set_dict2;
extern fn_av_opt_set          av_opt_set;
extern fn_av_opt_set_int      av_opt_set_int;
extern fn_av_opt_set_double   av_opt_set_double;
extern fn_av_opt_set_q        av_opt_set_q;
extern fn_av_opt_set_bin      av_opt_set_bin;
extern fn_av_opt_set_image_size av_opt_set_image_size;
extern fn_av_opt_set_pixel_fmt  av_opt_set_pixel_fmt;
extern fn_av_opt_set_sample_fmt av_opt_set_sample_fmt;
extern fn_av_opt_set_video_rate av_opt_set_video_rate;
extern fn_av_opt_set_channel_layout av_opt_set_channel_layout;
extern fn_av_opt_set_dict_val av_opt_set_dict_val;
extern fn_av_get_sample_fmt_name av_get_sample_fmt_name;
extern fn_av_get_bytes_per_sample av_get_bytes_per_sample;
extern fn_av_sample_fmt_is_planar av_sample_fmt_is_planar;
extern fn_av_hwdevice_iterate_types av_hwdevice_iterate_types;
extern fn_av_get_packed_sample_fmt av_get_packed_sample_fmt;


extern fn_sws_getCoefficients sws_getCoefficients;
extern fn_sws_isSupportedInput sws_isSupportedInput;
extern fn_sws_isSupportedOutput sws_isSupportedOutput;
extern fn_sws_isSupportedEndiannessConversion sws_isSupportedEndiannessConversion;
extern fn_sws_alloc_context sws_alloc_context;
extern fn_sws_init_context sws_init_context;
extern fn_sws_freeContext sws_freeContext;
extern fn_sws_getContext sws_getContext;
extern fn_sws_scale sws_scale;
extern fn_sws_setColorspaceDetails sws_setColorspaceDetails;
extern fn_sws_getColorspaceDetails sws_getColorspaceDetails;
extern fn_sws_allocVec sws_allocVec;
extern fn_sws_getGaussianVec sws_getGaussianVec;
extern fn_sws_scaleVec sws_scaleVec;
extern fn_sws_normalizeVec sws_normalizeVec;
extern fn_sws_freeVec sws_freeVec;
extern fn_sws_freeFilter sws_freeFilter;
extern fn_sws_getCachedContext sws_getCachedContext;
extern fn_sws_convertPalette8ToPacked32 sws_convertPalette8ToPacked32;
extern fn_sws_convertPalette8ToPacked24 sws_convertPalette8ToPacked24;


#if 0 
// avcodec 
const AVBitStreamFilter *av_bsf_get_by_name(const char *name);
int av_bsf_alloc(const AVBitStreamFilter *filter, AVBSFContext **ctx);
int av_bsf_init(AVBSFContext *ctx);
int av_bsf_send_packet(AVBSFContext *ctx, AVPacket *pkt);
int av_bsf_receive_packet(AVBSFContext *ctx, AVPacket *pkt);
void av_init_packet(AVPacket *pkt);
void av_packet_unref(AVPacket *pkt);
AVCodecContext *avcodec_alloc_context3(const AVCodec *codec);
void avcodec_free_context(AVCodecContext **avctx);
int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
int avcodec_close(AVCodecContext *avctx);
int avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt);
int avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame);
void avcodec_flush_buffers(AVCodecContext *avctx);
AVCodec *avcodec_find_decoder_by_name(const char *name);

int avcodec_parameters_from_context(AVCodecParameters *par,
                                    const AVCodecContext *codec);
AVPacket *av_packet_alloc(void);
void av_packet_free(AVPacket **pkt);
void av_packet_rescale_ts(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);

//avformat
int avio_closep(AVIOContext **s);
int av_write_trailer(AVFormatContext *s);
void avformat_free_context(AVFormatContext *s);
int avformat_alloc_output_context2(AVFormatContext **ctx, ff_const59 AVOutputFormat *oformat,
                                   const char *format_name, const char *filename);
AVStream *avformat_new_stream(AVFormatContext *s, const AVCodec *c);
void av_dump_format(AVFormatContext *ic,
                    int index,
                    const char *url,
                    int is_output);
int avio_open(AVIOContext **s, const char *url, int flags);
int avformat_write_header(AVFormatContext *s, AVDictionary **options);
int av_interleaved_write_frame(AVFormatContext *s, AVPacket *pkt);
AVIOContext *avio_alloc_context(
                  unsigned char *buffer,
                  int buffer_size,
                  int write_flag,
                  void *opaque,
                  int (*read_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int (*write_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int64_t (*seek)(void *opaque, int64_t offset, int whence));
void av_register_all(void);
int avformat_network_init(void);
int avformat_open_input(AVFormatContext **ps, const char *url, ff_const59 AVInputFormat *fmt, AVDictionary **options);
int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options);
int av_find_best_stream(AVFormatContext *ic,
int av_read_frame(AVFormatContext *s, AVPacket *pkt);
void avformat_close_input(AVFormatContext **s);
void avio_write(AVIOContext *s, const unsigned char *buf, int size);
void avio_wb32(AVIOContext *s, unsigned int val);
void avio_write_marker(AVIOContext *s, int64_t time, enum AVIODataMarkerType type);
int64_t avio_skip(AVIOContext *s, int64_t offset);
int          avio_r8  (AVIOContext *s);
unsigned int avio_rl32(AVIOContext *s);
unsigned int avio_rb24(AVIOContext *s);

//avutil
void av_log_set_callback(void (*callback)(void*, int, const char*, va_list));
void av_log_default_callback(void *avcl, int level, const char *fmt,
                             va_list vl);
void av_dict_free(AVDictionary **m);
AVDictionaryEntry * av_dict_get(const AVDictionary *m,
                               const char *key,
                               const AVDictionaryEntry *prev,
                               int flags);
int av_dict_set(AVDictionary **pm, const char *key, const char *value, int flags);
void av_bprint_init(AVBPrint *buf, unsigned size_init, unsigned size_max);
void av_bprint_init_for_buffer(AVBPrint *buf, char *buffer, unsigned size);
void av_vbprintf(AVBPrint *buf, const char *fmt, va_list vl_arg);
int av_bprint_finalize(AVBPrint *buf, char **ret_str);
AVFrame *av_frame_alloc(void);
void av_frame_free(AVFrame **frame);
AVFrame *av_frame_clone(const AVFrame *src);
void av_frame_unref(AVFrame *frame);
int av_frame_get_buffer(AVFrame *frame, int align);
int av_frame_copy(AVFrame *dst, const AVFrame *src);
int av_frame_copy_props(AVFrame *dst, const AVFrame *src);
AVFrameSideData *av_frame_new_side_data(AVFrame *frame,
AVFrameSideData *av_frame_new_side_data_from_buf(AVFrame *frame,
AVFrameSideData *av_frame_get_side_data(const AVFrame *frame,
void av_frame_remove_side_data(AVFrame *frame, enum AVFrameSideDataType type);
void av_free(void *ptr);
void av_freep(void *ptr);
const AVPixFmtDescriptor *av_pix_fmt_desc_get(enum AVPixelFormat pix_fmt);
const char *av_get_pix_fmt_name(enum AVPixelFormat pix_fmt);
void av_image_fill_max_pixsteps(int max_pixsteps[4], int max_pixstep_comps[4],
int av_image_fill_linesizes(int linesizes[4], enum AVPixelFormat pix_fmt, int width);
int av_image_fill_pointers(uint8_t *data[4], enum AVPixelFormat pix_fmt, int height,
void av_image_copy_plane(uint8_t       *dst, int dst_linesize,
void av_image_copy(uint8_t *dst_data[4], int dst_linesizes[4],
void av_image_copy_uc_from(uint8_t *dst_data[4],       const ptrdiff_t dst_linesizes[4],
int av_image_copy_to_buffer(uint8_t *dst, int dst_size,
int av_get_cpu_flags(void);
int64_t av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const;
int av_strerror(int errnum, char *errbuf, size_t errbuf_size);
int av_stristart(const char *str, const char *pfx, const char **ptr);

//swscale
const int *sws_getCoefficients(int colorspace);
int sws_isSupportedInput(enum AVPixelFormat pix_fmt);
int sws_isSupportedOutput(enum AVPixelFormat pix_fmt);
int sws_isSupportedEndiannessConversion(enum AVPixelFormat pix_fmt);
struct SwsContext *sws_alloc_context(void);
int sws_init_context(struct SwsContext *sws_context, SwsFilter *srcFilter, SwsFilter *dstFilter);
void sws_freeContext(struct SwsContext *swsContext);
struct SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
int sws_setColorspaceDetails(struct SwsContext *c, const int inv_table[4],
int sws_getColorspaceDetails(struct SwsContext *c, int **inv_table,
SwsVector *sws_allocVec(int length);
SwsVector *sws_getGaussianVec(double variance, double quality);
void sws_scaleVec(SwsVector *a, double scalar);
void sws_normalizeVec(SwsVector *a, double height);
void sws_freeVec(SwsVector *a);
void sws_freeFilter(SwsFilter *filter);
struct SwsContext *sws_getCachedContext(struct SwsContext *context,
void sws_convertPalette8ToPacked32(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);
void sws_convertPalette8ToPacked24(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);
#endif
typedef void (*ffwraper_log_cb)(int level, const char *msg);
bool InitModules(const TCHAR *dllpath, ffwraper_log_cb cb);
void UnInitModules();
void ExportLog();
void UnExportLog();

};

#endif // __FFMPEG_LOADER_H__
