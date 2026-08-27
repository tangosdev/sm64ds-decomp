#include "types.h"
typedef struct { int x, y; } V2;

extern void func_ov006_021146cc(int *a, int *b);
extern void func_ov006_0211470c(int *a, int *b);
extern void Vec2_Sub(V2 *o, V2 *a, V2 *b);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int func_ov006_02111d6c(u8 *p);
extern void func_ov006_021146e0(V2 *a, u8 *b);
extern void func_0203d388(V2 *p, s16 angle);

void func_ov006_02115830(int a, int b, int c, u8 *d, u8 *e)
{
    V2 delta;
    V2 A;
    V2 B;
    V2 vec1;
    V2 vec2;
    V2 z1;
    V2 z2;
    V2 tmp;
    V2 t1;
    V2 t2;
    V2 c1;
    V2 c2;
    s16 ang;
    int neg;

    if (b == c) {
        return;
    }
    if (*(int *)(d + 0xf8) != c) {
        return;
    }
    if (b > c) {
        if (*(int *)(e + 0xf8) == b) {
            return;
        }
    }
    if (*(u8 *)(d + 0x110) == 1 || *(u8 *)(e + 0x110) == 1) {
        z1.x = 0;
        z1.y = 0;
        func_ov006_021146cc((int *)d, &z1.x);
        z2.x = 0;
        z2.y = 0;
        func_ov006_021146cc((int *)e, &z2.x);
        *(u8 *)(d + 0x3b) = 1;
        *(u8 *)(e + 0x3b) = 1;
        return;
    }
    if (*(int *)(d + 0xfc) > 0 || *(int *)(e + 0xfc) > 0) {
        *(int *)(d + 0xfc) = 3;
        *(int *)(e + 0xfc) = 3;
        return;
    }
    *(int *)(d + 0xfc) = 3;
    *(int *)(e + 0xfc) = 3;
    func_ov006_0211470c(&A.x, (int *)d);
    func_ov006_0211470c(&B.x, (int *)e);
    Vec2_Sub(&tmp, &A, &B);
    delta = tmp;
    ang = _ZN4cstd5atan2E5Fix12IiES1_(delta.y, delta.x);
    if (func_ov006_02111d6c(d) != 0) {
        vec1.x = 0;
        vec1.y = 0;
    } else {
        func_ov006_021146e0(&t1, d);
        vec1 = t1;
    }
    if (func_ov006_02111d6c(e) != 0) {
        vec2.x = 0;
        vec2.y = 0;
    } else {
        func_ov006_021146e0(&t2, e);
        vec2 = t2;
    }
    neg = -ang;
    func_0203d388(&vec1, neg);
    func_0203d388(&vec2, neg);
    {
        int t = vec1.x;
        vec1.x = vec2.x;
        vec2.x = t;
    }
    func_0203d388(&vec1, ang);
    func_0203d388(&vec2, ang);
    c1 = vec1;
    func_ov006_021146cc((int *)d, &c1.x);
    c2 = vec2;
    func_ov006_021146cc((int *)e, &c2.x);
}
