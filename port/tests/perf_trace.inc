/* ---- SM64DS_PERF_TRACE: WHERE A FRAME'S TIME GOES (run perf2, lane PERFMAP)
 *
 * A measuring instrument, inert unless the variable is set. Unset, every hook
 * below is one test of a cached int (g_pt_on == 0) and returns; nothing is
 * allocated, no thread is started, no file is written, and the game's
 * pictures and timing are the same statement for statement.
 *
 *   SM64DS_PERF_TRACE=1  stage timers. The frame loops call pt_mark(stage) at
 *                        each boundary; the time since the previous mark is
 *                        charged to the stage that was running. One
 *                        QueryPerformanceCounter per mark (about twenty a
 *                        frame). pt_push / pt_pop nest a stage inside another
 *                        (present() inside the pacer's wait, the SmoothMotion
 *                        build). Per frame the stage times are kept, and at
 *                        exit perf_frames.tsv (one row per frame) is written to
 *                        the working directory with a summary on stderr.
 *   SM64DS_PERF_TRACE=2  the timers plus a SAMPLING PROFILER: a thread that
 *                        wakes every millisecond (a high-resolution waitable
 *                        timer), suspends each other thread of this process in
 *                        turn, reads its instruction pointer and resumes it.
 *                        Nothing is allocated and no lock is taken while a
 *                        thread is held. Each sample keeps the thread's index
 *                        and the main loop's stage at that moment. At exit
 *                        perf_samples.bin (20-byte records), perf_threads.tsv and
 *                        perf_modules.tsv are written; the offline resolver
 *                        maps the addresses through walk_window.map.
 *
 * Startup milestones (either value): "[perf] ms <t> <name> ws <MB> priv <MB>",
 * t measured from the process's own creation time, so the CRT start-up before
 * main is inside the first number.
 *
 * Every allocation is VirtualAlloc at arm time: nothing here adds to the
 * image's static size (the image must end below 0x02000000). kernel32 only.
 */
#include <tlhelp32.h>
#include "hal/hitch_probe.h"   /* HITCH9C */

enum PerfStage {
    PS_BOOT = 0,      /* before the first frame, and between loops */
    PS_HOST_IN,       /* level loop: host input, pad, keys, menus, pre-tick */
    PS_ROM_FRAME,     /* level loop: port_actor_frame(1) = the ROM's five walks
                         (behaviour, collision, the render walk and its gx
                         command decode + vertex transform) */
    PS_HOST_POST,     /* level loop: after the ROM frame to the camera phase */
    PS_CAMERA,        /* level loop: PH_CAMERA */
    PS_SUBMIT,        /* level loop: PH_SUBMIT (host geometry, player) */
    PS_RASTER,        /* level loop: fb clear + ntr::gx_render (sw or card) */
    PS_COMP_A,        /* level loop: port_message_composite_engine_a */
    PS_SUB,           /* level loop: hal_sub_screen_present (bottom screen) */
    PS_FADE_OVL,      /* level loop: fade composite, host overlays, toasts */
    PS_PRESENT,       /* present(): the tick's own picture to the window */
    PS_POST,          /* after the picture, before the pace: stats, events */
    PS_SOUND,         /* sdat_host_tick: the hosted ARM7 + mixer + waveOut */
    PS_PACE,          /* frame_pace: the pacer's wait (sleep or spin) */
    PS_PRESENT_X,     /* present() called from inside the pacer (FrameRate) */
    PS_SMOOTH,        /* SmoothMotion's replay + blend build */
    PS_SCENE_IN,      /* scene loop: host input */
    PS_SCENE_TICK,    /* scene loop: port_scene_tick (ROM + render + sound) */
    PS_SCENE_PRESENT, /* scene loop: scene_host_present_frame (2D + present) */
    PS_COUNT
};
static const char *const k_ps_name[PS_COUNT] = {
    "boot", "host_in", "rom_frame", "host_post", "camera", "submit",
    "raster", "comp_a", "sub", "fade_ovl", "present", "post", "sound",
    "pace", "present_x", "smooth", "scene_in", "scene_tick", "scene_present"
};

