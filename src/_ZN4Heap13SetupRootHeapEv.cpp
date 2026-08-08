//cpp
// @symbol _ZN4Heap13SetupRootHeapEv
/* Heap::SetupRootHeap() at 0x0203cb04 -- take the whole of the NitroSDK arena
 * selected by ROOT_HEAP_ARENA_ID and turn it into the root heap.
 *
 * The sequence is the standard OS_*Arena dance: read lo and hi, round lo up to
 * a 32-byte boundary and publish it back so nothing else claims the space, then
 * allocate everything from there to hi. `sz' is deliberately computed from the
 * RE-READ hi rather than from the one captured earlier.
 *
 * ROOT_HEAP_ARENA_ID is an arena id, not a pointer: Heap::InitializeRootHeap
 * stores 0 into it to select OS_ARENA_MAIN, and it is passed by value as the
 * first argument to each OS entry point. The shadow this file used to carry
 * declared it `void*', which is the same word in r0 and so byte-identical --
 * and wrong.
 *
 * The five OS entry points are still func_* . They live outside this class and
 * naming them is not this slice's claim to make. */
#include "Heap.h"

extern "C" {
    /* `extern' is load-bearing on both of these, and specifically because they
       sit inside a BRACED linkage block. Written `extern "C" u32 X;' on its own
       line the extern specifier is implied and the line is a declaration --
       _ZN4Heap18InitializeRootHeapEv.cpp spells ROOT_HEAP_ARENA_ID that way and
       links. Written `extern "C" { u32 X; }' mwccarm emits a DEFINITION, which
       collides with the ROM's own bss:

         mwldarm.exe: Multiply-defined: "data_020a0e9c" in
                      _ZN4Heap13SetupRootHeapEv.o
         mwldarm.exe: Previously defined in _dsd_gap@main_40.o

       build_pin.verify compiles a single file and never links, so it returned
       (True, '2004/b56') on the broken spelling. Only rombuild.py caught it. */
    extern u32   ROOT_HEAP_ARENA_ID;            /* 0x020a0ea4 */
    extern Heap* data_020a0e9c;                 /* the root heap; unnamed */

    int   func_02058ea0(u32 arena);             /* arena lo */
    int   func_02058eb4(u32 arena);             /* arena hi */
    int   func_02059040(u32 arena, int lo, int hi, int align);
    int   func_02058d58(u32 arena, int lo);     /* publish the new lo */
    void* func_02058cd0(u32 arena, int size, int align);
}

namespace Memory { extern Heap* defaultHeapPtr; }   /* 0x020a0ea0 */

Heap* Heap::SetupRootHeap()
{
    int lo = func_02058ea0(ROOT_HEAP_ARENA_ID);
    int hi = func_02058eb4(ROOT_HEAP_ARENA_ID);

    lo = func_02059040(ROOT_HEAP_ARENA_ID, lo, hi, 4);
    func_02058d58(ROOT_HEAP_ARENA_ID, lo);

    int top = func_02058eb4(ROOT_HEAP_ARENA_ID);
    int sz = top - ((lo + 0x1f) & ~0x1f);

    void* mem = func_02058cd0(ROOT_HEAP_ARENA_ID, sz, 0x20);
    if (mem == 0)
        goto fail;

    {
        Heap* heap = CreateRootHeap(mem, sz);
        if (heap != 0)
        {
            data_020a0e9c = heap;
            Memory::defaultHeapPtr = heap;
        }
        return heap;
    }

    /* The failure return sits at the END of the function, and that placement is
       load-bearing rather than stylistic: the ROM emits `beq' to a tail block
       holding its own `mov r0,#0 / pop / bx'. Written as a plain early
       `return 0;' the compiler predicates it in place instead --
       `moveq/popeq/bxeq' -- which is correct code and four bytes short of the
       ROM. The .c file this replaces used the same `goto fail'. */
fail:
    return 0;
}
