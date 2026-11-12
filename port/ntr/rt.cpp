#include "ntr/rt.h"

#include "ntr/mmio.h"
#include "hal/dsstate_seg.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

// runtime.cpp's side of the interrupt model. Declared here rather than in rt.h
// because it is the decomp's own symbol, not a host API.
extern "C" void *_ZN3IRQ13GetIRQHandlerEj(unsigned mask);

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace ntr {
namespace {

constexpr uint32_t REG_IME = 0x04000208;
constexpr uint32_t REG_IE = 0x04000210;
constexpr uint32_t REG_IF = 0x04000214;
constexpr uint32_t REG_DISPSTAT = 0x04000004;
constexpr uint32_t REG_VCOUNT = 0x04000006;
constexpr uint32_t REG_WIN0H = 0x04000040;      // WIN0H and WIN1H, one word
constexpr uint32_t REG_WIN0H_SUB = 0x04001040;
constexpr uint16_t DISPSTAT_HBLANK = 0x0002;    // the HBlank STATUS bit

inline volatile uint32_t &reg32(uint32_t a) {
    return *reinterpret_cast<volatile uint32_t *>(a);
}
inline volatile uint16_t &reg16(uint32_t a) {
    return *reinterpret_cast<volatile uint16_t *>(a);
}

struct Runtime {
    void *host_fiber = nullptr;
    void *game_fiber = nullptr;
    void (*entry)() = nullptr;
    FrameHook hook = nullptr;
    uint64_t frame = 0;
    uint64_t max_frames = 0;
    bool game_done = false;
    bool stop = false;
    uint32_t cpsr_i = 0;     // 0x80 when interrupts are masked
};

Runtime g;

#if defined(_WIN32)
void CALLBACK game_trampoline(void *) {
    g.entry();
    g.game_done = true;
    // The game returned. Hand control back for good.
    for (;;) SwitchToFiber(g.host_fiber);
}
#endif

// ---- THE HBLANK EDGE -------------------------------------------------------
//
// The DS raises IE/IF bit 1 once per scanline, for all 263 of them and not
// just the 192 visible ones, and the ROM's dWipe_c handler depends on both
// halves of that: lines 0..190 program the NEXT line's window bounds out of
// the motion table, and the first line past 191 runs the vblank-edge reset
// that swaps the double buffer. Delivering only the visible lines would build
// the table, read 191 rows of it and never swap.
//
// SM64DS_IRQ2_OFF=1 suppresses delivery on the same binary. It exists so the
// before/after is one build at one .dsstate base, which is the only comparison
// port/tools/battery.py's header lets a BMP carry, and so the pre-fix state
// stays reproducible after the fix. Same idiom as SM64DS_SUB_NO_SCENE_INIT.
bool hblank_off() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_IRQ2_OFF") ? 1 : 0;
    return v != 0;
}

bool hblank_trace() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_IRQ2_TRACE") ? 1 : 0;
    return v != 0;
}

unsigned long long g_hb_deliveries;
unsigned long long g_hb_window_writes;
bool g_hb_announced;
unsigned g_hb_gates_said;
// run link100, lane DET4: how many of the deliveries above saw the manager's
// pending flag go 0->1 during the handler's own call -- see hblank_line and
// the report this prints at exit for what that means.
unsigned long long g_hb_modeask;

// THE DEPTH HELPER, THROUGH A HOOK (run link100, lane DET4). hal/
// boot2_thread.cpp owns port_irq_mode_depth and the manager's pending flag
// this file wants to bracket the HBlank dispatch with (see that file's
// header, THE OTHER TWO DISPATCHES) -- but ntr.lib has to stay linkable into
// the smoke_* probes (port/CMakeLists.txt: smoke_gx, smoke_model,
// smoke_frames, smoke_soak, smoke_clsn, smoke_modelanim, smoke_oam,
// smoke_objwin, smoke_soak_anim, smoke_anim, smoke_actor, smoke_savestate,
// smoke_persist), NONE of which link hal/boot2_thread.cpp or
// hal/cxx_aliases.cpp. A direct extern reference to either symbol is an
// unresolved external on all of them (measured: this is exactly what broke
// before this file adopted the hook -- eight smoke_* link failures on
// port_irq_mode_enter/exit and data_020a6134, all through this TU). So:
// three hooks, null by default (a no-op bracket and an always-false
// mode-ask, which is this file's behaviour before this lane on every target
// that does not install them), that hal/boot2_thread.cpp wires up with a
// static registration -- see that file's IrqModeHookReg -- on every target
// that DOES link it (walk_window, walk_window_hires, smoke_player).
void (*g_irq_mode_enter)() = nullptr;
void (*g_irq_mode_exit)() = nullptr;
uint16_t (*g_irq_pending_flag_read)() = nullptr;

