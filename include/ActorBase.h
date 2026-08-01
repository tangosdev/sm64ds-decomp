#ifndef ACTORBASE_H
#define ACTORBASE_H

#include "types.h"

/* The root of the actor hierarchy, 0x02043494..0x02043e04.
 *
 * The chain is ActorBase -> ActorDerived -> Actor. See notes/actor-vtables.md;
 * Actor is NOT a direct child of this class.
 *
 * LAYOUT is read out of the ROM, not guessed. ActorBase::ActorBase stores its
 * vptr with `str r1, [r4]`, so the vptr is at 0x0. The same constructor does
 * `add r5, r4, #0x14` and passes r5 to SceneNode::SceneNode, then writes an
 * owner back-pointer with `str r4, [r5, #0x10]` -- which pins sceneNode at 0x14
 * and makes it 0x14 bytes, not the 0x10 that ActorBase__SceneNode.h describes.
 * It then initialises two identical 0x10-byte nodes at r5+0x14 (0x28) and
 * r5+0x24 (0x38), the two the destructor tears down in reverse order.
 *
 * VTABLE ORDER is read directly out of _ZTV9ActorBase (0x02099edc, 18 slots).
 * Every slot resolves to a named function, so no inference was needed here --
 * unlike include/Fader.h. Two consequences, both easy to get wrong:
 *
 *   * The destructor is at slots 16/17, NOT 0/1. Slot index follows declaration
 *     order, so ~ActorBase must be declared AFTER OnHeapCreated. Copying the
 *     Fader header shape, where the destructor comes first, shifts sixteen slots
 *     and silently changes every virtual call in the tree.
 *
 *   * AfterCleanupResources dispatches through vtable+0x40. That is slot 16, the
 *     D1 destructor -- not OnPendingDestroy, which is slot 12 (vtable+0x30, the
 *     slot MarkForDestruction uses). It destructs and then deallocates; calling
 *     that OnPendingDestroy would describe a leak.
 *
 * Field NAMES are inferred from behaviour and cannot change codegen, so they are
 * safe to improve. Offsets, widths and vtable slots are pinned by the bytes.
 */

#ifdef __cplusplus

/* 0x14 bytes: four words plus the owner back-pointer the constructor writes. */
struct ActorBase_SceneNode {
    s32 unk_000;
    s32 unk_004;
    s32 unk_008;
    s32 unk_00c;
    void *owner;        /* 0x10 -- the containing ActorBase */
};

/* 0x10 bytes. Two per ActorBase; the destructor tears them down through
   0x020440e8 in reverse order. */
struct ActorBase_ProcessingListNode {
    u8 raw[0x10];
};

struct ActorBase {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u32 uniqueID;                             /* 0x04 -- post-incremented global */
    u32 param1;                               /* 0x08 */
    u16 actorID;                              /* 0x0c */
    u8  aliveState;                           /* 0x0e -- 2 means already dying */
    u8  shouldBeKilled;                       /* 0x0f */
    u8  unk_010;                              /* 0x10 */
    u8  unk_011;                              /* 0x11 */
    u8  unk_012;                              /* 0x12 */
    u8  unk_013;                              /* 0x13 -- bits 1 and 3 from spawn flags */
    ActorBase_SceneNode sceneNode;            /* 0x14 */
    ActorBase_ProcessingListNode behavNode;   /* 0x28 */
    ActorBase_ProcessingListNode renderNode;  /* 0x38 */
    void *unk_048;                            /* 0x48 */
    void *heap;                               /* 0x4c -- Heap*, owned */

    /* --- vtable, in _ZTV9ActorBase order. Do not reorder. --- */
    virtual s32  InitResources();                      /* slot  0 */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual s32  CleanupResources();                   /* slot  3 */
    virtual int  BeforeCleanupResources();             /* slot  4 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual s32  Behavior();                           /* slot  6 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual void AfterBehavior(u32 vfSuccess);         /* slot  8 */
    virtual s32  Render();                             /* slot  9 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void AfterRender(u32 vfSuccess);           /* slot 11 */
    virtual void OnPendingDestroy();                   /* slot 12 -- vtable+0x30 */
    virtual int  Virtual34(u32 a, u32 b);              /* slot 13 -- vtable+0x34 */
    virtual int  Virtual38(u32 a, u32 b);              /* slot 14 -- vtable+0x38 */
    virtual bool OnHeapCreated();                      /* slot 15 -- vtable+0x3c */
    virtual ~ActorBase();                              /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    void MarkForDestruction();
    /* operator new is deliberately NOT declared here. CW 1.2 rejects an in-class
       declaration of it ("illegal 'operator' declaration"), and it is neither
       virtual nor layout-affecting, so src/_ZN9ActorBasenwEj.cpp defines it
       under its mangled name instead. */
};

#else

/* The same object for the C translation units, which cannot express the virtual
   functions, so the vptr the compiler would place is written out explicitly. */
struct ActorBase {
    void **vtable;          /* 0x00 */
    u32 uniqueID;           /* 0x04 */
    u32 param1;             /* 0x08 */
    u16 actorID;            /* 0x0c */
    u8  aliveState;         /* 0x0e */
    u8  shouldBeKilled;     /* 0x0f */
    u8  unk_010;            /* 0x10 */
    u8  unk_011;            /* 0x11 */
    u8  unk_012;            /* 0x12 */
    u8  unk_013;            /* 0x13 */
    u8  sceneNode[0x14];    /* 0x14 */
    u8  behavNode[0x10];    /* 0x28 */
    u8  renderNode[0x10];   /* 0x38 */
    void *unk_048;          /* 0x48 */
    void *heap;             /* 0x4c */
};

#endif /* __cplusplus */

#endif
