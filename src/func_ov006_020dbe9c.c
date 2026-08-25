// NONMATCHING: mwccarm extracts the sin/cos fixed-point results ry-before-rx, whereas the
// ROM extracts rx first (which also parks idx_l in r6 rather than lr); div=7, all inside the
// +0x80..+0x94 scheduling window. Logic verified byte-exact outside that window. The order is
// an mwccarm list-scheduler tie-break not reachable by C statement reordering, type, or pragma
// at 2004/b56 (every hand lever floors at 7; the permuter could not beat it either). Counts as
// decompiled, not matched.
//
// func_ov006_020dbe9c: dScMgCoin_c per-face rotated-sprite render, the 6th call of
// dScMgCoin_c::Render (src/_ZN11dScMgCoin_c6RenderEv.cpp). When the face's active flag
// (s[0x1bd]) is set, it builds a 2x2 rotation matrix {cos,sin,-sin,cos} from the shared
// sin/cos table data_02082214 (indexed by (idx_h>>4)*2, idx_h read from 0x51b8) and submits
// the face's OAM object data_ov006_02134b4c[idx_l] at screen (xr>>12, yr>>12) via the OAM
// render helper func_ov004_020b023c.
#pragma opt_propagation off
#include "common.h"
extern s16 data_02082214[];
extern void *data_ov006_02134b4c[];
extern void func_ov004_020b023c(void *obj, int x, int y, int w, int *vec);
void func_ov006_020dbe9c(char *c)
{
    char *s = c + 0x5000;
    if (*(u8 *)(s + 0x1bd) == 0)
        return;
    {
        u16 idx_h = *(u16 *)(c + 0x5100 + 0xb8);
        s32 xr = *(s32 *)(s + 0x1a8);
        s32 yr = *(s32 *)(s + 0x1ac);
        int i = (idx_h >> 4) * 2;
        s16 cosv = data_02082214[i + 1];
        s64 cos64 = (s64)cosv << 12;
        cos64 += 0x800;
        s64 sin64 = (s64)data_02082214[i] << 12;
        sin64 += 0x800;
        int rx = (int)(cos64 >> 12);
        u8 idx_l = *(u8 *)(s + 0x1be);
        int ry = (int)(sin64 >> 12);
        int vec[4];
        vec[0] = rx;
        vec[1] = ry;
        vec[2] = -ry;
        vec[3] = rx;
        func_ov004_020b023c(data_ov006_02134b4c[idx_l], xr >> 12, yr >> 12, -1, vec);
    }
}
