// @symbol func_02009374
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
/* decl_common.h calls this Vec3_DistSq and records the address in a comment, but no
   module defines that name -- the symbol at 0x0203cf94 is func_0203cf94, and Vec3_Dist
   is a different function 0x48 bytes later. The conjecture reads well and may even be
   right; it is kept here as a comment rather than as a reference nothing can resolve. */
extern long long func_0203cf94(const struct Vector3*, const struct Vector3*); /* "Vec3_DistSq" */
/* func_02009374 @ 0x02009374, size 0x60, ARM.
 * Camera member: scans the 12-slot STAR_MARKERS array; for each live marker,
 * computes the squared 3D distance (Fix12, 64-bit) from the camera's ownerPos
 * (Camera+0x98) to the marker actor's pos (dActor_c+0x5c). Returns 1 (true) as soon
 * as any marker is within threshold (distSq < 0x40000000), else 0.
 */

typedef int Fix12i; /* 20.12 fixed point */



/* Marker actor: only its world position (dActor_c::pos @ 0x5c) is touched here. */
struct dActor_c {
    char pad[0x5c];
    struct Vector3 pos; /* 0x5c */
};

/* Camera: only ownerPos (@ 0x98) is touched here. */
struct Camera {
    char pad[0x98];
    struct Vector3 ownerPos; /* 0x98 */
};

extern struct dActor_c *STAR_MARKERS[12];                  /* 0x0209f40c */

int func_02009374(struct Camera *self) {
    int i;
    for (i = 0; i < 12; i++) {
        struct dActor_c *marker = STAR_MARKERS[i];
        if (marker != 0) {
            if (func_0203cf94(&self->ownerPos, &marker->pos) < 0x40000000LL) {
                return 1;
            }
        }
    }
    return 0;
}
