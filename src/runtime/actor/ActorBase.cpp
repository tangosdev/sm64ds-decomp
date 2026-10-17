//cpp
/* leftovers:
 * - Source order stays highest address first. Reordering moves the run.
 * - The native destructor owns both process-list node lifetimes. The
 *   existing preparation path rebiases its vptr references to ROM slot arrays.
 * - Native InitResources emits the class metadata. The manifest verifies
 *   and externalizes it to the existing canonical ROM storage.
 * - decl_common.h stays out. Its spellings of these list heads
 *   disagree with the calls in this file.
 * - Heap locals stay void*. Virtual34 and Virtual38 still read
 *   the flags word at +4 and the heap top at +8.
 * - func_0203* and data_020a4b* keep their linker names. They are
 *   the list operations and the four global list heads.
 *
 * fBase_c, the root of the actor hierarchy. arm9 .text
 * 0x020433b8..0x020440e8.
 */
#include "fBase_c.h"
#include "Heap.h"

struct fBaseActorInfo {
    u32 unk_000;
    u16 behaviorPriority;
    u16 renderPriority;
};

/* Keep the list-head and pointer signatures used here local: decl_common.h
 * gives incompatible integer declarations for some of these symbols. */
extern "C" {

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
extern int  func_0203b2ec(void *l, void *n, void *p);

extern void *data_020a4b68;
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
extern int func_0204335c(void *self);
extern int  func_0204424c(char *p);
extern void *func_0206e2f8(void *p, int fill, unsigned int size);

/* Returns the parent actor. Declared once, up here, because the
 * caller is emitted above the definition. */
extern void *func_02043810(void *p);

/* Heap's methods come from include/Heap.h. The heap locals stay `void *`
   (and are cast at each call), deliberately: Virtual34 and Virtual38 both
   reach two of its fields by raw offset (`+4` flags word, `+8` top-of-heap)
   rather than by member, and reconciling those onto named members is a
   per-function codegen change this round has no reason to risk.
   include/Heap.h itself declares InitializeSolidHeapAsDefault as returning
   `void *`. Memory::Deallocate and data_020a0eac come from include/fBase_c.h,
   which needs them for its inline operator delete. */
extern void *_ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(unsigned int size, int align, void *heap);

}

/* aliveState is 0 before the actor joins the lists, 1 while it is alive
 * in them and 2 once it is dying; data_02099f24[0] is a global mode word.
 * These int-returning predicates are what make the ROM materialize 0/1
 * before testing it. */
static inline int IsPendingState(u32 state) {
    return state == 0;
}
static inline int IsAliveState(u32 state) {
    return state == 1;
}
static inline int IsDyingState(u32 state) {
    return state == 2;
}
static inline int GlobalModeIs(volatile int *mode, int value) {
    return mode[0] == value;
}
static inline int PriorityChanged(fLiNdBaPr_c *node) {
    return node->nextPriority != node->currentPriority;
}
static inline int OwnerUniqueIDDiffers(fLiNdBa_c *node, unsigned int id) {
    return node->owner->uniqueID != id;
}
static inline int OwnerActorIDDiffers(fLiNdBa_c *node, unsigned int id) {
    return node->owner->actorID != id;
}

/* List helpers keep address-named C entries. The sorting and process-list
 * descriptors retain local views; actor lookups use real actor-owned nodes. */

// @symbol func_0204405c
namespace Absorb0204405c {
struct N {
    struct N *prev;
    struct N *next;
    int pad8;
    volatile unsigned short c;
};
struct L {
    struct N *head;
    struct N *tail;
};

extern "C" int func_0204405c(void *list, void *nodep)
{
    struct L *l = (struct L *)list;
    struct N *n = (struct N *)nodep;
    struct N *node;
    struct N *next;
    node = l->head;
    if (n == 0)
        return 0;
    if (node == 0)
        return func_0203b244(l, n);
    if (node->c > n->c)
        return func_0203b2ec(l, n, 0);
    while ((next = node->next) != 0 && next->c <= n->c)
        node = next;
    return func_0203b2ec(l, n, node);
}
}

