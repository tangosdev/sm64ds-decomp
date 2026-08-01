//cpp
/* func_02038824 @ 0x02038824 (arm9, size 0x214)   [mwccarm 2004/b56]
 * Lever (notes 6ar): the hoisted address local is declared `int pos = (int)self + 0x3c;`,
 * NOT `char *pos = self + 0x3c;`. The local's DECLARED TYPE is the lever, not a cast on
 * the expression: keeping a pointer local and moving the arithmetic into it
 * (`char *pos = (char *)((int)self + 0x3c);`) only reaches 12. Retyping the local to int
 * flips the r4/r5 two-web swap (pos -> r4, the shared distance-temp/flags web -> r5) and
 * the 9-word prologue schedule cluster falls out with it. Do not "clean up" pos back to a
 * pointer or drop the (void*) cast at the Vec3_Dist call.
 * Ordering levers cannot reach this: pos is LICM-hoisted, so its web is pinned to the rank
 * of its ORIGINAL scope and cannot be pushed below an in-loop temp by decl-order or
 * scope-depth edits. The 6y/6aq booster family is entirely inert here.
 * Also takes 1.2/base|sp2|sp2p3 from 25 divergences to 5, so it is a better source shape
 * rather than a b56 artifact. Link-verified: VERIFIED, 0 diffs, 0 blind.
 */
struct Vec3 { int x, y, z; };

struct A {
    virtual int f0();
    virtual int f1();
    virtual int f2();
    virtual int f3();
    virtual int f4();
    virtual int f5();
    virtual int f6();
    virtual int f7();
    virtual int f8(void*);
};

struct B {
    char _pad0[0x5c];
    struct Vec3 pos;
    char _pad1[0xb0 - 0x68];
    int vb0;
    int vb4;
    int vb8;
};

extern "C" struct B *func_020393b4(struct A *p);
extern "C" int func_020393ac(struct A *p);
extern "C" int func_0203939c(struct A *p);
extern "C" int func_0203938c(struct A *p);
extern "C" int func_02035354(char *a, struct B *b);
extern "C" int Vec3_Dist(const void *a, const void *b);
extern "C" void func_02037fec(char *c, int i, int p2, struct B *p3, struct A *p4);
extern "C" void func_020379d0(char *c, int i, int d, struct B *e, struct A *f);
extern "C" void func_0203799c(char *c, int i, int d, struct B *e, struct A *f);
extern "C" void func_02037968(char *c, int i, int d, struct B *e, struct A *f);
extern "C" void func_02037a38(char *o);
extern "C" struct A *data_020a0c80[];

extern "C" int func_02038824(char *self)
{
    int limit = *(int*)(self + 0x48) + 0x1000;
    int ret = 0;
    int zero = 0;
    int one = 1;
    int i;
    for (i = 1; i < 0x18; i++) {
        struct A *o = data_020a0c80[i];
        if (o == 0) continue;
        struct B *sl = func_020393b4(o);
        if (func_02035354(self, sl) != 0) continue;
        if (sl != 0 && ((sl->vb0 & 2) ? one : zero)) {
            int pos = (int)self + 0x3c;
            struct Vec3 v;
            struct Vec3 *src = &sl->pos;
            int r5;
            v.x = src->x;
            v.y = src->y;
            v.z = src->z;
            r5 = func_0203939c(o);
            if (r5 == -0x1000) {
                int hi = sl->vb8;
                v.y = v.y + sl->vb4;
                r5 = hi << 3;
            } else {
                v.y = v.y + func_0203938c(o);
            }
            if (Vec3_Dist((void*)pos, &v) > r5 + limit) continue;
        }
        int flags = o->f8(self);
        if (flags == 0) continue;
        func_02037fec(self + 0x10, i, func_020393ac(o), func_020393b4(o), o);
        if (flags & 1) func_020379d0(self, i, func_020393ac(o), func_020393b4(o), o);
        if (flags & 2) func_0203799c(self, i, func_020393ac(o), func_020393b4(o), o);
        if (flags & 4) func_02037968(self, i, func_020393ac(o), func_020393b4(o), o);
        ret = one;
    }
    func_02037a38(self);
    return ret;
}
