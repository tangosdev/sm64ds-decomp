// ROLLBACK FEASIBILITY PROBE (port/rollback-spike). Measurement only.
//
// Inert unless one of these is set; the default build's frame loop takes one
// cached int compare per call site and nothing else.
//
//   SM64DS_ROLLBACK_PROBE=1   per-frame phase and span timing (QPC), summary at
//                             the end of a selftest run
//   SM64DS_ROLLBACK_PROBE=2   as 1, plus a whole-state save AND restore every
//                             frame from frame 30 on, timed; plus a raw memcpy
//                             of the arena alone and of the full capture set
//                             into preallocated buffers, timed. NOTE a restore
//                             resets the audio and drops the texture decode
//                             cache every frame, so the render phases in a
//                             mode-2 run are not the numbers to quote; take
//                             those from a mode-1 run.
//   SM64DS_ROLLBACK_DET=<F>   determinism check: save at the end of frame F,
//                             run N more frames, hash the world, restore the
//                             frame-F state, rewind the frame counter, run the
//                             same N frames again, hash again, compare. N is
//                             SM64DS_ROLLBACK_DET_N (default 8).
//   SM64DS_ROLLBACK_DET_SKIP=1  during the re-run skip the rasteriser and the
//                             present, which is what a real rollback would do.
//
// Everything here reads the same three regions lk6_savestate.cpp captures:
// the hosted arena, the .dsstate section, and the hardware content stores.

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

extern "C" {
void *port_arena_base(void);
void *port_arena_end(void);
unsigned port_hw_regions_size(void);
void port_hw_regions_copy_out(void *dst);
extern char dsstate_lo, dsstate_hi;
int lk6_savestate_save(void);
int lk6_savestate_load(void);
}

namespace {

enum {
    RB_ACTOR_TICK = 0, RB_ANIMS, RB_PARTICLE, RB_CYL, RB_SCENEPASS,
    RB_PH_INPUT, RB_PH_CAMERA, RB_PH_SUBMIT, RB_PH_RASTER, RB_PH_BLIT,
    RB_PH_FRAME,
    RB_SAVE, RB_LOAD, RB_ARENA_COPY, RB_FULL_COPY,
    RB_COUNT
};
const char *const kName[RB_COUNT] = {
    "actor_tick (phases 4/2/3)", "stage_advance_anims", "particle sim+submit",
    "cylinder_clsn_process", "actor_scene_pass",
    "PH_INPUT (keys+CheckInput+actor tick)", "PH_CAMERA (camera+comms)",
    "PH_SUBMIT (geometry, incl. the four spans above)", "PH_RASTER",
    "PH_BLIT", "PH_FRAME (whole body)",
    "lk6_savestate_save (malloc+memcpy+free+print)",
    "lk6_savestate_load (memcpy+hw copy_in+audio reset+print)",
    "memcpy arena only (prealloc)",
    "memcpy arena+dsstate+hw (prealloc)"
};

enum { kMaxSamples = 4096 };
struct Stat {
    double sum, max;
    int n;
    double s[kMaxSamples];
};
Stat g_stat[RB_COUNT];

int g_mode = -1;
int g_det_frame = -2;      // -1 = off
int g_det_n = 8;
int g_det_skip = 0;
int g_det_phase = 0;       // 0 waiting for F, 1 straight run, 2 re-run, 3 done
int g_resim = 0;

char *g_arena_buf, *g_ds_buf, *g_hw_buf;
char *g_cmp_arena, *g_cmp_ds, *g_cmp_hw;   // straight-run copies for the diff
uint64_t g_h_straight[3];

int cmp_double(const void *a, const void *b)
{
    const double x = *(const double *)a, y = *(const double *)b;
    return x < y ? -1 : (x > y ? 1 : 0);
}

void read_env()
{
    if (g_mode >= 0) return;
    const char *e = getenv("SM64DS_ROLLBACK_PROBE");
    g_mode = e ? atoi(e) : 0;
    const char *d = getenv("SM64DS_ROLLBACK_DET");
    g_det_frame = d ? atoi(d) : -1;
    const char *n = getenv("SM64DS_ROLLBACK_DET_N");
    if (n) g_det_n = atoi(n);
    if (g_det_n < 1) g_det_n = 1;
    g_det_skip = getenv("SM64DS_ROLLBACK_DET_SKIP") != 0;
}

double now_ms()
{
    static LARGE_INTEGER qpf;
    LARGE_INTEGER t;
    if (!qpf.QuadPart) QueryPerformanceFrequency(&qpf);
    QueryPerformanceCounter(&t);
    return t.QuadPart * 1000.0 / (double)qpf.QuadPart;
}

void note(int what, double ms)
{
    Stat &s = g_stat[what];
    s.sum += ms;
    if (ms > s.max) s.max = ms;
    if (s.n < kMaxSamples) s.s[s.n] = ms;
    ++s.n;
}

uint64_t hash_bytes(const void *p, size_t n)
{
    // word-wise mix; this is a comparator, not a cryptographic hash
    const unsigned char *b = (const unsigned char *)p;
    uint64_t h = 0x9E3779B97F4A7C15ull ^ (uint64_t)n;
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        uint64_t w;
        memcpy(&w, b + i, 8);
        h ^= w;
        h *= 0xBF58476D1CE4E5B9ull;
        h ^= h >> 29;
    }
    for (; i < n; ++i) {
        h ^= b[i];
        h *= 0x94D049BB133111EBull;
        h ^= h >> 31;
    }
    return h;
}

