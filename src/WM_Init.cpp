//cpp
/* NitroSDK wm_system.c WM_Init: take buf (0x1300 bytes, 32-byte aligned) as the
 * WM system buffer, once. ILLEGAL_STATE (3) on a second call, INVALID_PARAM (6)
 * for a null or misaligned buf or a DMA channel past 3, WM_DISABLE (4) when
 * ARM7 has no WM handler on PXI tag 0xa. Then clear the buffer with DMA channel
 * ch and carve ARM7's work area, the status block and the two FIFO buffers out
 * of it, clear bit 0 of the shared halfword at 0x027fff96
 * (WmClearFifoRecvFlag), clear every port callback, fill the free-buffer queue
 * with the ten 0x100-byte FIFO blocks and install the PXI receive callback
 * (WmReceiveFifo). */
#include "types.h"
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern void PXI_Init(void);
extern int PXI_IsCallbackReady(int tag, int proc);
/* MI_DmaFill32 */
extern void DMASyncFillTransfer(u32 dmaNo, void *dest, u32 data, u32 size);
extern void WmClearFifoRecvFlag(void);
extern int WM_SetPortCallback(int idx, int b, int c);
extern void OS_InitMessageQueue(void *s, int *v1, int v2);
extern int OS_SendMessage(void *q, void *val, int flag);
extern void PXI_SetFifoRecvCallback(int tag, void *callback);

extern u16 data_020a89a8;
extern WMArm9Buf *data_020a89ac;
extern void *data_020a89b0; /* the free FIFO-buffer message queue */
extern int data_020a89c4;
extern char data_020a8a40[];
extern void WmReceiveFifo(void);

int WM_Init(void *buf, int ch) {
    if (data_020a89a8 != 0)
        return 3;
    data_020a89a8 = 1;
    if (buf == 0) {
        data_020a89a8 = 0;
        return 6;
    }
    if ((unsigned int)ch > 3) {
        data_020a89a8 = 0;
        return 6;
    }
    if (((int)buf & 0x1f) != 0) {
        data_020a89a8 = 0;
        return 6;
    }

    PXI_Init();
    if (PXI_IsCallbackReady(0xa, 1) == 0) {
        data_020a89a8 = 0;
        return 4;
    }

    CP15::InvalidateDataCache((unsigned int)buf, 0x1300);
    DMASyncFillTransfer(ch, buf, 0, 0x1300);

    data_020a89ac = (WMArm9Buf *)buf;
    data_020a89ac->WM7 = (void *)((int)buf + 0x600);
    data_020a89ac->status = (WMStatus *)((int)data_020a89ac->WM7 + 0x100);
    data_020a89ac->fifo9to7 = (void *)((int)data_020a89ac->status + 0xa00);
    data_020a89ac->fifo7to9 = (void *)((int)data_020a89ac->fifo9to7 + 0x100);

    WmClearFifoRecvFlag();
    data_020a89ac->dmaNo = (u16)ch;
    data_020a89ac->initCleared = 0;

    {
        int i;
        for (i = 0; i < 0x10; i++)
            WM_SetPortCallback((u16)i, 0, 0);
    }

    OS_InitMessageQueue(&data_020a89b0, &data_020a89c4, 0xa);
    {
        int i;
        char *p = &data_020a8a40[0];
        for (i = 0; i < 0xa; i++) {
            OS_SendMessage(&data_020a89b0, p, 1);
            p += 0x100;
        }
    }

    PXI_SetFifoRecvCallback(0xa, (void *)WmReceiveFifo);
    return 0;
}
}
