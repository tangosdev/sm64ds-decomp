#ifndef FBASE_C_H
#define FBASE_C_H

#include "types.h"

/* The root of the actor hierarchy. Its code is one contiguous run in arm9,
 * 0x02043444..0x02043f4c, 25 functions.
 *
 * THAT RANGE IS A CORRECTION. This banner used to say "0x02043494..0x02043e04"
 * and both ends were wrong; src_tu/actors/fBase_c.cpp reconciled it against
 * the cartridge while rebuilding the translation unit.
 *
 *   0x02043444  is the real start: _ZN7fBase_cnwEj, this class's own
 *               operator new (size 0x50, discussed further down this file).
 *               The old 0x02043494 began at OnHeapCreated and excluded it.
 *   0x02043f4c  is the real end -- the byte after _ZN7fBase_cC2Ev, and the
 *               address of func_02043f4c, the next unrelated function.
 *   0x02043e04  was not a function boundary at all. It falls 0x18 bytes INSIDE
 *               the constructor (_ZN7fBase_cC2Ev, 0x02043dec, size 0x160), so
 *               the old end cut that function in half.
 *
 * The run is bracketed by unlabelled neighbours on both sides -- func_020433b8
 * ends at 0x02043444, func_02043f4c begins at 0x02043f4c -- and contains exactly
 * two unnamed functions, func_02043810 and func_02043880, both enclosed by named
 * members and both reading this layout directly. 24 of the 25 are byte-verified
 * together as one TU; the constructor is the exception, and the only source the
 * tree has for it is a NONMATCHING hand-written asm transcription that
 * config/arm9/delinks.txt does not enrol.
 *
 * SEPARATELY, the nested class fBase_c::SceneNode has two functions of its own
 * 0x8000 bytes away at 0x0203b4ac..0x0203b4dc, sitting next to the intrusive-
 * list primitives this class calls. Different translation unit; not part of the
 * run above.
 *
 * The chain is fBase_c -> dBase_c -> dActor_c. See notes/actor-vtables.md;
 * dActor_c is NOT a direct child of this class.
 *
 * LAYOUT is read out of the ROM, not guessed. fBase_c::fBase_c stores its
 * vptr with `str r1, [r4]`, so the vptr is at 0x0. The same constructor does
 * `add r5, r4, #0x14` and passes r5 to SceneNode::SceneNode, then writes an
 * owner back-pointer with `str r4, [r5, #0x10]` -- which pins sceneNode at 0x14
 * and makes it 0x14 bytes, not the 0x10 that ActorBase__SceneNode.h describes.
 * It then initialises two identical 0x10-byte nodes at r5+0x14 (0x28) and
 * r5+0x24 (0x38), the two the destructor tears down in reverse order.
 *
 * VTABLE ORDER is read directly out of _ZTV7fBase_c (0x02099edc, 18 slots).
 * Every slot resolves to a named function, so no inference was needed here --
 * unlike include/Fader.h. Two consequences, both easy to get wrong:
 *
 *   * The destructor is at slots 16/17, NOT 0/1. Slot index follows declaration
 *     order, so ~fBase_c must be declared AFTER OnHeapCreated. Copying the
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

/* The actor heap and its deallocator, for the inline operator delete at the end
   of the class. `data_020a0eac` is the heap every actor is allocated from -- the
   same one fBase_c::operator new (src/_ZN7fBase_cnwEj.cpp) passes to
   Memory::Allocate. Spelt exactly as include/decl_common.h and include/dActor_c.h
   spell them; see the note on operator delete below for why the `void *` second
   parameter is deliberate and not a mistake. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

/* 0x14 bytes: four words plus the owner back-pointer the constructor writes. */
struct ActorBase_SceneNode {
    s32 unk_000;
    s32 unk_004;
    s32 unk_008;
    s32 unk_00c;
    void *owner;        /* 0x10 -- the containing fBase_c */
};

/* 0x10 bytes. Two per fBase_c; the destructor tears them down through
   0x020440e8 in reverse order. */
struct ActorBase_ProcessingListNode {
    u8 raw[0x10];
};

struct fBase_c {
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

    /* --- vtable, in _ZTV7fBase_c order. Do not reorder. --- */
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
    virtual ~fBase_c();                              /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    void MarkForDestruction();
    /* operator new is deliberately NOT declared here. CW 1.2 rejects an in-class
       declaration of it ("illegal 'operator' declaration"), and it is neither
       virtual nor layout-affecting, so src/_ZN7fBase_cnwEj.cpp defines it
       under its mangled name instead. */

    /* operator delete IS declared here, and unlike operator new the compiler
       accepts it in-class. It is what lets a real `~fBase_c()` reproduce the
       ROM's deleting destructor: mwcc generates D0 as "run the destructor body,
       then call operator delete on the class", and without this it emits a call
       to the global `_ZdlPv`, which exists nowhere in this image.

       MEASURED, not assumed. The ROM's two D0s under this class both END with
       the same two instructions -- load the actor heap pointer, call
       Memory::Deallocate -- rather than calling a shared helper, and both are
       exactly their D1 body plus those instructions:
           fBase_c::~fBase_c [D0]    0x02043d78  0x44 = D1's 0x30 + 0x14
           dBase_c::~dBase_c   0x02013ea4  0x38 = D1's 0x24 + 0x14
       Compiled without this declaration, src_tu/actors/dBase_c.cpp's D0
       came out the wrong SIZE (`999 word(s) differ`); with it, 5/5 MATCH. Only
       an inline member produces that shape.

       WHY HERE AND ALSO ON dActor_c. include/dActor_c.h carries its own copy and its
       comment records why: mwcc inlines operator delete only when it is found in
       the class itself or its IMMEDIATE base, so a declaration here does NOT
       reach dActor_c (two levels down) and cannot replace dActor_c's. Nor does it
       reach HUD, Minimap or dScene_c, whose immediate base is dBase_c. The
       only classes it changes are fBase_c itself and dBase_c -- the two
       whose D0 the ROM shows inlining it. The two src/ files that declare a
       local `struct dActor_c : fBase_c` (EndKuppaScript.cpp,
       func_ov002_020b7e1c.cpp) use their own local shadow fBase_c, not this
       one, so they are out of scope too.

       No layout effect: an inline non-virtual member adds no field and no vtable
       slot, and the 0x50 assertion below still holds. Spelt exactly as
       include/dActor_c.h and include/decl_common.h spell the two names -- declaring
       the honest `Heap*` second parameter here instead would put two
       incompatible extern "C" declarations of one name in the same TU, which
       mwcc rejects as "illegal function overloading". */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};

#else

/* The same object for the C translation units, which cannot express the virtual
   functions, so the vptr the compiler would place is written out explicitly. */
struct fBase_c {
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

/* Outside the __cplusplus split on purpose, so it holds BOTH spellings to the
   same 0x50 -- the C one writes the vptr out by hand and would otherwise be free
   to drift from the C++ one. It is also what lets tools/check_header_offsets.py
   check the classes derived from this: a derived struct's own fields start at the
   base's size, and the tool refuses to guess it. */
typedef char fBase_c_size_must_be_0x50[sizeof(struct fBase_c) == 0x50 ? 1 : -1];

#endif
