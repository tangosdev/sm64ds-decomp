//cpp
#include "types.h"
// @symbol _ZN5Koopa8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Enemy.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Koopa.h"
extern "C" {
extern int _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(void *self, void *wm, void *ma, unsigned int j);
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void *self, void *c);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *self, void *c);
extern void _ZN8dActor_c8PoofDustEv(void *self);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern void _ZN5dCc_c5ClearEv(void *c);
extern void _ZN5dCc_c6UpdateEv(void *c);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *c);
extern int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(void *self, void *wm, int a, s16 b, int c, int d, int e);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, void *wm, unsigned int j);
extern void _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(void *self, void *wm);
}

int Koopa::Behavior()
{
    int state;
    int kind;

    if (_ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(
            ((char *)this), ((char *)this) + 0x144, ((char *)this) + 0x300, 3) != 0)
        return 1;

    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char *)this), ((char *)this) + 0x110);

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char *)this), ((char *)this) + 0x144) != 0) {
        int *pb0 = (int *)((char *)&mFlags);
        *pb0 = *pb0 & ~0x10000000;
        if (_ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(((char *)this), ((char *)this) + 0x110) != 0) {
            _ZN8dActor_c8PoofDustEv(((char *)this));
            func_ov062_021179e4(((char *)this));
            _ZN8dActor_c24KillAndTrackInDeathTableEv(((char *)this));
        }
        if (mEatenByYoshi != 0)
            func_ov062_02117570(((char *)this));
        func_ov062_02118334(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        if (mEatenByYoshi != 0 && unk_104 == 0)
            _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
        if (mKoopaVariant == 1)
            mState = 4;
        else
            mState = 1;
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn) != 0) {
            unk_3a8 = mPosX;
            unk_3ac = mPosY;
            unk_3b0 = mPosZ;
        }
        return 1;
    }

    if (mDeathState == 0) {
        if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), 0x5dc000) != 0)
            return 1;

        {
            int *pb0 = (int *)((char *)&mFlags);
            if (mLandingDustTimer != 0)
                *(u8 *)((char *)&mLandingDustTimer) -= 1;
            *pb0 = *pb0 | 0x10000000;
        }
        func_ov062_02118258(((char *)this), 0x3e8000);

        if (mState != 0)
            mModelAnim.Advance();

        kind = mKoopaVariant;
        state = mState;
        switch (kind) {
        case 0:
        case 2:
            func_ov062_02117acc(((char *)this));
            break;
        case 1:
            func_ov062_02117a3c(((char *)this));
            break;
        }

        {
            int ang = mPrevAngleY;
            mAngleY = (s16)ang;
            {
                u16 *p100 = (u16 *)((char *)&mStateTimer);
                *p100 = (u16)(*p100 + 1);
            }
        }
        if (state != mState || kind != mKoopaVariant) {
            mStateTimer = 0;
            mWalkState = 0;
        }
        func_ov062_02117c98(((char *)this));
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), ((char *)this) + 0x110);

        if (mDeathState == 0 && mState != 0) {
            if (_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(
                    ((char *)this), ((char *)this) + 0x144, 0x32000, 0x3800, 0, 1, 0x32000) != 0) {
                mPosX = unk_3a8;
                mPosY = unk_3ac;
                mPosZ = unk_3b0;
            } else {
                unk_3a8 = mPosX;
                unk_3ac = mPosY;
                unk_3b0 = mPosZ;
            }
        }

        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), ((char *)this) + 0x144, 0);
        func_ov062_02117570(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        if (mDeathState == 0) {
            if (mInvincibleTimer == 0) {
                _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
            } else {
                *(u16 *)((char *)&mInvincibleTimer) -= 1;
            }
        }
    } else {
        _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(((char *)this), ((char *)this) + 0x144);
    }

    func_ov062_02118334(((char *)this));
    return 1;
}
