/* THE PACE LOCK (run coopfix1, lane HITCH1): game ticks in step with the display.
 *
 * THE PROBLEM. The pacer (tests/walk_window.cpp frame_pace) runs the game on its
 * own clock: one DS vblank is 16.65 ms, a course tick is two of them, 33.30 ms.
 * A 60 Hz display refreshes every 16.667 ms, so two refreshes are 33.333 ms. The
 * two clocks differ by 0.033 ms a tick, and the moment a tick's picture is handed
 * over slides through the refresh by that much every tick: a whole refresh every
 * 500 ticks (16.7 s). While the hand-over sits near a refresh boundary, the
 * tick's own work (5 to 15 ms, different every tick) decides whether the picture
 * makes this refresh or the next, so pictures are held 1 or 3 refreshes instead
 * of 2, back and forth, for as long as the slide takes to cross the work's spread:
 * a few seconds of stutter every 16.7 s while the frame counter still reads 60.
 * That is the Steam Deck report word for word, and the dialog text stutter (a
 * scrolling line is where an uneven hold shows first).
 *
 * THE FIX. When the display's refresh period R is known and the pacer's budget B
 * is within 0.6% of k refreshes (60, 120, 240 Hz: k = 2, 4, 8 for a course tick;
 * 90 Hz: k = 3), the budget becomes exactly k * R, and the deadline grid is slewed
 * (at most half a millisecond a turn) so that a picture handed over after the
 * tick's usual work lands in the MIDDLE of a refresh. Then the work can vary by
 * almost half a refresh either way without changing which refresh shows it, and
 * every picture is held k refreshes. Nothing about a tick changes: the same
 * ticks run with the same input and the same physics; only the moment each one
 * starts moves (by at most 0.1% over a second: 30.03 ticks a second become
 * 30.00 at 60 Hz, where the cartridge runs 29.91). A refresh that is not near a
 * multiple (144 Hz: 4.8 refreshes a tick) is left exactly as before.
 *
 * WHERE R COMES FROM. dwmapi's DwmGetCompositionTimingInfo (the compositor's
 * own refresh period and the time of its last vblank), loaded with
 * LoadLibraryA/GetProcAddress so the import table does not move (see the rule at
 * the head of hal/perf_log.cpp). Without it (an old system, or a Wine that does
 * not fill it in), the display mode's refresh rate from EnumDisplaySettingsA,
 * which gives the period but no phase: the grid is still a whole number of
 * refreshes, so the hand-over stops sliding, and only its place inside the
 * refresh is unknown.
 *
 * KNOBS (all off unless set):
 *   SM64DS_PACE_LOCK=0        the lock is off (the pacer as it was); the
 *                             [pace] summary still reports what the display saw
 *   SM64DS_PACE_REFRESH_HZ=<hz> a synthetic refresh grid at <hz> instead of the
 *                             compositor's (proof rows on a display that is not
 *                             60 Hz; the summary measures against it too)
 *   SM64DS_PACE_LOG=1         one line per tick ("[pacelog]"), kept in memory and
 *                             written at exit, for a player to send us frame
 *                             timing (Steam: launch options
 *                             "SM64DS_PACE_LOG=1 %command%")
 *   SM64DS_PACE_SUM=0         no "[pace]" summary line (one every 30 s by default)
 */
#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hal/pace_lock.h"

