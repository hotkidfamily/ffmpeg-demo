#include "stdafx.h"
#include "FFMpeg_loader.h"
#include <string>
#include <windows.h>

#define MODULE_FUN_DECLARE(x) P##x x = NULL

extern "C" void _func_av_log_callback(void *ptr, int level, const char *fmt, va_list vl);

namespace FFMpeg
{
    fn_av_bsf_alloc av_bsf_alloc = nullptr;
    fn_av_bsf_get_by_name av_bsf_get_by_name = nullptr;
    fn_av_bsf_init av_bsf_init = nullptr;
    fn_av_bsf_receive_packet av_bsf_receive_packet = nullptr;
    fn_av_bsf_send_packet av_bsf_send_packet = nullptr;
    fn_av_init_packet av_init_packet = nullptr;
    fn_av_packet_alloc av_packet_alloc = nullptr;
    fn_av_packet_free av_packet_free = nullptr;
    fn_av_packet_rescale_ts av_packet_rescale_ts = nullptr;
    fn_av_packet_unref av_packet_unref = nullptr;
    fn_avcodec_alloc_context3 avcodec_alloc_context3 = nullptr;
    fn_avcodec_close avcodec_close = nullptr;
    fn_avcodec_decode_audio4 avcodec_decode_audio4 = nullptr;
    fn_avcodec_decode_video2 avcodec_decode_video2 = nullptr;
    fn_avcodec_encode_video2 avcodec_encode_video2 = nullptr;
    fn_avcodec_find_decoder avcodec_find_decoder = nullptr;
    fn_avcodec_find_decoder_by_name avcodec_find_decoder_by_name = nullptr;
    fn_avcodec_find_encoder avcodec_find_encoder = nullptr;
    fn_avcodec_find_encoder_by_name avcodec_find_encoder_by_name = nullptr;
    fn_avcodec_flush_buffers avcodec_flush_buffers = nullptr;
    fn_avcodec_free_context avcodec_free_context = nullptr;
    fn_avcodec_get_hw_config avcodec_get_hw_config = nullptr;
    fn_avcodec_open2 avcodec_open2 = nullptr;
    fn_avcodec_parameters_from_context avcodec_parameters_from_context = nullptr;
    fn_avcodec_parameters_to_context avcodec_parameters_to_context = nullptr;
    fn_avcodec_receive_frame avcodec_receive_frame = nullptr;
    fn_avcodec_register_all avcodec_register_all = nullptr;
    fn_avcodec_send_packet avcodec_send_packet = nullptr;


    fn_av_dump_format av_dump_format = nullptr;
    fn_av_find_best_stream av_find_best_stream = nullptr;
    fn_av_interleaved_write_frame av_interleaved_write_frame = nullptr;
    fn_av_read_frame av_read_frame = nullptr;
    fn_av_register_all av_register_all = nullptr;
    fn_av_write_trailer av_write_trailer = nullptr;
    fn_avformat_alloc_output_context2 avformat_alloc_output_context2 = nullptr;
    fn_avformat_close_input avformat_close_input = nullptr;
    fn_avformat_configuration avformat_configuration = nullptr;
    fn_avformat_find_stream_info avformat_find_stream_info = nullptr;
    fn_avformat_free_context avformat_free_context = nullptr;
    fn_avformat_network_init avformat_network_init = nullptr;
    fn_avformat_new_stream avformat_new_stream = nullptr;
    fn_avformat_open_input avformat_open_input = nullptr;
    fn_avformat_write_header avformat_write_header = nullptr;
    fn_avio_alloc_context avio_alloc_context = nullptr;
    fn_avio_close avio_close = nullptr;
    fn_avio_closep avio_closep = nullptr;
    fn_avio_open avio_open = nullptr;
    fn_avio_r8 avio_r8 = nullptr;
    fn_avio_rb24 avio_rb24 = nullptr;
    fn_avio_rl32 avio_rl32 = nullptr;
    fn_avio_skip avio_skip = nullptr;
    fn_avio_wb32 avio_wb32 = nullptr;
    fn_avio_write avio_write = nullptr;
    fn_avio_write_marker avio_write_marker = nullptr;