static double g_hl_thr = 28.0;     /* HITCH9C: the hitch log's threshold, ms of work */
static int g_hl_starsel;           /* HITCH1: star-select frames run inside this level frame (tests/walk_window.cpp port_interlude_frame) */
static int g_pt_on = -1;          /* -1 unread, 0 off, 1 timers, 2 +sampler */
static volatile int g_pt_stage = PS_BOOT;
static long long g_pt_last;       /* QPC of the previous mark */
static double g_pt_qpf_ms;        /* ms per QPC tick */
static double g_pt_cur[PS_COUNT]; /* this frame's per-stage ms */
static int g_pt_stack[8];
static int g_pt_sp;
static int g_pt_kind = -1;        /* 0 level, 1 scene, -1 none yet */
static int g_pt_level;
static long long g_pt_frame_t0;

struct PtFrame {
    float ms[PS_COUNT];
    float wall;                   /* frame start to next frame start */
    int level;                    /* port level id, or -1 - scene id */
    unsigned char kind;
};
enum { PT_MAX_FRAMES = 40000 };
static PtFrame *g_pt_frames;
static int g_pt_nframes;

/* sampler */
struct PtSample {
    unsigned eip;
    unsigned caller[3];           /* main thread outside the exe: the first
                                     three exe-range words on its stack */
    unsigned char thread;
    unsigned char stage;
    unsigned short frame_lo;
};
enum { PT_MAX_SAMPLES = 2 * 1024 * 1024, PT_MAX_THREADS = 64 };
static PtSample *g_pt_samp;
static volatile long g_pt_nsamp;
static DWORD g_pt_tids[PT_MAX_THREADS];
static int g_pt_ntids;
static DWORD g_pt_main_tid, g_pt_sampler_tid;
static volatile long g_pt_stop;
static HANDLE g_pt_sampler;
static long long g_pt_samp_busy_qpc;   /* the sampler's own time holding threads */
static long g_pt_ticks;
static unsigned g_pt_exe_lo, g_pt_exe_hi;      /* the exe's code range */
static unsigned g_pt_stackbuf[1024];
static int g_pt_deep;     /* SM64DS_PERF_DEEP=1 (OPTIM1): stack read for every main-thread sample; set at arm time, never in the sampler */

static int pt_on(void)
{
    if (g_pt_on < 0) {
        const char *e = getenv("SM64DS_PERF_TRACE");
        /* HITCH9C: with the variable unset the instrument runs in mode 3, the HITCH LOG: the stage
           timers only (no frame table, no sampler, no file), and a "[hitch]" line on stderr for a
           frame whose work ran past SM64DS_HITCH_MS (default 28). SM64DS_HITCH_MS=0 turns it off. */
        g_pt_on = e ? atoi(e) : 3;
        { const char *dv = getenv("SM64DS_PERF_DEEP"); g_pt_deep = (dv && *dv == '1') ? 1 : 0; }
        if (g_pt_on < 0) g_pt_on = 0;
        if (g_pt_on == 3) {
            const char *h = getenv("SM64DS_HITCH_MS");
            g_hl_thr = h && *h ? atof(h) : 28.0;
            if (g_hl_thr <= 0.0) g_pt_on = 0;
        }
    }
    return g_pt_on;
}

static long long pt_qpc(void)
{
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return t.QuadPart;
}

static inline void pt_mark(int stage)
{
    if (g_pt_on <= 0) return;
    const long long now = pt_qpc();
    g_pt_cur[g_pt_stage] += (now - g_pt_last) * g_pt_qpf_ms;
    g_pt_last = now;
    g_pt_stage = stage;
}

static inline void pt_push(int stage)
{
    if (g_pt_on <= 0) return;
    if (g_pt_sp < 8) g_pt_stack[g_pt_sp++] = g_pt_stage;
    pt_mark(stage);
}

static inline void pt_pop(void)
{
    if (g_pt_on <= 0) return;
    pt_mark(g_pt_sp > 0 ? g_pt_stack[--g_pt_sp] : PS_BOOT);
}

