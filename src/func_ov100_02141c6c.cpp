//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vec3 { s32 x, y, z; };
struct SFP { void* a; void* b; };

extern "C" {
    void* _ZN5Model8LoadFileER13SharedFilePtr(void* s);
    void* _ZN9Animation8LoadFileER13SharedFilePtr(void* s);
    int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* mb, void* bmd, int a, int b);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* bca, int a, int f, u32 e);
    int _ZN11ShadowModel12InitCylinderEv(void* sm);
    void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* w, void* a, int r, int rr, void* v, void* vv);
    void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* c, void* a, void* pos, int r, int rr, u32 e, u32 f);
    int RandomIntInternal(int* seed);
    int _ZNK9Animation13GetFrameCountEv(void* anim);
    u32 __aeabi_uidiv(u32 a, u32 b);
}

extern SFP ONE_UP_MUSHROOM_MODEL_PTR;
extern SFP data_ov100_02148600;
extern SFP data_ov100_02148668;
extern SFP data_ov100_02148608;
extern int RNG_STATE;

extern "C" int func_ov100_02141c6c(void* self)
{
    u8* c = (u8*)self;
    _ZN5Model8LoadFileER13SharedFilePtr(&ONE_UP_MUSHROOM_MODEL_PTR);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov100_02148600);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148668);
    void* bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148608);

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii((void*)(c+0xd4), bmd, 1, 1) == 0) return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c+0xd4), data_ov100_02148600.b, 0, 0x1000, 0);
    if (_ZN11ShadowModel12InitCylinderEv((void*)(c+0x188)) == 0) return 0;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii((void*)(c+0x138), data_ov100_02148668.b, 1, 1) == 0) return 0;
    if (_ZN11ShadowModel12InitCylinderEv((void*)(c+0x1b0)) == 0) return 0;

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_((void*)(c+0x1d8), self, 0x32000, 0x32000, 0, 0);

    Vec3 v;
    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj((void*)(c+0x394), self, &v, 0x32000, 0x64000, 0x200000, 0);

    int sub = (int)(u8)(*(u32*)(c+8) & 0x30);
    if (sub != 0x10 && sub != 0x20) {
        *(u8*)(c+0x3f0) = 0;
        *(s32*)(c+0x98) = 0x7800;
        *(s32*)(c+0x3e8) = ((u32)RandomIntInternal(&RNG_STATE) >> 16) % 100;
        if ((u32)(u8)(*(u32*)(c+8) & 0xf) > 1)
            *(s32*)(c+0x3e4) = 0;
        else
            *(s32*)(c+0x3e4) = 1;
    } else {
        *(s32*)(c+0x3e4) = 4;
    }

    *(s32*)(c+0x3d4) = *(s32*)(c+0x5c);
    *(s32*)(c+0x3d8) = *(s32*)(c+0x60);
    *(s32*)(c+0x3dc) = *(s32*)(c+0x64);

    int r = RandomIntInternal(&RNG_STATE);
    int fc = _ZNK9Animation13GetFrameCountEv((void*)(c+0x124));
    u32 rem = (u32)r % (u32)fc;
    *(s32*)(c+0x12c) = (rem << 16) >> 4;

    *(s32*)(c+0x3e0) = 0x1000;
    s32 spd = *(s32*)(c+0x3e0);
    *(s32*)(c+0x80) = spd;
    *(s32*)(c+0x84) = spd;
    *(s32*)(c+0x88) = spd;
    *(u8*)(c+0x3f1) = 1;
    return 1;
}
