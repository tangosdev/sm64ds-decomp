//cpp
extern "C" {
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned int u);
extern int MATRIX_SCRATCH_PAPER[];

typedef struct { int w[12]; } M48;

void func_ov066_021194fc(char* c)
{
    int v[3];
    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    if (*(int*)(c + 0x49c) == 0)
        *(M48*)(c + 0x3ec) = *(M48*)MATRIX_SCRATCH_PAPER;
    else
        *(M48*)(c + 0x37c) = *(M48*)MATRIX_SCRATCH_PAPER;
    if (*(int*)(c + 0x49c) == 0)
        return;
    if (*(int*)(c + 0x4a8) >= *(int*)(c + 0x60))
        return;
    {
        int d;
        if (*(int*)(c + 0x49c) == 2)
            d = 0x64000;
        else
            d = -0x64000;
        Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER,
            (*(int*)(c + 0x5c) + d) >> 3,
            (*(int*)(c + 0x60) - 0x8000) >> 3,
            (*(int*)(c + 0x64) + 0xa0000) >> 3);
    }
    *(M48*)(c + 0x45c) = *(M48*)MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x420, c + 0x45c, 0x140000, 0x258000, 0xf);
}
}
