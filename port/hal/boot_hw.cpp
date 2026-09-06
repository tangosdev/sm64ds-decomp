// ---------------------------------------------------------------------------
// boot_hw.cpp -- the ARM primitives the ROM's own boot chain reaches, and that
// no host can assemble.
//
// WHY THIS FILE EXISTS. port/slice_gate214.txt puts the ROM's pre-main OS
// bring-up and the runnable half of func_0201a054 into the binary, so the
// game's own boot functions now call each other for real instead of a host
// file standing in for the whole chain. The leaves of that chain are ARM
// assembly primitives: CP15 coprocessor writes, CPSR mode toggles, `swp`,
// literal-pool veneers, and the MSL formatter's 0x1360-byte hand-written DFA.
// Each src/ TU for those carries a HAND-ASM banner saying there is no C to
// decompile it to, and MSVC cannot assemble ARM, so the port keeps a host
// definition. Every one below carries a PORT_HOST_ABI: tag with the real
// reason -- these are exceptions in port/tools/linkage.py's sense, they do NOT
// raise the linked count, and they are here so the remaining gap is honest.
//
// NOTHING IN THIS FILE IS A SHORTCUT. Where the ROM primitive has an exact
// host equivalent (the atomic swap, the bit allocator, the IRQ toggles) it is
// written out in full; where it has none (CP15 cache/MPU, FIQ, the abort
// vector) the body says so in the comment rather than pretending.
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>

#include "ntr/rt.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

