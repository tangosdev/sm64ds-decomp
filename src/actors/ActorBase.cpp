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
#include "Heap.h"

struct fBaseActorInfo {
    u32 unk_000;
    u16 behaviorPriority;
    u16 renderPriority;
};

/* ------------------------------------------------------------------------- *
 * RECONCILED DECLARATIONS
 *
 * include/decl_common.h is deliberately NOT included, and this is the case its
 * warning in the skill notes describes. It declares three of the names below
 * with signatures that CONTRADICT the way this class's own sources use them:
 *
 *     decl_common.h                   what these functions are actually passed
 *     extern void func_0203b27c(int, int);   (&list, &node) -- two pointers
 *     extern int  data_020a4b6c;             taken by address as a list head
 *     extern int  data_020a4b48/54/60/...    (unused here)
 *
 * Seen from one TU those become two incompatible extern "C" declarations of one
 * name, which mwcc rejects outright as "illegal function overloading". The
 * skill's rule is to drop the header and restate the few lines it supplied,
 * keeping the MOST COMPLETE observation of each -- which is what follows. Every
 * one of these appeared in two or more of the legacy one-function files; where
 * they disagreed only in `int` vs `void *` vs `char` spelling of the same
 * address, the pointer spelling is kept.
 * ------------------------------------------------------------------------- */
