//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "cstd.h"

extern "C" {

/* Vector3 declares a constructor and destructor under C++, which makes its
 * assignment copy word by word. The ROM copies these with C's ldm/stm block
 * move, which C++ keeps for a struct with an array member. */
typedef struct Vec3Words {
    Fix12i w[3];
} Vec3Words;

extern int func_020531a4(int a);

typedef struct S {
    Vector3 *p;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
} S;

int func_ov007_020c704c(S *self, Vector3 *b, int r2arg, int r3arg, int radius) {
    int dx, dy, sum, spd, ratio;
    s64 sdx, sdy;

    dx = b->x - self->p->x;
    dy = b->y - self->p->y;
    sum = FX_MUL(dx, dx) + FX_MUL(dy, dy);
    sdx = dx;
    sdy = dy;

    if (sum <= FX_MUL(radius, radius)) {
        self->f18 = 0;
        self->f14 = self->f18;
        self->f10 = self->f14;
        self->fc = self->f10;
        self->f8 = self->fc;
        self->f4 = self->f8;
        *(Vec3Words *)self->p = *(Vec3Words *)b;
        return 2;
    }

    if (sum <= FX_MUL(r3arg, r3arg)) {
        spd = func_020531a4(sum);
        ratio = cstd::fdiv(r2arg, spd);

        self->f10 += FX_MUL(ratio, sdx);
        self->f14 += FX_MUL(ratio, sdy);
        return 1;
    }
    return 0;
}
}
