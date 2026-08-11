/* class MgBounceAndPounce, real ROM name dScMgD3DBase_c -- confirmed by
 * tools/rtti_extract.py: dScMgD3DBase_c : dScMgBase_c, single edge, and its
 * own vtable (ov006:0x0213c62c) is exactly what src/MgBounceAndPounce_Spawn.cpp
 * writes as `_ZTV17MgBounceAndPounce` -- an English name coined before the
 * class hierarchy was understood, kept here since it's already the mangled
 * identity of eight matched functions; renaming it is a separate change.
 *
 * NOT A LEAF: it has four RTTI descendants of its own (dScMgJump_c,
 * dScMgJump2_c, dScMgTrampoline_c, dScMgTrampoline2_c), evidenced two ways --
 * the RTTI graph, and MgBounceAndPounce_Spawn.cpp's own construction order:
 * `*(int*)p = _ZTV17MgBounceAndPounce` (THIS class's vtable, mid-construction)
 * runs BEFORE `*(int*)p = data_ov006_0213cbe4` (dScMgJump_c's own vtable,
 * confirmed against tools/rtti_vtables.py --own dScMgJump_c's slot-16/17
 * addresses matching dScMgJump_c's D1/D0) -- base-vtable-then-derived-vtable
 * is exactly the order a real constructor writes them in.
 *
 * THE DESTRUCTOR CASCADES A THIRD LEVEL, same mechanics as Scene's own fix
 * and dScMgBase_c's (include/Scene.h, include/dScMgBase_c.h). D2/D1 defined
 * INLINE so the four descendants above can inline them; own copy of
 * operator delete for the same reason (mwcc only inlines a D0 route through
 * the class itself or its IMMEDIATE base).
 *
 * FIELDS. sizeof is 0x5834, from MgBounceAndPounce_Spawn.cpp's own
 * `ActorBase::operator new(0x5834)` -- the "second witness" this tree's own
 * recipe (notes, [[matching-session-workflow]]) always prefers over a field
 * span. Three members corroborate it independently:
 *   - mSysTracker at 0x47e4, sizeof 0x81c (Particle::SysTracker, from
 *     include/Stage.h -- a THIRD class now measuring the same size, after
 *     Stage.h's own two gen_header.py shadows) -- ends exactly 0x5000,
 *     which is where the old flat header's own `unk_5000` field sat.
 *   - mModel at 0x501c, sizeof 0x50 (include/Model.h) -- ends exactly
 *     0x506c, which is exactly where the first func_020733a8-managed table
 *     below starts.
 *   - The two tables end exactly at 0x5834 -- the operator-new literal
 *     itself, the third and strongest confirmation.
 * The two tables (3 * 0xb8 at 0x506c, 6 * 0xf0 at 0x5294, both constructed
 * by a shared helper `func_020733a8(base, count, stride, ctor, dtor)`) are
 * left as raw bytes: their element type isn't evidenced yet, and inventing
 * one would be a guess this file's own convention refuses to make.
 */
#ifndef MGBOUNCEANDPOUNCE_H
#define MGBOUNCEANDPOUNCE_H
#include "dScMgBase_c.h"
#include "Stage.h"
#include "Model.h"

extern "C" void func_ov004_020b929c(void *);

struct MgBounceAndPounce : dScMgBase_c {
    /* Declared first -- see include/Scene.h's KEY FUNCTION note. Overrides
       slots 16 (D1) and 17 (D0). DEFINED INLINE so the four descendants
       above can inline it. Body: destroy mSysTracker, then call
       dScMgBase_c's D2 directly -- exactly src/_ZN17MgBounceAndPounceD1Ev.cpp's
       existing recovered body, now compiler-generated instead of hand-written. */
    virtual ~MgBounceAndPounce() {}

    /* Own copy, for the same reason dScMgBase_c has one -- unlocks D0 for
       the four descendants above. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- overrides. 1, 2, 5, 7, 10 re-override slots dScMgBase_c already
           gave a body; 11 (AfterRender) is the first override below
           Scene's own default (dScMgBase_c never touched it). --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void AfterRender(u32 vfSuccess);           /* slot 11 */

    /* Nine more of dScMgBase_c's own 18 new slots (18-35) are overridden
       here too (slots 24-31, 33 per tools/rtti_vtables.py --own
       dScMgD3DBase_c) -- left undeclared, same as dScMgBase_c.h's own
       slots 18-35: their targets are matched source but dScMgBase_c
       hasn't named or given signatures to the slots they'd be overriding
       yet, and a derived override can't be declared before its base is. */

    u8   pad_465d[0x187];             /* dScMgBase_c's own data ends 0x465d; the
                                          Itanium ABI lets a derived class reuse a
                                          polymorphic base's tail padding (see
                                          include/dScMgBase_c.h's size note) */
    Particle::SysTracker mSysTracker; /* 0x47e4 */
    u8   pad_5000[0x1c];
    /* Constructed as a real Model in MgBounceAndPounce_Spawn.cpp
       (_ZN5ModelC1Ev(p + 0x501c)) but NOT destroyed by ~MgBounceAndPounce --
       measured directly: a typed `Model mModel` member here made the
       compiler auto-generate a THIRD destructor call (Model's own D1) that
       the ROM's actual 0x38-byte body does not have (disassembly showed
       only two `bl`s -- SysTracker's D1, then dScMgBase_c's D2). Left as
       raw bytes rather than asserting a destruction the ROM doesn't do;
       same reasoning dScMgBase_c.h uses for touchIcon_0f4. */
    u8   mModel[0x50];                /* 0x501c */
    u8   table1_506c[0x228];          /* 0x506c -- func_020733a8(base,3,0xb8,...), elem type unevidenced */
    u8   table2_5294[0x5a0];          /* 0x5294 -- func_020733a8(base,6,0xf0,...), elem type unevidenced */
};

typedef char MgBounceAndPounce_size_must_be_0x5834[sizeof(MgBounceAndPounce) == 0x5834 ? 1 : -1];

#endif
