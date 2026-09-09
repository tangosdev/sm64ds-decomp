// ---------------------------------------------------------------------------
// boot_arms.cpp -- THE REFUSED ARMS OF func_0201a054, RUN FROM THE ROM'S OWN
// SEAM.
//
// Run link100, lane R2ABC, rungs R2a / R2b / R2c of lane BOOTSCOUT's staged
// boot plan. hal/rom_main.cpp made src/main.c real ROM code and left main's
// fifth call -- func_0201a054 -- as a host seam that only counted its hits.
// This file is what that seam calls: the arms of that body which
// hal/boot_os.cpp's transcription refused, in the ROM's own order.
//
// THE DERIVATION IS port/slice_r2abc.txt, not this header. Which refusal was
// stale and which still holds, the ROM's own bytes for each arm, the device
// table the identify walks, and the two blockers that keep R2b out are all
// measured there. What the comments below cover is what a reader of the CODE
// needs: the boot order, the storage checks, and the one edge that is still a
// host bridge.
//
// WHAT THE SEVEN NEWLY LINKED TUs ARE: LINKAGE COVERAGE. src/func_02013e64.c
// on R2a, and the six of R2c (func_02042f68, func_020603c8, func_020607e8,
// func_02060364, func_02060398, func_02060470), are the ROM's own code running
// on this path where host code used to stand in. That is what the count
// measures, and it is not seven removals of host behaviour. R2a's memset
// writes zeros over zeros here: all five sections are zero-initialised host
// storage and every host writer of the save block is below this seam. The
// identify reaches the same device row ntr/backup.cpp already parks statically,
// so func_0206045c returns the same 8192 either way and that file's row stays
// as the pre-boot default. Arm 1 of R2c is still a host body. What the seat
// changes is WHO decides, on the ROM's own object, at the ROM's own point.
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
// THE STORAGE CHECKS
// ===========================================================================
//
// R2a. The body is one line -- func_0205a588(data_0209caa0, 0, 0x32c), a
// memset of the 796-byte save block -- and hal/level_boot.cpp hosts that block
// as ONE grouped run of five .dsstate$savblk sections:
//
//     .dsstate$savblk0000  data_0209caa0  0x14   +0x000
//     .dsstate$savblk0001  data_0209cab4  0x1e   +0x014
//     .dsstate$savblk0002  data_0209cad2  0x12   +0x032
//     .dsstate$savblk0003  data_0209cae4  0x10   +0x044
//     .dsstate$savblk0004  data_0209caf4  728    +0x054 .. +0x32c
//
// A grouped-section run is a LAYOUT the linker could reorder, and a 796-byte
// memset onto a reordered run is silent corruption rather than a fault. So
// port_r2a_saveblock_layout() asserts the five offsets on EVERY boot, and when
// one has moved the arm REFUSES and names it rather than memsetting anyway.
//
// R2c. One word of the card driver's storage was hosted nowhere:
// data_020a8764, the halfword src/func_02060364.c copies out of the device
// row. hal/globals_link100.cpp measured its boundary and left it. It is hosted
// below at its ROM SPAN -- 0x020a8764 to data_020a8780, 28 bytes -- not at the
// four bytes func_02060364's own `unsigned int` declaration writes: that width
// is the undersized-hosted-global bug this port keeps finding, and the span is
// what config/arm9/symbols.txt says the object is. It is mutable DS state, so
// it goes in the .dsstate capture the save state rolls back.
//
// ===========================================================================
// THE ONE EDGE THAT IS STILL A HOST BRIDGE
// ===========================================================================
//
// Arm 1 of func_02042f68 is func_02060890, the card driver's bring-up, and it
// stays a host body because two of its statements cannot link:
//
//     data_020a8780 = func_02060a64   the FLASH read path, hosted nowhere and
//                                     reached by nothing in this game.
//     func_02061138()                 reaches func_020610fc, hand-asm with a
//                                     deliberate `b self` MSVC cannot
//                                     assemble -- family A, permanently.
//
// Six matched TUs stay out behind those two lines and are named in
// port/slice_r2abc.txt.
//
// THE FORM IS A RENAME, NOT A SHADOW, and it is hal/rom_main.cpp's own
// precedent. port/CMakeLists.txt compiles src/func_02042f68.c with
// `func_02060890=port_rom_card_bringup`, so the matched TU stays in the link
// byte for byte, src/func_02060890.c stays unenrolled and unclaimed rather
// than becoming a new SHADOW row in linkage.py's replacement queue, and one
// place in the tree says this edge is not ROM code yet. port_rom_card_bringup
// below is that place: func_02060890's ROM body with its two unlinkable
// statements removed and nothing else changed, INCLUDING the ROM's own
// `if (state != 0) return` guard.
//
// AND THAT GUARD IS WHY THIS DOES NOT DOUBLE-CREATE THE CARD THREAD. On this
// host the guard TRIPS: ntr/backup.cpp's PortBackupFill sets the work object's
// state word to 1 at static-init time, long before this seam, so the bring-up
// finds the driver already initialised and returns without calling
// func_0206002c. hal/boot_os.cpp's own direct call to func_0206002c at its
// own point in port_boot_rom_game_init_head stays the only one, and
// port/tools/thread_create_proof.py measures exactly the thread it always did.
//
// R2b -- func_0201fec8 -- IS REFUSED, AND NOTHING BELOW RUNS IT. Nothing is
// faked in its place either; the port simply does not make that call, exactly
// as hal/boot_os.cpp's transcription does not. The two blockers (one adjacency
// that belongs in hal/comms_conductor.cpp, and a fourteen-symbol arm9 .rodata
// island behind the body's own arguments) are measured in
// port/slice_r2abc.txt.
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstring>

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
//
// THE OFFSETS ARE COMPUTED FROM INTEGER ADDRESSES, not by subtracting the
// pointers. The five names are separately declared globals that the linker
// places adjacently because their grouped sections sort that way; adjoining
// sections do not make them one C++ array, and `k[i].p - data_0209caa0` would
// be pointer subtraction across distinct array objects. Each address is
// reduced to an address-sized integer first, in the port's own spelling --
// (unsigned)(size_t), the cast hal/actor_registry.cpp and the actor-table
// files already use on a host pointer, lossless here because the port builds
// 32-bit (port/build-port.cmd calls vcvars32). The refusal path is unchanged:
// a run that is not the ROM's shape makes the arm refuse rather than memset,
// and the message still names the symbol and both offsets.
int port_r2a_saveblock_layout(void)
{
    static const struct { const unsigned char *p; unsigned want; const char *n; } k[] = {
        { data_0209caa0, 0x000, "data_0209caa0" },
        { data_0209cab4, 0x014, "data_0209cab4" },
        { data_0209cad2, 0x032, "data_0209cad2" },
        { data_0209cae4, 0x044, "data_0209cae4" },
        { data_0209caf4, 0x054, "data_0209caf4" },
    };
    const unsigned head = (unsigned)(size_t)data_0209caa0;
    int bad = 0;
    for (int i = 0; i < 5; ++i) {
        unsigned at = (unsigned)(size_t)k[i].p - head;
        if (at != k[i].want) {
            std::fprintf(stderr, "  [r2a] SAVE BLOCK RUN BROKEN: %s at +0x%x, "
                                 "ROM says +0x%x\n", k[i].n, at, k[i].want);
            bad = 1;
        }
    }
    return bad;
}

