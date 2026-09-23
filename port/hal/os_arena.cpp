// Host OS arena: the memory region SetupRootHeap carves the root heap from.
//
// On the DS, data_020a0ea4 is the arena id Heap::SetupRootHeap passes and the
// func_02058* family are the NitroSDK arena accessors (get-low, get-high,
// set-low, allocate-from-low) over one table in the shared system block. On
// host the arena itself is one big block reserved at a fixed base, created on
// first use. The hosting targets run the ROM's own accessors over that table;
// the narrow smoke harnesses keep host stand-ins. THE OS ARENA TABLE block
// below says which is which and why.
#include <stdlib.h>
#include "dsstate_seg.h"
#include <stdio.h>

#if defined(_WIN32)
#include <windows.h>
#endif

typedef unsigned int u32;

enum { HOST_ARENA = 8 << 20 };   /* 8 MB: larger than the DS main-RAM arena */

static char *g_lo, *g_hi;

/* WHY THE ARENA WANTS A FIXED BASE (lane lk7, save states that outlive the run)
   ---------------------------------------------------------------------------
   The in-memory save state (hal/lk6_savestate.cpp) memcpy's the whole arena and
   restores it verbatim, which works because the block never moves for the life
   of the process: a pointer captured at save still addresses the same arena
   offset after restore. A save state written to DISK has to survive the process
   ENDING. On the next launch the arena is a fresh allocation, and a plain calloc
   lands wherever the host allocator puts it, which is a different absolute
   address every run. The disk state is full of raw absolute pointers into the
   arena (actor-list heads, model-matrix pointers, intrusive next-links), so it
   only loads correctly if the arena comes up at the SAME base it had when the
   state was written.

   So the arena is reserved at a fixed host address with VirtualAlloc, the same
   technique ntr/io.cpp uses to pin the DS ranges. The base sits above every DS
   range (main RAM 0x02000000..0x02400000, the shared block at 0x027ff000, and
   the MMIO/VRAM ranges up to 0x07000800) and well below 2GB so the truncating
   32-bit round-trips the loaders do stay exact. If the reservation fails at
   boot (something else holds the range), the arena falls back to calloc and the
   persist layer refuses disk load/save for the run; the in-memory slot still
   works exactly as before. port_arena_is_fixed() reports which path we took. */
#if defined(_WIN32)
enum : size_t { ARENA_FIXED_BASE = 0x30000000u };  /* 768 MB, above the DS ranges */
static int g_fixed_base;                            /* 1 = VirtualAlloc'd at ARENA_FIXED_BASE */
#endif

/* THE ARENA HAS TO BE DETERMINISTIC. It was a plain malloc, which left two
   things different on every run: the contents (whatever the host allocator
   last had there) and the absolute base. Both reach the game. Anything that
   reads a field before writing it picks up the old contents, and the carve in
   func_02058cd0 rounds `lo` up against the ABSOLUTE address, so a base that
   is not aligned to at least the largest alignment the game asks for shifts
   every later allocation by a different amount.
   The symptom was a walk_window selftest that produced five different final
   frames in six identical runs -- Mario ended up somewhere else each time,
   which makes any before/after comparison of the renderer meaningless.
   calloc zeroes it (the DS arena is cleared main RAM) and the 64K alignment
   makes every carve offset identical from one run to the next. */
enum { ARENA_ALIGN = 0x10000 };

/* The aligned base of the arena, i.e. g_lo as it stood the instant arena_init
   ran, before any carve advanced it. g_lo itself is the live carve cursor and
   moves; g_base does not. The save-state code (hal/lk6_savestate.cpp) needs the
   fixed base and the fixed ceiling to bound the region it snapshots, plus the
   live cursor as a value it captures and restores like any other allocator
   state. */
static char *g_base;

