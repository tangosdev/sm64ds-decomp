// THE DS THREAD SCHEDULER, RUNNING AS THE ROM WROTE IT. Run link2, lane THR.
//
// ============================ WHAT CHANGED =================================
//
// port/hal/os_thread.cpp used to carry HOST bodies for OS_SleepThread and
// OS_WakeupThread, and its own header explained why: the ROM's sleep
// reschedules through func_02057f54, which switches the ARM register file with
// ARMSaveContext / ARMRestoreContext, and hal/cxx_aliases.cpp stubbed those two
// out (a save that reports "already resumed" and a restore that traps). With
// the pair hosted, the ROM's own bodies could never link -- and neither could
// the scheduler behind them, nor the OS lock family that sleeps and wakes
// through it.
//
// THE STAND-IN MOVES DOWN ONE LAYER, to where it is unarguable. ARMSaveContext
// and ARMRestoreContext are hand-asm register-file primitives: src/ carries
// them with the HAND-ASM PRIMITIVE banner, they read and write r0-r12/sp/lr,
// the CPSR and the SVC-mode stack pointer, and no host can execute them. They
// are exactly the right place for a host body, and the port already owns the
// mechanism that gives them an honest one:
//
//   port/ntr/include/ntr/rt.h:11-14 -- "the game runs on its own fiber ... The
//   same mechanism generalises to OS_SleepThread / OS_WakeupThread when those
//   are needed."
//
// This file takes that literally. Every DS thread gets a Windows fiber:
//
//   ARMSaveContext(ctx)     bind ctx's thread to the fiber running right now,
//                           record its CPSR I bit, return 0 -- the ROM's
//                           setjmp arm, the one that goes on to switch.
//   ARMRestoreContext(ctx)  restore ctx's CPSR and SwitchToFiber into it. The
//                           thread that called it is suspended INSIDE this
//                           call and resumes here when someone restores it,
//                           which lands it back in func_02057f54 one statement
//                           later -- the same place the ARM path lands, by the
//                           other arm of the same branch.
//
// Nothing else about the ROM's scheduler is replaced. src/OS_SleepThread.c,
// src/OS_WakeupThread.c, src/func_02057f54.c, src/func_0205801c.c and the idle
// thread's own body src/func_02057e34.c run AS WRITTEN, and this file's other
// job is to give them the state they read.
//
// ============================ THE STATE THEY READ ==========================
//
// src/func_02058308.c is the ROM's thread-system init and it is NOT linked
// here. It fills data_020a621c's stack bounds out of DTCM and the ITCM entry
// symbols (func_00000000 / func_00000600 / data_023c0000), none of which the
// host maps, and it fills data_020a6134 through +0x54 -- the run/link100 BOOT
// lane refused it for exactly that reason. So the wiring below is a HOST BOOT
// SEAT, not a transcription of that function: it sets only the fields the
// linked TUs actually read, and it says which line of func_02058308 each one
// comes from.
//
// THE MANAGER, hal/cxx_aliases.cpp's data_020a6134, at the offsets src/ reads:
//
//   +0x00 u16   reschedule-pending          func_02057f54 `s->m0 = 1`
//   +0x02 u16   0x10                        func_02058308
//   +0x04 u16   reschedule-locked           func_02057f54 `if (s->m4 == 0)`
//   +0x06 u16   0
//   +0x08 ptr   CURRENT THREAD              func_02057f54 `data_020a6134.m8`
//   +0x0c ptr   ready-list head             func_0205801c, OS_WakeupThread
//   +0x10 ptr   switch callback (null)      func_02057f54 `s->m10`
//   +0x14..     the 16 thread-id slots      func_02058308 -- NOT hosted here,
//               they are hal/auto_bss.cpp's data_020a6148 and the only TU that
//               reaches them through data_020a6134 is func_02058308 itself.
//
// data_020a6134 was `int[4]` -- 0x10 bytes -- so func_02057f54's read of the
// +0x10 switch callback was four bytes past the end of the object. That is the
// undersized-hosted-global class, and the one existing cxx_aliases.cpp
// definition this lane resizes: `int data_020a6134[5]`. See the report.
//
// data_020a612c is a POINTER TO the current-thread slot, not the slot: the ROM
// spells the read `data_020a612c->node` (src/OS_SleepThread.c) and
// `data_020a612c[0]` (src/func_02057f54.c), and func_02058308's own line is
// `data_020a612c = data_020a613c` with data_020a613c at 0x020a613c, which is
// data_020a6134 + 8. So its value is &manager[+8] and nothing more.
//
// THE TWO THREADS ARE HOST STORAGE, DELIBERATELY. The ROM's are data_020a621c
// (the main thread, 0x15c of span) and data_020a6188 (the idle thread), and
// neither is hosted on this branch -- run/link100's BOOT lane hosts
// data_020a621c on ITS branch and hosting it twice is a duplicate definition.
// Nothing in the linked set names either symbol: func_02058308 is the only TU
// that does, and it is not linked. So the objects live here under host names,
// at the ROM's field offsets, and the day func_02058308 goes in they move.
//
// ============================ THE IDLE THREAD ==============================
//
// A sleep needs somewhere to go. src/func_02057e34.c is the ROM's idle thread
// and it is three lines: enable interrupts, then halt forever on CP15's
// wait-for-interrupt. That is the whole answer to "what runs while the game
// thread is blocked", and it is the ROM's answer, so it is the one used here.
//
// _ZN4CP1516WaitForInterruptEv is an mcr p15,0,r,c7,c0,4 -- it stops the ARM
// core until an IRQ. The host has no core to stop and no exception vector, so
// its body below models the whole hardware sequence that halt is one third of:
// the interrupt arrives, the dispatcher runs the handler, and the handler's
// wake reschedules. In order:
//
//   1. the host frame pump, if a frame loop installed one. This is where the
//      retired host OS_SleepThread's pump loop went -- same callback, same
//      bound, now driven from the ROM's idle thread instead of from a host
//      body standing in for the sleep. hal/comms_conductor.cpp is the caller
//      that installs one.
//   2. the VBlank edge, through the port's own registry. IF bit 0 up, dispatch
//      _ZN3IRQ13GetIRQHandlerEj(1), IF bit 0 down.
//   3. the wake that handler performs. src/_ZN3IRQ13VBlankHandlerEv.c:22 is
//      `OS_WakeupThread(&data_0209d4fc)`, and OS_WakeupThread is ROM code: it
//      clears the queue word, marks every thread whose bit is set runnable and
//      calls func_02057f54, which switches back to the sleeper. This is the
//      round trip the proof measures and both ends of it are the ROM's.
//   4. starvation. A sleeper waiting on a queue only game code can clear would
//      otherwise sit here forever, which the DS would call a hang. Bounded:
//      past the limit, mark every sleeping thread runnable and reschedule.
//      That is the SAME liveness the host stand-in had -- its sleep returned
//      at once -- reached through the ROM's scheduler instead of instead of it,
//      and counted separately so it is visible rather than silent.
//
// WHY STEP 2 IS NULL IN EVERY SHIPPED TARGET TODAY: ntr/runtime.cpp's
// _ZN3IRQ13SetIRQHandlerEjPFvvE stores handlers for mask 0x200000 (GXFIFO) and
// mask 2 (HBlank) and DROPS every other mask, mask 1 included, and
// src/_ZN3IRQ13VBlankHandlerEv.c is in no slice. Both are BOOT's files this
// run; the proposed hunks are in the report. Step 3 does the handler's line 22
// directly meanwhile, which is why it is tagged.
//
// ============================ THREAD CREATION ==============================
//
// (run link100, lane THREAD. This section replaces the THREAD CREATION and THE
// DS STACKS bullets of the WHAT IS NOT MODELLED block that stood here; both
// said the same thing -- func_02058200 was not linked -- and it is now.)
//
// src/func_02058200.c IS THE ROM'S OS_CreateThread AND IT RUNS AS WRITTEN.
// Nothing below hooks it, wraps it or reproduces it: it is on
// port/slice_gate223.txt with func_02058538 (the slot allocator),
// func_020584d0 (the priority insert, already on gate 215) and func_020581a8
// (the exit thunk whose address it stores). Read off extracted/arm9_dec.bin at
// 0x02058200 -- the transcription in src/ is exact, instruction for
// instruction, and every store below is one of that body's:
//
//     0205845c  str sb, [r1, r0, lsl #2]   data_020a6148[id] = self, r1 = the
//                                          pool word 0x020a6148
//     0205846c  str r6, [sb, #0x88]        stack TOP   (the 4th argument)
//     02058474  str r5, [sb, #0x84]        stack BASE  (top - size)
//     0205848c  str r1, [r0, #-4]          0xfddb597d at top - 4
//     02058498  str r2, [r1]               0x7bf9dd5b at base
//     020584a8  bl  0x02058568             OS_InitContext(self, entry, top-4)
//     020584b4  str r7, [sb, #4]           ctx r0 = the entry ARGUMENT
//     020584b8  str r0, [sb, #0x3c]        ctx lr = &func_020581a8, from the
//                                          pool word 0x020581a8
//     020584d0  bl  0x0205e67c             MultiStore_Int(0, base+4, size-8)
//
// WHAT IS REPRODUCED BYTE FOR BYTE, and it is everything the ROM's body
// writes. The record and the stack are BOTH hosted DS storage at the ROM's own
// addresses -- for the card driver's thread that is hal/globals_link100.cpp's
// grouped card span, where data_020a81bc's 1444 bytes are the OSThread record
// AND, in their top kilobyte, the stack data_020a8760 is the top of. So the
// two guard words land on the ROM's own bytes, the MultiStore_Int zero-fill
// covers the ROM's own [base+4, top-8), the slot-table entry is written
// through the ROM's own data_020a6148, and the context block at +0x00..+0x44
// is filled by the ROM's own OS_InitContext arithmetic. The port does not skip
// one store of it, and the ADOPTION below reads those exact words back as its
// admission test rather than trusting a pointer it was handed.
//
// PORT_HOST_ABI: THE STACK IS WRITTEN, NOT EXECUTED ON. The DS thread's 1 KB
//   region is filled exactly as above and is then bookkeeping: the fiber that
//   runs the thread gets a Windows stack of its own, because an x86 frame is
//   not an ARM frame and 1024 bytes of one would be gone before the entry
//   function's prologue finished. This is the SAME exception ARMSaveContext
//   already carries one screen down -- the fiber IS the context, so the ctx
//   block at +0x00..+0x44 is written by the ROM and then not resumed FROM, and
//   the stack at +0x84..+0x88 is written by the ROM and then not run ON. Both
//   halves stay READABLE, which is the part that would have been lost by
//   skipping the stores: src/func_02057f38.c's guard-word writer and
//   src/func_02058158.c's state read both see what the ROM put there.
//
// HOW A CREATED THREAD BECOMES A FIBER -- LAZILY, OFF THE RECORD, AT THE FIRST
// RESTORE. ARMRestoreContext used to refuse a context it had never seen. It
// now tries to ADOPT it first, and the evidence it demands is the ROM's own
// creation, read back out of the record it was handed:
//
//     +0x40  entry pc + 4     func_02058568's add r1,r1,#4 / str r1,[r0,#0x40]
//     +0x04  entry argument   func_02058200's str r7, [sb, #4]
//     +0x3c  &func_020581a8   func_02058200's exit thunk
//     +0x6c  the slot id, and data_020a6148[id] MUST POINT BACK AT THIS RECORD
//     +0x84 / +0x88          stack base and top, and the two GUARD WORDS must
//                            be at base and top-4
//
// A record that fails any of those did not come through func_02058200 and is
// refused exactly as before. A record that passes gets a fiber whose routine
// enters the ROM's entry with the ROM's argument and, when that returns, calls
// the ROM's OWN exit thunk out of +0x3c -- which is func_020581a8: it unhooks
// the thread from data_020a6148, marks it exited, wakes its join queue and
// reschedules. The trampoline's old park-forever tail is kept only as the
// backstop for a record with no thunk in it.
//
// * THE IRQ-EXIT RESCHEDULE. On hardware a wake taken in IRQ mode sets the
//   manager's pending flag and the exception return does the switch;
//   ARMProcessorMode is hosted at 0x1f (system mode) in cxx_aliases.cpp
//   because the host runs every handler as a plain call, so the switch happens
//   inline instead. Same order of events, one frame of the same thread.
//
// ============================ KNOBS ========================================
//
//   SM64DS_THREAD_TRACE=1     one line per switch, halt, wake and creation
//   SM64DS_THREAD_PROOF=1     run the ROM's own sleep once at boot and print a
//                             verdict -- port/tools/thread_proof.py reads it
//   SM64DS_THREAD_NOFIBER=1   the negative control for the SWITCH:
//                             ARMRestoreContext refuses to switch, so the
//                             ROM's sleep cannot reach the idle thread.
//                             Bounded by construction -- the ROM body does not
//                             loop, so the refusal returns.
//   SM64DS_THREAD_NOCREATE=1  the negative control for CREATION: adoption is
//                             refused, so a thread func_02058200 really made
//                             is never entered. port/tools/
//                             thread_create_proof.py reads both arms.

