/* NitroSDK wm_system.c: the ARM9 half of the WM library. WM_Init takes the
 * system buffer and connects the PXI FIFO to ARM7. WM_SendCommand passes a
 * request through one of ten FIFO blocks. WmReceiveFifo hands each result
 * ARM7 sends back to the callback registered for its API id, to the
 * indication callback or to the port callbacks. This SDK is older than
 * pret's pokediamond: WM_Init carves the buffer itself (there is no
 * WmInitCore or WMi_InitForScan), a command with no parameters sends a copy
 * of the command block its caller filled in, and WmReceiveFifo keeps no
 * connected-AID bitmap. */
#include "types.h"
#include "nitro/mi/dma.h"
#include "nitro/mi/memory.h"
#include "nitro/os/cache.h"
#include "nitro/os/message.h"
#include "nitro/os/system.h"
#include "nitro/os/systemWork.h"
#include "nitro/pxi/fifo.h"
#include "nitro/pxi/init.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"

typedef int BOOL;
#define FALSE 0
#define NULL 0

#define WM_BUF_MSG_NUM 10
#define WM_ARM9WM_BUF_SIZE 0x600
#define WM_ARM7WM_BUF_SIZE 0x100
#define WM_STATUS_BUF_SIZE 0xa00
#define WM_FIFO_BUF_SIZE 0x100
#define WM_SYSTEM_BUF_SIZE \
    (WM_ARM9WM_BUF_SIZE + WM_ARM7WM_BUF_SIZE + WM_STATUS_BUF_SIZE + WM_FIFO_BUF_SIZE + WM_FIFO_BUF_SIZE)
#define WM_SIZE_MACADDR 6
#define MI_DMA_MAX_NUM 3

/* nitro/wm/common/wm.h: the bit of OS_GetSystemWork()->wm_callback_control
 * ARM7 sets when it delivers a callback from an exception. */
#define WM_EXCEPTION_CB_MASK 0x0001

/* The result WmReceiveFifo reads past the WMCallback head of a port send. */
typedef struct WMPortSendCallback {
    u16 apiid;   /* 0x00 */
    u16 errcode; /* 0x02 */
    u8 reserved[0x10];
    WMCallbackFunc callback; /* 0x14 -- the one WM_SetMPDataToPort was given */
} WMPortSendCallback;

/* The CONNECTED and DISCONNECTED notices a parent (WM_StartParent) and a
 * child (WM_StartConnect) receive: this SDK's WmReceiveFifo reads the state,
 * the peer's MAC address and the AID at the same offsets in both. */
typedef struct WMStartParentCallback {
    u16 apiid;                      /* 0x00 */
    u16 errcode;                    /* 0x02 */
    u16 wlCmdID;                    /* 0x04 */
    u16 wlResult;                   /* 0x06 */
    u16 state;                      /* 0x08 */
    u8 macAddress[WM_SIZE_MACADDR]; /* 0x0a */
    u16 aid;                        /* 0x10 */
} WMStartParentCallback;

static u16 wmInitialized;
static WMArm9Buf *wm9buf;
static OSMessageQueue bufMsgQ;
static OSMessage bufMsg[WM_BUF_MSG_NUM];
static u32 fifoBuf[WM_BUF_MSG_NUM][WM_FIFO_BUF_SIZE / sizeof(u32)] __attribute__((aligned(32)));

static void WmReceiveFifo(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err);
static void WmClearFifoRecvFlag(void);

/* Take wmSysBuf (WM_SYSTEM_BUF_SIZE bytes, 32-byte aligned) as the system
 * buffer, once: ARM9's work, ARM7's work, the status block and the two FIFO
 * blocks. */
WMErrCode WM_Init(void *wmSysBuf, u16 dmaNo) {
    if (wmInitialized)
        return WM_ERRCODE_ILLEGAL_STATE;
    wmInitialized = 1;

    if (wmSysBuf == NULL) {
        wmInitialized = 0;
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (dmaNo > MI_DMA_MAX_NUM) {
        wmInitialized = 0;
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)wmSysBuf & 0x01f) {
        wmInitialized = 0;
        return WM_ERRCODE_INVALID_PARAM;
    }

    PXI_Init();
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WM, PXI_PROC_ARM7)) {
        wmInitialized = 0;
        return WM_ERRCODE_WM_DISABLE;
    }

    DC_InvalidateRange(wmSysBuf, WM_SYSTEM_BUF_SIZE);
    MI_DmaClear32(dmaNo, wmSysBuf, WM_SYSTEM_BUF_SIZE);

    wm9buf = (WMArm9Buf *)wmSysBuf;
    wm9buf->WM7 = (void *)((u32)wm9buf + WM_ARM9WM_BUF_SIZE);
    wm9buf->status = (WMStatus *)((u32)(wm9buf->WM7) + WM_ARM7WM_BUF_SIZE);
    wm9buf->fifo9to7 = (void *)((u32)(wm9buf->status) + WM_STATUS_BUF_SIZE);
    wm9buf->fifo7to9 = (void *)((u32)(wm9buf->fifo9to7) + WM_FIFO_BUF_SIZE);

    WmClearFifoRecvFlag();
    wm9buf->dmaNo = dmaNo;
    wm9buf->initCleared = 0;

    {
        s32 i;
        for (i = 0; i < WM_NUM_OF_PORT; i++)
            (void)WM_SetPortCallback((u16)i, NULL, NULL);
    }

    OS_InitMessageQueue(&bufMsgQ, bufMsg, WM_BUF_MSG_NUM);
    {
        s32 i;
        for (i = 0; i < WM_BUF_MSG_NUM; i++)
            (void)OS_SendMessage(&bufMsgQ, fifoBuf[i], OS_MESSAGE_BLOCK);
    }

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WM, WmReceiveFifo);
    return WM_ERRCODE_SUCCESS;
}

