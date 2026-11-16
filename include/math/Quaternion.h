#ifndef SM64DS_MATH_QUATERNION_H
#define SM64DS_MATH_QUATERNION_H

#include "types.h"

struct Matrix4x3;

/* The quaternion Matrix3x3_FromQuaternion and Matrix4x3_FromQuaternion take:
 * four fx32 in x, y, z, w order, as the two matrix builders read them. */
struct Quaternion {
    fx32 x, y, z, w;
};
typedef char Quaternion_size_must_be_0x10[sizeof(struct Quaternion) == 0x10 ? 1 : -1];

/* The arm9 quaternion helpers. Their definitions work on four ints; the C
 * files spell the FromVector3 and Multiply storage with a local Quat type, so
 * those two take the address as void * here. */
#ifdef __cplusplus
extern "C" {
#endif

void Quaternion_FromVector3(void *, Vector3 *, Vector3 *);
void Quaternion_Normalize(int *);
void Quaternion_SLerp(int *, int *, int, int *);
void Quaternion_Multiply(void *, void *, void *);
void Matrix4x3_FromQuaternion(const struct Quaternion *, struct Matrix4x3 *);

#ifdef __cplusplus
}
#endif

#endif
