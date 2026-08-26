// THE DS's SLEEP/WAKE PAIR, HOSTED. Run mg15, lane MP1.
//
// ============================ WHY THIS EXISTS ==============================
//
// port/ntr/include/ntr/rt.h:11-12 says the fiber mechanism "generalises to
// OS_SleepThread / OS_WakeupThread when those are needed." They are needed
// now: the ROM's wireless lockstep blocks on one, and the port has never
// exercised either. This file is the test of that claim and the answer to it.
//
// WHAT THE ROM'S PAIR IS, read off src/OS_SleepThread.c and src/OS_WakeupThread.c:
//
//   OS_SleepThread(q)   if q, set bit (1 << my thread id) in the halfword *q;
//                       mark myself not-runnable; reschedule (func_02057f54).
//   OS_WakeupThread(q)  if *q is non-zero, mark every thread whose bit is set
//                       runnable, store 0 to *q, reschedule.
//
// So the QUEUE WORD is the whole observable contract: a sleeper sets its bit
// and waits for the word to go to zero. Everything else is the ARM scheduler.
//
// WHY THE ROM BODIES CANNOT RUN HERE, measured and not argued:
//
//   1. OS_SleepThread's first act is `data_020a612c->node`.
//      port/hal/cxx_aliases.cpp:346 hosts data_020a612c as `int[4]`, zero.
//      That is a null dereference on the first call.
//   2. Past it, func_02057f54 calls ARMProcessorMode, ARMSaveContext and
//      ARMRestoreContext -- an ARM register-file context switch. There is no
//      host body for any of the three and there is no honest one to write.
//
//   port/nfs_names_map.txt:402 already said "OS_SleepThread would fault in
//   this port anyway". port/tests/mp_sleepwake.cpp MEASURES it rather than
//   quoting it: build target mp_sleepwake_rom links the ROM bodies and runs
//   the same probe under a structured-exception guard.
//
// ============================ WHAT REPLACES THEM ===========================
//
// The port has ONE game context. A block therefore needs something else to
// run, and the thing that has to run is a HOST FRAME -- because the sleep the
// wireless path performs is, in the ROM, a wait for the next VBlank:
//
//   src/func_02042778.c   tail-call veneer
//     -> src/func_0201a4d0.c   OS_SleepThread(data_0209d4fc)
//   src/_ZN3IRQ13VBlankHandlerEv.c:22   OS_WakeupThread(&data_0209d4fc)
//
// data_0209d4fc is the PER-VBLANK wake queue and the VBlank handler clears it
// every frame. (data_0209d500, three lines above, is the every-N-frames queue.
// The wireless veneer sleeps on the per-vblank one.) FRONTIER.md section 6
// reads the wireless lockstep as waiting on "the wireless callback"; the queue
// word says otherwise, and the queue word is the code.
//
// So: a PUMP. port_thread_set_pump installs a callback that advances the host
// by one frame's worth of interrupt work. OS_SleepThread runs it until the
// queue word clears or the pump limit is reached.
//
//   NO PUMP INSTALLED  -> the sleep returns immediately, counted.
//                         That is exactly the port's behaviour today for every
//                         caller that reaches a sleep, which is why installing
//                         this file changes nothing until a frame loop opts in.
//   PUMP INSTALLED     -> a real wait, one host frame per turn, bounded.
//
// IT CANNOT HANG. The bound is port_thread_set_pump_limit (default
// kPortThreadPumpLimitDefault). The ROM's own wireless loop carries a second,
// independent bound of its own (0x4B0 turns before init completes, 0x12C
// after -- src/func_0203ea5c.c:142-146) and drops the session to solo when it
// runs out. Two bounds, and neither of them is "wait forever".
//
// RE-ENTRANCY IS REFUSED. A pump that reaches game code that sleeps again
// would recurse. A sleep taken from inside a pump returns immediately and is
// counted separately (nested_returns), so the refusal is visible rather than
// silent.

#ifndef PORT_HAL_OS_THREAD_H
#define PORT_HAL_OS_THREAD_H

#include <stdint.h>

extern "C" {

// The DS pair, hosted. Signatures are the ROM's.
void OS_SleepThread(uint16_t *q);
void OS_WakeupThread(uint16_t *q);

}  // extern "C"

namespace port {

// The port's single game context occupies thread slot 0, so its queue bit is
// bit 0 -- `1 << node->unk6c` in src/OS_SleepThread.c with unk6c == 0.
enum : uint16_t { kPortThreadBit = 1u };

// How many pump turns a single sleep will take before it gives up and returns.
// 600 is two DS seconds, comfortably past the ROM wireless loop's own 0x12C
// (300) turn bound so that the ROM's timeout is the one that fires first and
// the ROM's own solo fallback is the one that runs.
enum : unsigned { kPortThreadPumpLimitDefault = 600 };

// Advance the host by one turn while a sleep is blocked. Return false to make
// the sleep give up. `spin` counts turns within this one sleep, from 0.
using ThreadPump = bool (*)(unsigned spin);

void thread_set_pump(ThreadPump pump);
ThreadPump thread_pump();
void thread_set_pump_limit(unsigned turns);
unsigned thread_pump_limit();

// A ready-made pump: raise one VBlank edge and dispatch whatever the game
// registered for IE/IF bit 0. On a boot that ran the ROM's own IRQ init that
// handler is _ZN3IRQ13VBlankHandlerEv, whose line 22 is the wake this file's
// header describes. Returns false if nothing is registered for bit 0, because
// a pump with no wake source is a busy-spin and should stop rather than burn
// the limit.
bool pump_vblank(unsigned spin);

// A pump for a target running on ntr::rt_run's fiber: yield to the host fiber
// exactly the way VBlankIntrWait does. This is the rt.h claim taken literally.
// Returns false when there is no fiber to yield to (rt_run is not running),
// which is the case in every target the port currently ships.
bool pump_fiber(unsigned spin);

struct ThreadStats {
    uint64_t sleeps;            // OS_SleepThread entries
    uint64_t wakes;             // OS_WakeupThread calls that cleared a word
    uint64_t immediate_returns; // sleeps that returned with no pump installed
    uint64_t nested_returns;    // sleeps refused because a pump was running
    uint64_t unqueued_sleeps;   // OS_SleepThread(0) -- no queue word to watch
    uint64_t pump_turns;        // total pump callbacks made
    uint64_t woken_sleeps;      // sleeps that ended because the word cleared
    uint64_t timeouts;          // sleeps that ended on the pump limit
    uint64_t pump_stops;        // sleeps that ended because the pump said stop
};

ThreadStats thread_stats();
void thread_stats_reset();

}  // namespace port

#endif  // PORT_HAL_OS_THREAD_H