void hblank_irqmode_report() {
    std::fprintf(stderr,
        "[det4] hblank: %llu dispatch(es), %llu of them saw the manager's "
        "pending flag (m0) go 0->1 during the call -- func_02057f54's own "
        "deferral, i.e. a ROM body asked ARMProcessorMode() from inside THIS "
        "handler and got 0x12 (run link100, lane DET4; SM64DS_DET3 gates the "
        "depth this counts against, same as hal/boot2_thread.cpp's own "
        "VBlank site). Dispatch count is ntr::rt_hblank_counters' own "
        "deliveries; printed unconditionally, including when both are zero.\n",
        g_hb_deliveries, g_hb_modeask);
    std::fflush(stderr);
}
struct HblankIrqModeReportReg {
    HblankIrqModeReportReg() { std::atexit(hblank_irqmode_report); }
} g_hblank_irqmode_report_reg;

// One scanline's HBlank. The status bit is up for the blanking period and down
// again for the next line's visible period, which is what the handler's own
// `DISPSTAT & 2` test reads, and the pending flag follows the same edge.
void hblank_line() {
    const uint32_t before_main = *reinterpret_cast<volatile uint32_t *>(REG_WIN0H);
    const uint32_t before_sub = *reinterpret_cast<volatile uint32_t *>(REG_WIN0H_SUB);

    reg16(REG_DISPSTAT) |= DISPSTAT_HBLANK;
    reg32(REG_IF) |= IRQ_HBLANK;
    // run link100, lane DET4: the depth helper brackets this call the same
    // way hal/boot2_thread.cpp's own VBlank dispatch does, and the pending-
    // flag read either side of it is this site's mode-ask census. Both go
    // through the hooks above; see their own comment for why.
    const bool m0_before =
        g_irq_pending_flag_read && g_irq_pending_flag_read() != 0;
    if (g_irq_mode_enter) g_irq_mode_enter();
    rt_hblank_dispatch();
    if (g_irq_mode_exit) g_irq_mode_exit();
    if (!m0_before && g_irq_pending_flag_read && g_irq_pending_flag_read() != 0)
        ++g_hb_modeask;
    reg32(REG_IF) &= ~IRQ_HBLANK;
    reg16(REG_DISPSTAT) &= ~DISPSTAT_HBLANK;

    ++g_hb_deliveries;
    if (*reinterpret_cast<volatile uint32_t *>(REG_WIN0H) != before_main ||
        *reinterpret_cast<volatile uint32_t *>(REG_WIN0H_SUB) != before_sub)
        ++g_hb_window_writes;
}

// rt_window_rows' record (see rt.h): the WIN1H:WIN0H word of each engine for
// each visible line of the last scan, and whether a handler wrote any of them.
uint32_t g_win_rows[2][192];
bool g_win_rows_live;

inline void win_rows_take(int y) {
    g_win_rows[0][y] = *reinterpret_cast<volatile uint32_t *>(REG_WIN0H);
    g_win_rows[1][y] = *reinterpret_cast<volatile uint32_t *>(REG_WIN0H_SUB);
}

// Advance the scanline counter across a frame. Anything spinning on VCOUNT --
// the decomp does, in func_02013f4c -- needs this to actually move.
void run_scanlines() {
    const bool off = hblank_off();
    bool rows_written = false;
    win_rows_take(0);
    for (uint16_t line = 0; line < 263; ++line) {
        reg16(REG_VCOUNT) = line;
        if (off) continue;
        // Re-asked every line: a handler may disarm itself mid-frame, and the
        // ROM's own func_0202fb30 is exactly that.
        if (rt_hblank_armed()) {
            if (!g_hb_announced) {
                g_hb_announced = true;
                std::fprintf(stderr, "[irq2] HBlank edge live: handler %p, "
                             "first delivery at VCOUNT %u\n",
                             _ZN3IRQ13GetIRQHandlerEj(IRQ_HBLANK),
                             static_cast<unsigned>(line));
                std::fflush(stderr);
            }
            hblank_line();
            if (line < 191) rows_written = true;
        } else if (hblank_trace()) {
            const unsigned g = rt_hblank_gates();
            if (g != g_hb_gates_said && (g & HBLANK_GATE_HANDLER)) {
                g_hb_gates_said = g;
                std::fprintf(stderr, "[irq2] mask-2 handler registered but the "
                             "edge is SHUT: gates handler=%d ie=%d cpsr=%d "
                             "ime=%d dispstat=%d\n",
                             (g & HBLANK_GATE_HANDLER) != 0,
                             (g & HBLANK_GATE_IE) != 0,
                             (g & HBLANK_GATE_CPSR) != 0,
                             (g & HBLANK_GATE_IME) != 0,
                             (g & HBLANK_GATE_DISPSTAT) != 0);
                std::fflush(stderr);
            }
        }
        if (line < 191) win_rows_take(line + 1);
    }
    g_win_rows_live = rows_written;
}

}  // namespace

