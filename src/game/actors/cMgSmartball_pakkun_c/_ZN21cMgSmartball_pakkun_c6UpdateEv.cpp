//cpp
/* Slot 1. While mActionTimer is counting down it renders a rotate/scale pair
 * of frames (the shift-by-12 fixed-point matrix built from data_02082214);
 * otherwise it renders one of two idle frames selected by unk_031, plus a
 * common third frame beneath it. */
#include "cMgSmartball_pakkun_c.h"

extern "C" int _ZN4cstd4fdivEii(int, int);
extern "C" short data_02082214[];
extern "C" void *data_ov006_02137630[];
extern "C" void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern "C" void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);

void cMgSmartball_pakkun_c::Update()
{
    if (mIsActive == 0) return;
    {
        int f38 = mActionTimer;
        if (f38 > 0) {
            int scale;
            int idx = 1;
            int b, a;
            struct { int _00, _01, _10, _11; } vec;
            if (f38 < 0x20) {
                scale = _ZN4cstd4fdivEii(0x20000, f38 << 12);
            } else {
                scale = 0x1000;
                if ((f38 - 0x20) % 16 < 4) idx = 0;
            }
            b = (int)((((long long)data_02082214[1] * scale) + 0x800) >> 12);
            a = (int)((((long long)data_02082214[0] * scale) + 0x800) >> 12);
            vec._00 = b;
            vec._01 = a;
            vec._10 = -a;
            vec._11 = b;
            RenderOamBothScreens(data_ov006_02137630[idx], mCurrent0 >> 12, mCurrent1 >> 12, -1, 0, &vec);
            RenderOamBothScreens(data_ov006_02137630[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2, &vec);
        } else {
            if (unk_031 != 0) {
                func_ov004_020afdd0(data_ov006_02137630[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 0);
                func_ov004_020afdd0(data_ov006_02137630[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2);
            } else {
                func_ov004_020afdd0(data_ov006_02137630[1], mCurrent0 >> 12, mCurrent1 >> 12, -1, 0);
                func_ov004_020afdd0(data_ov006_02137630[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2);
            }
        }
    }
}
