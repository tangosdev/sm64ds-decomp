#include "nitro/fx/fx.h"

/* Build a rotation about the (1, 1, 1) diagonal from its sine and cosine:
 * 0x555 is 1/3 and 0x93d is sqrt(1/3) in 20.12. The particle draws pick it or
 * func_0204c0a8 out of data_02099fbc; pret's SPL names it SPLUtil_RotateXYZ. */
void func_0204c0e8(int sin, int cos, MtxFx43 *mat) {
    int c = FX_MUL((0x1000 - cos), 0x555);
    int s = FX_MUL(sin, 0x93d);
    int diag;
    int plus = c + s;
    int minus = c - s;
    diag = c + cos;
    mat->m[0][0] = diag;
    mat->m[1][0] = plus;
    mat->m[2][0] = minus;
    mat->m[3][0] = 0;
    mat->m[0][1] = minus;
    mat->m[1][1] = diag;
    mat->m[2][1] = plus;
    mat->m[3][1] = 0;
    mat->m[0][2] = plus;
    mat->m[1][2] = minus;
    mat->m[2][2] = diag;
    mat->m[3][2] = 0;
}
