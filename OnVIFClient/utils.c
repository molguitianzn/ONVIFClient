#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "onvif_comm.h"
#include "onvif_dump.h"
#include "utils.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/pixdesc.h"

/************************************************************************
**函数：ONVIF_GetVideoEncoderConfigurationOptions
**功能：获取指定视频编码器配置的参数选项集
**参数：
        [in] MediaXAddr   - 媒体服务地址
        [in] ConfigurationToken - 视频编码器配置的令牌字符串，如果为NULL，则获取所有视频编码器配置的选项集（会杂在一起，
                                  无法区分选项集是归属哪个视频编码配置器的）
**返回：
        0表明成功，非0表明失败
**备注：
    1). 有两种方式可以获取指定视频编码器配置的参数选项集：一种是根据ConfigurationToken，另一种
        是根据ProfileToken
************************************************************************/
int ONVIF_GetVideoEncoderConfigurationOptions(const char* MediaXAddr, char* ConfigurationToken)
{
    int result = 0;
    struct soap* soap = NULL;
    struct _trt__GetVideoEncoderConfigurationOptions          req;
    struct _trt__GetVideoEncoderConfigurationOptionsResponse  rep;

    SOAP_ASSERT(NULL != MediaXAddr);
    SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

    memset(&req, 0x00, sizeof(req));
    memset(&rep, 0x00, sizeof(rep));
    req.ConfigurationToken = ConfigurationToken;

    ONVIF_SetAuthInfo(soap, "admin", "Admin123321");
    result = soap_call___trt__GetVideoEncoderConfigurationOptions(soap, MediaXAddr, NULL, &req, &rep);
    SOAP_CHECK_ERROR(result, soap, "GetVideoEncoderConfigurationOptions");

    dump_trt__GetVideoEncoderConfigurationOptionsResponse(&rep);

EXIT:

    if (NULL != soap) {
        ONVIF_soap_delete(soap);
    }

    return result;
}

/************************************************************************
**函数：ONVIF_GetVideoEncoderConfiguration
**功能：获取设备上指定的视频编码器配置信息
**参数：
        [in] MediaXAddr - 媒体服务地址
        [in] ConfigurationToken - 视频编码器配置的令牌字符串
**返回：
        0表明成功，非0表明失败
**备注：
************************************************************************/
int ONVIF_GetVideoEncoderConfiguration(const char* MediaXAddr, char* ConfigurationToken)
{
    int result = 0;
    struct soap* soap = NULL;
    struct _trt__GetVideoEncoderConfiguration          req;
    struct _trt__GetVideoEncoderConfigurationResponse  rep;

    SOAP_ASSERT(NULL != MediaXAddr);
    SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

    ONVIF_SetAuthInfo(soap, "admin", "Admin123321");

    memset(&req, 0x00, sizeof(req));
    memset(&rep, 0x00, sizeof(rep));
    req.ConfigurationToken = ConfigurationToken;
    result = soap_call___trt__GetVideoEncoderConfiguration(soap, MediaXAddr, NULL, &req, &rep);
    SOAP_CHECK_ERROR(result, soap, "GetVideoEncoderConfiguration");

    dump_trt__GetVideoEncoderConfigurationResponse(&rep);

EXIT:

    if (NULL != soap) {
        ONVIF_soap_delete(soap);
    }

    return result;
}

/************************************************************************
**函数：ONVIF_SetVideoEncoderConfiguration
**功能：修改指定的视频编码器配置信息
**参数：
        [in] MediaXAddr - 媒体服务地址
        [in] venc - 视频编码器配置信息
**返回：
        0表明成功，非0表明失败
**备注：
    1). 所设置的分辨率必须是GetVideoEncoderConfigurationOptions返回的“分辨率选项集”中的一种，
    否则调用SetVideoEncoderConfiguration会失败。
************************************************************************/
int ONVIF_SetVideoEncoderConfiguration(const char* MediaXAddr, struct tagVideoEncoderConfiguration* venc)
{
    int result = 0;
    struct soap* soap = NULL;

    struct _trt__GetVideoEncoderConfiguration           gVECfg_req;
    struct _trt__GetVideoEncoderConfigurationResponse   gVECfg_rep;

    struct _trt__SetVideoEncoderConfiguration           sVECfg_req;
    struct _trt__SetVideoEncoderConfigurationResponse   sVECfg_rep;

    SOAP_ASSERT(NULL != MediaXAddr);
    SOAP_ASSERT(NULL != venc);
    SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

    memset(&gVECfg_req, 0x00, sizeof(gVECfg_req));
    memset(&gVECfg_rep, 0x00, sizeof(gVECfg_rep));
    gVECfg_req.ConfigurationToken = venc->token;

    ONVIF_SetAuthInfo(soap, "admin", "Admin123321");
    result = soap_call___trt__GetVideoEncoderConfiguration(soap, MediaXAddr, NULL, &gVECfg_req, &gVECfg_rep);
    SOAP_CHECK_ERROR(result, soap, "GetVideoEncoderConfiguration");

    if (NULL == gVECfg_rep.Configuration) {
        SOAP_DBGERR("video encoder configuration is NULL.\n");
        goto EXIT;
    }

    memset(&sVECfg_req, 0x00, sizeof(sVECfg_req));
    memset(&sVECfg_rep, 0x00, sizeof(sVECfg_rep));

    sVECfg_req.ForcePersistence = xsd__boolean__true_;
    sVECfg_req.Configuration = gVECfg_rep.Configuration;

    if (NULL != sVECfg_req.Configuration->Resolution) {
        sVECfg_req.Configuration->Resolution->Width = venc->Width;
        sVECfg_req.Configuration->Resolution->Height = venc->Height;
    }

    ONVIF_SetAuthInfo(soap, "admin", "Admin123321");
    result = soap_call___trt__SetVideoEncoderConfiguration(soap, MediaXAddr, NULL, &sVECfg_req, &sVECfg_rep);
    SOAP_CHECK_ERROR(result, soap, "SetVideoEncoderConfiguration");

EXIT:

    if (SOAP_OK == result) {
        SOAP_DBGLOG("\nSetVideoEncoderConfiguration success!!!\n");
    }

    if (NULL != soap) {
        ONVIF_soap_delete(soap);
    }

    return result;
}

