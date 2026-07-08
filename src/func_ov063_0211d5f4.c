typedef short s16;
struct Vec3 { int x, y, z; };

extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(struct Vec3* in, void* m, struct Vec3* out);
extern void AddVec3(struct Vec3* a, struct Vec3* b, struct Vec3* c);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* self, void* sm, void* mtx, int fx, int t, unsigned int u);
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void* self, void* sm, void* mtx, int fx, int t1, int t2, unsigned int u);

extern struct Vec3 MATRIX_SCRATCH_PAPER;

void func_ov063_0211d5f4(char* self)
{
    struct Vec3 in, out;

    in.x = 0x40000;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = -0x10000;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(self + 0x8e));
    MulVec3Mat4x3(&in, &MATRIX_SCRATCH_PAPER, &out);
    AddVec3(&out, (struct Vec3*)(self + 0x5c), &out);
    Matrix4x3_FromRotationY(self + 0x45c, *(s16*)(self + 0x8e));
    *(int*)(self + 0x480) = out.x >> 3;
    *(int*)(self + 0x484) = *(int*)(self + 0x60) >> 3;
    *(int*)(self + 0x488) = out.z >> 3;

    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.y = 0;
    in.z = 0;
    in.x = -0x60000;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(self + 0x8e));
    MulVec3Mat4x3(&in, &MATRIX_SCRATCH_PAPER, &out);
    AddVec3(&out, (struct Vec3*)(self + 0x5c), &out);
    Matrix4x3_FromRotationY(self + 0x3fc, *(s16*)(self + 0x8e));
    *(int*)(self + 0x420) = out.x >> 3;
    *(int*)(self + 0x424) = *(int*)(self + 0x60) >> 3;
    *(int*)(self + 0x428) = out.z >> 3;

    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.x = 0;
    in.y = 0;
    in.z = -0x10000;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(self + 0x8e));
    MulVec3Mat4x3(&in, &MATRIX_SCRATCH_PAPER, &out);
    AddVec3(&out, (struct Vec3*)(self + 0x5c), &out);
    Matrix4x3_FromRotationY(self + 0x42c, *(s16*)(self + 0x8e));
    *(int*)(self + 0x450) = out.x >> 3;
    *(int*)(self + 0x454) = *(int*)(self + 0x60) >> 3;
    *(int*)(self + 0x458) = out.z >> 3;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, self + 0x3d4, self + 0x45c, 0xf0000, 0x50000, 0xf);
    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, self + 0x384, self + 0x3fc, 0xa0000, 0x50000, 0x110000, 0xf);
    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, self + 0x3ac, self + 0x42c, 0x80000, 0x50000, 0xf0000, 0xf);
}
