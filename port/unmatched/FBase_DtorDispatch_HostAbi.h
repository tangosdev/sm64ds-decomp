/* PORT_HOST_ABI. fBase_c, force-included ahead of
 * src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp so that translation unit
 * DISPATCHES the actor's own destructor through ROM vtable slot 16 instead of
 * calling fBase_c's own destructor body directly. Nothing in src/ or include/
 * changes. Run link100, lane DTORCALL.
 *
 * The mechanism is the one port/unmatched/Fader_HostAbi.h and
 * port/unmatched/MgSmartball_HostAbi.h already use: pre-define the real
 * header's own guard macro, so the src file still says
 * `#include "fBase_c.h"`, the include still happens, and the body is skipped
 * because the guard is already defined.
 *
 * ---- WHAT IS WRONG WITHOUT THIS ----------------------------------------
 *
 * The cartridge is unambiguous. _ZN7fBase_c21AfterCleanupResourcesEj at
 * 0x02043b2c (extracted/arm9_dec.bin; the arm9 main module loads at
 * 0x02004000, not 0x02000000 -- extracted/dsd/arm9/arm9.yaml base_address):
 *
 *     02043b78  mov r0, r4           ; this
 *     02043b7c  ldr r1, [r0]         ; the object's vtable
 *     02043b80  ldr r1, [r1, #0x40]  ; SLOT 16, the complete-object destructor
 *     02043b84  blx r1               ; DISPATCH -- the most-derived D1 runs
 *     02043b94  bl  #0x203c1e8       ; then Memory::Deallocate
 *
 * include/fBase_c.h says the same thing in its own banner: "vtable+0x40 (which
 * AfterCleanupResources dispatches through) is that D1 destructor". Every
 * actor's teardown in the game goes through this one call: dActor_c and
 * dScene_c override AfterCleanupResources only to delegate straight back to
 * it, and it is the only `this->~X()` in the whole of src/.
 *
 * Commit 9cbd99048 ("bring the two-arm destructor spelling onto the port
 * lineage") gave 341 headers the host spelling that keeps MSVC's slot
 * numbering equal to the ROM's:
 *
 *     #ifdef _MSC_VER
 *         virtual void Destructor1();   // D1, slot 16
 *         virtual void Destructor0();   // D0, slot 17
 *         ~fBase_c();                   // no slot
 *     #else
 *         virtual ~fBase_c();
 *     #endif
 *
 * That change is right and must stay: it is what closed all forty of lane
 * VTABLE's measured wrong-slot dispatch sites, fifteen of them the
 * frame-corrupting shape, plus the collision-cylinder bug that was live on the
 * render path. But it has a side effect nobody booked. With `~fBase_c()`
 * spelled NON-VIRTUAL, `this->~fBase_c()` binds to that declaration and MSVC
 * emits a DIRECT call to fBase_c's own destructor body:
 *
 *     005ef7d7  mov  ecx, esi
 *     005ef7d9  call 0x4f38b0        ; ??1fBase_c@@QAE@XZ, the BASE body
 *
 * so no derived destructor is reached and every actor's own cleanup is
 * skipped. Measured on the shipped artifact by two independent instruments
 * (out/INTWALK/d16scan.py, a byte-encoding scan; out/DTORCALL/slot16scan.py, a
 * per-function disassembly): ZERO slot-16 dispatch sites in the entire image,
 * against 23 sites at slot 15 and 43 at slot 18 either side of it. Nothing
 * reports it. The game runs and looks healthy.
 *
 * ---- WHY THIS SHAPE ----------------------------------------------------
 *
 * The shadow below declares fBase_c's sixteen ROM virtuals in ROM order and
 * then a VIRTUAL destructor, so MSVC gives the destructor slot 16 -- the same
 * word the cartridge dispatches -- and compiles `this->~fBase_c()` as a
 * virtual call through it. Only this one translation unit sees the shadow, and
 * it dispatches no slot above 16, so the two-arm spelling every other
 * translation unit sees is untouched and ACTORPORT's forty sites cannot move.
 *
 * MSVC routes an explicit destructor call through its own scalar deleting
 * destructor slot, which is __thiscall with ONE CALLEE-POPPED STACK ARGUMENT,
 * the delete flag. The cartridge pushes nothing. That difference is already
 * ruled on and already paid for in port/hal/port_d16.h: every slot-16 store in
 * the tree is a PORT_D16 wrapper that takes the flag as its one stack argument
 * and pops it, and port/tools/slot16_guard.py refuses any that is not. This
 * shadow is the dispatcher those wrappers were built for; without it they are
 * never called.
 *
 * The class is left ABSTRACT on purpose, exactly as Fader_HostAbi.h leaves the
 * fader hierarchy: slots 0..15 are pure and nothing here is ever instantiated,
 * so no vftable is emitted for the shadow and the virtual destructor never
 * needs an address. AfterCleanupResources is the one slot declared non-pure,
 * because this translation unit defines it.
 *
 * PORT_FBASE_DTOR_DISPATCH is the A/B control. Defined to 0 it spells the
 * destructor non-virtually and this file reproduces the current, defective
 * behaviour exactly, so the before and after builds differ by ONE declaration.
 * It ships at 1.
 */
