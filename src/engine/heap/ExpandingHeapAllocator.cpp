//cpp
/* arm9/ExpandingHeapAllocator -- the free-list allocator behind ExpandingHeap,
 * folded from nineteen legacy shards at .text 0x0204e084..0x0204e964. The span
 * also carries three adjacent heap-family functions: HeapAllocator::Destroy,
 * Heap::CreateExpandingHeapAllocator and MemoryNode::Target's constructor.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending; the roster reads ROM-ascending.
 *
 *   SizeofInternal..Allocate -- the public surface and the free-list walk.
 *   Destroy/CreateExpandingHeapAllocator -- the owning heap's entry points.
 *   FreeNode..UnlinkNode -- the node-list layer, all static.
 *   C1/CreateNode -- construction.
 *   MemoryNode::Target::Target -- the extent a node describes.
 *
 * The allocator's node bookkeeping is viewed through the overlay structs below
 * rather than the header's mFirstNode/mLastNode/mNodeID/mFlags2: the same words
 * play two roles (list sentinel fields and the node-list flag word), and the
 * recovered forms that reach them through this+0x24 are the ones the ROM
 * compiled. Where a named member measured byte-neutral the source uses it; the
 * leftover section lists what stayed raw and why.
 *
 * deslop leftovers:
 * - SetNodeID keeps its documented two-step addressing: `Inner* p = this+0x24`
 *   then `p->id` is load-bearing -- flattening to this+0x34 diffs by three
 *   words (the shard's own measurement; re-measured: `mNodeID` read and
 *   written directly still diffs by three).
 * - The constructor writes mNodeID/mFlags2 and the two list pairs through
 *   its `nodeList` NodeList view of this+0x24: spelling those stores as
 *   members of `this` re-bases them and diffs (re-measured). It passes the
 *   heap's first byte as `nodeList + 1` (this+0x38); `this + 1` also diffs.
 *   Deallocate likewise reaches the in-use list through its NodeList view;
 *   `&mUsedFirstNode` diffs.
 * - The AllocateForwards/AllocateBackwards NodeList overlay stays: it spells
 *   the header's mFirstNode..mFlags2 from this+0x24, but both searches
 *   also pass `c` onward to AllocateNode as the list sentinel -- one object
 *   serving both views. Splitting the access changed the emitted code.
 * - AllocateNode reads the allocator's mFlags through `c[-1]` -- the flag word
 *   sits at this+0x20, one word before the node list at this+0x24, and `c` is
 *   the sentinel parameter, not `this`.
 * - CreateNode indexes its Target* through `int* c`: the body's `c[0]`/`c[1]`
 *   arithmetic is verbatim -- retyping as extent->start/end is the kind of
 *   tidy that already produced a three-word divergence on this class.
 * - Reallocate's two Targets are block-scope locals, each declared in the
 *   branch that uses it, and the zero fill goes through the FillZero inline.
 *   mwccarm gives function-scope locals their frame slots first and an
 *   inlined helper's locals last, so the volatile zero must live in the
 *   helper to land below both Targets.
 * - The file-local `GetList` inline stays for the same reason it existed in
 *   the shard: spelling `this+0x24` as an accessor call is what mwccarm's
 *   register allocator keys on for Reallocate's r7/r8/sb assignment.
 * - Heap::CreateExpandingHeapAllocator calls the C1 by its mangled spelling:
 *   the factory runs the allocator constructor on an already-aligned address,
 *   an ABI seam a native `new` cannot spell.
 */

#include "ExpandingHeapAllocator.h"
#include "Heap.h"
#include "decl_HeapAllocator.h"
#include "cstd.h"

extern "C" void MultiStore_Int(int val, void *dst, int len);
/* local extern: the factory runs this constructor over storage that already
   exists (see the leftovers above); a placement new would add a null test. */
extern "C" ExpandingHeapAllocator *_ZN22ExpandingHeapAllocatorC1EPvj(
    ExpandingHeapAllocator *self, void *heapEnd, u32 flags);

