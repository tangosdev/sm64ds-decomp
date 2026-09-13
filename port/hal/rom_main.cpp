// ---------------------------------------------------------------------------
// rom_main.cpp -- THE ROM'S OWN main() IS THE PORT'S BOOT SPINE.
//
// Run link100, lane BOOTSCOUT measured this and lane BOOTR1 lands it. The gate
// SM64DS_ROM_MAIN now DEFAULTS ON: unset, or any value other than "0", runs
// src/main.c; SM64DS_ROM_MAIN=0 keeps the host's transcribed head. The default
// moved because the ROM's main is the faithful boot and the measurement found
// nothing that says otherwise -- see MEASURED, below.
//
// WHAT THE PORT DID BEFORE THIS FILE. tests/walk_window.cpp defines main() for
// Windows, so src/main.c -- the ROM's own eighteen-line main, arm9 0x02007000,
// 0x5c bytes -- was an UNDOCUMENTED SHADOW in linkage.py's queue, and lane
// REMAIN's cover puts 141 further matched TUs behind it with no other way in.
// main's body was transcribed instead, span by span, in hal/boot_os.cpp:
// port_boot_rom_main_head() is main's first three calls and the
// func_0201a054 pair is main's fifth.
//
// THE ROM'S main, read out of extracted/arm9_dec.bin at 0x02007000 and
// reproduced by src/main.c line for line:
//
//     02007008  bl 0x2059788        func_02059788()
//     0200700c  bl 0x2059bc0        func_02059bc0()
//     02007018  bl 0x2059578        func_02059578(&func_0201a03c, &data_020a621c)
//     02007028  bl 0x203c408        Heap::SetNodeID(*data_020a0e9c, 2)
//     0200702c  bl 0x201a054        func_0201a054()
//     0200703c  bl 0x203c408        Heap::SetNodeID(*data_020a0e9c, 3)
//     02007040  bl 0x20197b8        func_020197b8()
//
// Both seam calls below are BL instructions in main's own compiled body. They
// are not keep-alive references and nothing takes their address: the ROM's
// main really makes both calls.
//
// AND IT IS CALLED WHERE THE ROM CALLS IT. Entry (arm9 0x02004800) runs
// func_02019780 at 0x020048a4, func_02072f94 at 0x020048a8, then loads main's
// address out of its own literal pool (0x020048d0 = 0x02007000), loads a
// pooled lr of 0xffff0000, and TAIL-JUMPS: `bx r1`. tests/walk_window.cpp
// calls port_rom_main_run() at exactly that point -- immediately after
// port_boot_rom_pre_main() and Heap::InitializeRootHeap(), which are
// func_02019780's own body and its fourth call.
//
// WHAT THIS ADDS TO THE LINK. src/main.c is compiled with a per-source rename,
// the same mechanism port/CMakeLists.txt already uses for func_0203bb60_narrow:
//
//     main             -> rom_main               so the CRT's main still wins
//     func_0201a054    -> port_rom_a054_seam     the two calls this rung does
//     func_020197b8    -> port_rom_loop_seam     not yet run as ROM code
//
// The rename is deliberate and it is the honest form of this rung. Defining
// func_0201a054 and func_020197b8 as host bodies would have created two new
// SHADOW rows in the replacement queue -- work invented by the change -- while
// the rename leaves both ROM TUs exactly where they were, unenrolled and
// unclaimed (neither appears on any slice list), and says in one place that
// main's last two edges are not ROM code yet. What DOES become ROM code is
// main itself and Heap::SetNodeID, which the ROM calls twice from main and the
// port had never run.
//
// MEASURED, gate ON against gate OFF, on this tree:
//   linked into walk_window   10145 -> 10147 (+2: main, _ZN4Heap9SetNodeIDEj)
//   SHADOWS in linkage.py     15 -> 14 (main left the queue; no new row)
//   the six proofs, the full battery and a captured 20-frame run: see lane
//   BOOTR1's report. The gate is OBSERVED firing, not assumed: a proof that is
//   green with an environment variable that never reached the child proves
//   nothing, so the [rom-main] lines below are the evidence.
//
// THE ONE KNOWN DEVIATION, stated because it is real. The ROM's order is
//     head -> SetNodeID(2) -> func_0201a054 -> SetNodeID(3) -> loop
// and the port runs func_0201a054's own two transcribed spans
// (port_boot_rom_game_init_head / Heap::InitializeGameHeap / ..._tail) about
// 170 lines further down tests/walk_window.cpp, after the vtable fills, the
// ov002 pointer pass and its static initialisers. So with this gate on, both
// SetNodeID calls happen -- which is MORE faithful than before, where neither
// did -- but the second happens before func_0201a054's spans rather than
// after them. It is a heap-debugger name tag: Heap::SetNodeID is one virtual
// call through the heap's slot 0x34 that stamps an id into the allocator, and
// nothing in either span reads it.
//
// LANE BOOTR1 LOOKED AT MOVING THOSE SPANS INSIDE THE SEAM AND REFUSED, with
// reasons, because it is a rewrite of the port's boot and not a move:
//   * tests/walk_window.cpp:7430 seeds data_0209b3ec by hand right after this
//     seam, and hal/boot_os.cpp's port_boot_rom_game_init_tail banner says in
//     as many words that the ROM's own writer of those same words "runs later,
//     so the ROM's value is the one that stands". Run the tail inside the seam
//     and that order INVERTS: the host's hand seed would overwrite the ROM's
//     matrix. The tree predicts the regression itself.
//   * the game-heap carve at walk_window.cpp:7600 is followed by
//     `if (!data_020a0eac_c) return 2;` -- a return out of main. A void seam
//     called from ROM main cannot make that return; turning it into an exit()
//     or a flag rewrites the boot's failure contract.
//   * every SM64DS_SCENE run RETURNS at walk_window.cpp:7491, above the carve.
//     Moving the carve into the seam gives all 36 of the battery's scene rows
//     a game heap they have never had. That is a behaviour change across the
//     whole battery, not a reordering.
//   * between the seam and the spans sit the four vtable fills, port_ov002_patch,
//     port_cross_patch and 26 ov002 static initialisers. Hoisting the a054
//     spans over all of them is exactly rung R2f of the staged plan ("the real
//     func_0201a054 body"), which the plan already says needs all of R2 first.
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

