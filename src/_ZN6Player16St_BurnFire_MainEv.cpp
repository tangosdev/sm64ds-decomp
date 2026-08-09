//cpp
// @symbol _ZN6Player16St_BurnFire_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Player burn-in-fire state: early-exits via ChangeState on a couple of
 * flag checks, then (unless blocked by a hazard-level check) spawns two
 * fire/smoke particles plus a burn sound at the player's position offset
 * upward, then runs a small state machine (0/1) driving the burn anim and
 * a homing turn+speed approach before handing off to the common per-frame
 * tail function.
 */
#include "Player.h"

extern char data_ov002_0211013c;
extern s16 data_02082214[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *st);
extern void func_02012694(unsigned int id, const void *v);
extern int func_ov002_020d91e0(void *thiz, int damage, int doPre);
extern void func_ov002_020c5dec(void *c, int r1);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int sys, unsigned int id, int x, int y, int z, const s16 *vec, void *cb);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int handle, unsigned int id, unsigned int vol, const void *vec, unsigned int flags);
extern int _ZN6Player6IsAnimEj(void *c, unsigned int a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, unsigned int anim, int a, int b, unsigned int d);
extern int _ZNK6Player14GetBodyModelIDEjb(void *c, unsigned int a, int b);
extern void ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
extern void _Z14ApproachLinearRiii(int *cur, int target, int step);
extern void Player_AdvanceAnims(void *c);
}

int Player::St_BurnFire_Main()
{
    int speed = 0;

    if (mStateWork >= 0xa0 || mIsMetal != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211013c);
        return 1;
    }

    if (mIsInShallowWater != 0) {
        func_02012694(3, &mCamSpacePosX);
        _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211013c);
        return 1;
    }

    {
        u16 lvl = mStateTimer;
        if (lvl == 0x50 || lvl == 0x32 || lvl == 0x14) {
            if (func_ov002_020d91e0(this, 0x100, 1) != 0) {
                func_ov002_020c5dec(this, 2);
                return 1;
            }
        }
    }

    *(u8 *)(((long long)(int)&mStateWork) & 0xFFFFFFFFFFFFFFFFLL) += 2;

    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = mPosZ - 0x1e000;
        int yy = mPosY + 0x32000;
        int xx = mPosX;

        info.pos[0] = xx;
        info.pos[1] = yy;
        info.pos[2] = zz;
        info.vec[0] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2 + 1];

        unk_628 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            unk_628, 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        mParticle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticle2, 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        mLoopingSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            mLoopingSoundHandle, 0, 0x106, &mCamSpacePosX, 0);
    }

    switch (mStateStep) {
    case 0:
        if (_ZN6Player6IsAnimEj(this, 0x53) != 0) {
            /* The raw offset here is deliberate, not un-recovered. The
               matching increment above is byte-identical when written as
               &mStateWork; this one is not. Its add happens INSIDE the
               integer cast -- (int)this + 0x6e5, rather than
               (int)((char*)this + 0x6e5) -- and naming the field costs 4
               bytes. Both spellings were built and compared. */
            *(u8 *)(((long long)((int)this + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
        }
        if (mIsAirborne == 0) {
            mStateStep = 1;
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x3f, 0, 0x1000, 0);
            {
                char *anim = (char *)(((long long)(int)(*(char **)((char *)&mBodyModels + (_ZNK6Player14GetBodyModelIDEjb(this, param1 & 0xff, 0) << 2)) + 0x50)) & 0xffffffffffffffffLL);
                *(int *)(anim + 0xc) = 0x4000;
            }
        }
        break;
    case 1:
        {
            int idx = data_020a0e40 * 0x18;
            if ((*(u16 *)((char *)data_0209f49e + idx) & 2) != 0) {
                mStateStep = 0;
                _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x53, 0, 0x1000, 0);
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                mVertSpeed = 0x1e000;
                break;
            }
        }
        if (mIsAirborne != 0) {
            mStateStep = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x54, 0, 0x1000, 0);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            break;
        }
        _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x3f, 0, 0x1000, 0);
        {
            char *anim = (char *)(((long long)(int)(*(char **)((char *)&mBodyModels + (_ZNK6Player14GetBodyModelIDEjb(this, param1 & 0xff, 0) << 2)) + 0x50)) & 0xffffffffffffffffLL);
            *(int *)(anim + 0xc) = 0x4000;
        }
        {
            int idx = data_020a0e40 * 0x18;
            speed = 0x28000;
            if (*(s16 *)((char *)data_0209f4a0 + idx) != 0) {
                ApproachAngle(&mAngleY, mDesiredAngleY, 8, 0x4000, 0x10);
            }
            mPrevAngleY = mAngleY;
        }
        break;
    }

    _Z14ApproachLinearRiii(&mHorzSpeed, speed, 0x1000);
    Player_AdvanceAnims(this);
    return 1;
}
