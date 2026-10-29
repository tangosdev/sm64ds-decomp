#include "nitro/fx/fx.h"

typedef struct {
    int x, y;
} Vec2_Fix12;

void func_0203d680(Vec2_Fix12 *out, const Vec2_Fix12 *in, int scale) {
    int rx = FX_MUL(in->x, scale);
    int ry = FX_MUL(in->y, scale);
    out->x = rx;
    out->y = ry;
}

void func_0203d630(int *p, int m) {
    p[0] = FX_MUL(p[0], m);
    p[1] = FX_MUL(p[1], m);
}