#include <stdint.h>

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ntr/rt.h"
#include "os_thread.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// The manager BSS lives in hal/cxx_aliases.cpp inside the .dsstate capture, so
// a save state rolls the whole thread system back as one object. The pointers
// stored in it address the two thread objects below, which are ordinary host
// statics -- safe across a restore because walk_window and walk_window_hires
// both link /DYNAMICBASE:NO (port/CMakeLists.txt:12985, :13194), so a static's
// address is the same in every run of one binary.
extern "C" {
extern int data_020a612c[4];
extern int data_020a6134[5];
extern int data_020a6128;
// The ROM's sixteen-slot thread table, data_020a6134 + 0x14. It is 64 bytes of
// hal/cxx_aliases.cpp's grouped OSThreadInfo run and src/func_02058538.c scans
// all sixteen; the int[8] this used to declare covered half of it, which was
// harmless while only slots 0 and 1 were ever written and is not now that
// func_02058200 allocates out of the same table.
extern int data_020a6148[16];

// The per-VBlank wake queue, hosted by hal/comms_conductor.cpp as four bytes.
// src/func_0201a4d0.c sleeps on it; src/_ZN3IRQ13VBlankHandlerEv.c:22 wakes it.
extern unsigned char data_0209d4fc[4];
/* THE ROM'S WAIT FLAG, 0x0209d4f0, hosted in hal/boot_globals.cpp. It is up for
   exactly the length of func_020197b8's phase-7 wait -- :53 raises it, :56
   drops it -- which makes it the one word this file can read to tell THE
   FRAME'S OWN WAIT apart from any other wait that reaches this halt.
   src/_ZN3IRQ13VBlankHandlerEv.c:15 reads it for the same reason: the ROM's own
   wake fires only while it is up. Step 4's bound reads it (run link100, lane
   DET). */
extern unsigned char data_0209d4f0[4];
/* THE ROM'S FRAME DIVIDER, read by step 4's bound below and by nothing else in
   this file. 0x0208ee44, hosted in hal/auto_bss.cpp and written by
   Stage::InitResources (hal/level_boot.cpp:5174 records the value: 2 for a 3D
   level, which is how a 60 Hz VBlank becomes a 30 Hz game tick).
   src/_ZN3IRQ13VBlankHandlerEv.c:15 is the reader that matters:
       if (data_0209d514 >= data_0208ee44 && data_0209d4f0 != 0)
           OS_WakeupThread(&data_0209d500);
   so the sleeper at phase 7 needs THAT MANY VBlank edges before the ROM's own
   wake fires, and one edge is delivered per turn of the idle loop. */
extern int data_0208ee44;

// ROM code this file calls. Every one of these is a matched TU on
// port/slice_gate2thr.txt or already on port/slice_gate10.txt.
void func_02057f54(void);                 // the reschedule
void OS_WakeupThread(uint16_t *q);        // the wake
void func_02057e34(void);                 // the idle thread's body
void func_0201a4d0(void);                 // OS_SleepThread(data_0209d4fc)

// The port's IRQ registry (ntr/runtime.cpp).
void *_ZN3IRQ13GetIRQHandlerEj(unsigned mask);

// The two hand-asm context primitives this file replaces, and the CP15 halt.
int ARMSaveContext(void *ctx);
void ARMRestoreContext(void *ctx);
void _ZN4CP1516WaitForInterruptEv(void);
}

