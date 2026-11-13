#ifndef SM64DS_VECTOR2MATH_H
#define SM64DS_VECTOR2MATH_H
#ifndef __cplusplus
#define Vec2_Len _Z8Vec2_LenPK7Vector2
#endif

#include "types.h"

struct Vector2;

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif
Fix12i Vec2_Len(const Vector2 *v);
#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif
