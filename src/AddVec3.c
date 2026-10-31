/* One of the arm9 Vector3 helpers: c = a + b, per axis. C linkage, like the
   rest of the library (src/Vec3_Lsl.c, src/Vec3_HorzAngle.cpp); this being a
   C file is what makes Vector3 the plain triple here, since include/types.h
   guards its constructors and destructor with __cplusplus. */
#include "common.h"

void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c) {
    c->x = a->x + b->x;
    c->y = a->y + b->y;
    c->z = a->z + b->z;
}
