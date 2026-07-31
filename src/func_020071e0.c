#include "types.h"
typedef struct { int x, y, z; } Vec3;

extern void ApproachLinear(int *val, int target, int step);
extern void Vec3_RotateYAndTranslate(Vec3 *out, const Vec3 *trans, short ang, const Vec3 *add);
extern void func_02007c9c(const Vec3 *v0, const Vec3 *v1, int *outDist, short *outVertAng, short *outHorzAng);
extern void func_02007c14(Vec3 *res, const Vec3 *trans, int mag, short ang, short angY);
extern int func_02007cec(void *a, void *b, int c);
extern void func_02007d0c(Vec3 *self, Vec3 *src, int sx, int sy, int sz);

int func_020071e0(char *self) {
    Vec3 va, vb;
    int dist;
    short ang, angY;
    ApproachLinear((int*)(self + 0x170), 0x5a000, 0x2800);
    {
        char *o = *(char**)(self + 0x110);
        short a = (short)(*(short*)(o + 0x8e) + 0x8000);
        const Vec3 *trans = (const Vec3*)(o + 0x5c);
        Vec3_RotateYAndTranslate(&va, trans, a, (const Vec3*)(self + 0xb0));
        Vec3_RotateYAndTranslate(&vb, trans, a, (const Vec3*)(self + 0xbc));
    }
    func_02007c9c(&va, (const Vec3*)(self + 0x8c), &dist, &ang, &angY);
    ApproachLinear(&dist, 0, *(int*)(self + 0x170));
    func_02007c14((Vec3*)(self + 0x8c), &va, dist, ang, angY);
    func_02007cec(self + 0x8c, &va, 0x28);
    func_02007d0c((Vec3*)(self + 0x80), &vb, 0x800, 0xccc, 0x800);
    return 1;
}
