#include "nitro/fx/fx.h"
/* The 2D cross product a.x * b.y - a.y * b.x in fx32 (Vector2.h). The
   first argument is unused; dScMgSmartball_c passes one anyway. */
int func_0203d4d0(void *p, int *a, int *b)
{
    int t1 = FX_MUL(a[0], b[1]);
    int t2 = FX_MUL(a[1], b[0]);
    return t1 - t2;
}
