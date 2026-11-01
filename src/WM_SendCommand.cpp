//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

extern int OS_ReceiveMessage(void *a, void **out, int c);
/* MI_DmaCopy32 */
extern void DMASyncWordTransfer(u32 dmaNo, const void *src, void *dest, u32 size);
extern int PXI_SendWordByFifo(u32 a, u32 c, u32 b);
extern int OS_SendMessage(void *a, void *b, int c);

extern void *data_020a89b0;

typedef struct {
    char pad0[0xc];
    u32 field_c;
    char pad10[0x550];
    u16 field_560;
} Ctx;

extern Ctx *data_020a89ac;

int WM_SendCommand(short a0, unsigned short count, ...)
{
    int *buf;
    int *src;
    int i;
    int r;

    OS_ReceiveMessage(&data_020a89b0, (void **)&buf, 1);

    if (count == 0) {
        CP15::FlushDataCache(data_020a89ac->field_c, 0x100);
        CP15::InvalidateDataCache((u32)buf, 0x100);
        DMASyncWordTransfer(data_020a89ac->field_560, (const void *)data_020a89ac->field_c, buf,
                            0x100);
    }

    *(u16 *)buf = a0;
    src = (int *)((((u32)&count) & ~3u) + 4);
    for (i = 0; i < count; i++) {
        src++;
        (buf + i)[1] = src[-1];
    }

    CP15::FlushDataCache((u32)buf, 0x100);
    r = PXI_SendWordByFifo(0xa, (u32)buf, 0);
    OS_SendMessage(&data_020a89b0, buf, 1);
    if (r < 0) return 8;
    return 2;
}
}
