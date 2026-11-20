#include "Vector2.h"
#include "Vector2Math.h"

void Vec2_Sub(Vector2 *o, Vector2 *a, Vector2 *b) {
    int ax = a->x, bx = b->x, ay = a->y, by = b->y;
    o->x = ax - bx;
    o->y = ay - by;
}