void rt_vblank_wait() {
#if defined(_WIN32)
    if (!g.game_fiber) return;   // not running under rt_run; nothing to yield to
    reg32(REG_IF) |= IRQ_VBLANK;
    SwitchToFiber(g.host_fiber);
#endif
}

uint64_t rt_frame() { return g.frame; }

// ---- DS TIMER 0, THE OS TICK (run hunt2, lane HUD2) -------------------------
//
// WHAT THE ROM DOES WITH IT. src/unnamed/arm9/0205/func_02059788.c (called by main() at boot,
// hal/boot_os.cpp port_boot_rom_main_head) zeroes TM0CNT_L, writes TM0CNT_H =
// 0xc1 (bit 7 run, bit 6 IRQ on overflow, prescaler 1 = F/64) and registers
// func_02059700 on IRQ mask 8, which bumps the 48-bit overflow count at
// data_020a6438 and re-arms itself. src/unnamed/arm9/0205/func_02059650.c reads the tick as
// (overflows << 16) | TM0CNT_L, and everything that measures time reads that:
// Timer::GetTime (the course timer on the HUD, src/_ZN3HUD15RenderTimeTimerEv.cpp,
// and the Princess's Secret Slide's under-21-second star, src/game/player/Player.cpp
// case 16), the tick-to-ms helpers, the alarm list.
//
// WHAT THE PORT DID. Nothing moved the counter and nothing raised mask 8, so
// the tick read 0 for the whole run: every Timer read 0, the course timer sat
// at 00'00"00 and the slide's time test always passed.
//
// THE MODEL. The counter lives where the ROM reads it, the mapped I/O word at
// 0x04000100, and moves by the DS's own clock: a frame is 263 lines of 2130
// cycles of the 33.51 MHz bus, and the game tick is data_0208ee44 frames (the
// ROM's vblanks-per-tick word; 2 on a course). The frame loops call this once
// per game tick, beside the frame clock (func_020197b8 phase 6), and not on a
// frame the host froze (its menu, a level re-seat), because no DS time passes
// in a frame the DS never ran. A start edge (bit 7 going up) starts the count
// at the value the ROM just wrote to TM0CNT_L, which is the reload the DS
// latches from that write; each overflow reloads it and sets IF bit 3, and
// when IE bit 3, IME and the CPSR I bit let it through the ROM's own vector
// runs (runtime.cpp rt_timer0_dispatch), acknowledged first the way the DS
// dispatcher acknowledges IF. A masked overflow stays pending in IF, as on
// the DS, and func_02059650 already counts a pending one.
//
// DETERMINISTIC: the count is a function of the ticks run, never of the host
// clock, so a selftest's pictures and traces do not move between runs.
namespace {
uint16_t g_t0_ctl_was;
uint16_t g_t0_reload;
uint32_t g_t0_frac;
unsigned long long g_t0_irqs;
int g_t0_band = -1;
struct Timer0CensusReg {
    Timer0CensusReg() {
        if (std::getenv("SM64DS_IRQ_CENSUS")) std::atexit(report);
    }
    static void report() {
        std::fprintf(stderr, "[timer0] census: overflow irqs delivered=%llu "
                     "TM0CNT_L=%04x TM0CNT_H=%04x\n", g_t0_irqs,
                     static_cast<unsigned>(reg16(0x04000100)),
                     static_cast<unsigned>(reg16(0x04000102)));
        std::fflush(stderr);
    }
} g_t0_census_reg;
}  // namespace

extern "C" int port_irqcb_band_ok(void);

