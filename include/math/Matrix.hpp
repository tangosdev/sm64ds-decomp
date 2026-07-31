#ifndef SM64DS_MATH_MATRIX_HPP
#define SM64DS_MATH_MATRIX_HPP

#include "types.h"

/*
 * Fixed-point matrix ABI used by the game's C and C++ interfaces.
 *
 * The type names come from the game's mangled symbols. The element counts and
 * Matrix4x3 translation offsets are corroborated by the matched matrix
 * constructors and vector-transform routines.
 */
typedef struct Matrix3x3 {
    Fix12i m[9];
} Matrix3x3;

typedef struct Matrix4x3 {
    Fix12i m[12];
} Matrix4x3;

#define MATRIX4X3_TRANSLATION_X 9
#define MATRIX4X3_TRANSLATION_Y 10
#define MATRIX4X3_TRANSLATION_Z 11

typedef char Matrix3x3_size_must_be_0x24[
    sizeof(Matrix3x3) == 0x24 ? 1 : -1
];
typedef char Matrix4x3_size_must_be_0x30[
    sizeof(Matrix4x3) == 0x30 ? 1 : -1
];

#endif /* SM64DS_MATH_MATRIX_HPP */