#ifndef PORT_FBASE_DTORDISPATCH_HOSTABI_H
#define PORT_FBASE_DTORDISPATCH_HOSTABI_H

#ifndef PORT_FBASE_DTOR_DISPATCH
#define PORT_FBASE_DTOR_DISPATCH 1
#endif

#include "types.h"
#include "fListNode.h"

/* Spelled exactly as include/fBase_c.h spells them, for the inline
   operator delete at the end of the class. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

/* shadows include/fBase_c.h */
#define FBASE_C_H
struct fBase_c {
    /* Field block copied from include/fBase_c.h. The offsets are the ones the
       cartridge uses at 0x02043b2c: manager.sceneNode at +0x14, behaviorNode
       at +0x28, lifecycleState at +0x48, heap at +0x4c, sizeof 0x50. */
    struct SceneNode {
        SceneNode *parent;
        SceneNode *child;
        SceneNode *prev;
        SceneNode *next;
        fBase_c *owner;                       /* 0x10 */
    };

    struct Manager {
        SceneNode sceneNode;                  /* 0x00 within Manager */
        fLiNdBaPr_c behaviorNode;             /* 0x14 within Manager */
        fLiNdBaPr_c renderNode;               /* 0x24 within Manager */
    };

    /* 0x00 is the vptr. */
    u32 uniqueID;                             /* 0x04 */
    u32 param1;                               /* 0x08 */
    u16 actorID;                              /* 0x0c */
    u8  aliveState;                           /* 0x0e */
    u8  shouldBeKilled;                       /* 0x0f */
    u8  unk_010;                              /* 0x10 */
    u8  unk_011;                              /* 0x11 */
    u8  unk_012;                              /* 0x12 */
    u8  pauseFlags;                           /* 0x13 */
    Manager manager;                          /* 0x14 */
    void *lifecycleState;                     /* 0x48 */
    void *heap;                               /* 0x4c */

    /* --- vtable, in _ZTV7fBase_c order. Do not reorder. Slots 0..15 are pure
       so that the shadow stays abstract and emits no vftable of its own; this
       translation unit only ever holds fBase_c through `this`. --- */
    virtual s32  InitResources() = 0;                      /* slot  0 */
    virtual bool BeforeInitResources() = 0;                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess) = 0;    /* slot  2 */
    virtual s32  CleanupResources() = 0;                   /* slot  3 */
    virtual int  BeforeCleanupResources() = 0;             /* slot  4 */
    virtual void AfterCleanupResources(u32 vfSuccess);     /* slot  5 -- defined here */
    virtual s32  Behavior() = 0;                           /* slot  6 */
    virtual int  BeforeBehavior() = 0;                     /* slot  7 */
    virtual void AfterBehavior(u32 vfSuccess) = 0;         /* slot  8 */
    virtual s32  Render() = 0;                             /* slot  9 */
    virtual int  BeforeRender() = 0;                       /* slot 10 */
    virtual void AfterRender(u32 vfSuccess) = 0;           /* slot 11 */
    virtual void OnPendingDestroy() = 0;                   /* slot 12 */
    virtual int  Virtual34(u32 a, u32 b) = 0;              /* slot 13 */
    virtual int  Virtual38(u32 a, u32 b) = 0;              /* slot 14 */
    virtual bool OnHeapCreated() = 0;                      /* slot 15 */
#if PORT_FBASE_DTOR_DISPATCH
    virtual ~fBase_c();                                    /* slot 16 -- the ROM's D1 */
#else
    ~fBase_c();                                            /* A/B control: no slot */
#endif

    /* Identical to include/fBase_c.h's, so the COMDAT this translation unit
       emits for ??3fBase_c@@SAXPAX@Z folds onto the one every other
       translation unit emits. Do not add anything to this body. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};

/* The one layout fact that can be asserted here rather than read out of the
   built code. The member offsets themselves are checked against the
   cartridge's own 0x14 / 0x28 / 0x48 / 0x4c in the shipped bytes, by
   port/tools/dtor_dispatch_guard.py. */
typedef char port_fBase_c_shadow_size_must_be_0x50[sizeof(fBase_c) == 0x50 ? 1 : -1];

#endif
