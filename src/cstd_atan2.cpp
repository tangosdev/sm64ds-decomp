//cpp
/* cstd::atan2(int, int) at 0x0203b4dc.
 *
 * N64 SM64's atan2s (sm64/src/engine/math_util.c) in fixed point with its two
 * arguments swapped: atan2(a, b) here is atan2s(b, a) there. Each octant
 * indexes gArctanTable, the N64 table of 0x401 angles
 * (sm64/include/trig_tables.inc.c) copied byte for byte, at FX_Div(small, large) >> 2,
 * the same ratio * 1024 atan2_lookup takes.
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef int s32;
typedef short s16;

#include "nitro/fx/fx_cp.h"

extern "C" {

s32 func_01ffa4bc(s32 a);
s32 func_01ff98f4(s32 a, s32 b);
}

extern unsigned short gArctanTable[];


namespace cstd {

s16 atan2(s32 y, s32 x) {
    s32 r;
    if (y == 0) {
        if (x >= 0)
            r = 0;
        else
            r = 0x8000;
    } else if (x == 0) {
        if (y >= 0)
            r = 0x4000;
        else
            r = 0xc000;
    } else if (y >= 0) {
        if (x >= 0) {
            if (x >= y)
                r = gArctanTable[FX_Div(y, x) >> 2];
            else
                r = 0x4000 - gArctanTable[FX_Div(x, y) >> 2];
        } else {
            if (-x < y)
                r = gArctanTable[FX_Div(-x, y) >> 2] + 0x4000;
            else
                r = 0x8000 - gArctanTable[FX_Div(y, -x) >> 2];
        }
    } else if (func_01ff98f4(func_01ffa4bc(x), 0)) {
        if (x <= y)
            r = gArctanTable[FX_Div(-y, -x) >> 2] + 0x8000;
        else
            r = 0xc000 - gArctanTable[FX_Div(-x, -y) >> 2];
    } else {
        if (x < -y)
            r = gArctanTable[FX_Div(x, -y) >> 2] + 0xc000;
        else
            r = -gArctanTable[FX_Div(-y, x) >> 2];
    }
    return (s16)r;
}

}