    fn_av_bprint_finalize av_bprint_finalize = nullptr;
    fn_av_bprint_init av_bprint_init = nullptr;
    fn_av_bprint_init_for_buffer av_bprint_init_for_buffer = nullptr;
    fn_av_buffer_ref av_buffer_ref = nullptr;
    fn_av_buffer_unref av_buffer_unref = nullptr;
    fn_av_dict_free av_dict_free = nullptr;
    fn_av_dict_get av_dict_get = nullptr;
    fn_av_dict_set av_dict_set = nullptr;
    fn_av_frame_alloc av_frame_alloc = nullptr;
    fn_av_frame_clone av_frame_clone = nullptr;
    fn_av_frame_copy av_frame_copy = nullptr;
    fn_av_frame_copy_props av_frame_copy_props = nullptr;
    fn_av_frame_free av_frame_free = nullptr;
    fn_av_frame_get_buffer av_frame_get_buffer = nullptr;
    fn_av_frame_get_side_data av_frame_get_side_data = nullptr;
    fn_av_frame_make_writable av_frame_make_writable = nullptr;
    fn_av_frame_new_side_data av_frame_new_side_data = nullptr;
    fn_av_frame_new_side_data_from_buf av_frame_new_side_data_from_buf = nullptr;
    fn_av_frame_remove_side_data av_frame_remove_side_data = nullptr;
    fn_av_frame_unref av_frame_unref = nullptr;
    fn_av_free av_free = nullptr;
    fn_av_freep av_freep = nullptr;
    fn_av_get_bytes_per_sample av_get_bytes_per_sample = nullptr;
    fn_av_get_cpu_flags av_get_cpu_flags = nullptr;
    fn_av_get_media_type_string av_get_media_type_string = nullptr;
    fn_av_get_packed_sample_fmt av_get_packed_sample_fmt = nullptr;
    fn_av_get_pix_fmt_name av_get_pix_fmt_name = nullptr;
    fn_av_get_sample_fmt_name av_get_sample_fmt_name = nullptr;
    fn_av_hwdevice_ctx_create av_hwdevice_ctx_create = nullptr;
    fn_av_hwdevice_find_type_by_name av_hwdevice_find_type_by_name = nullptr;
    fn_av_hwdevice_get_type_name av_hwdevice_get_type_name = nullptr;
    fn_av_hwdevice_iterate_types av_hwdevice_iterate_types = nullptr;
    fn_av_hwframe_ctx_alloc av_hwframe_ctx_alloc = nullptr;
    fn_av_hwframe_ctx_init av_hwframe_ctx_init = nullptr;
    fn_av_hwframe_map av_hwframe_map = nullptr;
    fn_av_hwframe_transfer_data av_hwframe_transfer_data = nullptr;
    fn_av_image_alloc av_image_alloc = nullptr;
    fn_av_image_copy av_image_copy = nullptr;
    fn_av_image_copy_plane av_image_copy_plane = nullptr;
    fn_av_image_copy_to_buffer av_image_copy_to_buffer = nullptr;
    fn_av_image_copy_uc_from av_image_copy_uc_from = nullptr;
    fn_av_image_fill_linesizes av_image_fill_linesizes = nullptr;
    fn_av_image_fill_max_pixsteps av_image_fill_max_pixsteps = nullptr;
    fn_av_image_fill_pointers av_image_fill_pointers = nullptr;
    fn_av_image_get_buffer_size av_image_get_buffer_size;
    fn_av_log av_log = nullptr;
    fn_av_log_default_callback av_log_default_callback = nullptr;
    fn_av_log_set_callback av_log_set_callback = nullptr;
    fn_av_log_set_level av_log_set_level = nullptr;
    fn_av_malloc av_malloc = nullptr;
    fn_av_mallocz av_mallocz = nullptr;
    fn_av_opt_set          av_opt_set = nullptr;
    fn_av_opt_set_bin      av_opt_set_bin = nullptr;
    fn_av_opt_set_channel_layout av_opt_set_channel_layout = nullptr;
    fn_av_opt_set_defaults av_opt_set_defaults = nullptr;
    fn_av_opt_set_defaults2 av_opt_set_defaults2 = nullptr;
    fn_av_opt_set_dict av_opt_set_dict = nullptr;
    fn_av_opt_set_dict2 av_opt_set_dict2 = nullptr;
    fn_av_opt_set_dict_val av_opt_set_dict_val = nullptr;
    fn_av_opt_set_double   av_opt_set_double = nullptr;
    fn_av_opt_set_from_string av_opt_set_from_string = nullptr;
    fn_av_opt_set_image_size av_opt_set_image_size = nullptr;
    fn_av_opt_set_int      av_opt_set_int = nullptr;
    fn_av_opt_set_pixel_fmt  av_opt_set_pixel_fmt = nullptr;
    fn_av_opt_set_q        av_opt_set_q = nullptr;
    fn_av_opt_set_sample_fmt av_opt_set_sample_fmt = nullptr;
    fn_av_opt_set_video_rate av_opt_set_video_rate = nullptr;
    fn_av_pix_fmt_desc_get av_pix_fmt_desc_get = nullptr;
    fn_av_rescale_rnd av_rescale_rnd = nullptr;
    fn_av_sample_fmt_is_planar av_sample_fmt_is_planar = nullptr;
    fn_av_strerror av_strerror = nullptr;
    fn_av_stristart av_stristart = nullptr;
    fn_av_vbprintf av_vbprintf = nullptr;


