// THE DS's SLEEP/WAKE PAIR. Run mg15, lane MP1. RETIRED by run link2, lane THR.
// Everything about why this file existed is in os_thread.h. This is the body.
//
// ============================ WHAT IS LEFT HERE ============================
//
// THE HOSTED PAIR IS GONE FROM EVERY SHIPPED TARGET. The reason it existed --
// "the ROM's sleep reschedules through ARMSaveContext/ARMRestoreContext, which
// have no host body" -- stopped being true when hal/boot2_thread.cpp gave those
// two hand-asm primitives a fiber-backed body, which is what
// port/ntr/include/ntr/rt.h:11-14 said the fiber mechanism was for. So
// src/OS_SleepThread.c, src/OS_WakeupThread.c and src/func_02057f54.c link and
// run as written, on port/slice_gate2thr.txt, and the two host bodies below
// compile ONLY under PORT_OS_THREAD_HOST_PAIR -- which port/CMakeLists.txt
// defines for mp_sleepwake and mp_comms_seam and for nothing else. They are
// kept there because mp_sleepwake exists to MEASURE the difference between the
// ROM bodies and this stand-in; deleting the stand-in deletes the measurement.
//
// WHAT DOES STILL LIVE HERE UNCONDITIONALLY: the pump. hal/comms_conductor.cpp
// installs one through thread_set_pump, and the pump's job -- advance the host
// by one turn while a DS thread is blocked -- is the same job under the new
// scheduler. It is now called once per turn of the ROM's own IDLE THREAD, from
// hal/boot2_thread.cpp's CP15 wait-for-interrupt body, with the same bound
// (thread_pump_limit). Nothing about a pump's contract changed; the thing that
// calls it did.
//
// pump_vblank and pump_fiber are kept for the same reason: both are still
// callers' choices of what a blocked turn should do, and port/tests/
// mp_sleepwake.cpp measures both. pump_fiber's caveat is unchanged and now
// doubly true -- the idle thread runs on ITS OWN fiber, so yielding to rt_run's
// host fiber from inside a DS thread would resume rt_run's game fiber and not
// the DS thread that yielded. Do not install pump_fiber under the scheduler.

#include "os_thread.h"

#include "ntr/rt.h"

#include <cstdio>
#include <cstdlib>

extern "C" void *_ZN3IRQ13GetIRQHandlerEj(unsigned mask);

namespace port {
namespace {

ThreadPump g_pump = nullptr;
unsigned g_pump_limit = kPortThreadPumpLimitDefault;
bool g_in_pump = false;
ThreadStats g_stats;

// THE DEPTH HELPER, THROUGH A HOOK (run link100, lane DET4). hal/
// boot2_thread.cpp owns port_irq_mode_depth and the manager's pending flag
// this file wants to bracket pump_vblank's dispatch with (see that file's
// header, THE OTHER TWO DISPATCHES) -- but this file has to stay linkable
// into mp_sleepwake and mp_comms_seam, which measure the RETIRED host pair
// under PORT_OS_THREAD_HOST_PAIR below and, by this file's own header, link
// NEITHER hal/boot2_thread.cpp NOR hal/cxx_aliases.cpp. A direct extern
// reference to either symbol is an unresolved external on those two targets
// (measured: it is exactly what ntr.lib's HBlank site broke on eight smoke_*
// probes before this file adopted the same pattern -- see ntr/rt.cpp).
// So: three hooks, null by default (a no-op bracket and an always-false
// mode-ask, which is today's behaviour on every target that does not install
// them), that hal/boot2_thread.cpp wires up with a static registration --
// see that file's IrqModeHookReg -- on every target that DOES link it.
void (*g_irq_mode_enter)() = nullptr;
void (*g_irq_mode_exit)() = nullptr;
uint16_t (*g_irq_pending_flag_read)() = nullptr;

// run link100, lane DET4: pump_vblank's own census. See the report this
// prints at exit for what these mean and why they are 0 on every run this
// lane measured.
unsigned long long g_pump_vblank_dispatches;
unsigned long long g_pump_vblank_modeask;

bool trace_on() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_THREAD_TRACE") ? 1 : 0;
    return v != 0;
}

void pump_vblank_report() {
    std::fprintf(stderr,
        "[det4] pump_vblank: %llu dispatch(es), %llu of them saw the "
        "manager's pending flag (m0) go 0->1 during the call -- "
        "func_02057f54's own deferral, i.e. a ROM body asked "
        "ARMProcessorMode() from inside THIS handler and got 0x12 "
        "(run link100, lane DET4; SM64DS_DET3 gates the depth this counts "
        "against, same as hal/boot2_thread.cpp's own site). Printed "
        "unconditionally, including when both are zero: comms_conductor "
        "installs conductor_pump on every session this port makes today, so "
        "nothing calls thread_set_pump(pump_vblank) and this dispatch count "
        "is the direct proof, not an inference.\n",
        g_pump_vblank_dispatches, g_pump_vblank_modeask);
    std::fflush(stderr);
}
struct PumpVblankReportReg {
    PumpVblankReportReg() { std::atexit(pump_vblank_report); }
} g_pump_vblank_report_reg;

}  // namespace

void thread_set_pump(ThreadPump pump) { g_pump = pump; }
ThreadPump thread_pump() { return g_pump; }
void thread_set_pump_limit(unsigned turns) { g_pump_limit = turns; }
unsigned thread_pump_limit() { return g_pump_limit; }
ThreadStats thread_stats() { return g_stats; }
void thread_stats_reset() { g_stats = ThreadStats(); }

