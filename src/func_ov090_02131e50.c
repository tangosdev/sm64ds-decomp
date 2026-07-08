extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short rx, short ry, short rz);
struct M { int w[12]; };
extern struct M MATRIX_SCRATCH_PAPER;
void func_ov090_02131e50(char *c) {
    int src[3];
    int dst[3];
    src[0] = *(int*)(c+0x5c);
    src[1] = *(int*)(c+0x60);
    src[2] = *(int*)(c+0x64);
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, dst[0], dst[1], dst[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
    *(struct M*)(c+0x328) = MATRIX_SCRATCH_PAPER;
}
