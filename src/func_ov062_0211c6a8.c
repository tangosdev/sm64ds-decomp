extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern unsigned int _ZN5Actor10FindWithIDEj(unsigned int id);
extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned int u);

extern int MATRIX_SCRATCH_PAPER[];

typedef struct { int w[12]; } M48;

void func_ov062_0211c6a8(char* c)
{
    int v[3];
    char* actor;

    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48*)(c + 0x350) = *(M48*)MATRIX_SCRATCH_PAPER;

    if (*(unsigned int*)(c + 0x44c) != 0)
    {
        actor = (char*)_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x44c));
        if (actor != 0)
        {
            *(int*)(c + 0x450) = 0;
            *(int*)(c + 0x454) = 0;
            *(int*)(c + 0x458) = 0;
            MulMat4x3Mat4x3(*(char**)(c + 0x348) + 0x120, c + 0x350, c + 0x3fc);
            *(int*)(c + 0x450) = MATRIX_SCRATCH_PAPER[9];
            *(int*)(c + 0x454) = MATRIX_SCRATCH_PAPER[10];
            *(int*)(c + 0x458) = MATRIX_SCRATCH_PAPER[11];
            *(int*)(((int)c + 0x450) & 0xFFFFFFFFFFFFFFFF) <<= 3;
            *(int*)(((int)c + 0x454) & 0xFFFFFFFFFFFFFFFF) <<= 3;
            *(int*)(((int)c + 0x458) & 0xFFFFFFFFFFFFFFFF) <<= 3;
            if (*(int*)(c + 0x468) == 1)
            {
                Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, -0xd000, 0x3000, -0x4000);
                Matrix4x3_ApplyInPlaceToRotationXYZExt(MATRIX_SCRATCH_PAPER, 0, -0x8000, -0x4400);
            }
            else
            {
                Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, -0x7000, 0x1800, 0);
                Matrix4x3_ApplyInPlaceToRotationXYZExt(MATRIX_SCRATCH_PAPER, -0x3000, -0x8000, -0x4000);
            }
            MulMat4x3Mat4x3(MATRIX_SCRATCH_PAPER, c + 0x3fc, c + 0x3fc);
            *(void**)(actor + 0xc8) = c + 0x3fc;
        }
    }

    Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0x18000) >> 3, *(int*)(c + 0x64) >> 3);
    *(M48*)(c + 0x3cc) = *(M48*)MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x3a4, c + 0x3cc, 0x40000, 0x258000, 0xf);
}
