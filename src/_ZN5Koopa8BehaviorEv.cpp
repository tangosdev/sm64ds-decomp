//cpp
#include "types.h"
// @symbol _ZN5Koopa8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Enemy.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Koopa.h"
extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void *self, void *wm, void *ma, unsigned int j);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void *self, void *c);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *c);
extern void _ZN5Actor8PoofDustEv(void *self);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void _ZN12CylinderClsn6UpdateEv(void *c);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
extern int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);
extern void _ZN9Animation7AdvanceEv(void *a);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
extern int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(void *self, void *wm, int a, s16 b, int c, int d, int e);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, unsigned int j);
extern void _ZN5Enemy11UpdateDeathER12WithMeshClsn(void *self, void *wm);
}

int Koopa::Behavior()
{
    int state;
    int kind;

    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
            ((char *)this), ((char *)this) + 0x144, ((char *)this) + 0x300, 3) != 0)
        return 1;

    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char *)this), ((char *)this) + 0x110);

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((char *)this), ((char *)this) + 0x144) != 0) {
        int *pb0 = (int *)(((long long)(int)((char *)&unk_0b0)));
        *pb0 = *pb0 & ~0x10000000;
        if (_ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(((char *)this), ((char *)this) + 0x110) != 0) {
            _ZN5Actor8PoofDustEv(((char *)this));
            func_ov062_021179e4(((char *)this));
            _ZN5Actor24KillAndTrackInDeathTableEv(((char *)this));
        }
        if (unk_107 != 0)
            func_ov062_02117570(((char *)this));
        func_ov062_02118334(((char *)this));
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        if (unk_107 != 0 && unk_104 == 0)
            _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
        if (mKoopaVariant == 1)
            mState = 4;
        else
            mState = 1;
        if (_ZNK12WithMeshClsn10IsOnGroundEv((char *)&mWithMeshClsn) != 0) {
            unk_3a8 = mPosX;
            unk_3ac = mPosY;
            unk_3b0 = mPosZ;
        }
        return 1;
    }

    if (unk_10c == 0) {
        if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), 0x5dc000) != 0)
            return 1;

        {
            int *pb0 = (int *)((char *)&unk_0b0);
            if (unk_3ce != 0)
                *(u8 *)(((long long)(int)((char *)&unk_3ce))) -= 1;
            *pb0 = *pb0 | 0x10000000;
        }
        func_ov062_02118258(((char *)this), 0x3e8000);

        if (mState != 0)
            _ZN9Animation7AdvanceEv((char *)&mAnimation);

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
                u16 *p100 = (u16 *)(((long long)(int)((char *)&unk_100)));
                *p100 = (u16)(*p100 + 1);
            }
        }
        if (state != mState || kind != mKoopaVariant) {
            unk_100 = 0;
            unk_3c4 = 0;
        }
        func_ov062_02117c98(((char *)this));
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char *)this), ((char *)this) + 0x110);

        if (unk_10c == 0 && mState != 0) {
            if (_ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(
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

        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char *)this), ((char *)this) + 0x144, 0);
        func_ov062_02117570(((char *)this));
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        if (unk_10c == 0) {
            if (unk_3ca == 0) {
                _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
            } else {
                *(u16 *)(((long long)(int)((char *)&unk_3ca))) -= 1;
            }
        }
    } else {
        _ZN5Enemy11UpdateDeathER12WithMeshClsn(((char *)this), ((char *)this) + 0x144);
    }

    func_ov062_02118334(((char *)this));
    return 1;
}
