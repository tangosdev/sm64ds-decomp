/* hal/pmf_tc.cpp -- see hal/pmf_tc.h.

   WHERE THE STUBS LIVE, AND WHY IT HAS TO BE THE SAME PLACE EVERY RUN
   (run after093, lane DISKSTATE1).

   A stub's address is not private to this file. The seats write it into the
   state tables they rewrite (data_ov034_02114538, the Wiggler's eleven enter /
   tick cells, and some forty other tables like it), and those tables are hosted
   DS globals inside the .dsstate section. A save state captures the section, and
   lk7 writes it to disk. The stubs used to come out of a VirtualAlloc(0, ...)
   page, which lands somewhere else in every process, so a disk state loaded
   after a restart put the OLD process's stub addresses back into every seated
   table. The seats do not run again (their done-flags ride back in with the
   section), so the first dispatch through such a cell executed whatever the new
   process had at that address: the reported crash was the Wiggler's
   InitResources entering state 0 through its table at the next level load, an
   execute fault at a per-run address ending in 1d10 (+0x10 per cell).

   So the stub for a function is now put at an address computed from the
   function's own address: kRegionBase + 2 * (flat - text_lo), inside one range
   reserved at a fixed base (the hal/os_arena.cpp technique), committed a page at
   a time as stubs are made. /DYNAMICBASE:NO pins the image, so the same build
   puts the same stub at the same address in every process, and a captured word
   means the same thing after a restart. What a new process does not have is the
   stub BYTES for functions it has not stubbed yet; lk7 carries the list of
   stubbed functions in the disk state and calls port_pmf_tc_rebuild before the
   world is copied in.

   The stub is 8 bytes (push ecx; call rel32; pop ecx; ret) and slots are two
   bytes per byte of code, so two functions at least four bytes apart can never
   share bytes. If they ever did, or the fixed range is taken, or a function lies
   outside the image's code, the stub comes from the old per-process page
   instead and port_pmf_tc_pinned() turns 0, which lk7 reads as "disk states off
   this run", the same answer it gives when the arena missed its fixed base. */
#include "pmf_tc.h"
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {
struct Entry { const void *flat; void *stub; };
Entry g_ent[4096];
int g_n;

/* the per-process fallback page (the original scheme) */
unsigned char *g_page;
size_t g_used;
const size_t kPage = 65536;

/* the pinned range */
const uintptr_t kRegionBase = 0x2C000000u;   /* below the arena's 0x30000000 */
const size_t    kRegionMax  = 0x04000000u;   /* code up to 32 MB */
int       g_rgn_state;                       /* 0 untried, 1 pinned, -1 not */
uintptr_t g_text_lo, g_text_hi;
size_t    g_rgn_size;
int       g_volatile;                        /* a stub exists at a per-run address */

void region_init()
{
    if (g_rgn_state)
        return;
    g_rgn_state = -1;
    unsigned char *img = (unsigned char *)&__ImageBase;
    IMAGE_NT_HEADERS32 *nt = (IMAGE_NT_HEADERS32 *)(img + __ImageBase.e_lfanew);
    IMAGE_SECTION_HEADER *s = IMAGE_FIRST_SECTION(nt);
    uintptr_t lo = 0, hi = 0;
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++s) {
        if (!(s->Characteristics & IMAGE_SCN_MEM_EXECUTE))
            continue;
        uintptr_t a = (uintptr_t)img + s->VirtualAddress;
        uintptr_t b = a + s->Misc.VirtualSize;
        if (!lo || a < lo) lo = a;
        if (b > hi) hi = b;
    }
    if (!lo || hi <= lo)
        return;
    size_t size = ((hi - lo) * 2 + 8 + 0xFFFF) & ~(size_t)0xFFFF;
    if (size > kRegionMax) {
        std::fprintf(stderr, "[pmf_tc] code is %u bytes, larger than the pinned stub "
                     "range; stubs per run, disk save states off\n", (unsigned)(hi - lo));
        return;
    }
    void *r = VirtualAlloc((void *)kRegionBase, size, MEM_RESERVE, PAGE_NOACCESS);
    if (r != (void *)kRegionBase) {
        if (r) VirtualFree(r, 0, MEM_RELEASE);
        std::fprintf(stderr, "[pmf_tc] stub range 0x%08x not free; stubs per run, "
                     "disk save states off this run\n", (unsigned)kRegionBase);
        return;
    }
    g_text_lo = lo;
    g_text_hi = hi;
    g_rgn_size = size;
    g_rgn_state = 1;
}

void stub_bytes(unsigned char *out, const unsigned char *at, const void *flat)
{
    /* push ecx ; call rel32 flat ; pop ecx ; ret */
    out[0] = 0x51;
    out[1] = 0xE8;
    const int rel = (int)((const unsigned char *)flat - (at + 6));
    std::memcpy(out + 2, &rel, 4);
    out[6] = 0x59;
    out[7] = 0xC3;
}

