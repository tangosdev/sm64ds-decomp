#include "Vector2.h"
#include "Vector2Math.h"

/* A Vector2's squared length, in 64 bits and shifted down by 12
   (Vector2.h). */
long long func_0203d5bc(Vector2 *p) {
    long long s = (long long)p->x * p->x;
    s += (long long)p->y * p->y;
    return s >> 12;
}
