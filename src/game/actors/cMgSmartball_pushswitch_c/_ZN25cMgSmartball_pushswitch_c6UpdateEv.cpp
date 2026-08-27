//cpp
/* Slot 1. Draws two OAM sprites: the switch itself, selected by the base's
 * unk_031 flag (0/1 used directly as the table index), and a fixed second
 * frame beneath it. */
#include "cMgSmartball_pushswitch_c.h"

extern "C" void RenderOamBothScreens(int a, int b, int c, int d, int e, int f);
extern "C" int data_ov006_02137a9c[];

void cMgSmartball_pushswitch_c::Update()
{
    RenderOamBothScreens(data_ov006_02137a9c[unk_031],
                        mCurrent0 >> 12,
                        mCurrent1 >> 12,
                        -1, 1, 0);
    RenderOamBothScreens(data_ov006_02137a9c[2],
                        mCurrent0 >> 12,
                        (mCurrent1 >> 12) + 5,
                        -1, 2, 0);
}
