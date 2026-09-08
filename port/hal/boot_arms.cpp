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
//
// ===========================================================================
// R2b -- func_0201fec8: REFUSED, and the refusal is bigger than the one on
//        record. Nothing is seated for it and nothing below runs it.
// ===========================================================================
//
// The whole measurement is in port/slice_r2abc.txt beside the two other arms.
// The short form, because a reader of this file should not have to go looking:
// FIVE of the eight names func_0203db64 memsets are already grouped correctly
// (hal/comms_conductor.cpp's bands B and C, hal/camera_bridges.cpp's comms
// blocks), TWO of the three that are not can be standalone objects at their
// ROM spans, and the LAST one cannot be written from here at all --
// data_020a0fa0's memset runs 0x18 bytes over its own 6 and into
// data_020a0fa6, which is another file's band head. On top of that the body's
// six .rodata arguments are hosted nowhere and five of them carry four
// RELOCATED words each, so hosting them raw would put DS addresses in host
// storage, which hal/boot_globals.cpp refuses by name. Both fixes are outside
// this lane's files.
//
// ===========================================================================
// R2c -- func_02042f68, the card driver's bring-up and the device identify
// ===========================================================================
//
// The ROM body, verified instruction for instruction against
// extracted/arm9_dec.bin at 0x02042f68 (flat, load base 0x02004000):
//
//     02042f78  bl 0x0203da3c        if (func_0203da3c() == 2) return;
//     02042f8c  bl 0x02060890        arm 1: the card driver's bring-up
//     02042f94  bl 0x020603c8        arm 2: the device identify, r5 = 0xd01
//     02042fa4  bl 0x0205a588        arm 3: memset(data_020a4b40, 0, 8)
//     02042fa8..                     then eight bytes of the tag, stop at NUL
//
// ARM 3 IS NOT A CALL AND CAME FOR FREE. It is inline in func_02042f68 itself:
// clear the eight-byte record tag and copy data_0208ee50 into it, stopping at
// the first zero. hal/boot_globals.cpp hosts data_0208ee50 with the ROM's own
// twelve bytes (64 73 20 6d 61 72 69 6f 00 00 00 00 -- "ds mario"), so the ROM
// writes exactly the eight bytes ntr/backup.cpp had to define statically, and
// the definition there becomes the pre-boot default rather than the only
// writer. There are no zeros in the eight, so all eight land.
//
// ARM 2 RUNS AS ROM CODE, and what unblocked it was already in the tree.
// ntr/backup.cpp's ROMDATA PROPOSAL says these six TUs need "arm9 .rodata
// 0x020867bc (0xa0 bytes, the device table) and 0x0208ee50 (8 bytes, the tag)
// ... mounted through port/tools/romdata.py", and refuses to do it because
// ROM-CLEAN would emit a ZEROED device row: "size 0 makes func_0206045c return
// 0 ... a kit that cannot save at all, failing silently".
//
//   NEITHER SPAN NEEDS MOUNTING. hal/boot_globals.cpp already hosts BOTH, with
//   their ROM bytes, as ordinary .data globals -- data_020867bc[160] and
//   data_0208ee50[12], both in this build's map from boot_globals.cpp.obj. The
//   proposal was written against a state of the tree that lane BOOT had already
//   changed.
//   AND THE ROM-CLEAN OBJECTION HAS ITS OWN ANSWER EITHER WAY, which is the
//   second of the two the proposal itself offers ("a boot-order guarantee that
//   the blob is loaded before the first save"). port_romdata_load() runs at
//   tests/walk_window.cpp:7365 and this seam is reached at :7429, so even for a
//   span that DID come from romdata.py the identify would read filled bytes.
//
//   WHAT THE IDENTIFY THEN DOES, and it agrees with the static answer byte for
//   byte. func_02060398 walks data_020867bc for the row whose first word is
//   0xd01 and finds it at +0x28: 0x00000d01, 0x00002000, 0x00002000,
//   0x00020020, 0x0000004f -- the same five words ntr/backup.cpp typed into
//   data_port_backup_device by hand. func_02060364 parks that row in
//   data_020a8760 and copies its +0x0e halfword (2) to data_020a8764;
//   func_020607e8 puts the type word in the shared command block at +0x04 and
//   sends command 2, which ntr/backup.cpp's ARM7 face answers yes to because
//   the word matches its own row; func_0206081c clears the busy bits and wakes
//   the (empty) queue. So the chip size func_0206045c returns is still 8192 and
//   save_proof is measuring the same medium it always was -- only the ROM is
//   the one that decided it now.
//
// ARM 1 IS A HOST BODY AND STAYS ONE, because two of its statements cannot
// link. src/func_02060890.c, verified at 0x02060890:
//
//     data_020a8780 = func_02060a64   the FLASH read path. ntr/backup.cpp: "no
//                                     call site in this game reaches it", and
//                                     data_020a8780 is hosted nowhere (ROM
//                                     span 0x220).
//     func_02061138()                 IRQ::SetIRQHandler(0x100000,
//                                     IRQ::GameCardIREQMCHandler) reaches
//                                     func_020610fc, HAND-ASM with a
//                                     deliberate `b self` that MSVC cannot
//                                     assemble -- family A, permanently.
//                                     ntr/runtime.cpp's SetIRQHandler drops
//                                     mask 0x100000 anyway.
//
// Six matched TUs sit behind those two lines (func_02060890, func_02060a64,
// func_02060d98, func_02060ebc, func_02061138 and IRQ::GameCardIREQMCHandler)
// and all six stay out.
//
// THE FORM IS A RENAME, NOT A SHADOW, and it is hal/rom_main.cpp's own
// precedent. port/CMakeLists.txt compiles src/func_02042f68.c with
// `func_02060890=port_rom_card_bringup`, so the matched TU stays in the link
// byte for byte, src/func_02060890.c stays unenrolled and unclaimed rather
// than becoming a new SHADOW row in linkage.py's replacement queue, and one
// place in the tree says this edge is not ROM code yet. The body below is that
// place: it is func_02060890's ROM body with its two unlinkable statements
// removed and nothing else changed, INCLUDING the ROM's own
// `if (state != 0) return` guard at the top.
//
// AND THAT GUARD IS WHY THIS DOES NOT DOUBLE-CREATE THE CARD THREAD. On this
// host the guard TRIPS: ntr/backup.cpp's PortBackupFill sets the work object's
// state word to 1 at static-init time, long before this seam, so the bring-up
// finds the driver already initialised and returns without calling
// func_0206002c. hal/boot_os.cpp's own direct call to func_0206002c at its
// own point in port_boot_rom_game_init_head stays the only one, and
// port/tools/thread_create_proof.py measures exactly the thread it always did.
// A faithful transcription and no behaviour change is the same fact here.
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstring>
#include <stdint.h>

