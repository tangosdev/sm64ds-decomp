//cpp
extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void *, void *, void *, unsigned);
extern void func_ov081_021237ec(void *);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *, void *);
extern int _ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(void *, void *);
extern void func_ov081_02123b20(void *);
extern void _ZN12CylinderClsn5ClearEv(void *);
extern void _ZN12CylinderClsn6UpdateEv(void *);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void *, void *);
extern void func_ov081_02123910(void *);
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

int _ZN9Spindrift8BehaviorEv(char *c)
{
    int r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c + 0x1d0, c + 0x110, 3);
    if (r != 0) {
        if (r == 2)
            func_ov081_021237ec(c);
        return 1;
    }

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x1d0) != 0) {
        if (_ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(c, c + 0x19c) != 0)
            func_ov081_021237ec(c);
        func_ov081_02123b20(c);
        _ZN12CylinderClsn5ClearEv(c + 0x19c);
        if (*(unsigned char *)(c + 0x107) != 0 && *(unsigned short *)(c + 0x104) == 0) {
            _ZN12CylinderClsn6UpdateEv(c + 0x19c);
        }
        return 1;
    }

    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x19c);
    func_ov081_02123910(c);

    switch (*(unsigned char *)(c + 0x39a)) {
    case 0: {
            _Z14ApproachLinearRiii(c + 0x98, 0x4000, 0x1000);
            void *cp = _ZN5Actor13ClosestPlayerEv(c);
            if (cp != 0) {
                int *src = (int *)(((int)cp + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
                int v3[3];
                v3[0] = src[0];
                v3[1] = src[1];
                v3[2] = src[2];
                if (Vec3_HorzDist(c + 0x38c, (void *)v3) > 0x3e8000) {
                    *(short *)(c + 0x398) = Vec3_HorzAngle(c + 0x5c, c + 0x38c);
                } else if (Vec3_HorzDist(c + 0x5c, (void *)v3) > 0x12c000) {
                    *(short *)(c + 0x398) = Vec3_HorzAngle(c + 0x5c, (void *)v3);
                }
                goto after_st0;
            }
            *(short *)(c + 0x398) = Vec3_HorzAngle(c + 0x5c, c + 0x38c);
        after_st0:
            _Z14ApproachLinearRsss(c + 0x8e, *(short *)(c + 0x398), 0x200);
            *(short *)(c + 0x94) = *(short *)(c + 0x8e);
        }
        break;
    case 1:
        *(unsigned short *)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFFull) += 1;
        if (*(unsigned short *)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFFull) >= 0x14)
            *(unsigned char *)(c + 0x39a) = 0;
        break;
    }

    _ZN9Animation7AdvanceEv(c + 0x160);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x1d0, 0);

    if (_ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(c, c + 0x1d0, 0x3c000, 0x2888, 1, 1, 0x32000) != 0) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
    }
    func_ov081_02123b20(c);
    _ZN12CylinderClsn5ClearEv(c + 0x19c);
    _ZN12CylinderClsn6UpdateEv(c + 0x19c);
    return 1;
}
}
