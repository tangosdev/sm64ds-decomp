//cpp
/* fBase_c, the root of the actor hierarchy (arm9). 25 functions,
 * .text 0x02043444..0x02043f4c, enrolled and canonical (delinks
 * points here, not at per-function files).
 *
 * Source runs REVERSE of ROM (highest address first). Do not reorder.
 * The destructors stay extern "C" free functions: a real dtor stores
 * the vptr with addend 8 (vtable object), but the ROM's pools hold
 * the slot array (addend 0). InitResources stays a free function too:
 * defining it as a method would emit _ZTV7fBase_c a second time.
 * decl_common.h is deliberately NOT included (three signatures
 * contradict this TU's uses; see below).
 *
 * Leftover: the func_0203/data_ helpers and homes keep linker names.
 */
#include "fBase_c.h"

struct fBaseActorInfo {
    u32 unk_000;
    u16 behaviorPriority;
    u16 renderPriority;
};

/* Keep the list-head and pointer signatures used here local: decl_common.h
 * gives incompatible integer declarations for some of these symbols. */
extern "C" {

/* The ABI destructor variants below destroy both process-list nodes. */
extern "C" void _ZN11fLiNdBaPr_cD1Ev(void *node);

/* Intrusive-list operations on the four global list heads below. All four take
   (list head, node); the legacy files spelled the heads `int`, `char` and
   `int[]` in different files and passed `&x` or `x` accordingly. Unified on the
   array spelling, so every call site passes the bare name and every reloc is
   the same addend-0 reference it always was. */
extern int  func_0203b3c0(void *list, void *node);
extern int  func_0203b27c(void *list, void *node);
extern int  func_0203b20c(void *list, void *node);
extern int  func_0203b244(void *list, void *node);
extern int  func_0204405c(void *list, void *node);
extern int  func_0203b438(void *root, void *node, void *parent);

extern int  data_020a4b6c[];
extern int  data_020a4b78[];
extern int  data_020a4b88[];
extern int  data_020a4b98[];
extern int  data_020a4ba8[];
extern int  data_02099f24[];
extern u32  data_02099e70;
extern u32  data_020a4b60;
extern u16  data_020a4b54;
extern u8   data_020a4b48;
extern void *data_020a4b64;
extern fBaseActorInfo **data_020a4bb8;

extern void func_02044334(void *p);
extern int  func_0204424c(char *p);
extern void *func_0206e2f8(void *p, int fill, unsigned int size);

/* Defined LOWER DOWN in this file (ROM ordinal 4) but called from ordinal 6,
   which the reverse-emission order puts above it. Declared here rather than
   immediately before its caller so there is exactly one declaration of it. The
   two legacy files disagreed -- the definition said `int func_02043810(int *)`
   and its caller said `Obj *func_02043810(Obj *)` -- and nothing tested the
   disagreement, because each file only ever saw its own half. Reconciled to the
   pointer spelling, which is the more complete observation: it returns another
   actor, not an integer. */
extern void *func_02043810(void *p);

/* The class's own vtable, as DATA. Named, not defined: see the destructor note
   in the file banner for why this reference has to be addend 0. config/arm9
   carries two symbols at 0x02099edc -- `_ZTV7fBase_c` and the older
   placeholder `data_02099edc` -- and the legacy files used both. This is the
   one that says what it is. */
extern void *_ZTV7fBase_c[];

/* Heap and Memory, by their exact ROM names.
   `void *` rather than `Heap *` throughout, deliberately: include/Heap.h has a
   real polymorphic Heap class, but Virtual34 and Virtual38 both reach two of
   its fields by raw offset (`+4` flags word, `+8` top-of-heap) rather than by
   member, and reconciling those onto named members is a per-function codegen
   change this round has no reason to risk. include/Heap.h itself declares
   InitializeSolidHeapAsDefault as returning `void *`, so that part is the
   tree's own choice, not an evasion. Memory::Deallocate and data_020a0eac come
   from include/fBase_c.h, which needs them for its inline operator delete. */
extern void *_ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(u32 size, void *root,
                                                          int align);
extern void *_ZN6Memory8AllocateEjiP4Heap(unsigned int size, int align, void *heap);
extern void  _ZN4Heap20RestoreFromTemporaryEv(void);
extern void  _ZN4Heap8_DestroyEv(void *h);
extern u32   _ZN4Heap21MaxAllocationUnitSizeEv(void *h);
extern u32   _ZN4Heap11ResizeToFitEv(void *h);

}

