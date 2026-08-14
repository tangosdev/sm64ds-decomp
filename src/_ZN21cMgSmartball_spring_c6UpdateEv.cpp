//cpp
/* Slot 1. Draws one of two sprite pairs depending on a byte flag read
 * through the base's unk_004 scene pointer (offset 8, low byte) -- unrelated
 * to this class's own 0x34+ fields, which Update never touches at all. */
#include "cMgSmartball_spring_c.h"

extern "C" void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern "C" void *data_ov006_02138a20[];
extern "C" void *data_ov006_02138a38[];

void cMgSmartball_spring_c::Update()
{
    if ((*(int *)((char *)unk_004 + 8) & 0xff) == 0) {
        func_ov004_020afdd0(data_ov006_02138a20[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_02138a20[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2);
    } else {
        func_ov004_020afdd0(data_ov006_02138a38[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_02138a38[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2);
    }
}