namespace {

// ---------------------------------------------------------------------------
// The ROM's OSThread, at the offsets src/ reads. func_02058308 initialises
// through +0x90, so that is the size.
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct RomThread {
    // The ARM context block. src/func_02058568.c (OS_InitContext, the hand-asm
    // primitive whose host body is hal/scene_vs_menu.cpp) fills it and
    // src/func_02058200.c writes two more words into it; the adoption below
    // reads exactly those, because they are the only record of what thread the
    // ROM meant to make. The REGISTER FILE half is meaningless on a host --
    // the fiber is what carries the context.
    uint32_t cpsr;            // +0x00  0x1f, or 0x3f for a Thumb entry
    uint32_t r0;              // +0x04  the entry ARGUMENT (func_02058200)
    unsigned char r1_r12[0x38 - 0x08];
    uint32_t sp;              // +0x38  sp_svc - 0x40
    uint32_t lr;              // +0x3c  &func_020581a8, the exit thunk
    uint32_t pc4;             // +0x40  entry + 4
    uint32_t sp_svc;          // +0x44  stack top - 4
    unsigned char ctx_tail[0x64 - 0x48];
    uint32_t state;           // +0x64  1 runnable, 0 sleeping, 2 exited
    RomThread *next;          // +0x68  ready-list link
    uint32_t id;              // +0x6c  queue bit index, `1 << id`
    uint32_t prio;            // +0x70  ascending; func_020584d0 sorts on it
    uint32_t f74;             // +0x74
    uint32_t f78, f7c, f80;   // +0x78
    uint32_t stack_lo;        // +0x84
    uint32_t stack_hi;        // +0x88
    uint32_t f8c;             // +0x8c
    uint16_t joinq;           // +0x90  func_0205816c sleeps on this
    uint16_t f92;             // +0x92
};
#pragma pack(pop)

// The offsets above are the ones src/func_02058200.c and src/func_02058568.c
// write and the ones the adoption reads back, so they are checked rather than
// commented. #pragma pack(1) is what makes them hold on a 32-bit host.
static_assert(offsetof(RomThread, r0) == 0x04, "RomThread r0");
static_assert(offsetof(RomThread, lr) == 0x3c, "RomThread lr");
static_assert(offsetof(RomThread, pc4) == 0x40, "RomThread pc4");
static_assert(offsetof(RomThread, sp_svc) == 0x44, "RomThread sp_svc");
static_assert(offsetof(RomThread, state) == 0x64, "RomThread state");
static_assert(offsetof(RomThread, next) == 0x68, "RomThread next");
static_assert(offsetof(RomThread, id) == 0x6c, "RomThread id");
static_assert(offsetof(RomThread, prio) == 0x70, "RomThread prio");
static_assert(offsetof(RomThread, stack_lo) == 0x84, "RomThread stack_lo");
static_assert(offsetof(RomThread, stack_hi) == 0x88, "RomThread stack_hi");
static_assert(offsetof(RomThread, joinq) == 0x90, "RomThread joinq");

inline unsigned char *mgr() { return reinterpret_cast<unsigned char *>(data_020a6134); }
inline uint16_t &mgr_u16(unsigned off) { return *reinterpret_cast<uint16_t *>(mgr() + off); }
inline RomThread *&mgr_current() { return *reinterpret_cast<RomThread **>(mgr() + 8); }
inline RomThread *&mgr_head() { return *reinterpret_cast<RomThread **>(mgr() + 0xc); }
inline void *&mgr_switch_cb() { return *reinterpret_cast<void **>(mgr() + 0x10); }
inline uint16_t &vblank_queue() { return *reinterpret_cast<uint16_t *>(data_0209d4fc); }

// ---------------------------------------------------------------------------
// The two threads. Host storage on purpose -- see the header.
// ---------------------------------------------------------------------------
RomThread g_main;   // the port's one game context: id 0, priority 0x10
RomThread g_idle;   // src/func_02057e34.c:        id 1, priority 0x20

// One fiber per thread. SIXTEEN, which is the ROM's own cap and not a guess:
// src/func_02058538.c scans data_020a6134 + 0x14 for a free slot out of
// exactly sixteen and returns -1 when they are all taken, so a seventeenth
// thread cannot exist. Slots 0 and 1 are the two seated below (the game
// context and the idle thread); func_02058538 hands the first creation 2.
struct FiberSlot {
    RomThread *thread;
    void *fiber;
    void (*entry)();
    uint32_t arg;       // the ROM's entry argument, record +0x04
    void (*exitfn)();   // the ROM's exit thunk, record +0x3c
    uint32_t cpsr;      // the CPSR I bit this thread was last suspended with
    bool bound;         // fiber is the host fiber this thread already runs on
    bool created;       // came in through func_02058200, not the boot seat
};
FiberSlot g_slots[16];

RomThread *g_running;             // whose fiber is executing right now
bool g_booted;
unsigned long g_owner_tid;        // the OS thread the fibers belong to
unsigned g_starve;                // consecutive halts with nobody woken

// rung R3b step B2: the HOST FRAME pump, distinct from the wireless one in
// hal/os_thread.cpp. Null until rung R3d installs it; see step 1b below.
int (*g_host_frame_pump)(unsigned);

// THE RADIO DOES NOT TAKE A TURN INSIDE A RE-SIMULATED FRAME (run link100, lane
// DET). Nonzero while hal/rollback.cpp is replaying a frame the window has
// already played; tests/walk_window.cpp raises it around the ROM's phase-7
// sleep and drops it after. See step 1 of the halt below for the whole reason.
int g_pump_suspended;

// The VBlank edges THIS frame's wait has been given (run link100, lane DET).
// Reset by port_thread_frame_wait_begin, stepped by step 2 of the halt, read by
// step 4's bound. Per-wait, which is the whole point: g_starve is not.
unsigned g_frame_wait_edges;

// NONZERO WHILE STEP 2 OF THE HALT IS INSIDE IRQ::VBlankHandler (run link100,
// lane DET2, rung 2). A DS is in IRQ mode for the whole of that handler, and
// src/func_02057f54.c:24-28 is the ROM's own statement of what that means: the
// reschedule sees ARMProcessorMode() == 0x12, raises the manager's pending
// flag, and RETURNS -- the thread switch happens when the IRQ returns, not
// from inside the handler. See ARMRestoreContext and step 2's IRQ return.
unsigned g_in_vblank_handler;

struct Stats {
    unsigned long long saves, restores, resumes, refused, unknown_ctx;
    unsigned long long halts, pump_turns, vblank_dispatches, vblank_wakes;
    // ENTERED vs DISPATCHED (run link100, lane R3E). vblank_dispatches is
    // incremented after the handler returns and the handler does not always
    // return: its wake branch reschedules from inside OS_WakeupThread, which
    // leaves the idle fiber parked one statement above func_02019144. This
    // counter is taken before the call, so entries-minus-dispatches is exactly
    // the number of parked handlers, and dispatches is exactly the number of
    // times the ROM's VBlank display commit ran to the end.
    unsigned long long vblank_enters;
    unsigned long long frame_pump_turns;   // rung R3b step B2, step 1b
    unsigned long long starved, wrong_thread, idle_sleeps;
    unsigned long long adopted, entered, exited, rejected, nocreate;
    // Switches ARMRestoreContext handed to the IRQ return instead of taking
    // from inside the handler (run link100, lane DET2, rung 2). Printed LAST
    // on the [thr] line so every existing reader of that line keeps working.
    unsigned long long deferred;
} g_stat;

constexpr size_t kFiberStack = 256 * 1024;

bool trace_on() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_THREAD_TRACE") ? 1 : 0;
    return v != 0;
}
bool nofiber_on() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_THREAD_NOFIBER") ? 1 : 0;
    return v != 0;
}
bool nocreate_on() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_THREAD_NOCREATE") ? 1 : 0;
    return v != 0;
}

void trace(const char *fmt, ...) {
    if (!trace_on()) return;
    va_list ap;
    va_start(ap, fmt);
    std::fprintf(stderr, "[thr] ");
    std::vfprintf(stderr, fmt, ap);
    std::fprintf(stderr, "\n");
    va_end(ap);
    std::fflush(stderr);
}

FiberSlot *slot_of(RomThread *t) {
    if (!t) return nullptr;
    for (FiberSlot &s : g_slots)
        if (s.thread == t) return &s;
    return nullptr;
}

FiberSlot *slot_seat(RomThread *t, void (*entry)()) {
    for (FiberSlot &s : g_slots) {
        if (s.thread == nullptr) {
            s.thread = t;
            s.fiber = nullptr;
            s.entry = entry;
            s.arg = 0;
            s.exitfn = nullptr;
            s.cpsr = 0;
            s.bound = false;
            s.created = false;
            return &s;
        }
    }
    return nullptr;
}

// Become a fiber, if we are not one already. rt_run may have done it (it calls
// ConvertThreadToFiber itself) or nothing may have; either way GetCurrentFiber
// is the handle afterwards. ConvertFiberToThread is never called: undoing the
// conversion under a suspended game fiber would strand it.
void *host_current_fiber() {
#if defined(_WIN32)
    void *f = ConvertThreadToFiber(nullptr);
    if (f) return f;
    // Already one -- rt_run converts the thread itself. Asking the error code
    // rather than IsThreadAFiber keeps this off a _WIN32_WINNT floor.
    if (GetLastError() == ERROR_ALREADY_FIBER) return GetCurrentFiber();
    return nullptr;
#else
    return nullptr;
#endif
}

