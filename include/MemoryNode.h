/* Hand-written, from usage evidence. Not generated.
 *
 * One block header in an ExpandingHeapAllocator heap: 0x10 bytes, immediately BEFORE
 * the user pointer it describes. That offset is why ExpandingHeapAllocator::Deallocate
 * reaches its header at ptr-0x10 and why SizeofInternal needs no allocator instance --
 * the header is self-describing.
 *
 * WHY THIS TYPE EXISTS AT ALL. Five ExpandingHeapAllocator methods take MemoryNode or
 * MemoryNode::Target parameters -- `P10MemoryNode`, `PNS0_6TargetE` -- and cannot be
 * declared as C++ members until those names resolve to a class with that exact nested
 * shape. Without it the mangling comes out wrong and the ROM symbol is never defined.
 * Reconstructing it is the whole prerequisite for finishing that class (#1221).
 *
 * SEVEN LOCAL COPIES EXISTED AND FIVE DISAGREED -- on names, on signedness, and on
 * whether offset 0 was a u16 or a 2-char array:
 *
 *   u16 tag; u16 flags; u32 size; MemoryNode *prev, *next;   AllocateForwards/Backwards
 *   char magic[2]; u16 flags; u32 size; ...                  Deallocate, Reallocate
 *   u16 tag; u16 pad; int size; int prev; int next;          FreeNode
 *   u16 pad0; u16 bits2; int f4; char rest[8];               Target's constructor
 *
 * All seven agree on the OFFSETS, which is what makes this reconcilable. The
 * disagreements are resolved from the two functions that write the fields rather than
 * by majority vote:
 *
 * OFFSET 0 IS A u16, NOT `char magic[2]`. ExpandingHeapAllocator::CreateNode takes a
 * `u16` parameter (`...6TargetEt`) and stores it with `*(u16 *)(n + 0) = t` -- a 16-bit
 * store of a 16-bit argument. A 2-char array was a guess that happens to be the same
 * width. Whether the value is two ASCII characters is a separate question this header
 * does not answer.
 *
 * OFFSET 2 IS A FLAGS WORD WHOSE BITS 8..14 ARE THE ALIGNMENT PADDING skipped ahead of
 * this header. MemoryNode::Target's constructor is the evidence:
 *     start = (char *)node - ((node->flags >> 8) & 0x7f)
 * so the region the block really occupies begins that many bytes before the header.
 * CreateNode zeroes the whole word, so an unaligned allocation is the only thing that
 * ever sets it.
 *
 * OFFSET 4 IS THE USER-DATA SIZE, measured from `this + 0x10`, not from the header:
 * CreateNode stores `end - (n + 0x10)`, and Target's constructor recovers the end as
 * `node->size + (char *)node + 0x10`. Declared u32 because every consumer that compares
 * it -- the fit searches in AllocateForwards and AllocateBackwards -- compares unsigned;
 * only CreateNode's own store treats it as a signed difference, and that store is
 * byte-identical either way. Verified, not assumed.
 *
 * TARGET IS A HALF-OPEN POINTER PAIR describing the block's true extent, padding
 * included. Its explicit constructor recovers that extent from a node; the empty
 * default constructor preserves the aggregate-like scratch locals that callers fill
 * manually. */
#ifndef MEMORYNODE_H
#define MEMORYNODE_H
#include "types.h"

struct MemoryNode {
    u16 tag;            /* 0x0 -- set once by CreateNode from its u16 argument */
    u16 flags;          /* 0x2 -- bits 8..14: alignment padding before this header */
    u32 size;           /* 0x4 -- user bytes, measured from (this + 0x10) */
    /* `struct MemoryNode *`, not `MemoryNode *`: C has no injected class name, and
       ExpandingHeapAllocator's constructor is still a .c file that reaches this header
       through ExpandingHeapAllocator.h. Spelled the C way, it compiles as both. */
    struct MemoryNode* prev;   /* 0x8 */
    struct MemoryNode* next;   /* 0xc */

    /* The extent of a block including any alignment padding: [start, end).
       Nested so that `MemoryNode::Target*` mangles as `PN10MemoryNode6TargetE`, which
       is what the ROM's ExpandingHeapAllocator symbols require. */
    struct Target {
        char* start;
        char* end;
#ifdef __cplusplus
        Target() {}
        Target(MemoryNode* node);
#endif
    };
};

typedef char MemoryNode_size_must_be_0x10[
    sizeof(struct MemoryNode) == 0x10 ? 1 : -1];
typedef char MemoryNode_Target_size_must_be_0x8[
    sizeof(struct MemoryNode::Target) == 0x8 ? 1 : -1];

#endif
