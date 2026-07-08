typedef int s32;
typedef unsigned int u32;
typedef unsigned char u8;
typedef int Fix12;

typedef struct { s32 x, y, z; } Vec3;
typedef struct { s32 w[12]; } Mtx43;

extern void* _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, Fix12 c, Fix12 d, Fix12 e, const void* f);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, Fix12 c, Fix12 d, Fix12 e, const void* f, void* g);
extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(Mtx43* m, s32 x, s32 y, s32 z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, Fix12 f, Fix12 a, u32 b);

extern s32 data_ov002_02100320[];
extern s32 data_ov002_02100334[];
extern s32 data_ov002_02100348[];
extern Mtx43 MATRIX_SCRATCH_PAPER;

void func_ov002_020f8b24(void* arg0)
{
    char* c = (char*)arg0;
    Vec3 v;
    Vec3 asr;

    {
        s32 x = *(s32*)(c + 0x5c);
        v.x = x;
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if (*(u8*)(c + 0x36e) != 0)
            v.x = x * (u32)-1;
    }
    v.y = v.y + data_ov002_02100320[*(u8*)(c + 0x36d)];

    *(void**)(c + 0x370) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        *(u32*)(c + 0x370), data_ov002_02100334[*(u8*)(c + 0x36d)], v.x, v.y, v.z, 0);

    *(void**)(c + 0x374) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x374), data_ov002_02100348[*(u8*)(c + 0x36d)], v.x, v.y, v.z, 0, 0);

    {
        s32 x = *(s32*)(c + 0x5c);
        v.x = x;
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if (*(u8*)(c + 0x36e) != 0)
            v.x = x * (u32)-1;
    }
    Vec3_Asr(&asr, &v, 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, asr.x, asr.y, asr.z);
    *(Mtx43*)(c + 0x328) = MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x300, c + 0x328, 0x28000, 0x64000, 0xf);
}
