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
//       THIS PARAGRAPH IS STALE AND IS KEPT ONLY SO THE CHANGE IS VISIBLE. It
//       used to read "port/ntr models no IPC at all", and three of the four
//       arms now RUN from the span below: run link100 lane IPC wrote
//       port/ntr/ipc.cpp and port/hal/boot2_ipc.cpp, the ARM7 claims six
//       channels at power-on, and every `while (func_0205ba3c(ch, 1) == 0) ;`
//       turns. func_0205b858, func_02059e48 and func_0205fde8 are real calls
//       here. func_0206a88c is the one that did not come with them, and it is
//       blocked on `G` rather than on the FIFO -- see below.
//
//   func_02019ebc
//       func_02061128(func_02019f10) -- the wireless manager's thread, and the
//       head of the ~175-TU WM chain. RE-DERIVED after lanes IPC and THR, since
//       both of the things this used to name have moved:
//         * "the port runs the game on ONE fiber (ARMSaveContext reports
//           already resumed)" is no longer true. hal/boot2_thread.cpp gives
//           every DS thread a Windows fiber and the ROM's own func_02057f54
//           switches between them.
//         * hal/boot2_ipc.cpp claims channel 0xa, so WM_Init's readiness spin
//           would turn.
//       WHAT IT STILL WANTS, measured: (1) THREAD CREATION. boot2_thread.cpp is
//       explicit that func_02058200 / func_02058538 / func_02058568 are not
//       linked and that "the only two threads are the two seated below. A
//       restore of a context this file has never seen is REFUSED loudly rather
//       than guessed at." func_02019ebc's whole purpose is to create a THIRD,
//       so it needs the seam to grow a CreateFiber path with the ROM's own
//       thread record behind it -- and it needs the DS stack func_02058200
//       paints, which the fiber model deliberately does not have.
//       (2) AN ARM7 WIRELESS DRIVER. boot2_ipc.cpp holds channel 0xa OBSERVED
//       ONLY, on the stated grounds that "nothing in this build runs WM_Init,
//       so a fabricated wireless answer would be answering a question no one
//       asked". Running WM_Init asks the question, and then every
//       WM_SendCommand needs a completion or the ROM waits forever -- the same
//       shape as the channel-6 refusal below. Neither is this lane's; both are
//       now single named pieces of work rather than "there is one fiber".
//
//   func_02058c84 and func_02019780 THEMSELVES
//       Both CALL the arms still skipped below, so neither can be seated as a
//       body; their runnable spans are transcribed here instead. func_02058c84
//       is three arms away from being a real call now (calls 4, 6 and 11), and
//       all three want the same `G` fix.
//
//   func_02058f28 / func_02058ec8 / func_02059594 / func_0206a88c
//       THE OS ARENA AND THE GBA-SLOT ARMS. func_02058f28 ends in
//       `func_02058d58(n, OS_GetInitArenaLo(n))` and hal/os_arena.cpp OWNS
//       func_02058d58 over an 8MB host block -- feeding it a DS-derived arena
//       address would move the root heap out from under Heap::SetupRootHeap.
//       That part is unchanged.
//
//       THE OTHER THREE ARE BLOCKED ON `G`, AND THAT IS NOT WHAT THE LAST PASS
//       OF THIS FILE SAID. Run link100 lane BOOT2 re-derived them. The Slot-2
//       read BOOT named is real -- func_02058764 -> func_02058690 reads
//       0x08000000/0x08000004 for the "NINTENDO" magic and no ntr::kRegions
//       entry covers that address -- but it is the SECOND thing that happens,
//       not the first. func_02058690 takes the ARM7 lock before it reads the
//       magic:
//
//           if ((func_020570b8((u16 *)0x027fffe8) & 0x40) != 0 ||
//               (res = func_02057158(x)) == 0) { ...read the magic... }
//
//       and 0x027fffec is zero in the port's shared block, so func_02057158
//       always runs. func_02057158 passes &func_02057140 as its cleanup
//       callback and func_02057178 passes &func_02057128, and BOTH of those
//       bodies are one line:  `*(unsigned short *)G ^= 0x80`, in the two
//       directions.
//
//       `G` IS include/decl_common.h's GENERIC PLACEHOLDER, not one byte, and
//       in this binary it links to hal/heap_vtable.cpp's default-heap word (the
//       `_G` the /MAP names). So taking that lock writes the game's default
//       heap pointer. THE ROM'S OWN LITERAL POOLS SAY WHAT EACH USER REALLY IS,
//       read out of extracted/arm9_dec.bin (load base 0x02004000) at each
//       function's pool word, none of which carries a reloc in
//       config/arm9/relocs.txt because they are absolute I/O addresses:
//
//           func_02057128  pool 0x0205713c  0x04000204  EXMEMCNT, |= 0x0080
//           func_02057140  pool 0x02057154  0x04000204  EXMEMCNT, &= ~0x0080
//           func_020570c0  pool 0x020570d4  0x04000204  EXMEMCNT, |= 0x0800
//           func_020570d8  pool 0x020570ec  0x04000204  EXMEMCNT, &= ~0x0800
//           func_02059f48  pool 0x02059f54  0x04000247  WRAMCNT, strb
//           func_02055454  pool 0x02055460  0x04000010
//           func_02059640  pool 0x0205964c  0x04000100
//           func_0205f650  pool 0x0205f668  0x04000304
//
//       Every one of those addresses IS mapped by the port (ntr's I/O window is
//       0x04000000 + 0x2000), so the bodies would be exactly right the moment
//       the decomp spells the register instead of the placeholder. It cannot be
//       done from here: `extern unsigned short G[];` needs a SYMBOL at an
//       absolute address, and MSVC has no way to give one (the same wall
//       func_02058308 hits below), and a per-source COMPILE_DEFINITIONS rename
//       onto a host object would desync bits 2-4 of the same register, which
//       func_0206a458 and func_0206a424 read and write directly.
//
//       So: func_02058ec8 (func_02058c84's call 4), func_02059594 (call 6) and
//       func_0206a88c (call 11, PXI channel 0xd, which lane IPC otherwise
//       unblocked -- the channel is claimed and the readiness spin turns) all
//       want the SAME one-line-per-file decomp fix first, and the empty Slot-2
//       region second. Together they are ~29 matched TUs, about half of them
//       hand-asm exceptions.
//
//   func_020134c8 -> func_020133bc
//       The ROM's sound bring-up. It takes 1MB out of Memory::Allocate and
//       stands the ROM's own SDAT player up over it, and the port serves sound
//       through hal/sdat. Two players over one SDAT is not a boot step, it is
//       a separate lane. RE-DERIVED after lane THR: it is now blocked twice
//       over, because its closure reaches func_02058200 and hal/boot2_thread
//       .cpp states that thread CREATION is not modelled -- THR made the switch
//       real, not a third fiber.
//
//   func_0203bbc0
//       RUN link100 lane BOOT2 measured this instead of inheriting "Crash()es
//       on a refusal". The Crash() calls are downstream; what actually stops
//       the boot is func_0205ea10(8), a bare spin on data_020a80cc + 0x36 that
//       only the ARM7's channel-6 completion clears. The full measurement is at
//       its own point in the boot below and in port/slice_gate222.txt.
//
//   func_02053c40 / func_02019440 / func_020196cc
//       NO LONGER SKIPPED. Run link100 lane BOOT2 seated all three at their own
//       points in func_0201a054; see port/slice_gate222.txt and the comments
//       below. The graphics pair was BOOT's "left for the step that moves the
//       graphics bring-up as a whole", and this is that step; func_020196cc
//       needed lane IPC's ARM7 to answer channel 8 before it could return.
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
//       has already staged by this point in its own boot. RE-DERIVED, and the
//       harder half was missing: hal/level_boot.cpp hosts data_0209caa0 as a
//       0x14 object -- the dsd symbol's own span, next name data_0209cab4 --
//       while the ROM object is the full 0x32c. The arithmetic closes exactly:
//       0x0209caa0 + 0x32c = 0x0209cdcc, which is the next symbol AFTER the
//       four dsd names inside the run (data_0209cab4, data_0209cad2,
//       data_0209cae4, data_0209caf4). Seating the body today writes 796 bytes
//       into a 20-byte object. It wants one grouped run in hal/level_boot.cpp.
//
//   func_02059f48
//       REFUSED ON A NAME, and the name is now known. Its body is `G[0] = v`.
//       `G` is include/decl_common.h's generic placeholder (`extern int G;`),
//       used by eight different TUs for eight different absolutes, and the last
//       pass of this file guessed wrong twice: it is not one byte and it is not
//       the port's screen-gap word. In this binary `G` links to
//       hal/heap_vtable.cpp's default-heap word, and the byte THIS TU means is
//       0x04000247 -- WRAMCNT -- read out of its own literal pool at
//       0x02059f54, with no reloc against it in config/arm9/relocs.txt because
//       it is an absolute I/O address. func_02058c84's own call is
//       func_02059f48(3), i.e. WRAMCNT = 3. Still refused, and still for the
//       decomp to fix: see the `G` block above for why the port cannot bind it
//       from here. The full per-TU address table is there.
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
void func_02053c40(void);                     // the ROM's own display reset
int  func_02053be0(int enable);               // DISPSTAT VBlank-IRQ enable
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
void func_02019440(void);                     // the VRAM/OAM/palette clear + 3D
void func_0203b684(void);
void func_020233f0(void);
void func_020196cc(void);                     // the firmware read + sound mode
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
extern "C" void port_os_lock_words_seed(void);
extern "C" void func_0205b858(void);
extern "C" void func_02059e48(void);
extern "C" void func_0205fde8(void);
extern "C" void port_ipc_rom_boot_done(void);

