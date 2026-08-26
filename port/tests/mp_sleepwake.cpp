// MP1 STEP 1: THE SLEEP/WAKE PROBE. Run mg15, lane MP1.
//
// The multiplayer frontier map (runs/mg15/out/MP/FRONTIER.md section 10.2)
// names one thing as the largest unknown in the whole radio-seam plan:
//
//   "Does func_0203ea5c's blocking spin survive the port's fiber scheduler?"
//
// This probe answers it, in the order the map asked for:
//
//   1. THE ROM BODIES. Build target mp_sleepwake_rom links src/OS_SleepThread.c,
//      src/OS_WakeupThread.c and src/func_02057f54.c -- the three TUs
//      port/slice_gate10.txt already puts in every shipped binary -- and calls
//      the sleep through the SAME VENEER the wireless code uses, under a
//      structured-exception guard. port/nfs_names_map.txt:402 asserts these
//      "would fault in this port anyway"; this measures it.
//
//   2. THE HOST PAIR. Build target mp_sleepwake links port/hal/os_thread.cpp
//      instead and runs the identical probe.
//
//   3. THE LOOP. The ROM's wireless lockstep, transcribed from
//      src/func_0203ea5c.c lines 141-422 (the control skeleton verbatim, the
//      packet staging elided), against three transports:
//        answers immediately / answers after N turns / never answers.
//      The third is the one that decides the plan: it MUST fall back to solo
//      inside the ROM's own bound, not hang.
//
// THE VENEER IS REAL IN BOTH ARMS. src/func_02042778.c and src/func_0201a4d0.c
// are linked as themselves, so the call actually goes
//     func_02042778 -> func_0201a4d0 -> OS_SleepThread(data_0209d4fc)
// and the wake actually goes through the ROM's own expression from
// src/_ZN3IRQ13VBlankHandlerEv.c:22, OS_WakeupThread(&data_0209d4fc).
//
// SILENT BY CONSTRUCTION: console only, no window, no BMP.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ntr/rt.h"

#if !defined(MP_PROBE_ROM_THREADS)
#include "os_thread.h"
#endif

extern "C" {
// The veneer, real: src/func_02042778.c tail-calls src/func_0201a4d0.c, which
// is OS_SleepThread(data_0209d4fc).
void func_02042778(void);
// The per-VBlank wake queue. src/func_0201a4d0.c takes its address;
// src/_ZN3IRQ13VBlankHandlerEv.c:22 hands the same address to OS_WakeupThread.
extern unsigned short data_0209d4fc;
void OS_SleepThread(unsigned short *q);
void OS_WakeupThread(unsigned short *q);
}

// The queue word itself. In the shipped binaries it lives in the auto-hosted
// DS BSS; the probe links neither hal/auto_bss.cpp nor hal/cxx_aliases.cpp, so
// it is defined here at its ROM width (src/func_0201a4e4.c declares it u16).
extern "C" unsigned short data_0209d4fc;
unsigned short data_0209d4fc;

#if defined(MP_PROBE_ROM_THREADS)
// ---- ARM 1: the ROM bodies, exactly as slice_gate10 links them -------------
//
// The two thread-manager globals, at the width and initial value
// port/hal/cxx_aliases.cpp:346 and :415 give them in the shipped binaries.
// ZERO is the whole point: src/OS_SleepThread.c's first act is
// `data_020a612c->node`.
extern "C" {
int data_020a612c[4];
int data_020a6134[4];
int data_020a6128;
// func_02057f54's ARM context-switch helpers. There is no host body for any of
// them and there is no honest one to write; the probe aborts loudly if the
// fault it is measuring somehow does not happen first.
int ARMProcessorMode(void) { std::fprintf(stderr,
    "FATAL: ARMProcessorMode reached -- the null deref did not fire\n");
    std::abort(); }
int ARMSaveContext(void *) { std::fprintf(stderr,
    "FATAL: ARMSaveContext reached\n"); std::abort(); }
void ARMRestoreContext(void *) { std::fprintf(stderr,
    "FATAL: ARMRestoreContext reached\n"); std::abort(); }
void *func_0205801c(void) { std::fprintf(stderr,
    "FATAL: func_0205801c reached\n"); std::abort(); }
}
#endif

// ---------------------------------------------------------------------------

static int g_fail;
#define CHECK(cond) do { if (!(cond)) { ++g_fail; \
    std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static double now_ms() {
#if defined(_WIN32)
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    return 0.0;
#endif
}

