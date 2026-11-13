/* NitroSDK wm_sync.c: the WM calls that answer at once, without a request
 * to ARM7: the indication and port callbacks, and reads of the status block
 * ARM7 shares, each invalidated from the data cache before it is read. This
 * SDK is older than pret's pokediamond: WM_SetPortCallback only stores the
 * callback and its argument, and WM_GetAllowedChannel answers 0 before
 * WM_Init. */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/mi/memory.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

#define WM_NUM_MAX_CHILD 15

/* Install the callback for the indications ARM7 sends outside any request's
 * own callback. */
WMErrCode WM_SetIndCallback(WMCallbackFunc callback) {
    WMErrCode result;
    OSIntrMode e;

    e = OS_DisableInterrupts();

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS) {
        OS_RestoreInterrupts(e);
        return result;
    }

    WM_GetSystemWork()->indCallback = callback;
    OS_RestoreInterrupts(e);

    return WM_ERRCODE_SUCCESS;
}

/* Install port's receive callback and the argument its record carries. */
WMErrCode WM_SetPortCallback(u16 port, WMCallbackFunc callback, void *arg) {
    WMErrCode result;
    OSIntrMode e;

    e = OS_DisableInterrupts();

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS) {
        OS_RestoreInterrupts(e);
        return result;
    }

    {
        WMArm9Buf *p = WM_GetSystemWork();

        p->portCallbackTable[port] = callback;
        p->portCallbackArgument[port] = arg;
    }

    OS_RestoreInterrupts(e);

    return WM_ERRCODE_SUCCESS;
}

/* Copy the status block into statusBuf. */
WMErrCode WM_ReadStatus(WMStatus *statusBuf) {
    WMErrCode result;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
        return result;

    if (statusBuf == NULL)
        return WM_ERRCODE_INVALID_PARAM;

    DC_InvalidateRange(p->status, sizeof(WMStatus));
    MI_CpuCopyFast(p->status, statusBuf, sizeof(WMStatus));

    return WM_ERRCODE_SUCCESS;
}

/* The send buffer WM_StartMPEx needs, for a parent or a child that has not
 * started MP yet; 0 otherwise. */
int WM_GetMPSendBufferSize(void) {
    WMErrCode result;
    int maxSendSize;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WM_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
        return 0;

    DC_InvalidateRange(&(p->status->mp_flag), 4);
    if (p->status->mp_flag == TRUE)
        return 0;

    DC_InvalidateRange(&(p->status->mp_maxSendSize), 4);
    maxSendSize = p->status->mp_maxSendSize;

    return ((maxSendSize + 31) & ~0x1f);
}

/* The receive buffer WM_StartMPEx needs: a parent's holds one block per
 * child it may take, a child's one frame from the parent. Twice that, for
 * the two buffers ARM7 alternates between. */
int WM_GetMPReceiveBufferSize(void) {
    WMErrCode result;
    BOOL isParent;
    int maxReceiveSize;
    int maxEntry;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WM_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
        return 0;

    DC_InvalidateRange(&(p->status->mp_flag), 4);
    if (p->status->mp_flag == TRUE)
        return 0;

    DC_InvalidateRange(&(p->status->aid), 2);
    isParent = (p->status->aid == 0) ? TRUE : FALSE;
    DC_InvalidateRange(&(p->status->mp_maxRecvSize), 2);
    maxReceiveSize = p->status->mp_maxRecvSize;

    if (isParent == TRUE) {
        DC_InvalidateRange(&(p->status->pparam.maxEntry), 2);
        maxEntry = p->status->pparam.maxEntry;
        return (int)((sizeof(WMMpRecvHeader) - sizeof(WMMpRecvData) +
                      ((sizeof(WMMpRecvData) + maxReceiveSize - 2 + 2) * maxEntry) + 31) &
                     ~0x1f) *
               2;
    } else {
        return (int)((sizeof(WMMpRecvBuf) + maxReceiveSize - 4 + 31) & ~0x1f) * 2;
    }
}

/* Child aid's block in what the parent received, or NULL when that child is
 * not connected or sent nothing. */
WMMpRecvData *WM_ReadMPData(const WMMpRecvHeader *header, u16 aid) {
    int i;
    WMErrCode result;
    WMMpRecvData *recvdata_p[WM_NUM_MAX_CHILD];
    WMArm9Buf *p = WM_GetSystemWork();

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
        return NULL;

    if ((aid < 1) || (aid > WM_NUM_MAX_CHILD))
        return NULL;

    DC_InvalidateRange(&(p->status->child_bitmap), 2);
    if (0 == (p->status->child_bitmap & (0x0001 << aid)))
        return NULL;

    if (header->count == 0)
        return NULL;

    recvdata_p[0] = (WMMpRecvData *)(header->data);

    i = 0;
    do {
        if (recvdata_p[i]->aid == aid)
            return recvdata_p[i];

        ++i;
        recvdata_p[i] = (WMMpRecvData *)((u32)(recvdata_p[i - 1]) + header->length);
    } while (i < header->count);

    return NULL;
}

/* The channels the console may use, bit n-1 for channel n. */
u16 WM_GetAllowedChannel(void) {
    WMErrCode result;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WMi_CheckIdle();
    if (result != WM_ERRCODE_SUCCESS)
        return 0;

    DC_InvalidateRange(&(p->status->allowedChannel), 2);
    return p->status->allowedChannel;
}

/* The radio link level while MP or DCF runs, as a parent with at least one
 * child or as a child; WM_LINK_LEVEL_0 otherwise. */
WMLinkLevel WM_GetLinkLevel(void) {
    WMErrCode result;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
        return WM_LINK_LEVEL_0;

    DC_InvalidateRange(&(p->status->state), 2);
    switch (p->status->state) {
    case WM_STATE_MP_PARENT:
        DC_InvalidateRange(&(p->status->child_bitmap), 2);
        if (p->status->child_bitmap == 0)
            return WM_LINK_LEVEL_0;
        /* fall through */
    case WM_STATE_MP_CHILD:
    case WM_STATE_DCF_CHILD:
        DC_InvalidateRange(&(p->status->linkLevel), 2);
        return (WMLinkLevel)(p->status->linkLevel);
    }

    return WM_LINK_LEVEL_0;
}
