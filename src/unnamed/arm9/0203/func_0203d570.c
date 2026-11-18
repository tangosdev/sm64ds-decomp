#include "nitro/fx/fx.h"
#include "Vector2.h"
/* The 2D dot product in fx32 (Vector2.h). */
int func_0203d570(const Vector2 *a, const Vector2 *b) {
    return FX_MUL(a->x, b->x) + FX_MUL(a->y, b->y);
}
