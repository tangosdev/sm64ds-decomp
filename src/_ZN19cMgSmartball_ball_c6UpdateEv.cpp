//cpp
/* Slot 1. Picks one of two sprites (based on which edge of the level the
 * scene reports) and draws it at the ball's rounded position, with a
 * priority bumped from 1 to 3 while state3a is set. */
#include "cMgSmartball_ball_c.h"

extern "C" void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern "C" void *data_ov006_021382e0;
extern "C" void *data_ov006_021382f8;

void cMgSmartball_ball_c::Update()
{
    int r2;
    if (unk_030 == 0)
        return;
    if (mInPlay == 0)
        return;
    r2 = 1;
    if (state3a == 1)
        r2 = 3;
    if ((*(int *)((char *)unk_004 + 8) & 0xff) == 0) {
        int x = mCurrent0;
        int y = mCurrent1;
        func_ov004_020afdd0(*(void **)&data_ov006_021382e0, (x + 0x800) >> 12, (y + 0x800) >> 12, -1, r2);
    } else {
        int x = mCurrent0;
        int y = mCurrent1;
        func_ov004_020afdd0(*(void **)&data_ov006_021382f8, (x + 0x800) >> 12, (y + 0x800) >> 12, -1, r2);
    }
}