/* The free list sentinel: a view of the allocator's node bookkeeping from
 * this+0x24, with the header's member names. mFlags2 bit 0 is the
 * search-policy bit the two fit searches read. */
struct NodeList {
    MemoryNode *mFirstNode;     /* this+0x24 */
    MemoryNode *mLastNode;      /* this+0x28 */
    MemoryNode *mUsedFirstNode; /* this+0x2c */
    MemoryNode *mUsedLastNode;  /* this+0x30 */
    unsigned short mNodeID;     /* this+0x34 */
    unsigned short mFlags2;     /* this+0x36 */
};

/* UnlinkNode's recovered node view -- field order only, names are offsets. */
struct N { int p0, p1, prev, next; };

/* SetNodeID's receiver view -- see the leftover note above. */
struct Inner { char pad[0x10]; unsigned short id; };

/* The free-list head, viewed as a sentinel object at this+0x24. The shard
 * spelled it as an inline accessor, and the spelling is load-bearing for
 * Reallocate's register allocation. */
static inline MemoryNode **GetList(void *t) {
    return &((ExpandingHeapAllocator *)t)->mFirstNode;
}

// @symbol _ZN10MemoryNode6TargetC1EPS_
MemoryNode::Target::Target(MemoryNode *node)
    : start((char *)node - (u16)((node->flags >> 8) & 0x7f)),
      end((char *)(node->size + (u32)(node + 1))) {}

// @symbol _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_
/* Doubly-linked-list removal writing head/tail (c[0], c[1]) directly when the
 * node was at either end. Returns the former predecessor, which FreeNode needs
 * to decide whether the freed block merges backwards. `list` mangles
 * MemoryNode* but callers pass the embedded sentinel. */
void *ExpandingHeapAllocator::UnlinkNode(MemoryNode *list, MemoryNode *node_)
{
    int *c = (int *)list;
    N *node = (N *)node_;
    N *r2 = (N *)node->prev;
    N *r1 = (N *)node->next;
    if (r2) r2->next = (int)r1;
    else c[0] = (int)r1;
    if (r1) r1->prev = (int)r2;
    else c[1] = (int)r2;
    return r2;
}

// @symbol _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_
/* Insert `node` immediately after `prev`, or at the head when `prev` is null,
 * fixing both neighbours and the list's head/tail. The inverse of UnlinkNode,
 * and the reason UnlinkNode returns the predecessor. */
void *ExpandingHeapAllocator::LinkNode(MemoryNode *list, MemoryNode *node, MemoryNode *prev) {
    MemoryNode **c = (MemoryNode **)list;
    MemoryNode *next;
    node->prev = prev;
    if (prev) {
        next = prev->next;
        prev->next = node;
    } else {
        next = c[0];
        c[0] = node;
    }
    node->next = next;
    if (next) {
        next->prev = node;
    } else {
        c[1] = node;
    }
    return node;
}

// @symbol _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt
/* Stamp a fresh header at the start of an extent and return it. The recorded
 * size is `end - (node + 0x10)` -- user bytes from just past the header, the
 * definition Target's constructor inverts. The flags word is zeroed here; the
 * alignment padding its bits 8..14 carry is written by whoever placed the
 * node. */
void *ExpandingHeapAllocator::CreateNode(MemoryNode::Target *extent, u16 t)
{
    int *c = (int *)extent;
    MemoryNode *n = (MemoryNode *)c[0];
    n->tag = t;
    n->flags = 0;
    n->size = ((int)c[1]) - (int)(n + 1);
    n->prev = 0;
    n->next = 0;
    return n;
}

// @symbol _ZN22ExpandingHeapAllocatorC1EPvj
ExpandingHeapAllocator::ExpandingHeapAllocator(void *heapEnd, u32 flags)
{
    NodeList *nodeList = (NodeList *)&mFirstNode;
    MemoryNode::Target extent;
    MemoryNode *node;

    func_0204df54(this, 0x45585048, nodeList + 1, heapEnd, flags);
    nodeList->mNodeID = 0;
    nodeList->mFlags2 = 0;
    nodeList->mFlags2 &= ~1;

    extent.start = (char *)mStart;
    extent.end = (char *)mEnd;
    node = (MemoryNode *)CreateNode(&extent, 0x4652);
    nodeList->mFirstNode = node;
    nodeList->mLastNode = node;
    nodeList->mUsedFirstNode = 0;
    nodeList->mUsedLastNode = 0;
}

// @symbol _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjt
/* Carve one allocation out of a free block: unlink it, split into up to three
 * pieces -- leading gap, the allocation, trailing gap -- and stamp a fresh
 * header on each survivor. A gap smaller than a header (0x10) collapses into
 * the allocation. 'FR' (0x4652) tags the free remnants, 'UD' (0x5544) the
 * allocated one. The last parameter is u16 -- `Pvjt`, not the imported `Pvjj`:
 * the ROM reads the stack-passed argument with ldrh, the width mwccarm picks
 * from the declared type. */
void *ExpandingHeapAllocator::AllocateNode(MemoryNode *c, MemoryNode *node,
                                           void *target, u32 size, u16 z)
{
    MemoryNode::Target t0(node);
    MemoryNode::Target t1;
    MemoryNode::Target t2;
    void *link;
    int header;

    int oldLimit = (int)t0.end;
    header = (int)target - 0x10;
    int backStart = (int)size + (int)target;
    t0.end = (char *)header;
    t1.end = (char *)oldLimit;
    t1.start = (char *)backStart;

    link = UnlinkNode(c, node);

    unsigned int frontGap = (unsigned int)(t0.end - t0.start);
    if (frontGap < 0x10) {
        t0.end = t0.start;
    } else {
        void *newFront = CreateNode(&t0, 0x4652);
        link = LinkNode(c, (MemoryNode *)newFront, (MemoryNode *)link);
    }

    unsigned int backGap = (unsigned int)(t1.end - t1.start);
    if (backGap < 0x10) {
        t1.start = t1.end;
    } else {
        void *newBack = CreateNode(&t1, 0x4652);
        link = LinkNode(c, (MemoryNode *)newBack, (MemoryNode *)link);
    }

    int flagsWord = *(int *)((char *)c - 4);
    int *dst = (int *)t0.end;
    int len = t1.start - t0.end;
    if (((unsigned short)(flagsWord & 0xff)) & 1) {
        volatile int zero = 0;
        MultiStore_Int(zero, dst, len);
    }

    t2.start = (char *)header;
    t2.end = t1.start;
    void *allocNode = CreateNode(&t2, 0x5544);

    unsigned short *flagsPtr = &((MemoryNode *)allocNode)->flags;
    void *usedList = &((NodeList *)c)->mUsedFirstNode;
    *flagsPtr &= ~0x8000;
    *flagsPtr |= (z & 1) << 15;
    int t0e = (int)t0.end;
    *flagsPtr &= ~0x7f00;
    *flagsPtr |= (((unsigned short)((int)allocNode - t0e)) & 0x7f) << 8;
    unsigned int cRaw = ((NodeList *)c)->mNodeID;
    unsigned int cValue = cRaw & 0xff;
    *flagsPtr &= ~0xff;
    *flagsPtr |= cValue;

    void *usedTail = ((NodeList *)c)->mUsedLastNode;
    LinkNode((MemoryNode *)usedList, (MemoryNode *)allocNode, (MemoryNode *)usedTail);

    return target;
}

// @symbol _ZN22ExpandingHeapAllocator16AllocateForwardsEjj
/* Walk the free list forward from the head, aligning each node's data start
 * upward. Bit 0 of the list's flag word selects the search: clear is first
 * fit, set is best fit; the `nsize == size` break is an exact-fit shortcut for
 * both. */