// ---- the pump the probe installs ------------------------------------------
//
// It stands in for the frame the DS would have run while the main thread was
// off the scheduler. The line it exists for is the ROM's own:
//     src/_ZN3IRQ13VBlankHandlerEv.c:22   OS_WakeupThread(&data_0209d4fc)
// Every turn of this pump is one DS VBlank.

static unsigned long long g_host_frames;
static unsigned g_wake_after;      // turns of pump before the wake is delivered
static unsigned g_turns_this_wait;
static bool g_deliver_wake = true;

static bool probe_pump(unsigned spin) {
    ++g_host_frames;
    ++g_turns_this_wait;
    if (g_deliver_wake && g_turns_this_wait > g_wake_after) {
        // The VBlank handler's own line.
        OS_WakeupThread(&data_0209d4fc);
    }
    (void)spin;
    return true;
}

// ===========================================================================
// PART 3: the ROM's wireless lockstep, control skeleton verbatim.
//
// src/func_0203ea5c.c, lines 141-146 (the bound), 157 (the while), 190-232
// (the link-state switch), 413-421 (the sleep), 423-424 (the give-up).
// The packet staging (lines 158-189) and the unpack (lines 234-411) are
// elided: they touch no host runtime and cannot change the loop's shape.
// ===========================================================================

struct MockLink {
    int state;              // what func_02040714() answers
    int answer_after;       // turns of the loop before func_020406b4 returns 1
    int turns;
};
static MockLink g_link;

static int mock_func_02040714(void) { return g_link.state; }          // link state
static int mock_func_020406b4(void) {                                  // send+poll
    ++g_link.turns;
    if (g_link.answer_after < 0) return 0;                             // never
    return g_link.turns > g_link.answer_after ? 1 : 0;
}

struct LockstepResult {
    int role_after;         // data_020a0f04 -- 0 means it fell back to solo
    int turns;
    unsigned long long host_frames;
    double ms;
    bool completed;
};

static LockstepResult run_lockstep(int link_state, int answer_after,
                                   int bound, bool deliver_wake) {
    g_link.state = link_state;
    g_link.answer_after = answer_after;
    g_link.turns = 0;
    g_deliver_wake = deliver_wake;
    const unsigned long long frames0 = g_host_frames;

    int role = 2;                       // data_020a0f04, a wireless child
    int sp8 = 0;                        // "this frame's round has arrived"
    int sp4 = bound;                    // 0x12C after init, 0x4B0 before
    int turns = 0;

    const double t0 = now_ms();
    while ((sp8 == 0) && (sp4 != 0)) {  // :157
        ++turns;
        switch (mock_func_02040714()) { // :190
        case 3:                          // parent-connected
        case 4:                          // child-connected
            sp8 = mock_func_020406b4();  // :208 / :220
            break;
        default:
            sp8 = 0;
            break;
        }
        if (sp4 != 0) sp4 -= 1;          // :413-415
        if (sp8 == 0) {                  // :416
            g_turns_this_wait = 0;
            func_02042778();             // :417  THE SLEEP, through the veneer
        }
    }
    const double t1 = now_ms();

    if (sp4 == 0) { role = 0; }          // :423  give up -> SOLO

    LockstepResult r;
    r.role_after = role;
    r.turns = turns;
    r.host_frames = g_host_frames - frames0;
    r.ms = t1 - t0;
    r.completed = (sp8 != 0);
    return r;
}

// ===========================================================================

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);

#if defined(MP_PROBE_ROM_THREADS)
    std::printf("== MP1 sleep/wake probe -- ARM 1: THE ROM BODIES ==\n");
    std::printf("linked: src/OS_SleepThread.c src/OS_WakeupThread.c "
                "src/func_02057f54.c (port/slice_gate10.txt:340,347,349)\n");
    std::printf("data_020a612c[0] = %d   (hal/cxx_aliases.cpp:346 hosts it zero)\n",
                data_020a612c[0]);
    data_0209d4fc = 0;
    unsigned long code = 0;
    void *addr = nullptr;
    bool faulted = false;
#if defined(_WIN32)
    __try {
        func_02042778();        // -> func_0201a4d0 -> OS_SleepThread(data_0209d4fc)
    } __except (code = GetExceptionCode(),
                addr = GetExceptionInformation()->ExceptionRecord->ExceptionAddress,
                EXCEPTION_EXECUTE_HANDLER) {
        faulted = true;
    }
