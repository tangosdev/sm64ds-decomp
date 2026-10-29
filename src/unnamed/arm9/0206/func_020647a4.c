#include "private/arm9_wbt.h"

/* Mark packet value received: count it, remember it as the latest, and set
 * its bit. func_02063ea8 calls it once func_02064674 has copied the packet
 * into the buffer. */
void func_020647a4(char *self, int value)
{
    WBTPacketBitmap *pkt_bmp = (WBTPacketBitmap *)self;
    pkt_bmp->recv_count += 1;
    pkt_bmp->current = value;

    pkt_bmp->bitmap[value >> 5] |= 1 << (value & 0x1f);
}
