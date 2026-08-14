//cpp
/* Slot 1. While unk_040 is 0 or 1, draws two extra base sprites under the
 * main pair; then four OAM::Render calls draw a mirrored quad using the
 * eased angle at offset 0x32 (base tail padding -- see the header) to
 * spread each half away from vertical. */
#include "cMgSmartball_wing_c.h"
#include "OamAttr.h"

extern "C" void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern "C" void *data_ov006_02137994[];
extern "C" struct OamAttr *data_ov006_02137684[];

extern "C" void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int sub, struct OamAttr *data,
    s32 x, s32 y,
    s32 palette, s32 priority,
    Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);

void cMgSmartball_wing_c::Update()
{
    if (unk_040 <= 1) {
        func_ov004_020afdd0(data_ov006_02137994[0],
            mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_02137994[1],
            mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2);
    }
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[0],
        (mCurrent0 >> 12) - 0x10, (mCurrent1 >> 12) + 0x118,
        -1, 1, 0x1000, 0x1000,
        (unsigned short)(short)(-0x4000 - *(short *)((char *)this + 0x32)), -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[0],
        (mCurrent0 >> 12) + 0x10, (mCurrent1 >> 12) + 0x118,
        -1, 1, 0x1000, 0x1000,
        (unsigned short)(short)(*(short *)((char *)this + 0x32) - 0x4000), -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[1],
        (mCurrent0 >> 12) - 0x10, (mCurrent1 >> 12) + 0x11d,
        -1, 2, 0x1000, 0x1000,
        (unsigned short)(short)(-0x4000 - *(short *)((char *)this + 0x32)), -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[1],
        (mCurrent0 >> 12) + 0x10, (mCurrent1 >> 12) + 0x11d,
        -1, 2, 0x1000, 0x1000,
        (unsigned short)(short)(*(short *)((char *)this + 0x32) - 0x4000), -1);
}