size_t arena_size() {
    return (size_t)((char *)port_arena_end() - (char *)port_arena_base());
}
size_t ds_size() {
    char *lo = &dsstate_lo, *hi = &dsstate_hi;
    return hi > lo ? (size_t)(hi - lo) : 0;
}

void ensure_bufs()
{
    if (g_arena_buf) return;
    g_arena_buf = (char *)malloc(arena_size());
    g_ds_buf = (char *)malloc(ds_size() ? ds_size() : 1);
    const unsigned h = port_hw_regions_size();
    g_hw_buf = (char *)malloc(h ? h : 1);
}

void hash_world(uint64_t out[3])
{
    ensure_bufs();
    out[0] = hash_bytes(port_arena_base(), arena_size());
    out[1] = hash_bytes(&dsstate_lo, ds_size());
    const unsigned h = port_hw_regions_size();
    if (h) { port_hw_regions_copy_out(g_hw_buf); out[2] = hash_bytes(g_hw_buf, h); }
    else out[2] = 0;
}

void keep_world_copy()
{
    ensure_bufs();
    const size_t a = arena_size(), d = ds_size();
    const unsigned h = port_hw_regions_size();
    if (!g_cmp_arena) {
        g_cmp_arena = (char *)malloc(a);
        g_cmp_ds = (char *)malloc(d ? d : 1);
        g_cmp_hw = (char *)malloc(h ? h : 1);
    }
    memcpy(g_cmp_arena, port_arena_base(), a);
    memcpy(g_cmp_ds, &dsstate_lo, d);
    if (h) port_hw_regions_copy_out(g_cmp_hw);
}

void diff_region(const char *name, const char *want, const char *got,
                 size_t n, const char *base_for_addr)
{
    size_t differing = 0, first = (size_t)-1, ranges = 0;
    size_t i = 0;
    while (i < n) {
        if (want[i] == got[i]) { ++i; continue; }
        size_t j = i;
        while (j < n && want[j] != got[j]) ++j;
        differing += j - i;
        if (first == (size_t)-1) first = i;
        if (ranges < 12)
            fprintf(stderr, "[rb-det]     %s diff at +0x%zx (addr %p) len %zu: "
                    "straight %02x%02x%02x%02x rerun %02x%02x%02x%02x\n",
                    name, i, (const void *)(base_for_addr + i), j - i,
                    (unsigned char)want[i], (unsigned char)want[i + 1 < n ? i + 1 : i],
                    (unsigned char)want[i + 2 < n ? i + 2 : i], (unsigned char)want[i + 3 < n ? i + 3 : i],
                    (unsigned char)got[i], (unsigned char)got[i + 1 < n ? i + 1 : i],
                    (unsigned char)got[i + 2 < n ? i + 2 : i], (unsigned char)got[i + 3 < n ? i + 3 : i]);
        ++ranges;
        i = j;
    }
    fprintf(stderr, "[rb-det]   %s: %zu differing bytes in %zu ranges "
            "(first at +0x%zx)\n", name, differing, ranges,
            first == (size_t)-1 ? 0 : first);
}