/* Process lists hold real actor-owned nodes. Their descriptors contain the
 * existing frame callback records; all four current wrappers return int,
 * which this traversal discards. Those address-named free ABI wrappers are
 * still viewed through a member pointer here, not recovered native methods.
 * Save the next node before invoking a callback that may unlink its owner. */
// @symbol func_02043fdc
namespace Absorb02043fdc {
typedef int (fBase_c::*ProcessCallback)();

/* Local descriptor name is inferred from the list primitives and frame loop. */
struct ProcessListDescriptor {
    fLiNdBaPr_c *head;       // 0x0
    fLiNdBaPr_c *tail;       // 0x4
    ProcessCallback callback; // 0x8
};

extern "C" void *func_02043fdc(ProcessListDescriptor *list)
{
    fLiNdBaPr_c *node;
    if (list->callback == 0) return (void *)1;
    node = list->head;
    while (node != 0) {
        fLiNdBaPr_c *next;
        data_020a4b68 = node;
        next = node->next;
        (node->owner->*list->callback)();
        node = next;
    }
    data_020a4b68 = node;
    return (void *)1;
}
}

// @symbol func_02043f98
namespace Absorb02043f98 {
extern "C" fLiNdBa_c *func_02043f98(fLiNdBa_c **head, unsigned int key)
{
    fLiNdBa_c *node = *head;
    while (node != 0) {
        if (!OwnerUniqueIDDiffers(node, key))
            return node;
        node = node->next;
    }
    return 0;
}
}

