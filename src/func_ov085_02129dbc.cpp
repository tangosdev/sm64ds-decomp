//cpp
#include "common.h"

extern "C" {
extern void *_ZN8dActor_c13ClosestPlayerEv(void *c);
extern s32 Vec3_HorzDist(const void *a, const void *b);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern s16 Vec3_VertAngle(const void *a, const void *b);
extern s32 AngleDiff(s32 a, s32 b);
extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 rate);

void func_ov085_02129dbc(char *s)
{
    char *p = (char *)_ZN8dActor_c13ClosestPlayerEv(s);
    if (p == 0)
        return;

    Vector3 *psrc = (Vector3 *)(p + 0x5c);
    Vector3 v;
    v.x = psrc->x;
    v.y = psrc->y;
    v.z = psrc->z;

    s32 hd = Vec3_HorzDist(s + 0x5c, &v);
    v.y = v.y - 0x1e000;
    s16 ha = Vec3_HorzAngle(s + 0x5c, &v);
    s16 va = Vec3_VertAngle(s + 0x5c, &v);

    if (hd < 0x15e000 && AngleDiff(ha, *(s16 *)(s + 0x8e)) < 0x3000) {
        *(s16 *)(s + 0x364) = va;
        *(s16 *)(s + 0x366) = *(s16 *)(s + 0x8e) - ha;
    } else {
        *(s16 *)(s + 0x364) = 0;
        *(s16 *)(s + 0x366) = 0;
    }
    _Z14ApproachLinearRsss((s16 *)(s + 0x362), *(s16 *)(s + 0x366), 0x250);
    _Z14ApproachLinearRsss((s16 *)(s + 0x360), *(s16 *)(s + 0x364), 0x100);
}
}