void *ExpandingHeapAllocator::AllocateForwards(u32 size, u32 align)
{
    NodeList *c = (NodeList *)&mFirstNode;
    unsigned short flag = c->mFlags2;
    int firstFit = ((unsigned short)(flag & 1) == 0);
    MemoryNode *best = 0;
    MemoryNode *node = c->mFirstNode;
    unsigned int bestSize = 0xFFFFFFFF;
    void *bestTarget = 0;
    if (node != 0) {
        unsigned int mask = align - 1;
        do {
            char *data = (char *)(node + 1);
            char *aligned = (char *)((mask + (unsigned int)data) & ~mask);
            unsigned int pad = (unsigned int)(aligned - data);
            unsigned int nsize = node->size;
            if (nsize >= size + pad && bestSize > nsize) {
                best = node;
                bestSize = nsize;
                bestTarget = aligned;
                if (firstFit) break;
                if (nsize == size) break;
            }
            node = node->next;
        } while (node != 0);
    }
    if (best == 0) return 0;
    return AllocateNode((MemoryNode *)c, best, bestTarget, size, 0);
}

// @symbol _ZN22ExpandingHeapAllocator17AllocateBackwardsEjj
/* The mirror of AllocateForwards: walk from the tail and align downward so the
 * padding lands below the block rather than above it. The fit test is
 * `aligned - data >= 0` -- did it stay inside the node. */
void *ExpandingHeapAllocator::AllocateBackwards(u32 size, u32 align)
{
    NodeList *c = (NodeList *)&mFirstNode;
    unsigned short flag = c->mFlags2;
    int firstFit = ((unsigned short)(flag & 1) == 0);
    MemoryNode *best = 0;
    MemoryNode *node = c->mLastNode;
    unsigned int bestSize = 0xFFFFFFFF;
    void *bestTarget = 0;
    if (node != 0) {
        unsigned int mask = align - 1;
        do {
            char *data = (char *)(node + 1);
            unsigned int nsize = node->size;
            unsigned int t = nsize + (unsigned int)data - size;
            char *aligned = (char *)(~mask & t);
            if ((int)(aligned - data) >= 0 && bestSize > nsize) {
                best = node;
                bestSize = nsize;
                bestTarget = aligned;
                if (firstFit) break;
                if (nsize == size) break;
            }
            node = node->prev;
        } while (node != 0);
    }
    if (best == 0) return 0;
    return AllocateNode((MemoryNode *)c, best, bestTarget, size, 1);
}

// @symbol _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE
/* Merge a released extent into the address-ordered free list: absorb a free
 * node directly above it, coalesce with the free node directly below, then
 * link the merged extent back where the lower neighbour sat. The first
 * parameter is the embedded sentinel -- both callers pass this+0x24. */
int ExpandingHeapAllocator::FreeNode(MemoryNode *list, MemoryNode::Target *extent)
{
    struct TargetWords { u32 words[2]; };
    MemoryNode::Target merged;

    *(TargetWords *)&merged = *(TargetWords *)extent;

    MemoryNode *below = 0;
    MemoryNode *node = *(MemoryNode **)list;

    while (node != 0) {
        if ((u32)node < (u32)extent->start) {
            below = node;
        } else {
            if ((u32)node == (u32)extent->end) {
                merged.end = (char *)(node->size + ((u32)node + 0x10));
                UnlinkNode(list, node);
            }
            break;
        }
        node = node->next;
    }

    if (below != 0) {
        u32 belowEnd = below->size + ((u32)below + 0x10);
        if ((char *)belowEnd == extent->start) {
            merged.start = (char *)below;
            below = (MemoryNode *)UnlinkNode(list, below);
        }
    }

    if ((u32)(merged.end - merged.start) < 0x10)
        return 0;

    MemoryNode *node_ = (MemoryNode *)CreateNode(&merged, 0x4652);
    LinkNode(list, node_, below);
    return 1;
}

// @symbol _ZN5mHeap6Heap_t28CreateExpandingHeapAllocatorEPvjj
/* The expanding twin of Heap::CreateSolidHeapAllocator, identical bar the
 * minimum: 0x4c against the solid allocator's 0x30, because an expanding heap
 * keeps a free-node list. */
