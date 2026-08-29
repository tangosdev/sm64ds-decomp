//cpp
/* SHADOW translation unit -- arm9 / fBase_c (Nintendo's fBase_c), the ROOT of
 * the actor hierarchy.
 *
 * NOT ENROLLED, AND NOT CANONICAL. This file contributes nothing to the ROM
 * build; the one-function sources under src/ remain the enrolled owners of
 * 0x02043444..0x02043dec. See notes/translation-unit-reconstruction-plan.md and
 * notes/tu-reconstruction-pilot-report.md.
 *
 * THE SPAN, RECONCILED AGAINST THE ROM. include/fBase_c.h's banner has said
 * "0x02043494..0x02043e04" and BOTH ends of that are wrong:
 *
 *   0x02043444  _ZN7fBase_cnwEj      fBase_c::operator new -- the class's
 *                                      own member, and the header's own comment
 *                                      discusses it. It is 0x50 bytes BELOW the
 *                                      old start, which excluded it.
 *   0x02043e04  is not a function boundary at all. It falls 0x18 bytes INSIDE
 *               _ZN7fBase_cC2Ev (0x02043dec, size 0x160), so the old end cut
 *               the constructor in half.
 *   0x02043f4c  is where the run really ends -- the byte after the constructor,
 *               and the address of func_02043f4c, the next unrelated function.
 *
 * The class's whole contiguous run is therefore 0x02043444..0x02043f4c, 25
 * functions. The header has been corrected. This file licenses 24 of the 25 --
 * everything up to 0x02043dec. The 25th, the constructor, is excluded on
 * purpose: see LICENSED SPAN below.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one `.text` section per function and orders those sections in the
 * REVERSE of source order, so the LAST function of the ROM run is written
 * FIRST. Reading order below is therefore the destructor triple, Process,
 * InitResources, ... , OnHeapCreated, operator new.
 *
 * THE DESTRUCTORS ARE extern "C" FREE FUNCTIONS, NOT A REAL ~fBase_c(), AND
 * THE ROM IS WHAT FORCES THAT. A real C++ destructor stores its vptr through a
 * relocation on mwcc's OWN `_ZTV7fBase_c`, which names the vtable OBJECT --
 * two words of Itanium preamble before the slot array -- so the store carries
 * addend 8. This TU does NOT define the vtable (see KEY FUNCTION below), so
 * that symbol stays external and resolves against config's `_ZTV7fBase_c`,
 * which IS the slot array at 0x02099edc. The addend would land the vptr eight
 * bytes high. The cartridge is unambiguous about the right value: the literal
 * pools of all three variants hold exactly 0x02099edc --
 *     0x02043d74 -> 0x02099edc   (D2)
 *     0x02043db4 -> 0x02099edc   (D0)
 *     0x02043de8 -> 0x02099edc   (D1)
 * -- so the reference this TU needs is addend 0, which only the free-function
 * spelling can express. arm9/dActor_c reached link-verified under the same
 * arrangement.
 *
 * The variant ORDER still comes out right, and by two independent routes. mwcc
 * emits a single out-of-line destructor definition as the fixed group D2,D0,D1,
 * which is exactly the ROM's layout here (0x02043d48, 0x02043d78, 0x02043dbc)
 * and is good evidence the original source did define one. Three separate
 * definitions written in REVERSE (D1, D0, D2, as below) reverse to the same
 * D2,D0,D1. Both stories produce these bytes; only the second also produces the
 * right vptr addend, so that is the one used.
 *
 * KEY FUNCTION -- DO NOT DEFINE InitResources AS A METHOD. It is slot 0, the
 * first virtual the class declares, so CW 1.2 emits `_ZTV7fBase_c` into
 * whichever TU defines it. The ROM's vtable is already supplied as data by the
 * module's gap object, and a second definition makes the link fail with
 * "Multiply-defined: virtual table for fBase_c". Defining it below as an
 * extern "C" free function is what keeps this TU from being the key function's
 * definition -- and therefore what leaves the other 23 members free to be real
 * C++ methods. Confirmed by this round: the compiled object emits NO .data at
 * all, no _ZTV, no _ZTI, no _ZTS.
 *
 * LICENSED SPAN 0x02043444..0x02043dec, 24 of the run's 25 functions. The
 * missing one is `_ZN7fBase_cC2Ev` (0x02043dec, 0x160), and it belongs to
 * this TU by every boundary test -- it is inside the contiguous run, it carries
 * the class's own name, and nothing else claims it. It is left out because the
 * tree has no C++ for it: src/_ZN7fBase_cC2Ev.cpp is a whole-function
 * hand-written `asm` transcription, its own banner says "NONMATCHING ... does
 * NOT count as matched", and config/arm9/delinks.txt has no entry for it at all
 * -- it is neither compiled nor enrolled today. Admitting asm here would let
 * this TU claim a range it has not reproduced from source. The manifest entry
 * records it as the one unadmitted member.
 */
