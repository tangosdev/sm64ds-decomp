/* HOST COPIES of four LakituBro (daC_Jugem_c, ov085) state bodies that each call
 * Actor::ClosestPlayer() with NO argument and rely on `this` riding ARM r0:
 *
 *     src/func_ov085_0212d9b8.c    (approach/spin state)
 *     src/func_ov085_0212dd10.cpp  (turn-the-player state)
 *     src/func_ov085_0212e310.cpp  (carry-toward state)
 *     src/func_ov085_0212e4a4.c    (the caught/grab state)
 *
 * THE r0-PASSTHROUGH SEAM -- identical to Actor_ClosestPlayerWrappers.cpp and the
 * RABBIT's Ov085_Rabbit_b8dc.cpp on the sibling lane:
 *
 * Each of these bodies is a LakituBro state whose own `this` (the cameraman, ROM
 * r0) is its first parameter. It calls Actor::ClosestPlayer(), and the matched
 * src spells that call with no argument -- byte-identical on ARM because
 * ClosestPlayer is a nonstatic member that reads `this` from r0, and the state
 * body's own r0 is still live across the `bl`.
 *
 * On the host the ClosestPlayer definition (src/_ZN5Actor13ClosestPlayerEv.c,
 * bridged in hal/reverse_bridges.cpp) is `(void *self)` -- cdecl, `self` off the
 * stack. The zero-argument call pushes nothing, so `self` is stack garbage and
 * ClosestPlayer's loop, Vec3_Dist((char*)self + 0x5c, (char*)player + 0x5c),
 * reads a garbage base + 0x5c. With the garbage happening to be 0 that is the
 * null-this +0x5c fault, the same class as the live rabbit crash. func_ov085_
 * 0212e4a4 is the site that was caught taking a leaked receiver off the stack on
 * an ordinary level-1 run: its `this` is not the intended cameraman but whatever
 * the previous frame left in the saved slot, so it happens to work until it does
 * not.
 *
 * THE FIX passes each body's own first parameter to ClosestPlayer -- exactly the
 * value the ROM leaves in r0. The bodies below are the matched sources line for
 * line; only the ClosestPlayer declaration (now one-arg) and its call site change.
 * Every other extern is kept EXACTLY as its source spells it (func_ov085_0212e728
 * in particular has a different arity in d9b8/e310/dd10 versus e4a4; each copy
 * keeps its own).
 *
 * The four byte-locked sources are commented out of slice_gate18.txt in favour of
 * these copies; the sources are unchanged.
 */
#include "decl_common.h"
#include "common.h"

/* ---- func_ov085_0212d9b8 -------------------------------------------------- */
extern "C" {
void* _ZN5Actor13ClosestPlayerEv(void* self);   /* real one-arg (this) shape */
void _Z14ApproachLinearRiii(int* p, int a, int b);
/* _Z14ApproachLinearRsss demangles to ApproachLinear(short&, short, short); one
   shared decl for the dd10 and e310 bodies below. */
void _Z14ApproachLinearRsss(short& cur, short tgt, short step);
int func_ov002_020c3ea0(void* c);
int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
}
/* func_ov085_0212e728 and _Z14ApproachLinearRsss are declared in decl_common.h
   (int(void*,void*) and (short&,short,short) respectively); use those canonical
   decls -- redeclaring them here would collide (C2371). The matched sources spelled
   their own local prototypes; the real ROM symbol arity is the header's. */
extern short data_02082214[];
extern "C" short data_ov085_02130820;

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing state; the host passes c).
extern "C" int func_ov085_0212d9b8(char* c)
{
    void* pl = _ZN5Actor13ClosestPlayerEv(c);   /* <-- this, the ROM's r0 */
    if (pl == 0) return 1;

    *(int*)(((int)c + 0x2c8)) += 1;
    *(int*)(((int)c + 0x2cc)) += 0x500;

    {
        int v = (short)*(int*)(c + 0x2cc);
        int idx = (int)((unsigned int)(v << 16) >> 16) >> 4;
        short e = data_02082214[idx * 2];
        _Z14ApproachLinearRiii((int*)(c + 0x60),
            (int)(((long long)e * 0x1a000 + 0x800) >> 12) + *(int*)(c + 0x2a8),
            0x10000000);
    }

    if (*(int*)(c + 0x2c8) == 0x32) {
        func_ov002_020c3ea0(pl);
    }
    *(short*)(c + 0x8e) = 0x6000;
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
    {
        int a1 = 0x4000;
        if (*(int*)(c + 0x2c8) >= 0x4b) a1 = 0x8000;
        _Z14ApproachLinearRiii((int*)(c + 0x98), a1, 0x1000);
    }

    *(int*)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned int*)(c + 0x2e4), 3, 0x182, (void*)(c + 0x74), 0);

    if (*(int*)(c + 0x2c8) > 0x78) {
        *(short*)(c + 0x8e) = 0x4000;
        *(short*)(c + 0x94) = *(short*)(c + 0x8e);
        func_ov085_0212e728(c, &data_ov085_02130820);
    }
    return 1;
}