#if defined(_WIN32)
void CALLBACK thread_trampoline(void *p) {
    FiberSlot *s = static_cast<FiberSlot *>(p);
    ++g_stat.entered;
    trace("enter thread id=%u entry=%p arg=%08x exit=%p", s->thread->id,
          (void *)s->entry, s->arg, (void *)s->exitfn);
    if (s->entry) {
        // THE ROM'S ARGUMENT, THROUGH THE ROM'S REGISTER. func_02058200 puts
        // the caller's third parameter in the context block's r0 word, which
        // ARMRestoreContext's `ldmia r0, {r0-r12}^` would load; a __cdecl host
        // callee finds the same value as its first stack argument, and a
        // callee declared void ignores it with the caller cleaning up. Both of
        // this build's thread bodies (src/func_020602bc.cpp, the card driver's
        // service loop, and src/func_02057e34.c, the idle loop) take none and
        // the ROM passes 0 to both.
        reinterpret_cast<void (*)(uint32_t)>(s->entry)(s->arg);
    }
    // A DS thread body that returns lands in the thunk func_02058200 stored in
    // the record's lr word, which is func_020581a8 (OS_ExitThread): it unhooks
    // the thread from data_020a6148, marks it exited, wakes its join queue and
    // reschedules. Run the ROM's, out of the record, rather than imitating it.
    ++g_stat.exited;
    trace("thread id=%u returned; exit thunk %p", s->thread->id,
          (void *)s->exitfn);
    if (s->exitfn) s->exitfn();
    // A fiber routine that RETURNS ends the OS thread, and the ROM's own thunk
    // never comes back (it reschedules off an exited thread). This is the
    // backstop for a record with no thunk in it -- the two boot seats below,
    // whose bodies are for(;;) and cannot reach here anyway.
    trace("thread id=%u past its exit thunk; parking", s->thread->id);
    for (;;) {
        s->thread->state = 2;
        func_02057f54();
    }
}

// ---------------------------------------------------------------------------
// ADOPTION: turn a record src/func_02058200.c really created into a fiber.
//
// The header block lists the six words this reads and where each is written.
// Every one of them is checked rather than assumed, because the alternative to
// checking is CreateFiber on a garbage entry point: this runs on whatever
// pointer the ROM's scheduler picked out of its ready list, and the whole
// reason ARMRestoreContext refused unknown contexts before this lane was that
// it had no way to tell a created thread from a stray.
// ---------------------------------------------------------------------------
FiberSlot *adopt_created(RomThread *t) {
    if (nocreate_on()) {
        // THE NEGATIVE CONTROL FOR CREATION. Refuse the adoption and say so,
        // which is exactly the behaviour this file had before the lane: the
        // reschedule backs out and the caller carries on. Bounded for the same
        // reason -- nothing in the refusal path loops.
        ++g_stat.nocreate;
        std::fprintf(stderr, "[thr] SM64DS_THREAD_NOCREATE=1: refusing to "
                     "adopt created thread %p\n", (void *)t);
        std::fflush(stderr);
        return nullptr;
    }

    const uint32_t id = t->id;
    const uint32_t entry4 = t->pc4;
    const uint32_t lo = t->stack_lo;
    const uint32_t hi = t->stack_hi;

    const char *why = nullptr;
    if (id >= 16)
        why = "slot id out of the ROM's sixteen";
    else if (data_020a6148[id] != static_cast<int>(reinterpret_cast<intptr_t>(t)))
        why = "data_020a6148[id] does not point back at this record "
              "(func_02058200 never allocated it)";
    else if (entry4 < 8)
        why = "context +0x40 holds no entry pc (func_02058568 never ran)";
    else if (lo == 0 || hi == 0 || hi <= lo)
        why = "stack bounds at +0x84/+0x88 are not a range";
    else if (*reinterpret_cast<const uint32_t *>(
                 static_cast<uintptr_t>(lo)) != 0x7bf9dd5bu)
        why = "no 0x7bf9dd5b guard word at the stack base";
    else if (*reinterpret_cast<const uint32_t *>(
                 static_cast<uintptr_t>(hi - 4)) != 0xfddb597du)
        why = "no 0xfddb597d guard word at the stack top";

    if (why) {
        ++g_stat.rejected;
        std::fprintf(stderr, "[thr] ARMRestoreContext: refusing thread %p -- "
                     "%s\n", (void *)t, why);
        std::fflush(stderr);
        return nullptr;
    }

    FiberSlot *s = slot_seat(t, reinterpret_cast<void (*)()>(
                                    static_cast<uintptr_t>(entry4 - 4)));
    if (!s) {
        ++g_stat.rejected;
        std::fprintf(stderr, "[thr] ARMRestoreContext: no free fiber slot for "
                     "thread %p (all sixteen taken)\n", (void *)t);
        std::fflush(stderr);
        return nullptr;
    }
    s->arg = t->r0;
    s->exitfn = reinterpret_cast<void (*)()>(static_cast<uintptr_t>(t->lr));
    s->created = true;
    s->cpsr = (t->cpsr & 0x80u);
    ++g_stat.adopted;
    trace("adopt created thread id=%u rec=%p entry=%08x arg=%08x exit=%08x "
          "stack=[%08x,%08x) prio=%u",
          id, (void *)t, entry4 - 4, t->r0, t->lr, lo, hi, t->prio);
    return s;
}
#endif

// ---------------------------------------------------------------------------
// THE HOST BOOT SEAT for the ROM thread manager. See the header for where each
// field comes from in src/func_02058308.c.
// ---------------------------------------------------------------------------
void thread_boot() {
    if (g_booted) return;
    g_booted = true;
#if defined(_WIN32)
    g_owner_tid = GetCurrentThreadId();
#endif

    std::memset(&g_main, 0, sizeof g_main);
    std::memset(&g_idle, 0, sizeof g_idle);

    // func_02058308: the main thread is runnable, id 0, priority 0x10.
    g_main.state = 1;
    g_main.id = 0;
    g_main.prio = 0x10;
    g_main.next = &g_idle;

    // func_02058308's tail: func_02058200(data_020a6188, func_02057e34, ...)
    // then priority 0x20 and runnable. The id is the first free slot, 1.
    g_idle.state = 1;
    g_idle.id = 1;
    g_idle.prio = 0x20;
    g_idle.next = nullptr;

    mgr_u16(0) = 0;        // no reschedule pending
    mgr_u16(2) = 0x10;     // func_02058308
    mgr_u16(4) = 0;        // rescheduling not locked -- func_02057f54's gate
    mgr_u16(6) = 0;
    mgr_current() = &g_main;
    mgr_head() = &g_main;  // ordered by priority: main (0x10) then idle (0x20)
    mgr_switch_cb() = nullptr;

    // `data_020a612c = data_020a613c`, i.e. &manager[+8].
    data_020a612c[0] = static_cast<int>(reinterpret_cast<intptr_t>(mgr() + 8));
    data_020a6128 = 0;     // the global switch callback func_02057f54 may call

    data_020a6148[0] = static_cast<int>(reinterpret_cast<intptr_t>(&g_main));
    data_020a6148[1] = static_cast<int>(reinterpret_cast<intptr_t>(&g_idle));

    slot_seat(&g_main, nullptr);          // its fiber is whatever is running
    slot_seat(&g_idle, func_02057e34);    // its fiber is made on first restore

    g_running = &g_main;
    g_starve = 0;

    trace("boot main=%p id=0 prio=0x10 | idle=%p id=1 prio=0x20 "
          "entry=func_02057e34 | mgr=%p current=+8 head=+0xc",
          (void *)&g_main, (void *)&g_idle, (void *)mgr());
}

// The starvation wake. Nothing on the host is going to clear this sleeper's
// queue word, so give it the ONE thing OS_WakeupThread would have given it --
// runnable -- and let the ROM's own reschedule take it from there. The queue
// word is deliberately left alone: this file does not know which word the
// sleeper is on, and inventing one would be a lie the caller's own re-test
// would then act on.
// RUNG E1 (run link100, lane R3E). Default: narrow. SM64DS_R3E_WIDE_STARVE=1
// restores the sweep on the same binary; see this file's starve_wake note.
static bool wide_starve() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_R3E_WIDE_STARVE");
        v = (e && *e && !(e[0] == '0' && e[1] == '\0')) ? 1 : 0;
    }
    return v != 0;
}

