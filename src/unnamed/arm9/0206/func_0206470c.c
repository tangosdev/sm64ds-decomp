#include "private/arm9_wbt.h"

/* The next packet pkt_bmp still lacks, searching on from the one received
 * last (current) and wrapping at count; it is kept in next and returned, or
 * -1 once every bit is set. */
int func_0206470c(WBTPacketBitmap *pkt_bmp) {
    int idx = pkt_bmp->current + 1;
    int start;
    int bit;
    unsigned int *word;
    if (idx >= pkt_bmp->count)
        idx = 0;
    word = pkt_bmp->bitmap + (idx >> 5);
    start = idx;
    bit = idx & 0x1f;
    /* found/fail gotos kept. The ROM leaves the loop with a forward branch over the found
       tail to the fail tail, laid out in label order (0x98). Measured: returning from
       inside the loop, with a for, a break or a do-while, 0x90; breaking to the found
       tail with `return -1` in the loop, 0x94 (the fail return lands straight after
       the loop); while (*word & (1 << bit)) with the fail return inside, 0xa0. */
    for (;;) {
        if (!(*word & (1 << bit))) goto found;
        idx++;
        if (idx < pkt_bmp->count) {
            bit++;
            if (bit > 0x1f) {
                bit = 0;
                word++;
            }
        } else {
            idx = 0;
            bit = 0;
            word = pkt_bmp->bitmap;
        }
        if (idx == start) goto fail;
    }
found:
    pkt_bmp->next = idx;
    return idx;
fail:
    return -1;
}
