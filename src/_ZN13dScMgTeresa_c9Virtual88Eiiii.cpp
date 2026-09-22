//cpp
#include "dScMgTeresa_c.h"
// @symbol _ZN13dScMgTeresa_c9Virtual88Eiiii
/* dScMgTeresa_c::Virtual88 - slot 34, a STIPPLED brush.

   Same square walk and the same 4bpp nibble arithmetic as dScMgBase_c's, with
   one difference that changes what it is for: each cell is painted only if
   `data_ov006_0213f9e4[row] & (1 << (col*4))` is set, so the table is a
   bitmapped shape and this stamps that shape rather than a solid block.

   It also never consults the layer index at +0x6c -- it always draws into sub
   BG0 -- which is why `this` arrives and is never read.  The comment in
   include/dScMgTeresa_c.h predicted both of these before the slot was
   declared. */
#pragma opt_loop_invariants off
extern "C" {
extern int data_ov006_0213f9e4[];
extern int data_ov004_020beb6c;
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
}

void dScMgTeresa_c::Virtual88(int cx, int cy, int val, int n)
{
    int half;
    int x0;
    int j;
    int x;
    int buf;
    int *addr;
    int i;
    int y;

    half = n / 2;
    j = 0;
    if (j < n) {
        y = cy - half;
        x0 = cx - half;
        do {
            for (i = 0; i < n; i++) {
                if (data_ov006_0213f9e4[j] & (1 << (i * 4))) {
                    x = i + x0;
                    if (x >= 0 && x < 0x100) {
                        if (y >= -0xc0 - data_ov004_020beb6c && y >= 0 && y < 0xc0) {
                            addr = (int *)((char *)_ZN3G2S13GetBG0CharPtrEv() + ((x / 8) + (y / 8) * 32) * 32 + (y & 7) * 4);
                            MultiCopy_Int(addr, &buf, 4);
                            buf = (buf & (-1 ^ (0xf << ((x & 7) * 4)))) | (val << ((x & 7) * 4));
                            MultiCopy_Int(&buf, addr, 4);
                        }
                    }
                }
            }
            y++;
            j++;
        } while (j < n);
    }
}
