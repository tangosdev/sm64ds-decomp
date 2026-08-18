//cpp
// @symbol _ZN17daObjFloatBoard_c8BehaviorEv
#include "daObjFloatBoard_c.h"
/* daObjFloatBoard_c::Behavior -- vtable slot 6, ov002 0x020b5c4c.
 *
 * Named from the table: 0x020b5c4c is the word slot 6 of
 * _ZTV17daObjFloatBoard_c holds where _ZTV10dBgActor_c holds something else, so
 * it is this class's own override of the virtual fBase_c declares.
 *
 * The board floats at mWaterY, sinks 0x28000 further while someone is standing
 * on it (mRider), bobs on top of that from a sine table walked by mBobPhase,
 * and tilts toward the rider -- val1/val2 are the pitch and roll it eases into
 * with ApproachLinear. mRiderTimeout is what lets go of the rider a few frames
 * after they step off.
 *
 * TWO SPELLINGS OF mBobPhase ON PURPOSE, both kept from the pre-migration
 * file: the store goes through `ctr` and the reload two lines later goes
 * through a separate `char *` base. Collapsing them to one member access lets
 * mwcc reuse the loaded value instead of reloading it, which the ROM does not.
 *
 * ApproachLinear and the two Fix12<int>-carrying calls are declared rather
 * than `extern`-ed under their mangled names: a bare `extern` on a mangled
 * identifier is emitted verbatim from C but mangled a SECOND time from C++,
 * which this file now is (see the note at the top of include/SharedFilePtr.h).
 * ApproachLinear's two overloads mangle back to the ROM's own
 * _Z14ApproachLinearRiii and _Z14ApproachLinearRsss; cstd::atan2 and
 * dBgActor_c::IsClsnInRangeOnScreen take Fix12<int> BY VALUE, so their
 * definitions stay mangled free functions -- see include/ShadowModel.h -- and
 * only the call site changes. */

int  ApproachLinear(int &p, int value, int speed);
void ApproachLinear(short &p, short value, short speed);

extern "C" {
extern short data_02082214[];

void func_020393a4(int *p, int v);
int  Vec3_HorzDist(const void *a, const void *b);
unsigned short DecIfAbove0_Short(unsigned short *p);
int  func_ov002_020b5ab4(daObjFloatBoard_c *self);
void func_ov002_020b5b98(daObjFloatBoard_c *self);

short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
int   _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(dBgActor_c *self, int x, int z);
}

int daObjFloatBoard_c::Behavior()
{
    Vector3 tpos;
    int dist;
    short atanRes;
    short angleDiff;
    int j;
    short s, co, tv;
    short val1, val2;

    func_020393a4((int *)&mMeshCollider, 0x300000);

    if (func_ov002_020b5ab4(this) != 0) {
        if (ApproachLinear(mSinkOffset, mRider != 0 ? -0x28000 : 0, 0x5000) != 0) {
            short *ctr = &mBobPhase;
            short cval = *ctr;
            char *st = (char *)this + 0x300;
            *ctr = (short)(cval + 0xa00);
            j = (*(unsigned short *)(st + 0x38) >> 4) * 2;
            tv = data_02082214[j];
            mBobOffset = 10 * tv;
        }

        if (mRider == 0) {
            val2 = 0;
            val1 = val2;
        } else {
            Vector3 *tp = (Vector3 *)((char *)mRider + 0x5c);
            tpos.x = tp->x;
            tpos.y = tp->y;
            tpos.z = tp->z;
            dist = Vec3_HorzDist(&mPosX, &tpos) >> 0xc;
            atanRes = _ZN4cstd5atan2E5Fix12IiES1_(mPosX - tpos.x, mPosZ - tpos.z);
            angleDiff = atanRes - mAngleY;
            j = ((u16)angleDiff >> 4) * 2;
            s = data_02082214[j];
            co = data_02082214[j + 1];
            val1 = (short)((-dist * co) / 2200);
            val2 = (short)((dist * s) / 1500);
        }

        ApproachLinear(mAngleX, val1, 0xc0);
        ApproachLinear(mAngleZ, val2, 0xc0);

        mPosY = mWaterY + mSinkOffset + mBobOffset;
    } else {
        mPosY = mWaterY;
        mAngleZ = 0;
        mAngleX = mAngleZ;
    }

    func_ov002_020b5b98(this);
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();

    if (DecIfAbove0_Short(&mRiderTimeout) == 0)
        mRider = 0;

    return 1;
}
