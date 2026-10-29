#include "types.h"
/* recovered: shared common types */
#include "common.h"

extern void CrossVec3(Vector3 *a, Vector3 *b, Vector3 *result);
extern void NormalizeVec3(const struct Vector3 *src, struct Vector3 *dst);

void func_0203cf00(Vector3 *out, Vector3 *a, Vector3 *b) {
    Vector3 tmp;
    CrossVec3(a, b, &tmp);
    out->x = tmp.x;
    out->y = tmp.y;
    out->z = tmp.z;
}

void func_0203cebc(Vector3 *out, Vector3 *tmp, Vector3 *a, Vector3 *b) {
    CrossVec3(a, b, tmp);
    out->x = tmp->x;
    out->y = tmp->y;
    out->z = tmp->z;
}

/* func_0203ce80 at 0x0203ce80, size=0x3c
 * Normalizes *src in-place (NormalizeVec3(src,src)), then copies to *dst field-by-field.
 */
// @symbol func_0203ce80
void func_0203ce80(struct Vector3 *dst, struct Vector3 *src) {
    NormalizeVec3(src, src);
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}
