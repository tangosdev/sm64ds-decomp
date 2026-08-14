//cpp
/* Slot 1. Draws the pipe as five stacked sprites -- one cap, three body
 * segments, one base -- from a three-entry sprite table, all at the same x
 * with y stepping 0x10 units apart in screen space (the >> 12 is the fixed
 * point conversion). The last argument is a flag lifted from the scene. */
#include "cMgSmartball_dokan_c.h"

extern "C" void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern "C" int *data_ov006_0213ec04[];

void cMgSmartball_dokan_c::Update()
{
    int flag = 0;
    if (*(int *)((char *)unk_004 + 0x4660))
        flag = 1;
    func_ov004_020afdd0(data_ov006_0213ec04[0], mCurrent0 >> 12, (mCurrent1 >> 12) - 0x40, -1, flag);
    func_ov004_020afdd0(data_ov006_0213ec04[1], mCurrent0 >> 12, (mCurrent1 >> 12) - 0x30, -1, flag);
    func_ov004_020afdd0(data_ov006_0213ec04[1], mCurrent0 >> 12, mCurrent1 >> 12, -1, flag);
    func_ov004_020afdd0(data_ov006_0213ec04[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 0x10, -1, flag);
    func_ov004_020afdd0(data_ov006_0213ec04[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 0x20, -1, flag);
}