/* process-relative wall clock: ms since this process was created */
static double pt_since_create_ms(void)
{
    FILETIME c, e, k, u, now;
    if (!GetProcessTimes(GetCurrentProcess(), &c, &e, &k, &u)) return -1.0;
    GetSystemTimePreciseAsFileTime(&now);
    ULARGE_INTEGER a, b;
    a.LowPart = c.dwLowDateTime; a.HighPart = c.dwHighDateTime;
    b.LowPart = now.dwLowDateTime; b.HighPart = now.dwHighDateTime;
    return (double)(b.QuadPart - a.QuadPart) / 10000.0;
}

struct PtMemCounters {   /* PROCESS_MEMORY_COUNTERS_EX, declared here so no psapi.h */
    DWORD cb, PageFaultCount;
    SIZE_T PeakWorkingSetSize, WorkingSetSize, QuotaPeakPagedPoolUsage,
        QuotaPagedPoolUsage, QuotaPeakNonPagedPoolUsage,
        QuotaNonPagedPoolUsage, PagefileUsage, PeakPagefileUsage,
        PrivateUsage;
};
static void pt_mem(double *ws_mb, double *priv_mb, double *peak_ws_mb)
{
    typedef BOOL(WINAPI * Fn)(HANDLE, PtMemCounters *, DWORD);
    static Fn fn = (Fn)GetProcAddress(GetModuleHandleA("kernel32.dll"),
                                      "K32GetProcessMemoryInfo");
    *ws_mb = *priv_mb = *peak_ws_mb = 0.0;
    if (!fn) return;
    PtMemCounters m;
    memset(&m, 0, sizeof m);
    m.cb = sizeof m;
    if (fn(GetCurrentProcess(), &m, sizeof m)) {
        *ws_mb = m.WorkingSetSize / 1048576.0;
        *priv_mb = m.PrivateUsage / 1048576.0;
        *peak_ws_mb = m.PeakWorkingSetSize / 1048576.0;
    }
}

static double pt_ft_s(const FILETIME &f)
{
    ULARGE_INTEGER u;
    u.LowPart = f.dwLowDateTime; u.HighPart = f.dwHighDateTime;
    return u.QuadPart / 1e7;
}

static void pt_milestone(const char *name)
{
    if (pt_on() <= 0 || g_pt_on == 3) return;
    double ws, pv, pk;
    pt_mem(&ws, &pv, &pk);
    FILETIME c, e, k, u;
    double pk_s = 0, pu_s = 0, tk_s = 0, tu_s = 0;
    if (GetProcessTimes(GetCurrentProcess(), &c, &e, &k, &u)) {
        pk_s = pt_ft_s(k); pu_s = pt_ft_s(u);
    }
    if (GetThreadTimes(GetCurrentThread(), &c, &e, &k, &u)) {
        tk_s = pt_ft_s(k); tu_s = pt_ft_s(u);
    }
    fprintf(stderr, "[perf] ms %9.1f %-28s ws %7.1f priv %7.1f peakws %7.1f "
            "cpu proc u %.2f k %.2f thread u %.2f k %.2f\n",
            pt_since_create_ms(), name, ws, pv, pk, pu_s, pk_s, tu_s, tk_s);
    fflush(stderr);
}

static void pt_refresh_threads(void)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    THREADENTRY32 te;
    te.dwSize = sizeof te;
    const DWORD pid = GetCurrentProcessId();
    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID != pid) continue;
            if (te.th32ThreadID == g_pt_sampler_tid) continue;
            int known = 0;
            for (int i = 0; i < g_pt_ntids; ++i)
                if (g_pt_tids[i] == te.th32ThreadID) { known = 1; break; }
            if (!known && g_pt_ntids < PT_MAX_THREADS)
                g_pt_tids[g_pt_ntids++] = te.th32ThreadID;
            te.dwSize = sizeof te;
        } while (Thread32Next(snap, &te));
    }
    CloseHandle(snap);
}