/* Once ARM7 has ended WM (WM_STATE_READY), drop the PXI receive callback and
 * forget the system buffer. */
WMErrCode WM_Finish(void) {
    WMErrCode result;

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
        return WM_ERRCODE_ILLEGAL_STATE;

    result = WM_CheckStateEx(1, WM_STATE_READY);
    if (result != WM_ERRCODE_SUCCESS)
        return result;

    WmClearFifoRecvFlag();
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WM, NULL);
    wm9buf = 0;
    wmInitialized = 0;

    return WM_ERRCODE_SUCCESS;
}

/* The callback for request id's result. */
void WM_SetCallbackTable(WMApiid id, WMCallbackFunc callback) {
    wm9buf->callbackTable[id] = callback;
}

/* Send request id with paramNum word parameters through a free FIFO block.
 * With no parameters the request is the command block its caller already
 * filled in (WM_StartScan, WM_MeasureChannel), copied into the FIFO block.
 * The parameters are walked by hand, as va_arg does. */
WMErrCode WM_SendCommand(WMApiid id, u16 paramNum, ...) {
    u32 *tmpAddr;
    char *vlist;
    s32 i;
    int result;

    (void)OS_ReceiveMessage(&bufMsgQ, (OSMessage *)&tmpAddr, OS_MESSAGE_BLOCK);

    if (paramNum == 0) {
        DC_StoreRange(wm9buf->fifo9to7, WM_FIFO_BUF_SIZE);
        DC_InvalidateRange(tmpAddr, WM_FIFO_BUF_SIZE);
        MI_DmaCopy32(wm9buf->dmaNo, wm9buf->fifo9to7, tmpAddr, WM_FIFO_BUF_SIZE);
    }

    *(u16 *)tmpAddr = (u16)id;
    vlist = (char *)(((uintptr_t)(&paramNum)) & ~3) + 4;
    for (i = 0; i < paramNum; i++)
        *(tmpAddr + i + 1) = *(u32 *)((vlist += 4) - 4);

    DC_StoreRange(tmpAddr, WM_FIFO_BUF_SIZE);
    result = PXI_SendWordByFifo(PXI_FIFO_TAG_WM, (u32)tmpAddr, FALSE);
    (void)OS_SendMessage(&bufMsgQ, tmpAddr, OS_MESSAGE_BLOCK);

    if (result < 0)
        return WM_ERRCODE_FIFO_ERROR;
    return WM_ERRCODE_OPERATING;
}

WMArm9Buf *WM_GetSystemWork(void) {
    return wm9buf;
}

WMErrCode WMi_CheckInitialized(void) {
    if (!wmInitialized)
        return WM_ERRCODE_ILLEGAL_STATE;
    return WM_ERRCODE_SUCCESS;
}

/* Also ILLEGAL_STATE while ARM7 is still READY or STOP. */
WMErrCode WMi_CheckIdle(void) {
    WMErrCode result;

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
        return result;

    DC_InvalidateRange(&(wm9buf->status->state), 2);
    if ((wm9buf->status->state == WM_STATE_READY) || (wm9buf->status->state == WM_STATE_STOP))
        return WM_ERRCODE_ILLEGAL_STATE;

    return WM_ERRCODE_SUCCESS;
}

/* SUCCESS when the current state is one of the paramNum states that follow,
 * ILLEGAL_STATE otherwise. */
WMErrCode WM_CheckStateEx(s32 paramNum, ...) {
    WMErrCode result;
    u16 now;
    u32 temp;
    char *vlist;

    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
        return result;

    DC_InvalidateRange(&(wm9buf->status->state), 2);
    now = wm9buf->status->state;

    result = WM_ERRCODE_ILLEGAL_STATE;
    vlist = (char *)(((uintptr_t)(&paramNum)) & ~3) + 4;
    for (; paramNum; paramNum--) {
        temp = *(u32 *)((vlist += 4) - 4);
        if (temp == now)
            result = WM_ERRCODE_SUCCESS;
    }

    return result;
}