void rt_timer0_advance(unsigned vblanks) {
    static int off = -1;
    if (off < 0) off = std::getenv("SM64DS_TIMER0_OFF") ? 1 : 0;
    if (off) return;
    if (g_t0_band < 0) {
        g_t0_band = port_irqcb_band_ok() ? 1 : 0;
        if (!g_t0_band) {
            std::fprintf(stderr, "[timer0] the IRQ callback band came apart "
                         "(data_020a60f4 != data_020a60c4 + 0x30): timer 0 "
                         "stays frozen rather than run the tick's re-arm "
                         "into the wrong words\n");
            std::fflush(stderr);
        }
    }
    if (!g_t0_band) return;
    if (vblanks < 1 || vblanks > 4) vblanks = 2;   // port_frame_divider's reading

    const uint16_t ctl = reg16(0x04000102);
    const bool was_running = (g_t0_ctl_was & 0x80) != 0;
    g_t0_ctl_was = ctl;
    if (!(ctl & 0x80)) return;
    if (!was_running) {
        g_t0_reload = reg16(0x04000100);
        g_t0_frac = 0;
    }

    static const unsigned kShift[4] = {0, 6, 8, 10};   // F/1, /64, /256, /1024
    const unsigned sh = kShift[ctl & 3];
    const uint64_t cycles = static_cast<uint64_t>(vblanks) * 560190u + g_t0_frac;
    g_t0_frac = static_cast<uint32_t>(cycles & ((1u << sh) - 1u));
    uint64_t cnt = reg16(0x04000100) + (cycles >> sh);
    unsigned overflows = 0;
    while (cnt >= 0x10000u) {
        cnt = cnt - 0x10000u + g_t0_reload;
        ++overflows;
    }
    reg16(0x04000100) = static_cast<uint16_t>(cnt);

    if (overflows && (ctl & 0x40)) reg32(REG_IF) |= 0x8u;
    // One edge per delivery; the IF bit is a latch, so overflows that land
    // while the gates are shut collapse into the one pending bit, as on the DS.
    for (unsigned i = 0; i < (overflows ? overflows : 1u); ++i) {
        if (!(reg32(REG_IF) & 0x8u) || !rt_timer0_irq_gates_open()) break;
        reg32(REG_IF) &= ~0x8u;
        if (g_irq_mode_enter) g_irq_mode_enter();
        rt_timer0_dispatch();
        if (g_irq_mode_exit) g_irq_mode_exit();
        ++g_t0_irqs;
        if (i + 1 < overflows) reg32(REG_IF) |= 0x8u;
    }
}

// ---- TIMER 0 IN A SAVE STATE (run hunt3, lane POLISH1) ----------------------
//
// The OS tick is (overflow count << 16) | TM0CNT_L. The overflow count
// (data_020a6438) and every Timer object are hosted DS globals and ride in
// .dsstate; the low half is the I/O word at 0x04000100, and the save state
// deliberately captures no I/O shadow (ntr/io.cpp, port_hw_regions_*). So an
// F9 put the overflow count back but left the counter where the live run had
// it: the course timer came back off by up to 65535 counts (1/8 s), and a
// state saved in the first 1/8 s after the slide's timer started could read
// a small negative time, which the HUD caps to 99'59"99 until the next
// overflow.
//
// What the timer needs is exactly this: the counter, its control word, the
// pending-overflow bit in IF, and this model's own three words (the control
// word it last saw, the reload it latched, the prescaler remainder), so the
// next rt_timer0_advance continues from the saved instant as if the run had
// never left it. hal/lk6_savestate.cpp copies them in here just before it
// captures .dsstate and puts them back just after it restores it; a disk load
// (hal/lk7_persist.cpp) puts them back after its own .dsstate copy, before it
// hands the world to the slot. Nothing else reads this record: the rollback
// ring's .dsstate copies carry it inert, as they carried nothing before.
// THE STATE LAYOUT CHANGES by this record's 20 bytes; a disk state from an
// earlier build is already refused by its gittip.
extern "C" {
DSSTATE_BEGIN
struct PortTimer0Saved {
    uint32_t magic;       // kT0Magic once a save has filled it
    uint16_t cnt_l;       // TM0CNT_L, the counter
    uint16_t cnt_h;       // TM0CNT_H, run / IRQ / prescaler
    uint16_t ctl_was;     // g_t0_ctl_was
    uint16_t reload;      // g_t0_reload
    uint16_t if_tm0;      // IF bit 3, a pending overflow
    uint16_t pad;
    uint32_t frac;        // g_t0_frac
};
PortTimer0Saved port_timer0_saved;
DSSTATE_END
}
static const uint32_t kT0Magic = 0x56533054u;   // "T0SV"

