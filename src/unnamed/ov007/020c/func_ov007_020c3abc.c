extern short data_02082214[];
struct Mat3 { int m[9]; };
extern void func_02052550(struct Mat3 *m, int s, int c);
extern void func_0205256c(struct Mat3 *m, int s, int c);
extern void Matrix3x3_SetRotationZ(struct Mat3 *m, int s, int c);
extern void MulMat3x3Mat3x3(struct Mat3 *dst, struct Mat3 *a, struct Mat3 *b);

void func_ov007_020c3abc(struct Mat3 *out, int ax, int ay, int az)
{
    struct Mat3 tmp;
    int idx;
    if (ax != 0) {
        idx = ax >> 4;
        func_02052550(&tmp, data_02082214[idx*2], data_02082214[idx*2+1]);
        MulMat3x3Mat3x3(out, &tmp, out);
    }
    if (ay != 0) {
        idx = ay >> 4;
        func_0205256c(&tmp, data_02082214[idx*2], data_02082214[idx*2+1]);
        MulMat3x3Mat3x3(out, &tmp, out);
    }
    if (az != 0) {
        idx = az >> 4;
        Matrix3x3_SetRotationZ(&tmp, data_02082214[idx*2], data_02082214[idx*2+1]);
        MulMat3x3Mat3x3(out, &tmp, out);
    }
}