// run link100, lane DET4. Read by port/tests/walk_window.cpp's own census
// line, which ties these counts to a run's frame number; the report above
// (printed at process exit) is what carries them on the scene and captured-
// pair paths, which do not go through that file's exit block.
void pump_vblank_counts(unsigned long long *dispatches,
                        unsigned long long *modeask) {
    if (dispatches) *dispatches = g_pump_vblank_dispatches;
    if (modeask) *modeask = g_pump_vblank_modeask;
}

// run link100, lane DET4. hal/boot2_thread.cpp calls this once, at static
// init, on every target that links it -- see that file's IrqModeHookReg. Any
// argument left null (as all three are on mp_sleepwake/mp_comms_seam, which
// never call this at all) makes the corresponding bracket/read a no-op below,
// which is this file's behaviour before this lane.
void thread_set_irq_mode_hooks(void (*enter)(), void (*exit)(),
                               uint16_t (*pending_flag_read)()) {
    g_irq_mode_enter = enter;
    g_irq_mode_exit = exit;
    g_irq_pending_flag_read = pending_flag_read;
}

bool pump_vblank(unsigned) {
    void *h = _ZN3IRQ13GetIRQHandlerEj(ntr::IRQ_VBLANK);
    if (!h) return false;          // no wake source: stop, do not busy-spin
    volatile uint32_t *irq_if = reinterpret_cast<volatile uint32_t *>(0x04000214);
    *irq_if |= ntr::IRQ_VBLANK;
    // run link100, lane DET4: the depth helper brackets this call the same
    // way hal/boot2_thread.cpp's own VBlank dispatch does, and the pending-
    // flag read either side of it is this site's mode-ask census. Both go
    // through the hooks above; see their own comment for why.
    const bool m0_before =
        g_irq_pending_flag_read && g_irq_pending_flag_read() != 0;
    if (g_irq_mode_enter) g_irq_mode_enter();
    reinterpret_cast<void (*)()>(h)();
    if (g_irq_mode_exit) g_irq_mode_exit();
    ++g_pump_vblank_dispatches;
    if (!m0_before && g_irq_pending_flag_read && g_irq_pending_flag_read() != 0)
        ++g_pump_vblank_modeask;
    *irq_if &= ~ntr::IRQ_VBLANK;
    return true;
}

bool pump_fiber(unsigned) {
    // rt_vblank_wait is a no-op when rt_run is not driving a game fiber, and
    // a no-op pump is a busy-spin. ntr::rt_frame() moving is the only host-
    // visible proof that the yield actually reached the frame clock.
    const uint64_t before = ntr::rt_frame();
    ntr::rt_vblank_wait();
    return ntr::rt_frame() != before;
}

}  // namespace port

// ===========================================================================
// THE RETIRED HOST PAIR. Run link2 lane THR: compiled ONLY for the two console
// probes that measure it (port/CMakeLists.txt defines PORT_OS_THREAD_HOST_PAIR
// for mp_sleepwake and mp_comms_seam). In every shipped target the bodies below
// are src/OS_SleepThread.c and src/OS_WakeupThread.c instead, running on
// hal/boot2_thread.cpp's fiber-backed context primitives.
// ===========================================================================
#if defined(PORT_OS_THREAD_HOST_PAIR)

extern "C" {

// PORT_HOST_ABI: src/OS_SleepThread.c reschedules through ARMSaveContext/ARMRestoreContext, which have no host body; the queue-word half is reproduced exactly and the ARM context switch is replaced by a bounded host pump.
void OS_SleepThread(uint16_t *q) {
    ++port::g_stats.sleeps;

    // The ROM brackets the queue-word update in IRQ::Disable/Restore. Keep the
    // bracket: game code can observe ntr::rt_irq_masked(), and the update is
    // the half that has to be atomic against a wake. The PUMP runs outside it,
    // because the pump is where the interrupt work happens.
    const uint32_t saved = ntr::rt_irq_disable();
    if (q) *q = static_cast<uint16_t>(*q | port::kPortThreadBit);
    ntr::rt_irq_restore(saved);

    if (!q) ++port::g_stats.unqueued_sleeps;

    if (port::g_in_pump) {
        ++port::g_stats.nested_returns;
        if (q) *q = static_cast<uint16_t>(*q & ~port::kPortThreadBit);
        return;
    }
    if (!port::thread_pump()) {
        ++port::g_stats.immediate_returns;
        if (q) *q = static_cast<uint16_t>(*q & ~port::kPortThreadBit);
        return;
    }

    unsigned spin = 0;
    for (;;) {
        if (q && !(*q & port::kPortThreadBit)) { ++port::g_stats.woken_sleeps; break; }
        port::g_in_pump = true;
        const bool keep_going = port::thread_pump()(spin);
        port::g_in_pump = false;
        ++port::g_stats.pump_turns;
        ++spin;
        if (!q) break;                    // no word to watch: one turn, then out
        if (!keep_going) { ++port::g_stats.pump_stops; break; }
        if (spin >= port::thread_pump_limit()) {
            ++port::g_stats.timeouts;
            break;
        }
    }
    if (port::trace_on())
        std::fprintf(stderr, "[thread] sleep q=%p turns=%u\n",
                     static_cast<void *>(q), spin);

    const uint32_t saved2 = ntr::rt_irq_disable();
    if (q) *q = static_cast<uint16_t>(*q & ~port::kPortThreadBit);
    ntr::rt_irq_restore(saved2);
}

// PORT_HOST_ABI: the wake half of the DS sleep/wake pair; the port has no DS thread scheduler to reschedule into, so only the queue-word clear is reproducible on the host.
void OS_WakeupThread(uint16_t *q) {
    if (!q) return;
    const uint32_t saved = ntr::rt_irq_disable();
    if (*q != 0) {
        *q = 0;
        ++port::g_stats.wakes;
    }
    ntr::rt_irq_restore(saved);
}

}  // extern "C"

#endif  // PORT_OS_THREAD_HOST_PAIR