namespace {

typedef HRESULT(WINAPI *DwmTimingFn)(HWND, DWM_TIMING_INFO *);
typedef BOOL(WINAPI *EnumDsFn)(LPCSTR, DWORD, DEVMODEA *);

enum { SRC_NONE = 0, SRC_KNOB, SRC_DWM, SRC_MODE };
const char *const kSrcName[] = {"none", "knob", "dwm", "mode"};

struct Refresh {
    long long period;   /* QPC counts, 0 = unknown */
    long long vblank;   /* QPC of a vblank, 0 = no phase */
    int src;
};

long long g_qpf;
int g_init;
int g_lock_on = 1, g_log_on, g_sum_on = 1;
double g_knob_hz;
DwmTimingFn g_dwm;
EnumDsFn g_enumds;
Refresh g_ref;
long long g_ref_at;            /* when g_ref was read (QPC) */

long long qpc_now(void)
{
    LARGE_INTEGER n;
    QueryPerformanceCounter(&n);
    return n.QuadPart;
}

void init_once(void)
{
    if (g_init) return;
    g_init = 1;
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    g_qpf = f.QuadPart;
    const char *e = getenv("SM64DS_PACE_LOCK");
    g_lock_on = (e && e[0] == '0') ? 0 : 1;
    e = getenv("SM64DS_PACE_LOG");
    g_log_on = (e && e[0] == '1') ? 1 : 0;
    e = getenv("SM64DS_PACE_SUM");
    g_sum_on = (e && e[0] == '0') ? 0 : 1;
    e = getenv("SM64DS_PACE_REFRESH_HZ");
    g_knob_hz = e ? atof(e) : 0.0;
    if (g_knob_hz < 20.0 || g_knob_hz > 500.0) g_knob_hz = 0.0;
    if (!g_knob_hz) {
        if (HMODULE d = LoadLibraryA("dwmapi.dll"))
            g_dwm = (DwmTimingFn)GetProcAddress(d, "DwmGetCompositionTimingInfo");
        if (HMODULE u = LoadLibraryA("user32.dll"))
            g_enumds = (EnumDsFn)GetProcAddress(u, "EnumDisplaySettingsA");
    }
}

/* The refresh, read at most four times a second (a vblank time plus a whole
   number of periods is every vblank, so a quarter-second-old reading is as good
   as a fresh one). */
const Refresh &refresh(void)
{
    init_once();
    const long long now = qpc_now();
    if (g_ref_at && now - g_ref_at < g_qpf / 4) return g_ref;
    g_ref_at = now;
    Refresh r = {0, 0, SRC_NONE};
    if (g_knob_hz) {
        static long long anchor;
        if (!anchor) anchor = now;
        r.period = (long long)((double)g_qpf / g_knob_hz + 0.5);
        r.vblank = anchor;
        r.src = SRC_KNOB;
    } else {
        if (g_dwm) {
            DWM_TIMING_INFO ti;
            memset(&ti, 0, sizeof ti);
            ti.cbSize = sizeof ti;
            if (SUCCEEDED(g_dwm(NULL, &ti)) && ti.qpcRefreshPeriod > 0 &&
                ti.qpcRefreshPeriod < (ULONGLONG)(g_qpf / 20)) {
                r.period = (long long)ti.qpcRefreshPeriod;
                r.vblank = (long long)ti.qpcVBlank;
                r.src = SRC_DWM;
            }
        }
        if (!r.period && g_enumds) {
            static long long anchor;
            DEVMODEA dm;
            memset(&dm, 0, sizeof dm);
            dm.dmSize = sizeof dm;
            if (g_enumds(NULL, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency >= 24 &&
                dm.dmDisplayFrequency <= 500) {
                if (!anchor) anchor = now;
                double hz = (double)dm.dmDisplayFrequency;
                if (dm.dmDisplayFrequency == 59 || dm.dmDisplayFrequency == 119) hz = (hz + 1.0) * 1000.0 / 1001.0;
                r.period = (long long)((double)g_qpf / hz + 0.5);
                r.vblank = 0;
                r.src = SRC_MODE;
            }
        }
    }
    if (r.src != g_ref.src || (g_ref.period && llabs(r.period - g_ref.period) * 200 > g_ref.period))
        fprintf(stderr, "[pace] display refresh %.3f Hz (source %s%s)%s\n",
                r.period ? (double)g_qpf / (double)r.period : 0.0, kSrcName[r.src],
                r.vblank ? ", with its vblank" : "", g_lock_on ? "" : "; SM64DS_PACE_LOCK=0: the lock is off");
    g_ref = r;
    return g_ref;
}

/* k refreshes per step, or 0 when the step is not near a whole number of them */
int lock_k(long long step, const Refresh &r)
{
    if (!g_lock_on || !r.period || step <= 0) return 0;
    const double ratio = (double)step / (double)r.period;
    const int k = (int)floor(ratio + 0.5);
    if (k < 1 || k > 16) return 0;
    if (fabs(ratio - (double)k) / ratio > 0.006) return 0;
    return k;
}

double ms_of(long long q) { return (double)q * 1000.0 / (double)g_qpf; }

/* ---- the summary and the per-tick log -------------------------------------- */
long long g_win_t0;            /* the window's start (QPC) */
int g_win_ticks, g_win_over, g_win_lockturns, g_win_turns;
double g_win_tick_sum, g_win_tick_max, g_win_work_sum;
int g_hold_hist[8];            /* refreshes a tick's first picture was held: 0..6, 7 = more */
int g_holds, g_pics, g_reps;
double g_err_abs_sum;
int g_err_n;
int g_burst, g_burst_max;      /* uneven holds in a row, the longest this window */
int g_last_tick = -0x7fffffff;
long long g_last_tick_pic;     /* QPC of the previous tick's first picture */
long long g_last_idx;          /* its refresh index */
int g_have_idx;
long long g_last_turn_at;
int g_k_now;

char *g_log;
size_t g_log_n, g_log_cap = 16u << 20;

void log_flush(void)
{
    if (g_log && g_log_n) { fwrite(g_log, 1, g_log_n, stderr); fflush(stderr); g_log_n = 0; }
}

void log_line(const char *fmt, double a, double b, double c, double d, double e, int i0, int i1)
{
    if (!g_log_on) return;
    if (!g_log) {
        g_log = (char *)malloc(g_log_cap);
        if (!g_log) { g_log_on = 0; return; }
        atexit(log_flush);
    }
    if (g_log_n + 200 > g_log_cap) log_flush();
    g_log_n += (size_t)_snprintf(g_log + g_log_n, 200, fmt, a, b, c, d, e, i0, i1);
}

void sum_maybe(long long now)
{
    if (!g_win_t0) { g_win_t0 = now; return; }
    if (now - g_win_t0 < g_qpf * 30) return;
    if (g_sum_on && g_win_ticks > 0) {
        const Refresh &r = g_ref;
        char h[96];
        int n = 0;
        for (int i = 0; i < 8; ++i)
            if (g_hold_hist[i]) n += _snprintf(h + n, sizeof h - n, " %d%s:%d", i, i == 7 ? "+" : "", g_hold_hist[i]);
        if (!n) _snprintf(h, sizeof h, " (no refresh phase to measure against)");
        fprintf(stderr, "[pace] %.1fs: %d ticks, tick %.2f ms avg %.2f max, work %.2f ms avg, %d overran | display %.3f Hz (%s), "
                "lock %s (k=%d, %d of %d turns) | refreshes each tick's picture was held:%s; longest uneven run %d ticks; "
                "phase error %.2f ms avg | pictures %d (%d repeats)\n",
                ms_of(now - g_win_t0) / 1000.0, g_win_ticks, g_win_tick_sum / g_win_ticks, g_win_tick_max,
                g_win_work_sum / g_win_ticks, g_win_over, r.period ? (double)g_qpf / (double)r.period : 0.0,
                kSrcName[r.src], g_win_lockturns ? "on" : "off", g_k_now, g_win_lockturns, g_win_turns, h, g_burst_max,
                g_err_n ? g_err_abs_sum / g_err_n : 0.0, g_pics, g_reps);
    }
    /* the per-tick lines go out with the summary, once in 30 s: a game that is
       closed by force (a crash, a killed process) keeps all but the last window */
    log_flush();
    g_win_t0 = now;
    g_win_ticks = g_win_over = g_win_lockturns = g_win_turns = 0;
    g_win_tick_sum = g_win_tick_max = g_win_work_sum = 0.0;
    memset(g_hold_hist, 0, sizeof g_hold_hist);
    g_holds = g_pics = g_reps = 0;
    g_err_abs_sum = 0.0;
    g_err_n = 0;
    g_burst_max = 0;
}

double g_work_ema = -1.0;      /* the tick's usual work (ms), for the slew target */
double g_tick_ema;             /* the tick's usual length (ms), for "uneven" */

}  // namespace

extern "C" long long pace_lock_step(long long step, long long qpf)
{
    (void)qpf;
    const Refresh &r = refresh();
    const int k = lock_k(step, r);
    g_k_now = k;
    ++g_win_turns;
    if (!k) return step;
    ++g_win_lockturns;
    return (long long)k * r.period;
}

extern "C" long long pace_lock_slew(long long next, long long qpf, double work_ms)
{
    (void)qpf;
    const Refresh &r = g_ref;
    if (!g_k_now || !r.period || !r.vblank) return 0;
    if (work_ms >= 0.0) {
        const double rm = ms_of(r.period);
        double w = work_ms > rm ? rm : work_ms;   /* an outlier moves the target only so far */
        g_work_ema = g_work_ema < 0.0 ? w : g_work_ema * 0.9 + w * 0.1;
    }
    const double w = g_work_ema < 0.0 ? 0.0 : g_work_ema;
    /* the picture is handed over w after the turn starts; it should land half
       a refresh after a vblank, so the turn starts at (R/2 - w) mod R */
    long long target = r.period / 2 - (long long)(w * (double)g_qpf / 1000.0);
    target %= r.period;
    if (target < 0) target += r.period;
    long long e = (next - r.vblank - target) % r.period;
    if (e < 0) e += r.period;
    if (e >= r.period / 2) e -= r.period;
    g_err_abs_sum += fabs(ms_of(e));
    ++g_err_n;
    long long c = e / 8;
    const long long cap = g_qpf / 2000;   /* half a millisecond */
    if (c > cap) c = cap;
    if (c < -cap) c = -cap;
    return c;
}

extern "C" void pace_lock_note_turn(double work_ms, double slept_ms, int overran, int tick)
{
    init_once();
    const long long now = qpc_now();
    static int turn_tick = -0x7fffffff;
    if (tick != turn_tick) {   /* the tick's first turn: its work ran just before it */
        turn_tick = tick;
        ++g_win_ticks;
        g_win_work_sum += work_ms;
        if (g_last_turn_at) {
            const double t = ms_of(now - g_last_turn_at);
            g_win_tick_sum += t;
            if (t > g_win_tick_max) g_win_tick_max = t;
            g_tick_ema = g_tick_ema > 0.0 ? g_tick_ema * 0.95 + t * 0.05 : t;
            log_line("[pacelog] turn %.2f len %.2f work %.2f slept %.2f k %.0f over %d tick %d\n", ms_of(now), t, work_ms,
                     slept_ms, (double)g_k_now, overran, tick);
        }
        g_last_turn_at = now;
    }
    if (overran) ++g_win_over;
    sum_maybe(now);
}

extern "C" void pace_lock_note_present(int repeat, int tick)
{
    init_once();
    const long long now = qpc_now();
    ++g_pics;
    if (repeat) ++g_reps;
    if (tick == g_last_tick) return;
    g_last_tick = tick;
    const Refresh &r = refresh();
    if (!r.period || !r.vblank) { g_have_idx = 0; return; }
    /* refreshes are counted from one fixed vblank, so a fresh reading of the
       compositor's clock does not renumber them */
    static long long anchor, anchor_period;
    if (!anchor || anchor_period != r.period) { anchor = r.vblank; anchor_period = r.period; g_have_idx = 0; }
    const long long base = (long long)floor((double)(r.vblank - anchor) / (double)r.period + 0.5);
    /* the first vblank at or after the hand-over shows it */
    const long long d = now - r.vblank;
    const long long in = d >= 0 ? (d + r.period - 1) / r.period : -((-d) / r.period);
    const long long idx = base + in;
    if (g_have_idx) {
        const long long hold = idx - g_last_idx;
        const int hb = hold < 0 ? 0 : (hold > 7 ? 7 : (int)hold);
        ++g_hold_hist[hb];
        ++g_holds;
        /* uneven: not the hold the tick's length asks for */
        const long long want = g_tick_ema > 0.0 ? (long long)floor(g_tick_ema / ms_of(r.period) + 0.5) : hold;
        if (hold != want) {
            if (++g_burst > g_burst_max) g_burst_max = g_burst;
        } else {
            g_burst = 0;
        }
        log_line("[pacelog] pic %.2f gap %.2f phase %.2f hold %.0f refresh %.3f tick %d %d\n", ms_of(now), ms_of(now - g_last_tick_pic),
                 ms_of(d - (in - 1) * r.period), (double)hold, ms_of(r.period), tick, repeat);
    }
    g_last_idx = idx;
    g_last_tick_pic = now;
    g_have_idx = 1;
}