/* ---- func_ov085_0212dd10 -------------------------------------------------- */
extern "C" {
void func_ov002_020c3f18(char* p);
extern char data_ov085_021307c0[];
}

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing state; the host passes c).
extern "C" int func_ov085_0212dd10(char* c)
{
    char* p = (char*)_ZN5Actor13ClosestPlayerEv(c);   /* <-- this, the ROM's r0 */
    if (!p) return 1;
    {
        short v = *(short*)(c + 0x8e);
        short w = v + 0x8000;
        int* t = (int*)(((int)c + 0x2c8));
        *(short*)(p + 0x8c) = 0;
        *(short*)(p + 0x8e) = w;
        *(short*)(p + 0x90) = 0;
        *t += 1;
    }
    if (*(int*)(c + 0x2c8) == 0x5a) func_ov002_020c3f18(p);
    {
        int s = *(int*)(c + 0x2c8);
        if (s > 0x57 && s < 0x5b)
            _Z14ApproachLinearRsss(*(short*)(c + 0x8c), 0x2000, 0x400);
        else
            _Z14ApproachLinearRsss(*(short*)(c + 0x8c), 0x1000, 0x400);
    }
    if (*(int*)(c + 0x2c8) > 0x78)
        func_ov085_0212e728(c, data_ov085_021307c0);
    return 1;
}

/* ---- func_ov085_0212e310 -------------------------------------------------- */
extern "C" {
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fix, bool loop);
void Matrix4x3_FromRotationY(struct Matrix4x3 *m, int angY);
void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out);
void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 *cur, const Vector3 *tgt, int step);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern struct Matrix4x3 data_020a0e68;
}

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing state; the host passes c).
extern "C" int func_ov085_0212e310(char *c)
{
    Vector3 in;
    Vector3 out;
    char *p;

    p = (char *)_ZN5Actor13ClosestPlayerEv(c);   /* <-- this, the ROM's r0 */
    if (p == 0) {
        return 1;
    }

    *(int *)(p + 0x744) = *(int *)(c + 0x5c);
    *(int *)(p + 0x748) = *(int *)(c + 0x60);
    *(int *)(p + 0x74c) = *(int *)(c + 0x64);

    if (*(unsigned short *)(c + 0x100) == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x14, 0x7f, 0x15666, 0);
    }

    *(unsigned int *)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int *)(c + 0x2e4), 3, 0x182, (Vector3 *)(c + 0x74), 0);

    in.x = 0;
    in.y = 0;
    in.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = 0xc8000;
    Matrix4x3_FromRotationY(&data_020a0e68, 0);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    out.x += *(int *)(p + 0x5c);
    out.y += *(int *)(p + 0x60) + 0x70000;
    out.z += *(int *)(p + 0x64);

    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)(c + 0x5c), &out, 0x14000);
    _Z14ApproachLinearRsss(*(short *)(c + 0x94), Vec3_HorzAngle((Vector3 *)(c + 0x5c), &out), 0x800);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);

    if (Vec3_Dist((Vector3 *)(c + 0x5c), &out) < 0x14000) {
        *(void **)(c + 0x2a0) = p;
        func_ov085_0212e728(c, data_ov085_02130810);
    }

    return 1;
}

/* ---- func_ov085_0212e4a4 (the caught/grab state, the leaked-receiver site) -- */
/* The matched src declared func_ov085_0212e728 as (int,int,int) and called it
   (self, ptr, 0); the real ROM symbol is (C*, PMF*) 2-arg and the trailing 0 is a
   cdecl caller-cleaned extra the callee never reads. Call it with the two live
   args, matching the real signature. */
extern "C" {
int _ZN6Player17SetNoControlStateEhih(int self, unsigned char a, int b, unsigned char c);
extern int data_0209caa0[];
extern int data_ov085_02130800[];
}

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing state; the host passes self).
extern "C" int func_ov085_0212e4a4(unsigned int self)
{
    int p = (int)_ZN5Actor13ClosestPlayerEv((void*)self);   /* <-- this, the ROM's r0 */
    if (p != 0) {
        struct V3 { int x, y, z; } v = *(struct V3 *)(p + 0x5c);
        if ((data_0209caa0[2] & 0x10000) != 0 &&
            v.z > -0x28000 &&
            _ZN6Player17SetNoControlStateEhih(p, 0x12, -1, 0) != 0) {
            *(int *)(p + 0x744) = *(int *)(self + 0x5c);
            *(int *)(p + 0x748) = *(int *)(self + 0x60);
            *(int *)(p + 0x74c) = *(int *)(self + 0x64);
            *(int *)(self + 0x5c) = v.x;
            *(int *)(self + 0x60) = v.y;
            *(int *)(self + 0x64) = v.z;
            *(int *)(((long long)((int)(self + 0x5c)))) -= 0x3e8000;
            *(int *)(self + 0x60) = *(int *)(p + 0x644) + 0x3e8000;
            *(int *)(self + 0x2a4) = *(int *)(self + 0x5c);
            *(int *)(self + 0x2a8) = *(int *)(self + 0x60);
            *(int *)(self + 0x2ac) = *(int *)(self + 0x64);
            *(signed char *)(self + 0x2dc) = 0;
            func_ov085_0212e728((void*)self, (void*)(int)data_ov085_02130800);
        }
    }
    return 1;
}
