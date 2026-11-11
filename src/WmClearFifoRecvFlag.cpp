//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "private/wm_views.h"
#include "nitro/mi/memory.h"
#include "nitro/os/system.h"

extern "C" {

typedef void (*Handler)(void *msg);

typedef struct {
    u16 w0;      /* 0x00 */
    u16 w1;      /* 0x02 */
    u16 w2;      /* 0x04 */
    u16 w6;      /* 0x06 */
    u32 d8;      /* 0x08 */
    u32 dc;      /* 0x0c */
    u16 w10;     /* 0x10 */
    u16 w12;     /* 0x12 */
    char _14[6]; /* 0x14 */
    u16 w1a;     /* 0x1a */
    u32 d1c;     /* 0x1c */
} BufT;

/* NitroSDK WMPortSendCallback, the field read here: at 0x14 the callback
 * WM_SetMPDataToPortEx was given, which ARM7 hands back with the result. */
typedef struct WMPortSendCallback {
    u16 apiid;        /* 0x00 */
    u16 errcode;      /* 0x02 */
    char _04[0x10];   /* 0x04 */
    Handler callback; /* 0x14 */
} WMPortSendCallback;

extern int WM_Finish(void);
extern void WmClearFifoRecvFlag(void);
extern WMArm9Buf *data_020a89ac;
extern BufT data_020a89ec;
extern char data_020a8a00;

/* NitroSDK wm_system.c's static WmReceiveFifo, the PXI receive callback
 * WM_Init installs for tag 0xa: msg is the WMCallback ARM7 left in the
 * fifo7to9 buffer. Unless err, dispatch it to the
 * indication callback, the port callback or the callback registered for
 * its API id, call WM_Finish after a successful WM_APIID_END (2), and
 * otherwise end in WmClearFifoRecvFlag. */
void WmReceiveFifo(int tag, u16 *msg, int err) {
    u16 op;

    if (err != 0)
        return;

    DC_InvalidateRange(data_020a89ac->fifo7to9, 0x100);
    DC_InvalidateRange(data_020a89ac->status, 0xa00);

    op = msg[0];
    if (op >= 0x25) {
        if (op == 0x80) {
            if (msg[1] == 0x13)
                OS_Terminate();
            if ((Handler)data_020a89ac->indCallback != 0)
                ((Handler)data_020a89ac->indCallback)(msg);
        } else if (op == 0x82) {
            WMPortRecvCallback *recv = (WMPortRecvCallback *)msg;
            if (data_020a89ac->portCallbackTable[recv->port] != 0) {
                recv->arg = data_020a89ac->portCallbackArgument[recv->port];
                DC_InvalidateRange(recv->recvBuf, data_020a89ac->status->mp_recvBufSize);
                ((Handler)data_020a89ac->portCallbackTable[recv->port])(msg);
            }
        } else if (op == 0x81) {
            msg[0] = 0xf;
            WMPortSendCallback *send = (WMPortSendCallback *)msg;
            if (send->callback != 0)
                send->callback(msg);
        }
    } else {
        if (op == 0xe) {
            if ((u16)(msg[2] + 0xfff5) <= 1 && msg[1] == 0) {
                DC_InvalidateRange(*(void **)(msg + 4), data_020a89ac->status->mp_recvBufSize);
            }
        }

        {
            u16 t = msg[0];
            if ((Handler)data_020a89ac->callbackTable[t] != 0)
                ((Handler)data_020a89ac->callbackTable[t])(msg);
        }

        {
            u16 op2 = msg[0];
            if (op2 == 8 || op2 == 0xc) {
                u16 objType;
                u16 v5;
                void *p;

                if (op2 == 8) {
                    p = &msg[5];
                    objType = msg[4];
                    v5 = msg[8];
                } else if (op2 == 0xc) {
                    objType = msg[4];
                    v5 = msg[8];
                    p = &msg[5];
                }

                if (objType == 7 || objType == 9) {
                    u16 i;
                    data_020a89ec.w0 = 0x82;
                    data_020a89ec.w1 = 0;
                    data_020a89ec.w2 = objType;
                    data_020a89ec.d8 = 0;
                    data_020a89ec.dc = 0;
                    data_020a89ec.w10 = 0;
                    data_020a89ec.w12 = v5;
                    data_020a89ec.w1a = 0xffff;
                    MIi_CpuCopy16(p, &data_020a8a00, 6);
                    for (i = 0; i < 0x10; i++) {
                        data_020a89ec.w6 = i;
                        if (data_020a89ac->portCallbackTable[i] != 0) {
                            data_020a89ec.d1c = (u32)data_020a89ac->portCallbackArgument[i];
                            ((Handler)data_020a89ac->portCallbackTable[i])(&data_020a89ec);
                        }
                    }
                }
            }
        }

        if (msg[0] == 2 && msg[1] == 0) {
            WM_Finish();
            return;
        }
    }

    DC_InvalidateRange(data_020a89ac->fifo7to9, 0x100);
    WmClearFifoRecvFlag();
}

/* NitroSDK wm_system.c's static WmClearFifoRecvFlag: clear bit 0 of the
 * WM callback-control halfword shared with ARM7 (0x027fff96). */
void WmClearFifoRecvFlag(void) {
    u16 *p = (u16 *)0x027fff96;
    if (*p & 1)
        *p &= ~1;
}
}