void cb_discovery(char* DeviceXAddr)
{
    int stmno = 0;                                                              // 码流序号，0为主码流，1为辅码流
    int profile_cnt = 0;                                                        // 设备配置文件个数
    struct tagProfile* profiles = NULL;                                         // 设备配置文件列表
    struct tagCapabilities capa;                                                // 设备能力信息

    char uri[ONVIF_ADDRESS_SIZE] = { 0 };                                         // 不带认证信息的URI地址
    char uri_auth[ONVIF_ADDRESS_SIZE + 50] = { 0 };                               // 带有认证信息的URI地址

    ONVIF_GetCapabilities(DeviceXAddr, &capa, "admin", "Admin123321");                                  // 获取设备能力信息（获取媒体服务地址）

    profile_cnt = ONVIF_GetProfiles(DeviceXAddr, &profiles, "admin", "Admin123321");                    // 获取媒体配置信息（主/辅码流配置信息）

    if (profile_cnt > stmno) {
        ONVIF_GetStreamUri(capa.MediaXAddr, profiles[stmno].token, uri, sizeof(uri)); // 获取RTSP地址
        struct tagVideoEncoderConfiguration venc;
        char* vencToken = profiles[stmno].venc.token;

        ONVIF_GetVideoEncoderConfigurationOptions(capa.MediaXAddr, vencToken);  // 获取该码流支持的视频编码器参数选项集

        ONVIF_GetVideoEncoderConfiguration(capa.MediaXAddr, vencToken);         // 获取该码流当前的视频编码器参数

        venc = profiles[stmno].venc;
        venc.Height = 960;
        venc.Width = 1280;
        ONVIF_SetVideoEncoderConfiguration(capa.MediaXAddr, &venc);             // 设置该码流当前的视频编码器参数

        ONVIF_GetVideoEncoderConfiguration(capa.MediaXAddr, vencToken);         // 观察是否修改成功
    }

    if (NULL != profiles) {
        free(profiles);
        profiles = NULL;
    }
}