#endif
    if (faulted) {
        std::printf("VERDICT: the ROM sleep FAULTED through the veneer. "
                    "exception 0x%08lx at %p\n", code, addr);
        std::printf("  chain: func_02042778 -> func_0201a4d0 -> "
                    "OS_SleepThread(&data_0209d4fc) -> data_020a612c->node\n");
        std::printf("  This is port/nfs_names_map.txt:402's claim, measured.\n");
    } else {
        std::printf("VERDICT: the ROM sleep RETURNED without faulting. "
                    "nfs_names_map.txt:402 is WRONG and this probe's whole "
                    "premise needs re-deriving.\n");
        ++g_fail;
    }
    std::printf("\n%s\n", g_fail ? "PROBE ARM 1: UNEXPECTED" : "PROBE ARM 1: OK");
    return g_fail ? 1 : 0;
#else
    using namespace port;
    std::printf("== MP1 sleep/wake probe -- ARM 2: THE HOST PAIR ==\n");
    std::printf("linked: port/hal/os_thread.cpp; veneer src/func_02042778.c + "
                "src/func_0201a4d0.c are the ROM's own\n\n");

    // ---- 2a. NO PUMP: today's behaviour. Must return, must not hang. ------
    thread_stats_reset();
    thread_set_pump(nullptr);
    data_0209d4fc = 0;
    double t0 = now_ms();
    for (int i = 0; i < 1000; ++i) func_02042778();
    double t1 = now_ms();
    ThreadStats s = thread_stats();
    std::printf("2a NO PUMP (the port's behaviour today)\n");
    std::printf("   1000 sleeps through the veneer in %.3f ms "
                "(%.4f us each)\n", t1 - t0, (t1 - t0) * 1000.0 / 1000.0);
    std::printf("   sleeps=%llu immediate_returns=%llu pump_turns=%llu\n",
                (unsigned long long)s.sleeps,
                (unsigned long long)s.immediate_returns,
                (unsigned long long)s.pump_turns);
    CHECK(s.sleeps == 1000);
    CHECK(s.immediate_returns == 1000);
    CHECK(s.pump_turns == 0);
    CHECK(data_0209d4fc == 0);          // the bit is cleared on the way out

    // ---- 2b. PUMP, wake on the first turn --------------------------------
    thread_stats_reset();
    g_host_frames = 0;
    g_wake_after = 0;
    thread_set_pump(probe_pump);
    data_0209d4fc = 0;
    g_turns_this_wait = 0;
    t0 = now_ms();
    for (int i = 0; i < 1000; ++i) { g_turns_this_wait = 0; func_02042778(); }
    t1 = now_ms();
    s = thread_stats();
    std::printf("\n2b PUMP, woken on turn 1\n");
    std::printf("   1000 sleeps in %.3f ms (%.4f us each), host frames %llu\n",
                t1 - t0, (t1 - t0), (unsigned long long)g_host_frames);
    std::printf("   sleeps=%llu woken=%llu wakes=%llu pump_turns=%llu "
                "timeouts=%llu\n",
                (unsigned long long)s.sleeps, (unsigned long long)s.woken_sleeps,
                (unsigned long long)s.wakes, (unsigned long long)s.pump_turns,
                (unsigned long long)s.timeouts);
    CHECK(s.sleeps == 1000);
    CHECK(s.woken_sleeps == 1000);      // every sleep ended on a real wake
    CHECK(s.wakes == 1000);
    CHECK(s.pump_turns == 1000);        // exactly one host frame per sleep
    CHECK(s.timeouts == 0);

    // ---- 2c. PUMP, wake after 5 turns ------------------------------------
    thread_stats_reset();
    g_host_frames = 0;
    g_wake_after = 5;
    data_0209d4fc = 0;
    g_turns_this_wait = 0;
    func_02042778();
    s = thread_stats();
    std::printf("\n2c PUMP, wake delivered on turn 6\n");
    std::printf("   pump_turns=%llu woken=%llu timeouts=%llu\n",
                (unsigned long long)s.pump_turns,
                (unsigned long long)s.woken_sleeps,
                (unsigned long long)s.timeouts);
    CHECK(s.pump_turns == 6);
    CHECK(s.woken_sleeps == 1);
    CHECK(s.timeouts == 0);

    // ---- 2d. PUMP, wake NEVER delivered: must time out, must not hang ----
    thread_stats_reset();
    g_host_frames = 0;
    thread_set_pump_limit(50);
    g_deliver_wake = false;
    data_0209d4fc = 0;
    g_turns_this_wait = 0;
    t0 = now_ms();
    func_02042778();
    t1 = now_ms();
    s = thread_stats();
    std::printf("\n2d PUMP, wake NEVER delivered\n");
    std::printf("   returned after %llu turns in %.3f ms; timeouts=%llu\n",
                (unsigned long long)s.pump_turns, t1 - t0,
                (unsigned long long)s.timeouts);
    CHECK(s.pump_turns == 50);
    CHECK(s.timeouts == 1);
    CHECK(s.woken_sleeps == 0);
    thread_set_pump_limit(kPortThreadPumpLimitDefault);
    g_deliver_wake = true;

    // ---- 2e. nested sleep is refused, not recursed ------------------------
    thread_stats_reset();
    g_host_frames = 0;
    struct Nest {
        static bool pump(unsigned) {
            func_02042778();            // a sleep from inside the pump
            OS_WakeupThread(&data_0209d4fc);
            return true;
        }
    };
    thread_set_pump(Nest::pump);
    data_0209d4fc = 0;
    func_02042778();
    s = thread_stats();
    std::printf("\n2e nested sleep from inside the pump\n");
    std::printf("   sleeps=%llu nested_returns=%llu pump_turns=%llu\n",
                (unsigned long long)s.sleeps,
                (unsigned long long)s.nested_returns,
                (unsigned long long)s.pump_turns);
    CHECK(s.nested_returns == 1);
    CHECK(s.pump_turns == 1);
    thread_set_pump(probe_pump);

    // ---- 2f. pump_fiber outside rt_run: must refuse, not busy-spin -------
    thread_stats_reset();
    thread_set_pump(pump_fiber);
    data_0209d4fc = 0;
    func_02042778();
    s = thread_stats();
    std::printf("\n2f pump_fiber with no rt_run fiber (every shipped target)\n");
    std::printf("   pump_turns=%llu pump_stops=%llu timeouts=%llu\n",
                (unsigned long long)s.pump_turns,
                (unsigned long long)s.pump_stops,
                (unsigned long long)s.timeouts);
    std::printf("   -> rt.h:11-12's 'generalises to' is TRUE for the mechanism "
                "and FALSE for the shipped targets: rt_vblank_wait is a no-op "
                "when rt_run is not driving a fiber (rt.cpp:149), and nothing "
                "the port ships calls rt_run.\n");
    CHECK(s.pump_stops == 1);
    CHECK(s.timeouts == 0);
    thread_set_pump(probe_pump);

    // ---- 3. THE ROM'S LOCKSTEP -------------------------------------------
    std::printf("\n== PART 3: the ROM's lockstep, src/func_0203ea5c.c "
                "control skeleton ==\n");

    g_wake_after = 0;
    LockstepResult r;

    r = run_lockstep(/*link*/4, /*answer_after*/0, /*bound*/0x12C, true);
    std::printf("\n3a transport answers IMMEDIATELY (link state 4)\n");
    std::printf("   loop turns=%d  host frames=%llu  %.3f ms  role_after=%d "
                "completed=%d\n", r.turns, r.host_frames, r.ms, r.role_after,
                (int)r.completed);
    CHECK(r.turns == 1);
    CHECK(r.completed);
    CHECK(r.role_after == 2);          // still a child: the round arrived

    r = run_lockstep(4, 9, 0x12C, true);
    std::printf("\n3b transport answers after 10 turns\n");
    std::printf("   loop turns=%d  host frames=%llu  %.3f ms  role_after=%d "
                "completed=%d\n", r.turns, r.host_frames, r.ms, r.role_after,
                (int)r.completed);
    CHECK(r.turns == 10);
    CHECK(r.host_frames == 9);         // one sleep per unanswered turn
    CHECK(r.completed);
    CHECK(r.role_after == 2);

    r = run_lockstep(4, -1, 0x12C, true);
    std::printf("\n3c transport NEVER answers -- the decision case\n");
    std::printf("   loop turns=%d  host frames=%llu  %.3f ms  role_after=%d "
                "completed=%d\n", r.turns, r.host_frames, r.ms, r.role_after,
                (int)r.completed);
    std::printf("   -> role_after 0 is src/func_0203ea5c.c:423, "
                "data_020a0f04 = 0: the ROM's OWN silent drop back to solo.\n");
    CHECK(r.turns == 0x12C);
    CHECK(!r.completed);
    CHECK(r.role_after == 0);          // FELL BACK TO SOLO. Did not hang.

    r = run_lockstep(0, -1, 0x4B0, true);
    std::printf("\n3d link never comes up at all (state 0), pre-init bound\n");
    std::printf("   loop turns=%d  host frames=%llu  %.3f ms  role_after=%d\n",
                r.turns, r.host_frames, r.ms, r.role_after);
    CHECK(r.turns == 0x4B0);
    CHECK(r.role_after == 0);

    std::printf("\n%s  (%d failures)\n",
                g_fail ? "PROBE ARM 2: FAILED" : "PROBE ARM 2: OK", g_fail);
    return g_fail ? 1 : 0;
#endif
}
