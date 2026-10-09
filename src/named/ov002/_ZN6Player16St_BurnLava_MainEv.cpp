//cpp
// @symbol _ZN6Player16St_BurnLava_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Burn-in-lava, per frame: kills the player once outside battle levels, spawns
 * smoke/steam particles with the burn SFX, then runs the three-hop bounce
 * (halving horizontal speed each hop, hop height 0x14000 then 0xa000) until
 * the burnt animation finishes, and leaves via state change or death.
 */
#include "Player.h"
extern u8 data_ov002_0211117c;
extern s8 data_0209f2f8;              /* current level id */
extern s16 data_02082214[];           /* sin/cos pair table, indexed by angle >> 4 */
extern char data_ov002_0211013c;      /* the walk state */

extern "C" {
extern void KillPlayer(void);
/* Particle::System::New, Sound::PlayLong and SetAnim all take Fix12<int> by
   value -- the mwccarm 6az wall, runbook section 7 -- so they stay extern "C"
   with scalars in those slots. */
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int, unsigned int, int, int, int, s16 *, void *);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(int, unsigned int, unsigned int, void *, unsigned int);
extern void func_ov002_020e28d4(void *, int, int);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned int, int, int, unsigned int);
extern void func_ov002_020c5dec(void *, int);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *, char *);
extern void Player_AdvanceAnims(void *);
}

int Player::St_BurnLava_Main()
{
    if (data_ov002_0211117c == 0 && mStateArg == 0) {
        KillPlayer();
        mStateArg = 1;
        mIsBodyClsnEnabled = 0;
    }

    {
        s8 lv = data_0209f2f8;
        if (lv != 0x13 && lv != 0x31 && mVertSpeed >= 0) {
            /* Field order is the stack layout the original built, not a choice. */
            struct Info {
                s16 vec[3];
                int pos[3];
            } info;
            int zz = mPosZ;
            int yy = mPosY;
            int xx = mPosX;
            info.pos[0] = xx;
            info.pos[2] = zz - 0x1e000;
            info.pos[1] = yy;
            /* The u16 cast is the semantics, not a missing type fix. mAngleY
               really is signed -- AngleDiff takes it signed and mPrevAngleY is
               assigned from it directly -- but indexing this table wants the
               angle to WRAP, so the shift must be logical. Spelling it
               `mAngleY >> 4` costs 2 words and changes the meaning for
               negative angles. */
            info.vec[0] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2];
            info.vec[1] = 0;
            info.vec[2] = data_02082214[(*(u16 *)&mAngleY >> 4) * 2 + 1];
            mParticle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticle1, 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
            mParticle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticle2, 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
            mLoopingSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                mLoopingSoundHandle, 0, 0x106, &mCamSpacePosX, 0);
        }
    }

    if (mStateStep == 0) {
        func_ov002_020e28d4(this, 0x1000, 0x1000);
        if (mIsAirborne == 0) {
            mStateWork++;
            if (mStateWork >= 3) {
                _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x17, 0x40000000, 0x1000, 0);
                mHorzSpeed = 0;
                mStateStep++;
            } else {
                mHorzSpeed >>= 1;
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                if (mStateWork == 1)
                    mVertSpeed = 0x14000;
                else
                    mVertSpeed = 0xa000;
            }
        }
    } else {
        if (FinishedAnim() != 0) {
            mPrevAngleY = mAngleY;
            if (GetHealth() == 0)
                func_ov002_020c5dec(this, 2);
            else
                _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211013c);
        }
    }

    Player_AdvanceAnims(this);
    return 1;
}
