#pragma once
#ifndef _UTILS_H_
#define _UTILS_H_
int ONVIF_GetVideoEncoderConfigurationOptions(const char* MediaXAddr, char* ConfigurationToken);
int ONVIF_GetVideoEncoderConfiguration(const char* MediaXAddr, char* ConfigurationToken);
int ONVIF_SetVideoEncoderConfiguration(const char* MediaXAddr, struct tagVideoEncoderConfiguration* venc);
void cb_discovery(char* DeviceXAddr);
void open_rtsp(char* rtsp);
int ONVIF_GetStreamUri(const char* MediaXAddr, char* ProfileToken, char* uri, unsigned int sizeuri);
void getURI(char* dest,const char* DeviceXAddr, int uriChar, const char* username, const char* password);
#endif // !_UTILS_H_