// Run link100, lane DET. ON by default; SM64DS_DET_FRAMEWAIT=0 puts the old
// reading back on the same binary, which is what makes the A/B one run.
// See step 4 of the halt for what it gates and why.
static bool det_framewait() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_DET_FRAMEWAIT");
        v = (e && e[0] == '0' && e[1] == '\0') ? 0 : 1;
    }
    return v != 0;
}

// Run link100, lane DET. ON by default; SM64DS_DET_VBLFRAME=0 puts the
// once-per-halt edge back on the same binary. See step 2 of the halt.
static bool det_vblframe() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_DET_VBLFRAME");
        v = (e && e[0] == '0' && e[1] == '\0') ? 0 : 1;
    }
    return v != 0;
}

// Run link100, lane DET2, rung 2. ON by default; SM64DS_DET2_IRQDEFER=0 puts
// the switch-from-inside-the-wake back on the same binary. See ARMRestoreContext
// and the IRQ return at the end of step 2 of the halt.
static bool det2_irqdefer() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_DET2_IRQDEFER");
        v = (e && e[0] == '0' && e[1] == '\0') ? 0 : 1;
    }
    return v != 0;
}

// Run link100, lane DET2, rung 1. ON by default; SM64DS_DET2_OWED1=0 puts the
// exact-divider frame-wait bound back on the same binary. See step 4.
static bool det2_owed1() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_DET2_OWED1");
        v = (e && e[0] == '0' && e[1] == '\0') ? 0 : 1;
    }
    return v != 0;
}

void starve_wake() {
    ++g_stat.starved;
    bool any = false;
    // THE SWEEP IS THE WIRELESS WAIT'S, NOT THE FRAME'S (rung E1, lane R3E).
    // Marking EVERY sleeping thread runnable wakes threads the cartridge
    // leaves asleep. On the frame path that is thread 2, func_020602bc -- the
    // ROM's file-request thread, adopted at boot and asleep ever since -- and
    // once phase 7 is the ROM's own sleep it is resumed once per frame.
    // Measured (lane R3E gate 1, SM64DS_ROM_LOOP=1): the run faults at frame
    // 31 with Memory::defaultHeapPtr null, inside Player::SetAnim's
    // SharedFilePtr::Release. Forward progress needs only the line below this
    // block, which is what this function's own banner says; the sweep stays
    // for a session that installed a wireless pump, because that wait is what
    // it was written for.
    if (wide_starve() || port::thread_pump()) {
        for (RomThread *t = mgr_head(); t; t = t->next) {
            if (t->state == 0) {
                t->state = 1;
                any = true;
            }
        }
    }
    // FORWARD PROGRESS IS NOT OPTIONAL. The idle thread's body is a for(;;),
    // so a halt that returns without a runnable game context is the one hang
    // this file could produce. It cannot: the game context is runnable after a
    // starvation wake whatever the list said, and the ROM's own reschedule is
    // what puts control back on it (func_02057f54 returns without switching if
    // the current thread is already the pick, which is the case when the halt
    // was reached from the game context rather than from the idle thread).
    g_main.state = 1;
    trace("starve: woke=%d after %u halts", (int)any, g_starve);
    func_02057f54();
}

}  // namespace

// ===========================================================================
// THE THREE HOST PRIMITIVES
// ===========================================================================

