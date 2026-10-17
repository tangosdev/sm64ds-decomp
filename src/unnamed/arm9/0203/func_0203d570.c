#include "nitro/fx/fx.h"
struct V2 { int x; int y; };
int func_0203d570(struct V2* a, struct V2* b) {
    return FX_MUL(a->x, b->x) + FX_MUL(a->y, b->y);
}