/* main's fifth call. The ROM body is src/func_0201a054.c and it is still not
   run WHOLE as ROM code: tests/walk_window.cpp runs its two transcribed spans
   at their own point, for the four reasons in the header block, and rung R2f
   of lane BOOTSCOUT's staged plan is where the real body goes.

   WHAT THE SEAM DOES RUN, from run link100 lane R2ABC, is the arms of that
   body hal/boot_os.cpp's transcription REFUSED. port_rom_a054_arms() lives in
   hal/boot_arms.cpp and calls them in the ROM's own order, each behind the
   storage check its own refusal asked for. Nothing the transcription already
   runs is repeated there: the seam and the transcription are disjoint by
   construction, and boot_arms.cpp names which call belongs to which. */
void port_rom_a054_arms(void);

void port_rom_a054_seam(void)
{
    ++g_a054_hits;
    std::fprintf(stderr, "[rom-main] reached main's func_0201a054 call "
                         "(host seam; the ROM body is not run whole by this "
                         "rung -- its refused arms are)\n");
    port_rom_a054_arms();
}

/* main's last call, src/func_020197b8.c: the ROM's own game loop, a
   do { ... } while (1) that never returns. Returning here is the staged
   path's hand-back -- main runs to its loop and the host frame loop takes the
   frame from there. Rung R3d of the staged plan is where this seam goes. */
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

/* THE GATE, DEFAULT ON (run link100, lane BOOTR1). Unset or empty runs the
   ROM's own main, and so does any value other than "0"; SM64DS_ROM_MAIN=0 is
   the one spelling that keeps the host's transcribed head. The escape hatch
   stays because the transcription is still in hal/boot_os.cpp and a lane that
   wants to A/B the boot should not have to rebuild to do it. */
int port_rom_main_enabled(void)
{
    const char *e = std::getenv("SM64DS_ROM_MAIN");
    if (!e || !*e)
        return 1;
    return !(e[0] == '0' && e[1] == '\0');
}

/* ENTRY'S OWN STATIC-INITIALISER CALL, run link100 lane CTOR rung C1a. The
   ROM's Entry runs func_02019780 at 0x020048a4, func_02072f94 at 0x020048a8
   and then tail-jumps to main -- the three instructions the header block above
   quotes. tests/walk_window.cpp already runs func_02019780's body and its
   fourth call immediately before it calls this function, so the head of this
   function IS 0x020048a8 and nothing had to move to make room. The table that
   call walks, the bindings of its 23 words and the two once-only counters are
   in hal/ctor_runner.cpp.

   IT IS INSIDE THE SM64DS_ROM_MAIN GATE, and Entry's call is not main's, which
   this rung states rather than hides. The gate DEFAULTS ON and nothing in
   port/tools sets it to 0 -- it is a hand A/B hatch for comparing the ROM's
   main against the host transcription -- so every battery, proof and captured
   run takes this path. SM64DS_ROM_MAIN=0 now also skips the .ctor walk. The
   call belongs one line above the `if` in tests/walk_window.cpp, and that file
   belongs to another lane tonight. */
void port_rom_entry_ctors(void);

void port_rom_main_run(void)
{
    port_rom_entry_ctors();
    std::fprintf(stderr, "[rom-main] calling the ROM's own main (src/main.c, "
                         "arm9 0x02007000)\n");
    rom_main();
    std::fprintf(stderr, "[rom-main] main returned; func_0201a054 seam hit %d, "
                         "func_020197b8 seam hit %d\n", g_a054_hits, g_loop_hits);
}

}  /* extern "C" */