// @symbol _ZN7fBase_cC2Ev
fBase_c::fBase_c() : manager(this)
{
    uniqueID = data_02099e70;
    data_02099e70++;
    param1 = data_020a4b60;
    actorID = data_020a4b54;
    unk_012 = data_020a4b48;

    func_0203b438(data_020a4b6c, &manager.sceneNode, data_020a4b64);

    fBaseActorInfo *info = data_020a4bb8[actorID];
    manager.SetBehaviorPriority(info->behaviorPriority);
    manager.SetRenderPriority(info->renderPriority);

    fBase_c *parent = (fBase_c *)func_02043810(this);
    if (parent != 0) {
        if ((parent->pauseFlags & 1) || (parent->pauseFlags & 2))
            pauseFlags |= 2;
        if ((parent->pauseFlags & 4) || (parent->pauseFlags & 8))
            pauseFlags |= 8;
    }
}

/* ========================================================================= *
 * ROM ordinals 23, 22 and 21 -- the destructor variants.
 *   _ZN7fBase_cD2Ev  0x02043d48  size 0x30   (base-object)
 *   _ZN7fBase_cD0Ev  0x02043d78  size 0x44   (deleting)
 *   _ZN7fBase_cD1Ev  0x02043dbc  size 0x30   (complete-object)
 * Written D1, D0, D2 so the reversal emits them D2, D0, D1 -- the ROM's order.
 *
 * fBase_c is the root: there is no base destructor to chain to. Each variant
 * stores the vptr, then destroys the two ProcessingListNode members in REVERSE
 * declaration order (manager.renderNode at 0x38 before
 * manager.behaviorNode at 0x28), and returns
 * `this`. D0 additionally hands the object back to the actor heap, which is
 * what an inline `operator delete` compiles to -- see include/fBase_c.h.
 * D1 and D2 are byte-identical, which for a root class they must be.
 * ========================================================================= */
// @symbol _ZN7fBase_cD1Ev
extern "C" fBase_c *_ZN7fBase_cD1Ev(fBase_c *self)
{
    *(int *)self = (int)_ZTV7fBase_c;
    _ZN11fLiNdBaPr_cD1Ev(&self->manager.renderNode);
    _ZN11fLiNdBaPr_cD1Ev(&self->manager.behaviorNode);
    return self;
}

// @symbol _ZN7fBase_cD0Ev
extern "C" fBase_c *_ZN7fBase_cD0Ev(fBase_c *self)
{
    *(int *)self = (int)_ZTV7fBase_c;
    _ZN11fLiNdBaPr_cD1Ev(&self->manager.renderNode);
    _ZN11fLiNdBaPr_cD1Ev(&self->manager.behaviorNode);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}

// @symbol _ZN7fBase_cD2Ev
extern "C" fBase_c *_ZN7fBase_cD2Ev(fBase_c *self)
{
    *(int *)self = (int)_ZTV7fBase_c;
    _ZN11fLiNdBaPr_cD1Ev(&self->manager.renderNode);
    _ZN11fLiNdBaPr_cD1Ev(&self->manager.behaviorNode);
    return self;
}

/* Run the guard, then the action when allowed, and always notify the after hook. */
// @symbol _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE
int fBase_c::Process(ProcessFunction action, BeforeProcessFunction before,
                     AfterProcessFunction after)
{
    int result = (this->*before)();
    u32 status;
    if (result != 0) {
        result = (this->*action)();
        if (result == -1) status = 3;
        else if (result == 1) status = 2;
        else status = 1;
    } else {
        status = 0;
    }
    (this->*after)(status);
    return result;
}

/* vtable slot 0. Default initialization succeeds without loading resources.
 *
 * THE KEY FUNCTION, AND THE REASON THIS ONE IS NOT A METHOD. See the file
 * banner: a real `s32 fBase_c::InitResources()` anywhere makes that TU emit
 * _ZTV7fBase_c and collide with the gap object's copy of the ROM's own
 * vtable. Its DECLARATION in the class is required and harmless -- removing it
 * would delete slot 0 and shift the other seventeen. */
