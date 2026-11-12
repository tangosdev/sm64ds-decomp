#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"

extern int func_02065bd0(void);

/* NitroSDK WBT: reset one peer's packet bitmap for a transfer of length
 * bytes into buffer: the packet count (length over the packet size,
 * rounded up), zeroed cursors, then the bitmap words cleared. */
void func_020647fc(WBTPacketBitmap *pkt, int length, unsigned int *bitmap, unsigned char *buffer) {
    int d = func_02065bd0();
    int r;
    pkt->length = length;
    pkt->next = 0;
    pkt->current = 0;
    pkt->recv_count = 0;
    pkt->count = length / d + (length % d != 0);
    pkt->bitmap = bitmap;
    pkt->buffer = buffer;
    r = WBT_CalcPacketbitmapSize(length);
    MI_CpuClear8(bitmap, r);
}
