//cpp
/* Slot 1. While the BASE's unk_031 flag is set, picks a warning-sprite
 * index from this class's countdown (mRespawnTimer) and draws it under the main
 * sprite; the main sprite is always drawn. unk_031 belongs to the base
 * class, not this one -- reached with the same explicit cast the old C
 * used, see cMgSmartball_ana_c.h. */
#include "cMgSmartball_ana_c.h"

extern "C" void func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern "C" int data_ov006_02138318[];

void cMgSmartball_ana_c::Update()
{
    if (unk_031 == 1) {
        int v = mRespawnTimer;
        int idx;
        if (v < 0x14) idx = 2;
        else if (v < 0x28) idx = 1;
        else idx = 0;
        if (idx >= 1) {
            func_ov004_020afdd0(data_ov006_02138318[idx], mCurrent0 >> 0xc, mCurrent1 >> 0xc, -1, 2);
        }
    }
    func_ov004_020afdd0(data_ov006_02138318[0], mCurrent0 >> 0xc, mCurrent1 >> 0xc, -1, 3);
}