#include "dsstate_seg.h"

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
        uintptr_t at = (uintptr_t)k[i].p - (uintptr_t)data_0209caa0;
        if (at != (uintptr_t)k[i].want) {
            std::fprintf(stderr, "  [r2a] SAVE BLOCK RUN BROKEN: %s at +0x%lx, "
                                 "ROM says +0x%lx\n", k[i].n, (unsigned long)at, k[i].want);
            bad = 1;
        }
    }
    return bad;
}

// ---- R2c: the card driver's storage the identify needs --------------------
//
// data_020a8764 is the one word src/func_02060364.c writes that nothing in
// this port hosted. hal/globals_link100.cpp measured its boundary and left it
// -- "data_020a8764 is the neighbour above the top ... nothing spans the
// boundary and data_020a8764 stays whatever hosts it" -- and nothing did. It
// is hosted here at its ROM SPAN, 0x020a8764 to data_020a8780 = 28 bytes, not
// at the four bytes func_02060364's own `unsigned int` declaration writes:
// that width is the undersized-hosted-global bug this port keeps finding, and
// the span is what config/arm9/symbols.txt says the object is. It is mutable
// DS state, so it goes in the .dsstate capture the save state rolls back.
DSSTATE_BEGIN
unsigned char data_020a8764[28];
DSSTATE_END