static DWORD WINAPI pt_sampler_main(void *)
{
    /* the sampler outranks the game so its 1 ms tick is kept under load */
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    HANDLE tmr = CreateWaitableTimerExW(NULL, NULL,
                                        0x00000002 /* HIGH_RESOLUTION */,
                                        TIMER_ALL_ACCESS);
    if (!tmr) tmr = CreateWaitableTimerExW(NULL, NULL, 0, TIMER_ALL_ACCESS);
    HANDLE th[PT_MAX_THREADS];
    for (int i = 0; i < PT_MAX_THREADS; ++i) th[i] = 0;
    int opened = 0;
    long tick = 0;
    while (!g_pt_stop) {
        LARGE_INTEGER due;
        due.QuadPart = -10000;   /* 1 ms, relative */
        if (tmr && SetWaitableTimer(tmr, &due, 0, NULL, NULL, FALSE))
            WaitForSingleObject(tmr, 50);
        else
            Sleep(1);
        if ((tick++ & 255) == 0) {
            pt_refresh_threads();
            for (; opened < g_pt_ntids; ++opened)
                th[opened] = OpenThread(THREAD_SUSPEND_RESUME |
                                            THREAD_GET_CONTEXT |
                                            THREAD_QUERY_INFORMATION,
                                        FALSE, g_pt_tids[opened]);
        }
        const long long t0 = pt_qpc();
        const int stage = g_pt_stage;
        const unsigned short flo = (unsigned short)g_pt_nframes;
        for (int i = 0; i < opened; ++i) {
            if (!th[i]) continue;
            if (g_pt_nsamp >= PT_MAX_SAMPLES) break;
            if (SuspendThread(th[i]) == (DWORD)-1) {
                CloseHandle(th[i]);   /* the thread has gone */
                th[i] = 0;
                continue;
            }
            CONTEXT cx;
            cx.ContextFlags = CONTEXT_CONTROL;
            const BOOL ok = GetThreadContext(th[i], &cx);
            unsigned eip = 0, sp = 0;
#if defined(_M_IX86)
            eip = (unsigned)cx.Eip; sp = (unsigned)cx.Esp;
#else
            eip = (unsigned)cx.Rip; sp = (unsigned)cx.Rsp;
#endif
            SIZE_T got = 0;
            /* the main thread outside the exe (ntdll, a driver): copy the top
               of its stack while it is still held. ReadProcessMemory is a
               system call on our own address space, takes no user-mode lock,
               and fails cleanly on an uncommitted page. */
            /* SM64DS_PERF_DEEP=1 (OPTIM1): read the main thread's stack for every sample, so a leaf inside the exe's own
               runtime (the big memcpy) also names the callers above it */
            const int want_stack = ok && g_pt_tids[i] == g_pt_main_tid &&
                                   (g_pt_deep || eip < g_pt_exe_lo || eip >= g_pt_exe_hi);
            if (want_stack &&
                !ReadProcessMemory(GetCurrentProcess(), (void *)(size_t)sp,
                                   g_pt_stackbuf, sizeof g_pt_stackbuf, &got))
                if (!ReadProcessMemory(GetCurrentProcess(), (void *)(size_t)sp,
                                       g_pt_stackbuf, 256, &got))
                    got = 0;
            ResumeThread(th[i]);
            if (!ok) continue;
            PtSample &s = g_pt_samp[g_pt_nsamp];
            s.eip = eip;
            s.caller[0] = s.caller[1] = s.caller[2] = 0;
            {
                int n = 0;
                for (SIZE_T w = 0; w < got / 4 && n < 3; ++w) {
                    const unsigned v = g_pt_stackbuf[w];
                    if (v >= g_pt_exe_lo && v < g_pt_exe_hi) s.caller[n++] = v;
                }
            }
            s.thread = (unsigned char)i;
            s.stage = (unsigned char)stage;
            s.frame_lo = flo;
            ++g_pt_nsamp;
        }
        g_pt_samp_busy_qpc += pt_qpc() - t0;
        ++g_pt_ticks;
    }
    for (int i = 0; i < opened; ++i)
        if (th[i]) CloseHandle(th[i]);
    if (tmr) CloseHandle(tmr);
    return 0;
}

/* HITCH9C: the hitch log. Called with the frame's stage times just closed; prints one line when the
   frame's work (everything but the pacer's wait) ran past the threshold, naming the stages and the
   named costs (hal/hitch_probe.h) that account for it. Always clears the named costs for the next frame. */
