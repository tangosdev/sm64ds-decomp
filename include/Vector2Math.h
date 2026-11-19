#ifndef SM64DS_VECTOR2MATH_H
#define SM64DS_VECTOR2MATH_H

#include "types.h"

struct Vector2;

#ifdef __cplusplus
extern "C" {
#endif
void func_0203d388(Vector2 *, s16);
int func_0203d434(Vector2 *);
void func_0203d480(Vector2 *, Vector2 *);
fx32 Vec2_Len(const Vector2 *v);
void func_0203d630(Vector2 *, fx32);
void func_0203d680(Vector2 *, const Vector2 *, int);
int func_0203d524(const Vector2 *, const Vector2 *);
void func_0203d704(Vector2 *, Vector2 *, Vector2 *);
void Vec2_Sub(Vector2 *, Vector2 *, Vector2 *);
long long func_0203d5bc(Vector2 *);
#ifdef __cplusplus
}

fx32 func_0203d5dc(const Vector2 *, const Vector2 *);
#endif

#endif
