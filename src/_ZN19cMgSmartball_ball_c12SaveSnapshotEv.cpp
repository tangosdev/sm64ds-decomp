//cpp
/* Slot 0. Does NOT open with the base's SaveSnapshot body inline (unlike
 * every sibling so far) -- this is the one override in the family that
 * replaces the base behaviour instead of repeating it, gating the ball's
 * own countdown/zone/bounce/physics state machine every frame. */
#include "cMgSmartball_ball_c.h"

typedef struct V2 {
    int x;
    int z;
} V2;

extern "C" void Vec2_Sub(V2 *out, V2 *a, V2 *b);
extern "C" int Vec2_Len(V2 *p);
extern "C" int func_0203d434(V2 *p);
extern "C" void func_0203d630(V2 *p, int scale);
extern "C" void func_0203d480(V2 *out, V2 *in);
extern "C" int func_0203d524(V2 *a, V2 *b);
extern "C" int func_ov006_02114590(char *c, V2 *a, V2 *b, V2 *d, V2 *e);
extern "C" int func_ov006_02111dcc(char *c, int value);
extern "C" void func_ov006_02111e48(char *c);
extern "C" void func_ov006_021128fc(char *c);
extern "C" void func_ov006_02112ad8(char *c);

void cMgSmartball_ball_c::SaveSnapshot()
{
    V2 vc;
    V2 vd;
    V2 d1;
    V2 pa;
    V2 pb;
    V2 pc;
    V2 pd;
    V2 pe;
    V2 pf;
    V2 pg;
    V2 ph;
    V2 v5c;
    V2 d2;
    int t;
    int spd;
    int mag;
    int dist;

    if (soundTimer > 0)
        soundTimer -= 1;
    if (mIsActive == 0)
        return;
    if (mExpireTimer > 0) {
        mExpireTimer -= 1;
        if (mExpireTimer <= 0) {
            mIsActive = 0;
            return;
        }
    }
    if (mIsFrozen == 1) {
        mCurrent0 = mFrozenPos0;
        mCurrent1 = mFrozenPos1;
        mVel0 = 0;
        mVel1 = 0;
        return;
    }
    if (mCurrent0 >= 0x8000 && mCurrent0 < 0xd8000 &&
        mCurrent1 >= 0x74000 && mCurrent1 < 0x7c000) {
        if (mZoneDwell < 0x1e) {
            mZoneDwell += 1;
        } else if (mZoneDwell < 0x12c) {
            state3b = 1;
            mZoneDwell += 1;
        } else {
            state3b = 1;
            if (func_ov006_02111dcc((char *)this, 1) != 0) {
                func_ov006_02111e48((char *)this);
                return;
            }
        }
    } else {
        mZoneDwell = 0;
    }
    if (mIndex == *(int *)((char *)mpManager + 0x4664)) {
        if (mInPlay != 0)
            mIsWaiting = 0;
    }
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mCurrent0 < 0 || mCurrent0 >= 0x100000 ||
        mCurrent1 < -0xf0000 || mCurrent1 >= 0xc0000) {
        mStuckFrames += 1;
    } else {
        if ((*(int *)((char *)mpManager + 8) & 0xff) != 0) {
            Vec2_Sub(&d1, (V2 *)&mCurrent0, (V2 *)&mLastPos0);
            t = Vec2_Len(&d1) < 0x8000;
            if (t != 0) {
                mStuckFrames += 1;
                goto after2;
            }
        }
        mStuckFrames = 0;
        mLastPos0 = mCurrent0;
        mLastPos1 = mCurrent1;
    }
after2:
    if (mCollisionCooldown > 0)
        mCollisionCooldown -= 1;
    if (mZoneCooldown > 0)
        mZoneCooldown -= 1;
    if (state3a != 1) {
        if (mUpperWallSolid == 0) {
            if (mCurrent0 >= 0xe8000 && mCurrent0 < 0xf0000 &&
                mCurrent1 >= 0x78000 && mCurrent1 < 0xa8000) {
                if (mZoneCooldown <= 0) {
                    mUpperWallSolid = 1;
                    unk_039 = 0;
                    state3a = 0;
                }
            }
        }
    }
    if (state3a == 0) {
        pa.x = mCurrent0;
        pa.z = mCurrent1;
        pb.x = 0x8000;
        pb.z = 0x8c000;
        pc.x = 0x8000;
        pc.z = 0x92000;
        pd.x = 0xe0000;
        pd.z = 0x96000;
        if (func_ov006_02114590((char *)this, &pa, &pb, &pc, &pd) != 0)
            goto bounce;
        pe.x = mCurrent0;
        pe.z = mCurrent1;
        pf.x = 0x8000;
        pf.z = 0x92000;
        pg.x = 0xe0000;
        pg.z = 0x96000;
        ph.x = 0xe0000;
        ph.z = 0x9e000;
        if (func_ov006_02114590((char *)this, &pe, &pf, &pg, &ph) != 0) {
bounce:
            mVel0 += 0x80;
            if (mVel0 >= 0x2000)
                mVel0 = 0x2000;
            mVel1 += 0x80;
        } else {
            mVel1 += 0x100;
        }
    }
    if (mVel0 != 0 || mVel1 != 0) {
        spd = Vec2_Len((V2 *)&mVel0);
        if (spd >= 0x8000)
            spd = 0x8000;
        if (func_0203d434((V2 *)&mVel0) != 0)
            func_0203d630((V2 *)&mVel0, spd);
    }
    mCurrent0 += mVel0;
    mCurrent1 += mVel1;
    if (mIsWaiting == 1) {
        func_ov006_021128fc((char *)this);
    } else {
        func_ov006_02112ad8((char *)this);
        if (mExitGateOpen == 0) {
            vc.x = 0x20000;
            vc.z = -0x8000;
            func_0203d480(&v5c, &vc);
            {
                int dz = mCurrent1 + 0x80000;
                int dx = mCurrent0 - 0xd8000;
                vd.x = dx;
                vd.z = dz;
            }
            if (func_0203d524(&vc, &vd) < -mRadius)
                mExitGateOpen = 1;
        }
    }
    mag = Vec2_Len((V2 *)&mVel0);
    Vec2_Sub(&d2, (V2 *)&mCurrent0, (V2 *)&mSnapshot0);
    dist = Vec2_Len(&d2);
    if (specialHit != 0)
        return;
    if (mag < dist)
        return;
    if (mag < 0x800)
        return;
    if (func_0203d434((V2 *)&mVel0) == 0)
        return;
    func_0203d630((V2 *)&mVel0, dist);
}
