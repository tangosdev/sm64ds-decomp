// ---------------------------------------------------------------------------
// boot_os.cpp -- the ROM's own boot chain, run at the points the ROM runs it.
//
// WHAT THIS REPLACES. The port boots through tests/walk_window.cpp's main(),
// which stands in for the ROM's Entry and main(). Two of the ROM's boot steps
// were already there and named as such -- Heap::InitializeRootHeap (the
// func_02019780 step) and Heap::InitializeGameHeap(0x3b000, 0) (the
// func_0201a054 step) -- with the derivation written above each. Everything
// else the ROM does between and around them was simply not done: the OS tick,
// the thread system, the VBlank handler install, the debug/fatal vector table.
// The 503 matched translation units that hang off Entry/main were unlinked for
// exactly that reason, not because they could not build.
//
// This file is the next slice of that same transcription. Each block below is
// one span of a ROM function, in the ROM's order, calling the ROM's own bodies:
//
//   port_boot_rom_pre_main()        src/func_02019780.c, before the root heap
//   port_boot_rom_main_head()       src/main.c, its first three calls
//   port_boot_rom_game_init_head()  src/func_0201a054.c, before the game heap
//   port_boot_rom_game_init_tail()  src/func_0201a054.c, after the game heap
//
// THE RULE THIS FILE OBEYS. Nothing here calls a ROM function anywhere except
// where the ROM calls it. Where a ROM step cannot run on the host the step is
// SKIPPED and the comment says which hardware is missing -- it is never
// replaced by a host imitation, and it is never called from somewhere else to
// make a translation unit link.
//
// WHAT IS SKIPPED, AND WHY (measured, not assumed):
//
//   func_0205b858 / func_02059e48 / func_0206a88c / func_0205fde8
//       The four PXI arms of func_02058c84. Each one ends in
//       `while (func_0205ba3c(ch, 1) == 0) ;` -- a spin on the ARM7's channel
//       readiness bits in the shared block at 0x027ffc00 + 0x388, which only
//       the ARM7 sets. port/ntr models no IPC at all (port/docs/
//       mmio-inventory.md: IPC, 8 registers, 0 modelled), so those words never
//       change and the boot never comes back. Modelling the ARM7 side of the
//       handshake is the work that unblocks them, and it unblocks 81 more
//       matched TUs under func_02058c84 when it lands.
//
//   func_02019ebc
//       func_02061128(func_02019f10) -- the wireless manager's thread. It
//       creates a DS thread and resumes it, and the port runs the game on ONE
//       fiber (ARMSaveContext reports "already resumed", hal/cxx_aliases.cpp),
//       so a second live thread has nowhere to run.
//
//   func_02058c84 and func_02019780 THEMSELVES
//       Both CALL the skipped arms above, so neither can be seated as a body;
//       their runnable spans are transcribed here instead. They go back to
//       being real calls the moment the PXI model lands.
//
//   func_02058f28 / func_02058ec8 / func_02059594
//       The OS arena bring-up. func_02058f28 ends in
//       `func_02058d58(n, OS_GetInitArenaLo(n))` and hal/os_arena.cpp OWNS
//       func_02058d58 over an 8MB host block -- feeding it a DS-derived arena
//       address would move the root heap out from under Heap::SetupRootHeap.
//       func_02058ec8 and func_02059594 both go through func_02058764, which
//       reads the GBA slot at 0x08000000/0x08000004 looking for the "NINTENDO"
//       magic, and 0x08000000 is in NO ntr region (ntr::kRegions is main / IO /
//       palette / VRAM / OAM / shared), so the read faults. Both want the same
//       small piece of hardware model: an empty Slot-2.
//
//   func_020134c8 -> func_020133bc
//       The ROM's sound bring-up. It takes 1MB out of Memory::Allocate and
//       stands the ROM's own SDAT player up over it, and the port serves sound
//       through hal/sdat. Two players over one SDAT is not a boot step, it is
//       a separate lane.
//
//   func_0201fec8 / func_0203bbc0 / func_02018aa4 / func_02019440
//       Left for the next step. func_0203bbc0 goes into the comms stack and
//       Crash()es on a refusal, func_02019440 clears VRAM/OAM/palette (correct
//       at the ROM's point in the boot, not obviously correct at the host's),
//       and func_0201fec8 reads the user settings block. Each needs its own
//       reading; none is blocked by hardware.
//
//   func_02058308 / func_0201a9fc / func_02013e64
//       REFUSED ON A HOST-GLOBAL SIZE, not on hardware. func_02058308 is the
//       OS thread system and fills data_020a6134.slots[0..15] -- bytes 0x14
//       through 0x54 of an object hal/cxx_aliases.cpp hosts as `int[4]`, 16
//       bytes, with the next global on the same declaration line behind it.
//       func_0201a9fc writes
//       an s64 at data_0209d574 + 0x38 and hal/actor_vtables.cpp hosts
//       data_0209d574 as `int[8]` -- 32 bytes, so the write lands past the end
//       of the object. func_02013e64 memsets 0x32c bytes of data_0209caa0, the
//       save block, which the host has already staged by this point in its own
//       boot. Both are one-line calls that become correct the moment the
//       hosted globals are sized by ROM span; see the port's undersized-global
//       rule.
//
//   func_02059f48
//       REFUSED ON A NAME. Its body is `G[0] = v`, and `G` is a placeholder
//       the decomp gave an unnamed byte -- there is no G in
//       config/arm9/symbols.txt at all. In this binary `G` is the port's own
//       screen-gap word (hal/screen_gap.cpp), so seating the ROM body would
//       have the boot set the screen gap to 3. It needs the decomp to name the
//       byte, not the port to work around it.
// ---------------------------------------------------------------------------

