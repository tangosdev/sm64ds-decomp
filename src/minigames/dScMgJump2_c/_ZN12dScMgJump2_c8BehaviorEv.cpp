//cpp
// @symbol _ZN12dScMgJump2_c8BehaviorEv
#include "dScMgJump2_c.h"
/* dScMgJump2_c::Behavior -- vtable slot 6, ov006 0x020ef3e0.
 *
 * Attributed by the vtable: dScMgJump2_c's own table is ov006 0x0213ccfc and
 * its slot 6 relocates here. The `recovered name:` comment on the
 * pre-migration file agreed, but the vtable is what settles it. The signature
 * is include/dScMgBase_c.h's own slot 6, `virtual s32 Behavior()`.
 *
 * unk_5a6c IS A PARTICLE SYSTEM UNIQUE ID, passed in and back out of
 * Particle::System::NewUnkCallback818 each frame and then used to look the
 * system up again. The two writes at +0x50 and +0x58 of whatever FromUniqueID
 * returns are left as raw offsets: no header in this tree types that object.
 *
 * The two Particle::System declarations keep the local `unsigned int` /
 * `int` spelling copied from the pre-migration file rather than the true
 * Fix12<int> parameters their ROM names carry -- mwccarm passes by-value class
 * parameters differently at the call site, so declaring the real types breaks
 * the byte match. notes/mwccarm-codegen.md 6az records it; the sibling
 * src/_ZN18dScMgTrampoline2_c8BehaviorEv.cpp says the same at its own site.
 *
 * THE STATE CALLBACK IS A POINTER-TO-MEMBER STORED IN THE OBJECT, in the
 * eight bytes at 0x5004 that include/dScMgJump2_c.h calls pad_5004 -- eight
 * being mwccarm's member-pointer size. The pre-migration file cast through a
 * local incomplete `struct C` for the receiver; naming the real class here is
 * byte-identical, which is what makes the field's extent evidence rather than
 * a guess. */

struct Vector3_16f;

extern "C" {
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, int c, int d, int e, const Vector3_16f *f);
void *_ZN8Particle6System12FromUniqueIDEj(unsigned int id);
void func_ov006_020eef90(void);
}

typedef void (dScMgJump2_c::*dScMgJump2_cState)();

s32 dScMgJump2_c::Behavior()
{
    unk_5a6c = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        unk_5a6c, 0xf0, 0x400000, 0x800000, -0x480000, 0);
    void *o = _ZN8Particle6System12FromUniqueIDEj(unk_5a6c);
    if (o != 0) {
        *(int *)((char *)o + 0x50) = 0x4000;
        *(unsigned char *)((char *)o + 0x58) = 0x2c;
    }
    func_ov006_020eef90();
    {
        dScMgJump2_cState *pp = (dScMgJump2_cState *)pad_5004;
        (this->**pp)();
    }
    return 1;
}
