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
//   func_0203bbc0 / func_02019440
//       Left for the next step. func_0203bbc0 goes into the comms stack and
//       Crash()es on a refusal, and func_02019440 clears VRAM/OAM/palette
//       (correct at the ROM's point in the boot, not obviously correct at the
//       host's). Each needs its own reading; neither is blocked by hardware.
//
//   func_0201fec8
//       REFUSED, AND NOT FOR THE REASON THE LAST PASS OF THIS FILE GAVE. Run
//       link100 lane GLOBALS sized data_0209d574 by its ROM span, which was
//       the blocker written down here, and then measured what else the body
//       reaches before seating it. func_0201fec8's fall-through arm (the port
//       takes it: *(u16*)0x027ffc40 is 0, so buf[0] off the zeroed shared
//       block selects the default case) is
//
//           func_0203db64(data_02075358, data_02075304)
//
//       and THAT body is a second undersized-global field of the same class,
//       eight names wide. Counted against config/arm9/symbols.txt:
//
//           data_020a1064   dsd span   64, written 0x40   NO HOST AT ALL
//           data_020a0fec   dsd span   52, written 0x1c   NO HOST AT ALL
//           data_020a0fa0   dsd span    6, written 0x18   NO HOST AT ALL
//           data_020a1040   dsd span    4, written 0x24   hal/camera_bridges
//           data_020a0fb8   dsd span    6, written 0x18   hal/comms_conductor
//           data_020a10a4   dsd span    2, written 0x58   hal/comms_conductor
//           data_020a10fc   dsd span    2, written 0x58   hal/comms_conductor
//           data_020a1154   dsd span   12, written 0x90   hal/camera_bridges
//
//       Every one of those is a dsd-split run whose host grouping would have
//       to be proved contiguous for the FULL memset length before the call is
//       safe, three of them are not hosted at all, and the two grouped runs
//       that do exist belong to the comms and camera lanes rather than to
//       this one. Sizing data_0209d574 unblocked func_0201a9fc; it did not
//       unblock this. The measurement is left here so the next lane starts
//       from it instead of from the old one-line claim.
//
//   func_02058308
//       REFUSED, AND THE HOST-GLOBAL SIZE WAS NEVER THE WHOLE STORY. The
//       storage half is fixed: the ROM's OS thread-info record is one
//       0x54-byte object under three dsd names and hal/cxx_aliases.cpp's
//       link100 GLOBALS block now hosts it as one grouped run (the old int[4]
//       and hal/auto_bss.cpp's separate data_020a6148 were the bug, and the
//       second of those was the quiet half -- func_02058538 scanned one array
//       while func_02058200 wrote another). What still refuses the call is an
//       ADDRESS, not a size. src/func_02058308.c reads two absolute linker
//       symbols:
//
//           ovr = (s32)func_00000000;                 the DS's SYS stack size
//           ... - (s32)func_00000600;                 the DS's IRQ stack size
//
//       Neither is in config/arm9/symbols.txt at all: dsd invented the names
//       from literal-pool words whose VALUE is 0 and 0x600, because on the DS
//       these are absolute linker symbols whose ADDRESS is the number. MSVC
//       has no way to give a symbol an absolute address -- the same wall
//       hal/scene_boot.cpp writes up for overlay_100 -- so the body does not
//       even link today (two unresolved externals), and defining them as
//       ordinary host functions is worse than not linking: with
//       (s32)func_00000000 non-zero the body takes the other arm, computes
//
//           end = &data_023c0000 + 0x3fc0 - (s32)func_00000600
//
//       from a host code address, and stores 0xfddb597d through it. On the DS
//       those two arms resolve to base 0x023c0020 / end 0x023c39c0 -- DTCM,
//       the main thread's stack -- which is what the port would have to model
//       to seat this at all.
//
//   func_02013e64
//       memsets 0x32c bytes of data_0209caa0, the save block, which the host
//       has already staged by this point in its own boot.
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
void func_0201a490(void);          // data_0209a03c(data_0208ee60)
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
void func_02018aa4(void);                     // the file-system bring-up
void func_0203ad84(void);
/* func_0201a9fc(data_0209d574): the ROM calls this twice in func_0201a054,
   once on each side of Heap::InitializeGameHeap. hal/actor_vtables.cpp hosts
   data_0209d574 at its ROM span of 68 bytes, which is what the +0x38 store
   needs; the object is declared here as the char[] the matched TU's own
   prototype takes. */
