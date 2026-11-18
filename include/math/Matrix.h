#ifndef SM64DS_MATH_MATRIX_H
#define SM64DS_MATH_MATRIX_H

#include "types.h"
#include "nitro/fx/fx.h"

/*
 * Fixed-point matrix ABI used by the game's C and C++ interfaces.
 *
 * The type names come from the game's mangled symbols. The element counts and
 * Matrix4x3 translation offsets are corroborated by the matched matrix
 * constructors and vector-transform routines.
 */
typedef struct Matrix3x3 {
    fx32 m[9];
} Matrix3x3;

/* A 4x3 matrix, row by row: the X, Y and Z axis rows (r, words 0-8) and then
 * the translation (t, words 9-11). Code that works on all twelve words at once
 * indexes m.
 *
 * A union, the way the SDK's own MtxFx43 is one (named elements overlaid on
 * the array), because that is what the cartridge shows:
 *  - Every whole-matrix copy is one twelve-word block (three ldm/stm of four).
 *    mwccarm copies a union as one block, but splits a struct copy at member
 *    boundaries, so a plain struct of a 3x3 and a vector copies nine words and
 *    then three.
 *  - The type is trivially destructible. daPiano_c's and daObjCvNewsLift_c's
 *    destructors leave their Matrix4x3 arrays alone, and
 *    __sinit_d_mg3d_esp_anim_set.cpp registers no destructor for its static
 *    one; a destructor would add a __cxa_vec_cleanup and 0x18 bytes to each
 *    of the first two and a registration to the third. So t is the SDK's POD
 *    VecFx32: the game's Vector3 declares a destructor, and a union cannot
 *    hold it. */
typedef union Matrix4x3 {
    fx32 m[12];
    struct {
        Matrix3x3 r;
        VecFx32 t;
    };
} Matrix4x3;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Matrix3x3_size_must_be_0x24[
    sizeof(Matrix3x3) == 0x24 ? 1 : -1
];
typedef char Matrix4x3_size_must_be_0x30[
    sizeof(Matrix4x3) == 0x30 ? 1 : -1
];
#endif

#endif /* SM64DS_MATH_MATRIX_H */