extern "C" {

// PORT_HOST_ABI: hand-asm ARM register-file save (src/ARMSaveContext.c carries
//   the HAND-ASM PRIMITIVE banner): mrs cpsr, an stmia of r0-r12/sp/lr, the
//   SVC-mode sp read under `msr cpsr_c, #0xd3`, and the resume PC. MSVC cannot
//   assemble it and a host has no ARM register file to save. The fiber IS the
//   context here: this binds the calling thread to the fiber it is running on
//   and records the CPSR I bit, which is the one architectural bit the ROM's
//   scheduler observes (func_02057e34 enables interrupts for the idle thread
//   only). Returns 0 -- the ROM's "saved, now switch" arm. The "resumed"
//   return of 1 has no host counterpart because a fiber resumes inside
//   ARMRestoreContext instead, one statement further down the same function.
int ARMSaveContext(void *ctx) {
    thread_boot();
    RomThread *t = static_cast<RomThread *>(ctx);
    FiberSlot *s = slot_of(t);
    if (!s) {
        ++g_stat.unknown_ctx;
        std::fprintf(stderr, "[thr] ARMSaveContext: unknown thread %p -- "
                     "thread creation (func_02058200) is not linked; refusing\n",
                     ctx);
        std::fflush(stderr);
        return 1;                       // back the reschedule out, do not guess
    }
#if defined(_WIN32)
    // func_02057f54 only ever saves the thread it is running on, so the fiber
    // executing right now IS this thread's context. Asking the OS rather than
    // trusting the slot also catches the one way the two could disagree.
    void *f = host_current_fiber();
    if (!f) {
        std::fprintf(stderr, "[thr] ARMSaveContext: ConvertThreadToFiber "
                     "failed; refusing to switch\n");
        std::fflush(stderr);
        return 1;
    }
    if (s->fiber && s->fiber != f) {
        std::fprintf(stderr, "[thr] ARMSaveContext: thread %u is on fiber %p "
                     "but its slot says %p; refusing\n", t->id, f, s->fiber);
        std::fflush(stderr);
        return 1;
    }
    s->fiber = f;
    s->bound = true;
#endif
    s->cpsr = ntr::rt_irq_masked() ? 0x80u : 0u;
    ++g_stat.saves;
    return 0;
}

// PORT_HOST_ABI: hand-asm ARM register-file restore (src/ARMRestoreContext.c
//   carries the HAND-ASM PRIMITIVE banner): `msr spsr_fsxc`, an ldmia with the
//   `^` user-bank suffix and `subs pc, lr, #4`. There is no host instruction
//   for any of it. The fiber switch below is the same act: the target's CPSR I
//   bit is restored, control moves to its stack, and the thread that called
//   this resumes here -- which is where the ARM path resumes too, by the other
//   arm of func_02057f54's ARMSaveContext branch.
void ARMRestoreContext(void *ctx) {
    thread_boot();
    RomThread *to = static_cast<RomThread *>(ctx);
    RomThread *from = g_running;
    FiberSlot *ts = slot_of(to);
#if defined(_WIN32)
    // A context this file has not seen is a thread src/func_02058200.c made
    // while nobody was watching, which is the ordinary case now: the card
    // driver's own bring-up creates one. Adopt it off its record, or refuse it
    // for a named reason.
    if (!ts) ts = adopt_created(to);
#endif
    if (!ts) {
        ++g_stat.unknown_ctx;
        std::fprintf(stderr, "[thr] ARMRestoreContext: thread %p has no fiber "
                     "and could not be adopted; refusing\n", ctx);
        std::fflush(stderr);
        return;
    }
    if (nofiber_on()) {
        // THE NEGATIVE CONTROL. Refuse the switch and say so. Bounded by
        // construction: src/OS_SleepThread.c does not loop, so it returns and
        // its caller re-tests, which is the port's behaviour before this lane.
        ++g_stat.refused;
        trace("SM64DS_THREAD_NOFIBER=1: refusing %u -> %u",
              from ? from->id : 0u, to->id);
        return;
    }
    // A DS SWITCHES THREADS ON IRQ RETURN, NOT FROM INSIDE THE HANDLER (run
    // link100, lane DET2, rung 2).
    //
    // src/_ZN3IRQ13VBlankHandlerEv.c is five statements and the wake is the
    // second of them:
    //     data_0209d514 = data_0209d514 + 1;
    //     if (data_0209d514 >= data_0208ee44 && data_0209d4f0 != 0) {
    //         OS_WakeupThread(&data_0209d500);
    //         data_0209d514 = 0;          <- the frame boundary's own zero
    //         func_02019144();            <- the display commit
    //     }
    //     OS_WakeupThread(&data_0209d4fc);
    //     ... |= 1; func_02019100();
    // On the cartridge all five run, in one go, before the interrupt returns:
    // OS_WakeupThread reaches src/func_02057f54.c and its FIRST test is
    //     if (s->m4 == 0) { if (ARMProcessorMode() != 0x12) goto cont; }
    //     s->m0 = 1; return;
    // -- in IRQ mode (0x12) the reschedule raises the manager's pending flag
    // and returns, and the IRQ return path is what performs the switch.
    //
    // This port's ARMProcessorMode is hosted at 0x1f (system mode) in
    // hal/cxx_aliases.cpp, which this lane does not own, so func_02057f54 runs
    // all the way through and its ARMRestoreContext -- this function -- took
    // the switch from inside the wake. The handler was then left parked one
    // statement short of `data_0209d514 = 0` and finished on the next halt, so
    // the ROM's VBlank count read the DIVIDER at every frame boundary instead
    // of zero (measured, lane DET's [rb-det] words line: data_0209d514=2 with
    // data_0208ee44=2, straight run and re-run alike) and func_02019144's
    // display commit ran at the head of the following frame.
    //
    // The deferral is taken here instead, one call later than the ROM takes
    // it, and it puts the manager into exactly the state the ROM's own early
    // return leaves it in: func_02057f54.c:37 has already written the
    // current-thread word, which the ROM never reaches in IRQ mode, so put it
    // back; and raise m0, which the ROM raises instead. Step 2 of the halt
    // clears m0 and calls func_02057f54 again when the handler returns, which
    // is the IRQ return. Nothing in src/ reads or writes m0 -- every other
    // data_020a6134 access in the linked set is at +2, +8, +0xc, +0x10, +0x14
    // or the slot array -- so the reader this stands in for is the ROM's IRQ
    // return path, exactly as step 2 already stands in for the dispatcher.
    //
    // SM64DS_DET2_IRQDEFER=0 puts the switch-from-inside back on the same
    // binary.
    if (g_in_vblank_handler && det2_irqdefer()) {
        ++g_stat.deferred;
        mgr_current() = from;
        mgr_u16(0) = 1;
        trace("defer %u -> %u to the IRQ return",
              from ? from->id : 0u, to->id);
        return;
    }
#if defined(_WIN32)
    if (GetCurrentThreadId() != g_owner_tid) {
        ++g_stat.wrong_thread;
        std::fprintf(stderr, "[thr] ARMRestoreContext from OS thread %lu, the "
                     "fibers belong to %lu; refusing\n",
                     GetCurrentThreadId(), g_owner_tid);
        std::fflush(stderr);
        return;
    }
    if (!ts->fiber) {
        if (!ts->entry) {
            ++g_stat.unknown_ctx;
            std::fprintf(stderr, "[thr] ARMRestoreContext: thread %u has never "
                         "run and has no entry point; refusing\n", to->id);
            std::fflush(stderr);
            return;
        }
        ts->fiber = CreateFiber(kFiberStack, thread_trampoline, ts);
        if (!ts->fiber) {
            std::fprintf(stderr, "[thr] CreateFiber failed for thread %u\n",
                         to->id);
            std::fflush(stderr);
            return;
        }
        ts->bound = true;
        trace("created fiber %p for thread %u (stack %u KiB)",
              ts->fiber, to->id, (unsigned)(kFiberStack / 1024));
    }
    trace("switch %u -> %u  (%s)  d4fc=%04x",
          from ? from->id : 0u, to->id,
          (from && from->state == 0) ? "sleep" : "wake",
          (unsigned)vblank_queue());
    ++g_stat.restores;
    g_running = to;
    ntr::rt_irq_restore(ts->cpsr);
    SwitchToFiber(ts->fiber);
    // --- resumed: somebody restored `from` ---------------------------------
    g_running = from;
    if (FiberSlot *fs = slot_of(from)) ntr::rt_irq_restore(fs->cpsr);
    ++g_stat.resumes;
    trace("resume %u", from ? from->id : 0u);
#else
    (void)from;
#endif
}

// PORT_HOST_ABI: CP15 `mcr p15,0,r,c7,c0,4` -- wait for interrupt, which stops
//   the ARM core until an IRQ arrives. The host has no core to stop and no
//   exception vector, so this models the whole hardware sequence the halt is
//   one third of: the interrupt arrives, the dispatcher runs the handler, the
//   handler's wake reschedules. Step 3 stands in for
//   src/_ZN3IRQ13VBlankHandlerEv.c:22 because ntr/runtime.cpp's SetIRQHandler
//   drops mask 1 and that handler is in no slice -- proposed hunks in the
//   lane report. Step 4 is the bound: it cannot spin forever.
void _ZN4CP1516WaitForInterruptEv(void) {
    ++g_stat.halts;

    // 1. the host frame pump, where the retired host sleep's pump loop went.
    //
    //    AND IT DOES NOT RUN INSIDE A RE-SIMULATED FRAME (run link100, lane
    //    DET). hal/comms_conductor.cpp's conductor_pump is the RADIO: every
    //    turn of it polls the transport and gives the host ARM7 a turn
    //    (comms_arm7_turn), and an ARM7 turn POSTS A QUEUED WM REPLY, which the
    //    ROM answers by sending the next WM command -- so the ROM's own command
    //    queue at data_020a89b0 walks its ten slots once per turn. On a
    //    connected session that same pump then spends up to one VBlank of WALL
    //    TIME polling for a datagram (lane VS7's fix, which is right for a real
    //    frame). Neither belongs in a frame the window has ALREADY PLAYED and
    //    is only re-running to fold in a corrected input: the radio turns of
    //    that frame happened once, in real time, and re-running them is host
    //    I/O inside the simulation. That is the same class as the rasteriser
    //    and the present, which hal/rollback.cpp already stands down for a
    //    replayed frame, and it is what made the DET rung's restore+retick read
    //    DSSTATE-DIFFERS on data_020a89b0+0xc once rung H2 made phase 7 the
    //    ROM's own sleep: measured, 6 of 6 windows, every repetition.
    //
    //    With the pump suspended step 4's bound below falls back to the ROM's
    //    own divider, so a replayed frame's phase-7 sleep ends on
    //    IRQ::VBlankHandler's own wake -- the DS's own frame end -- rather than
    //    on this port's starvation wake at a wall-clock boundary.
    bool pump_stop = false;
    if (!g_pump_suspended) {
        if (port::ThreadPump p = port::thread_pump()) {
            ++g_stat.pump_turns;
            if (!p(g_starve)) pump_stop = true;
        }
    }

    // 1b. THE HOST FRAME'S OWN DUTIES (run link100, boot plan rung R3b).
    //
    //   The pump above is the WIRELESS one: hal/comms_conductor.cpp installs it
    //   for the duration of a radio wait and nothing else does. This second
    //   one is the FRAME's -- the pacer today, and whatever else rung R3b's
    //   later steps move out of tests/walk_window.cpp's loop body -- and it is
    //   here because a frame's wait IS its pace: func_020197b8 sleeps at phase
    //   7 and the VBlank is what ends the frame.
    //
    //   NOTHING INSTALLS IT TODAY, deliberately, and the measurement says why:
    //   a 300-frame level run reports halts=0, so this whole function is
    //   entered zero times while tests/walk_window.cpp drives the frame. That
    //   file calls port_host_frame_pump itself, at the point the duty always
    //   ran. Rung R3d hands the frame to func_020197b8, and the install below
    //   is the one line that keeps the duty running once per frame afterwards.
    //   Counted separately from pump_turns so the two pumps can never be
    //   confused for one another in a report.
    if (g_host_frame_pump) {
        ++g_stat.frame_pump_turns;
        g_host_frame_pump(g_starve);
    }

    // 2. the VBlank edge, through the port's own registry.
    //
    //    ONE EDGE PER FRAME'S WAIT, NOT ONE PER HALT (run link100, lane DET).
    //    This host has no 60 Hz interrupt source. The halt is its whole model of
    //    "a VBlank arrived", and a halt is not a clock: it happens as often as
    //    some piece of host code decides to sleep, which on a wireless wait is
    //    as often as a datagram is late. Delivered on every halt, the edge made
    //    data_0209d514 -- whose ONE ROM reader is the handler's own wake test at
    //    src/_ZN3IRQ13VBlankHandlerEv.c:15 -- a count of RADIO WAITS rather than
    //    a count of frames, and a count of radio waits is not something a
    //    re-simulated frame can reproduce: a replayed frame's comms are served
    //    from hal/rollback.cpp's record and take no wait at all. That is what
    //    the DET rung read as DSSTATE-DIFFERS on data_0209d500 and
    //    data_0209d514.
    //
    //    On a DS the count is per frame: func_020197b8 raises data_0209d4f0,
    //    sleeps, and data_0208ee44 VBlanks later the handler's own branch wakes
    //    it and puts the count back to zero -- and func_020197b8.c:63 zeroes it
    //    again after the wait, which is the cartridge saying out loud that a
    //    frame boundary reads zero. So the edge is delivered while THE FRAME'S
    //    OWN WAIT is what is asleep, and a halt that is a radio wait
    //    (data_0209d4f0 down) does not manufacture one.
    //
    //    NOTHING IS STRANDED BY THAT. Step 3 below is the wake for the per-
    //    VBlank queue data_0209d4fc and it runs whether or not the handler was
    //    dispatched -- it exists for exactly the case where the handler is not
    //    there -- so a thread sleeping on that queue during a radio wait is
    //    still woken. What stops happening in a radio wait is the ROM's own
    //    display commit (func_02019144), which is a per-frame duty and now runs
    //    once per frame instead of once per wait.
    //
    //    SM64DS_DET_VBLFRAME=0 restores the old reading on the same binary.
    const bool vbl_owed = !det_vblframe() || data_0209d4f0[0] != 0;
    if (void *h = vbl_owed ? _ZN3IRQ13GetIRQHandlerEj(ntr::IRQ_VBLANK) : 0) {
        volatile uint32_t *irq_if = reinterpret_cast<volatile uint32_t *>(0x04000214);
        *irq_if |= ntr::IRQ_VBLANK;
        ++g_stat.vblank_enters;   // before the call: see the counter's note
        // and the wait's own edge count, BEFORE the call for the same reason:
        // the handler's wake branch reschedules from inside, so a statement
        // after the call does not run until the idle fiber is resumed -- which
        // is the next frame's wait, and charging this frame's edge to that one
        // is the carried-count defect step 4 below exists to end.
        if (data_0209d4f0[0] != 0) ++g_frame_wait_edges;
        ++g_in_vblank_handler;
        reinterpret_cast<void (*)()>(h)();
        --g_in_vblank_handler;
        *irq_if &= ~ntr::IRQ_VBLANK;
        ++g_stat.vblank_dispatches;
        // AND THIS IS THE IRQ RETURN (run link100, lane DET2, rung 2). On the
        // cartridge the switch a wake inside the handler asked for happens
        // here, out of IRQ mode, once the handler has run to its end; see
        // ARMRestoreContext for the whole reason and for what m0 is. The ROM's
        // own reschedule is what performs it, so the pick, the save, the
        // callbacks and the current-thread word are all func_02057f54's, exactly
        // as they are on every other switch this port takes.
        if (det2_irqdefer() && mgr_u16(0) != 0) {
            mgr_u16(0) = 0;
            func_02057f54();
        }
    }

    // 3. the wake that handler performs, in ROM code. OS_WakeupThread clears
    //    the word, marks every thread whose bit is set runnable and calls
    //    func_02057f54 -- which switches back to the sleeper from inside here.
    if (vblank_queue() != 0) {
        ++g_stat.vblank_wakes;
        g_starve = 0;
        trace("halt: d4fc=%04x -> OS_WakeupThread", (unsigned)vblank_queue());
        OS_WakeupThread(&vblank_queue());
        return;
    }

    // 4. the bound. With a pump installed this is the pump's own limit, which
    //    is what hal/os_thread.h documented for the wireless wait.
    //
    //    WITH NONE IT USED TO BE ONE TURN, and run link100 lane R3D measured
    //    what that costs: a phase-7 sleep reports vbl_wakes=0 starved=1. The
    //    idle loop delivers ONE VBlank edge per turn and the ROM's own wake
    //    needs data_0208ee44 of them, which is 2 on a 3D level, so a one-turn
    //    bound returned control before the wake could ever fire and THIS
    //    port's starvation wake paced the frame instead of the VBlank. That is
    //    exactly the thing rung D5 hands the frame to.
    //
    //    So with no pump the bound is the ROM's own divider. It is still a
    //    HOST bound and not ROM behaviour -- the ROM has no such counter -- and
    //    it is still what stops this function spinning: the ceiling keeps a
    //    zero or unhosted divider on the old one-turn liveness and refuses an
    //    absurd one outright, so the worst case is a handful of turns rather
    //    than a number read out of uninitialised memory.
    unsigned limit;
    if (port::thread_pump() && !g_pump_suspended) {
        limit = port::thread_pump_limit();
    } else {
        const int div = data_0208ee44;
        limit = (div > 0 && div <= 8) ? static_cast<unsigned>(div) : 1u;
    }
    // AND THE ROM'S OWN FRAME WAIT IS BOUNDED BY ITS OWN VBLANK EDGES, NOT BY
    // THE PUMP'S VOTE OR BY A CARRIED-OVER HALT COUNT (run link100, lane DET;
    // the follow-up lane VS7 left this file's owner).
    //
    // data_0209d4f0 is up for exactly the length of func_020197b8's phase-7
    // wait and down everywhere else, so `up` here means "the thing asleep is
    // THE FRAME". While it is up the DS ends the wait one way and one way only:
    // data_0208ee44 VBlank edges arrive and IRQ::VBlankHandler's own branch
    // wakes data_0209d500. Two host accidents used to end it sooner, and both
    // are measured:
    //
    //   THE PUMP'S STOP VOTE. hal/comms_conductor.cpp's pump returns false on a
    //   connected session -- it means "I have nothing more to poll for", a
    //   statement about the radio and not about the frame -- and step 4 read it
    //   as "end the wait". A 300-frame level run on the ROM's loop then read
    //   starved=149: one frame in two ending on the port's own bound rather
    //   than the ROM's edge, with data_0209d514 left holding a PARTIAL count
    //   that alternated 1, 0, 1, 0 with the frame number.
    //
    //   THE CARRIED HALT COUNT. g_starve is not per-wait, and the halt that
    //   RESUMES the previous frame's parked handler is charged to this frame's
    //   wait: with a divider of 2 that is one of the two turns spent before a
    //   single edge of this frame has been delivered, so the wait ended after
    //   one edge with data_0209d500 still holding the sleeper's bit (starve_wake
    //   deliberately does not clear a queue word it cannot identify). Measured
    //   on the DET rung: straight run data_0209d500=0 data_0209d514=2, re-run
    //   data_0209d500=1 data_0209d514=1, every window, every repetition.
    //
    // So while the frame's wait is what is asleep, the bound counts THE EDGES
    // THIS WAIT HAS BEEN GIVEN -- reset by port_thread_frame_wait_begin below,
    // stepped by step 2 -- and allows it the divider's worth of them. The ROM's
    // own wake fires on the last of those, from inside step 2, so this bound is
    // reached only if the wake did not: it is liveness, not pacing. It cannot
    // hang, and it cannot end the wait early.
    //
    // data_0209d514 used to read the DIVIDER rather than zero at a frame
    // boundary, and the reason was that the ROM's handler resets it in the
    // statement after OS_WakeupThread while this port's OS_WakeupThread
    // switched away from inside, leaving the handler parked one statement
    // short until the next halt. That was deterministic but it was not the
    // cartridge. The IRQ-return deferral in ARMRestoreContext ends it (run
    // link100, lane DET2, rung 2): the handler now runs to its end in one go,
    // so the boundary reads zero and the reset is inside the frame it belongs
    // to. SM64DS_DET2_IRQDEFER=0 puts the old reading back.
    //
    // SM64DS_DET_FRAMEWAIT=0 restores the old reading on the same binary.
    //
    // AND THE ALLOWANCE IS ONE HALT WIDER THAN THE DIVIDER (run link100, lane
    // DET2, rung 1). The two counters this bound compares are stepped in the
    // same statement pair of step 2 above -- the edge is counted immediately
    // before the handler is called, and the handler's first line is
    // `data_0209d514 = data_0209d514 + 1` -- so on the wait's Nth halt the
    // allowance reads N and the ROM's own count reads N as well, and the ROM's
    // wake fires from INSIDE that call on the halt where both reach the
    // divider. The bound is therefore reached only on a halt where the ROM's
    // count says the wake was due and the wake did not happen, and there is
    // exactly one way for that: OS_WakeupThread(&data_0209d500) with an EMPTY
    // queue word does not reschedule (src/OS_WakeupThread.c:23 returns without
    // calling func_02057f54 when *self is 0), so the handler runs to its end,
    // returns, and this bound ends the frame instead of the ROM's wake.
    // Giving the wait one halt beyond the divider hands that frame back to the
    // ROM: the handler's own test is true again on the next halt, and if the
    // sleeper's bit has arrived by then the ROM's wake is what ends the frame.
    // Liveness is unchanged in kind -- the wait still cannot spin -- it is one
    // turn longer. MEASURED on solo level 1, 300 frames, nothing exported: the
    // census is identical with the slack on and off (halts=598 vbl_enter=598
    // vbl_dispatch=597 vbl_wakes=0 starved=0 both ways), because on that path
    // the queue word is never empty when the wake is due and the bound is
    // never reached at all. SM64DS_DET2_OWED1=0 puts the exact-divider bound
    // back on the same binary.
    if (data_0209d4f0[0] != 0 && det_framewait()) {
        const int fdiv = data_0208ee44;
        unsigned owed = (fdiv > 0 && fdiv <= 8) ? static_cast<unsigned>(fdiv) : 1u;
        if (det2_owed1()) owed += 1u;
        if (g_frame_wait_edges >= owed) {
            g_starve = 0;
            g_frame_wait_edges = 0;
            starve_wake();
        }
        return;
    }
    if (pump_stop || ++g_starve >= limit) {
        g_starve = 0;
        starve_wake();
    }
}

// THE RE-SIMULATION GATE (run link100, lane DET). tests/walk_window.cpp raises
// this around the ROM's phase-7 sleep on a frame hal/rollback.cpp is replaying
// and drops it after; step 1 of the halt above is what reads it. Kept as a
// setter rather than a direct rb_replaying() call because hal/rollback.cpp is
// not in every target that compiles this file (smoke_player has no rollback),
// which is the same reason hal/star_flow.cpp and hal/luigi_infection.cpp go
// through a plain int rather than calling rb_replaying() themselves.
void port_thread_pump_suspend(int on)
{
    g_pump_suspended = on ? 1 : 0;
}

// THE FRAME'S OWN WAIT STARTS WITH A FRESH BOUND (run link100, lane DET).
// g_starve is the halt bound's counter and it is not per-wait: a radio wait
// earlier in the same frame could leave it one short of the limit, and the
// frame's phase-7 sleep would then be cut off by step 4 on its FIRST halt --
// before the ROM's own wake could fire. A sleep that ends that way leaves
// data_0209d500 holding the sleeper's bit, because starve_wake deliberately
// does not clear a queue word it cannot identify, and the next frame boundary
// then reads a word the ROM's own wake would have zeroed. Called from
// tests/walk_window.cpp immediately before func_0201a4bc's sleep, which is the
// only place the frame's own wait begins.
void port_thread_frame_wait_begin(void)
{
    g_starve = 0;
    g_frame_wait_edges = 0;
}

}  // extern "C"

