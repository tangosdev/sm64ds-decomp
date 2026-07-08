struct Mat43 { int m[12]; };

extern short SINE_TABLE[];
extern void func_0205283c(struct Mat43 *out, short a, short b);
extern void func_02052820(struct Mat43 *out, short a, short b);
extern void func_02052800(struct Mat43 *out, short a, short b);
extern void MulMat4x3Mat4x3(struct Mat43 *dst, struct Mat43 *a, struct Mat43 *b);

void func_ov007_020c39f8(struct Mat43 *mat, int rx, int ry, int rz) {
    struct Mat43 t;
    int ix = (rx >> 4) * 2;
    int iy = (ry >> 4) * 2;
    int iz = (rz >> 4) * 2;
    func_0205283c(&t, SINE_TABLE[iz], SINE_TABLE[iz + 1]);
    MulMat4x3Mat4x3(mat, &t, mat);
    func_02052820(&t, SINE_TABLE[iy], SINE_TABLE[iy + 1]);
    MulMat4x3Mat4x3(mat, &t, mat);
    func_02052800(&t, SINE_TABLE[ix], SINE_TABLE[ix + 1]);
    MulMat4x3Mat4x3(mat, &t, mat);
}