extern "C" void port_timer0_state_save(void) {
    port_timer0_saved.magic = kT0Magic;
    port_timer0_saved.cnt_l = reg16(0x04000100);
    port_timer0_saved.cnt_h = reg16(0x04000102);
    port_timer0_saved.ctl_was = g_t0_ctl_was;
    port_timer0_saved.reload = g_t0_reload;
    port_timer0_saved.if_tm0 = static_cast<uint16_t>(reg32(REG_IF) & 0x8u);
    port_timer0_saved.pad = 0;
    port_timer0_saved.frac = g_t0_frac;
}

extern "C" void port_timer0_state_load(void) {
    if (port_timer0_saved.magic != kT0Magic) return;   // saved before this record existed
    reg16(0x04000100) = port_timer0_saved.cnt_l;
    reg16(0x04000102) = port_timer0_saved.cnt_h;
    g_t0_ctl_was = port_timer0_saved.ctl_was;
    g_t0_reload = port_timer0_saved.reload;
    reg32(REG_IF) = (reg32(REG_IF) & ~0x8u) | (port_timer0_saved.if_tm0 & 0x8u);
    g_t0_frac = port_timer0_saved.frac;
}

uint32_t rt_irq_disable() {
    const uint32_t prev = g.cpsr_i;
    g.cpsr_i = 0x80;
    return prev;
}

uint32_t rt_irq_enable() {
    const uint32_t prev = g.cpsr_i;
    g.cpsr_i = 0;
    return prev;
}

uint32_t rt_irq_restore(uint32_t prev) {
    const uint32_t was = g.cpsr_i;
    g.cpsr_i = prev & 0x80;
    return was;
}

bool rt_irq_masked() { return g.cpsr_i != 0; }

void rt_scanout_frame() { run_scanlines(); }

bool rt_window_rows(int engine, const uint32_t **rows) {
#ifndef PORT_ROM_CLEAN
    /* SM64DS_WIPE_FAKE_CIRCLE=<radius>: test-only (FIELDUI1 U3). Reports the per-line window rows the ROM's type-2 circle wipe builds
       (src/runtime/graphics/fader/d_wipe.cpp func_0202ee94) for that radius, so the smooth circle cut can be pictured on any scene. */
    if (engine >= 0 && engine <= 1 && rows) {
        static int fake = -2;
        static uint32_t fr[192];
        if (fake == -2) { const char *e = std::getenv("SM64DS_WIPE_FAKE_CIRCLE"); fake = e ? std::atoi(e) : -1; }
        if (fake > 0) {
            for (int y = 0; y < 192; ++y) {
                const int i = y < 96 ? y : 191 - y;
                int lo = 0x80, hi = 0x7f;      /* the table's own fill (0x80807f7f): both windows empty */
                const int a = i - 96 + fake;
                if (i >= 96 - fake && a >= 0) {
                    int sq = (int)std::sqrt((double)(a * (2 * fake - a)));
                    lo = 0x80 - sq; hi = 0x80 + sq;
                    if (lo < 0) lo = 0;
                    if (hi > 0xff) hi = 0;
                }
                /* WIN0H = x1 0x7f, x2 hi ; WIN1H = x1 lo, x2 0x80 (the table's a b c d bytes) */
                const uint32_t w0 = (uint32_t)((0x7f << 8) | (hi & 0xFF));
                const uint32_t w1 = (uint32_t)(((lo & 0xFF) << 8) | 0x80);
                fr[y] = w0 | (w1 << 16);
            }
            *rows = fr;
            return true;
        }
    }
#endif
    if (!g_win_rows_live || engine < 0 || engine > 1 || !rows) return false;
    *rows = g_win_rows[engine];
    return true;
}

void rt_hblank_counters(unsigned long long *deliveries,
                        unsigned long long *window_writes) {
    if (deliveries) *deliveries = g_hb_deliveries;
    if (window_writes) *window_writes = g_hb_window_writes;
}

// run link100, lane DET4. Read by port/tests/walk_window.cpp's own census
// line, which ties this count to a run's frame number; the report in this
// file (printed at process exit) is what carries it on the scene and
// captured-pair paths, which do not go through that file's exit block.
void rt_hblank_modeask(unsigned long long *modeask) {
    if (modeask) *modeask = g_hb_modeask;
}

