/* Shared value types, consolidated from thousands of duplicate inline
 * declarations across the loose function files. */
#ifndef COMMON_H
#define COMMON_H
#include "types.h"

struct Matrix4x3 { s32 m[12]; };
/* Vector3 lives in types.h (as {Fix12i x,y,z}); not redefined here. */
struct Vector3_16 { s16 x, y, z; };

typedef struct Matrix4x3 Matrix4x3;
typedef struct Vector3_16 Vector3_16;

#endif
