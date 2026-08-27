// @symbol func_ov002_020ae844
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;

extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);

int func_ov002_020ae844(void *c, void *a) {
    s16 angle = Vec3_HorzAngle((struct Vector3 *)((char *)a + 0x5c), (struct Vector3 *)((char *)c + 0x5c));
    *(short *)((char *)c + 0x94) = angle;
    return func_ov002_020ae968(c, a);
}
