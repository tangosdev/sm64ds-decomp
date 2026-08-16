//cpp
// @symbol _ZN18dScMgTrampoline2_c8BehaviorEv
#include "decl_common.h"
#include "dScMgTrampoline2_c.h"
/* dScMgTrampoline2_c::Behavior -- vtable slot 6, ov006 0x02123340.
 *
 * Attributed by the vtable: dScMgTrampoline2_c's own table is ov006 0x0213fc7c
 * and its slot 6 relocates here. The signature is include/dScMgBase_c.h's own
 * slot 6, `virtual s32 Behavior()`.
 *
 * IT READS THE GAP include/dScMgTrampoline2_c.h CALLS UNEVIDENCED. pad_7ac4 is
 * 0xc bytes between mArray5 and mArray6; this function uses the first word as
 * a Particle::System unique ID -- passed in and back out of NewUnkCallback818
 * each frame, exactly as dScMgJump2_c::Behavior does with its own unk_5a6c --
 * and the second as a fix12 value ramped towards 0x14000 and written into the
 * system's byte at +0x58. Both stay inside the pad rather than becoming named
 * fields: the third word is still unreached, and cutting the gap two-thirds of
 * the way along would assert a boundary nothing shows.
 *
 * The two Particle::System declarations keep the pre-migration file's local
 * `int` spelling rather than the true Fix12<int> their ROM names carry --
 * mwccarm passes by-value class parameters differently at the call site, so
 * declaring the real types breaks the byte match (notes/mwccarm-codegen.md
 * 6az). Its sibling src/_ZN12dScMgJump2_c8BehaviorEv.cpp says the same.
 *
 * ApproachLinear IS THE REAL OVERLOAD, `_Z14ApproachLinearRiii` at 0x0203ae58,
 * reached by declaring the C++ signature and letting the compiler mangle it --
 * not by decl_common.h's ApproachLinearI(int *, ...), which is a different
 * symbol.
 *
 * THE STATE CALLBACK IS A POINTER-TO-MEMBER STORED IN THE OBJECT, in the eight
 * bytes at 0x5004 the header calls pad_5004 -- the same arrangement, at the
 * same offset, dScMgJump2_c has, which is what two children of
 * dScMgD3DBase_c sharing a base ought to look like. */

struct Vector3_16f;

extern "C" {
void *_ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, int c, int d, int e, const Vector3_16f *f);
void *_ZN8Particle6System12FromUniqueIDEj(unsigned int id);
void func_ov006_020eef90(void);
}

extern void ApproachLinear(int &x, int a, int b);

typedef void (dScMgTrampoline2_c::*dScMgTrampoline2_cState)();

s32 dScMgTrampoline2_c::Behavior()
{
    char *c = (char *)this;
    int saved = data_ov006_02140830;

    func_ov006_02120c40();
    func_ov006_020eef90();
    func_ov006_02122ab8();
    *(void **)(c + 0x7ac4) =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(unsigned int *)(c + 0x7ac4), 0xf0, 0x280000, 0x700000, -0x580000, 0);
    void *p = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int *)(c + 0x7ac4));
    if (p != 0) {
        *(char *)((char *)p + 0x58) = (char)(*(int *)(c + 0x7ac8) >> 12);
        ApproachLinear(*(int *)(c + 0x7ac8), 0x14000, 0x200);
    }
    {
        dScMgTrampoline2_cState *pp = (dScMgTrampoline2_cState *)pad_5004;
        (this->**pp)();
    }
    func_ov006_02123938(this);
    if (saved != data_ov006_02140830) {
        func_ov004_020adb1c(data_ov006_02140830);
    }
    return 1;
}