// ---- R2c: the card driver's storage the identify needs --------------------
//
// data_020a8764 is the one word src/func_02060364.c writes that nothing in
// this port hosted. The storage-check block above says why it is 28 bytes and
// not the four its own declaration writes.
DSSTATE_BEGIN
unsigned char data_020a8764[28];
DSSTATE_END

// The card driver's work object and the device table, both hosted elsewhere:
// hal/globals_link100.cpp's grouped card span and hal/boot_globals.cpp's
// arm9 .data bytes. Read here only to report what the retained pointer
// names after the call, never to say which lookup ran.
// data_020867bc carries its extent because the diagnostic below has to test
// whether the retained row is inside the ROM's table at all; 160 bytes is the
// span hal/boot_globals.cpp defines and config/arm9/symbols.txt states.
extern unsigned char data_020a8180[];
extern unsigned char data_020a8760[];
extern unsigned char data_020867bc[160];
extern unsigned char data_020a4b40[];
extern unsigned char data_0208ee50[];

// ntr/backup.cpp's SEPARATELY HOSTED default row: five words typed by hand,
// the same five the ROM's table carries at +0x28. Its static init parks a
// pointer to it in data_020a8760 before this seam runs, so that is the object
// the retained pointer names until the ROM's own identify replaces it. Named
// here only so the diagnostic below can say which object it is looking at.
extern int data_port_backup_device[10];

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

        /* WHICH OBJECT DOES THE RETAINED POINTER NAME? An offset into
           data_020867bc only means something if the row is inside
           data_020867bc, and it need not be: ntr/backup.cpp's
           data_port_backup_device is a separately hosted row of the same five
           words, parked in data_020a8760 at static-init time. So the span is
           tested first, on integer addresses in the port's own width, and the
           line names the object it found rather than printing a difference
           between two unrelated ones.

           AND IT NAMES THE OBJECT ONLY, NEVER THE LOOKUP. Every branch below
           reports what data_020a8760 holds after the call and stops there,
           because no reading of that pointer tells you which lookup ran.
           src/func_02042f68.c:11-12 returns before everything else when
           func_0203da3c() reads 2, so on that path the card bring-up does not
           run, func_020603c8 is never called, the device search in
           src/func_02060398.c never walks data_020867bc and the tag loop never
           writes data_020a4b40: the pointer simply keeps whatever static init
           left in it. A null reading is therefore consistent with the early
           return and with a search that found no 0xd01, and a table row is
           consistent with a search that ran now and with a row an earlier call
           left behind. The same rule the NOTE line below is written under. */
        const unsigned table = (unsigned)(size_t)data_020867bc;
        const unsigned row   = (unsigned)(size_t)row_after;
        if (!row_after) {
            std::fprintf(stderr, "  [rom-a054] R2c func_02042f68: after the "
                                 "call data_020a8760 holds NO DEVICE ROW "
                                 "(null)\n");
        } else if (row - table < (unsigned)sizeof data_020867bc) {
            std::fprintf(stderr,
                         "  [rom-a054] R2c func_02042f68: after the call "
                         "data_020a8760 holds row +0x%x of data_020867bc -- "
                         "type 0x%x, size %d bytes, kind %d; tag \"%.8s\"\n",
                         row - table,
                         *(unsigned *)row_after,
                         *(int *)(row_after + 4),
                         (int)*(unsigned short *)data_020a8764,
                         (const char *)data_020a4b40);
        } else if (row == (unsigned)(size_t)data_port_backup_device) {
            std::fprintf(stderr,
                         "  [rom-a054] R2c func_02042f68: after the call "
                         "data_020a8760 holds ntr/backup.cpp's separately "
                         "hosted default row (data_port_backup_device), not a "
                         "row of data_020867bc -- type 0x%x, size %d bytes; "
                         "data_020a8764 reads %d; tag \"%.8s\"\n",
                         *(unsigned *)row_after,
                         *(int *)(row_after + 4),
                         (int)*(unsigned short *)data_020a8764,
                         (const char *)data_020a4b40);
        } else {
            std::fprintf(stderr,
                         "  [rom-a054] R2c func_02042f68: after the call "
                         "data_020a8760 holds %08x, which is neither inside "
                         "data_020867bc nor the hosted default row\n", row);
        }

        /* OBSERVED, AND NO MORE THAN OBSERVED. An unchanged pointer does not
           prove the identify never reached src/func_02060364.c: that body
           writes data_020a8760 and data_020a8764 and nothing else, so
           re-selecting the same row writes the same two words and leaves
           exactly this reading. Measuring the call itself would need a counter
           inside ROM code, or a sentinel written over DS state before the call
           and left behind when it does not run -- a behaviour change either
           way. So the line reports the observation and stops there. */
        if (row_before == row_after)
            std::fprintf(stderr, "  [rom-a054] R2c NOTE: the device pointer in "
                                 "data_020a8760 did not move across the call\n");
    }
}

}  // extern "C"
