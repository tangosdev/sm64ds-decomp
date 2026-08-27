typedef long long s64;
typedef signed short s16;

extern s16 data_02082214[];

void func_02045178(int *out, int ax, int ay, int az, int tx, int ty, int tz) {
    s16 *t = data_02082214;
    int ia = ax >> 4;
    int ib = ay >> 4;
    int ic = az >> 4;
    int a0 = t[ia * 2];
    int a1 = t[ia * 2 + 1];
    int b0 = t[ib * 2];
    int b1 = t[ib * 2 + 1];
    int c0 = t[ic * 2];
    int c1 = t[ic * 2 + 1];
    int p00 = (int)(((s64)a0 * c0) >> 12);
    int p11 = (int)(((s64)a1 * c1) >> 12);
    int p01 = (int)(((s64)a0 * c1) >> 12);
    int p10 = (int)(((s64)a1 * c0) >> 12);

    out[0] = (int)(((s64)b1 * c1) >> 12);
    out[1] = (int)(((s64)b1 * c0) >> 12);
    out[2] = -b0;
    out[3] = (int)(((s64)b0 * p01 + 0x800) >> 12) - p10;
    out[4] = p11 + (int)(((s64)b0 * p00 + 0x800) >> 12);
    out[5] = (int)(((s64)b1 * a0) >> 12);
    out[6] = p00 + (int)(((s64)b0 * p11 + 0x800) >> 12);
    out[7] = (int)(((s64)b0 * p10 + 0x800) >> 12) - p01;
    out[8] = (int)(((s64)b1 * a1) >> 12);
    out[9] = tx;
    out[10] = ty;
    out[11] = tz;
}
