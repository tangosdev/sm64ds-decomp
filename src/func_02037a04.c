// @symbol func_02037a04
/* recovered: shared common types */
#include "common.h"
struct Obj { char pad[0x58]; struct Vector3 a; struct Vector3 b; };

void func_02037a04(struct Obj *o, struct Vector3 *d1, struct Vector3 *d2)
{
    d1->x = o->a.x;
    d1->y = o->a.y;
    d1->z = o->a.z;
    d2->x = o->b.x;
    d2->y = o->b.y;
    d2->z = o->b.z;
}