/* The pinned slot for flat, or 0 when it has none. Never moves anything: a
   slot already holding this stub is returned as is. */
unsigned char *pinned_slot(const void *flat)
{
    region_init();
    if (g_rgn_state != 1)
        return 0;
    const uintptr_t f = (uintptr_t)flat;
    if (f < g_text_lo || f >= g_text_hi)
        return 0;
    unsigned char *p = (unsigned char *)kRegionBase + 2 * (f - g_text_lo);
    if (!VirtualAlloc(p, 8, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
        return 0;
    unsigned char want[8];
    stub_bytes(want, p, flat);
    if (std::memcmp(p, want, 8) == 0)
        return p;
    static const unsigned char zero[8] = {0};
    if (std::memcmp(p, zero, 8) != 0)
        return 0;                    /* a neighbour's stub overlaps: no slot */
    std::memcpy(p, want, 8);
    FlushInstructionCache(GetCurrentProcess(), p, 8);
    return p;
}

void *pool_stub(const void *flat)
{
    if (!g_page || g_used + 16 > kPage) {
        g_page = (unsigned char *)VirtualAlloc(0, kPage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!g_page) {
            std::fprintf(stderr, "FATAL: port_pmf_tc: no executable page\n");
            std::abort();
        }
        g_used = 0;
    }
    unsigned char *p = g_page + g_used;
    g_used += 16;
    stub_bytes(p, p, flat);
    if (!g_volatile) {
        g_volatile = 1;
        std::fprintf(stderr, "[pmf_tc] stub for %p is at a per-run address; disk save "
                     "states off this run\n", flat);
    }
    return p;
}

int is_stub(unsigned fn)
{
    if (g_rgn_state == 1 && fn >= kRegionBase && fn < kRegionBase + g_rgn_size)
        return 1;
    if (g_page && fn >= (unsigned)(size_t)g_page && fn < (unsigned)(size_t)g_page + kPage)
        return 1;
    for (int i = 0; i < g_n; ++i)
        if ((unsigned)(size_t)g_ent[i].stub == fn)
            return 1;
    return 0;
}
}

extern "C" void *port_pmf_tc(const void *flat)
{
    if (!flat)
        return 0;
    for (int i = 0; i < g_n; ++i)
        if (g_ent[i].flat == flat)
            return g_ent[i].stub;
    if (g_n >= 4096) {
        std::fprintf(stderr, "FATAL: port_pmf_tc table full\n");
        std::abort();
    }
    void *p = pinned_slot(flat);
    if (!p)
        p = pool_stub(flat);
    g_ent[g_n].flat = flat;
    g_ent[g_n].stub = p;
    ++g_n;
    return p;
}

/* port_pmf_tc_cells -- put the thiscall adapter over the function word of each listed
   {fn, delta} pointer-to-member cell whose word is a FLAT function. For the tables main
   builds natively out of `union { void (*fn)(T *); void (T::*pmf)(); }` descriptors (the
   mwcc-only free-function-as-member pun): the host initialiser stores the free function's
   own address, which the native dispatch then enters with `this` in ECX. Call it after the
   class TU's initialiser has run. A cell already holding a stub, or null, is left alone. */
extern "C" void port_pmf_tc_cells(void *base, const int *idx, int n)
{
    region_init();
    unsigned *c = (unsigned *)base;
    for (int k = 0; k < n; ++k) {
        unsigned *cell = c + idx[k] * 2;
        const unsigned fn = cell[0];
        if (!fn || cell[1] != 0)
            continue;
        if (is_stub(fn))
            continue;
        cell[0] = (unsigned)(size_t)port_pmf_tc((const void *)(size_t)fn);
    }
}

/* ---- what lk7 needs (hal/lk7_persist.cpp) ---------------------------------- */

extern "C" int port_pmf_tc_pinned(void)
{
    region_init();
    return g_rgn_state == 1 && !g_volatile;
}

extern "C" unsigned port_pmf_tc_base(void)
{
    region_init();
    return g_rgn_state == 1 ? (unsigned)kRegionBase : 0u;
}

extern "C" int port_pmf_tc_count(void) { return g_n; }

extern "C" void port_pmf_tc_entry(int i, unsigned *flat, unsigned *stub)
{
    *flat = (unsigned)(size_t)g_ent[i].flat;
    *stub = (unsigned)(size_t)g_ent[i].stub;
}

/* Make the stub for every listed function, each of which must land at the
   address the list says (the writer's process put it there). Returns -1 when
   all of them do, else the index of the first one that does not, which lk7
   turns into a refusal before it copies a byte of the world. Stubs this
   process already had are found, not remade. */
extern "C" int port_pmf_tc_rebuild(const unsigned *pairs, int n)
{
    for (int i = 0; i < n; ++i) {
        const unsigned flat = pairs[2 * i], stub = pairs[2 * i + 1];
        if ((unsigned)(size_t)port_pmf_tc((const void *)(size_t)flat) != stub)
            return i;
    }
    return -1;
}
