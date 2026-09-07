//cpp
#include "types.h"
#include "dScMgSlot3_c.h"
// @symbol _ZN12dScMgSlot3_c13OnYoshiTryEatEi
// recovered name: dScMgSlot3_c_OnYoshiTryEat_0210b314
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern void func_02012790(unsigned int id);
}

#pragma opt_strength_reduction off

void dScMgSlot3_c::OnYoshiTryEat(int mode)
{
    char *c = (char *)this;

    if (mode == 3 || mode == 0x12) {
        *(int *)(c + 0xa8) = 0xc;
        *(int *)(c + 0xac) = *(int *)(c + 0xa8);
        *(int *)(c + 0xbc) = *(int *)(c + 0x5004);
        if (*(u32 *)(c + 0xbc) > 0x270e) {
            *(int *)(c + 0xbc) = 0x270e;
        }
        *(int *)(c + 0xb4) = 0;
        func_ov004_020adb1c(*(int *)(c + 0xb4));
        func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    } else if (mode == 4) {
        if (*(int *)(c + 0x5004) < 5) {
            *(int *)(int)(c + 0x5004) += 1;
        }
        *(int *)(int)(c + 0xbc) += 1;
        if (*(u32 *)(c + 0xbc) > 0x270e) {
            *(int *)(c + 0xbc) = 0x270e;
        }
    }

    if (mode == 3 || mode == 0x12 || mode == 4) {
        int v;
        int v2;
        int i;
        int j;
        u8 k;
        int m;

        v = *(int *)(c + 0x5004);
        if (v <= 0) {
            *(u8 *)(c + 0x503a) = 3;
        } else if (v <= 2) {
            *(u8 *)(c + 0x503a) = 4;
        } else {
            *(u8 *)(c + 0x503a) = 5;
        }

        v2 = *(int *)(c + 0x5004);
        if (v2 <= 0 || v2 == 1 || v2 == 3) {
            *(u8 *)(c + 0x5040) = 0;
            *(u8 *)(c + 0x5041) = 1;
            *(u8 *)(c + 0x5042) = 0;
        } else if (v2 == 2 || v2 == 4) {
            *(u8 *)(c + 0x5040) = 1;
            *(u8 *)(c + 0x5041) = 0;
            *(u8 *)(c + 0x5042) = 1;
        } else {
            for (i = 0; i < 3; i++) {
                if ((u8)(((u32)RandomIntInternal(&data_0209e650) >> 16) & 1)) {
                    *(u8 *)(c + i + 0x5040) = 1;
                } else {
                    *(u8 *)(c + i + 0x5040) = 0;
                }
            }
        }

        *(u8 *)(c + 0x503c) = ((u32)RandomIntInternal(&data_0209e650) >> 16) % *(u8 *)(c + 0x503a);
        if (mode != 4) {
            *(int *)(c + 0x500c) = 0;
            *(u8 *)(c + 0x503b) = *(u8 *)(c + 0x503c);
        } else {
            *(int *)(c + 0x500c) = 0x50;
            func_02012790(0x1aa);
        }

        {
            char *w = c;
            for (j = 0; j < 3; j++, w += 5) {
                for (k = 0; k < *(u8 *)(c + 0x503a); k++) {
                    if (j == 1) {
                        *(u8 *)(c + k + 0x5021) = *(u8 *)(c + 0x503a) - k - 1;
                    } else {
                        *(u8 *)(w + k + 0x501c) = k;
                    }
                }
            }
        }
        *(int *)(c + 0x4ff0) = (*(u8 *)(c + 0x503a) * 0x50) << 12;
    }

    {
        int j2;
        int k2;
        int m2;
        char *w2 = c;
        for (j2 = 0; j2 < 3; j2++, w2 += 3) {
            *(u8 *)(c + j2 + 0x502b) = 1;
            *(u8 *)(c + j2 + 0x502e) = 0;
            for (k2 = 0; k2 < 3; k2++) {
                *(u8 *)(w2 + k2 + 0x5031) = *(u8 *)(c + 0x503a);
            }
        }
        for (m2 = 0; m2 < 3; m2++) {
            *(int *)(c + m2 * 4 + 0x4ff4) = 0;
        }
        *(u8 *)(c + 0x503d) = 0;
        *(u8 *)(c + 0x503e) = 0x3c;
        *(int *)(c + 0x5000) = 0;
        *(int *)(c + 0x5010) = -1;
        *(int *)(c + 0x5014) = 0;
    }
}
