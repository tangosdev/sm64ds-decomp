// ---------------------------------------------------------------------------
// rom_main_scout.cpp -- MEASUREMENT ONLY (run link100, lane BOOTSCOUT).
//
// THIS IS NOT A LANDING. It exists to answer one question with a build instead
// of an argument: can the ROM's own src/main.c BE the port's boot spine, and
// what does the linkage number do when it is.
//
// WHAT THE PORT DOES TODAY. tests/walk_window.cpp defines main() for Windows,
// so src/main.c -- the ROM's own eighteen-line main, arm9 0x02007000, 0x5c
// bytes -- is an UNDOCUMENTED SHADOW in linkage.py's queue and 141 further
// matched TUs sit behind it with no other way in. main's body is transcribed
// instead, span by span, in hal/boot_os.cpp: port_boot_rom_main_head() is
// main's first three calls, and the func_0201a054 pair is main's fifth.
//
// WHAT THIS ADDS. src/main.c is compiled with a per-source rename, the same
// mechanism port/CMakeLists.txt already uses for func_0203bb60_narrow:
//
//     main             -> rom_main               so the CRT's main still wins
//     func_0201a054    -> port_rom_a054_seam     the two calls this rung does
//     func_020197b8    -> port_rom_loop_seam     not yet run as ROM code
//
// The rename is deliberate and it is the honest form of this rung. Defining
// func_0201a054 and func_020197b8 as host bodies would have created two new
// SHADOW rows in the replacement queue -- work invented by the measurement --
// while the rename leaves both ROM TUs exactly where they were, unenrolled and
// unclaimed, and says in one place that main's last two edges are not ROM code
// yet. What DOES become ROM code is main itself and Heap::SetNodeID, which the
// ROM calls twice from main and the port has never run.
//
// The gate is SM64DS_ROM_MAIN and it defaults OFF. With it off this file
// changes nothing that executes: the same port_boot_rom_main_head() runs at
// the same line of tests/walk_window.cpp.
//
// KNOWN DEVIATION WITH THE GATE ON, stated because it is real. The ROM's order
// is  head -> SetNodeID(2) -> func_0201a054 -> SetNodeID(3) -> loop, and the
// port runs func_0201a054's own two spans about 170 lines further down
// tests/walk_window.cpp, after the vtable fills and the cross-patches. So with
// this gate on, both SetNodeID calls happen (which is MORE faithful than today,
// where neither does) but the second one happens before func_0201a054's spans
// rather than after them. That is a heap-debugger name tag, not state the game
// reads, and it is the price of running main's shell before the boot's middle
// has been moved. Rung 2 of the staged plan is exactly the job of removing it.
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>

/* src/_ZN4Heap9SetNodeIDEj.cpp is a real C++ method (`//cpp`), so its object
   carries MSVC's mangling and nothing carries the flat Itanium name src/main.c
   calls. This is the two-name-space bridge hal/heap_vtable.cpp already writes
   for the allocator's own NodeID accessors, done once more for the base class.
   The declaration matches include/Heap.h:33 exactly (public, non-const,
   unsigned int), so the call mangles onto the same symbol the matched TU
   defines. */
struct Heap {
    void SetNodeID(unsigned int a_);
};

extern "C" {

void rom_main(void);              /* src/main.c, renamed at compile time */

static int g_a054_hits;
static int g_loop_hits;

/* main's fifth call. The ROM body is src/func_0201a054.c and it is not run as
   ROM code by this rung: tests/walk_window.cpp still runs its two transcribed
   spans at their own point. */
void port_rom_a054_seam(void)
{
    ++g_a054_hits;
    std::fprintf(stderr, "[rom-main] reached main's func_0201a054 call "
                         "(host seam; the ROM body is not run by this rung)\n");
}

/* main's last call, src/func_020197b8.c: the ROM's own game loop, a
   do { ... } while (1) that never returns. Returning here is the staged
   path's hand-back -- main runs to its loop and the host frame loop takes the
   frame from there. */
void port_rom_loop_seam(void)
{
    ++g_loop_hits;
    std::fprintf(stderr, "[rom-main] reached main's func_020197b8 call "
                         "(the ROM's game loop); handing the frame back to the "
                         "host loop\n");
}

/* The flat Itanium name src/main.c calls, onto the matched method. The ROM
   passes `this` in r0 and main.c's own prototype spells that as a leading
   unsigned int, so the two words arrive in the same order a cdecl call makes
   them. */
void _ZN4Heap9SetNodeIDEj(void *self, unsigned int id)
{
    if (!self) {
        std::fprintf(stderr, "[rom-main] Heap::SetNodeID(%u) on a null heap; "
                             "skipped\n", id);
        return;
    }
    ((Heap *)self)->SetNodeID(id);
}

int port_rom_main_enabled(void)
{
    const char *e = std::getenv("SM64DS_ROM_MAIN");
    return e && *e && *e != '0';
}

void port_rom_main_run(void)
{
    std::fprintf(stderr, "[rom-main] calling the ROM's own main (src/main.c, "
                         "arm9 0x02007000)\n");
    rom_main();
    std::fprintf(stderr, "[rom-main] main returned; func_0201a054 seam hit %d, "
                         "func_020197b8 seam hit %d\n", g_a054_hits, g_loop_hits);
}

}  /* extern "C" */