void snapshot_timing()
{
    ensure_bufs();
    const size_t a = arena_size(), d = ds_size();
    const unsigned h = port_hw_regions_size();
    double t;

    t = now_ms();
    memcpy(g_arena_buf, port_arena_base(), a);
    note(RB_ARENA_COPY, now_ms() - t);

    t = now_ms();
    memcpy(g_arena_buf, port_arena_base(), a);
    memcpy(g_ds_buf, &dsstate_lo, d);
    if (h) port_hw_regions_copy_out(g_hw_buf);
    note(RB_FULL_COPY, now_ms() - t);

    t = now_ms();
    lk6_savestate_save();
    note(RB_SAVE, now_ms() - t);

    t = now_ms();
    lk6_savestate_load();
    note(RB_LOAD, now_ms() - t);
}

void report(int frames)
{
    fprintf(stderr, "[rb-probe] ===== rollback probe summary, %d frames, "
            "arena %zu bytes, dsstate %zu bytes, hw %u bytes =====\n",
            frames, arena_size(), ds_size(), port_hw_regions_size());
    for (int k = 0; k < RB_COUNT; ++k) {
        Stat &s = g_stat[k];
        if (!s.n) continue;
        const int m = s.n < kMaxSamples ? s.n : kMaxSamples;
        qsort(s.s, m, sizeof s.s[0], cmp_double);
        fprintf(stderr, "[rb-probe] %-52s n=%4d mean %7.3f ms  p50 %7.3f  "
                "p95 %7.3f  max %7.3f\n", kName[k], s.n, s.sum / s.n,
                s.s[m / 2], s.s[(m * 95) / 100], s.max);
    }
}

} // namespace