void func_0201a9fc(void *c);
extern char data_0209d574[];
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
    /* func_02058308() -- the OS thread system. STILL REFUSED, and the reason
       has changed: the host-global size named here is fixed (the ROM's one
       0x54-byte thread-info record is a grouped run in hal/cxx_aliases.cpp
       now), and what is left is func_00000000 / func_00000600, two ABSOLUTE
       linker symbols the body reads as numbers. See the header block for the
       arithmetic they feed and why MSVC cannot supply them. */
    /* func_02059e48() -- PXI channel 0xc */
    /* func_0206a88c() -- PXI channel 0xd */
    /* func_02060890() -- the game card, reached instead through func_02042f68 */
    /* func_0205fde8() -- PXI channel 8 */

    /* func_0201a490() -- one line, `data_0209a03c(data_0208ee60)`. The .data
       word at 0x0209a03c is RELOCATED (config/arm9/relocs.txt:
       from:0x0209a03c to:0x02057e30), so the ROM's own bytes there are a DS
       address that means nothing here; hal/ptr_tables.cpp binds it to the host
       func_02057e30 the way it binds the ROM's other function tables, and with
       that binding this is a real call again. */
    func_0201a490();
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
    /* func_0201fec8() -- the user settings block. Still refused; the blocker
       is func_0203db64's eight-name wireless run and not data_0209d574, which
       is sized now. The count is in the header block. */
    /* func_02042f68(0xd01, data_0208ee50) -- STILL REFUSED, and the storage
       half of the old refusal is gone. It reaches func_02060890 ->
       func_0206002c, which stands the game card's thread up with
           func_02058200(&data_020a81bc, func_020602bc, 0, &data_020a8760,
                         0x400, 4)
       -- func_02058200 takes the FOURTH argument as the TOP of the new
       thread's stack and fills 0x400 bytes DOWNWARD from it. hal/
       globals_link100.cpp hosts that storage now, as one grouped run, and its
       boundary is NOT the one written here before: func_0206002c's own base
       register is data_020a8180 and it stores at +0xd4, which is
       data_020a81bc + 0x98, so the group has to START at data_020a8180 and it
       has to END past data_020a8760 (the stack top itself). 60 + 1444 + 4,
       with the ROM evidence in that file.
       WHAT ACTUALLY REFUSES IT is below the storage. func_0206002c's next two
       calls are func_02058048(&data_020a81bc), which resumes the new thread
       through func_02057f54 -- the ROM scheduler, ARMSaveContext and
       ARMRestoreContext, on a port that runs the game on ONE fiber -- and
       func_0205ba64(0xb, func_02060310), a PXI channel registration on the
       seam hal/os_arena.cpp and the four func_02058c84 arms are already
       waiting on. The thread this creates is func_020602bc, the card driver's
       own service loop. That is the same ARM7 model the header block names,
       not a global size, so the estimate of "about 37 more matched TUs" that
       used to sit here belongs to the PXI lane and not to this one. */
    func_02018aa4();          // FS_Init's once-guard (already tripped) +
                              // func_02017e60, which clears the twelve
                              // overlay-resident records at data_0209d3c4
    func_0203ad84();
    /* The ROM calls this on both sides of Heap::InitializeGameHeap; this is
       the first of the two. data_0209d574 is 68 bytes now, its ROM span, so
       the s64 the tick goes into at +0x38 is inside the object. */
    func_0201a9fc(data_0209d574);
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
    /* the second of func_0201a054's two calls, the one after the game heap */
    func_0201a9fc(data_0209d574);
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
