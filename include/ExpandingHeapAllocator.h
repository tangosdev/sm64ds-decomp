/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2.
 *
 * class ExpandingHeapAllocator: 18 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * THE __cplusplus BLOCK BELOW HAD NEVER BEEN COMPILED. It declared
 * `forwards void* Allocate(...)` -- `forwards` is not a keyword, and the generator
 * evidently spilled a fragment of a parameter name into the return type. Only two
 * files include this header and both were .c, so the block was always skipped and
 * the syntax error sat there unnoticed. Migrating the first method to C++ is what
 * compiled it for the first time. */
#ifndef EXPANDINGHEAPALLOCATOR_H
#define EXPANDINGHEAPALLOCATOR_H
#include "types.h"
#include "HeapAllocator.h"
#include "MemoryNode.h"   /* the five node methods below take it by pointer */

/* The generator also emitted `struct align_; struct ptr; struct size_;` as forward
 * declarations -- parameter names mistaken for type names. No source ever referenced
 * them; dropped, as in SolidHeapAllocator.h. */
struct ExpandingHeapAllocator : HeapAllocator {
    MemoryNode* mFirstNode; /* 0x024 */
    MemoryNode* mLastNode;  /* 0x028 */
    u32 unk_02c;            /* 0x02c */
    u32 unk_030;            /* 0x030 */
    u16 mNodeID;            /* 0x034 */
    u16 mFlags2;            /* 0x036 */
#ifdef __cplusplus
    ExpandingHeapAllocator(void* heapEnd, u32 flags);

    /* methods. Parameter types are read off the mangled name: `Eji` is
       (unsigned int, int), `EPv` is (void*), `Ev` is (). */
    void* Allocate(u32 size, int align);
    int   Deallocate(void* ptr);
    u32   GetNodeID();
    int   SetNodeID(u32 id);              /* returns the previous ID */
    u32   MemoryLeft();                   /* sum of every free node's size */
    int   MaxAllocatableSize(int align);  /* largest single block, at that alignment */
    u32   Reallocate(void* ptr, u32 size);/* the new SIZE, or 0 -- its own body
                                             returns `size', 0 or `node->size',
                                             never a pointer. ExpandingHeap's
                                             VReallocate had been declaring it
                                             pointer-returning in a local copy,
                                             which is where the tree's `void*'
                                             for Heap vtable slot 8 came from. */

    /* The two fit searches behind Allocate. `Ejj` is (u32, u32) -- two declared
       parameters against three body arguments, so the leading one is `this` and
       these are instance methods. Note SolidHeapAllocator's same-named pair mangles
       `EPvjj` and IS static: the arity test decides it per class, and the name is
       no guide at all. */
    void* AllocateForwards(u32 size, u32 align);
    void* AllocateBackwards(u32 size, u32 align);

    /* Static: the ROM body takes no `this`. SizeofInternal reads the block's
       MemoryNode header, which sits immediately before the user pointer, so it
       needs no allocator instance. InvokeDeallocate declares three parameters
       (`EPvPS_j`) and its body takes exactly three -- it is the trampoline
       DeallocateAll hands each block to, so the allocator arrives as an explicit
       argument rather than as `this`. */
    static u32  SizeofInternal(void* userPtr);
    static void InvokeDeallocate(void* ptr, ExpandingHeapAllocator* alloc, u32 size);

    /* What DeallocateAll hands each block to, and exactly InvokeDeallocate's shape --
       that trampoline is the one the game passes. The ROM's symbol declared this
       parameter as a pointer TO this pointer (`PPFvPvPS_jE`); it is not, see below. */
    typedef void (*DeallocationFunction)(void*, ExpandingHeapAllocator*, u32);
    void* DeallocateAll(DeallocationFunction fn, u32 arg);

    /* The node-list layer. These were the last five unmigratable methods of this class:
       their parameters mangle `P10MemoryNode` and `PNS0_6TargetE`, so they could not be
       declared as members until MemoryNode existed as a class with a nested Target --
       see include/MemoryNode.h.

       Four are static by the arity test (declared parameters == body arguments, no room
       for `this`); FreeNode declares two and takes three, so it is an instance method.

       NOTE the first parameter of the static four is typed MemoryNode* because that is
       what the ROM's mangled name says, but every caller passes the allocator's embedded
       node-list at `this + 0x24`, not a real node. The original evidently treated that
       list head as a sentinel node. The bodies cast it back to the shape they use; the
       declaration follows the ROM, not the usage. */
    static void*       CreateNode(MemoryNode::Target* extent, u16 tag);
    static void*       LinkNode(MemoryNode* list, MemoryNode* node, MemoryNode* prev);
    static void*       UnlinkNode(MemoryNode* list, MemoryNode* node);

    /* THE LAST PARAMETER IS u16, NOT u32, AND THE IMPORTED SYMBOL WAS WRONG. It ended
       `Pvjj`. Declared that way this member is ONE WORD off, at +0x10c:

           ROM   e1dd33b8   ldrh r3, [sp, #0x38]
           u32   e59d3038   ldr  r3, [sp, #0x38]

       The fifth argument is stack-passed (AAPCS puts args 5+ on the stack) and the ROM
       reads only its low halfword -- the load width mwccarm picks from the parameter's
       DECLARED type. Three attempts to reproduce that from a u32 declaration were
       measured and all failed: a truncating local (999 words -- it re-homes the
       argument), `*(u16 *)&param` (5 words -- taking the address re-homes it), and casts
       at the use site (no effect; the width follows the declaration, not the use).

       Declared `u16` it mangles `Pvjt` and matches exactly. Nothing about the body
       changed -- the recovered body had always spelled this parameter `unsigned short`,
       which is why it reproduced while the member signature could not.

       WHY THE NAME WAS THE THING THAT WAS WRONG, and the general lesson: `Pvjj` came
       verbatim from the imported symbols.x, whose only backing is a hand-written
       declaration for CALLING this function from mod code. A caller cannot detect an
       over-wide integer parameter -- AAPCS widens it to a word on the way in either way,
       so the callee silently ignores the high half and the link resolves regardless.
       The width is observable only from inside the callee. Treat imported parameter
       types as unverified wherever a member signature cannot reproduce the bytes; the
       identifier and the address are far better attested than the types are.
       DeallocateAll's `PPFvPvPS_jE` was the same error, corrected the same way. */
    static void* AllocateNode(MemoryNode* list, MemoryNode* node, void* address,
                              u32 size, u16 fromHighEnd);

    /* FreeNode is still not declared. The type no longer blocks it -- it would mangle
       correctly now -- but it has not been attempted, and a declared-but-undefined
       member is a landmine. It declares two parameters and takes three, so when it does
       land it is an instance method. */
#endif
};

typedef char ExpandingHeapAllocator_size_must_be_0x38[
    sizeof(struct ExpandingHeapAllocator) == 0x38 ? 1 : -1];

#endif
