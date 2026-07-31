typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct Mtx43 { int w[12]; } Mtx43;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))

extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(Mtx43* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(Mtx43* m, s16 ang);
extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
extern void Vec3_LslInPlace(Vec3* v, int sh);
extern void SubVec3(Vec3* a, Vec3* b, Vec3* c);

extern Mtx43 data_020a0e68;
extern u8 data_ov084_021302ec[];
extern s16 data_02082214[];

void func_ov084_0212ec60(char* c)
{
    volatile s16 ang[3];
    struct { Mtx43 saved; Vec3 tv; Vec3 v; } L;
    int lr;
    int r3;

    Vec3_Asr(&L.v, (Vec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, L.v.x, L.v.y, L.v.z);
    L.saved = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
    *(Mtx43*)(c + 0x12c) = data_020a0e68;

    ang[0] = 0;
    ang[1] = 0;
    ang[2] = 0;
    for (lr = 0; lr < 5; lr++) {
        ang[1] = (s16)(ang[1] + *(s16*)(*(char**)(c + 0x120) + data_ov084_021302ec[lr] * 0x34 + 0x1c));
    }

    *(s32*)(c + 0x440) = 0;
    *(s32*)(c + 0x444) = 0;
    *(s32*)(c + 0x448) = 0;
    data_020a0e68 = L.saved;
    MulMat4x3Mat4x3((char*)*(void**)(c + 0x124) + 0x120, &data_020a0e68, &data_020a0e68);
    *(s32*)(c + 0x440) = data_020a0e68.w[9];
    *(s32*)(c + 0x444) = data_020a0e68.w[10];
    *(s32*)(c + 0x448) = data_020a0e68.w[11];
    Vec3_LslInPlace((Vec3*)(c + 0x440), 3);
    SubVec3((Vec3*)(c + 0x440), (Vec3*)(c + 0x5c), (Vec3*)(c + 0x440));

    *(s32*)AT(c, 0x444) -= data_02082214[((u16)ang[0] >> 4) << 1] * (s16)0x14 + 0x32000;
    *(s32*)AT(c, 0x448) += 0x32000;

    L.tv.x = *(s32*)(c + 0x5c);
    L.tv.y = *(s32*)(c + 0x60);
    L.tv.z = *(s32*)(c + 0x64);

    {
        int rr = (int)((u32)(*(s32*)(c + 0x168) << 4) >> 0x10);
        int d = rr - 0x28;
        if (d < 0)
            d = -d;
        r3 = ((d << 12) / 10) + 0x400;
        if (r3 < 0x800)
            r3 = 0x800;
        *(s32*)(c + 0x428) = r3;
        *(s32*)(c + 0x42c) = r3;
        *(s32*)(c + 0x430) = r3;
    }

    L.tv.x = data_02082214[(*(u16*)(c + 0x8e) >> 4) << 1] * (s16)0xe0 + L.tv.x;
    r3 = r3 - 0xc00;
    L.tv.z = data_02082214[((*(u16*)(c + 0x8e) >> 4) << 1) + 1] * (s16)0xe0 + L.tv.z;
    L.tv.y = L.tv.y + (r3 * 0x18 + 0x38000);

    *(s32*)(c + 0x434) = L.tv.x;
    *(s32*)(c + 0x438) = L.tv.y;
    *(s32*)(c + 0x43c) = L.tv.z;

    Matrix4x3_FromTranslation((Mtx43*)(c + 0x190), L.tv.x >> 3, L.tv.y >> 3, L.tv.z >> 3);
}