void port_boot_rom_pre_main(void)
{
    // func_02058c84's arms, in the ROM's order. The gaps are the PXI four;
    // see the header block for the measurement that closes them.
    /* func_02058f28() -- the OS arena, owned by hal/os_arena.cpp */
    func_0205b858();   /* call 2: PXI init (func_0205bad8); the ARM7 half is
                          hal/boot2_ipc.cpp's model, attached before main() */
    /* func_02057320() -- takes the 0x7e lock and spins on the shared block.
       Its two stores (the OS lock words at 0x027fffb0 = -1, -0x10000) are
       the only part a host can honour; hal/os_lockid.cpp writes them here,
       at the ROM's own point in the order, so OS_GetLockID hands out ids
       the way the ROM does instead of -3. */
    port_os_lock_words_seed();
    /* func_02058ec8() -- REFUSED. Run link100 lane BOOT2 re-derived it: the
       unmapped GBA slot at 0x08000000 is real but it is the SECOND blocker.
       func_02058764 -> func_02058690 takes the ARM7 lock through func_02057158
       first, whose cleanup callback func_02057140 writes `G` -- the decomp's
       generic placeholder, which links to hal/heap_vtable.cpp's default-heap
       word here and MEANS 0x04000204 (EXMEMCNT) per the ROM's own literal pool
       at 0x02057154. Running this arm writes the default heap pointer. The
       header block carries the whole table. */
    func_02057000();
    /* func_02059594() -- REFUSED for the same reason: same func_02058764 path,
       same lock, same `G`. Everything else about it is host-safe -- it stores
       func_020593f4 into 0x027ffd9c (mapped, shared block) and 0x023c3fdc
       (mapped, inside ntr's main-RAM reservation). */
    /* func_02059f48(3) -- REFUSED ON THE SAME NAME, and the byte is now known.
       `G` here is 0x04000247, WRAMCNT, out of this TU's own literal pool at
       0x02059f54 (no reloc, because it is an absolute I/O address), so the
       ROM's line is WRAMCNT = 3. The port maps that address; what it cannot do
       is give `extern int G;` an absolute address under MSVC. The decomp has
       to name the register. See the header block. */
    func_02059cb4();
    /* func_02058308() -- the OS thread system. STILL REFUSED, and the reason
       has changed: the host-global size named here is fixed (the ROM's one
       0x54-byte thread-info record is a grouped run in hal/cxx_aliases.cpp
       now), and what is left is func_00000000 / func_00000600, two ABSOLUTE
       linker symbols the body reads as numbers. See the header block for the
       arithmetic they feed and why MSVC cannot supply them. */
    func_02059e48();   /* call 10: PXI channel 0xc, answered by the model */
    /* func_0206a88c() -- PXI channel 0xd. LANE IPC OPENED THE CHANNEL AND `G`
       CLOSED IT AGAIN. hal/boot2_ipc.cpp claims 0xd at power-on, so the
       readiness spin `while (func_0205ba3c(0xd, 1) == 0)` turns now, and
       func_0206a6d0's GBA-slot DMA is not even reached (its own guard is
       `if ((*(volatile u16 *)0x4000300 & 1) == 0) return;` -- REG_POSTFLG,
       which nothing in this port writes, so the mapped I/O word is zero and the
       body returns before the 0x08000080 read). What blocks it is func_0206a3a4,
       three calls in: `func_02057158(arg0)`, the same OS lock whose cleanup
       callback writes `G`. Same one-line-per-file decomp fix as the two arms
       above; ~18 matched TUs behind it, several of them hand-asm. */
    /* func_02060890() -- the game card, reached instead through func_02042f68 */
    func_0205fde8();   /* call 13: PXI channel 8, answered by the model */

    /* func_0201a490() -- one line, `data_0209a03c(data_0208ee60)`. The .data
       word at 0x0209a03c is RELOCATED (config/arm9/relocs.txt:
       from:0x0209a03c to:0x02057e30), so the ROM's own bytes there are a DS
       address that means nothing here; hal/ptr_tables.cpp binds it to the host
       func_02057e30 the way it binds the ROM's other function tables, and with
       that binding this is a real call again. */
    func_0201a490();
    say("pre-main OS init (func_02019780)");
    port_ipc_rom_boot_done();   /* the PXI model's report + selftest, after the ROM's handshake */
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
    /* THE DISPLAY RESET, run link100 lane BOOT2. It resets POWCNT1, calls
       func_02053d9c (the VRAM bank shadow and VRAMCNT A..I), zeroes DISPSTAT
       and DISPCNT, clears both 2D register banks and seeds the EIGHT BG affine
       words to 0x100. BOOT left it "for the step that moves the graphics
       bring-up as a whole" on the grounds that it was not obviously correct
       where the host is by this line. MEASURED, and it is:

         * this seam runs before ANY of the port's own graphics bring-up. The
           level boot in tests/walk_window.cpp is below it, and
           hal_sub_screen_init -- the host file that writes four of these eight
           affine words by hand -- is called ~800 lines further down still, so
           the ROM's reset cannot wipe host state that has not been written.
         * hal/sub_screen.cpp:947 is the port's own record of the gap this
           closes: it seeds the SUB engine's four and says the main engine's
           four are "still unseeded, which is a real gap and deliberately not
           closed from this line". The ROM's own body seeds all eight.
         * POWCNT1 bit 15 (main engine drives the top screen) SURVIVES: the
           middle store's mask is 0xfffffdf1, which keeps bit 15, and the first
           store sets it. The host line in sub_screen.cpp sets it again later.
         * the VRAM banks func_02053d9c turns off are turned back on by the
           ROM's own func_02019440 -> func_02054430(0x1ff) and func_0200f4f4 in
           the tail below, and after that by each scene's InitResources. */
    func_02053c40();
    /* func_0205b858() -- PXI */
    func_0203d740();
    /* func_02013e64() -- memsets the save block; see the header block */
    func_0201a4e4();          // IRQ::SetIRQHandler(1, IRQ::VBlankHandler)
    /* the IRQ trio: ntr::rt_irq_boot_state() */
    /* DISPSTAT bit 3, the VBlank-IRQ enable. Already-linked ROM body, so it
       adds nothing to the count; it is here because it is the ROM's own next
       line and the port can run it. It touches bit 3 only, and ntr's HBlank
       gate reads bit 4 (ntr/runtime.cpp's DISPSTAT_HBLANK_IRQ_ENABLE), so the
       two do not meet. func_02019440 in the tail makes the same call again,
       exactly as the ROM does. */
    func_02053be0(1);
    /* func_0203bbc0() -- REFUSED, and the reason is now measured rather than
       "Crash()es on a refusal". Its first call, func_0205f270, registers the
       touch/SPI receive callback on PXI channel 6 and comes back fine now that
       lane IPC claims the channel. What does not come back is the THIRD line:
       func_0205edd8(3, 0x1e) posts two words on channel 6 and sets
       data_020a80cc + 0x36 bit 3, and func_0205ea10(8) is nothing but
           while (*(volatile u16 *)(data_020a80cc + 0x36) & mask) ;
       Only func_0205f300 clears that bit, and only for an ARM7 completion whose
       (msg & 0x7f00) >> 8 is the same bit with (msg & 0xff) == 0.
       hal/boot2_ipc.cpp holds channel 6 OBSERVED ONLY and its no-driver reply
       carries the no-handler flag, which func_0205f300 turns into a SET of f34
       rather than a clear of f36 -- so the spin never turns. WHAT IT WANTS is a
       host ARM7 touch/SPI driver (completions for bit 3 and bit 1, and live
       samples at 0x027fffaa/0x027fffac), which is a subsystem and not a boot
       step. Its host-side blocker is real too and belongs to another lane:
       hal/scene_boot.cpp hosts data_020a80cc as int[6], 24 bytes, and the ROM
       object runs 0x38 -- 0x020a80cc to 0x020a8104, absorbing the dsd name
       data_020a80e4 at +0x18. */
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
    /* THE OTHER HALF OF THE GRAPHICS BRING-UP, run link100 lane BOOT2.
       func_0203bd24 (empty in the ROM), POWCNT1 again, func_02053be0(1),
       func_02054430(0x1ff) -- which is what turns the VRAM banks
       func_02053d9c cleared back on -- 0xa4000 bytes of VRAM zeroed from
       0x06800000, 0x400 of OAM to 0xc0 and 0x400 of palette to 0, the three
       stage words, and then func_0200f4f4: Initialise3dGraphics(0),
       GX::SetBankForTex(3), GX::SetBankForTexPltt(0x30), func_02045d9c and the
       data_02082128 -> data_0209b3ec matrix with its inverse.
       IT IS SAFE HERE FOR THE SAME REASON func_02053c40 IS: nothing the port
       draws has happened yet at this line. tests/walk_window.cpp seeds
       data_0209b3ec by hand right after port_boot_rom_main_head(); this is the
       ROM's own writer of the same words and it runs later, so the ROM's value
       is the one that stands. The 0x06800000 span is sub-engine BG VRAM, which
       no scene has loaded into by this line. */
    func_02019440();
    /* func_020134c8() -- the ROM's sound bring-up. STILL REFUSED, and lane THR
       did not open it: besides the two-players-over-one-SDAT reason BOOT gave,
       its closure reaches func_02058200, and hal/boot2_thread.cpp states that
       thread CREATION is not modelled -- the fiber seam has exactly two
       threads and refuses a context it has never seen. */
    func_0203b684();
    /* data_020a4bb8 = data_02090864 -- already seated by the host boot */
    func_020233f0();
    /* Scene::PrepareToSpawnBoot() -- picks the ROM boot scene */
    /* THE FIRMWARE READ AND THE SOUND MODE, run link100 lane BOOT2. BOOT could
       not have run this: func_0205f8e0 -> func_0205fb1c -> func_0205fb58 posts
       a channel-8 command and then SPINS in func_0205ff08 until the ARM7's
       answer clears data_020a8114, and there was no ARM7. There is one now, and
       ntr/ipc.cpp dispatches the reply INLINE from ipc_arm7_send, so the word
       is back through the ROM's own IRQ::IPCRxFifoNotEmptyHandler ->
       func_0205fcfc -> func_0205feac before IPCSend returns.
       WHAT THE ANSWER SAYS. hal/boot2_ipc.cpp holds channel 8 with no driver
       behind it, so it replies with the SDK's own no-handler shape. That makes
       func_0205fcfc take its flag arm and func_0205f8e0 return 2 -- a FAILED
       READ, which is a truthful answer, and func_020196cc's own arm for a
       failed read is to leave data_0208ee3c alone. Nothing here fabricates a
       user-settings block to make the other arm run. The two lines that always
       execute -- func_0203b9b4(data_0209d4b8, 0x4d2) and
       SetSoundMode(data_0209d4b4[0]) -- are the ROM's, and hal/scene_mg.cpp
       already carried a hand copy of the first of them for the minigames. */
    func_020196cc();
    func_0201a5cc();
    say("game init tail (func_0201a054)");
}

}  // extern "C"