// @symbol _ZN7fBase_c13InitResourcesEv
extern "C" int _ZN7fBase_c13InitResourcesEv(void)
{
    return 1;
}

/* vtable slot 1. Allow initialization by default. */
// @symbol _ZN7fBase_c19BeforeInitResourcesEv
bool fBase_c::BeforeInitResources()
{
    return 1;
}

/* vtable slot 2. Only acts on VS_SUCCESS (2): unlink from the pending list,
 * then either flag the actor as deferred (when the global at data_02099f24
 * reads 3) or link it into both the behaviour and render lists and mark it
 * alive. */
// @symbol _ZN7fBase_c18AfterInitResourcesEj
void fBase_c::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    func_0203b27c(data_020a4b88, &manager.behaviorNode);
    volatile int *p = data_02099f24;
    bool b = (p[0] == 3);
    if (b) {
        unk_010 = 1;
        return;
    }
    func_0204405c(data_020a4b78, &manager.behaviorNode);
    func_0204405c(data_020a4b98, &manager.renderNode);
    aliveState = 1;
}

/* vtable slot 3. Default cleanup succeeds without releasing resources. */
// @symbol _ZN7fBase_c16CleanupResourcesEv
s32 fBase_c::CleanupResources()
{
    return 1;
}

/* vtable slot 4. Wait for pending lifecycle work and remaining scene children. */
// @symbol _ZN7fBase_c22BeforeCleanupResourcesEv
int fBase_c::BeforeCleanupResources()
{
    int v = (int)lifecycleState;
    if (v != 0) {
        if (func_0204424c((char *)v) == 0)
            goto ret0;
    }
    if (manager.sceneNode.child == 0)
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}

/* On success, unlink the actor, release its heap and pending lifecycle work,
 * then call the virtual complete-object destructor (slot 16) and deallocate.
 * This function is enrolled with the rest of the production TU. */
// @symbol _ZN7fBase_c21AfterCleanupResourcesEj
void fBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    func_0203b3c0(data_020a4b6c, &manager.sceneNode);
    func_0203b27c(data_020a4ba8, &manager.behaviorNode);
    if (heap)
        _ZN4Heap8_DestroyEv(heap);
    if (lifecycleState)
        func_02044334(lifecycleState);
    this->~fBase_c();   /* vtable+0x40 = slot 16 = D1 */
    _ZN6Memory10DeallocateEPvP4Heap(this, data_020a0eac);
}

/* vtable slot 6. The default update succeeds without doing any work. */
// @symbol _ZN7fBase_c8BehaviorEv
s32 fBase_c::Behavior()
{
    return 1;
}

/* vtable slot 7. Skips the tick once the actor is marked for death, or when
 * bit 1 of the spawn-flag byte is set. */
// @symbol _ZN7fBase_c14BeforeBehaviorEv
int fBase_c::BeforeBehavior()
{
    if (shouldBeKilled != 0)
        goto ret0;
    if ((pauseFlags & 2) == 0)
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}

/* vtable slot 8. Base does nothing; leaf classes override. */
// @symbol _ZN7fBase_c13AfterBehaviorEj
void fBase_c::AfterBehavior(u32 vfSuccess)
{
    u32 unused = vfSuccess;
}

/* vtable slot 9. The default render succeeds without drawing anything. */
// @symbol _ZN7fBase_c6RenderEv
s32 fBase_c::Render()
{
    return 1;
}

/* vtable slot 10. The render twin of BeforeBehavior, on bit 3 instead of 1. */
// @symbol _ZN7fBase_c12BeforeRenderEv
int fBase_c::BeforeRender()
{
    if (shouldBeKilled != 0)
        goto ret0;
    if ((pauseFlags & 8) == 0)
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}

/* vtable slot 11. Base does nothing; leaf classes override. */
// @symbol _ZN7fBase_c11AfterRenderEj
void fBase_c::AfterRender(u32 vfSuccess)
{
    u32 unused = vfSuccess;
}

/* vtable slot 12 (vtable+0x30), fired by MarkForDestruction. Base does
 * nothing; leaf classes override to release or notify. */
// @symbol _ZN7fBase_c16OnPendingDestroyEv
void fBase_c::OnPendingDestroy()
{
}

