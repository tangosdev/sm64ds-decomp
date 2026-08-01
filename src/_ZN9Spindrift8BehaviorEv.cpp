//cpp
// @symbol _ZN9Spindrift8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Enemy.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Spindrift.h"
extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void *, void *, void *, unsigned);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *, void *);
extern void _ZN12CylinderClsn5ClearEv(void *);
extern void _ZN12CylinderClsn6UpdateEv(void *);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void *, void *);
extern void _Z14ApproachLinearRiii(void *, int, int);
extern void *_ZN5Actor13ClosestPlayerEv(void *);
extern int Vec3_HorzDist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern void _Z14ApproachLinearRsss(void *, int, int);
extern void _ZN9Animation7AdvanceEv(void *);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *, void *);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *, void *, unsigned);
extern int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(void *, void *, int, short, int, int, int);
/* sig: (this, WithMeshClsn&, Fix12, short, bool, bool, Fix12) */
}

int Spindrift::Behavior()
{
    int r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(((char *)this), ((char *)this) + 0x1d0, ((char *)this) + 0x110, 3);
    if (r != 0) {
        if (r == 2)
            func_ov081_021237ec(((char *)this));
        return 1;
    }

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((char *)this), ((char *)this) + 0x1d0) != 0) {
        if (_ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(((char *)this), ((char *)this) + 0x19c) != 0)
            func_ov081_021237ec(((char *)this));
        func_ov081_02123b20(((char *)this));
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        if (unk_107 != 0 && unk_104 == 0) {
            _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
        }
        return 1;
    }

    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char *)this), ((char *)this) + 0x19c);
    func_ov081_02123910(((char *)this));

    switch (unk_39a) {
    case 0: {
            _Z14ApproachLinearRiii(((char *)this) + 0x98, 0x4000, 0x1000);
            void *cp = _ZN5Actor13ClosestPlayerEv(((char *)this));
            if (cp != 0) {
                int *src = (int *)(((int)cp + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
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
        *(unsigned short *)(((int)((char *)this) + 0x100) & 0xFFFFFFFFFFFFFFFFull) += 1;
        if (*(unsigned short *)(((int)((char *)this) + 0x100) & 0xFFFFFFFFFFFFFFFFull) >= 0x14)
            unk_39a = 0;
        break;
    }

    _ZN9Animation7AdvanceEv((char *)&mAnimation);
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char *)this), 0);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char *)this), ((char *)this) + 0x1d0, 0);

    if (_ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(((char *)this), ((char *)this) + 0x1d0, 0x3c000, 0x2888, 1, 1, 0x32000) != 0) {
        mPosX = unk_068;
        mPosY = unk_06c;
        mPosZ = unk_070;
    }
    func_ov081_02123b20(((char *)this));
    _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
    return 1;
}