/************************************************************************
**函数：open_rtsp
**功能：从RTSP获取音视频流数据
**参数：
        [in]  rtsp - RTSP地址
**返回：无
************************************************************************/
void open_rtsp(char* rtsp)
{
    printf("here!");
    unsigned int    i;
    int             ret;
    int             video_st_index = -1;
    int             audio_st_index = -1;
    AVFormatContext* ifmt_ctx = NULL;
    AVPacket        pkt;
    AVStream* st = NULL;
    char            errbuf[64];
    AVFrame* frame; // ================================

    // av_register_all();                                                          // Register all codecs and formats so that they can be used.
    avformat_network_init();                                                    // Initialization of network components

    if ((ret = avformat_open_input(&ifmt_ctx, rtsp, 0, NULL)) < 0) {            // Open the input file for reading.
        printf("Could not open input file '%s' (error '%s')\n", rtsp, av_make_error_string(errbuf, sizeof(errbuf), ret));
        goto EXIT;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {                // Get information on the input file (number of streams etc.).
        printf("Could not open find stream info (error '%s')\n", av_make_error_string(errbuf, sizeof(errbuf), ret));
        goto EXIT;
    }

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {                                // dump information
        av_dump_format(ifmt_ctx, i, rtsp, 0);
    }

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {                                // find video stream index
        st = ifmt_ctx->streams[i];
        switch (st->codec->codec_type) {
        case AVMEDIA_TYPE_AUDIO: audio_st_index = i; break;
        case AVMEDIA_TYPE_VIDEO: video_st_index = i; break;
        default: break;
        }
    }
    if (-1 == video_st_index) {
        printf("No H.264 video stream in the input file\n");
        goto EXIT;
    }

    // av_init_packet(&pkt);                                                       // initialize packet.
    pkt.data = NULL;
    pkt.size = 0;
    // frame = av_frame_alloc(); // =================================
    while (1)
    {
        do {
            ret = av_read_frame(ifmt_ctx, &pkt);                                // read frames
        } while (ret == AVERROR(EAGAIN));

        if (ret < 0) {
            printf("Could not read frame (error '%s')\n", av_make_error_string(errbuf, sizeof(errbuf), ret));
            break;
        }

        if (pkt.stream_index == video_st_index) {                               // video frame
            printf("Video Packet size = %d\n", pkt.size);
        }
        else if (pkt.stream_index == audio_st_index) {                         // audio frame
            printf("Audio Packet size = %d\n", pkt.size);
        }
        else {
            printf("Unknow Packet size = %d\n", pkt.size);
        }

        // avcodec_decode_video2(ifmt_ctx, frame, ) // =============================
        av_packet_unref(&pkt);
    }

EXIT:

    if (NULL != ifmt_ctx) {
        // av_frame_free(&frame);  // ======================
        avformat_close_input(&ifmt_ctx);
        ifmt_ctx = NULL;
    }

    return;
}

/************************************************************************
**函数：ONVIF_GetStreamUri
**功能：获取设备码流地址(RTSP)
**参数：
        [in]  MediaXAddr    - 媒体服务地址
        [in]  ProfileToken  - the media profile token
        [out] uri           - 返回的地址
        [in]  sizeuri       - 地址缓存大小
**返回：
        0表明成功，非0表明失败
**备注：
************************************************************************/
int ONVIF_GetStreamUri(const char* MediaXAddr, char* ProfileToken, char* uri, unsigned int sizeuri)
{
    int result = 0;
    struct soap* soap = NULL;
    struct tt__StreamSetup              ttStreamSetup;
    struct tt__Transport                ttTransport;
    struct _trt__GetStreamUri           req;
    struct _trt__GetStreamUriResponse   rep;

    SOAP_ASSERT(NULL != MediaXAddr);
    SOAP_ASSERT(NULL != uri);
    memset(uri, 0x00, sizeuri);

    SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

    memset(&req, 0x00, sizeof(req));
    memset(&rep, 0x00, sizeof(rep));
    memset(&ttStreamSetup, 0x00, sizeof(ttStreamSetup));
    memset(&ttTransport, 0x00, sizeof(ttTransport));
    ttStreamSetup.Stream = tt__StreamType__RTP_Unicast;
    ttStreamSetup.Transport = &ttTransport;
    ttStreamSetup.Transport->Protocol = tt__TransportProtocol__RTSP;
    ttStreamSetup.Transport->Tunnel = NULL;
    req.StreamSetup = &ttStreamSetup;
    req.ProfileToken = ProfileToken;

    ONVIF_SetAuthInfo(soap, "admin", "admin123321");
    result = soap_call___trt__GetStreamUri(soap, MediaXAddr, NULL, &req, &rep);
    SOAP_CHECK_ERROR(result, soap, "GetServices");

    dump_trt__GetStreamUriResponse(&rep);

    result = -1;
    if (NULL != rep.MediaUri) {
        if (NULL != rep.MediaUri->Uri) {
            if (sizeuri > strlen(rep.MediaUri->Uri)) {
                strcpy(uri, rep.MediaUri->Uri);
                result = 0;
            }
            else {
                SOAP_DBGERR("Not enough cache!\n");
            }
        }
    }

EXIT:

    if (NULL != soap) {
        ONVIF_soap_delete(soap);
    }

    return result;
}

void getURI(char * dest, const char * DeviceXAddr, int uriChar, const char * username,const char * password)
{
    int stmno = 0;                                                              // 码流序号，0为主码流，1为辅码流
    int profile_cnt = 0;                                                        // 设备配置文件个数
    struct tagProfile* profiles = NULL;                                         // 设备配置文件列表
    struct tagCapabilities capa;                                                // 设备能力信息

    char uri[ONVIF_ADDRESS_SIZE] = { 0 };                                         // 不带认证信息的URI地址
    char uri_auth[ONVIF_ADDRESS_SIZE + 50] = { 0 };                               // 带有认证信息的URI地址

    ONVIF_GetCapabilities(DeviceXAddr, &capa, "admin", "Admin123321");                                  // 获取设备能力信息（获取媒体服务地址）

    profile_cnt = ONVIF_GetProfiles(DeviceXAddr, &profiles, "admin", "Admin123321");         // 获取媒体配置信息（主/辅码流配置信息）

    if (profile_cnt > stmno) {
        ONVIF_GetStreamUri(capa.MediaXAddr, profiles[stmno].token, uri, sizeof(uri)); // 获取RTSP地址

        // make_uri_withauth(uri, USERNAME, PASSWORD, uri_auth, sizeof(uri_auth)); // 生成带认证信息的URI（有的IPC要求认证）
        make_uri_withauth(uri, username, password, uri_auth, sizeof(uri_auth)); // 生成带认证信息的URI（有的IPC要求认证）
        resetInformation(dest, uri_auth, uriChar);
        // open_rtsp(uri_auth);                                                    // 读取主码流的音视频数据
    }

    if (NULL != profiles) {
        free(profiles);
        profiles = NULL;
    }
}