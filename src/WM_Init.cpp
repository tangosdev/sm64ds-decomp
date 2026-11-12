//cpp
/* NitroSDK wm_system.c WM_Init: take buf (0x1300 bytes, 32-byte aligned) as the
 * WM system buffer, once. ILLEGAL_STATE (3) on a second call, INVALID_PARAM (6)
 * for a null or misaligned buf or a DMA channel past 3, WM_DISABLE (4) when
 * ARM7 has no WM handler on PXI_FIFO_TAG_WM. Then clear the buffer with DMA channel
 * ch and carve ARM7's work area, the status block and the two FIFO buffers out
 * of it, clear the exception bit of OS_GetSystemWork()->wm_callback_control
 * (WmClearFifoRecvFlag), clear every port callback, fill the free-buffer queue
 * with the ten 0x100-byte FIFO blocks and install the PXI receive callback
 * (WmReceiveFifo). */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"
#include "nitro/os/message.h"
#include "nitro/mi/dma.h"
#include "nitro/pxi/fifo.h"
#include "nitro/pxi/init.h"

typedef int BOOL;

extern "C" {

extern void WmClearFifoRecvFlag(void);

extern u16 data_020a89a8;
extern WMArm9Buf *data_020a89ac;
extern OSMessageQueue data_020a89b0; /* the free FIFO-buffer message queue */
extern OSMessage data_020a89c4[];
extern char data_020a8a40[];
extern void WmReceiveFifo(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err);

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
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WM, PXI_PROC_ARM7)) {
        data_020a89a8 = 0;
        return 4;
    }

    DC_InvalidateRange(buf, 0x1300);
    MI_DmaFill32(ch, buf, 0, 0x1300);

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

    OS_InitMessageQueue(&data_020a89b0, data_020a89c4, 0xa);
    {
        int i;
        char *p = &data_020a8a40[0];
        for (i = 0; i < 0xa; i++) {
            OS_SendMessage(&data_020a89b0, p, OS_MESSAGE_BLOCK);
            p += 0x100;
        }
    }

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WM, WmReceiveFifo);
    return 0;
}
}
