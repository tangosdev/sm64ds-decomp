/* Shared value types, consolidated from thousands of duplicate inline
 * declarations across the loose function files. */
#ifndef COMMON_H
#define COMMON_H
#include "types.h"

/* Guarded on the same principle as Vector3_16 below: math/Matrix.h defines this
 * type too, structured as `Matrix3x3 r; Vector3 t;` rather than flat, and any
 * header reaching Model.h -> ModelBase.h -> math/Matrix.h brings it in. Both
 * spellings are 0x30 bytes and both are in real use -- Matrix4x3_ApplyInPlace*
 * wants `m[i]`, the model headers want `.r`/`.t` -- so neither can simply win.
 * Whichever is seen FIRST in a translation unit stands, and the other stands
 * down. Without this a TU that reaches both fails outright, which is what blocked
 * typing Model/ModelAnim members into the actor headers. */
#ifndef MATRIX4X3_DEFINED
#define MATRIX4X3_DEFINED
struct Matrix4x3 { s32 m[12]; };
#endif
/* Vector3 lives in types.h (as {Fix12i x,y,z}); not redefined here. */
/* Guarded so MeshColliderBase.h, which needs this type and cannot assume common.h
 * was included first, can define it too without colliding. Both spell the guard
 * VECTOR3_16_DEFINED; whichever is seen first wins and the other stands down. */
#ifndef VECTOR3_16_DEFINED
#define VECTOR3_16_DEFINED
struct Vector3_16 { s16 x, y, z; };
#endif

typedef struct Matrix4x3 Matrix4x3;
typedef struct Vector3_16 Vector3_16;

#endif