extern "C" {

int rb_probe_mode(void) { read_env(); return g_mode; }
double rb_now_ms(void) { return now_ms(); }
void rb_note(int what, double ms) { if (what >= 0 && what < RB_COUNT) note(what, ms); }
int rb_resim_skip_render(void) { return g_resim && g_det_skip; }
int rb_det_active(void) { read_env(); return g_det_frame >= 0; }

// Called at the frame boundary, after the frame's tick, render, present and
// audio, before ++frame. May rewind *frame (the determinism re-run).
void rb_probe_frame_end(int *frame, int selftest)
{
    read_env();
    const int f = *frame;

    if (g_mode >= 2 && f >= 30) snapshot_timing();

    if (g_det_frame >= 0) {
        if (g_det_phase == 0 && f == g_det_frame) {
            uint64_t h[3];
            hash_world(h);
            fprintf(stderr, "[rb-det] f%d save: arena %016llx dsstate %016llx "
                    "hw %016llx\n", f, (unsigned long long)h[0],
                    (unsigned long long)h[1], (unsigned long long)h[2]);
            keep_world_copy();   // so the post-restore state can be diffed
            if (lk6_savestate_save()) g_det_phase = 1;
            else fprintf(stderr, "[rb-det] save FAILED, check off\n");
        } else if (g_det_phase == 1 && f == g_det_frame + g_det_n) {
            hash_world(g_h_straight);
            // the straight-run world, kept for the diff; the save-time copy is
            // first diffed against what the restore puts back (below)
            char *save_arena = g_cmp_arena, *save_ds = g_cmp_ds, *save_hw = g_cmp_hw;
            g_cmp_arena = g_cmp_ds = g_cmp_hw = 0;
            keep_world_copy();
            fprintf(stderr, "[rb-det] f%d straight run: arena %016llx dsstate "
                    "%016llx hw %016llx; restoring f%d and re-running %d "
                    "frames%s\n", f, (unsigned long long)g_h_straight[0],
                    (unsigned long long)g_h_straight[1],
                    (unsigned long long)g_h_straight[2], g_det_frame, g_det_n,
                    g_det_skip ? " with raster+present skipped" : "");
            const double t = now_ms();
            const int ok = lk6_savestate_load();
            fprintf(stderr, "[rb-det] restore %s in %.3f ms\n",
                    ok ? "ok" : "FAILED", now_ms() - t);
            uint64_t h[3];
            hash_world(h);
            fprintf(stderr, "[rb-det] after restore: arena %016llx dsstate "
                    "%016llx hw %016llx\n", (unsigned long long)h[0],
                    (unsigned long long)h[1], (unsigned long long)h[2]);
            fprintf(stderr, "[rb-det] restore vs the save-time bytes:\n");
            diff_region("arena", save_arena, (const char *)port_arena_base(),
                        arena_size(), (const char *)port_arena_base());
            diff_region("dsstate", save_ds, &dsstate_lo, ds_size(), &dsstate_lo);
            if (port_hw_regions_size()) {
                port_hw_regions_copy_out(g_hw_buf);
                diff_region("hw", save_hw, g_hw_buf, port_hw_regions_size(), 0);
            }
            free(save_arena); free(save_ds); free(save_hw);
            g_det_phase = 2;
            g_resim = 1;
            *frame = g_det_frame;
        } else if (g_det_phase == 2 && f == g_det_frame + g_det_n) {
            uint64_t h[3];
            hash_world(h);
            g_resim = 0;
            g_det_phase = 3;
            const int same = h[0] == g_h_straight[0] && h[1] == g_h_straight[1] &&
                             h[2] == g_h_straight[2];
            fprintf(stderr, "[rb-det] f%d re-run: arena %016llx dsstate %016llx "
                    "hw %016llx -> %s\n", f, (unsigned long long)h[0],
                    (unsigned long long)h[1], (unsigned long long)h[2],
                    same ? "BYTE-IDENTICAL to the straight run"
                         : "DIVERGED from the straight run");
            if (!same && getenv("SM64DS_ROLLBACK_DET_DUMP")) {
                // both .dsstate images, for an offline symbol-by-symbol diff
                // against the link map (port/tools has no reader; the spike's
                // status doc carries the one-off script)
                FILE *fa = fopen("rb_ds_straight.bin", "wb");
                FILE *fb = fopen("rb_ds_rerun.bin", "wb");
                if (fa && fb) {
                    fwrite(g_cmp_ds, 1, ds_size(), fa);
                    fwrite(&dsstate_lo, 1, ds_size(), fb);
                    fprintf(stderr, "[rb-det] dumped both .dsstate images, base %p\n",
                            (void *)&dsstate_lo);
                }
                if (fa) fclose(fa);
                if (fb) fclose(fb);
            }
            if (!same) {
                if (h[0] != g_h_straight[0])
                    diff_region("arena", g_cmp_arena, (const char *)port_arena_base(),
                                arena_size(), (const char *)port_arena_base());
                if (h[1] != g_h_straight[1])
                    diff_region("dsstate", g_cmp_ds, &dsstate_lo, ds_size(),
                                &dsstate_lo);
                if (h[2] != g_h_straight[2]) {
                    port_hw_regions_copy_out(g_hw_buf);
                    diff_region("hw", g_cmp_hw, g_hw_buf, port_hw_regions_size(), 0);
                }
            }
            printf("rb-det: %s\n", same ? "IDENTICAL" : "DIVERGED");
        }
    }

    if (g_mode >= 1 && selftest && *frame + 1 >= selftest) report(*frame + 1);
}

} // extern "C"
