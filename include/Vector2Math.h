#ifndef SM64DS_VECTOR2MATH_H
#define SM64DS_VECTOR2MATH_H
#ifndef __cplusplus
#define Vec2_Len _Z8Vec2_LenPK7Vector2
#endif

#include "types.h"

struct Vector2;
struct V2;

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif
Fix12i Vec2_Len(const Vector2 *v);
/* These three are C definitions. Vec2_Len is the C++ symbol. */
#ifdef __cplusplus
#ifndef _MSC_VER
extern "C"
#endif
#endif
    int
    func_0203d524(struct V2 *, struct V2 *);
#ifdef __cplusplus
#ifndef _MSC_VER
extern "C"
#endif
#endif
    void
    func_0203d704(int *, int *, int *);
#ifdef __cplusplus
#ifndef _MSC_VER
extern "C"
#endif
#endif
    void
    Vec2_Sub(int *, int *, int *);
#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif
