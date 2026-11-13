//cpp
/* leftovers:
 * - Source order stays highest address first. Reordering moves the run.
 * - The native destructor owns both process-list node lifetimes. The
 *   existing preparation path rebiases its vptr references to ROM slot arrays.
 * - Native InitResources emits the class metadata. The manifest verifies
 *   and externalizes it to the existing canonical ROM storage.
 * - decl_common.h stays out. Its spellings of these list heads
 *   disagree with the calls in this file.
 * - Heap locals stay void*, cast to Heap at each use. Virtual34 and
 *   Virtual38 test bit 4 of heapStart (+4: an arena that does not start
 *   on a 0x20 boundary gets a 0x10-byte pad allocation) and read
 *   heapSize (+8) less MaxAllocationUnitSize as the bytes in use. Older
 *   notes called these the flags word and the heap top; Heap.h's
 *   constructor evidence names them.
 * - func_0203* and data_020a4b* keep their linker names. They are
 *   the list operations and the four global list heads.
 *
 * fBase_c, the root of the actor hierarchy. arm9 .text
 * 0x02042fe4..0x020440e8. Below runCreate sit the four Process
 * wrappers, the child-creation check d_scene.cpp polls, and the create
 * entry fBase_make with its hooks and the two creation calls
 * ActorDerived.cpp makes. The run has NSMBW's f_base.cpp layout, and the
 * address-named members took that file's names where the bodies match:
 * connectProc, getConnectParent, runCreate, createPack, deletePack,
 * executePack, drawPack, checkChildProcessCreateState, setTmpCtData,
 * fBase_make, createChild and createRoot.
 */
#include "fBase_c.h"
#include "Heap.h"
#include "Memory.h"
#include "msl/string.h"

struct fBaseActorInfo {
    fBase_c *(*classInit)();
    u16 behaviorPriority;
    u16 renderPriority;
};

/* Keep the list-head and pointer signatures used here local: decl_common.h
 * gives incompatible integer declarations for some of these symbols. */

/* Intrusive-list operations on the four global list heads below. All four take
   (list head, node); the legacy files spelled the heads `int`, `char` and
   `int[]` in different files and passed `&x` or `x` accordingly. Unified on the
   array spelling, so every call site passes the bare name and every reloc is
   the same addend-0 reference it always was. */
namespace Absorb0203b3c0 {
struct Node;
int func_0203b3c0(int *list, Node *node);
}
namespace Absorb0203b438 {
struct N;
int func_0203b438(N *root, N *node, N *parent);
}
extern "C" {

extern int  func_0203b27c(void *list, void *node);
extern int  func_0203b20c(void *list, void *node);
extern int  func_0203b244(void *list, void *node);
}

extern int  func_0204405c(void *list, void *node);
extern "C" {

extern int  func_0203b2ec(void *l, void *n, void *p);
}

extern void *data_020a4b68;
extern int  data_020a4b6c[];
extern int  data_020a4b78[];
extern int  data_020a4b88[];
extern int  data_020a4b98[];
extern int  data_020a4ba8[];
extern int data_02099f24;
extern u32  data_02099e70;
extern u32  data_020a4b60;
extern u16  data_020a4b54;
extern u8   data_020a4b48;
extern void *data_020a4b64;
extern fBaseActorInfo **data_020a4bb8;

struct LifecycleSequence;
extern void func_02044334(LifecycleSequence *p);
extern int  func_0204424c(char *p);

/* Heap's methods and fields come from include/Heap.h. The heap locals stay
   `void *` (and are cast at each use): Virtual34 and Virtual38 read its
   heapStart and heapSize through those casts. include/Heap.h itself
   declares InitializeSolidHeapAsDefault as returning `void *`.
   Memory::Allocate and Memory::Deallocate come from include/Memory.h;
   data_020a0eac, the actor heap they are handed, comes from
   include/fBase_c.h, which needs it for its inline operator delete. */

/* aliveState is 0 before the actor joins the lists, 1 while it is alive
 * in them and 2 once it is dying; data_02099f24 is the frame loop's phase word.
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
static inline int GlobalModeIs(int value) {
    return data_02099f24 == value;
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
}

int func_0204405c(void *list, void *nodep) {
    Absorb0204405c::L *l = (Absorb0204405c::L *)list;
    Absorb0204405c::N *n = (Absorb0204405c::N *)nodep;
    Absorb0204405c::N *node;
    Absorb0204405c::N *next;
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

/* Process lists hold real actor-owned nodes. Each descriptor's callback is
 * an fBase_c member (the four Process wrappers below 0x020433b8); all four
 * return int, which this traversal discards. Save the next node before
 * invoking a callback that may unlink its owner. */
namespace Absorb02043fdc {
typedef int (fBase_c::*ProcessCallback)();

/* Local descriptor name is inferred from the list primitives and frame loop. */
struct ProcessListDescriptor {
    fLiNdBaPr_c *head;       // 0x0
    fLiNdBaPr_c *tail;       // 0x4
    ProcessCallback callback; // 0x8
};

void *func_02043fdc(ProcessListDescriptor *list) {
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

fLiNdBa_c *func_02043f98(fLiNdBa_c **head, unsigned int key) {
    fLiNdBa_c *node = *head;
    while (node != 0) {
        if (!OwnerUniqueIDDiffers(node, key))
            return node;
        node = node->next;
    }
    return 0;
}

fLiNdBa_c *func_02043f4c(fLiNdBa_c **head, unsigned int key, fLiNdBa_c *after) {
    fLiNdBa_c *node = after ? after->next : *head;
    while (node != 0) {
        if (!OwnerActorIDDiffers(node, key))
            return node;
        node = node->next;
    }
    return 0;
}

fBase_c::fBase_c() : manager(this)
{
    uniqueID = data_02099e70;
    data_02099e70++;
    param1 = data_020a4b60;
    actorID = data_020a4b54;
    mGroupType = data_020a4b48;

    Absorb0203b438::func_0203b438((Absorb0203b438::N *)data_020a4b6c,
                                  (Absorb0203b438::N *)&manager.sceneNode,
                                  (Absorb0203b438::N *)data_020a4b64);

    fBaseActorInfo *info = data_020a4bb8[actorID];
    manager.SetBehaviorPriority(info->behaviorPriority);
    manager.SetRenderPriority(info->renderPriority);

    fBase_c *parent = getConnectParent();
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
fBase_c::~fBase_c()
{
}

/* Run the guard, then the action when allowed, and always notify the after hook. */
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
int fBase_c::InitResources()
{
    return 1;
}

/* vtable slot 1. Allow initialization by default. */
bool fBase_c::BeforeInitResources()
{
    return 1;
}

/* vtable slot 2. Only acts on VS_SUCCESS (2): unlink from the pending list,
 * then either flag the actor as deferred (when the global at data_02099f24
 * reads 3) or link it into both the behaviour and render lists and mark it
 * alive. */
void fBase_c::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    func_0203b27c(data_020a4b88, &manager.behaviorNode);
    if (GlobalModeIs(3)) {
        mDeferExecute = 1;
        return;
    }
    func_0204405c(data_020a4b78, &manager.behaviorNode);
    func_0204405c(data_020a4b98, &manager.renderNode);
    aliveState = 1;
}

/* vtable slot 3. Default cleanup succeeds without releasing resources. */
s32 fBase_c::CleanupResources()
{
    return 1;
}

/* vtable slot 4. Wait for pending lifecycle work and remaining scene children. */
int fBase_c::BeforeCleanupResources()
{
    if ((lifecycleState != 0 && func_0204424c((char *)lifecycleState) == 0) ||
        manager.sceneNode.child != 0)
        return 0;
    return 1;
}

/* On success, unlink the actor, release its heap and pending lifecycle work,
 * then call the virtual complete-object destructor (slot 16) and deallocate.
 * This function is enrolled with the rest of the production TU. */
void fBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    Absorb0203b3c0::func_0203b3c0(data_020a4b6c, (Absorb0203b3c0::Node *)&manager.sceneNode);
    func_0203b27c(data_020a4ba8, &manager.behaviorNode);
    if (heap)
        ((Heap *)heap)->_Destroy();
    if (lifecycleState)
        func_02044334((LifecycleSequence *)lifecycleState);
    this->~fBase_c();   /* vtable+0x40 = slot 16 = D1 */
    Memory::Deallocate(this, data_020a0eac);
}

/* vtable slot 6. The default update succeeds without doing any work. */
s32 fBase_c::Behavior()
{
    return 1;
}

/* vtable slot 7. Skips the tick once the actor is marked for death, or when
 * bit 1 of the spawn-flag byte is set. */
int fBase_c::BeforeBehavior()
{
    if (shouldBeKilled != 0 || (pauseFlags & 2) != 0)
        return 0;
    return 1;
}

/* vtable slot 8. Base does nothing; leaf classes override. */
void fBase_c::AfterBehavior(u32 vfSuccess)
{
}

/* vtable slot 9. The default render succeeds without drawing anything. */
s32 fBase_c::Render()
{
    return 1;
}

/* vtable slot 10. The render twin of BeforeBehavior, on bit 3 instead of 1. */
int fBase_c::BeforeRender()
{
    if (shouldBeKilled != 0 || (pauseFlags & 8) != 0)
        return 0;
    return 1;
}

/* vtable slot 11. Base does nothing; leaf classes override. */
void fBase_c::AfterRender(u32 vfSuccess)
{
}

/* vtable slot 12 (vtable+0x30), fired by MarkForDestruction. Base does
 * nothing; leaf classes override to release or notify. */
void fBase_c::OnPendingDestroy()
{
}

/* Reconcile list membership, inherited pause flags and pending destruction.
 * Scene children link through SceneNode::next, and each node names its
 * actor owner. The process tree calls this through the ProcessFunction
 * constant data_02099f60, with every process in r0. NSMBW's connectProc
 * has the same three arms: a delete request, the parent's pause flags,
 * then a changed priority or a deferred create/execute. */
int fBase_c::connectProc() {
    if (shouldBeKilled != 0) {
        fBase_c::SceneNode *it;
        shouldBeKilled = 0;
        if (IsAliveState(aliveState)) {
            func_0203b27c(data_020a4b78, &manager.behaviorNode);
            func_0203b27c(data_020a4b98, &manager.renderNode);
        } else {
            func_0203b27c(data_020a4b88, &manager.behaviorNode);
        }
        func_0203b20c(data_020a4ba8, &manager.behaviorNode);
        aliveState = 2;
        for (it = manager.sceneNode.child; it != 0; it = it->next) {
            it->owner->MarkForDestruction();
        }
    } else {
        u32 state;
        fBase_c *other = getConnectParent();
        if (other != 0) {
            if ((other->pauseFlags & 1) || (other->pauseFlags & 2))
                pauseFlags |= 2;
            else if (pauseFlags & 2)
                pauseFlags &= ~2;
            if ((other->pauseFlags & 4) || (other->pauseFlags & 8))
                pauseFlags |= 8;
            else if (pauseFlags & 8)
                pauseFlags &= ~8;
        }
        state = aliveState;
        if (IsAliveState(state)) {
            {
                fLiNdBaPr_c *n = (&manager.behaviorNode);
                if (PriorityChanged(n)) {
                    fLiNdBaPr_c *m;
                    func_0203b27c(data_020a4b78, &manager.behaviorNode);
                    m = (&manager.behaviorNode);
                    m->currentPriority = m->nextPriority;
                    func_0204405c(data_020a4b78, m);
                }
            }
            {
                fLiNdBaPr_c *n = (&manager.renderNode);
                if (PriorityChanged(n)) {
                    fLiNdBaPr_c *m;
                    func_0203b27c(data_020a4b98, &manager.renderNode);
                    m = (&manager.renderNode);
                    m->currentPriority = m->nextPriority;
                    func_0204405c(data_020a4b98, m);
                }
            }
        } else if (!IsDyingState(state)) {
            if (mDeferRetryCreate != 0) {
                mDeferRetryCreate = 0;
                func_0203b244(data_020a4b88, &manager.behaviorNode);
            } else if (mDeferExecute != 0) {
                mDeferExecute = 0;
                func_0204405c(data_020a4b78, &manager.behaviorNode);
                func_0204405c(data_020a4b98, &manager.renderNode);
                aliveState = 1;
            }
        }
    }
    return 1;
}

/* Non-virtual. Idempotent, and a no-op once the actor is already dying;
 * otherwise sets the flag and fires slot 12. */
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
fBase_c *fBase_c::getConnectParent() {
    fBase_c::SceneNode *parent = manager.sceneNode.parent;
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
 * OnHeapCreated -- so this now reads as the virtual it is. The heap's
 * heapStart and heapSize are read as in the Heap note above. */
int fBase_c::Virtual34(u32 a, u32 b)
{
    void *h = 0;
    u32 avail;

    if (heap != 0)
        return 1;

    if (a != 0) {
        h = Heap::InitializeSolidHeapAsDefault(a, (Heap *)b, 0x20);
        if (h != 0) {
            u32 flagA = (u32)((Heap *)h)->heapStart & 0x10;
            if (flagA != 0)
                Memory::Allocate(0x10, 4, 0);
            int res = OnHeapCreated();
            u32 okA;
            if (flagA != 0) {
                okA = (u32)res;
            } else {
                void *allocRes = Memory::Allocate(0x10, 4, 0);
                okA = (allocRes != 0) ? (u32)res : 0;
            }
            Heap::RestoreFromTemporary();
            if (okA == 0) {
                ((Heap *)h)->_Destroy();
                h = 0;
            } else {
                u32 topA = ((Heap *)h)->heapSize;
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
        u32 flagB = (u32)((Heap *)h)->heapStart & 0x10;
        if (flagB != 0)
            Memory::Allocate(0x10, 4, 0);
        a = (u32)OnHeapCreated();
        if (flagB == 0) {
            void *allocRes2 = Memory::Allocate(0x10, 4, 0);
            if (allocRes2 == 0)
                a = 0;
        }
        Heap::RestoreFromTemporary();
        if (a == 0) {
            ((Heap *)h)->_Destroy();
            MarkForDestruction();
            return 0;
        }
        u32 topB = ((Heap *)h)->heapSize;
        avail = topB - ((Heap *)h)->MaxAllocationUnitSize();
        avail = (avail + 0x1f) & ~0x1f;
    }

    if (h != 0) {
        u32 topH = ((Heap *)h)->heapSize;
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
int fBase_c::Virtual38(u32 a, u32 b)
{
    if (heap != 0)
        return 1;
    if (a != 0) {
        void *h = Heap::InitializeSolidHeapAsDefault(a, (Heap *)b, 0x20);
        if (h != 0) {
            int ok;
            int flag = (int)((Heap *)h)->heapStart & 0x10;
            if (flag != 0)
                Memory::Allocate(0x10, 4, 0);
            ok = OnHeapCreated();
            if (flag == 0) {
                if (Memory::Allocate(0x10, 4, 0) == 0)
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
bool fBase_c::OnHeapCreated()
{
    return 1;
}

/* Actor-heap allocation, cleared before construction. Every `new` of a
 * process class lands here. */
void *fBase_c::operator new(size_t size) {
    void *p = Memory::Allocate(size, -4, data_020a0eac);
    if (!p)
        return 0;
    memset(p, 0, size);
    return p;
}

/* ROM ordinal 0 -- the run's backward neighbour, absorbed (reverse
 * emission puts the lowest address last). Body as recovered. */

/* fBase_make calls this on each new process. Same test order as NSMBW's
 * runCreate: not delete-requested, not deferred, still creating; then flag
 * a retry while the create loop runs (data_02099f24 == 2), or queue it on
 * the create list. */
void fBase_c::runCreate() {
    createPack();
    if (shouldBeKilled != 0)
        return;
    if (mDeferExecute != 0)
        return;
    if (!IsPendingState(aliveState))
        return;
    if (GlobalModeIs(2)) {
        mDeferRetryCreate = 1;
        return;
    }
    func_0203b244(data_020a4b88, &manager.behaviorNode);
}

/* The four Process wrappers below 0x020433b8. Each runs Process with one
 * triple of fBase_c member-function pointers from .data; the triples are
 * ROM-owned constants and stay extern. The wrappers are fBase_c members:
 * the process lists call them through the ProcessFunction constants
 * data_02099f70, f50, f48 and f68. Each triple's vtable offsets pick one
 * do/before/after group (0/4/8, 0xc/0x10/0x14, 0x18/0x1c/0x20,
 * 0x24/0x28/0x2c), so the wrappers are NSMBW's createPack, deletePack,
 * executePack and drawPack. */

extern fBase_c::ProcessFunction data_02099ebc;
extern fBase_c::BeforeProcessFunction data_02099ec4;
extern fBase_c::AfterProcessFunction data_02099e94;
extern fBase_c::ProcessFunction data_02099e8c;
extern fBase_c::BeforeProcessFunction data_02099e7c;
extern fBase_c::AfterProcessFunction data_02099e84;
extern fBase_c::ProcessFunction data_02099ea4;
extern fBase_c::BeforeProcessFunction data_02099eac;
extern fBase_c::AfterProcessFunction data_02099eb4;
extern fBase_c::ProcessFunction data_02099ecc;
extern fBase_c::BeforeProcessFunction data_02099e74;
extern fBase_c::AfterProcessFunction data_02099e9c;
/* Defined below; the calls are emitted first. */
extern void func_0204302c(int actorID);
extern int func_02043060(int actorID);
extern "C" {

/* The create hooks a scene installs, and the create entry's progress:
   data_020a4b4c steps 2..5 through fBase_make and data_020a4b50 holds
   the profile being created (0xffff when idle). */
extern void (*data_020a4b5c)(int actorID);
extern int (*data_020a4b58)(int actorID);
}

extern u16 data_020a4b50;
extern u8 data_020a4b4c;

int fBase_c::createPack() {
    return Process(data_02099ebc, data_02099ec4, data_02099e94);
}

int fBase_c::deletePack() {
    /* The actor ID is read before Process runs and handed to the
     * func_0204302c hook on a result of 1. */
    unsigned short id = actorID;
    int r = Process(data_02099e8c, data_02099e7c, data_02099e84);
    if (r == 1)
        func_0204302c(id);
    return r;
}

int fBase_c::executePack() {
    return Process(data_02099ea4, data_02099eac, data_02099eb4);
}

int fBase_c::drawPack() {
    return Process(data_02099ecc, data_02099e74, data_02099e9c);
}

/* Scene-tree walk: func_0203b394 steps to the first child or, failing
   that, to func_0203b358's next node outside the subtree, which is also
   where the walk ends. */
extern fBase_c::SceneNode *func_0203b358(fBase_c::SceneNode *node);
extern fBase_c::SceneNode *func_0203b394(fBase_c::SceneNode *node);

/* Is any descendant still uncreated (aliveState 0)? d_scene.cpp waits on
   this before it moves on. */
int fBase_c::checkChildProcessCreateState() {
    fBase_c::SceneNode *base = &manager.sceneNode;
    fBase_c::SceneNode *end = func_0203b358(base);
    fBase_c::SceneNode *node = base->child;
    int uncreated;
    while (node != 0 && node != end) {
        /* Materialized as 0/1 before the test, as the ROM does. */
        uncreated = !node->owner->aliveState;
        if (uncreated != 0)
            return 1;
        node = func_0203b394(node);
    }
    return 0;
}

/* Stage the arguments fBase_c::fBase_c reads while the factory runs, in
   NSMBW's setTmpCtData store order (param, profile, group type, parent). */
void fBase_c::setTmpCtData(int actorID, void *parentNode, u32 param1, int groupType) {
    data_020a4b60 = param1;
    data_020a4b54 = actorID;
    data_020a4b48 = groupType;
    data_020a4b64 = parentNode;
}

/* The create entry: ask the scene's hook, stage the constructor's
   arguments, run the profile's factory, then link the new process. NSMBW's
   fBase_make has the same core (setTmpCtData, classInit, runCreate on a
   built process); the scene hook and the data_020a4b4c progress steps are
   this game's own. */
fBase_c *fBase_c::fBase_make(int actorID, void *parentNode, u32 param1, int groupType) {
    fBase_c *actor;
    data_020a4b50 = (u16)actorID;
    data_020a4b4c = 2;
    if (func_02043060(actorID) == 3)
        return 0;
    data_020a4b4c = 3;
    setTmpCtData(actorID, parentNode, param1, groupType);
    data_020a4b4c = 4;
    actor = data_020a4bb8[actorID]->classInit();
    if (actor == 0) {
        data_020a4b4c = 0;
        data_020a4b50 = 0xffff;
        return 0;
    }
    data_020a4b4c = 5;
    actor->runCreate();
    data_020a4b4c = 0;
    data_020a4b50 = 0xffff;
    return actor;
}

int func_02043060(int actorID) {
    if (data_020a4b58 == 0)
        return 2;
    return data_020a4b58(actorID);
}

void func_0204302c(int actorID) {
    if (data_020a4b5c)
        data_020a4b5c(actorID);
}

/* Create a child of parent; none without a parent. */
fBase_c *fBase_c::createChild(u32 actorID, fBase_c *parent, u32 param1, int groupType) {
    if (parent == 0)
        return 0;
    return fBase_make(actorID, &parent->manager.sceneNode, param1, groupType);
}

/* Create a process at the root of the scene tree. */
int fBase_c::createRoot(int actorID, u32 param1, int groupType) {
    return (int)fBase_make(actorID, 0, param1, groupType);
}
