//cpp
// @symbol _ZN21SnowmanBreathParticleC1Ev
#include "SnowmanBreath.h"

extern "C" {
extern Vector3 data_ov027_02113d10;
/* Fix12<int> by-value arguments are kept in their measured raw ABI view: a
 * direct C++ definition of this imported signature hits the mwccarm 6az wall. */
void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
    dCcPos_c *, const Vector3 *, s32, s32, u32, u32);
}

SnowmanBreathParticle::SnowmanBreathParticle()
{
    mPos = data_ov027_02113d10;
    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
        &mCollider, &mPos, 0x32000, 0x28000, 0x800002, 0x800000);
    mTimer = 0;
}
