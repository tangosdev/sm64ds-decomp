//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "private/wm_views.h"
#include "nitro/os/message.h"
#include "nitro/mi/dma.h"

extern "C" {

extern int PXI_SendWordByFifo(u32 a, u32 c, u32 b);
extern OSMessageQueue data_020a89b0;

extern WMArm9Buf *data_020a89ac;

int WM_SendCommand(short a0, unsigned short count, ...)
{
    int *buf;
    int *src;
    int i;
    int r;

    OS_ReceiveMessage(&data_020a89b0, (OSMessage *)&buf, OS_MESSAGE_BLOCK);

    if (count == 0) {
        DC_StoreRange(data_020a89ac->fifo9to7, 0x100);
        DC_InvalidateRange(buf, 0x100);
        MI_DmaCopy32(data_020a89ac->dmaNo, data_020a89ac->fifo9to7, buf, 0x100);
    }

    *(u16 *)buf = a0;
    src = (int *)((((u32)&count) & ~3u) + 4);
    for (i = 0; i < count; i++) {
        src++;
        (buf + i)[1] = src[-1];
    }

    DC_StoreRange(buf, 0x100);
    r = PXI_SendWordByFifo(0xa, (u32)buf, 0);
    OS_SendMessage(&data_020a89b0, buf, OS_MESSAGE_BLOCK);
    if (r < 0) return 8;
    return 2;
}
}