// @symbol func_02043f4c
namespace Absorb02043f4c {
extern "C" fLiNdBa_c *func_02043f4c(fLiNdBa_c **head, unsigned int key,
                                    fLiNdBa_c *after)
{
    fLiNdBa_c *node = after ? after->next : *head;
    while (node != 0) {
        if (!OwnerActorIDDiffers(node, key))
            return node;
        node = node->next;
    }
    return 0;
}
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

/* Manager destruction tears down renderNode before behaviorNode. The compiler
 * emits D2, D0, D1 in ROM order; the existing class operator delete returns
 * the object to the actor heap for D0. */
// @symbol _ZN7fBase_cD1Ev
fBase_c::~fBase_c()
{
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
 * This key function emits metadata checked against canonical ROM storage. */
// @symbol _ZN7fBase_c13InitResourcesEv
int fBase_c::InitResources()
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
    if (GlobalModeIs(data_02099f24, 3)) {
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
    if ((v != 0 && func_0204424c((char *)v) == 0) || manager.sceneNode.child != 0)
        return 0;
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
        ((Heap *)heap)->_Destroy();
    if (lifecycleState)
        func_02044334(lifecycleState);
    this->~fBase_c();   /* vtable+0x40 = slot 16 = D1 */
    _ZN6Memory10DeallocateEPvPN5mHeap6Heap_tE(this, data_020a0eac);
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
    if (shouldBeKilled != 0 || (pauseFlags & 2) != 0)
        return 0;
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
    if (shouldBeKilled != 0 || (pauseFlags & 8) != 0)
        return 0;
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
/* Reconcile list membership, inherited pause flags and pending destruction.
 * This routine uses the existing fBase_c and intrusive-node fields: scene
 * children link through SceneNode::next, and each node names its actor owner.
 * Replacing the local layout copies and offset casts with these members keeps
 * the complete raw and normally prepared objects identical under 2004/b56.
 * The address-named free function is retained; its original name and whether
 * it was a member are not established by the field accesses. */
extern "C" int func_02043880(fBase_c *o)
{
    if (o->shouldBeKilled != 0) {
        fBase_c::SceneNode *it;
        o->shouldBeKilled = 0;
        if (IsAliveState(o->aliveState)) {
            func_0203b27c(data_020a4b78, &o->manager.behaviorNode);
            func_0203b27c(data_020a4b98, &o->manager.renderNode);
        } else {
            func_0203b27c(data_020a4b88, &o->manager.behaviorNode);
        }
        func_0203b20c(data_020a4ba8, &o->manager.behaviorNode);
        o->aliveState = 2;
        for (it = o->manager.sceneNode.child; it != 0; it = it->next) {
            it->owner->MarkForDestruction();
        }
    } else {
        u32 state;
        fBase_c *other = (fBase_c *)func_02043810(o);
        if (other != 0) {
            if ((other->pauseFlags & 1) || (other->pauseFlags & 2))
                o->pauseFlags |= 2;
            else if (o->pauseFlags & 2)
                o->pauseFlags &= ~2;
            if ((other->pauseFlags & 4) || (other->pauseFlags & 8))
                o->pauseFlags |= 8;
            else if (o->pauseFlags & 8)
                o->pauseFlags &= ~8;
        }
        state = o->aliveState;
        if (IsAliveState(state)) {
            {
                fLiNdBaPr_c *n = (&o->manager.behaviorNode);
                if (PriorityChanged(n)) {
                    fLiNdBaPr_c *m;
                    func_0203b27c(data_020a4b78, &o->manager.behaviorNode);
                    m = (&o->manager.behaviorNode);
                    m->currentPriority = m->nextPriority;
                    func_0204405c(data_020a4b78, m);
                }
            }
            {
                fLiNdBaPr_c *n = (&o->manager.renderNode);
                if (PriorityChanged(n)) {
                    fLiNdBaPr_c *m;
                    func_0203b27c(data_020a4b98, &o->manager.renderNode);
                    m = (&o->manager.renderNode);
                    m->currentPriority = m->nextPriority;
                    func_0204405c(data_020a4b98, m);
                }
            }
        } else if (!IsDyingState(state)) {
            if (o->unk_011 != 0) {
                o->unk_011 = 0;
                func_0203b244(data_020a4b88, &o->manager.behaviorNode);
            } else if (o->unk_010 != 0) {
                o->unk_010 = 0;
                func_0204405c(data_020a4b78, &o->manager.behaviorNode);
                func_0204405c(data_020a4b98, &o->manager.renderNode);
                o->aliveState = 1;
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
    if (IsDyingState(aliveState))
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
                _ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(0x10, 4, 0);
            int res = OnHeapCreated();
            u32 okA;
            if (flagA != 0) {
                okA = (u32)res;
            } else {
                void *allocRes = _ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(0x10, 4, 0);
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
            _ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(0x10, 4, 0);
        a = (u32)OnHeapCreated();
        if (flagB == 0) {
            void *allocRes2 = _ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(0x10, 4, 0);
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

    if (h != 0) {
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
        if (h != 0) {
            ((Heap *)h)->ResizeToFit();
            heap = h;
            return 1;
        }
    }

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
                _ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(0x10, 4, 0);
            ok = OnHeapCreated();
            if (flag == 0) {
                if (_ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(0x10, 4, 0) == 0)
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
    void *p = _ZN6Memory8AllocateEjiPN5mHeap6Heap_tE(size, -4, data_020a0eac);
    if (!p)
        return 0;
    func_0206e2f8(p, 0, size);
    return p;
}

/* ROM ordinal 0 -- the run's backward neighbour, absorbed (reverse
 * emission puts the lowest address last). Body as recovered. */

// @symbol func_020433b8
extern "C" {
void func_020433b8(char* self)
{
    fBase_c *actor = (fBase_c *)self;
    func_0204335c(self);
    if (actor->shouldBeKilled != 0) return;
    if (actor->unk_010 != 0) return;
    if (!IsPendingState(actor->aliveState))
        return;
    if (GlobalModeIs(data_02099f24, 2)) {
        actor->unk_011 = 1;
        return;
    }
    func_0203b244(data_020a4b88, &actor->manager.behaviorNode);
}
}