extern "C" {

/* The node destructor each fBase_c destructor variant runs twice.
 *
 * NAME CORRECTED. This block used to declare it `func_020440e8`, and so does
 * include/decl_common.h. NO MODULE EXPORTS THAT NAME: arm9's own
 * config/arm9/symbols.txt:1842 gives 0x020440e8 as
 * `_ZN11fLiNdBaPr_cD1Ev kind:function(arm,size=0x1c)`, and there is a source
 * for it at src/_ZN11fLiNdBaPr_cD1Ev.cpp. A shadow TU is not in the build, so
 * the fabricated name compiled and byte-matched here and would only have
 * surfaced as `Undefined : "func_020440e8"` at ROM link time, on promotion --
 * the same failure mode daObjKm3_Dorifu_c hit. Declared under the real mangled
 * name: same address, same relocation, a reference that resolves.
 *
 * This is still a poorer recovery than the legacy one-function file that held
 * fBase_c::~fBase_c,
 * which is a real `fBase_c::~fBase_c() {}` and lets the compiler synthesize
 * both member teardowns. Restoring that here means unwinding the three
 * hand-mangled D0/D1/D2 definitions below, and a hand-mangled D0 beside a real
 * destructor is the known mwccarm ICE (ELFgen.c:483). Left for the promotion
 * of this TU, which has to solve the destructor shape anyway. */
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

/* Heap's methods come from include/Heap.h. The heap locals stay `void *`
   (and are cast at each call), deliberately: Virtual34 and Virtual38 both
   reach two of its fields by raw offset (`+4` flags word, `+8` top-of-heap)
   rather than by member, and reconciling those onto named members is a
   per-function codegen change this round has no reason to risk.
   include/Heap.h itself declares InitializeSolidHeapAsDefault as returning
   `void *`. Memory::Deallocate and data_020a0eac come from include/fBase_c.h,
   which needs them for its inline operator delete. */
extern void *_ZN6Memory8AllocateEjiP4Heap(unsigned int size, int align, void *heap);

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

/* The per-frame driver: run the `before` guard, then the work, then the
 * `after` hook with a VirtualFuncSuccess code derived from the work's result.
 * The three arguments are POINTERS TO MEMBER FUNCTIONS -- the mangled name says
 * so (`MS_FivE`, `MS_FbvE`, `MS_FvjE` = pointer-to-member-of-fBase_c), and
 * that is the one part of a mangled parameter list this project did not have to
 * guess, because no other type spells `M`.
 *
 * KEPT AS AN extern "C" FREE FUNCTION WITH AN EXPLICIT `self`. The mangled name
 * describes a real method, so the honest form would be a member. It is not used
 * here for a measured reason: a pointer-to-member of a POLYMORPHIC class is a
 * different representation from one of the flat `struct fBase_c { int v0(); }`
 * the legacy file declared, and the legacy shadow is what these bytes were
 * recovered against. Swapping in the real class here is a codegen change, not a
 * spelling change. The local shadow it needed could not survive in this TU
 * (the name collides with the real class), so the parameters are declared with
 * a distinctly-named stand-in instead and the definition stays extern "C".
 */
struct ActorBase_ProcessSelf {
    int v0();
};
typedef int  (ActorBase_ProcessSelf::*ActorBase_PMFi)();
typedef void (ActorBase_ProcessSelf::*ActorBase_PMFv)(int);

// @symbol _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE
extern "C" int _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(
    ActorBase_ProcessSelf *self, ActorBase_PMFi b, ActorBase_PMFi a,
    ActorBase_PMFv c)
{
    int r = (self->*a)();
    int code;
    if (r != 0) {
        r = (self->*b)();
        if (r == -1) code = 3;
        else if (r == 1) code = 2;
        else code = 1;
    } else {
        code = 0;
    }
    (self->*c)(code);
    return r;
}

/* vtable slot 0. Base loads nothing and returns VS_FAIL (1).
 *
 * THE KEY FUNCTION, AND THE REASON THIS ONE IS NOT A METHOD. See the file
 * banner: a real `s32 fBase_c::InitResources()` anywhere makes that TU emit
 * _ZTV7fBase_c and collide with the gap object's copy of the ROM's own
 * vtable. Its DECLARATION in the class is required and harmless -- removing it
 * would delete slot 0 and shift the other seventeen. */
// @symbol _ZN7fBase_c13InitResourcesEv
extern "C" int _ZN7fBase_c13InitResourcesEv(void)
{
    return 1; /* VS_FAIL */
}

/* vtable slot 1, the init guard. Base returns VS_FAIL (1). */
// @symbol _ZN7fBase_c19BeforeInitResourcesEv
bool fBase_c::BeforeInitResources()
{
    return 1; /* VS_FAIL */
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
    func_0203b27c(data_020a4b88, ((char *)this) + 0x28);
    volatile int *p = data_02099f24;
    bool b = (p[0] == 3);
    if (b) {
        *(bool *)((char *)&unk_010) = true;
        return;
    }
    func_0204405c(data_020a4b78, ((char *)this) + 0x28);
    func_0204405c(data_020a4b98, ((char *)this) + 0x38);
    *(bool *)((char *)&aliveState) = true;
}

/* vtable slot 3. Base releases nothing and returns VS_FAIL (1). */
// @symbol _ZN7fBase_c16CleanupResourcesEv
s32 fBase_c::CleanupResources()
{
    return 1; /* VS_FAIL */
}

/* vtable slot 4. Refuses cleanup while lifecycleState is still busy, or once the
 * scene node has been unlinked. */
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

/* vtable slot 5, and the one member that destroys the object. Only runs on
 * VS_SUCCESS (2): unlink the scene node and the behaviour node, tear down the
 * actor's own heap and lifecycle state, then run the destructor and free.
 *
 * THE DESTRUCTOR CALL IS A VIRTUAL DISPATCH THROUGH vtable+0x40, AND THAT IS
 * SLOT 16 -- the D1 complete-object destructor, NOT OnPendingDestroy, which is
 * slot 12 at vtable+0x30. include/fBase_c.h records the same thing: calling
 * vtable+0x40 OnPendingDestroy would describe a leak, because the deallocation
 * on the next line is separate.
 *
 * Written as `this->~fBase_c()`, which is a real virtual call: for a class
 * with a virtual destructor an explicit pseudo-destructor call dispatches on
 * the dynamic type, so it lands on slot 16 exactly as the ROM does. The legacy
 * file had to fake this with a shadow class carrying a dummy
 * `virtual void Destructor();` at index 16, and that shadow could not survive
 * in this TU -- its name is the real class's.
 *
 * NOTE ON WHAT THIS MEMBER'S ENROLMENT STATUS WAS: this is the one function of
 * the 24 whose legacy file is NOT marked `complete` in config/arm9/delinks.txt,
 * so it is not compiled by the ROM build today; dsd supplies its bytes from the
 * cartridge instead. It does reproduce -- verified under the pin both as the
 * legacy file and here -- but "matching" and "enrolled" are different
 * questions, and this one was only ever the former. */
// @symbol _ZN7fBase_c21AfterCleanupResourcesEj
void fBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    func_0203b3c0(data_020a4b6c, &manager.sceneNode);
    func_0203b27c(data_020a4ba8, &manager.behaviorNode);
    if (heap)
        ((Heap *)heap)->_Destroy();
    if (lifecycleState)
        func_02044334(lifecycleState);
    this->~fBase_c();   /* vtable+0x40 = slot 16 = D1 */
    _ZN6Memory10DeallocateEPvP4Heap(this, data_020a0eac);
}

