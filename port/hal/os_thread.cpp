// THE DS's SLEEP/WAKE PAIR, HOSTED. Run mg15, lane MP1.
// Everything about why this file exists is in os_thread.h. This is the body.
//
// PORT_HOST_ABI: src/OS_SleepThread.c derefs data_020a612c->node (a null host
// global) and reschedules through ARMSaveContext/ARMRestoreContext, neither of
// which has a host body. The queue-word half of the contract is reproduced
// here exactly; the ARM context switch is replaced by a bounded host pump.

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

bool trace_on() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_THREAD_TRACE") ? 1 : 0;
    return v != 0;
}

}  // namespace

void thread_set_pump(ThreadPump pump) { g_pump = pump; }
ThreadPump thread_pump() { return g_pump; }
void thread_set_pump_limit(unsigned turns) { g_pump_limit = turns; }
unsigned thread_pump_limit() { return g_pump_limit; }
ThreadStats thread_stats() { return g_stats; }
void thread_stats_reset() { g_stats = ThreadStats(); }

bool pump_vblank(unsigned) {
    void *h = _ZN3IRQ13GetIRQHandlerEj(ntr::IRQ_VBLANK);
    if (!h) return false;          // no wake source: stop, do not busy-spin
    volatile uint32_t *irq_if = reinterpret_cast<volatile uint32_t *>(0x04000214);
    *irq_if |= ntr::IRQ_VBLANK;
    reinterpret_cast<void (*)()>(h)();
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

extern "C" {

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
