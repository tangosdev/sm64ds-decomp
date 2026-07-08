//cpp
extern "C" {
struct Vec3 { int x, y, z; };
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);
extern int MATRIX_SCRATCH_PAPER[];

typedef struct { int w[12]; } M48;

void func_ov070_02120070(char* c)
{
    Vec3 v;
    Vec3_Asr(&v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48*)(c + 0x31c) = *(M48*)MATRIX_SCRATCH_PAPER;
    Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0xa000) >> 3, *(int*)(c + 0x64) >> 3);
    *(M48*)(c + 0x38c) = *(M48*)MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x364, c + 0x38c, 0x76000, 0x320000, 0xf);
}
}
