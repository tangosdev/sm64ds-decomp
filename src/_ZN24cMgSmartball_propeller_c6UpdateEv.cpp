//cpp
/* Slot 1. Looks up a rotation matrix from the base's angle field at 0x32,
 * reading it UNSIGNED (>> 4, lsr not asr) to index the sine table -- one of
 * the conflicting readings cMgSmartball_object_c.h records for that field's
 * contested width, so it stays a raw cast rather than the base's member
 * name. Draws two OAM sprites (front and back cap) at the base's position
 * (mCurrent0/mCurrent1) using that matrix; touches nothing in this class's
 * own 0x34+ region at all. */
#include "cMgSmartball_propeller_c.h"

extern "C" short data_02082214[];
extern "C" void* data_ov006_02138a08[];
extern "C" void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);

void cMgSmartball_propeller_c::Update()
{
    int b = (short)((((long long)data_02082214[(*(unsigned short*)((char*)this + 0x32) >> 4) * 2 + 1] << 12) + 0x800) >> 12);
    int a;
    int vec[4];

    vec[0] = b;
    a = (short)((((long long)data_02082214[(*(unsigned short*)((char*)this + 0x32) >> 4) * 2] << 12) + 0x800) >> 12);
    vec[3] = b;
    vec[1] = a;
    vec[2] = -a;

    RenderOamBothScreens(data_ov006_02138a08[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1, vec);
    RenderOamBothScreens(data_ov006_02138a08[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1, 2, vec);
}