static void arena_init(void)
{
    if (!g_lo) {
        /* soaks and tools can ask for more than the DS ever had */
        const char *env = getenv("SM64DS_HOST_ARENA_MB");
        size_t mb = env ? (size_t)atoi(env) : 0;
        size_t size = mb ? mb << 20 : (size_t)HOST_ARENA;

#if defined(_WIN32)
        /* Try the fixed base first. ARENA_FIXED_BASE is 64K-aligned, so the
           reservation starts exactly there and no post-alignment is needed --
           the base is deterministic across runs, which is what the disk save
           state leans on. VirtualAlloc zeroes committed pages, matching the
           calloc'd DS arena. */
        void *fixed = VirtualAlloc((void *)ARENA_FIXED_BASE, size,
                                   MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (fixed) {
            g_lo = (char *)fixed;
            g_hi = g_lo + size;
            g_base = g_lo;
            g_fixed_base = 1;
            return;
        }
        /* Fell through: something holds 0x30000000. Say so once and keep the
           game running on a calloc'd arena; the persist layer reads
           port_arena_is_fixed() and turns disk states off for the run. */
        fprintf(stderr, "[savestate] arena not at fixed base; disk states off this run\n");
#endif
        char *base = (char *)calloc(size + ARENA_ALIGN, 1);
        if (!base) return;
        g_lo = (char *)(((size_t)base + (ARENA_ALIGN - 1))
                        & ~(size_t)(ARENA_ALIGN - 1));
        g_hi = g_lo + size;
        g_base = g_lo;
    }
}

#ifdef SM64DS_OS_ARENA_ROM
/* The ROM's OS arena table in the shared system block (NitroSDK OSArenaInfo,
   OS_ARENA_MAX = 9 ids): lo[id] at 0x027ffda0, hi[id] at 0x027ffdc4. The
   addresses are the ROM's own: src/func_02058ea0.c, src/func_02058eb4.c and
   src/func_02058d58.c spell them, and the cartridge's bodies at 0x02058ea0 /
   0x02058eb4 / 0x02058d58 compute 0x027ff000 + 0xda0 / 0xdc4 + id * 4. */
typedef unsigned int os_word;
static os_word *const os_arena_lo_word = (os_word *)(size_t)0x027ffda0u;
static os_word *const os_arena_hi_word = (os_word *)(size_t)0x027ffdc4u;
enum { OS_ARENA_MAIN = 0 };
/* 1 once os_arena_seed has run. Host session state, not DS state: it only
   ever goes 0 -> 1, at the first arena read of the process. */
static int g_os_seeded;
#endif

extern "C" {
/* Read-only arena window accessors for the save-state layer. base and end are
   fixed for the process lifetime; cursor is the live low-water carve pointer.
   All three return 0 before the first carve forces arena_init, which is why
   the save-state code calls port_arena_base() (it runs arena_init) before
   reading any of them.

   WHERE THE CURSOR LIVES. On the narrow harnesses it is g_lo, which the host
   stand-ins below move. On the hosting targets (SM64DS_OS_ARENA_ROM) the ROM's
   own func_02058d58 / func_02058cd0 move the OS_ARENA_MAIN lo word of the
   table instead, so once the table is seeded the cursor IS that word: the
   save state captures and restores it exactly as it captured g_lo before.
   After boot both read the arena end, because Heap::SetupRootHeap carves the
   whole arena into the root heap. */
void *port_arena_base(void)   { arena_init(); return g_base; }
void *port_arena_end(void)    { arena_init(); return g_hi; }
void *port_arena_cursor(void)
{
    arena_init();
#ifdef SM64DS_OS_ARENA_ROM
    if (g_os_seeded)
        return (void *)(size_t)os_arena_lo_word[OS_ARENA_MAIN];
#endif
    return g_lo;
}
void  port_arena_set_cursor(void *p)
{
#ifdef SM64DS_OS_ARENA_ROM
    if (g_os_seeded) {
        os_arena_lo_word[OS_ARENA_MAIN] = (os_word)(size_t)p;
        return;
    }
#endif
    g_lo = (char *)p;
}

/* 1 if the arena came up at its fixed host base (VirtualAlloc succeeded), 0 if
   it fell back to calloc. The disk save state (hal/lk7_persist.cpp) is only
   loadable and savable when this is 1, because a disk state carries absolute
   pointers into the arena and only a fixed base makes them valid across a
   restart. On non-Windows the arena is always calloc'd, so this is 0 and disk
   states are off (the shipped port is Windows-only). */
int port_arena_is_fixed(void)
{
    arena_init();
#if defined(_WIN32)
    return g_fixed_base;
#else
    return 0;
#endif
}

/* THE INSTALL VERDICT, and why it lives in this file.

   WHAT THE ASSET FOLDER TURNED OUT TO BE is invisible in a crash dump, and the
   dump is the only thing a remote report gives us. The 2026-08-26 burst spent
   real human time on exactly this question -- six dumps read side by side,
   comparing SM64DS_ASSET_ROOT in the env block against the version in the
   launcher metadata -- and the answer that comparison produced was WRONG,
   because a folder's NAME is not its contents. The facts that actually settle
   it are known at BOOT (hal/romdata_loader.cpp knows which blob it loaded and
   whether the exe was built for it; hal/lk7_persist.cpp knows whether a disk
   state was loaded or refused and why), and they are gone by the time anything
   faults. So they are written here, once, into a plain host .bss buffer, and
   tests/fault_probe.h copies it verbatim into the dump.

   Note what this block does and does not settle. `romdata ... MATCH` says the
   game data in the folder is the data this exe was built for, whatever the
   folder is called -- it would have ended that argument in an hour. It says
   nothing about whether a restored world is SOUND; a save state can carry every
   matching stamp and still restore a broken world (see hal/lk7_persist.cpp).

   THIS BUFFER IS HOST SESSION STATE AND MUST NOT BE IN .dsstate. It describes
   the process, not the game: rolling it back on an F9 would make a dump
   describe the install as it was at save time. It sits outside the
   DSSTATE_BEGIN/END bracket below deliberately, and its name carries no DS
   address, so tools/dsstate_guard.py correctly ignores it.

   It lives in os_arena.cpp because that is the one hal file every target that
   can write a dump already links -- lk7_persist needs port_arena_base, and the
   ROM-CLEAN window targets link both. tests/fault_probe.h still carries a weak
   default (the /alternatename trick it already uses for port_build_gittip) so
   the narrow probes that include it without this file keep linking. */
char port_install_verdict[512];

/* WHICH GAME DATA THIS PROCESS BOOTED FROM, for the same reason and with the
   same placement rule. hal/romdata_loader.cpp fills these once, at boot, from
   the blob it verified; hal/lk7_persist.cpp stamps them into a disk save state
   and compares them on load, because a save state is a snapshot of a world that
   this data produced and no EXE-side field can see the difference. The storage
   is here rather than in the loader because the loader is compiled only into
   ROM-CLEAN targets while lk7_persist is compiled into more than that; empty
   and zero is the correct reading for a build that loads no blob.

   Host session state, deliberately outside the .dsstate bracket: an F9 must not
   roll back which folder the process booted from. */
char               port_romdata_sha[65];
unsigned long long port_romdata_bytes;
char               port_asset_root_seen[512];

/* Append one line, bounded, always NUL-terminated. Safe to call before or after
   anything. A line that does not fit is TRUNCATED, not dropped, and a buffer
   with fewer than three bytes left loses the CRLF separator too, so the tail
   would run on -- at 512 bytes with two lines of roughly 200 that cannot
   happen today, and it is written down rather than prevented because this is
   diagnostic text in a process that may be about to die, not a log. */
void port_install_verdict_add(const char *line)
{
    unsigned n = 0, i = 0;
    if (!line) return;
    while (n < sizeof port_install_verdict - 1 && port_install_verdict[n])
        ++n;
    /* CRLF: the dump this ends up in (tests/fault_probe.h) is a Windows text
       file the player opens in Notepad, and every other line in it is CRLF. */
    if (n && n + 2 < sizeof port_install_verdict - 1) {
        port_install_verdict[n++] = '\r';
        port_install_verdict[n++] = '\n';
    }
    while (line[i] && n < sizeof port_install_verdict - 1)
        port_install_verdict[n++] = line[i++];
    port_install_verdict[n] = 0;
}
}

extern "C" {
// The arena id Heap::SetupRootHeap passes to every accessor below
// (ROOT_HEAP_ARENA_ID, bound here by /alternatename in hal/cxx_aliases.cpp).
// Heap::InitializeRootHeap stores 0 into it, OS_ARENA_MAIN. The narrow
// harnesses' stand-ins ignore it; the ROM's accessors index the table by it.
DSSTATE_BEGIN
char data_020a0ea4[4];
DSSTATE_END

/* THE OS ARENA TABLE (run linkfull wave 23, lane OSARENA1; LINK15 batch W23-4)
   -------------------------------------------------------------------------
   The five PORT_HOST_ABI rows this block used to carry all said the DS
   arena words at 0x027ffda0 / 0x027ffdc4 were UNMAPPED. They have not been
   since 08-26: ntr/io.cpp maps the shared system block 0x027ff000..0x027fffff
   as a required region, so the ROM's accessor bodies can run on the table as
   they are. What the port never did was WRITE the table, which on the DS is
   OS_InitArena's job (func_02058f28, the first arm of func_02058c84).
   hal/boot_os.cpp's pre-main span skips that arm ("the OS arena, owned by
   hal/os_arena.cpp"), so this file seeds it, from the port's own arena block.

   SM64DS_OS_ARENA_ROM selects the ROM side. port/CMakeLists.txt's W23-4 block
   sets it on the three hosting targets' compile of this file ONLY (the shared
   object library and walk_window, walk_window_hires, smoke_player), the same
   targets port/slice_w23_arena.txt puts the ROM bodies on:

     func_02058eb4  OS_GetArenaHi        src/, slice_w23_arena
     func_02058d58  OS_SetArenaLo        src/, slice_w23_arena
     func_02058cd0  OS_AllocFromArenaLo  src/, slice_w23_arena
     func_02058ea0  OS_GetArenaLo        here: the ROM's own read, after the seed
     func_02059040  OS_InitAlloc         here: held, see its own comment

   THE SEED is what the ROM's OS_InitArena writes for OS_ARENA_MAIN, with the
   port's block in place of the cartridge's linker-defined bounds:
       hi[OS_ARENA_MAIN] = g_hi   (the end of the arena block)
       lo[OS_ARENA_MAIN] = g_lo   (its 64K-aligned base, nothing carved yet)
   on the default 8 MB fixed-base arena that is lo 0x30000000, hi 0x30800000.
   Every other id stays 0. OS_InitArena also fills MAINEX (with 0), ITCM,
   DTCM, SHARED and WRAM_MAIN from DS ranges this port does not map, and no
   linked ROM code reads any id but 0 (the only linked caller of these five
   is Heap::SetupRootHeap, with id 0); a 0 lo makes OS_AllocFromArenaLo
   refuse, which is the honest answer for an arena that does not exist here.

   WHY THE SEED RUNS AT THE FIRST ARENA READ AND NOT IN arena_init OR AT
   START-UP. Two things have to be true when it runs: the arena block exists,
   and ntr holds the shared block. The first read of the table is always
   func_02058ea0 at the top of Heap::SetupRootHeap (func_02058cd0 reads lo
   through it too), and every hosting target calls SetupRootHeap after
   ntr::io_init has succeeded, so both are true there by construction.
   arena_init is also reachable from host callers whose timing is not tied
   to io_init (port_arena_base from the GPU device and the save-state layer),
   and a static initialiser runs after ntr's early TLS claim but BEFORE
   io_init's retry: on a launch where the early claim lost the shared block
   and the retry won it (the rescue ntr/io.cpp exists for), a start-up seed
   would find no table to write and Heap::SetupRootHeap would fault. So
   func_02058ea0 stays in this file as the seed point. It retires the day the
   seed moves to the ROM's own point in the order, hal/boot_os.cpp's
   func_02058f28 line in port_boot_rom_pre_main (and smoke_player's main,
   which calls SetupRootHeap straight after io_init). */
#ifdef SM64DS_OS_ARENA_ROM
static void os_arena_seed(void)
{
    if (g_os_seeded)
        return;
    g_os_seeded = 1;
    arena_init();
    if (!g_lo)
        return;   /* no arena block: leave the table 0, the allocator refuses */
    /* OS_InitArena's order: hi first, then lo (func_02058d6c, func_02058d58) */
    os_arena_hi_word[OS_ARENA_MAIN] = (os_word)(size_t)g_hi;
    os_arena_lo_word[OS_ARENA_MAIN] = (os_word)(size_t)g_lo;
    fprintf(stderr, "[arena] OS arena %d seeded: lo %08x hi %08x\n",
            (int)OS_ARENA_MAIN, os_arena_lo_word[OS_ARENA_MAIN],
            os_arena_hi_word[OS_ARENA_MAIN]);
}

/* PORT_HOST_ABI: OS_GetArenaLo is the port's OS_InitArena seam: the first arena read seeds the table (hal/boot_os.cpp skips func_02058f28), then the ROM's own read.
   The return statement IS src/func_02058ea0.c's body; the seed call is the
   only difference, and it does nothing after the first call. */
unsigned int func_02058ea0(int idx)
{
    os_arena_seed();
    return os_arena_lo_word[idx];
}

/* PORT_HOST_ABI: OS_InitAlloc is HELD: the ROM body reserves its 0x44-byte OS heap-info block at the arena base and returns base+0x60, which moves the root heap; W23-4 keeps the root heap bounds identical.
   Measured against the cartridge (arm9 0x02059040, 0xbc bytes; match.py
   2004/b56 MATCHING on src/func_02059040.c): it is NitroSDK OS_InitAlloc.
   Heap::SetupRootHeap calls it as (id, lo, hi, 4); it stores lo into
   OSiHeapInfo[id] (data_020a637c), builds the heap-info header plus four
   0xc-byte descriptors at lo, and returns round32(lo + 0x14 + 4 * 0xc). On
   the port's arena that is 0x30000060, so the ROM body would start the root
   heap 0x60 bytes higher and 0x60 bytes smaller than this stand-in does, and
   every allocation address would move by the same amount. That is the
   cartridge's own layout, and the brief this lane ran under says a root-heap
   bound that moves by a byte is reverted, so the row is reported for a ruling
   rather than taken. What this stand-in returns is unchanged: lo rounded up
   to the fourth argument, which is lo itself on the 64K-aligned base. */
int func_02059040(int idx, int lo, int hi, int align)
{
    (void)idx;
    (void)hi;
    return (lo + align - 1) & ~(align - 1);
}
#else
/* THE NARROW HARNESSES' STAND-INS. Every smoke_* target that links
   Heap::SetupRootHeap links it without port/slice_w23_arena.txt, and two of
   them (smoke_roots, smoke_fs) link no ntr at all, so there is no shared
   block under the table: these five host bodies are the only definitions
   those links can have. They carve the same arena the same way they always
   did. */
int func_02058ea0(void *) { arena_init(); return (int)(size_t)g_lo; }   /* arena lo */
int func_02058eb4(void *) { arena_init(); return (int)(size_t)g_hi; }   /* arena hi */

/* the round-up SetupRootHeap's OS_InitAlloc call returns here: see the
   SM64DS_OS_ARENA_ROM arm above for what the ROM body does instead */
int func_02059040(void *, int lo, int hi, int align)
{
    (void)hi;
    return (lo + align - 1) & ~(align - 1);
}

void func_02058d58(void *, int newLo) { g_lo = (char *)(size_t)newLo; }  /* set lo */

/* carve `size` bytes aligned `align` from the low side */
void *func_02058cd0(void *, int size, int align)
{
    arena_init();
    char *p = (char *)(((size_t)g_lo + align - 1) & ~(size_t)(align - 1));
    if (p + size > g_hi)
        return 0;
    g_lo = p + size;
    return p;
}
#endif
}

/* ===================================================================== *
 * THE GLOBAL DEALLOCATOR SEAM, AND WHY IT IS DECIDED IN THIS FILE
 * ===================================================================== *
 *
 * This image contains TWO programs with two separate memory systems, and
 * C++ gives a program exactly ONE global operator new / operator delete
 * pair. That is the whole defect.
 *
 *   THE CARTRIDGE'S SYSTEM. On the DS the pair is matched and both halves
 *   are the game heap: _Znwj at 0x0203cbe4 tail-calls func_0203cc0c, which
 *   is Heap::Allocate(data_020a0ea0, size), and _ZdlPv at 0x0203cbf0 is
 *   Memory::defaultHeapPtr->_Deallocate(ptr). Both are matched and linked
 *   here, and that heap is carved out of THIS FILE'S arena.
 *
 *   THE HOST'S SYSTEM. The ntr graphics emulation, the sound host, the
 *   editor channel and every std::vector / std::string / std::function
 *   inside them allocate from the CRT. There is no DS equivalent of any of
 *   it; it does not exist on a cartridge.
 *
 * WHAT WENT WRONG. src/_ZdlPv.cpp is a real `operator delete(void*)`
 * definition, so MSVC binds it as the program-wide global deallocator --
 * correctly, because ROM code needs it: a census of the linked image finds
 * FIVE ROM call sites reaching it (ModelD1Ev, BlendModelAnimD1Ev,
 * CleanCommonModelDataArr, func_0203cbc0, func_02073244) plus every ROM
 * class whose MSVC scalar deleting destructor runs. But src/_Znwj.cpp
 * cannot match it on the allocating side: mwccarm 2004/b56 rejects
 * `void* operator new(u32)` outright, so _Znwj stays a hand-spelled flat
 * name and the CRT's operator new wins. Host allocations were therefore
 * malloc'd by Windows and handed back to the cartridge's allocator, which
 * has never seen them, and ExpandingHeapAllocator::UnlinkNode faulted
 * walking a node header that was really somebody else's malloc block.
 *
 * WHY THE SIZED FORM IS THE WHOLE FIX. The census says the host reaches
 * the ROM deallocator through exactly ONE edge: LIBCMT's sized-delete
 * shim ??3@YAXPAXI@Z, which every host deallocation funnels through and
 * which does nothing but forward to the unsized global. Replacing that one
 * shim with a router splits the two systems apart and touches nothing
 * else. src/_ZdlPv.cpp keeps its definition, stays byte-matched and stays
 * linked; no decomp file moves; no TU leaves the link.
 *
 * THE TEST IS EXACT, NOT A HEURISTIC. The arena is one contiguous region
 * reserved at a fixed base above every DS range, and the ROM heap is
 * carved from it, so a pointer belongs to the cartridge if and only if it
 * lies inside [g_base, g_hi). Nothing else can be allocated there: the
 * region is reserved and committed to us for the life of the process.
 *
 * DELIBERATELY NO arena_init() HERE. If the arena does not exist yet then
 * the game has not allocated anything yet, so the pointer is necessarily a
 * host one and free() is the right answer. Reading the bounds without
 * forcing the arena up also keeps arena creation at exactly the moment it
 * happened before, which the determinism this file argues for depends on.
 *
 * NOT DONE, AND ON PURPOSE: the allocating side is left alone. ROM code
 * never calls the global operator new in this link (34 call sites, every
 * one host), so routing it to the game heap would buy nothing and would
 * pour host vertex buffers into the cartridge's 8 MB arena, changing the
 * addresses the ROM's own allocator hands out. That would be a divergence
 * from the cartridge, not a step towards it.
 */
#include <new>

void operator delete(void *ptr, size_t) noexcept
{
    if (!ptr)
        return;
    if ((char *)ptr >= g_base && (char *)ptr < g_hi) {
        /* The cartridge allocated it. Hand it to the ROM's own matched
           deallocator, which is src/_ZdlPv.cpp's operator delete(void*). */
        ::operator delete(ptr);
        return;
    }
    /* Windows allocated it, through the CRT's operator new. */
    free(ptr);
}