// @symbol func_02043880
/* The scene-graph list maintainer: reconciles one actor's list membership with
 * its state and its parent's flags. Unnamed in config, and it is not an
 * fBase_c member by any evidence -- but it is INSIDE the class's contiguous
 * run, between MarkForDestruction and OnPendingDestroy, and it reads the
 * fBase_c layout directly (state at 0x0e, flags at 0x13, the two list nodes
 * at 0x28 and 0x38), so nothing else can own it.
 *
 * The legacy file reached that layout through three local structs called
 * `Node`, `LNode` and `Obj`. All three names are far too generic to survive in
 * a TU that includes the project headers, so all three are PREFIXED here -- but
 * only renamed. The field accesses are left exactly as they were, offset for
 * offset, because raw-cast versus named-member is a per-FUNCTION codegen choice
 * (a named member folds into `[rN,#imm]`, a cast makes the compiler materialise
 * the address into a scratch register and reuse it) and these 0x240 bytes were
 * recovered against the casts.
 *
 * For the record, since restating a class by hand is exactly what a TU rebuild
 * is supposed to retire, here is what `Obj` maps onto in include/fBase_c.h --
 * so the next pass can try the real members one at a time and measure:
 *     0x0e state  = aliveState        0x12 pad12 = unk_012
 *     0x0f dirty  = shouldBeKilled    0x13 flags = pauseFlags
 *     0x10 f10    = unk_010           0x18 list  = manager.sceneNode.child
 *     0x11 f11    = unk_011
 * and `LNode` is SceneNode seen through its 0x0c next pointer and 0x10
 * owner fields -- so the loop below walks this actor's scene-graph children and
 * marks each one for destruction. */
struct ActorBase_PListNode {
    ActorBase_PListNode *prev;
    ActorBase_PListNode *next;
    void *owner;
    u16 c;
    u16 e;
};

struct ActorBase_ChildLink {
    char pad[0xc];
    ActorBase_ChildLink *next;
    void *obj;
};

struct ActorBase_Raw {
    char pad0[0xe];
    u8 state;                   /* 0x0e */
    u8 dirty;                   /* 0x0f */
    u8 f10;                     /* 0x10 */
    u8 f11;                     /* 0x11 */
    u8 pad12;                   /* 0x12 */
    u8 flags;                   /* 0x13 */
    int pad14;                  /* 0x14 */
    ActorBase_ChildLink *list;  /* 0x18 */
    char pad1c[0xc];
    ActorBase_PListNode n28;    /* 0x28 */
    ActorBase_PListNode n38;    /* 0x38 */
};

#define ACTORBASE_FLAGP(o) ((u8 *)((char *)(o) + 0x13))
#define ACTORBASE_NODEP(o, OFF) ((ActorBase_PListNode *)((char *)(o) + (OFF)))

extern "C" int func_02043880(ActorBase_Raw *o)
{
    if (o->dirty != 0) {
        ActorBase_ChildLink *it;
        int b = 0;
        o->dirty = b;
        if (o->state == 1) b = 1;
        if (b != 0) {
            func_0203b27c(data_020a4b78, &o->n28);
            func_0203b27c(data_020a4b98, &o->n38);
        } else {
            func_0203b27c(data_020a4b88, &o->n28);
        }
        func_0203b20c(data_020a4ba8, &o->n28);
        o->state = 2;
        for (it = o->list; it != 0; it = it->next) {
            ((fBase_c *)it->obj)->MarkForDestruction();
        }
    } else {
        u32 state;
        int b;
        ActorBase_Raw *other = (ActorBase_Raw *)func_02043810(o);
        if (other != 0) {
            {
                u8 f = other->flags;
                int t = f & 1;
                if (t == 0) {
                    t = f & 2;
                    if (t == 0) goto Lelse1;
                }
                {
                    u8 *q = ACTORBASE_FLAGP(o);
                    *q |= 2;
                }
                goto Lafter1;
            Lelse1:
                if (o->flags & 2) {
                    u8 *q = ACTORBASE_FLAGP(o);
                    *q &= ~2;
                }
            Lafter1:;
            }
            {
                u8 f = other->flags;
                int t = f & 4;
                if (t == 0) {
                    t = f & 8;
                    if (t == 0) goto Lelse2;
                }
                {
                    u8 *q = ACTORBASE_FLAGP(o);
                    *q |= 8;
                }
                goto Lafter2;
            Lelse2:
                if (o->flags & 8) {
                    u8 *q = ACTORBASE_FLAGP(o);
                    *q &= ~8;
                }
            Lafter2:;
            }
        }
        state = o->state;
        b = (int)((state == 1) ? (char *)1 : (char *)0);
        if (b != 0) {
            {
                ActorBase_PListNode *n = ACTORBASE_NODEP(o, 0x28);
                int c = (int)((n->e != n->c) ? (char *)1 : (char *)0);
                if (c != 0) {
                    ActorBase_PListNode *m;
                    func_0203b27c(data_020a4b78, &o->n28);
                    m = ACTORBASE_NODEP(o, 0x28);
                    m->c = m->e;
                    func_0204405c(data_020a4b78, m);
                }
            }
            {
                ActorBase_PListNode *n = ACTORBASE_NODEP(o, 0x38);
                int c = (int)((n->e != n->c) ? (char *)1 : (char *)0);
                if (c != 0) {
                    ActorBase_PListNode *m;
                    func_0203b27c(data_020a4b98, &o->n38);
                    m = ACTORBASE_NODEP(o, 0x38);
                    m->c = m->e;
                    func_0204405c(data_020a4b98, m);
                }
            }
        } else {
            int b2 = (int)((state == 2) ? (char *)1 : (char *)0);
            if (b2 == 0) {
                if (o->f11 != 0) {
                    o->f11 = 0;
                    func_0203b244(data_020a4b88, &o->n28);
                } else if (o->f10 != 0) {
                    o->f10 = 0;
                    func_0204405c(data_020a4b78, &o->n28);
                    func_0204405c(data_020a4b98, &o->n38);
                    o->state = 1;
                }
            }
        }
    }
    return 1;
}

