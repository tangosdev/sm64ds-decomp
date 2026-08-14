//cpp
/* Slot 1. Switches on unk_034 (see the header): state 0 draws two base
 * sprites via func_ov004_020afdd0; state 1 computes an ease vector from the
 * base's unk_028 (bails out once unk_028 < 0x1000) and draws two mirrored
 * sprites via RenderOamBothScreens; there is no case for state 2, so
 * nothing renders. */
#include "cMgSmartball_kinoko_c.h"

extern "C" short data_02082214[];
extern "C" void *data_ov006_0213765c[];
extern "C" void *data_ov006_02137674[];
extern "C" void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern "C" void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern "C" int _ZN4cstd4fdivEii(int, int);

void cMgSmartball_kinoko_c::Update()
{
    switch (unk_034) {
    case 0:
        func_ov004_020afdd0(data_ov006_0213765c[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_0213765c[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2);
        break;
    case 1:
        {
            int d;
            int b, a;
            int vec[4];
            if (unk_028 < 0x1000) return;
            d = _ZN4cstd4fdivEii(0x7000, unk_028);
            b = (int)(((long long)data_02082214[1] * d + 0x800) >> 12);
            a = (int)(((long long)data_02082214[0] * d + 0x800) >> 12);
            vec[0] = b;
            vec[1] = a;
            vec[2] = -a;
            vec[3] = b;
            RenderOamBothScreens(data_ov006_02137674[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1, vec);
            RenderOamBothScreens(data_ov006_02137674[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2, vec);
        }
        break;
    }
}