namespace mHeap {

ExpandingHeapAllocator *Heap_t::CreateExpandingHeapAllocator(void *address, u32 size, u32 flags)
{
    u32 end   = (size + (u32)address) & ~3u;
    u32 start = ((u32)address + 3u)   & ~3u;

    if (start > end || (end - start) < 0x4c)
        return 0;

    return _ZN22ExpandingHeapAllocatorC1EPvj((ExpandingHeapAllocator *)start, (void *)end, flags);
}

} /* namespace mHeap */

// @symbol _ZN13HeapAllocator7DestroyEv
/* Detach this allocator from its containing heap -- a 12-byte tail veneer to
 * Remove(). */
void HeapAllocator::Destroy()
{
    Remove();
}

// @symbol _ZN22ExpandingHeapAllocator8AllocateEji
/* The public entry point. A zero-byte request becomes one byte and the size
 * rounds up to a multiple of 4. THE SIGN OF `align` IS THE DIRECTION, not part
 * of the alignment: non-negative allocates from the low end, negative from the
 * high end, the magnitude used either way -- hence `-align` on the backwards
 * call, and hence `int` where a u32 would delete the branch. */
void *ExpandingHeapAllocator::Allocate(u32 size, int align)
{
    if (size == 0) size = 1;
    size = (size + 3) & ~3u;
    if (align >= 0) {
        return this->AllocateForwards(size, (u32)align);
    }
    return this->AllocateBackwards(size, (u32)-align);
}

/* Zero len bytes at p through a volatile zero, so the store loop takes its
 * value from a stack slot. */
static inline void FillZero(void *p, u32 len) {
    volatile int zero;
    zero = 0;
    MultiStore_Int(zero, p, len);
}

// @symbol _ZN22ExpandingHeapAllocator10ReallocateEPvj
/* Grow a block in place by swallowing the free node directly after it, or
 * shrink it and hand the tail back to the free list. The allocator's low flag
 * bit asks for freshly-gained bytes to be zero filled. */
u32 ExpandingHeapAllocator::Reallocate(void *ptrRaw, u32 size)
{
    MemoryNode *block;
    u32 blockSize;
    MemoryNode *node;
    MemoryNode **list;
    char *ptr = (char *)ptrRaw;

    node = (MemoryNode *)ptr;
    node = (MemoryNode *)((char *)node - 0x10);
    list = GetList(this);
    blockSize = node->size;
    size = (size + 3) & ~3u;

    if (size == blockSize) {
        return size;
    }

    if (size > blockSize) {
        char *end = (char *)(blockSize + ((u32)node + 0x10));
        char *oldStart;
        MemoryNode *prev;

        block = *list;
        while (block != 0) {
            if (block == (MemoryNode *)end) break;
            block = block->next;
        }
        if (block == 0 || size > blockSize + 0x10 + block->size) {
            return 0;
        }

        MemoryNode::Target tgt(block);
        prev = (MemoryNode *)UnlinkNode((MemoryNode *)list, block);
        oldStart = tgt.start;
        tgt.start = (char *)(size + (u32)ptr);
        if ((u32)(tgt.end - tgt.start) < 0x10) {
            tgt.start = tgt.end;
        }
        node->size = tgt.start - ptr;
        if ((u32)(tgt.end - tgt.start) >= 0x10) {
            MemoryNode *newNode = (MemoryNode *)CreateNode(&tgt, 0x4652);
            LinkNode((MemoryNode *)list, newNode, prev);
        }
        {
            u16 opt = (u16)(mFlags & 0xff);
            u32 len = (u32)(tgt.start - oldStart);
            if (opt & 1) {
                FillZero(oldStart, len);
            }
        }
    } else {
        MemoryNode::Target tgt2;
        tgt2.start = (char *)(size + (u32)ptr);
        tgt2.end = (char *)(node->size + ((u32)node + 0x10));
        node->size = size;
        if (!FreeNode((MemoryNode *)list, &tgt2)) {
            node->size = blockSize;
        }
    }

    return node->size;
}