/* Non-virtual. Idempotent, and a no-op once the actor is already dying;
 * otherwise sets the flag and fires slot 12. */
// @symbol _ZN7fBase_c18MarkForDestructionEv
void fBase_c::MarkForDestruction()
{
    if (shouldBeKilled != 0)
        return;
    unsigned char b = (aliveState == 2);
    if (b != 0)
        return;
    shouldBeKilled = 1;
    OnPendingDestroy();   /* vtable+0x30 = slot 12 */
}

/* Return the parent scene node's actor, or null at the root. */
// @symbol func_02043810
extern "C" void *func_02043810(void *base)
{
    fBase_c::SceneNode *parent = ((fBase_c *)base)->manager.sceneNode.parent;
    if (parent)
        return parent->owner;
    return 0;
}

/* vtable slot 13 (vtable+0x34). Builds the actor's dedicated heap. Tries the
 * requested size first, then an unbounded solid heap, then a right-sized
 * retry, calling OnHeapCreated (slot 15) inside each attempt and marking the
 * actor for destruction if none of them works.
 *
 * `this->v15()` in the legacy shadow is slot 15, which the real class names
 * OnHeapCreated -- so this now reads as the virtual it is. The heap's flags
 * word (+4) and top (+8) are still reached by raw offset: see the note on the
 * Heap declarations above. */
