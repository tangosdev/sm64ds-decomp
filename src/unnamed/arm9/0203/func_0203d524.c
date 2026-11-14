#include "nitro/fx/fx.h"
struct V2 { int x; int y; };
/* The 2D cross product a.x * b.y - a.y * b.x in fx32 (Vector2.h): which
   side of a the vector b lies on. */
int func_0203d524(struct V2* a, struct V2* b) {
    return FX_MUL(a->x, b->y) - FX_MUL(a->y, b->x);
}
