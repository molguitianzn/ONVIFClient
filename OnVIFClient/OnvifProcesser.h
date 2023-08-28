#pragma once
# ifndef _ONVIF_PROCESSOR_
# define _ONVIF_PROCESSOR_
int ONVIF_GetSnapshotUri(const char* MediaXAddr, char* ProfileToken, char* uri, unsigned int sizeuri);
void cb_discovery(char* DeviceXAddr);
# endif