// ===========================================================================
// THE PROOF, and the counters port/tools/thread_proof.py reads.
// ===========================================================================

// ---------------------------------------------------------------------------
// WHAT hal/thread_create.cpp READS. Two C entry points, both narrow on
// purpose: a counter snapshot and the one repair the negative control needs.
// ---------------------------------------------------------------------------
extern "C" void port_thread_sched_counts(unsigned long long *adopted,
                                         unsigned long long *entered,
                                         unsigned long long *restores,
                                         unsigned long long *rejected,
                                         unsigned long long *nocreate)
{
    if (adopted)  *adopted  = g_stat.adopted;
    if (entered)  *entered  = g_stat.entered;
    if (restores) *restores = g_stat.restores;
    if (rejected) *rejected = g_stat.rejected;
    if (nocreate) *nocreate = g_stat.nocreate;
}

// src/func_02057f54.c stores its PICK into the manager's current-thread word
// BEFORE calling ARMRestoreContext:
//
//     data_020a6134.m8 = r5;
//     ARMRestoreContext(r5);
//
// so a REFUSED restore -- SM64DS_THREAD_NOFIBER or SM64DS_THREAD_NOCREATE, the
// two negative controls -- leaves that word naming a thread nobody switched
// to, while the fiber that is really executing is the old one. This puts the
// word back on the thread that is actually running. It is the same repair
// thread_proof() below performs after its own control arm, and it exists ONLY
// for that: on the armed path no caller ever needs it, because the switch
// happened and the word is right.
// rung R3b step B2: install the HOST FRAME pump that step 1b of
// CP1516WaitForInterrupt calls. Nothing calls this yet -- rung R3d is what
// calls it, once, when func_020197b8 becomes the thing that drives the frame.
// Until then tests/walk_window.cpp calls port_host_frame_pump itself at the
// point the duty always ran, so the frame is unchanged to the statement.
extern "C" void port_install_host_frame_pump(int (*pump)(unsigned))
{
    g_host_frame_pump = pump;
}

