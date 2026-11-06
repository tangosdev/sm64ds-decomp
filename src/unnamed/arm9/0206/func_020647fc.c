#include "private/arm9_wbt.h"

extern int func_02065bd0(void);
extern int func_020659f8(int a);
extern void func_0205a588(void *dst, unsigned char value, unsigned int size);

/* NitroSDK WBT: reset one peer's packet bitmap for a transfer of length
 * bytes into buffer: the packet count (length over the packet size,
 * rounded up), zeroed cursors, then the bitmap words cleared. */
void func_020647fc(WBTPacketBitmap *pkt, int length, unsigned int *bitmap, unsigned char *buffer) {
    int d = func_02065bd0();
    int r;
    pkt->length = length;
    pkt->pad_08 = 0;
    pkt->current = 0;
    pkt->recv_count = 0;
    pkt->count = length / d + (length % d != 0);
    pkt->bitmap = bitmap;
    pkt->buffer = buffer;
    r = func_020659f8(length);
    func_0205a588(bitmap, 0, r);
}
