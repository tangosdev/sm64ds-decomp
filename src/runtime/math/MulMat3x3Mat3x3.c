typedef long long s64;
typedef struct { int m[9]; } Mat9;

void MulMat3x3Mat3x3(const int *A, const int *B, int *dst)
{
    int tmp[9];
    int a0, a1, a2;
    int b0, b1, b2;
    int *d = dst;

    if (dst == B) d = tmp;

    a0 = A[0]; a1 = A[1]; a2 = A[2];
    d[0] = (int)(((s64)a0 * B[0] + (s64)a1 * B[3] + (s64)a2 * B[6]) >> 12);
    d[1] = (int)(((s64)a0 * B[1] + (s64)a1 * B[4] + (s64)a2 * B[7]) >> 12);
    b0 = B[2]; b1 = B[5]; b2 = B[8];
    d[2] = (int)(((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A[3]; a1 = A[4]; a2 = A[5];
    d[5] = (int)(((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d[4] = (int)(((s64)a0 * B[1] + (s64)a1 * B[4] + (s64)a2 * B[7]) >> 12);
    b0 = B[0]; b1 = B[3]; b2 = B[6];
    d[3] = (int)(((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A[6]; a1 = A[7]; a2 = A[8];
    d[6] = (int)(((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d[7] = (int)(((s64)a0 * B[1] + (s64)a1 * B[4] + (s64)a2 * B[7]) >> 12);
    d[8] = (int)(((s64)a0 * B[2] + (s64)a1 * B[5] + (s64)a2 * B[8]) >> 12);

    if (d == tmp) {
        *(Mat9 *)dst = *(Mat9 *)tmp;
    }
}
