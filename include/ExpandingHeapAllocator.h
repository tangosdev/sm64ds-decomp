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
#include "MemoryNode.h"   /* the five node methods below take it by pointer */

/* The generator also emitted `struct align_; struct ptr; struct size_;` as forward
 * declarations -- parameter names mistaken for type names. No source ever referenced
 * them; dropped, as in SolidHeapAllocator.h. */
struct ExpandingHeapAllocator {
    u8  pad_000[0xc];
    s32 unk_00c;            /* 0x00c */
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x7];
    s32 unk_018;            /* 0x018 */
    s32 unk_01c;            /* 0x01c */
    s32 unk_020;            /* 0x020 */
    u8  unk_024;            /* 0x024 */
    u8  pad_025[0x7];
    u8  unk_02c;            /* 0x02c */
#ifdef __cplusplus
    /* methods. Parameter types are read off the mangled name: `Eji` is
       (unsigned int, int), `EPv` is (void*), `Ev` is (). */
    void* Allocate(u32 size, int align);
    int   Deallocate(void* ptr);
    u32   GetNodeID();
    int   SetNodeID(u32 id);              /* returns the previous ID */
    u32   MemoryLeft();                   /* sum of every free node's size */
    int   MaxAllocatableSize(int align);  /* largest single block, at that alignment */

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

    /* AllocateNode and FreeNode are NOT declared here yet, deliberately. The type no
       longer blocks them -- both would mangle correctly now -- but AllocateNode does not
       yet reproduce, and a declared-but-undefined member is a landmine. AllocateNode is
       ONE WORD off, at +0x10c:

           ROM   e1dd33b8   ldrh r3, [sp, #0x38]
           ours  e59d3038   ldr  r3, [sp, #0x38]

       Its fifth argument is stack-passed (AAPCS puts args 5+ on the stack), and the ROM
       reads only its low halfword -- the load width mwccarm picks when the declared type
       is 16 bit. But the mangled name ends `jj`, fixing both trailing parameters as u32,
       and a u32 parameter gets a word load. The recovered body squared that circle by
       declaring the parameter `unsigned short`, which reproduces but cannot be a member
       signature. Tried and rejected: a truncating local (999 words -- it changes how the
       argument is homed), `*(unsigned short *)&param` (5 words -- taking the address
       re-homes it), and casts at the use site (no effect; the load width is chosen from
       the parameter's declared type, not from what is done with the value).

       So this is the same shape as DeallocateAll's `PPFvPvPS_jE`: the mangled name and
       the reproducing body disagree about a parameter type, and the ROM's own codegen is
       the tiebreaker evidence. Worth its own investigation rather than a guess. */
#endif
};

#endif
