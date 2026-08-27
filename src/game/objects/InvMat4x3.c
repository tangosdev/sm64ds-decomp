typedef signed int s32;
typedef signed long long s64;

extern void _ZN4cstd16reciprocal_asyncE5Fix12IiE(int a);
extern int _ZN4cstd11fdiv_resultEv(void);
extern void Copy48Bytes(int *src, int *dst);

int InvMat4x3(int *src, int *dst)
{
    s32 buf[12];
    int *out;
    int p, q, r;
    int det;
    int recip;

    out = (src == dst) ? buf : dst;

    p = (int)(((s64)src[4] * src[8] - (s64)src[5] * src[7] + 0x800) >> 12);
    q = (int)(((s64)src[3] * src[8] - (s64)src[5] * src[6] + 0x800) >> 12);
    r = (int)(((s64)src[3] * src[7] - (s64)src[4] * src[6] + 0x800) >> 12);

    det = (int)(((s64)src[0] * p - (s64)src[1] * q + (s64)src[2] * r + 0x800) >> 12);
    if (det == 0) {
        return -1;
    }

    _ZN4cstd16reciprocal_asyncE5Fix12IiE(det);

    {
        int x1 = (int)(((s64)src[1] * src[8] - (s64)src[7] * src[2]) >> 12);
        int x2 = (int)(((s64)src[1] * src[5] - (s64)src[4] * src[2]) >> 12);
        int x3 = (int)(((s64)src[0] * src[8] - (s64)src[6] * src[2]) >> 12);
        int x4 = (int)(((s64)src[0] * src[5] - (s64)src[3] * src[2]) >> 12);

        recip = _ZN4cstd11fdiv_resultEv();

        out[0] = (int)(((s64)recip * p) >> 12);
        out[1] = -(int)(((s64)recip * x1) >> 12);
        out[2] = (int)(((s64)recip * x2) >> 12);
        out[3] = -(int)(((s64)recip * q) >> 12);
        out[4] = (int)(((s64)recip * x3) >> 12);
        out[5] = -(int)(((s64)recip * x4) >> 12);
        out[6] = (int)(((s64)recip * r) >> 12);
    }

    {
        int x5 = (int)(((s64)src[0] * src[7] - (s64)src[6] * src[1]) >> 12);
        out[7] = -(int)(((s64)recip * x5) >> 12);
    }
    {
        int x6 = (int)(((s64)src[0] * src[4] - (s64)src[3] * src[1]) >> 12);
        out[8] = (int)(((s64)recip * x6) >> 12);
    }

    out[9]  = -(int)((((s64)out[0] * src[9]) + ((s64)out[3] * src[10]) + ((s64)out[6] * src[11])) >> 12);
    out[10] = -(int)((((s64)out[1] * src[9]) + ((s64)out[4] * src[10]) + ((s64)out[7] * src[11])) >> 12);
    out[11] = -(int)((((s64)out[2] * src[9]) + ((s64)out[5] * src[10]) + ((s64)out[8] * src[11])) >> 12);

    if (out == buf) {
        Copy48Bytes(buf, dst);
    }

    return 0;
}
