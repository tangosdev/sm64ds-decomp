struct Matrix4x3 { int m[12]; };
struct Vec3 { int x, y, z; };

extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *mF, short angX);
extern unsigned int _ZN5Actor10FindWithIDEj(unsigned int id);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *mtx, int a, int b, unsigned int g);

extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
extern struct Matrix4x3 _ZN9Matrix3x38IDENTITYE;

void func_ov085_021295bc(char *c)
{
    struct Matrix4x3 *dst;
    void *actor;
    unsigned int id;
    int vx, vy, vz;

    Matrix4x3_FromRotationY(c + 0x124, *(short*)(c + 0x8e));
    *(int*)(c + 0x148) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x14c) = (*(int*)(c + 0x60) + 0x4000) >> 3;
    *(int*)(c + 0x150) = *(int*)(c + 0x64) >> 3;

    MATRIX_SCRATCH_PAPER = *(struct Matrix4x3*)(*(char**)(c + 0x11c) + 0xf0);
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c + 0x200));
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short*)(c + 0x204));
    *(struct Matrix4x3*)(*(char**)(c + 0x11c) + 0xf0) = MATRIX_SCRATCH_PAPER;

    id = *(unsigned int*)(c + 0x1f4);
    if (id != 0) {
        actor = (void*)_ZN5Actor10FindWithIDEj(id);
        if (actor != 0) {
            *(int*)&vx = 0;
            *(int*)&vy = 0;
            *(int*)&vz = 0;
            dst = (struct Matrix4x3*)(*(char**)(c + 0x11c) + 0xf0);
            MulMat4x3Mat4x3(dst, c + 0x124, c + 0x194);

            MATRIX_SCRATCH_PAPER = *(struct Matrix4x3*)(c + 0x194);
            Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, 0x4000, -0xa00);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, -0x1000, 0, 0);
            *(struct Matrix4x3*)(c + 0x194) = MATRIX_SCRATCH_PAPER;

            vx = MATRIX_SCRATCH_PAPER.m[9];
            vy = MATRIX_SCRATCH_PAPER.m[10];
            vz = MATRIX_SCRATCH_PAPER.m[11];
            vy <<= 3;
            vx <<= 3;
            vz <<= 3;
            *(int*)((char*)actor + 0x5c) = vx;
            *(int*)((char*)actor + 0x60) = vy;
            *(int*)((char*)actor + 0x64) = vz;
            *(void**)((char*)actor + 0xc8) = c + 0x194;
        }
    }

    *(struct Matrix4x3*)(c + 0x1c4) = _ZN9Matrix3x38IDENTITYE;
    *(int*)(c + 0x1e8) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x1ec) = (*(int*)(c + 0x60) - 0x8000) >> 3;
    *(int*)(c + 0x1f0) = *(int*)(c + 0x64) >> 3;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x16c, c + 0x1c4, 0x50000, 0x64000, 0xf);
}