static void hl_close(long long now)
{
    static unsigned printed, frames;
    static double t_first_ms = -1.0;
    ++frames;
    /* the tick's own work: everything but the pacer's wait and the repeated pictures the pacer builds and
       presents inside it (FrameRate above the tick rate); those two are judged on their own below */
    double work = 0.0;
    for (int s = 0; s < PS_COUNT; ++s)
        if (s != PS_PACE && s != PS_PRESENT_X && s != PS_SMOOTH) work += g_pt_cur[s];
    const double repeats = g_pt_cur[PS_PRESENT_X] + g_pt_cur[PS_SMOOTH];
    if (work > g_hl_thr || repeats > g_hl_thr) {
        const double wall = (now - g_pt_frame_t0) * g_pt_qpf_ms;
        const int load = g_pt_cur[PS_HOST_IN] > 150.0 || g_pt_cur[PS_SCENE_IN] > 150.0;
        if (load || printed < 400) {
            char b[720];
            /* HITCH1: a level frame that held the star select is the player choosing an act on a running, paced
               screen (its own frames, input and pictures), not a stalled loop: say so on the line */
            char ss[48] = "";
            if (g_hl_starsel) _snprintf(ss, sizeof ss, " STARSEL(%d frames ran inside)", g_hl_starsel);
            int n = _snprintf(b, sizeof b, "[hitch] t=%.1fs frame=%u lvl=%d work=%.1f wall=%.1f%s%s |",
                              pt_since_create_ms() / 1000.0, frames, g_pt_level, work, wall, load ? " LOAD" : "", ss);
            for (int s = 0; s < PS_COUNT && n > 0 && n < 400; ++s)
                if (s != PS_PACE && g_pt_cur[s] >= 1.5)
                    n += _snprintf(b + n, sizeof b - n, " %s=%.1f", k_ps_name[s], g_pt_cur[s]);
            if (n > 0 && n < 400) n += _snprintf(b + n, sizeof b - n, " |");
            for (int k = 0; k < port::HK_COUNT && n > 0 && n < 660; ++k)
                if (port::g_hitch_cell[k].n && port::g_hitch_cell[k].ms >= 0.3)
                    n += _snprintf(b + n, sizeof b - n, " %s=%ux/%.1fms", port::kHitchKindName[k],
                                   port::g_hitch_cell[k].n, port::g_hitch_cell[k].ms);
            fprintf(stderr, "%s\n", b);
            if (!load) ++printed;
            (void)t_first_ms;
        }
    }
#ifndef PORT_ROM_CLEAN
    /* TITLEPERF1 (dev only, SM64DS_TP_SECT=<frames per line>): the mean cost of every named section and of every stage over
       a window of frames, printed as one "[tpsect]" line. Frames with a gap over 150 ms (boot, loads) are skipped. */
    {
        static int on = -1;
        static double sk[port::HK_COUNT], ss[PS_COUNT], swork;
        static int nf;
        if (on < 0) { const char *e = getenv("SM64DS_TP_SECT"); on = e ? atoi(e) : 0; }
        if (on > 0 && frames > 120 && work < 150.0) {
            for (int k = 0; k < port::HK_COUNT; ++k) sk[k] += port::g_hitch_cell[k].ms;
            for (int q = 0; q < PS_COUNT; ++q) ss[q] += g_pt_cur[q];
            swork += work;
            if (++nf >= on) {
                char b[900]; int n = _snprintf(b, sizeof b, "[tpsect] f%u n=%d work=%.2f |", frames, nf, swork / nf);
                for (int q = 0; q < PS_COUNT && n > 0 && n < 700; ++q)
                    if (q != PS_PACE && ss[q] / nf >= 0.005) n += _snprintf(b + n, sizeof b - n, " %s=%.2f", k_ps_name[q], ss[q] / nf);
                if (n > 0 && n < 800) n += _snprintf(b + n, sizeof b - n, " |");
                for (int k = 0; k < port::HK_COUNT && n > 0 && n < 880; ++k)
                    if (sk[k] / nf >= 0.005) n += _snprintf(b + n, sizeof b - n, " %s=%.2f", port::kHitchKindName[k], sk[k] / nf);
                fprintf(stderr, "%s\n", b);
                nf = 0; swork = 0;
                for (int k = 0; k < port::HK_COUNT; ++k) sk[k] = 0;
                for (int q = 0; q < PS_COUNT; ++q) ss[q] = 0;
            }
        }
    }
#endif
    for (int k = 0; k < port::HK_COUNT; ++k) port::g_hitch_cell[k].ms = 0.0, port::g_hitch_cell[k].n = 0;
    g_hl_starsel = 0;
}

