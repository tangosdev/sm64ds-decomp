// @symbol func_ov091_02133f60
/* recovered: shared common types */
#include "common.h"
void *_ZN5Actor13ClosestPlayerEv(void *thiz);
short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
int ApproachAngle(short *cur, int target, int a, int b, int c);
int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
int func_ov091_02134044(void *c, void *p);

extern char data_ov091_021356b0[];

int func_ov091_02133f60(char *c)
{
    void *pl = _ZN5Actor13ClosestPlayerEv(c);
    struct Vector3 v;
    short ang;
    struct Vector3 *src;
    if (pl == 0) goto done;

    src = (struct Vector3*)(((int)pl + 0x5c));
    v.x = src->x;
    v.y = src->y;
    v.z = src->z;
    ang = Vec3_HorzAngle((struct Vector3*)(c + 0x5c), &v);
    ApproachAngle((short*)(c + 0x94), ang, 0xa, 0x200, 0x100);
    if (*(unsigned short*)(c + 0x100) != 0) goto done;
    if (Vec3_Dist((struct Vector3*)(c + 0x5c), &v) >= 0x3e8000) goto done;
    if (ang >= 0x1000) goto done;
    func_ov091_02134044(c, data_ov091_021356b0);
done:
    return 1;
}
