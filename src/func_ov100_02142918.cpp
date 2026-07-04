//cpp
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" {
void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void* self, void* c);
int func_020ad660(void* a, void* b, void* d, int e);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void func_02012694(int a, void* p);
void func_ov100_02141fb0(void* c);
void func_ov100_02142130(char* c);
s16 _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void* self, int a, int b, s16 ang);
int Vec3_HorzLen(void* v);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* c);
s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
void _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(void* self);
int _ZNK12WithMeshClsn13JustHitGroundEv(void* c);
void _ZN5Actor11LandingDustEb(void* self, int b);
void* _ZN5Sound8PlayLongEjjjRK7Vector3j(void* a, unsigned int b, unsigned int c, void* v, unsigned int d);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void* self, void* c);
void func_ov100_02142264(char* c);
void _ZN12CylinderClsn5ClearEv(void* c);
void _ZN12CylinderClsn6UpdateEv(void* c);
}

extern s16 data_02082214[];

extern "C" void func_ov100_02142918(char* c)
{
    int r;
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x374);
    r = func_020ad660(c, c + 0x110, c + 0x2cc, 3);
    if (r != 0) {
        if (r != 2)
            return;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
        func_02012694(0x173, c + 0x74);
        return;
    }

    func_ov100_02141fb0(c);
    if (*(int*)(c + 0x10c) == 8)
        return;
    func_ov100_02142130(c);

    if (*(u8*)(c + 0x3d1) != 0) {
        int ang;
        *(s16*)(c + 0x94) = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(c, *(int*)(c + 0xe0), *(int*)(c + 0xe8), *(s16*)(c + 0x94));
        ang = *(u16*)(c + 0x94);
        *(int*)(c + 0xa4) = (int)(((long long)*(int*)(c + 0x98) * data_02082214[(ang >> 4) << 1] + 0x800) >> 12);
        ang = *(u16*)(c + 0x94);
        *(int*)(c + 0xac) = (int)(((long long)*(int*)(c + 0x98) * data_02082214[((ang >> 4) << 1) + 1] + 0x800) >> 12);
        {
            int* pa4 = (int*)((((long long)(int)(c + 0xa4)) & 0xFFFFFFFFFFFFFFFFLL));
            int* pac = (int*)((((long long)(int)(c + 0xac)) & 0xFFFFFFFFFFFFFFFFLL));
            *pa4 = *pa4 - *(int*)(c + 0xe0) * 3;
            *pac = *pac - *(int*)(c + 0xe8) * 3;
            *(int*)(c + 0x98) = Vec3_HorzLen(pa4);
        }
        goto Lend;
    }

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110) == 0)
        goto Lend;

    {
        int* pa4 = (int*)((((long long)(int)(c + 0xa4)) & 0xFFFFFFFFFFFFFFFFLL));
        int* pac = (int*)((((long long)(int)(c + 0xac)) & 0xFFFFFFFFFFFFFFFFLL));
        *pa4 = *pa4 + *(int*)(c + 0xd4) * 3;
        *pac = *pac + *(int*)(c + 0xdc) * 3;
        *(int*)(c + 0x98) = Vec3_HorzLen(pa4);
    }
    if (*(int*)(c + 0xd8) == 0x1000 && *(int*)(c + 0x98) > 0x1c000) {
        *(int*)(c + 0x98) = 0x1c000;
        *(s16*)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(c + 0xa4), *(int*)(c + 0xac));
        _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(c);
    }

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x110) != 0) {
        func_02012694(0x40, c + 0x74);
        _ZN5Actor11LandingDustEb(c, 1);
        goto Lend;
    }

    *(void**)(c + 0x3cc) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(void**)(c + 0x3cc), 3, 0x8a, c + 0x74, 0);

Lend:
    {
        int nv = *(int*)(c + 0xa8) + *(int*)(c + 0x9c);
        int lim = *(int*)(c + 0xa0);
        if (nv >= lim)
            lim = nv;
        *(int*)(c + 0xa8) = lim;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, 0);
    func_ov100_02142264(c);
    _ZN12CylinderClsn5ClearEv(c + 0x374);
    _ZN12CylinderClsn6UpdateEv(c + 0x374);
}