static void pt_close_frame(long long now)
{
    if (g_pt_kind < 0) return;
    if (g_pt_on == 3) {
        g_pt_cur[g_pt_stage] += (now - g_pt_last) * g_pt_qpf_ms;
        g_pt_last = now;
        hl_close(now);
        return;
    }
    if (!g_pt_frames) return;
    g_pt_cur[g_pt_stage] += (now - g_pt_last) * g_pt_qpf_ms;
    g_pt_last = now;
    if (g_pt_nframes < PT_MAX_FRAMES) {
        PtFrame &f = g_pt_frames[g_pt_nframes];
        for (int s = 0; s < PS_COUNT; ++s) f.ms[s] = (float)g_pt_cur[s];
        f.wall = (float)((now - g_pt_frame_t0) * g_pt_qpf_ms);
        f.level = g_pt_level;
        f.kind = (unsigned char)g_pt_kind;
    }
    ++g_pt_nframes;
}

static int pt_cmpf(const void *a, const void *b)
{
    const float x = *(const float *)a, y = *(const float *)b;
    return x < y ? -1 : x > y ? 1 : 0;
}

static void pt_dump(void)
{
    if (g_pt_on <= 0 || !g_pt_frames) return;
    const long long now = pt_qpc();
    pt_close_frame(now);
    g_pt_kind = -1;
    g_pt_stop = 1;
    if (g_pt_sampler) {
        WaitForSingleObject(g_pt_sampler, 2000);
        CloseHandle(g_pt_sampler);
        g_pt_sampler = 0;
    }
    const int nf = g_pt_nframes < PT_MAX_FRAMES ? g_pt_nframes : PT_MAX_FRAMES;
    if (FILE *f = fopen("perf_frames.tsv", "w")) {
        fprintf(f, "frame\tkind\tlevel\twall");
        for (int s = 0; s < PS_COUNT; ++s) fprintf(f, "\t%s", k_ps_name[s]);
        fprintf(f, "\n");
        for (int i = 0; i < nf; ++i) {
            const PtFrame &p = g_pt_frames[i];
            fprintf(f, "%d\t%d\t%d\t%.4f", i, p.kind, p.level, p.wall);
            for (int s = 0; s < PS_COUNT; ++s) fprintf(f, "\t%.4f", p.ms[s]);
            fprintf(f, "\n");
        }
        fclose(f);
    }
    /* stderr summary: every stage's median / p95 over all frames recorded */
    float *col = (float *)VirtualAlloc(0, (nf + 1) * sizeof(float),
                                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (col && nf > 0) {
        fprintf(stderr, "[perf] %d frames; stage  median  p95  mean (ms)\n", nf);
        for (int s = -1; s < PS_COUNT; ++s) {
            double sum = 0;
            for (int i = 0; i < nf; ++i) {
                col[i] = s < 0 ? g_pt_frames[i].wall : g_pt_frames[i].ms[s];
                sum += col[i];
            }
            if (sum <= 0.0) continue;
            qsort(col, nf, sizeof(float), pt_cmpf);
            fprintf(stderr, "[perf]   %-14s %8.3f %8.3f %8.3f\n",
                    s < 0 ? "WALL" : k_ps_name[s], col[nf / 2],
                    col[(int)(nf * 0.95)], sum / nf);
        }
        VirtualFree(col, 0, MEM_RELEASE);
    }
    if (g_pt_samp) {
        if (FILE *f = fopen("perf_samples.bin", "wb")) {
            fwrite(g_pt_samp, sizeof(PtSample), g_pt_nsamp, f);
            fclose(f);
        }
        if (FILE *f = fopen("perf_threads.tsv", "w")) {
            fprintf(f, "index\ttid\tmain\n");
            for (int i = 0; i < g_pt_ntids; ++i)
                fprintf(f, "%d\t%lu\t%d\n", i, (unsigned long)g_pt_tids[i],
                        g_pt_tids[i] == g_pt_main_tid);
            fclose(f);
        }
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE |
                                               TH32CS_SNAPMODULE32, 0);
        if (snap != INVALID_HANDLE_VALUE) {
            if (FILE *f = fopen("perf_modules.tsv", "w")) {
                MODULEENTRY32 me;
                me.dwSize = sizeof me;
                if (Module32First(snap, &me)) {
                    do {
                        fprintf(f, "%08lx\t%08lx\t%s\n",
                                (unsigned long)(size_t)me.modBaseAddr,
                                (unsigned long)me.modBaseSize, me.szModule);
                        me.dwSize = sizeof me;
                    } while (Module32Next(snap, &me));
                }
                fclose(f);
            }
            CloseHandle(snap);
        }
        fprintf(stderr, "[perf] sampler: %ld samples over %ld ticks, %.1f ms "
                "spent holding threads (%.3f ms a tick)\n",
                (long)g_pt_nsamp, g_pt_ticks,
                g_pt_samp_busy_qpc * g_pt_qpf_ms,
                g_pt_ticks ? g_pt_samp_busy_qpc * g_pt_qpf_ms / g_pt_ticks : 0.0);
    }
    pt_milestone("exit");
}

