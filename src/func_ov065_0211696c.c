typedef unsigned int u32;
typedef short s16;

typedef struct Mtx43 { int w[12]; } Mtx43;

extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* dst, void* a, void* b);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* c, void* shadow, void* mtx, int rad, int height, u32 flags);

extern int data_020a0e68[];

void func_ov065_0211696c(char* c)
{
    int v[3];

    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68,
        *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));

    *(Mtx43*)(c + 0x31c) = *(Mtx43*)data_020a0e68;
    *(int*)(c + 0x3c0) = 0;
    *(int*)(c + 0x3c4) = 0;
    *(int*)(c + 0x3c8) = 0;
    *(Mtx43*)data_020a0e68 = *(Mtx43*)(c + 0x31c);

    MulMat4x3Mat4x3(*(char**)(c + 0x314) + 0xc0, data_020a0e68, data_020a0e68);

    *(int*)(c + 0x3c0) = data_020a0e68[9];
    *(int*)(c + 0x3c4) = data_020a0e68[10];
    *(int*)(c + 0x3c8) = data_020a0e68[11];
    *(int*)(((int)c + 0x3c0)) <<= 3;
    *(int*)(((int)c + 0x3c4)) <<= 3;
    *(int*)(((int)c + 0x3c4)) -= 0xa000;
    *(int*)(((int)c + 0x3c8)) <<= 3;

    Matrix4x3_FromTranslation(data_020a0e68,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0x18000) >> 3,
        *(int*)(c + 0x64) >> 3);

    *(Mtx43*)(c + 0x38c) = *(Mtx43*)data_020a0e68;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x364, c + 0x38c, 0x64000, 0x258000, 0xf);
}
