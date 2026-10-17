#include "nitro/fx/fx.h"
struct V2 { int x; int y; };
int func_0203d524(struct V2* a, struct V2* b) {
    return FX_MUL(a->x, b->y) - FX_MUL(a->y, b->x);
}
