/* recovered: shared common types */
#include "common.h"
/* func_020458a8 at 0x020458a8, size=0x38
 * Normalizes each of 3 source Vector3s into corresponding destination Vector3s.
 * VEC_Normalize(src, dst): normalizes src into dst.
 */

struct Matrix3x3_Vecs {
    struct Vector3 v0;
    struct Vector3 v1;
    struct Vector3 v2;
};

extern void VEC_Normalize(const struct Vector3 *src, struct Vector3 *dst);

void func_020458a8(struct Matrix3x3_Vecs* dst, struct Matrix3x3_Vecs* src)
{
    VEC_Normalize(&dst->v0, &src->v0);
    VEC_Normalize(&dst->v1, &src->v1);
    VEC_Normalize(&dst->v2, &src->v2);
}
