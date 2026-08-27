// @symbol func_02037a38
/* recovered: shared common types */
#include "common.h"
struct Obj { char pad[0x4c]; struct Vector3 d; struct Vector3 a; struct Vector3 b; };

void func_02037a38(struct Obj *o)
{
    o->d.x = o->a.x + o->b.x;
    o->d.y = o->a.y + o->b.y;
    o->d.z = o->a.z + o->b.z;
}
