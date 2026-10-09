//cpp
/* Castle grounds birds -- ov009/daSBird_c, head of the class TU.
 *
 * The class linker run is 0x021111a0..0x02111a70, split by the
 * func_ov009_0211145c hatch at 0x0211145c. This file owns
 * 0x02111224..0x0211145c; the upper half lives in d_a_s_bird.cpp.
 *
 * D1/D0 stay enrolled as their own files (src/named/ov009/_ZN9daSBird_cD1Ev.cpp /
 * D0Ev.cpp) -- the destructor is out of line and those files emit the
 * class vtable and RTTI. This TU does not.
 *
 * Both functions are daSBird_c members: 02111234 is ordinal 3 of the
 * data_ov009_02113c48 PMF table (ROM record at 0x02113914), and 02111224
 * is the follower-attach the two spawn loops call on the spawned bird.
 * Helper names are not recovered; the members keep their addresses (S33).
 *
 * deslop leftovers:
 * - cstd::atan2 6az (Fix12<int> by value): mangled extern.
 * - Vec3_Sub / Vec3_HorzLen / LenVec3 / ApproachLinear: no shared header
 *   this TU can take without a campaign.
 * - data_02082214 sine table stays an extern s16[].
 * - &mPosX is a Vector3 pun for Vec3_Sub (same dActor_c campaign as the
 *   upper half).
 */

#include "daSBird_c.h"
#include "common.h"

bool ApproachLinear(short &value, short target, short step);

extern "C" {
extern s16 data_02082214[];
extern void Vec3_Sub(void *out, void *a, void *b);
extern s32 Vec3_HorzLen(void *v);
extern s32 LenVec3(void *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
}

/* Fly state -- data_ov009_02113c48[3]. Steers toward the leader's position
   (or the target point when leading) and derives forward/vertical speed.
   Defined first so the object emits in ROM order (02111224 < 02111234). */
// @symbol _ZN9daSBird_c19func_ov009_02111234Ev
void daSBird_c::func_ov009_02111234()
{
    UpdatePos(0);
    dActor_c *f = dActor_c::FindWithID(mOwnerID);
    if (f == 0 || f->mPosY > 0xbb8000) {
        MarkForDestruction();
        return;
    }

    Vector3 diff;
    if (mIsLeader) {
        Vector3 d1;
        Vec3_Sub(&d1, &mTargetPos, (Vector3 *)&mPosX);
        diff.x = d1.x;
        diff.y = d1.y;
        diff.z = d1.z;
        s32 hl = Vec3_HorzLen(&diff);
        mTargetPitch = _ZN4cstd5atan2E5Fix12IiES1_(hl, mPosY + 0xfd8f0000);
        mTargetYaw = _ZN4cstd5atan2E5Fix12IiES1_(diff.z, diff.x);
    } else {
        Vector3 d2;
        Vec3_Sub(&d2, (Vector3 *)&f->mPosX, (Vector3 *)&mPosX);
        diff.x = d2.x;
        diff.y = d2.y;
        diff.z = d2.z;
        s32 hl = Vec3_HorzLen(&diff);
        mTargetPitch = _ZN4cstd5atan2E5Fix12IiES1_(hl, -diff.y);
        mTargetYaw = _ZN4cstd5atan2E5Fix12IiES1_(diff.z, diff.x);
        mFlySpeed = LenVec3(&diff) / 25 + 0x14000;
    }

    ApproachLinear(mPrevAngleX, mTargetPitch, 0x8c);
    ApproachLinear(mPrevAngleY, mTargetYaw, 0x320);

    s32 hi = 0x3000;
    s32 dd = (s16)(mPrevAngleY - mTargetYaw);
    if (dd < -hi)
        dd = -hi;
    else if (dd > hi)
        dd = hi;
    mBank = (s16)dd;
    ApproachLinear(mAngleZ, mBank, 0x258);

    {
        u16 a = *(u16 *)&mPrevAngleX;
        s32 t = mFlySpeed;
        s16 s = data_02082214[((a >> 4) << 1) + 1];
        mHorzSpeed = (s32)(((s64)t * s + 0x800) >> 12);
    }
    {
        u16 a = *(u16 *)&mPrevAngleX;
        s32 t = mFlySpeed;
        s16 s = data_02082214[(a >> 4) << 1];
        mVertSpeed = (s32)(((s64)t * s + 0x800) >> 12);
    }
}

/* Follower attach -- spawn loops store the leader's uniqueID. */
// @symbol _ZN9daSBird_c19func_ov009_02111224Ei
void daSBird_c::func_ov009_02111224(int ownerID)
{
    mIsLeader = 0;
    mOwnerID = ownerID;
}
