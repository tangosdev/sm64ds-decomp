//cpp
// @symbol _ZN13InvisiblePole13InitResourcesEv
/* Vtable slot 0. param1's low byte carries the pole's height in units of ten, and
 * bit 0x100 selects a second collision behaviour. */
#include "InvisiblePole.h"

extern "C" {
/* Both carry Fix12<int> parameters, so their definitions stay mangled free
   functions -- see include/ShadowModel.h. A call is unaffected. */
void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(Actor *self, int a, int b, int c, int d);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        MovingCylinderClsn *self, Actor *actor, int radius, int height, u32 e, u32 f);
}

s32 InvisiblePole::InitResources()
{
    int height = (((param1 & 0xff) - 0xa) * 0xa) << 0xc;
    int half;
    if (height <= 0) height = 0x1000;
    half = height >> 1;
    _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(this, half, half, half + 0x640000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        &mClsn, this, 0x35555, height,
        (param1 & 0x100) ? 0x280000c : 0x80000c, 0);
    return 1;
}
