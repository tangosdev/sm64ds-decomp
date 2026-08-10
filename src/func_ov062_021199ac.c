#include "types.h"
// @symbol func_ov062_021199ac
/* recovered: shared common types */
#include "common.h"
extern void *_ZN5Actor18ClosestWithActorIDEj(void *thiz, u32 id);

extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern s32 Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern s32 _ZN5Actor14GetSubtractionEss(void *thiz, s16 a, s16 b);

extern s16 data_02082214[];

int func_ov062_021199ac(char *self)
{
    char *other;
    s16 angle;
    s32 dist;
    s16 selfAngle;
    s16 otherAngle;
    u16 idx;
    s16 tableVal;
    s32 otherK;
    s32 fixed1;
    s32 r;

    other = (char *)_ZN5Actor18ClosestWithActorIDEj(self, 0xdc);
    if (other == 0)
        goto ret0;
    if (*(u8 *)(other + 0x3d0) != 2)
        goto ret0;

    angle = Vec3_HorzAngle((const struct Vector3 *)(self + 0x5c), (const struct Vector3 *)(other + 0x5c));
    dist = Vec3_Dist((const struct Vector3 *)(self + 0x5c), (const struct Vector3 *)(other + 0x5c));
    selfAngle = *(s16 *)(self + 0x94);
    otherAngle = *(s16 *)(other + 0x94);
    idx = (u16)(s16)(otherAngle - selfAngle);
    tableVal = data_02082214[(idx >> 4) * 2 + 1];
    otherK = *(s32 *)(other + 0x98);
    fixed1 = (s32)(((long long)otherK * tableVal + 0x800) >> 12);

    r = _ZN5Actor14GetSubtractionEss(self, selfAngle, angle);
    if (r < 0x4000) {
        s32 selfK;
        s32 fixed2;
        if (dist >= 0x190000)
            goto ret0;
        selfK = *(s32 *)(self + 0x98);
        fixed2 = (s32)(((long long)selfK * 0xb33 + 0x800) >> 12);
        if (fixed1 < fixed2)
            return 1;
        *(s32 *)(self + 0x98) -= 0x2000;
        goto ret0;
    } else {
        if (dist >= 0x12c000)
            goto ret0;
        if (fixed1 > *(s32 *)(self + 0x98))
            return -1;
        goto ret0;
    }

ret0:
    return 0;
}
