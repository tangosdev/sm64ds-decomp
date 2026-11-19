#include "types.h"
/* recovered: shared common types */
#include "common.h"
#include "math/CalcLib.h"

void func_0203cf00(Vector3 *out, Vector3 *a, Vector3 *b) {
    Vector3 cross;
    VEC_CrossProduct(a, b, &cross);
    out->x = cross.x;
    out->y = cross.y;
    out->z = cross.z;
}

void func_0203cebc(Vector3 *out, Vector3 *cross, Vector3 *a, Vector3 *b) {
    VEC_CrossProduct(a, b, cross);
    out->x = cross->x;
    out->y = cross->y;
    out->z = cross->z;
}

/* func_0203ce80 at 0x0203ce80, size=0x3c
 * Normalizes *src in-place (VEC_Normalize(src,src)), then copies to *dst field-by-field.
 */
void func_0203ce80(struct Vector3 *dst, struct Vector3 *src) {
    VEC_Normalize(src, src);
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}
