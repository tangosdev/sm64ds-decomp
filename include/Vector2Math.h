#ifndef SM64DS_VECTOR2MATH_H
#define SM64DS_VECTOR2MATH_H
#ifndef __cplusplus
#define Vec2_Len _Z8Vec2_LenPK7Vector2
#endif

#include "types.h"

struct Vector2;
struct V2;

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif
Fix12i Vec2_Len(const Vector2 *v);
int func_0203d524(struct V2 *, struct V2 *);
void func_0203d704(int *, int *, int *);
void Vec2_Sub(int *, int *, int *);
#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif
