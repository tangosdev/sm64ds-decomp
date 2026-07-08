extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short rx, short ry, short rz);
struct M { int w[12]; };
extern struct M MATRIX_SCRATCH_PAPER;
void func_ov002_020fed7c(char *c) {
    int v[3];
    Vec3_Asr(v, c+0x5c, 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
    *(struct M*)(c+0x31c) = MATRIX_SCRATCH_PAPER;
}
