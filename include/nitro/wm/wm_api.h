#ifndef NITRO_WM_WM_API_H
#define NITRO_WM_WM_API_H

/*
 * The Nitro SDK's nitro/wm/ARM9/wm_api.h, the calls whose bodies are folded
 * so far: wm_standard.c (src/engine/wm/WM_Standard.c), wm_mp.c
 * (src/engine/wm/WM_Mp.c), WM_Init and the MP buffer sizes.
 *
 * The asynchronous calls return WM_ERRCODE_OPERATING when the request was
 * queued; the callback then answers with the result. The SDK's BOOL
 * parameters are ints here. Parameters are unnamed: a named parameter in a
 * shared header costs every including C++ file an @NNN number.
 */

#include "types.h"
#include "nitro/wm/wm.h"

#ifdef __cplusplus
extern "C" {
#endif

int WM_Init(void *, int);

WMErrCode WM_Enable(WMCallbackFunc);
WMErrCode WM_Disable(WMCallbackFunc);
WMErrCode WM_PowerOn(WMCallbackFunc);
WMErrCode WM_PowerOff(WMCallbackFunc);
WMErrCode WM_Initialize(void *, WMCallbackFunc, u16);
WMErrCode WM_Reset(WMCallbackFunc);
WMErrCode WM_End(WMCallbackFunc);
WMErrCode WM_SetParentParameter(WMCallbackFunc, const WMParentParam *);
WMErrCode WM_StartParent(WMCallbackFunc);
WMErrCode WM_StartScan(WMCallbackFunc, const WMScanParam *);
WMErrCode WM_StartConnect(WMCallbackFunc, const WMBssDesc *, const u8 *);
WMErrCode WM_Disconnect(WMCallbackFunc, u16);

int WM_GetMPSendBufferSize(void);
int WM_GetMPReceiveBufferSize(void);

WMErrCode WM_StartMPEx(WMCallbackFunc, u16 *, u16, u16 *, u16, u16, u16, int, int, int, int);
WMErrCode WM_SetMPDataToPortEx(WMCallbackFunc, void *, const u16 *, u16, u16, u16, u16);
WMErrCode WM_SetMPDataToPort(WMCallbackFunc, const u16 *, u16, u16, u16, u16);

#ifdef __cplusplus
}
#endif

#endif
