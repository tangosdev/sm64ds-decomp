/* One of the arm9 Vector3 helpers: c = a - b, per axis. See src/AddVec3.c on
   why a C file sees Vector3 as a plain triple. */
#include "common.h"

void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c) {
    c->x = a->x - b->x;
    c->y = a->y - b->y;
    c->z = a->z - b->z;
}