#include <cstdio>

#include "dsstate_seg.h"

// main()'s own thread object. src/main.c passes `&data_020a621c` to
// func_02059578, and no host file defines it, so the port has to. SIZED BY ROM
// SPAN, not by the fields the callers happen to touch: config/arm9/symbols.txt
// puts data_020a621c at 0x020a621c in .bss with the next symbol at 0x020a6378,
// so the object is 348 bytes. (func_02058308 writes as far as +0x74 and is
// refused for an unrelated reason below; an object sized to that write instead
// of to the span would be the exact undersized-global bug this port keeps
// finding.) It is mutable DS state, so it goes in the .dsstate capture section
// the save state rolls back, the same as hal/auto_bss.cpp's globals.
DSSTATE_BEGIN
extern "C" {
char data_020a621c[348];
}
DSSTATE_END

extern "C" {

// --- src/func_02058c84.c, the runnable arms ---------------------------------
void func_02057000(void);          // DTCM word 0 = 0
void func_02059cb4(void);          // the DMA-channel bookkeeping + DisableIRQs(4)
// --- src/func_02019780.c ----------------------------------------------------
/* func_0201a490 is NOT called -- see the refusal at its point in the boot */
// --- src/main.c -------------------------------------------------------------
void func_02059788(void);          // OS_InitTick: timer 0 + its IRQ handler
void func_02059bc0(void);          // the alarm system
void func_02059578(int a, int b);  // the main thread record
void func_0201a03c(void);          // main()'s thread entry, passed by address
// --- src/func_0201a054.c, in its own order ----------------------------------
void func_020427f8(void);                     // veneer -> func_02059788
unsigned long long func_02042784(void);       // the boot timestamp
void func_02053a8c(void);
void func_02053c40(void);
void func_0203d740(void);
void func_0201a4e4(void);                     // install IRQ::VBlankHandler
void func_0203bb5c(void);
/* func_02042f68 is NOT called -- see the refusal at its point in the boot */
void func_0203ad84(void);
void func_0203b684(void);
void func_020233f0(void);
void func_0201a5cc(void);                     // the fatal-vector pair


// Which arms actually ran, printed once so a boot log says what the ROM did
// rather than what this file intended.
static void say(const char *what) {
    std::fprintf(stderr, "[boot] ROM %s\n", what);
}

// ---------------------------------------------------------------------------
// src/func_02019780.c, the step Entry runs before main.
//
//     func_02058c84();   <- transcribed below, four PXI arms skipped
//     func_0201a490();
//     func_02019ebc();   <- skipped, the wireless thread
//     Heap::InitializeRootHeap();   <- ALREADY in tests/walk_window.cpp
//     Heap::SetNodeID(*data_020a0e9c, 1);
//
// Called immediately before that InitializeRootHeap line, which is where the
// ROM's own order puts it.
// ---------------------------------------------------------------------------
void port_boot_rom_pre_main(void)
{
    // func_02058c84's arms, in the ROM's order. The gaps are the PXI four;
    // see the header block for the measurement that closes them.
    /* func_02058f28() -- the OS arena, owned by hal/os_arena.cpp */
    /* func_0205b858() -- PXI */
    /* func_02057320() -- takes the 0x7e lock and spins on the shared block */
    /* func_02058ec8() -- reaches func_02058764 -> the unmapped GBA slot */
    func_02057000();
    /* func_02059594() -- reaches func_02058764 -> the unmapped GBA slot */
    /* func_02059f48(3) -- src/func_02059f48.c writes `G[0] = v`, and `G` is a
       placeholder name the decomp gave an unnamed byte: there is no G in
       config/arm9/symbols.txt. In THIS binary `G` is the port's own screen-gap
       word (hal/screen_gap.cpp), so seating the ROM body would have this line
       set the screen gap to 3. Refused until the decomp names the byte. */
    func_02059cb4();
    /* func_02058308() -- the OS thread system. REFUSED ON A HOST-GLOBAL SIZE,
       not on hardware: it fills data_020a6134.slots[0..15], i.e. bytes 0x14
       through 0x54 of that object, and hal/cxx_aliases.cpp hosts
       data_020a6134 as `int[4]` -- 16 bytes, with nothing behind it but the
       next global on the same declaration line. Sizing that object by its ROM
       span unblocks the thread system, func_02058200/func_02058048 and
       func_02057f54 in one go; cxx_aliases.cpp is not this lane's file. */
    /* func_02059e48() -- PXI channel 0xc */
    /* func_0206a88c() -- PXI channel 0xd */
    /* func_02060890() -- the game card, reached instead through func_02042f68 */
    /* func_0205fde8() -- PXI channel 8 */

    /* func_0201a490() -- one line, `data_0209a03c(data_0208ee60)`, and
       data_0209a03c is a .data FUNCTION POINTER the ROM initialises to
       0x02057e30. config/arm9/relocs.txt says that word is relocated, so the
       ROM byte in it is a DS address and hosting it verbatim would call into
       nothing. Binding it to the host func_02057e30 the way hal/ptr_tables.cpp
       binds the ROM's other function tables is the fix, and that file is not
       this lane's. */
    say("pre-main OS init (func_02019780)");
}

// ---------------------------------------------------------------------------
// src/main.c, its first three calls. main() runs these before func_0201a054,
// and Entry runs func_02019780 before main, so this belongs immediately after
// the root heap comes up.
//
//     func_02059788();
//     func_02059bc0();
//     func_02059578(&func_0201a03c, &data_020a621c);
//     Heap::SetNodeID(*data_020a0e9c, 2);
//
// The two SetNodeID calls are name-tagging for the heap debugger and are left
// to the existing host line that already carries the root heap.
// ---------------------------------------------------------------------------
void port_boot_rom_main_head(void)
{
    func_02059788();
    func_02059bc0();
    // main() hands the thread entry and the thread object by ADDRESS; the
    // two words go into data_020a63a0/data_020a63a4 and nothing calls
    // through them until the scheduler actually switches, which the port's
    // single fiber never does.
    func_02059578((int)(void *)&func_0201a03c, (int)(void *)data_020a621c);
    say("main() head: tick, alarms, main-thread record");
}

// ---------------------------------------------------------------------------
// src/func_0201a054.c, the span BEFORE Heap::InitializeGameHeap(0x3b000, 0).
// The ROM's order, with the skips named in the header block:
//
//     func_020427f8(); func_02042784(); func_02053a8c(); func_02053c40();
//     func_0205b858(); func_0203d740(); func_02013e64(); func_0201a4e4();
//     IRQ::EnableIRQs(1); IME = 1; IRQ::Enable();
//     func_02053be0(1); func_0203bbc0(); func_0203bb5c();
//     if (!r4) func_0201fec8();
//     func_02042f68(0xd01, data_0208ee50);
//     if (!r4) func_02018aa4();
//     if (r4) IRQ::DisableIRQs(0x100000);
//     if (!r4) func_0203ad84();
//     func_0201a9fc(data_0209d574);
//
// r4 is `*(u16*)0x027ffc40 == 2`, and ntr/io.cpp writes 0 to that word at
// io_init, so every `if (!r4)` arm is the one the port takes -- which is what
// makes func_0203ad84 a step of this boot and not a branch of it.
//
// The IRQ trio (EnableIRQs(1), IME = 1, IRQ::Enable) is ALREADY done, by
// ntr::rt_irq_boot_state(), whose own comment cites this exact line of
// func_0201a054 as its source. It is not repeated here.
// ---------------------------------------------------------------------------
void port_boot_rom_game_init_head(void)
{
    func_020427f8();          // -> func_02059788, guarded and idempotent
    func_02042784();          // the boot timestamp off the tick
    func_02053a8c();
    /* func_02053c40() -- resets POWCNT/DISPCNT and re-clears the 2D register
       banks. Correct where the ROM has it (nothing has drawn yet) and not
       obviously correct where the host is by this line; left for the step
       that moves the graphics bring-up as a whole. */
    /* func_0205b858() -- PXI */
    func_0203d740();
    /* func_02013e64() -- memsets the save block; see the header block */
    func_0201a4e4();          // IRQ::SetIRQHandler(1, IRQ::VBlankHandler)
    /* the IRQ trio: ntr::rt_irq_boot_state() */
    /* func_02053be0(1) -- DISPSTAT VBlank-IRQ enable, part of the graphics
       bring-up above */
    /* func_0203bbc0() -- the comms stack; Crash()es on a refusal */
    func_0203bb5c();
    /* func_0201fec8() -- the user settings block */
    /* func_02042f68(0xd01, data_0208ee50) -- REFUSED, and this one is the same
       undersized-global class as func_02058308 rather than a new problem. It
       reaches func_02060890 -> func_0206002c, which stands the game card's
       thread up with
           func_02058200(&data_020a81bc, func_020602bc, 0, &data_020a8760,
                         0x400, 4)
       -- and func_02058200 takes the FOURTH argument as the TOP of the new
       thread's stack and writes 0x400 bytes DOWNWARD from it. On the DS that
       is in bounds by construction: symbols.txt gives data_020a81bc a span of
       1444 bytes ending exactly at data_020a8760, so the thread record and its
       1KB stack are one contiguous bss object. Two separate host globals do
       not reproduce that, and the write lands in whatever the host linker put
       below. func_02058200 also indexes data_020a6134.arr[0..15] through
       func_02058538, which is the same 16-byte host object that refuses
       func_02058308. Hosting data_020a81bc..data_020a8760 as ONE span (a
       grouped section, the idiom hal/scene_boot.cpp already uses for a split
       run) plus sizing data_020a6134 unblocks the card driver AND the thread
       system together -- about 37 more matched TUs. */
    /* func_02018aa4() -- the file-system bring-up */
    func_0203ad84();
    /* func_0201a9fc(data_0209d574) -- writes past the hosted global's end */
    say("game init head (func_0201a054)");
}

// ---------------------------------------------------------------------------
// src/func_0201a054.c, the span AFTER Heap::InitializeGameHeap(0x3b000, 0):
//
//     func_0201a9fc(data_0209d574);
//     func_02019440();
//     if (!r4) func_020134c8();
//     func_0203b684();
//     data_020a4bb8 = data_02090864;
//     func_020233f0();
//     Scene::PrepareToSpawnBoot();
//     func_020196cc();
//     data_0209ee90.f44 = 0x100;
//     *(u32*)0x040004c8 = 0x296a5800; ... (the 3D fog/edge registers)
//     func_0201a5cc();
//
// `data_020a4bb8 = data_02090864` is the actor spawn table pointer, which
// tests/walk_window.cpp already seats on its own line, and
// Scene::PrepareToSpawnBoot picks the ROM's boot scene (0x168) -- a decision
// the host boot has already made by this point, so it is not taken back here.
// ---------------------------------------------------------------------------
void port_boot_rom_game_init_tail(void)
{
    /* func_0201a9fc(data_0209d574) -- see the header block */
    /* func_02019440() -- clears VRAM/OAM/palette */
    /* func_020134c8() -- the ROM's sound bring-up */
    func_0203b684();
    /* data_020a4bb8 = data_02090864 -- already seated by the host boot */
    func_020233f0();
    /* Scene::PrepareToSpawnBoot() -- picks the ROM boot scene */
    /* func_020196cc() -- reads the wireless id and sets the sound mode */
    func_0201a5cc();
    say("game init tail (func_0201a054)");
}

}  // extern "C"
