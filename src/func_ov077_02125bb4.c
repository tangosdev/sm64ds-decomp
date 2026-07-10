typedef int Fix12i;

struct Vector3 {
    int x, y, z;
};

extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *p);
extern void func_ov077_021250a8(void *c);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *p);
extern void func_020383fc(void *p);
extern int func_ov077_02124ce4(void *c);
extern void func_02012694(int id, void *pos);
extern void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void *dir, void *callback);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void *p);
extern void _ZN5Actor8PoofDustEv(void *c);
extern void _ZN9ActorBase18MarkForDestructionEv(void *c);
extern void _ZN12WithMeshClsn15ClearLimMovFlagEv(void *p);
extern void func_ov077_02125e94(void *c, int a);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i x, Fix12i y, Fix12i z);
extern void func_0201267c(int id, void *pos);
extern void _ZN12CylinderClsn5ClearEv(void *p);
extern void _ZN12CylinderClsn6UpdateEv(void *p);

extern int data_0209f32c;

int func_ov077_02125bb4(char *c)
{
    int r4;
    int d;
    int x, y, z;
    struct Vector3 vec;

    *(short *)(c + 0x8c) = *(short *)(c + 0x8c) + 0x4e20;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x1b0);
    func_ov077_021250a8(c);
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x1e4);
    func_020383fc(c + 0x1e4);

    r4 = func_ov077_02124ce4(c);
    if (r4) {
        if (*(unsigned char *)(c + 0x3e4) == 0) {
            func_02012694(0xe2, c + 0x74);
            _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(
                *(int *)(c + 0x5c), data_0209f32c, *(int *)(c + 0x64));
            *(unsigned int *)(c + 0x3e0) =
                _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(unsigned int *)(c + 0x3e0), 0x109,
                    *(int *)(c + 0x5c), data_0209f32c, *(int *)(c + 0x64),
                    0, 0);
        }
        *(int *)(c + 0x9c) = -0x400;
        *(int *)(c + 0xa0) = -0x5000;
        *(int *)(c + 0x98) = 0x2000;
    } else {
        *(int *)(c + 0x9c) = -0x2000;
        *(int *)(c + 0xa0) = -0x3c000;
        *(int *)(c + 0x98) = 0x4000;
    }
    *(unsigned char *)(c + 0x3e4) = (unsigned char)r4;

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x1e4)) {
        *(int *)(c + 0xa8) = (*(int *)(c + 0xa8) * -0x3c) / 100;
        d = *(int *)(c + 0x3dc) ? *(int *)(c + 0x60) - *(int *)(c + 0x3dc) : 0;
        if (d < -0xc8000) {
            _ZN5Actor8PoofDustEv(c);
            func_02012694(0x166, c + 0x74);
            _ZN9ActorBase18MarkForDestructionEv(c);
        } else if (*(int *)(c + 0xa8) < 0xa000) {
            *(int *)(c + 0xa8) = 0;
            *(short *)(c + 0x8c) = *(short *)(c + 0x92);
            *(short *)(c + 0x8e) = *(short *)(c + 0x94);
            *(short *)(c + 0x90) = *(short *)(c + 0x96);
            _ZN12WithMeshClsn15ClearLimMovFlagEv(c + 0x1e4);
            *(unsigned int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x10000000u;
            func_ov077_02125e94(c, 1);
        } else {
            x = *(int *)(c + 0x5c);
            y = *(int *)(c + 0x60) + 0x28000;
            z = *(int *)(c + 0x64);
            ((int *)&vec)[0] = x;
            ((int *)&vec)[1] = y;
            ((int *)&vec)[2] = z;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, c + 0x74);
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x1b0);
    _ZN12CylinderClsn6UpdateEv(c + 0x1b0);
    return 1;
}