    fn_sws_allocVec sws_allocVec = nullptr;
    fn_sws_alloc_context sws_alloc_context = nullptr;
    fn_sws_convertPalette8ToPacked24 sws_convertPalette8ToPacked24 = nullptr;
    fn_sws_convertPalette8ToPacked32 sws_convertPalette8ToPacked32 = nullptr;
    fn_sws_freeContext sws_freeContext = nullptr;
    fn_sws_freeFilter sws_freeFilter = nullptr;
    fn_sws_freeVec sws_freeVec = nullptr;
    fn_sws_getCachedContext sws_getCachedContext = nullptr;
    fn_sws_getCoefficients sws_getCoefficients = nullptr;
    fn_sws_getColorspaceDetails sws_getColorspaceDetails = nullptr;
    fn_sws_getContext sws_getContext = nullptr;
    fn_sws_getGaussianVec sws_getGaussianVec = nullptr;
    fn_sws_init_context sws_init_context = nullptr;
    fn_sws_isSupportedEndiannessConversion sws_isSupportedEndiannessConversion = nullptr;
    fn_sws_isSupportedInput sws_isSupportedInput = nullptr;
    fn_sws_isSupportedOutput sws_isSupportedOutput = nullptr;
    fn_sws_normalizeVec sws_normalizeVec = nullptr;
    fn_sws_scale sws_scale = nullptr;
    fn_sws_scaleVec sws_scaleVec = nullptr;
    fn_sws_setColorspaceDetails sws_setColorspaceDetails = nullptr;


volatile LONG vsdk_test_reference_count = 0;

HMODULE hModules[]={NULL,NULL,NULL,NULL,NULL};
#if _WIN64
const TCHAR *szModules[] = { _T("avutil-sc.x64-56.dll"), _T("avcodec-sc.x64-58.dll"),
                             _T("avformat-sc.x64-58.dll"), _T("swscale-sc.x64-5.dll"),
                             _T("avfilter-sc.x64-7.dll") };
#elif _WIN32
const TCHAR *szModules[]
    = { _T("avutil-sc.x86-56.dll"), _T("avcodec-sc.x86-58.dll"), _T("avformat-sc.x86-58.dll"),
        _T("swscale-sc.x86-5.dll")};
// { _T("avutil-sc.x86-56.dll"), _T("avcodec-sc.x86-58.dll"), _T("avformat-sc.x86-58.dll"),
//     _T("swscale-sc.x86-5.dll"), _T("avfilter-sc.x86-7.dll") };
#endif


#define LOG_INFO(...) 

#define GET_MODULE_FUNS(mod, func)                                            \
    FFMpeg::##func = reinterpret_cast<fn_##func>(GetProcAddress(hModules[mod], #func)); \
    if (!FFMpeg::##func) {                                                          \
        LOG_INFO("%s, [%ws] can not entry point of %s!\n", __FUNCTION__, hModules[mod], #func); \
        break;    \
    }

#define GET_AVUTIL_FUNS(x)     GET_MODULE_FUNS(0, x)
#define GET_AVCODEC_FUNS(x)    GET_MODULE_FUNS(1, x)
#define GET_AVFORMAT_FUNS(x)   GET_MODULE_FUNS(2, x)
#define GET_SWSCALE_FUNS(x)    GET_MODULE_FUNS(3, x)
#define GET_AVFILTER_FUNS(x)   GET_MODULE_FUNS(4, x)
#define GET_SWRESAMPLE_FUNS(x) GET_MODULE_FUNS(5, x)

#define kFfmpegLogPrefix "[test-ffmpeg] "
static ffwraper_log_cb _logcb = nullptr;

static void _loginfo(int level, const char *fmt, ...)
{
    char info[256] = {0};
    va_list va;
    va_start(va, fmt);
    sprintf_s(info, 256, fmt, va);
    va_end(va);

    if (_logcb) {
        _logcb(level, info);
    }
}


static void ffwraper_av_log_callback(void *ptr, int level, const char *fmt, va_list vl)
{
    char *msg = nullptr;

    AVBPrint msgBuf;
    FFMpeg::av_bprint_init(&msgBuf, 0, 1);
    FFMpeg::av_vbprintf(&msgBuf, fmt, vl);
    FFMpeg::av_bprint_finalize(&msgBuf, &msg);

    if (level <= AV_LOG_WARNING)
    {
        _logcb(1, msg);
    }

    FFMpeg::av_freep(&msg);
}

void ExportLog()
{
    if (FFMpeg::av_log_set_callback)
    {
        _loginfo(1, kFfmpegLogPrefix"ExportLog callback %p", ffwraper_av_log_callback);
        FFMpeg::av_log_set_callback(ffwraper_av_log_callback);
    }
}

void UnExportLog()
{
    if (FFMpeg::av_log_set_callback)
    {
        _loginfo(1, kFfmpegLogPrefix"UnExportLog callback %p", FFMpeg::av_log_default_callback);
        FFMpeg::av_log_set_callback(FFMpeg::av_log_default_callback);
    }
}


static bool UnicodeToAnsi(const std::wstring& wstrcode, std::string& asciiString)
{
    if (wstrcode.empty()) {
        asciiString.clear();
        return true;
    }
    int asciisize = ::WideCharToMultiByte(CP_ACP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
    if (asciisize == 0) {
        return false;
    }

    char* tmpStr = new char[asciisize + 1];
    memset(tmpStr, 0, sizeof(char) * (asciisize + 1));
    int convresult = ::WideCharToMultiByte(CP_ACP, 0, wstrcode.c_str(), -1, tmpStr, asciisize, NULL, NULL);
    if (convresult != asciisize) {
        delete[] tmpStr;
        return false;
    }

    asciiString = tmpStr;
    delete[] tmpStr;
    return true;
}


bool InitModules(const TCHAR *dllpath, ffwraper_log_cb cb)
{
	bool ret = false;

    LONG ref = InterlockedIncrement(&vsdk_test_reference_count);

    if (ref > 1) {
        return true;
    }
	
    _logcb = cb;

    std::wstring appRootPath = dllpath;
    std::wstring filePath;

	ZeroMemory(hModules, sizeof(hModules[0]) * ARRAYSIZE(hModules));

	for (int i = 0; i<ARRAYSIZE(szModules); i++) {
        filePath = appRootPath + szModules[i];

        hModules[i] = LoadLibraryEx(filePath.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

        if (!hModules[i]) {
            std::string mo;
            UnicodeToAnsi(szModules[i], mo);
            _loginfo(1, kFfmpegLogPrefix "load %s failed => %d", mo.c_str(), GetLastError());
            return false;
        }
	}
    
    do {
        GET_AVCODEC_FUNS(av_bsf_alloc);
        GET_AVCODEC_FUNS(av_bsf_get_by_name);
        GET_AVCODEC_FUNS(av_bsf_init);
        GET_AVCODEC_FUNS(av_bsf_receive_packet);
        GET_AVCODEC_FUNS(av_bsf_send_packet);
        GET_AVCODEC_FUNS(av_init_packet);
        GET_AVCODEC_FUNS(av_packet_alloc);
        GET_AVCODEC_FUNS(av_packet_free);
        GET_AVCODEC_FUNS(av_packet_rescale_ts);
        GET_AVCODEC_FUNS(av_packet_unref);
        GET_AVCODEC_FUNS(avcodec_alloc_context3);
        GET_AVCODEC_FUNS(avcodec_close);
        GET_AVCODEC_FUNS(avcodec_decode_audio4);
        GET_AVCODEC_FUNS(avcodec_decode_video2);
        GET_AVCODEC_FUNS(avcodec_encode_video2);
        GET_AVCODEC_FUNS(avcodec_find_decoder);
        GET_AVCODEC_FUNS(avcodec_find_decoder_by_name);
        GET_AVCODEC_FUNS(avcodec_find_encoder);
        GET_AVCODEC_FUNS(avcodec_find_encoder_by_name);
        GET_AVCODEC_FUNS(avcodec_flush_buffers);
        GET_AVCODEC_FUNS(avcodec_free_context);
        GET_AVCODEC_FUNS(avcodec_get_hw_config);
        GET_AVCODEC_FUNS(avcodec_open2);
        GET_AVCODEC_FUNS(avcodec_parameters_from_context);
        GET_AVCODEC_FUNS(avcodec_parameters_to_context);
        GET_AVCODEC_FUNS(avcodec_receive_frame);
        GET_AVCODEC_FUNS(avcodec_register_all);
        GET_AVCODEC_FUNS(avcodec_send_packet);


        GET_AVFORMAT_FUNS(av_dump_format);
        GET_AVFORMAT_FUNS(av_find_best_stream);
        GET_AVFORMAT_FUNS(av_interleaved_write_frame);
        GET_AVFORMAT_FUNS(av_read_frame);
        GET_AVFORMAT_FUNS(av_register_all);
        GET_AVFORMAT_FUNS(av_write_trailer);
        GET_AVFORMAT_FUNS(avformat_alloc_output_context2);
        GET_AVFORMAT_FUNS(avformat_close_input);
        GET_AVFORMAT_FUNS(avformat_configuration);
        GET_AVFORMAT_FUNS(avformat_find_stream_info);
        GET_AVFORMAT_FUNS(avformat_free_context);
        GET_AVFORMAT_FUNS(avformat_network_init);
        GET_AVFORMAT_FUNS(avformat_new_stream);
        GET_AVFORMAT_FUNS(avformat_open_input);
        GET_AVFORMAT_FUNS(avformat_write_header);
        GET_AVFORMAT_FUNS(avio_alloc_context);
        GET_AVFORMAT_FUNS(avio_close);
        GET_AVFORMAT_FUNS(avio_closep);
        GET_AVFORMAT_FUNS(avio_open);
        GET_AVFORMAT_FUNS(avio_r8);
        GET_AVFORMAT_FUNS(avio_rb24);
        GET_AVFORMAT_FUNS(avio_rl32);
        GET_AVFORMAT_FUNS(avio_skip);
        GET_AVFORMAT_FUNS(avio_wb32);
        GET_AVFORMAT_FUNS(avio_write);
        GET_AVFORMAT_FUNS(avio_write_marker);


        GET_AVUTIL_FUNS(av_bprint_finalize);
        GET_AVUTIL_FUNS(av_bprint_init);
        GET_AVUTIL_FUNS(av_bprint_init_for_buffer);
        GET_AVUTIL_FUNS(av_buffer_ref);
        GET_AVUTIL_FUNS(av_buffer_unref);
        GET_AVUTIL_FUNS(av_dict_free);
        GET_AVUTIL_FUNS(av_dict_get);
        GET_AVUTIL_FUNS(av_dict_set);
        GET_AVUTIL_FUNS(av_frame_alloc);
        GET_AVUTIL_FUNS(av_frame_clone);
        GET_AVUTIL_FUNS(av_frame_copy);
        GET_AVUTIL_FUNS(av_frame_copy_props);
        GET_AVUTIL_FUNS(av_frame_free);
        GET_AVUTIL_FUNS(av_frame_get_buffer);
        GET_AVUTIL_FUNS(av_frame_get_side_data);
        GET_AVUTIL_FUNS(av_frame_make_writable);
        GET_AVUTIL_FUNS(av_frame_new_side_data);
        GET_AVUTIL_FUNS(av_frame_new_side_data_from_buf);
        GET_AVUTIL_FUNS(av_frame_remove_side_data);
        GET_AVUTIL_FUNS(av_frame_unref);
        GET_AVUTIL_FUNS(av_free);
        GET_AVUTIL_FUNS(av_freep);
        GET_AVUTIL_FUNS(av_get_bytes_per_sample);
        GET_AVUTIL_FUNS(av_get_cpu_flags);
        GET_AVUTIL_FUNS(av_get_media_type_string);
        GET_AVUTIL_FUNS(av_get_packed_sample_fmt);
        GET_AVUTIL_FUNS(av_get_pix_fmt_name);
        GET_AVUTIL_FUNS(av_get_sample_fmt_name);
        GET_AVUTIL_FUNS(av_hwdevice_ctx_create);
        GET_AVUTIL_FUNS(av_hwdevice_find_type_by_name);
        GET_AVUTIL_FUNS(av_hwdevice_get_type_name);
        GET_AVUTIL_FUNS(av_hwdevice_iterate_types);
        GET_AVUTIL_FUNS(av_hwframe_ctx_alloc);
        GET_AVUTIL_FUNS(av_hwframe_ctx_init);
        GET_AVUTIL_FUNS(av_hwframe_map);
        GET_AVUTIL_FUNS(av_hwframe_transfer_data);
        GET_AVUTIL_FUNS(av_image_alloc);
        GET_AVUTIL_FUNS(av_image_copy);
        GET_AVUTIL_FUNS(av_image_copy_plane);
        GET_AVUTIL_FUNS(av_image_copy_to_buffer);
        GET_AVUTIL_FUNS(av_image_copy_uc_from);
        GET_AVUTIL_FUNS(av_image_fill_linesizes);
        GET_AVUTIL_FUNS(av_image_fill_max_pixsteps);
        GET_AVUTIL_FUNS(av_image_fill_pointers);
        GET_AVUTIL_FUNS(av_image_get_buffer_size);
        GET_AVUTIL_FUNS(av_log);
        GET_AVUTIL_FUNS(av_log_default_callback);
        GET_AVUTIL_FUNS(av_log_set_callback);
        GET_AVUTIL_FUNS(av_log_set_level);
        GET_AVUTIL_FUNS(av_malloc);
        GET_AVUTIL_FUNS(av_mallocz);
        GET_AVUTIL_FUNS(av_opt_set);
        GET_AVUTIL_FUNS(av_opt_set_bin);
        GET_AVUTIL_FUNS(av_opt_set_channel_layout);
        GET_AVUTIL_FUNS(av_opt_set_defaults);
        GET_AVUTIL_FUNS(av_opt_set_defaults2);
        GET_AVUTIL_FUNS(av_opt_set_dict);
        GET_AVUTIL_FUNS(av_opt_set_dict2);
        GET_AVUTIL_FUNS(av_opt_set_dict_val);
        GET_AVUTIL_FUNS(av_opt_set_double);
        GET_AVUTIL_FUNS(av_opt_set_from_string);
        GET_AVUTIL_FUNS(av_opt_set_image_size);
        GET_AVUTIL_FUNS(av_opt_set_int);
        GET_AVUTIL_FUNS(av_opt_set_pixel_fmt);
        GET_AVUTIL_FUNS(av_opt_set_q);
        GET_AVUTIL_FUNS(av_opt_set_sample_fmt);
        GET_AVUTIL_FUNS(av_opt_set_video_rate);
        GET_AVUTIL_FUNS(av_pix_fmt_desc_get);
        GET_AVUTIL_FUNS(av_rescale_rnd);
        GET_AVUTIL_FUNS(av_sample_fmt_is_planar);
        GET_AVUTIL_FUNS(av_strerror);
        GET_AVUTIL_FUNS(av_stristart);
        GET_AVUTIL_FUNS(av_vbprintf);


        GET_SWSCALE_FUNS(sws_allocVec);
        GET_SWSCALE_FUNS(sws_alloc_context);
        GET_SWSCALE_FUNS(sws_convertPalette8ToPacked24);
        GET_SWSCALE_FUNS(sws_convertPalette8ToPacked32);
        GET_SWSCALE_FUNS(sws_freeContext);
        GET_SWSCALE_FUNS(sws_freeFilter);
        GET_SWSCALE_FUNS(sws_freeVec);
        GET_SWSCALE_FUNS(sws_getCachedContext);
        GET_SWSCALE_FUNS(sws_getCoefficients);
        GET_SWSCALE_FUNS(sws_getColorspaceDetails);
        GET_SWSCALE_FUNS(sws_getContext);
        GET_SWSCALE_FUNS(sws_getGaussianVec);
        GET_SWSCALE_FUNS(sws_init_context);
        GET_SWSCALE_FUNS(sws_isSupportedEndiannessConversion);
        GET_SWSCALE_FUNS(sws_isSupportedInput);
        GET_SWSCALE_FUNS(sws_isSupportedOutput);
        GET_SWSCALE_FUNS(sws_normalizeVec);
        GET_SWSCALE_FUNS(sws_scale);
        GET_SWSCALE_FUNS(sws_scaleVec);
        GET_SWSCALE_FUNS(sws_setColorspaceDetails);

        ret = true;

    } while (0);
    
    ExportLog();

	return ret;
}

void UnInitModules()
{
    LONG ref = InterlockedDecrement(&vsdk_test_reference_count);
    if (ref > 0)
    {
        return;
    }

    UnExportLog();

    for (auto &m : hModules)
    {
        _loginfo(1, kFfmpegLogPrefix "unload module %x", m);
        if (m != nullptr)
            FreeLibrary(m);

        m = nullptr;
    }


    av_bsf_alloc = nullptr;
    av_bsf_get_by_name = nullptr;
    av_bsf_init = nullptr;
    av_bsf_receive_packet = nullptr;
    av_bsf_send_packet = nullptr;
    av_init_packet = nullptr;
    av_packet_unref = nullptr;
    avcodec_alloc_context3 = nullptr;
    avcodec_free_context = nullptr;
    avcodec_close = nullptr;
    avcodec_find_encoder = nullptr;
    avcodec_find_encoder_by_name = nullptr;
    avcodec_find_decoder = nullptr;
    avcodec_find_decoder_by_name = nullptr;
    avcodec_flush_buffers = nullptr;
    avcodec_open2 = nullptr;
    avcodec_receive_frame = nullptr;
    avcodec_send_packet = nullptr;
    avcodec_get_hw_config = nullptr;
    avcodec_parameters_to_context = nullptr;
    avcodec_register_all = nullptr;
    avcodec_decode_audio4 = nullptr;
    avcodec_decode_video2 = nullptr;
    avcodec_encode_video2 = nullptr;

    avformat_configuration = nullptr;
    av_register_all = nullptr;
    avformat_network_init = nullptr;
    avformat_open_input = nullptr;
    avformat_find_stream_info = nullptr;
    av_find_best_stream = nullptr;
    av_read_frame = nullptr;
    avformat_close_input = nullptr;
    avio_write = nullptr;
    avio_wb32 = nullptr;
    avio_write_marker = nullptr;
    avio_skip = nullptr;
    avio_r8 = nullptr;
    avio_rl32 = nullptr;
    avio_rb24 = nullptr;
    avio_closep = nullptr;
    avio_close = nullptr;
    av_write_trailer = nullptr;
    avformat_free_context = nullptr;
    avformat_alloc_output_context2 = nullptr;
    avformat_new_stream = nullptr;
    av_dump_format = nullptr;
    avio_open = nullptr;
    avformat_write_header = nullptr;
    av_interleaved_write_frame = nullptr;
    avio_alloc_context = nullptr;


    av_log_set_level = nullptr;
    av_log_set_callback = nullptr;
    av_log_default_callback = nullptr;
    av_bprint_finalize = nullptr;
    av_bprint_init = nullptr;
    av_dict_free = nullptr;
    av_dict_get = nullptr;
    av_dict_set = nullptr;
    av_frame_alloc = nullptr;
    av_frame_clone = nullptr;
    av_frame_copy = nullptr;
    av_frame_copy_props = nullptr;
    av_frame_free = nullptr;
    av_frame_get_buffer = nullptr;
    av_frame_get_side_data = nullptr;
    av_frame_new_side_data = nullptr;
    av_frame_remove_side_data = nullptr;
    av_frame_unref = nullptr;
    av_malloc = nullptr;
    av_mallocz = nullptr;
    av_free = nullptr;
    av_freep = nullptr;
    av_get_cpu_flags = nullptr;
    av_get_pix_fmt_name = nullptr;
    av_image_copy = nullptr;
    av_image_fill_linesizes = nullptr;
    av_image_fill_max_pixsteps = nullptr;
    av_image_fill_pointers = nullptr;
    av_image_get_buffer_size = nullptr;
    av_log = nullptr;
    av_pix_fmt_desc_get = nullptr;
    av_rescale_rnd = nullptr;
    av_strerror = nullptr;
    av_stristart = nullptr;
    av_vbprintf = nullptr;
    av_buffer_ref = nullptr;
    av_buffer_unref = nullptr;
    av_hwdevice_find_type_by_name = nullptr;
    av_hwdevice_ctx_create = nullptr;
    av_hwframe_ctx_alloc = nullptr;
    av_hwframe_transfer_data = nullptr;
    av_hwframe_map = nullptr;
    av_get_media_type_string = nullptr;
    av_frame_make_writable = nullptr;
    av_hwdevice_get_type_name = nullptr;
    av_hwframe_ctx_init = nullptr;
    av_image_alloc = nullptr;
    av_opt_set_defaults = nullptr;
    av_opt_set_defaults2 = nullptr;
    av_opt_set_from_string = nullptr;
    av_opt_set_dict = nullptr;
    av_opt_set_dict2 = nullptr;
    av_opt_set = nullptr;
    av_opt_set_int = nullptr;
    av_opt_set_double = nullptr;
    av_opt_set_q = nullptr;
    av_opt_set_bin = nullptr;
    av_opt_set_image_size = nullptr;
    av_opt_set_pixel_fmt = nullptr;
    av_opt_set_sample_fmt = nullptr;
    av_opt_set_video_rate = nullptr;
    av_opt_set_channel_layout = nullptr;
    av_opt_set_dict_val = nullptr;
    av_get_sample_fmt_name = nullptr;
    av_get_bytes_per_sample = nullptr;
    av_sample_fmt_is_planar = nullptr;
    av_hwdevice_iterate_types = nullptr;
    av_get_packed_sample_fmt = nullptr;


    sws_freeContext = nullptr;
    sws_getCoefficients = nullptr;
    sws_getContext = nullptr;
    sws_scale = nullptr;
    sws_setColorspaceDetails = nullptr;
}

};
