#include "common.h"

typedef long long s64;

void MTX_Concat43(const Matrix4x3 *A, const Matrix4x3 *B, Matrix4x3 *dst) {
    Matrix4x3 tmp;
    int a0, a1, a2;
    int b0, b1, b2;
    Matrix4x3 *d = dst;

    if (dst == B)
        d = &tmp;

    a0 = A->m[0];
    a1 = A->m[1];
    a2 = A->m[2];
    d->m[0] = (((s64)a0 * B->m[0] + (s64)a1 * B->m[3] + (s64)a2 * B->m[6]) >> 12);
    d->m[1] = (((s64)a0 * B->m[1] + (s64)a1 * B->m[4] + (s64)a2 * B->m[7]) >> 12);
    b0 = B->m[2];
    b1 = B->m[5];
    b2 = B->m[8];
    d->m[2] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A->m[3];
    a1 = A->m[4];
    a2 = A->m[5];
    d->m[5] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d->m[4] = (((s64)a0 * B->m[1] + (s64)a1 * B->m[4] + (s64)a2 * B->m[7]) >> 12);
    b0 = B->m[0];
    b1 = B->m[3];
    b2 = B->m[6];
    d->m[3] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A->m[6];
    a1 = A->m[7];
    a2 = A->m[8];
    d->m[6] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d->m[7] = (((s64)a0 * B->m[1] + (s64)a1 * B->m[4] + (s64)a2 * B->m[7]) >> 12);
    b0 = B->m[2];
    b1 = B->m[5];
    b2 = B->m[8];
    d->m[8] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A->m[9];
    a1 = A->m[10];
    a2 = A->m[11];
    d->m[11] = ((s64)B->m[11] + (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12));
    d->m[10] =
        ((s64)B->m[10] + (((s64)a0 * B->m[1] + (s64)a1 * B->m[4] + (s64)a2 * B->m[7]) >> 12));
    d->m[9] = ((s64)B->m[9] + (((s64)a0 * B->m[0] + (s64)a1 * B->m[3] + (s64)a2 * B->m[6]) >> 12));

    if (d == &tmp) {
        *dst = tmp;
    }
}