#include "fBase_c.h"

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

/* ProcessingListNode::~ProcessingListNode -- torn down twice per destructor. */
extern void func_020440e8(void *node);

/* Intrusive-list operations on the four global list heads below. All four take
   (list head, node); the legacy files spelled the heads `int`, `char` and
   `int[]` in different files and passed `&x` or `x` accordingly. Unified on the
   array spelling, so every call site passes the bare name and every reloc is
   the same addend-0 reference it always was. */
extern void func_0203b3c0(void *list, void *node);
extern void func_0203b27c(void *list, void *node);
extern void func_0203b20c(void *list, void *node);
extern void func_0203b244(void *list, void *node);
extern void func_0204405c(void *list, void *node);

extern int  data_020a4b6c[];
extern int  data_020a4b78[];
extern int  data_020a4b88[];
extern int  data_020a4b98[];
extern int  data_020a4ba8[];
extern int  data_02099f24[];

extern void func_02044334(void *p);
extern int  func_0204424c(int p);
extern void func_0206e2f8(void *p, int fill, unsigned int size);

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
extern void  _ZN4Heap11ResizeToFitEv(void *h);

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
extern "C" fBase_c *_ZN7fBase_cD1Ev(fBase_c *self)
{
    *(int *)self = (int)_ZTV7fBase_c;
    func_020440e8(&self->manager.renderNode);
    func_020440e8(&self->manager.behaviorNode);
    return self;
}

extern "C" fBase_c *_ZN7fBase_cD0Ev(fBase_c *self)
{
    *(int *)self = (int)_ZTV7fBase_c;
    func_020440e8(&self->manager.renderNode);
    func_020440e8(&self->manager.behaviorNode);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}

