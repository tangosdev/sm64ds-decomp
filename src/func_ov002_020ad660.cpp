//cpp
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *o);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *o);
extern void _ZN5Enemy9SpawnCoinEv(void *o);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *o);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *o, void *cc);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *o, void *w, unsigned int j);
extern void Vec3_Asr(void *d, const void *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
extern char data_020a0e68;
}

struct VB {
    virtual void d00();
    virtual void d01();
    virtual void d02();
    virtual void d03();
    virtual void d04();
    virtual void d05();
    virtual void d06();
    virtual void d07();
    virtual void d08();
    virtual void d09();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void d18();
    virtual void d19();
    virtual void d20();
    virtual void d21();
    virtual void d22();
    virtual void d23();
    virtual void d24();
    virtual void d25();
    virtual void d26();
    virtual void d27();
    virtual void d28();
    virtual int m29();
};

struct M48 { int w[12]; };

#define LAUNDER(p) ((int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

extern "C" int func_ov002_020ad660(void *cc, void *pp, void *r5p, int flags)
{
    char *c = (char *)cc;
    char *p = (char *)pp;
    char *r5 = (char *)r5p;
    int v[3];

    if (*(int *)(c + 0x10c) != 8)
        return 0;

    if (*(unsigned short *)(c + 0x102) == 0 ||
        (p != 0 && (_ZNK12WithMeshClsn10IsOnGroundEv(p) || _ZNK12WithMeshClsn8IsOnWallEv(p)))) {
        if (flags & 1)
            _ZN5Enemy9SpawnCoinEv(c);
        if (flags & 2)
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        *(int *)(c + 0x10c) = 0;
        return 2;
    }

    *(unsigned short *)LAUNDER(c + 0x102) -= 1;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (p != 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, p, 0);

    if (r5 != 0) {
        *(short *)LAUNDER(c + 0x8c) += *(short *)(c + 0xec);
        *(short *)LAUNDER(c + 0x8e) += *(short *)(c + 0xee);
        *(short *)LAUNDER(c + 0x90) += *(short *)(c + 0xf0);
        Vec3_Asr(v, c + 0x5c, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            ((VB *)(void *)c)->m29() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
            *(short *)(c + 0x8c), *(short *)(c + 0x8e), *(short *)(c + 0x90));
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            (-((VB *)(void *)c)->m29()) >> 3, 0);
        *(M48 *)(r5 + 0x1c) = *(M48 *)&data_020a0e68;
    }
    return 1;
}