// The card driver's work object and the device table, both hosted elsewhere:
// hal/globals_link100.cpp's grouped card span and hal/boot_globals.cpp's
// arm9 .data bytes. Read here only to report what the identify decided.
extern unsigned char data_020a8180[];
extern unsigned char data_020a8760[];
extern unsigned char data_020867bc[];
extern unsigned char data_020a4b40[];
extern unsigned char data_0208ee50[];

void func_0206002c(void);          // src/func_0206002c.c, the card thread
void func_02042f68(int a0, unsigned char *src);   // the ROM's own arm

// ---- ARM 1 of func_02042f68, as a host body ------------------------------
//
// src/func_02060890.c minus its last two statements, which cannot link. The
// per-source rename in port/CMakeLists.txt points the ROM's own call here.
// Every store below is the ROM's, at the ROM's offset, in the ROM's order
// (0x02060890: read +0x34, return if non-zero, then +0x34 = 1, +0x20 = 0,
// +0x1c = +0x20, +0x18 = +0x1c, +0x24 = -1, +0x28 = 0, +0x2c = 0, then
// bl func_0206002c).
void port_rom_card_bringup(void)
{
    char *g = (char *)data_020a8180;
    if (*(int *)(g + 0x34) != 0) {
        /* The port reaches this line, every boot: ntr/backup.cpp seeds the
           work object statically at start-up, so the ROM's own guard says the
           driver is already up. Said once rather than silently. */
        std::fprintf(stderr, "  [rom-a054] R2c arm 1 (func_02060890): the card "
                             "driver is already initialised (state %d), so the "
                             "ROM's own guard returns -- func_0206002c stays "
                             "hal/boot_os.cpp's single call\n",
                     *(int *)(g + 0x34));
        return;
    }
    *(int *)(g + 0x34) = 1;
    *(int *)(g + 0x20) = 0;
    *(int *)(g + 0x1c) = *(int *)(g + 0x20);
    *(int *)(g + 0x18) = *(int *)(g + 0x1c);
    *(int *)(g + 0x24) = -1;
    *(int *)(g + 0x28) = 0;
    *(int *)(g + 0x2c) = 0;
    func_0206002c();
    /* data_020a8780 = func_02060a64  -- NOT RUN: the FLASH read path, hosted
       nowhere and reached by nothing in this game.
       func_02061138()               -- NOT RUN: hand-asm `b self`, family A. */
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

    /* ARM R2b. func_0201fec8() -- NOT RUN. See the header block and
       port/slice_r2abc.txt: the storage half is two files away and the body's
       own .rodata arguments are a fourteen-symbol relocated island nothing
       hosts. Nothing is faked in its place; the port simply does not make this
       call, exactly as hal/boot_os.cpp's transcription does not. */

    /* ARM R2c. func_02042f68(0xd01, data_0208ee50) -- the card bring-up, the
       device identify and the record tag, all three as the ROM's own body. */
    {
        unsigned char *row_before = 0, *row_after = 0;
        std::memcpy(&row_before, data_020a8760, sizeof row_before);
        func_02042f68(0xd01, data_0208ee50);
        std::memcpy(&row_after, data_020a8760, sizeof row_after);
        if (!row_after) {
            std::fprintf(stderr, "  [rom-a054] R2c func_02042f68: the device "
                                 "pointer is null after the call\n");
        } else {
            uintptr_t row = (uintptr_t)row_after;
            uintptr_t table = (uintptr_t)data_020867bc;
            std::fprintf(stderr, "  [rom-a054] R2c func_02042f68: device row ");
            if (row >= table && row - table < 0xa0)
                std::fprintf(stderr, "+0x%x of data_020867bc", (unsigned)(row - table));
            else
                std::fprintf(stderr, "%p outside data_020867bc", (void *)row_after);
            std::fprintf(stderr,
                         ", type 0x%x, size %d bytes, kind %d; tag \"%.8s\"\n",
                         *(unsigned *)row_after,
                         *(int *)(row_after + 4),
                         (int)*(unsigned short *)data_020a8764,
                         (const char *)data_020a4b40);
        }
        if (row_before == row_after)
            std::fprintf(stderr, "  [rom-a054] R2c NOTE: the device pointer "
                                 "is unchanged\n");
    }
}

}  // extern "C"