extern "C" fBase_c *_ZN7fBase_cD2Ev(fBase_c *self)
{
    *(int *)self = (int)_ZTV7fBase_c;
    func_020440e8(&self->manager.renderNode);
    func_020440e8(&self->manager.behaviorNode);
    return self;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 20 -- fBase_c::Process, 0x02043c88, size 0xc0               */
/* ------------------------------------------------------------------------- */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 19 -- fBase_c::InitResources, 0x02043c80, size 0x8          */
/* ------------------------------------------------------------------------- */
/* vtable slot 0. Base loads nothing and returns VS_FAIL (1).
 *
 * THE KEY FUNCTION, AND THE REASON THIS ONE IS NOT A METHOD. See the file
 * banner: a real `s32 fBase_c::InitResources()` anywhere makes that TU emit
 * _ZTV7fBase_c and collide with the gap object's copy of the ROM's own
 * vtable. Its DECLARATION in the class is required and harmless -- removing it
 * would delete slot 0 and shift the other seventeen. */
extern "C" int _ZN7fBase_c13InitResourcesEv(void)
{
    return 1; /* VS_FAIL */
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 18 -- fBase_c::BeforeInitResources, 0x02043c78, size 0x8    */
/* ------------------------------------------------------------------------- */
/* vtable slot 1, the init guard. Base returns VS_FAIL (1). */
bool fBase_c::BeforeInitResources()
{
    return 1; /* VS_FAIL */
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 17 -- fBase_c::AfterInitResources(u32), 0x02043bf8, 0x80    */
/* ------------------------------------------------------------------------- */
/* vtable slot 2. Only acts on VS_SUCCESS (2): unlink from the pending list,
 * then either flag the actor as deferred (when the global at data_02099f24
 * reads 3) or link it into both the behaviour and render lists and mark it
 * alive. */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 16 -- fBase_c::CleanupResources, 0x02043bf0, size 0x8       */
/* ------------------------------------------------------------------------- */
/* vtable slot 3. Base releases nothing and returns VS_FAIL (1). */
s32 fBase_c::CleanupResources()
{
    return 1; /* VS_FAIL */
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 15 -- fBase_c::BeforeCleanupResources, 0x02043bac, 0x44     */
/* ------------------------------------------------------------------------- */
/* vtable slot 4. Refuses cleanup while lifecycleState is still busy, or once the
 * scene node has been unlinked. */
int fBase_c::BeforeCleanupResources()
{
    int v = (int)lifecycleState;
    if (v != 0) {
        if (func_0204424c(v) == 0)
            goto ret0;
    }
    if (manager.sceneNode.child == 0)
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 14 -- fBase_c::AfterCleanupResources(u32), 0x02043b2c, 0x80 */
/* ------------------------------------------------------------------------- */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 13 -- fBase_c::Behavior, 0x02043b24, size 0x8              */
/* ------------------------------------------------------------------------- */
/* vtable slot 6, the per-frame update tick. Base does nothing, VS_FAIL (1). */
s32 fBase_c::Behavior()
{
    return 1; /* VS_FAIL */
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 12 -- fBase_c::BeforeBehavior, 0x02043afc, size 0x28       */
/* ------------------------------------------------------------------------- */
/* vtable slot 7. Skips the tick once the actor is marked for death, or when
 * bit 1 of the spawn-flag byte is set. */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 11 -- fBase_c::AfterBehavior(u32), 0x02043af8, size 0x4    */
/* ------------------------------------------------------------------------- */
/* vtable slot 8. Base does nothing; leaf classes override. */
void fBase_c::AfterBehavior(u32 vfSuccess)
{
    u32 unused = vfSuccess;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 10 -- fBase_c::Render, 0x02043af0, size 0x8                */
/* ------------------------------------------------------------------------- */
/* vtable slot 9. Base draws nothing and returns VS_FAIL (1). */
s32 fBase_c::Render()
{
    return 1; /* VS_FAIL */
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 9 -- fBase_c::BeforeRender, 0x02043ac8, size 0x28          */
/* ------------------------------------------------------------------------- */
/* vtable slot 10. The render twin of BeforeBehavior, on bit 3 instead of 1. */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- fBase_c::AfterRender(u32), 0x02043ac4, size 0x4       */
/* ------------------------------------------------------------------------- */
/* vtable slot 11. Base does nothing; leaf classes override. */
void fBase_c::AfterRender(u32 vfSuccess)
{
    u32 unused = vfSuccess;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- fBase_c::OnPendingDestroy, 0x02043ac0, size 0x4       */
/* ------------------------------------------------------------------------- */
/* vtable slot 12 (vtable+0x30), fired by MarkForDestruction. Base does
 * nothing; leaf classes override to release or notify. */
void fBase_c::OnPendingDestroy()
{
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_02043880, 0x02043880, size 0x240                   */
/* ------------------------------------------------------------------------- */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- fBase_c::MarkForDestruction, 0x02043824, size 0x5c    */
/* ------------------------------------------------------------------------- */
/* Non-virtual. Idempotent, and a no-op once the actor is already dying;
 * otherwise sets the flag and fires slot 12. */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_02043810, 0x02043810, size 0x14                    */
/* ------------------------------------------------------------------------- */
/* The parent-actor accessor: follow manager.sceneNode.parent (0x14 -- the
 * legacy file read it as p[0x14/4], i.e. the first word of the manager) and
 * return the owner back-pointer the constructor writes at its +0x10. Reads as
 * fBase_c but is unnamed in config, and func_02043880 above is its only
 * caller in this run. */
extern "C" void *func_02043810(void *p)
{
    int *q = (int *)((int *)p)[0x14 / 4];
    if (q)
        return (void *)q[0x10 / 4];
    return 0;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- fBase_c::Virtual34(u32, u32), 0x0204357c, size 0x294  */
/* ------------------------------------------------------------------------- */
/* vtable slot 13 (vtable+0x34). Builds the actor's dedicated heap. Tries the
 * requested size first, then an unbounded solid heap, then a right-sized
 * retry, calling OnHeapCreated (slot 15) inside each attempt and marking the
 * actor for destruction if none of them works.
 *
 * `this->v15()` in the legacy shadow is slot 15, which the real class names
 * OnHeapCreated -- so this now reads as the virtual it is. The heap's flags
 * word (+4) and top (+8) are still reached by raw offset: see the note on the
 * Heap declarations above. */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- fBase_c::Virtual38(u32, u32), 0x0204349c, size 0xe0   */
/* ------------------------------------------------------------------------- */
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 1 -- fBase_c::OnHeapCreated, 0x02043494, size 0x8          */
/* ------------------------------------------------------------------------- */
/* vtable slot 15 (vtable+0x3c), fired by Virtual34/Virtual38 once the actor's
 * heap exists. Base returns VS_FAIL (1); leaf classes override. */
bool fBase_c::OnHeapCreated()
{
    return 1; /* VS_FAIL */
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 0 -- fBase_c::operator new(u32), 0x02043444, size 0x50     */
/* ------------------------------------------------------------------------- */
/* THE LOWEST FUNCTION OF THE RUN, and the one include/fBase_c.h's old
 * 0x02043494 start excluded. Every actor factory in the image calls it -- the
 * literal it is passed is how this project reads each class's size -- so it is
 * unambiguously this class's member.
 *
 * NOT declared in the class, and that is not a choice either: CW 1.2 rejects an
 * in-class declaration of operator new outright ("illegal 'operator'
 * declaration"). Its counterpart operator delete IS accepted in-class and is
 * declared there, which is what lets the destructors above reproduce D0. */
extern "C" void *_ZN7fBase_cnwEj(unsigned int size)
{
    void *p = _ZN6Memory8AllocateEjiP4Heap(size, -4, data_020a0eac);
    if (!p)
        return 0;
    func_0206e2f8(p, 0, size);
    return p;
}
