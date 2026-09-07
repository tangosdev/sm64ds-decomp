#ifndef FBASE_C_H
#define FBASE_C_H

#include "types.h"

/* The root of the actor hierarchy. Its code is one contiguous run in arm9,
 * 0x02043444..0x02043f4c, 25 functions. The chain is fBase_c -> dBase_c ->
 * dActor_c; dActor_c is NOT a direct child of this class.
 *
 * DO NOT REORDER THE VIRTUALS -- _ZTV7fBase_c (0x02099edc) is 18 slots and slot
 * index follows declaration order. Two that are easy to get wrong: the
 * destructor is at 16/17, NOT 0/1, and vtable+0x40 (which AfterCleanupResources
 * dispatches through) is that D1 destructor, not OnPendingDestroy at slot 12.
 *
 * InitResources is declared here but src/_ZN7fBase_c13InitResourcesEv.cpp
 * deliberately defines it as an extern "C" free function, because a class's
 * first non-inline virtual is its key function and CW emits the vtable into
 * whichever TU defines it -- colliding with the copy the module's gap object
 * supplies from ROM. Do not "fix" that file into a real method, and do not
 * remove this declaration: removing it deletes slot 0 and shifts all 18.
 *   0x02043444  is the real start: _ZN7fBase_cnwEj, this class's own
 *               operator new (size 0x50, discussed further down this file).
 *               The old 0x02043494 began at OnHeapCreated and excluded it.
 *   0x02043f4c  is the real end -- the byte after _ZN7fBase_cC2Ev, and the
 *               address of func_02043f4c, the next unrelated function.
 *   0x02043e04  was not a function boundary at all. It falls 0x18 bytes INSIDE
 *               the constructor (_ZN7fBase_cC2Ev, 0x02043dec, size 0x160), so
 *               the old end cut that function in half.
 *
 * Offsets, widths and vtable slots are pinned by the bytes; field names are not
 * and are safe to improve. The 0x34-byte manager at 0x14 owns the scene-tree
 * node and the two process-list nodes, matching both the constructor's retained
 * manager address and the later EAD engine's independently recovered fManager_c
 * organization. See notes/actor-core-provenance.md and notes/actor-vtables.md.
 */

#ifdef __cplusplus

#include "fListNode.h"

/* The actor heap and its deallocator, for the inline operator delete at the end
   of the class. data_020a0eac is the heap every actor is allocated from. Spelt
   exactly as include/decl_common.h and include/dActor_c.h spell them -- the
   honest Heap* second parameter would put two incompatible extern "C"
   declarations of one name in a single TU, which mwcc rejects. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct fBase_c {
    /* Intrusive scene-graph node owned by every actor. */
    struct SceneNode {
        SceneNode *parent;
        SceneNode *child;
        SceneNode *prev;
        SceneNode *next;
        fBase_c *owner;                         /* 0x10 */

        SceneNode();
        void Reset();
    };

    /* Process bookkeeping kept together as one manager subobject. */
    struct Manager {
        SceneNode sceneNode;
        fLiNdBaPr_c behaviorNode;
        fLiNdBaPr_c renderNode;

        Manager(fBase_c *owner_)
        {
            sceneNode.owner = owner_;
            {
                fLiNdBaPr_c *node = &behaviorNode;
                node->prev = 0;
                node->next = 0;
                node->owner = owner_;
                node->currentPriority = 0;
                node->nextPriority = 0;
            }
            {
                fLiNdBaPr_c *node = &renderNode;
                node->prev = 0;
                node->next = 0;
                node->owner = owner_;
                node->currentPriority = 0;
                node->nextPriority = 0;
            }
        }

        /* Both halves move together when a process enters the behavior/render
           lists. The method names are inferred; the paired writes are ROM-proven. */
        void SetBehaviorPriority(u16 priority)
        {
            fLiNdBaPr_c *node = &behaviorNode;
            node->currentPriority = priority;
            node->nextPriority = priority;
        }

        void SetRenderPriority(u16 priority)
        {
            fLiNdBaPr_c *node = &renderNode;
            node->currentPriority = priority;
            node->nextPriority = priority;
        }

    };

    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u32 uniqueID;                             /* 0x04 -- post-incremented global */
    u32 param1;                               /* 0x08 */
    u16 actorID;                              /* 0x0c */
    u8  aliveState;                           /* 0x0e -- 2 means already dying */
    u8  shouldBeKilled;                       /* 0x0f */
    u8  unk_010;                              /* 0x10 */
    u8  unk_011;                              /* 0x11 */
    u8  unk_012;                              /* 0x12 */
    /* 0x13. Bit 1 pauses this object's behaviour and bit 4 hides it; bits 2 and
       8 are the effective versions, which the constructor seeds by OR-ing the
       parent's 1|2 and 4|8 down. BeforeBehavior gates on 2, BeforeRender on 8. */
    u8  pauseFlags;                           /* 0x13 */
    Manager manager;                          /* 0x14 */
    /* Optional lifecycle-operation sequence. BeforeCleanupResources waits for
       it to leave its pending state, and AfterCleanupResources drains it.
       The role is ROM-proven; the original member spelling is not. */
    void *lifecycleState;                     /* 0x48 */
    void *heap;                               /* 0x4c -- Heap*, owned */

    fBase_c();

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
    /* operator new is deliberately NOT declared here: CW rejects an in-class
       declaration of it, so src/_ZN7fBase_cnwEj.cpp defines it under its mangled
       name instead. It is neither virtual nor layout-affecting.

       operator delete IS accepted in-class, and must be, INLINE: CW builds D0
       (slot 17) as "run the destructor, then call operator delete", and without
       this it calls the global _ZdlPv, which exists nowhere in this image. The
       ROM's D0s under this class are each exactly their D1 plus the two
       instructions this expands to. Note it reaches only fBase_c and dBase_c --
       CW inlines it from the class itself or its IMMEDIATE base -- which is why
       include/dActor_c.h carries its own copy. */
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
    u8  pauseFlags;         /* 0x13 */
    u8  manager[0x34];      /* 0x14 */
    void *lifecycleState;   /* 0x48 */
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
