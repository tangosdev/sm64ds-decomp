// @symbol func_02037eb0
/* recovered: shared common types */
#include "common.h"
struct AB { int a; int b; };
struct Obj { int a; int b; struct Vector3 v; };

void func_02037eb0(struct Obj *o, struct AB ab, struct Vector3 *v)
{
    o->a = ab.a;
    o->b = ab.b;
    o->v.x = v->x;
    o->v.y = v->y;
    o->v.z = v->z;
}
