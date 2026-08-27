//cpp
// @symbol _ZN9Spindrift8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Enemy.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Spindrift.h"
extern "C" {
extern int _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(void *, void *, void *, unsigned);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *, void *);
extern void _ZN5dCc_c5ClearEv(void *);
extern void _ZN5dCc_c6UpdateEv(void *);
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void *, void *);
extern void _Z14ApproachLinearRiii(void *, int, int);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *);
extern int Vec3_HorzDist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern void _Z14ApproachLinearRsss(void *, int, int);
extern void _ZN9Animation7AdvanceEv(void *);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *, void *, unsigned);
extern int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(void *, void *, int, short, int, int, int);
/* sig: (this, dBgCh_Actr&, Fix12, short, bool, bool, Fix12) */
}

int Spindrift::Behavior()
{
    int r = _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(((char *)this), ((char *)this) + 0x1d0, ((char *)this) + 0x110, 3);
    if (r != 0) {
        if (r == 2)
            func_ov081_021237ec(((char *)this));
        return 1;
    }

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char *)this), ((char *)this) + 0x1d0) != 0) {
        if (_ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(((char *)this), ((char *)this) + 0x19c) != 0)
            func_ov081_021237ec(((char *)this));
        func_ov081_02123b20(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCcAc_c);
        if (mEatenByYoshi != 0 && unk_104 == 0) {
            _ZN5dCc_c6UpdateEv((char *)&mdCcAc_c);
        }
        return 1;
    }

    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char *)this), ((char *)this) + 0x19c);
    func_ov081_02123910(((char *)this));

    switch (unk_39a) {
    case 0: {
            _Z14ApproachLinearRiii(((char *)this) + 0x98, 0x4000, 0x1000);
            void *cp = _ZN8dActor_c13ClosestPlayerEv(((char *)this));
            if (cp != 0) {
                int *src = (int *)((int)cp + 0x5c);
                int v3[3];
                v3[0] = src[0];
                v3[1] = src[1];
                v3[2] = src[2];
                if (Vec3_HorzDist(((char *)this) + 0x38c, (void *)v3) > 0x3e8000) {
                    unk_398 = Vec3_HorzAngle(((char *)this) + 0x5c, ((char *)this) + 0x38c);
                } else if (Vec3_HorzDist(((char *)this) + 0x5c, (void *)v3) > 0x12c000) {
                    unk_398 = Vec3_HorzAngle(((char *)this) + 0x5c, (void *)v3);
                }
                goto after_st0;
            }
            unk_398 = Vec3_HorzAngle(((char *)this) + 0x5c, ((char *)this) + 0x38c);
        after_st0:
            _Z14ApproachLinearRsss(((char *)this) + 0x8e, unk_398, 0x200);
            mPrevAngleY = mAngleY;
        }
        break;
    case 1:
        *(unsigned short *)((int)((char *)this) + 0x100) += 1;
        if (*(unsigned short *)((int)((char *)this) + 0x100) >= 0x14)
            unk_39a = 0;
        break;
    }

    _ZN9Animation7AdvanceEv((char *)(Animation *)&mModelAnim);
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), 0);
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), ((char *)this) + 0x1d0, 0);

    if (_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(((char *)this), ((char *)this) + 0x1d0, 0x3c000, 0x2888, 1, 1, 0x32000) != 0) {
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
    }
    func_ov081_02123b20(((char *)this));
    _ZN5dCc_c5ClearEv((char *)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char *)&mdCcAc_c);
    return 1;
}
