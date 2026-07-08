// NONMATCHING: different op / idiom (div=7). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12i;
typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Matrix4x3;

extern Matrix4x3 MATRIX_SCRATCH_PAPER;

extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
extern void InvMat4x3(Matrix4x3 *dst, Matrix4x3 *inv);

void func_ov080_0212555c(char *self)
{
    Vec3 v;
    Vec3_Asr(&v, (Vec3*)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, *(short*)(self + 0x8c), *(short*)(self + 0x8e), *(short*)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, -(((unsigned char)(*(int*)(self + 8) & 0xf) + 1) * 0x64000) / 16, 0, 0);
    *(Matrix4x3*)(self + 0xd4) = MATRIX_SCRATCH_PAPER;
    InvMat4x3(&MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    *(Matrix4x3*)(self + 0x104) = MATRIX_SCRATCH_PAPER;
}
