#include "types.h"
#include "nitro/fx/fx.h"

/* Build a rotation about Y from its sine and cosine. The particle draws pick
 * it or func_0204c0e8 out of data_02099fbc by the definition's polygon
 * rotation axis; pret's SPL names it SPLUtil_RotateY. */
void func_0204c0a8(int sin, int cos, MtxFx43 *mat) {
    mat->m[0][0] = cos;
    mat->m[1][0] = 0;
    mat->m[2][0] = sin;
    mat->m[3][0] = 0;
    mat->m[0][1] = 0;
    mat->m[1][1] = 0x1000;
    mat->m[2][1] = 0;
    mat->m[3][1] = 0;
    mat->m[0][2] = -sin;
    mat->m[1][2] = 0;
    mat->m[2][2] = cos;
    mat->m[3][2] = 0;
}
