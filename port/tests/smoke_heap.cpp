// Gate-2 smoke: the game's ExpandingHeapAllocator running on a host arena.
//
// Everything here goes through the exact entry points the game uses, the way
// the src/ TUs call each other. A fill-pattern torture test is the point: the
// LP64 bug class corrupted allocator node headers silently, and an allocator
// that survives thousands of randomized alloc/free cycles with per-block
// patterns intact is strong evidence the recovered layouts are byte-faithful
// on the host.
//
// THE THREE ENTRY POINTS MOVED, and this harness follows them (lane
// SMOKELINK2, run link100 wave 10 round 2). It used to declare them as flat
// extern "C" Itanium names with the receiver as a first stack argument --
//   ExpandingHeapAllocator *_ZN4Heap28CreateExpandingHeapAllocatorEPvjj(...)
//   void *_ZN22ExpandingHeapAllocator8AllocateEji(self, size, align)
//   int _ZN22ExpandingHeapAllocator10DeallocateEPv(self, ptr)
// -- which is what the ROM's own callers spell and what the src/ TUs emitted
// before the 09-14 sync. They are now real C++ members: Allocate and
// Deallocate are __thiscall with the receiver in ECX
// (?Allocate@ExpandingHeapAllocator@@QAEPAXIH@Z,
// ?Deallocate@ExpandingHeapAllocator@@QAEHPAX@Z) and
// Heap::CreateExpandingHeapAllocator is a static
// (?CreateExpandingHeapAllocator@Heap@@SAPAUExpandingHeapAllocator@@PAXII@Z,
// include/Heap.h:252). Spelling the flat names here bought a link error; more
// to the point, an extern "C" declaration of a __thiscall member is the
// receiver-in-the-wrong-place bug the port keeps finding, and a test harness
// that calls the game wrongly proves nothing about the game.
//
// This file is HOST TEST CODE, not ROM code, so it may include the class
// headers and call the members the way C++ calls members -- which is also what
// the synced src/ TUs now do. The entry points are unchanged; only the
// spelling is. The three bodies it reaches are src/_ZN4Heap28Create...cpp,
// src/_ZN22ExpandingHeapAllocator8AllocateEji.cpp and
// src/_ZN22ExpandingHeapAllocator10DeallocateEPv.cpp, the same three objects
// this target has always linked.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "Heap.h"
#include "ExpandingHeapAllocator.h"

#define CreateAllocator Heap::CreateExpandingHeapAllocator
#define Alloc(a, n)     ((a)->Allocate((n), 4))
#define Free(a, p)      ((a)->Deallocate(p))

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

enum { ARENA = 1 << 20, SLOTS = 64, OPS = 5000 };

static u32 lcg_state = 0x1234567u;   /* fixed seed: the run is reproducible */
static u32 lcg(void) { return lcg_state = lcg_state * 1664525u + 1013904223u; }

int main(void)
{
    void *arena = malloc(ARENA);
    CHECK(arena != NULL);
    ExpandingHeapAllocator *heap = CreateAllocator(arena, ARENA, 0);
    CHECK(heap != NULL);

    /* basics: distinct, aligned, writable, freeable, reusable */
    void *a = Alloc(heap, 64), *b = Alloc(heap, 64), *c = Alloc(heap, 64);
    CHECK(a && b && c && a != b && b != c);
    CHECK(((size_t)a % 4) == 0 && ((size_t)b % 4) == 0);
    memset(a, 0xAA, 64); memset(b, 0xBB, 64); memset(c, 0xCC, 64);
    CHECK(((unsigned char *)a)[63] == 0xAA && ((unsigned char *)b)[0] == 0xBB);
    CHECK(Free(heap, b) != 0);
    void *b2 = Alloc(heap, 64);
    CHECK(b2 != NULL);              /* freed space is allocatable again */
    Free(heap, a); Free(heap, b2); Free(heap, c);

    /* torture: randomized alloc/free with per-slot fill patterns */
    struct { void *p; u32 n; unsigned char tag; } slot[SLOTS] = {};
    u32 live = 0, peak = 0, allocs = 0, frees = 0, failed_allocs = 0;
    for (u32 op = 0; op < OPS; ++op) {
        u32 i = lcg() % SLOTS;
        if (slot[i].p == NULL) {
            u32 n = 1 + lcg() % 2048;
            void *p = Alloc(heap, n);
            if (!p) { ++failed_allocs; continue; }   /* full is legal */
            memset(p, (unsigned char)(0x40 + i), n);
            slot[i].p = p; slot[i].n = n; slot[i].tag = (unsigned char)(0x40 + i);
            ++allocs; ++live; if (live > peak) peak = live;
        } else {
            unsigned char *p = (unsigned char *)slot[i].p;
            int intact = 1;
            for (u32 k = 0; k < slot[i].n; ++k)
                if (p[k] != slot[i].tag) { intact = 0; break; }
            CHECK(intact);          /* neighbours never scribbled on us */
            CHECK(Free(heap, slot[i].p) != 0);
            slot[i].p = NULL; ++frees; --live;
        }
    }
    for (u32 i = 0; i < SLOTS; ++i)   /* verify + drain the survivors */
        if (slot[i].p) {
            unsigned char *p = (unsigned char *)slot[i].p;
            int intact = 1;
            for (u32 k = 0; k < slot[i].n; ++k)
                if (p[k] != slot[i].tag) { intact = 0; break; }
            CHECK(intact);
            CHECK(Free(heap, slot[i].p) != 0);
        }

    /* coalescing: after full drain one near-arena-sized block must fit */
    void *big = Alloc(heap, ARENA - 0x200);
    CHECK(big != NULL);
    Free(heap, big);

    if (g_failures) {
        fprintf(stderr, "smoke_heap: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_heap: all checks passed (%u allocs, %u frees, peak %u live, "
           "%u full-arena rejections, coalesce OK)\n",
           allocs, frees, peak, failed_allocs);
    return 0;
}