// @symbol _ZN22ExpandingHeapAllocator10DeallocateEPv
/* Free one block: build a Target describing the node, unlink it from the
 * in-use list, and hand it to FreeNode to merge back into the free list. The
 * MemoryNode header sits 0x10 before the user pointer; the in-use list head is
 * 8 past the embedded bookkeeping at this+0x24. */
int ExpandingHeapAllocator::Deallocate(void *ptr)
{
    NodeList *inner;
    MemoryNode *node;

    node = (MemoryNode *)((char *)ptr - 0x10);
    inner = (NodeList *)&mFirstNode;
    MemoryNode::Target target(node);
    UnlinkNode((MemoryNode *)&inner->mUsedFirstNode, node);
    return FreeNode((MemoryNode *)inner, &target);
}

// @symbol _ZN22ExpandingHeapAllocator10MemoryLeftEv
/* Total free bytes: walk the free-node list and sum each node's size. NOT the
 * largest allocatable block -- MaxAllocatableSize answers that, and the two
 * differ by fragmentation. */
u32 ExpandingHeapAllocator::MemoryLeft()
{
    MemoryNode *n = mFirstNode;
    unsigned int total = 0;
    while (n) {
        total += n->size;
        n = n->next;
    }
    return total;
}

// @symbol _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi
/* The largest single block still obtainable at this alignment: align each
 * free node's data start upward and keep the largest usable remainder, with
 * ties broken toward the node wasting least on alignment padding. `align` goes
 * through cstd::abs -- the sign is Allocate's direction convention, accepted
 * and ignored here. */
int ExpandingHeapAllocator::MaxAllocatableSize(int arg)
{
    int align = cstd::abs(arg);
    unsigned int bestSize = 0;
    unsigned int bestOffset = -1;
    MemoryNode *n = mFirstNode;
    while (n) {
        unsigned int base = (unsigned int)n + 0x10;
        unsigned int aligned = ((align - 1) + base) & ~(align - 1);
        unsigned int end = n->size + base;
        if (aligned < end) {
            unsigned int usable = end - aligned;
            unsigned int offset = aligned - base;
            if (bestSize < usable || (bestSize == usable && bestOffset > offset)) {
                bestSize = usable;
                bestOffset = offset;
            }
        }
        n = n->next;
    }
    return bestSize;
}

// @symbol _ZN22ExpandingHeapAllocator9SetNodeIDEj
/* Stamp the ID subsequently allocated nodes carry and return the previous one.
 * The field is the header's mNodeID (u16 at +0x34); the incoming u32 truncates
 * on the way in and the u16 widens to int on the way out. */
int ExpandingHeapAllocator::SetNodeID(u32 id)
{
    Inner *p = (Inner *)&mFirstNode;
    unsigned short old = p->id;
    p->id = (unsigned short)id;
    return old;
}

// @symbol _ZN22ExpandingHeapAllocator9GetNodeIDEv
u32 ExpandingHeapAllocator::GetNodeID()
{
    return this->mNodeID;
}

// @symbol _ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj
/* Walk the in-use list and hand each block's user pointer (node + 0x10) to the
 * callback, along with the allocator and the caller's opaque argument. `next`
 * is read BEFORE the call, because the callback frees the node it is given.
 * InvokeDeallocate is exactly this callback's shape: the trampoline that turns
 * each block back into an allocator->Deallocate(ptr) call. */
void *ExpandingHeapAllocator::DeallocateAll(DeallocationFunction fn, u32 ctx)
{
    MemoryNode *node = mUsedFirstNode;
    if (!node) return node;
    do {
        MemoryNode *next = node->next;
        fn((char *)(node + 1), this, ctx);
        node = next;
    } while (node);
    return node;
}

// @symbol _ZN22ExpandingHeapAllocator14SizeofInternalEPv
/* The allocated size of a block from a user pointer alone: every block carries
 * a MemoryNode header immediately before its user data, and `size` sits three
 * words (0xc) back. */
u32 ExpandingHeapAllocator::SizeofInternal(void *userPtr)
{
    return ((s32 *)userPtr)[-3];
}