extern "C" {

// --- CPSR interrupt masking, all-interrupts form ----------------------------
// src/_ZN3IRQ10DisableAllEv.cpp and src/_ZN3IRQ10RestoreAllEj.cpp are
// `mrs r0, cpsr / orr #0xc0 / msr cpsr_c` -- the I *and* F bits together,
// where hal/cxx_aliases.cpp's IRQ::Disable/Restore pair is the I bit alone.
// The host has one interrupt-mask flag (ntr::rt_irq_*), so both forms land on
// it; the F half has nothing behind it because the port raises no FIQ.
// PORT_HOST_ABI: ARM CPSR mrs/msr on the I+F mask bits, MSVC cannot assemble.
unsigned int _ZN3IRQ10DisableAllEv(void) { return ntr::rt_irq_disable(); }
// PORT_HOST_ABI: ARM CPSR mrs/msr on the I+F mask bits, MSVC cannot assemble.
unsigned int _ZN3IRQ10RestoreAllEj(unsigned int prev) {
    return ntr::rt_irq_restore(prev);
}

// --- CP15 MPU region 7 ------------------------------------------------------
// The protection-unit region that covers DTCM. func_0206d868/func_0206d890
// save and restore it around the debugger's memory window. There is no MPU on
// a Win32 process and nothing the port does depends on the value, so the
// getter reports the ROM's own power-on encoding for region 7 (base 0x027e0000,
// enabled, 16KB: (0x027e0000) | (0x0d << 1) | 1) and the setter drops it. That
// is a stored-and-returned pair rather than a fabricated one: whatever the ROM
// hands the setter comes back out of the getter.
static unsigned int g_mpu_region7 = 0x027e001bu;
// PORT_HOST_ABI: CP15 coprocessor write (mcr p15,0,x,c6,c7,0), no host MPU.
void _ZN4CP1514MPUDataRegion7Ej(unsigned int val) { g_mpu_region7 = val; }
// PORT_HOST_ABI: CP15 coprocessor read (mrc p15,0,v,c6,c7,0), no host MPU.
unsigned int _ZN4CP1517MPUGetDataRegion7Ev(void) { return g_mpu_region7; }

// --- CP15 wait-for-interrupt ------------------------------------------------
// src/func_02057e34.c is the OS idle thread: `IRQ::Enable(); for(;;)
// CP15::WaitForInterrupt();`. It is REFERENCED by the thread-system init
// (func_02058308 hands it to the idle thread record) and never entered on the
// port, because the port runs the game on one fiber and never schedules the
// idle thread -- ARMSaveContext reports "already resumed" and func_02057f54
// backs out without switching (see hal/cxx_aliases.cpp). Halting the host
// thread here would deadlock the frame loop, so this returns; if the idle
// thread is ever really entered the `for(;;)` above it spins and that is a
// louder failure than a hang inside a primitive.
// Since the run link100 THR fold, hal/boot2_thread.cpp defines this primitive
// as the modelled halt (interrupt arrives, dispatcher runs the handler, the
// wake reschedules), because the ROM's idle thread IS entered now that the
// scheduler runs as the ROM wrote it. The empty body that lived here stood in
// only while nothing could reach the idle thread; two definitions collided at
// integration and the modelled one stays.

// --- the trap instruction ---------------------------------------------------
// src/_ZN4cstd14__builtin_trapEv.c is literally `dcd 0xe7ffffff`, the ARM
// undefined instruction MSL uses to abort. cstd::__assert calls it after
// printing. The host equivalent is a breakpoint under a debugger and abort()
// without one -- the same "stop here, this is a bug" contract.
// PORT_HOST_ABI: ARM undefined-instruction trap word, MSVC cannot assemble.
void _ZN4cstd14__builtin_trapEv(void) {
#if defined(_WIN32)
    if (IsDebuggerPresent()) __debugbreak();
#endif
    std::fputs("[boot_hw] cstd::__builtin_trap reached -- ROM assert fired\n", stderr);
    std::fflush(stderr);
    std::abort();
}

// --- the backward LZ decompressor (autoload segments) -----------------------
// src/func_020048d8.c is the CRT0's own decompressor: an `ldmdb` backward walk
// over a packed segment with a rolling bit flag, followed by CP15 cache
// maintenance. Entry sets r0 to the END of the region. On the port the
// autoload segments are never packed -- port_romdata_load lays the image down
// already decompressed out of extracted/arm9_dec.bin, which IS the decompressed
// arm9 -- so the one caller that survives into the binary (func_0205dd9c, the
// backup-media path) has nothing to unpack. Doing nothing is the faithful
// answer for an already-unpacked region; doing something would corrupt it.
// PORT_HOST_ABI: ARM ldmdb backward walk + CP15 cache maintenance, no host form.
void func_020048d8(int end) { (void)end; }

// --- literal-pool veneers ---------------------------------------------------
// `ldr rN, =target; bx rN`. Hand-written because compiled C always picks ip for
// a tail-call veneer; the register choice is what identifies them as assembly.
// The host form is the tail call itself.
int func_02057178(unsigned int arg);
// PORT_HOST_ABI: hand-asm literal-pool veneer (bx through r1, not ip).
void func_02057014(unsigned int arg) { func_02057178(arg); }

// --- the calibrated delay loop ----------------------------------------------
// src/func_02059d8c.c is `subs r0,r0,#4; bhs` -- a cycle-counted busy wait, and
// the `bhs` carry test is why it could not be recovered as C. A host has no
// bus to wait for and burning wall-clock time in the frame loop would be worse
// than useless, so this returns immediately.
// PORT_HOST_ABI: cycle-counted ARM delay loop (bhs on the carry flag).
void func_02059d8c(int cycles) { (void)cycles; }

// --- the atomic swap --------------------------------------------------------
// `swp r0, r0, [r1]` -- ARM's pre-LDREX atomic exchange, which mwccarm had no
// way to spell (it predates C11 _Atomic). Two symbols, same instruction:
// func_0205a74c is the OS lock's, func_0206dab4 the debugger channel's. The
// host equivalent is exact.
static unsigned int host_swp(unsigned int val, void *addr) {
#if defined(_WIN32)
    return (unsigned int)InterlockedExchange((volatile LONG *)addr, (LONG)val);
#else
    unsigned int old = *(volatile unsigned int *)addr;
    *(volatile unsigned int *)addr = val;
    return old;
#endif
}
// PORT_HOST_ABI: ARM `swp` atomic exchange, no mwccarm/MSVC C equivalent.
unsigned int func_0205a74c(unsigned int val, void *addr) { return host_swp(val, addr); }
// PORT_HOST_ABI: ARM `swp` atomic exchange, no mwccarm/MSVC C equivalent.
unsigned int func_0206dab4(unsigned int val, void *addr) { return host_swp(val, addr); }

// --- the dual-entry CPSR toggles (debugger channel) -------------------------
// func_0206d9cc (F bit) and func_0206da18 (I bit) each have TWO entry points
// four bytes apart that fall into one shared body: entering at +0 loads r0=0
// and masks the bit, entering at +8 loads r0=1 and clears it. Both return 1
// when the bit was previously CLEAR (interrupts were on). Only the +0 entries
// are referenced from the TUs this slice seats.
// PORT_HOST_ABI: ARM CPSR mrs/msr, dual entry into one body; MSVC has neither.
int func_0206da18(void) { return ntr::rt_irq_disable() == 0 ? 1 : 0; }
// The FIQ bit. The port raises no FIQ at all, so there is no state to toggle
// and "they were enabled" is the only answer the model can give.
// PORT_HOST_ABI: ARM CPSR F-bit toggle; the host models no FIQ.
int func_0206d9cc(void) { return 1; }
// The SAME two functions under the names the debugger TUs spell them with:
// IRQ::SetIRQState(bool) is func_0206da18 and IRQ::SetFIQState(bool) is
// func_0206d9cc, with the `_false` suffix naming the +0 entry (mask it, report
// whether it had been on) and the bare name the +8 entry (set it to the
// argument). func_0206da28 and func_0206d9dc are those +8 entries by address.
// PORT_HOST_ABI: ARM CPSR mrs/msr, dual entry into one body.
unsigned int _ZN3IRQ11SetIRQStateEb_false(void) {
    return ntr::rt_irq_disable() == 0 ? 1u : 0u;
}
// PORT_HOST_ABI: ARM CPSR F-bit toggle; the host models no FIQ.
unsigned int _ZN3IRQ11SetFIQStateEb_false(void) { return 1u; }
// PORT_HOST_ABI: ARM CPSR mrs/msr, dual entry into one body.
void _ZN3IRQ11SetIRQStateEb(int on) {
    if (on) ntr::rt_irq_enable(); else ntr::rt_irq_disable();
}
// PORT_HOST_ABI: ARM CPSR F-bit toggle; the host models no FIQ.
void _ZN3IRQ11SetFIQStateEb(int on) { (void)on; }
// PORT_HOST_ABI: ARM CPSR mrs/msr, the +8 entry of func_0206da18.
void func_0206da28(int on) { _ZN3IRQ11SetIRQStateEb(on); }
// PORT_HOST_ABI: ARM CPSR F-bit toggle, the +8 entry of func_0206d9cc.
void func_0206d9dc(int on) { (void)on; }

// --- the linker's own symbols ------------------------------------------------
// These are not code and not data: on the DS they are symbols the LINKER
// defines, whose ADDRESS carries the value, and MSVC cannot give a C++ global
// an absolute address. hal/scene_boot.cpp already made this exact ruling for
// overlay_64 / overlay_66 / overlay_100 / overlay_102 -- read the block above
// its definitions, which states the same limitation and the same reason it is
// acceptable. This is the second instance of it, and the exposure here is
// strictly smaller: every reader below sits on the crash-screen and
// overlay-load path (GetSceneOverlayID, func_0201a614, func_0201a754,
// FS_LoadOverlay), and no path the port runs enters it. It is written down
// rather than papered over: `&overlay_6` is NOT 6 here, so if that path is
// ever seated for real, the overlay loader needs a host id map first.
// PORT_HOST_ABI: DS linker symbol whose ADDRESS is the overlay id; MSVC has no
//                absolute-address global. Same ruling as hal/scene_boot.cpp's
//                overlay_64/66/100/102 block.
int overlay_2, overlay_3, overlay_4, overlay_5, overlay_6, overlay_7, overlay_75;
// The end of the arm9 image. src/FS_LoadOverlay.c compares an overlay's
// requested RAM base against `(unsigned)func_020aa420` -- config/arm9's .bss
// ends at exactly 0x020aa420 -- and Crash()es below it. There is no host
// address that means "the end of the DS arm9 image", and the comparison is on
// the same overlay-load path as the ids above.
// PORT_HOST_ABI: DS linker end-of-image symbol used as an address constant.
void func_020aa420(void) {}

// --- the abort / undefined-instruction vector -------------------------------
// src/func_020593f4.c is the CPU exception dispatcher: it reads the processor
// mode out of CPSR, hand-saves registers with stmdb, repoints sp and spins.
// func_02059594 STORES its address into the shared block's exception word
// (0x027ffd9c) and into DTCM at 0x023c3ffc; nothing calls it. On the port the
// host fault probe (PORT_INSTALL_FAULT_PROBE / port_fault_probe_with_file in
// tests/walk_window.cpp) is what actually catches a fault, so this exists only
// to be the address that word holds. Reaching it means a DS-side exception
// vector was really taken, which cannot happen here; say so and stop.
// PORT_HOST_ABI: ARM exception-mode dispatcher (CPSR mode read, manual sp).
void func_020593f4(void) {
    std::fputs("[boot_hw] the ROM exception vector was entered -- impossible on host\n",
               stderr);
    std::fflush(stderr);
    std::abort();
}

// --- the bit allocator over the shared block --------------------------------
// src/func_02057020.c (OS_GetLockID) is hosted in hal/os_lockid.cpp, which
// every binary links (smoke_player's SaveData path needs it too); the seed of
// the two lock words lives beside it. Moved there at integration.

// --- the game-card IREQ_MC fatal path ---------------------------------------
// src/func_020610fc.c is an IPC send loop for command 0xd followed by a
// deliberate `b self` hang: the ROM's answer to the card raising IREQ_MC is to
// tell the ARM7 and then stop the world. IRQ::GameCardIREQMCHandler is the only
// caller, and the port never delivers a card interrupt (ntr models the card
// registers as memory and raises nothing), so this is unreachable. It is not
// silently ignored -- reaching it would mean the model started delivering an
// interrupt it does not implement, which is worth a line and a stop.
// PORT_HOST_ABI: hand-asm IPC send loop with an intentional hang; there is no
//                ARM7 to send to (port/docs/mmio-inventory.md: IPC, 0 modelled).
int func_020610fc(void) {
    std::fputs("[boot_hw] game-card IREQ_MC fatal path reached -- no ARM7 to notify\n",
               stderr);
    std::fflush(stderr);
    std::abort();
}

// --- the MSL printf core ----------------------------------------------------
// src/func_0206a928.c is MSL's 0x1360-byte formatting DFA, kept as assembly
// because of a stack-home wall under C. Its one caller here is func_0206ccd8,
// the ROM's varargs print, which this slice seats only because the debug/assert
// arm of func_0201a5cc reaches it -- no path the port runs calls it. A stub
// that quietly formatted nothing would be a silent wrong answer if that ever
// changed, so it says where it is and returns without writing.
// PORT_HOST_ABI: hand-asm MSL format DFA (0x1360 bytes, stack-home wall).
void func_0206a928(void *state, int first, void *ap, void *end) {
    (void)state; (void)first; (void)ap; (void)end;
    static int said;
    if (!said) {
        said = 1;
        std::fputs("[boot_hw] the ROM's MSL formatter was called; the host does "
                   "not assemble it, so this print produced nothing\n", stderr);
        std::fflush(stderr);
    }
}

// ---------------------------------------------------------------------------
// THIS ONE IS NOT AN ABI EXCEPTION, and deliberately carries no
// PORT_HOST_ABI: tag, so port/tools/linkage.py keeps counting it as a SHADOW --
// work the port still owes rather than a ruling it has made.
//
// src/func_02057198.c is ordinary C with no assembly in it, and it does not
// compile under MSVC for one reason:
//
//     static u32 func_0205a74c(u32 val, LockObj *addr);
//
// func_0205a74c is a GLOBAL ROM symbol (arm9 0x0205a74c, the `swp` primitive
// standing in above), and declaring it `static` in this TU promises a
// definition in this TU that never comes -- C2129, "static function declared
// but not defined". mwccarm accepted it, MSVC does not, and the whole rest of
// the lock family compiles clean. THE FIX IS ONE WORD IN THE DECOMP: `static`
// -> `extern`, which is what every other caller of that symbol already says.
// src/ is not this lane's to edit, so the body is transcribed here from the
// matched TU (verbatim, including the useAll branch) and the TU stays out of
// port/slice_gate214.txt until the declaration is corrected.
//
// It is link-only in this build: the callers are func_02057158 and
// func_020572c8, and the only seated path into them is the debug/fatal chain
// under func_0201a5cc, which nothing the port runs enters.
// ---------------------------------------------------------------------------
unsigned int _ZN3IRQ7DisableEv(void);
unsigned int _ZN3IRQ7RestoreEj(unsigned int);
unsigned int func_02057198(unsigned int val, void *addr, void (*cleanupFn)(void),
                           unsigned int useAll)
{
    unsigned int state = useAll ? _ZN3IRQ10DisableAllEv() : _ZN3IRQ7DisableEv();
    unsigned int old = func_0205a74c(val, addr);
    if (old == 0) {
        if (cleanupFn) cleanupFn();
        *(unsigned short *)((char *)addr + 4) = (unsigned short)val;
    }
    if (useAll) _ZN3IRQ10RestoreAllEj(state);
    else _ZN3IRQ7RestoreEj(state);
    return old;
}

}  // extern "C"

