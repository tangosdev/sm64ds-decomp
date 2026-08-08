//cpp
extern "C" {
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void*, void*);
extern int func_020ad660(void*, void*, void*, int);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern void func_02012694(int, void*);
extern void func_ov100_02141fb0(void*);
extern void func_ov100_02142130(void*);
extern void _ZN5Actor14TriplePoofDustEv(void*);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern int Vec3_HorzLen(void*);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void*);
extern void _ZN5Actor11LandingDustEb(void*, int);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int, unsigned int, unsigned int, void*, unsigned int);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void*, int);
extern void func_ov100_02142264(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
}

extern "C" void func_ov100_0214272c(char *c)
{
    int r;

    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x374);
    r = func_020ad660(c, c + 0x110, c + 0x2cc, 3);
    if (r != 0) {
        if (r != 2)
            return;

        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0xf, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
        func_02012694(0x173, c + 0x74);
        return;
    }

    func_ov100_02141fb0(c);
    func_ov100_02142130(c);

    if (*(int *)(c + 0x60) >= *(int *)(c + 0x3c8)) {
        if (*(unsigned char *)(c + 0x3d1) == 0)
            goto ground;
    }

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0xf,
        *(int *)(c + 0x5c),
        *(volatile int *)(c + 0x60),
        *(int *)(c + 0x64));
    func_02012694(0x173, c + 0x74);
    _ZN5Actor14TriplePoofDustEv(c);
    _ZN9ActorBase18MarkForDestructionEv(c);
    return;

ground:
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110)) {
        int *pa4;
        int *pac;

        pa4 = (int *)(((int)c + 0xa4));
        pac = (int *)(((int)c + 0xac));

        *pa4 = *pa4 + *(int *)(c + 0xd4) * 3;
        *pac = *pac + *(int *)(c + 0xdc) * 3;
        *(int *)(c + 0x98) = Vec3_HorzLen(pa4);

        *(short *)(c + 0x94) =
            _ZN4cstd5atan2E5Fix12IiES1_(
                *(int *)(c + 0xa4), *(int *)(c + 0xac));

        if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x110)) {
            func_02012694(0x40, c + 0x74);
            _ZN5Actor11LandingDustEb(c, 1);

            if (*(int *)(c + 0xa8) < -0x8000)
                *(int *)(c + 0xa8) = *(int *)(c + 0xa8) * -3 / 2;
        } else {
            *(int *)(c + 0x3cc) =
                _ZN5Sound8PlayLongEjjjRK7Vector3s(
                    *(int *)(c + 0x3cc), 3, 0x8a, c + 0x74, 0);
        }
    }

    {
        int v;
        int lim;

        v = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
        lim = *(int *)(c + 0xa0);
        if (v >= lim)
            lim = v;
        *(int *)(c + 0xa8) = lim;
    }

    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, 0);
    func_ov100_02142264(c);
    _ZN12CylinderClsn5ClearEv(c + 0x374);
    _ZN12CylinderClsn6UpdateEv(c + 0x374);
}
