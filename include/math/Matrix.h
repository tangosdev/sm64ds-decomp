#ifndef SM64DS_MATH_MATRIX_H
#define SM64DS_MATH_MATRIX_H

#include "types.h"

/*
 * Fixed-point matrix ABI used by the game's C and C++ interfaces.
 *
 * The type names come from the game's mangled symbols. The element counts and
 * Matrix4x3 translation offsets are corroborated by the matched matrix
 * constructors and vector-transform routines.
 *
 * Matrix4x3 embeds Matrix3x3 rather than repeating the nine elements, so the
 * 3x3-taking routines can be handed the rotation part of a 4x3 by name instead
 * of by cast, and the translation reads as a Vector3 instead of as indices 9-11.
 * The layout is unchanged: rotation at 0x00, translation at 0x24.
 */
typedef struct Matrix3x3 {
    Fix12i m[9];
} Matrix3x3;

/* See include/common.h: this type has a second, flat spelling there. Same 0x30
 * bytes; whichever a translation unit sees first stands. */
#ifndef MATRIX4X3_DEFINED
#define MATRIX4X3_DEFINED
typedef struct Matrix4x3 {
    Matrix3x3 r;   /* 0x00 */
    Vector3   t;   /* 0x24 */
} Matrix4x3;
#endif

typedef char Matrix3x3_size_must_be_0x24[
    sizeof(Matrix3x3) == 0x24 ? 1 : -1
];
/* With `r` first and 0x24 bytes wide, these two also pin `t` to 0x24. */
typedef char Matrix4x3_size_must_be_0x30[
    sizeof(Matrix4x3) == 0x30 ? 1 : -1
];

#endif /* SM64DS_MATH_MATRIX_H */