// ---------------------------------------------------------------------------
// NAME BRIDGES, not bodies. Some of the TUs this slice seats are C++ and
// reference these symbols WITHOUT extern "C", so they emit MSVC's mangling
// while the definition carries the ROM's C name. Same story as
// hal/heap_globals.cpp's block, and the same safety rule applies: an
// /alternatename is only sound when the two spellings agree about the calling
// convention. All four functions below are __cdecl free functions on both
// sides (the definitions are extern "C" in ntr/runtime.cpp and above), and
// data has no convention at all. The decorated spellings are copied verbatim
// out of the linker's own error text -- the linker is the authority on
// decoration, not a guess about how a name mangles.
// ---------------------------------------------------------------------------
#pragma comment(linker, "/alternatename:?Disable@IRQ@@YAIXZ=__ZN3IRQ7DisableEv")
#pragma comment(linker, "/alternatename:?Restore@IRQ@@YAXI@Z=__ZN3IRQ7RestoreEj")
#pragma comment(linker, "/alternatename:?DrainWriteBuffer@CP15@@YAXXZ=__ZN4CP1516DrainWriteBufferEv")
#pragma comment(linker, "/alternatename:?FlushAndInvalidateDataCache@CP15@@YAXII@Z=__ZN4CP1527FlushAndInvalidateDataCacheEjj")
#pragma comment(linker, "/alternatename:?data_020868a0@@3DA=_data_020868a0")
#pragma comment(linker, "/alternatename:?data_020868d4@@3DA=_data_020868d4")
#pragma comment(linker, "/alternatename:?data_020a6148@@3PAHA=_data_020a6148")
#pragma comment(linker, "/alternatename:?data_020a612c@@3PAUObj581@@A=_data_020a612c")