/* armed from main's first statement */
static void pt_arm(void)
{
    if (pt_on() <= 0) return;
    LARGE_INTEGER q;
    QueryPerformanceFrequency(&q);
    g_pt_qpf_ms = 1000.0 / (double)q.QuadPart;
    g_pt_last = pt_qpc();
    if (g_pt_on == 3) return;           /* the hitch log needs nothing else */
    g_pt_frames = (PtFrame *)VirtualAlloc(0, sizeof(PtFrame) * PT_MAX_FRAMES,
                                          MEM_COMMIT | MEM_RESERVE,
                                          PAGE_READWRITE);
    g_pt_main_tid = GetCurrentThreadId();
    {
        const unsigned char *b = (const unsigned char *)GetModuleHandleA(NULL);
        const IMAGE_NT_HEADERS *nt =
            (const IMAGE_NT_HEADERS *)(b + ((const IMAGE_DOS_HEADER *)b)->e_lfanew);
        g_pt_exe_lo = (unsigned)(size_t)b + nt->OptionalHeader.BaseOfCode;
        g_pt_exe_hi = g_pt_exe_lo + nt->OptionalHeader.SizeOfCode;
    }
    atexit(pt_dump);
    if (g_pt_on == 2) {
        g_pt_samp = (PtSample *)VirtualAlloc(0, sizeof(PtSample) * PT_MAX_SAMPLES,
                                             MEM_COMMIT | MEM_RESERVE,
                                             PAGE_READWRITE);
        if (g_pt_samp) {
            g_pt_sampler = CreateThread(0, 64 * 1024, pt_sampler_main, 0, 0,
                                        &g_pt_sampler_tid);
        }
    }
    fprintf(stderr, "[perf] SM64DS_PERF_TRACE=%d armed (%s)\n", g_pt_on,
            g_pt_on == 2 ? "stage timers + 1 ms sampler" : "stage timers");
    pt_milestone("main entry");
}

/* a frame loop's first statement: closes the previous frame's record */
static void pt_frame_begin(int kind, int level)
{
    if (g_pt_on <= 0) return;
    const long long now = pt_qpc();
    pt_close_frame(now);
    if (g_pt_kind != kind) {
        pt_milestone(kind ? "first scene frame" : "first level frame");
    } else if (g_pt_level != level) {
        char b[48];
        sprintf(b, "level change to %d", level);
        pt_milestone(b);
    }
    g_pt_kind = kind;
    g_pt_level = level;
    g_pt_frame_t0 = now;
    g_pt_sp = 0;
    for (int s = 0; s < PS_COUNT; ++s) g_pt_cur[s] = 0.0;
}
