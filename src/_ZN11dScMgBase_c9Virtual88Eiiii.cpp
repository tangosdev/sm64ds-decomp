//cpp
// @symbol _ZN11dScMgBase_c9Virtual88Eiiii
/* dScMgBase_c::Virtual88 - slot 34.  THE BRUSH.

   Walks a `size` x `size` square centred on (cx, cy) and paints every cell
   inside it with palette index `colour`.  The address arithmetic is the DS
   4bpp background-character layout: `(x/8 + (y/8)*32)*32 + (y&7)*4` picks the
   word holding eight pixels of one row of one 8x8 tile, `(x&7)*4` is the
   nibble inside it, and MultiCopy_Int is used for the read and the write-back
   rather than a plain load/store.  Sixteen colours, one nibble per pixel.  All
   four sides clip.

   It reads exactly the two object fields slot 33 (Virtual84) initialises.
   `obj+0x6c` is the layer index: 0..3 select G2S::GetBG0CharPtr through
   GetBG3CharPtr, and anything else returns without drawing -- which is what
   slot 33's -1 buys.  `obj+0x68` gates the wrapped region above the touch
   screen, where the MAIN engine's G2::GetBG*CharPtr are used instead and y is
   folded by `+ data_ov004_020beb6c + 0xc0`.  So slot 33 leaves the brush
   disabled and a minigame arms it by choosing a layer.

   Its only in-family caller is ov004:0x020ae5c4, the line rasteriser sitting
   immediately after it in the image, which dispatches through +0x88 at seven
   separate sites as it walks a segment.  See the slot-34 block in
   include/dScMgBase_c.h for the arity and return-type measurements.

   `Virtual88` is a placeholder after the +0x88 vtable offset, not a ROM name --
   but unlike slots 29-32 there is no wrong name being retired here, because
   the recovery pass never guessed one for this address. */
#include "types.h"
#include "dScMgBase_c.h"
extern "C" {
extern int* func_02054efc(void);
extern int* func_02054ea8(void);
extern int* _ZN2G213GetBG2CharPtrEv(void);
extern int* func_02054d88(void);
extern int* _ZN3G2S13GetBG0CharPtrEv(void);
extern int* _ZN3G2S13GetBG1CharPtrEv(void);
extern int* _ZN3G2S13GetBG2CharPtrEv(void);
extern int* _ZN3G2S13GetBG3CharPtrEv(void);
extern void MultiCopy_Int(void* a, void* b, int n);
}

extern "C" {
extern int data_ov004_020beb64;
extern int data_ov004_020beb6c;
}

void dScMgBase_c::Virtual88(int cx, int cy, int val, int n)
{
    char *obj = (char *)this;

    int oi;
    int x0;
    int ci0;
    int m;
    int xx;
    int yy;
    int ci;
    int yrow;
    int four;
    int half;

    half = n / 2;
    oi = 0;
    if (n <= 0)
        return;
    x0 = cx - half;
    ci0 = oi;
    m = 0xf;
    yrow = cy - half;
    four = 4;
    do {
        ci = ci0;
        if (n > 0) {
            do {
                yy = yrow;
                xx = ci + x0;
                int* base;
                int* cell;
                int sh;

                if (xx < 0)
                    goto inc;
                if (xx >= 0x100)
                    goto inc;
                if (yrow < -0xc0 - data_ov004_020beb6c)
                    goto inc;
                if (yrow < -data_ov004_020beb6c) {
                    if (*(u8*)(obj + 0x68) == 0)
                        goto inc;
                    switch (*(int*)(obj + 0x6c)) {
                    case 0: base = func_02054efc(); break;
                    case 1: base = func_02054ea8(); break;
                    case 2: base = _ZN2G213GetBG2CharPtrEv(); break;
                    case 3: base = func_02054d88(); break;
                    default: return;
                    }
                    yy += data_ov004_020beb6c + 0xc0;
                } else {
                    if (yrow < 0)
                        goto inc;
                    if (yrow >= 0xc0)
                        goto inc;
                    switch (*(int*)(obj + 0x6c)) {
                    case 0: base = _ZN3G2S13GetBG0CharPtrEv(); break;
                    case 1: base = _ZN3G2S13GetBG1CharPtrEv(); break;
                    case 2: base = _ZN3G2S13GetBG2CharPtrEv(); break;
                    case 3: base = _ZN3G2S13GetBG3CharPtrEv(); break;
                    default: return;
                    }
                }
                cell = (int*)((char*)base + ((xx / 8 + (yy / 8) * 32) * 32) + ((yy & 7) * 4));
                MultiCopy_Int(cell, &data_ov004_020beb64, four);
                sh = (xx & 7) << 2;
                data_ov004_020beb64 = (data_ov004_020beb64 & (-1 ^ (m << sh))) | (val << sh);
                MultiCopy_Int(&data_ov004_020beb64, cell, four);
            inc:;
            } while (++ci < n);
        }
        yrow += 1;
        oi += 1;
    } while (oi < n);
}


