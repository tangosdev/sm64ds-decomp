#include "types.h"
#include "WmWork.h"
extern char *data_020a9db4;
extern char *data_020a9db8;

extern int func_02061c20(int *arg);
extern void func_0206879c(int x, int kind);
extern int func_02061ce4(void *x);
extern int func_02063574(void *a, int b, int c, int d);
extern int func_020620b0(void *self, char *o);
extern int func_020635d8(void *a, int b);
extern int func_02061fdc(void *x);
extern int func_020625fc(void *arg0, int arg1, int arg2, int arg3, u16 arg4, u16 arg5, u16 arg6, int arg7, int arg8, int arg9, int arg10);
extern int func_0206a230(void);
extern int func_020621b8(void *x);

#define DISPATCH(a, b) WM_WORK->userCallback((a), (b))

/* NitroSDK WM callback arguments, as func_02066a94.c and func_02040014.c
 * spell them. Every one starts with WMCallback's apiid/errcode; the switch
 * below picks the shape by apiid. */
typedef struct WmCallback {
    u16 apiid;   /* 0x00 */
    u16 errcode; /* 0x02 */
} WmCallback;

/* Leading fields of WMStartParentCallback (apiid 8). */
typedef struct WmStartParentCallback {
    u16 apiid;    /* 0x00 */
    u16 errcode;  /* 0x02 */
    u16 wlCmdID;  /* 0x04 */
    u16 wlResult; /* 0x06 */
    u16 state;    /* 0x08 -- 2 BEACON_SENT, 7 CONNECTED, 9 DISCONNECTED */
} WmStartParentCallback;

/* Leading fields of WMStartMPCallback (apiid 0xe). */
typedef struct WmStartMPCallback {
    u16 apiid;   /* 0x00 */
    u16 errcode; /* 0x02 */
    u16 state;   /* 0x04 -- 0xa MP_START, 0xb MPEND_IND */
    u8 reserved[2];
    void *recvBuf; /* 0x08 -- the received MP data, passed on for MPEND_IND */
} WmStartMPCallback;

/* Leading fields of WMIndCallback (apiid 0x80). */
typedef struct WmIndCallback {
    u16 apiid;   /* 0x00 */
    u16 errcode; /* 0x02 */
    u16 state;   /* 0x04 */
} WmIndCallback;

void func_02069994(char *arg0)
{
    WmCallback *cb = (WmCallback *)arg0;

    switch (cb->apiid) {
    case 0x0:
        if (cb->errcode == 0) {
            func_0206879c(0x83, func_02061c20((int *)&WM_WORK->status));
            func_0206879c(0x80, func_02061ce4((void*) func_02069994));
            func_0206879c(0x1D, func_02063574((void*) func_02069994, 0xFFFF, 0x28, 5));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0x1D:
        if (cb->errcode == 0) {
            func_0206879c(7, func_020620b0((void *)func_02069994, (char *)&WM_WORK->parentParam));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0x7:
        DISPATCH(0x15, arg0);
        func_0206879c(0x19, func_020635d8((void*) func_02069994, 1));
        return;

    case 0x19:
        if (cb->errcode == 0) {
            func_0206879c(8, func_02061fdc((void*) func_02069994));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0x8:
        if (cb->errcode == 0) {
            switch (((WmStartParentCallback *)arg0)->state) {
            case 0:
                return;
            case 7:
                func_0206879c(0x83, func_02061c20((int *)&WM_WORK->status));
                WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
                DISPATCH(0, arg0);
                /* WmStatus +0x10: a BOOL WM_ReadStatus fills in; which SDK
                   field it is isn't pinned down here, so it stays raw */
                if (*(int *)(data_020a9db4 + 0x1428) == 0) {
                    /* data_020a9db8 +0x1324 (set before func_020625fc, cleared in its callback) and
                       +0x1316 lie in arm9_a9db8_mb_parent.h's padding, with
                       no evidenced name yet, so they stay raw */
                    char *g8 = data_020a9db8;
                    if (*(int *)(g8 + 0x1324) == 0) {
                        int v = 1;
                        *(int *)(g8 + 0x1324) = 1;
                        if (WM_WORK->mpContinuous != 0) {
                            v = 0;
                        }
                        func_0206879c(0xE,
                                      func_020625fc((void *)func_02069994, (int)WM_WORK->recvBuf,
                                                    WM_WORK->recvBufSize, (int)WM_WORK->sendBuf,
                                                    WM_WORK->sendBufSize, (u16)v, 0, 0, 0, 1, 1));
                        return;
                    }
                }
                if (func_0206a230() == 0) {
                    return;
                }
                DISPATCH(0x19, (void *)0);
                return;
            case 9:
                func_0206879c(0x83, func_02061c20((int *)&WM_WORK->status));
                WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
                DISPATCH(1, arg0);
                return;
            case 2:
                DISPATCH(0x1C, arg0);
                return;
            default:
                DISPATCH(0x100, arg0);
                return;
            }
        } else {
            DISPATCH(0x100, arg0);
            return;
        }

    case 0xE:
        *(int *)(data_020a9db8 + 0x1324) = 0;
        switch (((WmStartMPCallback *)arg0)->state) {
        case 0xA:
            WM_WORK->mpStarted = 1;
            DISPATCH(0x19, (void *)0);
            return;
        case 0xB:
            DISPATCH(3, ((WmStartMPCallback *)arg0)->recvBuf);
            return;
        default:
            DISPATCH(0x100, arg0);
            return;
        }

    case 0xF:
        WM_WORK->mpSending = 0;
        {
            u16 t = cb->errcode;
            if (t == 0) {
                DISPATCH(2, arg0);
                DISPATCH(0x19, (void *)0);
                return;
            }
            if (t == 0xA) {
                DISPATCH(0x2A, arg0);
                return;
            }
            DISPATCH(0x13, arg0);
            DISPATCH(0x19, (void *)0);
            return;
        }

    case 0x1:
        if (cb->errcode == 0) {
            func_0206879c(2, func_020621b8((void*) func_02069994));
            return;
        }
        WM_WORK->resetting = 0;
        DISPATCH(0x100, arg0);
        return;

    case 0x2:
        if (cb->errcode == 0) {
            WM_WORK->initialized = 0;
            *(u16 *)(data_020a9db8 + 0x1316) = 0;
            DISPATCH(0x11, arg0);
            return;
        }
        WM_WORK->resetting = 0;
        DISPATCH(0x100, arg0);
        return;

    case 0xD:
        if (cb->errcode != 0) {
            return;
        }
        func_0206879c(0x83, func_02061c20((int *)&WM_WORK->status));
        WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
        return;

    case 0x15:
        if (func_0206a230() == 0) {
            return;
        }
        DISPATCH(0x19, (void *)0);
        return;

    case 0x80:
        switch (((WmIndCallback *)arg0)->state - 0x10) {
        case 0:
            DISPATCH(0x1D, arg0);
            return;
        case 1:
            DISPATCH(0x1F, arg0);
            return;
        case 2:
            DISPATCH(0x20, arg0);
            return;
        case 3:
            DISPATCH(0x21, arg0);
            return;
        }
        break;

    default:
        DISPATCH(0x100, arg0);
        return;
    }
}