extern "C" void port_thread_repair_current(void)
{
    if (!g_running) return;
    mgr_current() = g_running;
    g_running->state = 1;
}

namespace port {

void thread_sched_report(const char *tag) {
    std::fprintf(stderr,
                 "[thr] %s saves=%llu switches=%llu resumes=%llu refused=%llu "
                 "halts=%llu pump=%llu framepump=%llu vbl_enter=%llu "
                 "vbl_dispatch=%llu vbl_wakes=%llu "
                 "starved=%llu unknown=%llu adopted=%llu entered=%llu "
                 "exited=%llu rejected=%llu nocreate=%llu deferred=%llu\n",
                 tag, g_stat.saves, g_stat.restores, g_stat.resumes,
                 g_stat.refused, g_stat.halts, g_stat.pump_turns,
                 g_stat.frame_pump_turns, g_stat.vblank_enters,
                 g_stat.vblank_dispatches, g_stat.vblank_wakes, g_stat.starved,
                 g_stat.unknown_ctx, g_stat.adopted, g_stat.entered,
                 g_stat.exited, g_stat.rejected, g_stat.nocreate,
                 g_stat.deferred);
    std::fflush(stderr);
}

}  // namespace port

namespace {

// THE ROUND TRIP, driven by the ROM's own sleep site.
//
//   src/func_0201a4d0.c  ->  OS_SleepThread(data_0209d4fc)   [ROM]
//     -> src/func_02057f54.c  -> src/func_0205801c.c picks the idle thread
//       -> ARMRestoreContext  -> src/func_02057e34.c        [ROM]
//         -> the halt's step 3: OS_WakeupThread(&data_0209d4fc)  [ROM]
//           -> src/func_02057f54.c picks the main thread again
//             -> back inside ARMRestoreContext, and func_0201a4d0 returns.
//
// Everything except the two context primitives and the halt is ROM code, and
// the two ends of the wait are the ROM's own sleep and the ROM's own wake.
void thread_proof() {
    if (!std::getenv("SM64DS_THREAD_PROOF")) return;

    const unsigned long long r0 = g_stat.restores;
    const unsigned long long w0 = g_stat.vblank_wakes;
    const unsigned long long s0 = g_stat.starved;
    const unsigned long long f0 = g_stat.refused;

    vblank_queue() = 0;
    std::fprintf(stderr, "[thr] PROOF begin: calling src/func_0201a4d0.c "
                 "(OS_SleepThread(data_0209d4fc)), current=%u\n",
                 g_running ? g_running->id : 0u);
    std::fflush(stderr);

    func_0201a4d0();

    const unsigned long long switches = g_stat.restores - r0;
    const unsigned long long wakes = g_stat.vblank_wakes - w0;
    const unsigned long long starved = g_stat.starved - s0;
    const unsigned long long refused = g_stat.refused - f0;

    const bool round_trip = switches >= 2 && wakes >= 1 && g_running == &g_main;
    std::fprintf(stderr,
                 "[thr] PROOF switches=%llu vbl_wakes=%llu starved=%llu "
                 "refused=%llu word=%04x current=%u main_state=%u "
                 "VERDICT %s\n",
                 switches, wakes, starved, refused, (unsigned)vblank_queue(),
                 g_running ? g_running->id : 0u, g_main.state,
                 round_trip ? "ROUND-TRIP" : (refused ? "NO-SWITCH" : "NO-WAKE"));
    std::fflush(stderr);
    port::thread_sched_report("PROOF-STATS");

    // The negative control leaves the ROM's sleep half-done on purpose (the
    // switch was refused, so nothing cleared the word or made the thread
    // runnable again). Put it back so the rest of the run is not poisoned.
    if (refused) {
        g_main.state = 1;
        vblank_queue() = 0;
        mgr_current() = &g_main;
        g_running = &g_main;
        std::fprintf(stderr, "[thr] PROOF control: state repaired\n");
        std::fflush(stderr);
    }
}

// Run before ordinary user initialisers so the manager is wired before any
// other translation unit's constructor can reach a sleep.
#pragma init_seg(lib)
struct ThreadBoot {
    ThreadBoot() {
        thread_boot();
        thread_proof();
    }
    // WHAT THE WHOLE RUN DID, not just the probe. This is the line that says
    // whether gameplay reaches a ROM sleep at all -- a proof that only ever
    // fires its own probe has measured the probe, and port/tools/
    // thread_proof.py reads both.
    ~ThreadBoot() {
        if (trace_on() || std::getenv("SM64DS_THREAD_PROOF"))
            port::thread_sched_report("EXIT-STATS");
    }
} g_thread_boot;

}  // namespace