/* The PXI receive callback for PXI_FIFO_TAG_WM: fifo_buf_adr is the result
 * ARM7 wrote. A parent's or a child's CONNECTED and DISCONNECTED notices are
 * also passed to every port callback as a port-receive record. */
static void WmReceiveFifo(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err) {
#pragma unused(tag)
    WMCallback *callback_p = (WMCallback *)fifo_buf_adr;

    if (err)
        return;

    DC_InvalidateRange(wm9buf->fifo7to9, WM_FIFO_BUF_SIZE);
    DC_InvalidateRange(wm9buf->status, WM_STATUS_BUF_SIZE);

    if (callback_p->apiid >= WM_NUM_OF_CALLBACK) {
        if (callback_p->apiid == WM_APIID_INDICATION) {
            if (callback_p->errcode == WM_ERRCODE_FLASH_ERROR)
                OS_Terminate();
            if (wm9buf->indCallback != NULL)
                wm9buf->indCallback((void *)callback_p);
        } else if (callback_p->apiid == WM_APIID_PORT_RECV) {
            WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback_p;

            if (wm9buf->portCallbackTable[cb_Port->port] != NULL) {
                cb_Port->arg = wm9buf->portCallbackArgument[cb_Port->port];
                DC_InvalidateRange(cb_Port->recvBuf, wm9buf->status->mp_recvBufSize);
                (wm9buf->portCallbackTable[cb_Port->port])((void *)cb_Port);
            }
        } else if (callback_p->apiid == WM_APIID_PORT_SEND) {
            WMPortSendCallback *cb_Port = (WMPortSendCallback *)callback_p;

            callback_p->apiid = WM_APIID_SET_MP_DATA;
            if (cb_Port->callback != NULL)
                (cb_Port->callback)((void *)cb_Port);
        }
    } else {
        if (callback_p->apiid == WM_APIID_START_MP) {
            WMStartMPCallback *cb_StartMP = (WMStartMPCallback *)callback_p;

            if (cb_StartMP->state == WM_STATECODE_MPEND_IND ||
                cb_StartMP->state == WM_STATECODE_MP_IND) {
                if (cb_StartMP->errcode == WM_ERRCODE_SUCCESS)
                    DC_InvalidateRange(cb_StartMP->recvBuf, wm9buf->status->mp_recvBufSize);
            }
        }

        if (wm9buf->callbackTable[callback_p->apiid] != NULL)
            (wm9buf->callbackTable[callback_p->apiid])((void *)callback_p);

        if (callback_p->apiid == WM_APIID_START_PARENT ||
            callback_p->apiid == WM_APIID_START_CONNECT) {
            u16 state;
            u16 aid;
            u8 *macAddress;

            if (callback_p->apiid == WM_APIID_START_PARENT) {
                WMStartParentCallback *cb = (WMStartParentCallback *)callback_p;
                macAddress = cb->macAddress;
                state = cb->state;
                aid = cb->aid;
            } else if (callback_p->apiid == WM_APIID_START_CONNECT) {
                WMStartParentCallback *cb = (WMStartParentCallback *)callback_p;
                state = cb->state;
                aid = cb->aid;
                macAddress = cb->macAddress;
            }

            if (state == WM_STATECODE_CONNECTED || state == WM_STATECODE_DISCONNECTED) {
                static WMPortRecvCallback cb_Port;
                u16 iPort;

                cb_Port.apiid = WM_APIID_PORT_RECV;
                cb_Port.errcode = WM_ERRCODE_SUCCESS;
                cb_Port.state = state;
                cb_Port.recvBuf = NULL;
                cb_Port.data = NULL;
                cb_Port.length = 0;
                cb_Port.aid = aid;
                cb_Port.seqNo = 0xffff;
                MI_CpuCopy16(macAddress, cb_Port.macAddress, WM_SIZE_MACADDR);

                for (iPort = 0; iPort < WM_NUM_OF_PORT; iPort++) {
                    cb_Port.port = iPort;
                    if (wm9buf->portCallbackTable[iPort] != NULL) {
                        cb_Port.arg = wm9buf->portCallbackArgument[iPort];
                        (wm9buf->portCallbackTable[iPort])((void *)&cb_Port);
                    }
                }
            }
        }

        if (callback_p->apiid == WM_APIID_END && callback_p->errcode == WM_ERRCODE_SUCCESS) {
            (void)WM_Finish();
            return;
        }
    }

    DC_InvalidateRange(wm9buf->fifo7to9, WM_FIFO_BUF_SIZE);
    WmClearFifoRecvFlag();
}

static void WmClearFifoRecvFlag(void) {
    if (OS_GetSystemWork()->wm_callback_control & WM_EXCEPTION_CB_MASK)
        OS_GetSystemWork()->wm_callback_control &= ~WM_EXCEPTION_CB_MASK;
}
