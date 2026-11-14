#include "Vector2.h"
#include "nitro/fx/fx.h"

/* Scale a Vector2 by an fx32 factor into out (Vector2.h). Callers include
   dScMgCard_c and the hanachan minigame. */
void func_0203d680(Vector2 *out, const Vector2 *in, int scale) {
    int rx = FX_MUL(in->x, scale);
    int ry = FX_MUL(in->y, scale);
    out->x = rx;
    out->y = ry;
}

/* Scale a Vector2 by an fx32 factor in place (Vector2.h); e.g. the amida
   and base minigame scenes damp velocities with it, and hanachan spaces
   its segments. */
void func_0203d630(int *p, int m) {
    p[0] = FX_MUL(p[0], m);
    p[1] = FX_MUL(p[1], m);
}
