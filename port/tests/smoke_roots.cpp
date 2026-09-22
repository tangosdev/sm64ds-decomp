// Gate-3a smoke: the game's global heap bring-up on a host arena.
//
// SetupRootHeap -> CreateRootHeap -> ExpandingHeap over the gate-2 allocator,
// then Memory::Allocate/Deallocate -- the exact allocation path every game
// system uses -- exercised end to end on the host OS-arena seam.
//
// THE BRING-UP ENTRY POINT MOVED, and this harness follows it (lane
// SMOKELINK3, run link100 wave 10 round 2, continuing SMOKELINK2's rule for
// port/tests/smoke_heap.cpp). It used to declare the entry as a flat extern
// "C" Itanium name over a forward-declared `struct HeapS' --
//   HeapS *_ZN4Heap13SetupRootHeapEv(void);
// -- which is what the ROM's own callers spell and what src/ emitted before
// the 09-14 sync. It is now Heap::SetupRootHeap(), a real static member
// (include/Heap.h:236, ?SetupRootHeap@Heap@@SAPAU1@XZ), so the flat spelling
// bought a link error.
//
// This file is HOST TEST CODE, not ROM code, so it may include the class
// header and call the member the way C++ calls it -- which is also what the
// synced src/ TUs now do. It reaches the same object it always linked,
// src/_ZN4Heap13SetupRootHeapEv.cpp; only the spelling changed, and the
// harness now gets the real Heap type instead of an opaque one.
//
// SetupRootHeap is a STATIC, so this particular row would also have linked
// through an /alternatename. It is ported anyway: the flat name it used to
// spell is gone from the tree, so an alias would be a bridge to a spelling
// nothing else uses, and a harness is the one place in this build that can
// simply say what it means.
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "Heap.h"

extern "C" {
int func_02058ea0(void *);
int func_02058eb4(void *);
}
namespace Memory {
void *Allocate(u32 size);
void Deallocate(void *p);
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static u32 lcg_state = 0xC0FFEEu;
static u32 lcg(void) { return lcg_state = lcg_state * 1664525u + 1013904223u; }

int main(void)
{
    Heap *root = Heap::SetupRootHeap();
    CHECK(root != NULL);
    char *arena_lo = (char *)(size_t)func_02058ea0(0);
    char *arena_hi = (char *)(size_t)func_02058eb4(0);

    /* allocations land inside the host arena and are usable */
    void *a = Memory::Allocate(256);
    CHECK(a != NULL);
    CHECK((char *)a > arena_lo - (8 << 20) && (char *)a < arena_hi);
    memset(a, 0x5A, 256);
    CHECK(((unsigned char *)a)[255] == 0x5A);
    Memory::Deallocate(a);

    /* torture through the global path */
    enum { SLOTS = 48, OPS = 3000 };
    struct { void *p; u32 n; unsigned char tag; } slot[SLOTS] = {};
    u32 allocs = 0, frees = 0;
    for (u32 op = 0; op < OPS; ++op) {
        u32 i = lcg() % SLOTS;
        if (!slot[i].p) {
            u32 n = 1 + lcg() % 4096;
            void *p = Memory::Allocate(n);
            if (!p) continue;
            memset(p, (unsigned char)(0x21 + i), n);
            slot[i].p = p; slot[i].n = n; slot[i].tag = (unsigned char)(0x21 + i);
            ++allocs;
        } else {
            unsigned char *p = (unsigned char *)slot[i].p;
            int intact = 1;
            for (u32 k = 0; k < slot[i].n; ++k)
                if (p[k] != slot[i].tag) { intact = 0; break; }
            CHECK(intact);
            Memory::Deallocate(slot[i].p);
            slot[i].p = NULL; ++frees;
        }
    }
    for (u32 i = 0; i < SLOTS; ++i)
        if (slot[i].p) { Memory::Deallocate(slot[i].p); }

    if (g_failures) {
        fprintf(stderr, "smoke_roots: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_roots: all checks passed (root heap up, %u allocs, %u frees "
           "through Memory::Allocate)\n", allocs, frees);
    return 0;
}
