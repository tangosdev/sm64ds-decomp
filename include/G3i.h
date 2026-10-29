/* Hand-edited, against evidence. G3i has no instance state: both of the
 * class's functions write the geometry engine's matrix ports through literal
 * hardware addresses (0x4000440 selects the matrix mode, 0x4000458/0x400045c
 * stream the projection/position rows) and divide through the 64-bit math
 * coprocessor at 0x4000290. Neither takes a `this` -- every parameter comes
 * from the mangled name, the same shape as include/G2x.h and include/OAM.h.
 *
 * PerspectiveW_ is declared below with six Fix12i scalars (they mangle as
 * int), the SDK's BOOL load flag as an int, and a Matrix4x3 *. With a bool
 * flag the definition does not reproduce the ROM body; with int it does.
 * Parameters are unnamed: a named parameter renumbers @NNN in every
 * includer.
 */
#ifndef G3I_H
#define G3I_H
#include "types.h"

struct Matrix4x3;

struct G3i {
#ifdef __cplusplus
    /* All static -- see the header note. */
    static void LookAt_(const Vector3 *camPos, const Vector3 *up, const Vector3 *target, bool draw,
                        Matrix4x3 *mat);
    static void PerspectiveW_(Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, int, Matrix4x3 *);
#endif
};

#endif
