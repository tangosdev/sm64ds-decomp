// @symbol func_ov064_021166f0
/* recovered: shared common types */
#include "common.h"
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern void _Z14ApproachLinearRsss(short *a, short b, short c);
struct dActor_c { char pad; };
extern int _ZN8dActor_c15IsPlayerInRangeERK7Vector3i(struct dActor_c *t, const struct Vector3 *v, int i);
int func_ov064_021166f0(char *t)
{
    struct Vector3 v;
    *(short *)(t + 0x3e4) = Vec3_HorzAngle((struct Vector3 *)(t + 0x5c), (struct Vector3 *)(t + 0x3a8));
    _Z14ApproachLinearRsss((short *)(t + 0x94), *(short *)(t + 0x3e4), 0x140);
    v.x = *(int *)(t + 0x3a8);
    v.y = *(int *)(t + 0x3ac);
    v.z = *(int *)(t + 0x3b0);
    return _ZN8dActor_c15IsPlayerInRangeERK7Vector3i((struct dActor_c *)t, &v, 0x320);
}
