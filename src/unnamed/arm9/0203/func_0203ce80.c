// @symbol func_0203ce80
/* recovered: shared common types */
#include "common.h"
/* func_0203ce80 at 0x0203ce80, size=0x3c
 * Normalizes *src in-place (NormalizeVec3(src,src)), then copies to *dst field-by-field.
 */

typedef int Fix12i;



extern void NormalizeVec3(const struct Vector3* src, struct Vector3* dst);

void func_0203ce80(struct Vector3* dst, struct Vector3* src)
{
    NormalizeVec3(src, src);
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}
