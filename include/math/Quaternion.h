#ifndef SM64DS_MATH_QUATERNION_H
#define SM64DS_MATH_QUATERNION_H

#include "types.h"

/* The quaternion Matrix3x3_FromQuaternion and Matrix4x3_FromQuaternion take:
 * four Fix12i in x, y, z, w order, as the two matrix builders read them. */
struct Quaternion {
    Fix12i x, y, z, w;
};
typedef char Quaternion_size_must_be_0x10[sizeof(struct Quaternion) == 0x10 ? 1 : -1];

#endif
