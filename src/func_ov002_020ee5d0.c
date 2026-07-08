typedef short s16;
struct Vec3 { int x, y, z; };
struct Matrix4x3 { int w[12]; };
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
extern void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);

void func_ov002_020ee5d0(unsigned char *self, int arg)
{
    struct Vec3 v;
    Vec3_Asr(&v, (struct Vec3 *)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, arg >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&MATRIX_SCRATCH_PAPER,
        *(s16 *)(self + 0x8c), *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, -arg >> 3, 0);
    *(struct Matrix4x3 *)(self + 0xf0) = MATRIX_SCRATCH_PAPER;
}
