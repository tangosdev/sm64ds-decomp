// ---------------------------------------------------------------------------
// boot_arms.cpp -- THE REFUSED ARMS OF func_0201a054, RUN FROM THE ROM'S OWN
// SEAM.
//
// Run link100, lane R2ABC, rungs R2a / R2b / R2c of lane BOOTSCOUT's staged
// boot plan. hal/rom_main.cpp made src/main.c real ROM code and left main's
// fifth call -- func_0201a054 -- as a host seam that only counted its hits.
// This file is what that seam calls. Each arm below is a call the ROM's own
// func_0201a054 body makes, in the ROM's own order, and each one arrives here
// with the reason it used to be refused and the measurement that retired it.
//
// THE ORDER IS THE ROM'S, not a convenience. src/func_0201a054.c's head runs
//
//     func_020427f8(); func_02042784(); func_02053a8c(); func_02053c40();
//     func_0205b858(); func_0203d740(); func_02013e64(); func_0201a4e4();
//     IRQ::EnableIRQs(1); IME = 1; IRQ::Enable();
//     func_02053be0(1); func_0203bbc0(); func_0203bb5c();
//     if (!r4) func_0201fec8();
//     func_02042f68(0xd01, data_0208ee50);
//     ...
//
// so func_02013e64 comes first, func_0201fec8 next and func_02042f68 last.
// hal/boot_os.cpp's port_boot_rom_game_init_head() still runs the ARMS AROUND
// THEM at its own point in tests/walk_window.cpp -- this file does not move
// that transcription and does not duplicate any call it makes. What it adds is
// the three the transcription refused.
//
// WHERE THIS RUNS, AND WHY THAT IS THE SAFE POINT. port_rom_a054_seam() is
// reached from tests/walk_window.cpp:7429, immediately after
// Heap::InitializeRootHeap and well after port_romdata_load() at :7365. The
// host's own boot staging -- the pending character at :7743, the opening-seen
// bit at :8134, the level boot itself -- is all BELOW it, so an arm that
// writes DS state here writes it before anything host-side has staged, which
// is exactly the ROM's own order.
//
// ===========================================================================
// R2a -- func_02013e64, and the refusal that had already been retired
// ===========================================================================
//
// The body is one line:
//
//     func_0205a588(data_0209caa0, 0, 0x32c);      memset(save block, 0, 796)
//
// hal/boot_os.cpp and port/slice_gate222.txt both refuse it in the same words:
// "hal/level_boot.cpp hosts data_0209caa0 as a 0x14 object -- the dsd symbol's
// own span -- while the ROM object is the full 0x32c ... Seating the body today
// writes 796 bytes into a 20-byte object. It wants one grouped run in
// hal/level_boot.cpp."
//
// THAT RUN ALREADY EXISTS AND THE REFUSAL IS STALE. hal/level_boot.cpp's
// SAVEBLK block is five grouped sections, not four, and run link60 Stage 5
// lane SV1 added the fifth for a different write (SaveData::SetDefaultValuesMg
// clearing 0x2e4 bytes through data_0209cae4). Its own comment says so:
// "config/arm9/symbols.txt runs caa0, cab4, cad2, cae4, caf4, then
// data_0209cdcc, so caf4's ROM span is 728 bytes and the whole save object is
// one 0x32c run of five symbols."
//
//     .dsstate$savblk0000  data_0209caa0  0x14   +0x000
//     .dsstate$savblk0001  data_0209cab4  0x1e   +0x014
//     .dsstate$savblk0002  data_0209cad2  0x12   +0x032
//     .dsstate$savblk0003  data_0209cae4  0x10   +0x044
//     .dsstate$savblk0004  data_0209caf4  728    +0x054 .. +0x32c
//
// MEASURED IN THIS TREE'S OWN MAP before the seat, not reasoned about:
//   _data_0209caa0 0004:0010667c   _data_0209cab4 0004:00106690  (+0x14)
//   _data_0209cad2 0004:001066ae   (+0x1e)   _data_0209cae4 0004:001066c0 (+0x12)
//   _data_0209caf4 0004:001066d0   (+0x10)   end +0x32c = 0004:001069a8
//   _dsstate_hi    0004:00107b88             -- the whole run is captured.
//
// So the storage half of the refusal was retired by another run for another
// reason, and nothing had gone back to the arm that was waiting on it. What
// this file adds is the part that was missing either way: A CHECK. The five
// offsets are asserted before the memset runs, because a grouped-section run
// is a LAYOUT the linker could reorder and a 796-byte memset onto a reordered
// run is silent corruption, not a crash. If the layout is ever wrong the arm
// REFUSES and says which name moved; it does not memset anyway.
//
// AND THE WRITE ITSELF IS A NO-OP ON THIS PATH, which is worth saying so that
// nobody reads a green battery as proof of more than it is. All five sections
// are zero-initialised host storage and every host writer of the block is
// below this seam, so the ROM's memset writes zeros over zeros here. What the
// seat buys is that the ROM's own body is the thing that does it, on the ROM's
// own object, at the ROM's own point -- and that the object is now proved to
// be the ROM's shape on every boot instead of assumed to be.
// ---------------------------------------------------------------------------

#include <cstdio>

extern "C" {

// ---- R2a: the save block, hosted as one 0x32c run by hal/level_boot.cpp ----
// Declared as arrays rather than pointers so the offsets below are the
// linker's own addresses. hal/level_boot.cpp owns the storage; this file only
// checks it and hands the head to the ROM's body.
extern unsigned char data_0209caa0[];
extern unsigned char data_0209cab4[];
extern unsigned char data_0209cad2[];
extern unsigned char data_0209cae4[];
extern unsigned char data_0209caf4[];

// src/func_02013e64.c -- the ROM's own clear of the save block.
void func_02013e64(void);

// The layout gate. Returns 0 when the five names are the ROM's own run.
int port_r2a_saveblock_layout(void)
{
    static const struct { const unsigned char *p; long want; const char *n; } k[] = {
        { data_0209caa0, 0x000, "data_0209caa0" },
        { data_0209cab4, 0x014, "data_0209cab4" },
        { data_0209cad2, 0x032, "data_0209cad2" },
        { data_0209cae4, 0x044, "data_0209cae4" },
        { data_0209caf4, 0x054, "data_0209caf4" },
    };
    int bad = 0;
    for (int i = 0; i < 5; ++i) {
        long at = (long)(k[i].p - data_0209caa0);
        if (at != k[i].want) {
            std::fprintf(stderr, "  [r2a] SAVE BLOCK RUN BROKEN: %s at +0x%lx, "
                                 "ROM says +0x%lx\n", k[i].n, at, k[i].want);
            bad = 1;
        }
    }
    return bad;
}

// ---- the seam body hal/rom_main.cpp calls ---------------------------------
//
// One entry point, the arms inside it in the ROM's order. hal/rom_main.cpp
// keeps its own [rom-main] line; the per-arm lines below are what a captured
// run shows firing.
void port_rom_a054_arms(void)
{
    /* ARM R2a. func_02013e64() -- memset(data_0209caa0, 0, 0x32c). */
    if (port_r2a_saveblock_layout()) {
        std::fprintf(stderr, "  [rom-a054] R2a REFUSED: the save block is not "
                             "the ROM's 0x32c run, so its memset is not run\n");
    } else {
        func_02013e64();
        std::fprintf(stderr, "  [rom-a054] R2a func_02013e64: the ROM cleared "
                             "its own 0x32c save block at 0x0209caa0\n");
    }
}

}  // extern "C"
