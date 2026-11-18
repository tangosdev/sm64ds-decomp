#ifndef SM64DS_VECTOR2MATH_H
#define SM64DS_VECTOR2MATH_H

#include "types.h"

struct Vector2;

#ifdef __cplusplus
extern "C" {
#endif
fx32 Vec2_Len(const Vector2 *v);
int func_0203d524(const Vector2 *, const Vector2 *);
void func_0203d704(int *, int *, int *);
void Vec2_Sub(int *, int *, int *);
#ifdef __cplusplus
}
#endif

#endif
