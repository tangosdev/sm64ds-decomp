#include "private/arm9_wbt.h"

/* Whether packet i is marked received in the bitmap (func_020647a4 sets the
 * bit). */
int func_020647d8(WBTPacketBitmap *p, int i) {
    int w = p->bitmap[i >> 5];
    i = 1 << (i & 0x1f);
    return (w & i) != 0;
}