// @symbol _ZN7fBase_c9Virtual34Ejj
int fBase_c::Virtual34(u32 a, u32 b)
{
    void *h = 0;
    u32 avail;

    if (heap != 0)
        return 1;

    if (a != 0) {
        h = _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(a, (void *)b, 0x20);
        if (h != 0) {
            u32 flagA = (*(u32 *)((char *)h + 4)) & 0x10;
            if (flagA != 0)
                _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
            int res = OnHeapCreated();
            u32 okA;
            if (flagA != 0) {
                okA = (u32)res;
            } else {
                void *allocRes = _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
                okA = (allocRes != 0) ? (u32)res : 0;
            }
            _ZN4Heap20RestoreFromTemporaryEv();
            if (okA == 0) {
                _ZN4Heap8_DestroyEv(h);
                h = 0;
            } else {
                u32 topA = *(u32 *)((char *)h + 8);
                avail = topA - _ZN4Heap21MaxAllocationUnitSizeEv(h);
                avail = (avail + 0x1f) & ~0x1f;
                if (a == avail) {
                    _ZN4Heap11ResizeToFitEv(h);
                    heap = h;
                    return 1;
                }
            }
        }
    }

    if (h == 0) {
        h = _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i((u32)-1, (void *)b, 0x20);
        u32 flagB = (*(u32 *)((char *)h + 4)) & 0x10;
        if (flagB != 0)
            _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
        a = (u32)OnHeapCreated();
        if (flagB == 0) {
            void *allocRes2 = _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
            if (allocRes2 == 0)
                a = 0;
        }
        _ZN4Heap20RestoreFromTemporaryEv();
        if (a == 0) {
            _ZN4Heap8_DestroyEv(h);
            MarkForDestruction();
            return 0;
        }
        u32 topB = *(u32 *)((char *)h + 8);
        avail = topB - _ZN4Heap21MaxAllocationUnitSizeEv(h);
        avail = (avail + 0x1f) & ~0x1f;
    }

    if (h == 0)
        goto fail;

    {
        u32 topH = *(u32 *)((char *)h + 8);
        void *h2 = 0;
        u32 availInH = topH - _ZN4Heap21MaxAllocationUnitSizeEv(h);
        u32 needed = ((availInH + 0xf) & ~0xf) + 0x30;
        if (needed < _ZN4Heap21MaxAllocationUnitSizeEv((void *)b)) {
            h2 = _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(avail, (void *)b, 0x20);
        }
        if (h2 != 0) {
            if ((u32)h2 < (u32)h) {
                _ZN4Heap8_DestroyEv(h);
                h = 0;
                int res3 = OnHeapCreated();
                u32 okC = (u32)res3;
                _ZN4Heap20RestoreFromTemporaryEv();
                if (okC == 0) {
                    _ZN4Heap8_DestroyEv(h2);
                    h2 = h;
                }
            } else {
                _ZN4Heap20RestoreFromTemporaryEv();
                _ZN4Heap8_DestroyEv(h2);
                h2 = 0;
            }
        }
        if (h2 != 0) {
            _ZN4Heap11ResizeToFitEv(h2);
            heap = h2;
            return 1;
        }
    }

    if (h != 0) {
        _ZN4Heap11ResizeToFitEv(h);
        heap = h;
        return 1;
    }

fail:
    MarkForDestruction();
    return 0;
}

/* vtable slot 14 (vtable+0x38). The short form of Virtual34: one attempt, no
 * right-sizing retry.
 *
 * The legacy file reached Heap and Memory through local classes with real
 * methods (`h->_Destroy()`, `Heap::RestoreFromTemporary()`,
 * `Memory::Allocate(...)`) while Virtual34 reached the same ROM functions
 * through their mangled names. Both spellings compile to the same non-virtual
 * direct calls, but two definitions of `struct Heap` cannot coexist in one TU,
 * so this member is reconciled onto Virtual34's mangled-name form -- the one of
 * the two that needs no class definition at all. */
// @symbol _ZN7fBase_c9Virtual38Ejj
int fBase_c::Virtual38(u32 a, u32 b)
{
    if (heap != 0)
        return 1;
    if (a != 0) {
        void *h = _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(a, (void *)b, 0x20);
        if (h != 0) {
            int ok;
            int flag = (*(int *)((char *)h + 4)) & 0x10;
            if (flag != 0)
                _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
            ok = OnHeapCreated();
            if (flag == 0) {
                if (_ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0) == 0)
                    ok = 0;
            }
            _ZN4Heap21MaxAllocationUnitSizeEv(h);
            _ZN4Heap20RestoreFromTemporaryEv();
            if (ok == 0) {
                _ZN4Heap8_DestroyEv(h);
            } else {
                heap = h;
                return 1;
            }
        }
    }
    MarkForDestruction();
    return 0;
}

/* vtable slot 15 (vtable+0x3c), fired by Virtual34/Virtual38 once the actor's
 * heap exists. The default hook accepts the heap. */
// @symbol _ZN7fBase_c13OnHeapCreatedEv
bool fBase_c::OnHeapCreated()
{
    return 1;
}

/* Actor-heap allocation, cleared before construction. The configured ABI takes
 * unsigned; the class's size_t operator-new overload forwards here because
 * the compiler requires size_t for an in-class operator new. */
// @symbol _ZN7fBase_cnwEj
extern "C" void *_ZN7fBase_cnwEj(unsigned int size)
{
    void *p = _ZN6Memory8AllocateEjiP4Heap(size, -4, data_020a0eac);
    if (!p)
        return 0;
    func_0206e2f8(p, 0, size);
    return p;
}