// run link100, lane DET4. hal/boot2_thread.cpp calls this once, at static
// init, on every target that links it -- see that file's IrqModeHookReg. Any
// argument left null (as all three are on every smoke_* probe, which never
// calls this at all) makes the corresponding bracket/read in hblank_line a
// no-op, which is this file's behaviour before this lane.
void rt_install_irq_mode_hook(void (*enter)(), void (*exit)(),
                              uint16_t (*pending_flag_read)()) {
    g_irq_mode_enter = enter;
    g_irq_mode_exit = exit;
    g_irq_pending_flag_read = pending_flag_read;
}

// THE DS COMES UP WITH IME SET, and until this lane nothing said so outside
// rt_run. THE SOURCE FOR THAT IS src/unnamed/arm9/0201/func_0201a054.c, the game's own IRQ init,
// which does EnableIRQs(1) then IME = 1 then IRQ::Enable -- the exact pair
// below, in that order. It is in NO SLICE, which is precisely why the host has
// to stand in for it. (rt_run's line used to credit "the CRT0"; that was
// unsourced and the ROM body is better evidence. See port/irq2_map.txt
// section 2, which also has the seat's retirement condition.)
//
// It mattered because the ROM's arming code brackets SetIRQHandler in
// `saved = IME; IME = 0; ...; if (saved) IME = 1`, so on a host that booted
// with IME at zero the bracket LEAVES IT AT ZERO and the interrupt it just
// armed can never be delivered. Hoisted here so the frame loops that do not
// run on the fiber (walk_window's level loop, port_scene_run) get the same
// power-on state. Idempotent, and it does not touch IE bit 1: only the game
// arms HBlank.
//
// SM64DS_IRQ2_NO_IME=1 skips the seat, which reproduces the pre-lane state on
// the shipped binary. It is here so the claim "IME was zero and the ROM could
// not raise it" stays a MEASUREMENT after the fix instead of decaying into an
// enumeration of writers. Run it with SM64DS_IRQ2_TRACE=1 to see which gate
// goes dark. See port/irq2_map.txt section 2.
void rt_irq_boot_state() {
    static bool done;
    if (done) return;
    done = true;
    if (std::getenv("SM64DS_IRQ2_NO_IME")) {
        std::fprintf(stderr, "[irq2] SM64DS_IRQ2_NO_IME=1: IME left at its "
                     "host default (the pre-fix behaviour)\n");
        std::fflush(stderr);
        return;
    }
    reg32(REG_IME) = 1;
    reg32(REG_IE) |= IRQ_VBLANK;
}

uint64_t rt_run(void (*game)(), FrameHook hook, uint64_t max_frames) {
#if !defined(_WIN32)
    (void)game; (void)hook; (void)max_frames;
    std::fprintf(stderr, "rt_run: fiber backend is Windows-only so far\n");
    return 0;
#else
    if (!io_init()) {
        std::fprintf(stderr, "rt_run: io_init failed\n");
        return 0;
    }

    g = Runtime{};
    g.entry = game;
    g.hook = hook;
    g.max_frames = max_frames;

    g.host_fiber = ConvertThreadToFiber(nullptr);
    if (!g.host_fiber) {
        std::fprintf(stderr, "rt_run: ConvertThreadToFiber failed\n");
        return 0;
    }
    g.game_fiber = CreateFiber(256 * 1024, game_trampoline, nullptr);
    if (!g.game_fiber) {
        std::fprintf(stderr, "rt_run: CreateFiber failed\n");
        return 0;
    }

    // The DS comes up with interrupts enabled and IME set. This line used to
    // say "by the CRT0", which was folklore; the sourced version is in
    // rt_irq_boot_state's own comment.
    rt_irq_boot_state();

    while (!g.game_done && !g.stop) {
        SwitchToFiber(g.game_fiber);          // run until the game blocks
        if (g.game_done) break;

        run_scanlines();
        ++g.frame;
        if (g.hook && !g.hook(g.frame)) g.stop = true;
        if (g.max_frames && g.frame >= g.max_frames) g.stop = true;

        // The handler is what clears the pending flag on real hardware.
        reg32(REG_IF) &= ~IRQ_VBLANK;
    }

    DeleteFiber(g.game_fiber);
    g.game_fiber = nullptr;
    ConvertFiberToThread();
    return g.frame;
#endif
}

}  // namespace ntr