/* vtable slot 6, the per-frame update tick. Base does nothing, VS_FAIL (1). */
// @symbol _ZN7fBase_c8BehaviorEv
s32 fBase_c::Behavior()
{
    return 1; /* VS_FAIL */
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

/* vtable slot 9. Base draws nothing and returns VS_FAIL (1). */
// @symbol _ZN7fBase_c6RenderEv
s32 fBase_c::Render()
{
    return 1; /* VS_FAIL */
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

/* The parent-actor accessor: follow manager.sceneNode.parent (0x14 -- the
 * legacy file read it as p[0x14/4], i.e. the first word of the manager) and
 * return the owner back-pointer the constructor writes at its +0x10. Reads as
 * fBase_c but is unnamed in config, and func_02043880 above is its only
 * caller in this run. */
// @symbol func_02043810
extern "C" void *func_02043810(void *base)
{
    int *q = (int *)((int *)base)[0x14 / 4];
    if (q)
        return (void *)q[0x10 / 4];
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
 * Heap note above. */
// @symbol _ZN7fBase_c9Virtual34Ejj
int fBase_c::Virtual34(u32 a, u32 b)
{
    void *h = 0;
    u32 avail;

    if (heap != 0)
        return 1;

    if (a != 0) {
        h = Heap::InitializeSolidHeapAsDefault(a, (Heap *)b, 0x20);
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
            Heap::RestoreFromTemporary();
            if (okA == 0) {
                ((Heap *)h)->_Destroy();
                h = 0;
            } else {
                u32 topA = *(u32 *)((char *)h + 8);
                avail = topA - ((Heap *)h)->MaxAllocationUnitSize();
                avail = (avail + 0x1f) & ~0x1f;
                if (a == avail) {
                    ((Heap *)h)->ResizeToFit();
                    heap = h;
                    return 1;
                }
            }
        }
    }

    if (h == 0) {
        h = Heap::InitializeSolidHeapAsDefault((u32)-1, (Heap *)b, 0x20);
        u32 flagB = (*(u32 *)((char *)h + 4)) & 0x10;
        if (flagB != 0)
            _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
        a = (u32)OnHeapCreated();
        if (flagB == 0) {
            void *allocRes2 = _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
            if (allocRes2 == 0)
                a = 0;
        }
        Heap::RestoreFromTemporary();
        if (a == 0) {
            ((Heap *)h)->_Destroy();
            MarkForDestruction();
            return 0;
        }
        u32 topB = *(u32 *)((char *)h + 8);
        avail = topB - ((Heap *)h)->MaxAllocationUnitSize();
        avail = (avail + 0x1f) & ~0x1f;
    }

    if (h == 0)
        goto fail;

    {
        u32 topH = *(u32 *)((char *)h + 8);
        void *h2 = 0;
        u32 availInH = topH - ((Heap *)h)->MaxAllocationUnitSize();
        u32 needed = ((availInH + 0xf) & ~0xf) + 0x30;
        if (needed < ((Heap *)b)->MaxAllocationUnitSize()) {
            h2 = Heap::InitializeSolidHeapAsDefault(avail, (Heap *)b, 0x20);
        }
        if (h2 != 0) {
            if ((u32)h2 < (u32)h) {
                ((Heap *)h)->_Destroy();
                h = 0;
                int res3 = OnHeapCreated();
                u32 okC = (u32)res3;
                Heap::RestoreFromTemporary();
                if (okC == 0) {
                    ((Heap *)h2)->_Destroy();
                    h2 = h;
                }
            } else {
                Heap::RestoreFromTemporary();
                ((Heap *)h2)->_Destroy();
                h2 = 0;
            }
        }
        if (h2 != 0) {
            ((Heap *)h2)->ResizeToFit();
            heap = h2;
            return 1;
        }
    }

    if (h != 0) {
        ((Heap *)h)->ResizeToFit();
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
 * Heap is reached through include/Heap.h's real methods, the same
 * non-virtual direct calls Virtual34 makes. */
// @symbol _ZN7fBase_c9Virtual38Ejj
int fBase_c::Virtual38(u32 a, u32 b)
{
    if (heap != 0)
        return 1;
    if (a != 0) {
        void *h = Heap::InitializeSolidHeapAsDefault(a, (Heap *)b, 0x20);
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
            ((Heap *)h)->MaxAllocationUnitSize();
            Heap::RestoreFromTemporary();
            if (ok == 0) {
                ((Heap *)h)->_Destroy();
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
 * heap exists. Base returns VS_FAIL (1); leaf classes override. */
// @symbol _ZN7fBase_c13OnHeapCreatedEv
bool fBase_c::OnHeapCreated()
{
    return 1; /* VS_FAIL */
}

/* THE LOWEST FUNCTION OF THE RUN, and the one include/fBase_c.h's old
 * 0x02043494 start excluded. Every actor factory in the image calls it -- the
 * literal it is passed is how this project reads each class's size -- so it is
 * unambiguously this class's member.
 *
 * NOT declared in the class, and that is not a choice either: CW 1.2 rejects an
 * in-class declaration of operator new outright ("illegal 'operator'
 * declaration"). Its counterpart operator delete IS accepted in-class and is
 * declared there, which is what lets the destructors above reproduce D0. */
// @symbol _ZN7fBase_cnwEj
extern "C" void *_ZN7fBase_cnwEj(unsigned int size)
{
    void *p = _ZN6Memory8AllocateEjiP4Heap(size, -4, data_020a0eac);
    if (!p)
        return 0;
    func_0206e2f8(p, 0, size);
    return p;
}
