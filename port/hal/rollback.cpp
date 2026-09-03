// ROLLBACK NETCODE, the frame-boundary half (port/rollback).
//
// The transport (hal/comms_loopback.cpp, the ROLLBACK LEDGER banner) serves
// every round the frame it opens, guessing any block that has not arrived and
// keeping a record of what it guessed. This file is the other half: once per
// frame, at THE FRAME BOUNDARY in tests/walk_window.cpp (after the tick, the
// render, the present and the sound drain, before ++frame), it
//
//   1. takes a SNAPSHOT of the world into a ring of preallocated slots, tagged
//      with the transport's round (the world "before round R is consumed");
//   2. asks the transport whether any consumed round has since been
//      contradicted by the wire (comms_rb_scan);
//   3. if so, RESTORES the snapshot for that round, rewinds the transport's
//      clock and the frame counter, mutes the output stage, and lets the frame
//      loop run again up to the present with the host inputs, the camera rig,
//      the rasteriser and the present standing down (rb_replaying /
//      rb_skip_render). Each replayed frame re-snapshots its round on the way
//      through, so a second contradiction inside the same window is a second
//      short rewind, not a lost one.
//
// WHAT A SNAPSHOT IS. The same three regions hal/lk6_savestate.cpp captures:
// the hosted DS arena (8 MB), the .dsstate section (every hosted out-of-arena
// DS global, ~1 MB) and the hardware content stores (palette, video and sprite
// memory, ~9.4 MB). The first two are straight copies into slots allocated
// once (12 x 9 MB). The third is NOT copied per frame: the stores are reserved
// with MEM_WRITE_WATCH (ntr/io.cpp), so the boundary asks the kernel which of
// their pages the frame wrote, and each slot keeps only the PREVIOUS contents
// of those pages -- an undo log against a shadow image. In every run measured
// the stores do not move inside a level (the spike hashed them identical every
// frame), so the per-frame cost is a page-table query and no copy at all; a
// restore applies the logs newer than the target in reverse and touches the
// texture decode cache only when video memory actually came back. A frame
// that rewrites more than a slot's log can hold (an area change) is the one
// event the ring cannot straddle: the ring is flushed, the transport stops
// guessing until every open round is confirmed, and play continues.
//
// WHAT IS NOT IN A SNAPSHOT, AND WHY THAT IS RIGHT.
//   * the transport's ring and ledger: they ARE the record of what was sent
//     and what was guessed, and a rewind reads them; rolling them back would
//     erase the evidence the rewind is acting on.
//   * the frame loop's pointers into the world (player, camera, level
//     collider): an in-level restore puts the same objects back at the same
//     addresses (spike, section 3; walk_window's ss_reseat re-derives and
//     finds them unchanged).
//   * the analog/freecam camera rig (fc_yaw, an_pivot in walk_window.cpp):
//     host-only. Its one game-visible output is the heading it writes into the
//     local record, and that byte is on the wire in this console's own block
//     for the round, which the replay serves from the ring. During a replay
//     the rig stands down and keeps its live values, so after the replay the
//     player is looking where they were looking.
//   * the sound queue's host cursors: re-seeded on restore (sd_consumer_reset,
//     the same call lk6 makes); the sequencer and mixer are NOT reset, because
//     a rewind is a few frames and the music must not hiccup. The output
//     stage is muted while replaying (sd_host_mute).
//
// Everything here is inert unless NetMode is rollback (comms_rb_mode); in
// lockstep the boundary hook is one cached compare.

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "comms_loopback.h"

// hal/sdat: C++-linkage free functions (see the note in lk6_savestate.cpp).
void sd_consumer_reset(void);
void sd_sdat_reseat(void);

namespace ntr { void gx_invalidate_textures(); }

extern "C" {
void *port_arena_base(void);
void *port_arena_end(void);
void *port_arena_cursor(void);
void  port_arena_set_cursor(void *p);
extern char dsstate_lo, dsstate_hi;
unsigned port_hw_regions_size(void);
int  port_hw_region_at(int i, void **base, unsigned *size);
void port_hw_regions_copy_out(void *dst);
void port_hw_regions_copy_in(const void *src);
int  port_ss_rollguard_begin(void);
void port_ss_rollguard_end(int began);
void sd_host_mute(int on);
void port_comms_counters_get(unsigned long long *exchanges, unsigned long long *rounds);
void port_comms_counters_set(unsigned long long exchanges, unsigned long long rounds);
int  port_dh_frame_get(void);
void port_dh_frame_set(int f);
extern unsigned char data_020a1154[];     // the ROM's per-slot comms records
extern int data_020a4b98[];               // the actor walker's list-5 view
// THE HOSTED ARM7 SOUND COMMAND QUEUE (hal/player_bridges.cpp, hal/sdat).
// The one part of .dsstate a restore does not put back byte-for-byte: the
// restore re-seeds the queue (sd_consumer_reset, the same call lk6 makes),
// the replay re-triggers sounds into the reset queue, and the queue's node
// pool, its batch ring and its cursors drift from the straight run's. Nothing
// in the game reads the queue back, and the replay is muted. The DET rung
// names these bytes rather than counting them as a divergence.
extern int data_020a6484[], data_020a6488[], data_020a648c[], data_020a6490[],
           data_020a6494[], data_020a6498[], data_020a649c[], data_020a64a0[],
           data_020a64a4[];
extern void *data_020a64a8[];             // the 16-slot batch ring
extern int data_020a6760[];               // the 256 x 0x18 node pool
extern unsigned char data_020a50ec[];     // sdat sound bss, 0x440
}

namespace {

enum { kSlots = 12 };
enum { kPage = 4096 };
enum { kUndoPages = 640 };           // 2.5 MB of previous page contents per slot
enum { kHwRegions = 3 };

struct HwRegion {
    char    *base;
    unsigned size;
    unsigned pages;
    char    *shadow;                 // contents as of the last snapshot
};

struct UndoEntry { unsigned short region; unsigned short pad; unsigned page; };

struct Slot {
    bool      valid;
    unsigned  tag;                   // the round this world precedes
    int       frame;
    // host bookkeeping outside the three regions, put back with them
    unsigned long long exchanges, rounds;   // the seam's counters
    int       dh_frame;                     // the divergence detector's clock
    char     *arena;
    char     *ds;
    void     *cursor;
    unsigned  undo_n;
    UndoEntry *undo;
    char     *undo_data;             // kUndoPages x kPage
    char     *hw_full;               // fallback only: a whole copy
};

Slot     g_ring[kSlots];
bool     g_ring_ready = false;
bool     g_ring_live  = false;
HwRegion g_hw[kHwRegions];
int      g_hw_n = 0;
bool     g_ww = false;               // GetWriteWatch usable on the stores
void   **g_ww_buf = 0;
unsigned g_ww_pages = 0;              // entries in g_ww_buf
size_t   g_arena_size = 0, g_ds_size = 0;

bool     g_replaying = false;        // host side: a rewind is being re-run
unsigned g_replay_from = 0, g_replay_to = 0;
double   g_replay_t0 = 0;

// knobs
int      g_env_read = 0;
int      g_pause_frame = -1, g_pause_ms = 0, g_pause_done = 0;
int      g_det_frame = -1, g_det_n = 8, g_det_phase = 0;
unsigned g_det_tag = 0;
uint64_t g_det_straight[3];
char    *g_det_arena = 0, *g_det_ds = 0, *g_det_hw = 0;
int      g_force_every = 0, g_force_depth = 0;   // SM64DS_ROLLBACK_FORCE=<every>:<depth>
int      g_local_probe = 0;
int      g_verbose = 0;
int      g_no_render_skip = 0;       // SM64DS_ROLLBACK_FULL_RESIM=1: conservative re-sim
int      g_keep_actor_render = 0;    // SM64DS_ROLLBACK_ACTOR_RENDER=1: replay runs the actors' Render bodies

// counters and timing
struct Stat { double sum, max; int n; double s[4096]; };
Stat g_snap, g_restore, g_rbevent, g_rbframe, g_rbframes;
Stat g_rp[8];                        // replayed frames, per loop phase
static const char *const kPhase[8] = {
    "replay PH_INPUT (tick)", "replay PH_CAMERA (exchange)",
    "replay PH_SUBMIT", "replay PH_RASTER", "replay PH_BLIT",
    "replay PH_FRAME (loop body)", "replay func_0203df40 (ROM comms)",
    "replay sync_tick" };
unsigned long long g_snapshots = 0, g_rollbacks = 0, g_frames_replayed = 0,
                   g_hw_pages_logged = 0, g_hw_pages_restored = 0,
                   g_flushes = 0, g_unrecoverable = 0, g_stalls_paused = 0;
unsigned long long g_probe_frames = 0, g_probe_same = 0, g_probe_diff = 0;
unsigned g_max_depth = 0;
int g_reported = 0;

double now_ms()
{
    static LARGE_INTEGER f;
    LARGE_INTEGER t;
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return t.QuadPart * 1000.0 / (double)f.QuadPart;
}
void note(Stat &s, double ms)
{
    s.sum += ms;
    if (ms > s.max) s.max = ms;
    if (s.n < 4096) s.s[s.n] = ms;
    ++s.n;
}
int cmp_d(const void *a, const void *b)
{
    const double x = *(const double *)a, y = *(const double *)b;
    return x < y ? -1 : (x > y ? 1 : 0);
}
void print_stat(const char *name, Stat &s)
{
    if (!s.n) return;
    const int m = s.n < 4096 ? s.n : 4096;
    qsort(s.s, m, sizeof s.s[0], cmp_d);
    fprintf(stderr, "[rollback] %-34s n=%5d mean %7.3f ms  p50 %7.3f  p95 %7.3f"
            "  max %7.3f\n", name, s.n, s.sum / s.n, s.s[m / 2],
            s.s[(m * 95) / 100], s.max);
}

void read_env()
{
    if (g_env_read) return;
    g_env_read = 1;
    if (const char *p = getenv("SM64DS_ROLLBACK_PAUSE")) {
        // <frame>:<ms>  -- at that boundary, sleep with the pump stopped, the
        // way a hung or dragged window does. The stall test's one knob.
        g_pause_frame = atoi(p);
        if (const char *c = strchr(p, ':')) g_pause_ms = atoi(c + 1);
    }
    if (const char *d = getenv("SM64DS_ROLLBACK_DET")) g_det_frame = atoi(d);
    if (const char *n = getenv("SM64DS_ROLLBACK_DET_N")) g_det_n = atoi(n);
    if (g_det_n < 1) g_det_n = 1;
    if (const char *f = getenv("SM64DS_ROLLBACK_FORCE")) {
        // <every>:<depth> -- every so many frames, rewind `depth` rounds
        // through the real path and replay them serving exactly what was
        // served. A cost benchmark, not a correctness knob: it exercises the
        // restore and the re-sim under whatever load the level carries.
        g_force_every = atoi(f);
        if (const char *c = strchr(f, ':')) g_force_depth = atoi(c + 1);
        if (g_force_depth < 1) g_force_depth = 1;
    }
    g_local_probe = getenv("SM64DS_ROLLBACK_LOCALPROBE") != 0;
    g_verbose = getenv("SM64DS_ROLLBACK_VERBOSE") != 0;
    g_no_render_skip = getenv("SM64DS_ROLLBACK_FULL_RESIM") != 0;
    g_keep_actor_render = getenv("SM64DS_ROLLBACK_ACTOR_RENDER") != 0;
}

uint64_t hash_bytes(const void *p, size_t n)
{
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
    for (; i < n; ++i) { h ^= b[i]; h *= 0x94D049BB133111EBull; h ^= h >> 31; }
    return h;
}

size_t ds_size()
{
    char *lo = &dsstate_lo, *hi = &dsstate_hi;
    return hi > lo ? (size_t)(hi - lo) : 0;
}

char *big_alloc(size_t n)
{
    return (char *)VirtualAlloc(0, n ? n : 1, MEM_RESERVE | MEM_COMMIT,
                                PAGE_READWRITE);
}

void hw_shadow_refresh_all()
{
    for (int r = 0; r < g_hw_n; ++r) memcpy(g_hw[r].shadow, g_hw[r].base, g_hw[r].size);
}
void hw_watch_reset_all()
{
    if (!g_ww) return;
    for (int r = 0; r < g_hw_n; ++r) ResetWriteWatch(g_hw[r].base, g_hw[r].size);
}

bool ring_init()
{
    if (g_ring_ready) return true;
    g_arena_size = (size_t)((char *)port_arena_end() - (char *)port_arena_base());
    g_ds_size = ds_size();
    if (!g_arena_size || !g_ds_size) {
        fprintf(stderr, "[rollback] no arena or no .dsstate section; the ring "
                "cannot be built, rollback is OFF for this process\n");
        return false;
    }
    g_hw_n = 0;
    for (int i = 0; i < kHwRegions; ++i) {
        void *b; unsigned sz;
        if (!port_hw_region_at(i, &b, &sz)) break;
        g_hw[g_hw_n].base = (char *)b;
        g_hw[g_hw_n].size = sz;
        g_hw[g_hw_n].pages = (sz + kPage - 1) / kPage;
        g_hw[g_hw_n].shadow = big_alloc(sz);
        ++g_hw_n;
    }
    // Is write-watch live on the stores? Ask about the first; a reservation
    // made without the flag answers ERROR_INVALID_PARAMETER.
    // The query buffer holds every page of the largest store (video memory
    // is 0x900000 = 2304 pages), so a frame that rewrote all of it -- boot,
    // an area change -- is REPORTED as an overflow rather than failing the
    // query, and the reset flag is never applied to a partial answer.
    g_ww = false;
    g_ww_pages = 0;
    for (int r = 0; r < g_hw_n; ++r)
        if (g_hw[r].pages > g_ww_pages) g_ww_pages = g_hw[r].pages;
    if (g_hw_n) {
        g_ww_buf = (void **)malloc(sizeof(void *) * (g_ww_pages + 1));
        ULONG_PTR cnt = g_ww_pages + 1;
        DWORD gran = 0;
        UINT rc = GetWriteWatch(WRITE_WATCH_FLAG_RESET, g_hw[0].base,
                                g_hw[0].size, g_ww_buf, &cnt, &gran);
        g_ww = rc == 0 && gran == kPage;
    }
    size_t total = 0;
    for (int i = 0; i < kSlots; ++i) {
        Slot &s = g_ring[i];
        memset(&s, 0, sizeof s);
        s.arena = big_alloc(g_arena_size);
        s.ds    = big_alloc(g_ds_size);
        total += g_arena_size + g_ds_size;
        if (g_ww) {
            s.undo = (UndoEntry *)malloc(sizeof(UndoEntry) * kUndoPages);
            s.undo_data = big_alloc((size_t)kUndoPages * kPage);
            total += (size_t)kUndoPages * kPage;
        } else if (g_hw_n) {
            s.hw_full = big_alloc(port_hw_regions_size());
            total += port_hw_regions_size();
        }
        if (!s.arena || !s.ds) {
            fprintf(stderr, "[rollback] out of memory building the ring\n");
            return false;
        }
    }
    for (int r = 0; r < g_hw_n; ++r) total += g_hw[r].size;
    hw_shadow_refresh_all();
    hw_watch_reset_all();
    g_ring_ready = true;
    fprintf(stderr, "[rollback] ring: %d slots x (arena %zu + dsstate %zu) + "
            "hw stores %u bytes in %d regions (%s), %.1f MB preallocated\n",
            kSlots, g_arena_size, g_ds_size, port_hw_regions_size(), g_hw_n,
            g_ww ? "write-watched, undo logs" : "NO write-watch, full copies",
            total / 1048576.0);
    return true;
}

void ring_flush(const char *why)
{
    for (int i = 0; i < kSlots; ++i) { g_ring[i].valid = false; g_ring[i].undo_n = 0; }
    hw_shadow_refresh_all();
    hw_watch_reset_all();
    ++g_flushes;
    if (why) fprintf(stderr, "[rollback] ring flushed: %s\n", why);
}

// Collect the frame's dirty store pages into the slot's undo log. Returns 0,
// or 1 when the frame changed more than the log holds (the shadow is rebuilt
// and the caller flushes the ring).
int hw_collect(Slot &s)
{
    if (!g_hw_n) return 0;
    if (!g_ww) { port_hw_regions_copy_out(s.hw_full); return 0; }
    for (int r = 0; r < g_hw_n; ++r) {
        HwRegion &R = g_hw[r];
        ULONG_PTR cnt = g_ww_pages + 1;
        DWORD gran = 0;
        const UINT rc = GetWriteWatch(WRITE_WATCH_FLAG_RESET, R.base, R.size,
                                      g_ww_buf, &cnt, &gran);
        if (rc != 0 || s.undo_n + cnt > (ULONG_PTR)kUndoPages) goto overflow;
        for (ULONG_PTR i = 0; i < cnt; ++i) {
            if (s.undo_n >= (unsigned)kUndoPages) goto overflow;
            const unsigned page = (unsigned)(((char *)g_ww_buf[i] - R.base) / kPage);
            const unsigned len = (page + 1) * kPage <= R.size ? kPage : R.size - page * kPage;
            UndoEntry &e = s.undo[s.undo_n];
            e.region = (unsigned short)r;
            e.pad = 0;
            e.page = page;
            memcpy(s.undo_data + (size_t)s.undo_n * kPage, R.shadow + (size_t)page * kPage, len);
            memcpy(R.shadow + (size_t)page * kPage, R.base + (size_t)page * kPage, len);
            ++s.undo_n;
            ++g_hw_pages_logged;
        }
    }
    return 0;
overflow:
    hw_shadow_refresh_all();
    hw_watch_reset_all();
    s.undo_n = 0;
    return 1;
}

// Undo the logs of every slot NEWER than `tag`, newest first, entries in
// reverse, so the stores read as they did at snapshot `tag`.
void hw_restore_to(unsigned tag, const Slot &target)
{
    if (!g_hw_n) return;
    if (!g_ww) {
        port_hw_regions_copy_in(target.hw_full);     // drops the decode cache
        ++g_hw_pages_restored;
        return;
    }
    bool vram_touched = false;
    // slots newer than tag, in descending tag order: tag+1.. is at most 11 away
    for (unsigned t = tag + kSlots - 1; (int)(t - tag) > 0; --t) {
        Slot &s = g_ring[t % kSlots];
        if (!s.valid || s.tag != t) continue;
        for (unsigned i = s.undo_n; i-- > 0;) {
            const UndoEntry &e = s.undo[i];
            HwRegion &R = g_hw[e.region];
            const unsigned len = (e.page + 1) * kPage <= R.size ? kPage : R.size - e.page * kPage;
            memcpy(R.base + (size_t)e.page * kPage, s.undo_data + (size_t)i * kPage, len);
            memcpy(R.shadow + (size_t)e.page * kPage, s.undo_data + (size_t)i * kPage, len);
            if (e.region == 1) vram_touched = true;
            ++g_hw_pages_restored;
        }
    }
    hw_watch_reset_all();
    if (vram_touched) ntr::gx_invalidate_textures();
}

Slot *ring_find(unsigned tag)
{
    Slot &s = g_ring[tag % kSlots];
    return (s.valid && s.tag == tag) ? &s : 0;
}

// Snapshot the world as "before round `tag`". Same tag twice in a row (a frame
// that consumed no round) appends to the slot's undo log rather than replacing
// it, so the chain back to the previous tag stays whole.
int snapshot(unsigned tag, int frame)
{
    const double t0 = now_ms();
    Slot &s = g_ring[tag % kSlots];
    if (!(s.valid && s.tag == tag)) s.undo_n = 0;
    s.valid = true;
    s.tag = tag;
    s.frame = frame;
    port_comms_counters_get(&s.exchanges, &s.rounds);
    s.dh_frame = port_dh_frame_get();
    memcpy(s.arena, port_arena_base(), g_arena_size);
    memcpy(s.ds, &dsstate_lo, g_ds_size);
    s.cursor = port_arena_cursor();
    const int ovf = hw_collect(s);
    ++g_snapshots;
    note(g_snap, now_ms() - t0);
    return ovf;
}

bool restore(unsigned tag)
{
    Slot *s = ring_find(tag);
    if (!s) return false;
    const double t0 = now_ms();
    const int norg = port_ss_rollguard_begin();
    memcpy(port_arena_base(), s->arena, g_arena_size);
    memcpy(&dsstate_lo, s->ds, g_ds_size);
    port_ss_rollguard_end(norg);
    port_arena_set_cursor(s->cursor);
    port_comms_counters_set(s->exchanges, s->rounds);
    port_dh_frame_set(s->dh_frame);
    hw_restore_to(tag, *s);
    sd_consumer_reset();
    sd_sdat_reseat();
    for (int i = 0; i < kSlots; ++i)
        if (g_ring[i].valid && (int)(g_ring[i].tag - tag) > 0) g_ring[i].valid = false;
    note(g_restore, now_ms() - t0);
    return true;
}

void hash_world(uint64_t out[3])
{
    out[0] = hash_bytes(port_arena_base(), g_arena_size);
    out[1] = hash_bytes(&dsstate_lo, g_ds_size);
    out[2] = 0;
    for (int r = 0; r < g_hw_n; ++r) out[2] ^= hash_bytes(g_hw[r].base, g_hw[r].size) * (r + 1);
}

void keep_world(char **a, char **d, char **h)
{
    if (!*a) { *a = big_alloc(g_arena_size); *d = big_alloc(g_ds_size); *h = big_alloc(port_hw_regions_size()); }
    memcpy(*a, port_arena_base(), g_arena_size);
    memcpy(*d, &dsstate_lo, g_ds_size);
    if (g_hw_n) port_hw_regions_copy_out(*h);
}

// The nearest actor at or below an arena address, for naming a DET diff:
// walks the render list the way port_actor_render_replay does and reports
// the actor id and the offset into it. Diagnostic only.
void name_arena_addr(size_t off)
{
    struct Node { int pad; Node *next; unsigned char *obj; };
    const unsigned char *addr = (const unsigned char *)port_arena_base() + off;
    const unsigned char *best = 0;
    unsigned best_id = 0;
    for (Node *node = (Node *)(size_t)data_020a4b98[0]; node; node = node->next) {
        if (node->obj <= addr && (!best || node->obj > best)) {
            best = node->obj;
            best_id = *(const unsigned *)(node->obj + 0x0c) & 0xFFFFu;
        }
    }
    if (best)
        fprintf(stderr, "[rb-det]       (arena +0x%zx is actor id %u at %p, +0x%zx)\n",
                off, best_id, (const void *)best, (size_t)(addr - best));
}

bool in_sound_queue(const char *p)
{
    struct Span { const void *base; size_t n; };
    static const Span spans[] = {
        { data_020a6484, 16 }, { data_020a6488, 16 }, { data_020a648c, 16 },
        { data_020a6490, 16 }, { data_020a6494, 16 }, { data_020a6498, 16 },
        { data_020a649c, 16 }, { data_020a64a0, 16 }, { data_020a64a4, 16 },
        { data_020a64a8, 16 * sizeof(void *) }, { data_020a6760, 256 * 0x18 },
        { data_020a50ec, 0x440 },
    };
    for (size_t i = 0; i < sizeof spans / sizeof spans[0]; ++i) {
        const char *b = (const char *)spans[i].base;
        if (p >= b && p < b + spans[i].n) return true;
    }
    return false;
}

// `unexplained`, when given, counts the differing bytes that are NOT in the
// sound command queue (used for the .dsstate region only).
size_t diff_region(const char *name, const char *want, const char *got, size_t n,
                   size_t *unexplained = 0)
{
    size_t differing = 0, ranges = 0, i = 0, first = (size_t)-1;
    while (i < n) {
        if (want[i] == got[i]) { ++i; continue; }
        size_t j = i;
        while (j < n && want[j] != got[j]) ++j;
        differing += j - i;
        if (unexplained)
            for (size_t q = i; q < j; ++q)
                if (!in_sound_queue(got + q)) ++*unexplained;
        if (first == (size_t)-1) first = i;
        if (ranges < 8)
            fprintf(stderr, "[rb-det]     %s diff at +0x%zx len %zu: was %02x %02x %02x %02x"
                    " now %02x %02x %02x %02x\n", name, i, j - i,
                    (unsigned char)want[i], (unsigned char)want[i + 1],
                    (unsigned char)want[i + 2], (unsigned char)want[i + 3],
                    (unsigned char)got[i], (unsigned char)got[i + 1],
                    (unsigned char)got[i + 2], (unsigned char)got[i + 3]);
        if (ranges < 8 && name[0] == 'a') name_arena_addr(i);
        ++ranges;
        i = j;
    }
    fprintf(stderr, "[rb-det]   %s: %zu differing bytes in %zu ranges\n", name, differing, ranges);
    return differing;
}

void report()
{
    if (g_reported) return;
    g_reported = 1;
    if (!g_ring_ready && !g_snapshots) return;
    const port::CommsRollbackStats c = port::comms_rb_stats();
    fprintf(stderr, "[rollback] ===== summary: snapshots=%llu rollbacks=%llu "
            "frames_replayed=%llu max_depth=%u flushes=%llu unrecoverable=%llu "
            "hw_pages_logged=%llu hw_pages_restored=%llu | transport: "
            "predicted=%llu ok=%llu mispredicted=%llu rewinds=%llu "
            "replayed=%llu stalled=%llu stallevents=%llu drops=%llu =====\n",
            g_snapshots, g_rollbacks, g_frames_replayed, g_max_depth, g_flushes,
            g_unrecoverable, g_hw_pages_logged, g_hw_pages_restored,
            c.predicted, c.confirmed_ok, c.mispredicted, c.rewinds, c.replayed,
            c.stalled, c.stall_events, c.drops);
    print_stat("snapshot (arena+dsstate+hw log)", g_snap);
    print_stat("restore", g_restore);
    print_stat("rollback event (restore+replay)", g_rbevent);
    print_stat("per replayed frame", g_rbframe);
    print_stat("frames per rollback", g_rbframes);
    for (int p = 0; p < 8; ++p) print_stat(kPhase[p], g_rp[p]);
    if (g_local_probe)
        fprintf(stderr, "[rb-local] frames with a round served=%llu same-frame "
                "input in the record=%llu mismatches=%llu -> %s\n",
                g_probe_frames, g_probe_same, g_probe_diff,
                g_probe_diff ? "FAIL" : (g_probe_frames ? "OK" : "no rounds"));
}

void end_replay(bool count = true)
{
    g_replaying = false;
    sd_host_mute(0);
    port::comms_rb_det_reuse(false);
    const double ms = now_ms() - g_replay_t0;
    const unsigned n = g_replay_to - g_replay_from;
    if (!count) return;              // the session ended mid-replay
    note(g_rbevent, ms);
    note(g_rbframes, (double)n);
    if (n) note(g_rbframe, ms / n);
    if (g_verbose)
        fprintf(stderr, "[rollback] replay %u..%u done: %u frame(s) in %.3f ms\n",
                g_replay_from, g_replay_to, n, ms);
}

bool rollback_to(unsigned D, int *frame, unsigned tag, bool det)
{
    if (!ring_find(D)) {
        ++g_unrecoverable;
        fprintf(stderr, "[rollback] round %u is contradicted but the ring's "
                "oldest world is %u rounds back; cannot rewind\n", D, kSlots);
        port::comms_rb_leave("the snapshot ring no longer holds the round");
        ring_flush("unrecoverable");
        return false;
    }
    g_replay_t0 = now_ms();
    // the frame the snapshot was taken at, read BEFORE the restore (which
    // only invalidates slots newer than D). Frames and rounds only agree
    // one-to-one while every frame consumes a round; a frame that served
    // none (a load) re-tags the same slot, so the slot's own frame is the
    // truth and a round-count subtraction is not.
    const int at_frame = ring_find(D)->frame;
    if (!restore(D)) return false;
    if (!port::comms_rb_rewind(D)) {
        // the transport refused (its ring lost the round); the world is
        // already restored, so the honest thing is to stay restored and
        // let the transport's leave run its course
        ++g_unrecoverable;
        ring_flush("the transport could not rewind");
        return false;
    }
    const unsigned depth = tag - D;
    if (depth > g_max_depth) g_max_depth = depth;
    *frame = at_frame;
    g_replaying = true;
    g_replay_from = D;
    g_replay_to = tag;
    ++g_rollbacks;
    g_frames_replayed += depth;
    if (det) port::comms_rb_det_reuse(true);
    sd_host_mute(1);
    if (g_verbose)
        fprintf(stderr, "[rollback] rewind to round %u (%u frame(s)) at frame %d%s\n",
                D, depth, at_frame, det ? " [det]" : "");
    return true;
}

void local_probe(unsigned tag)
{
    unsigned r;
    const unsigned char *mine = port::comms_rb_my_served(&r);
    if (!mine || r + 1 != tag) return;      // no round served this frame
    const int slot = port::comms_loopback_stats().slot;
    const unsigned char *rec = data_020a1154 + slot * 0x24;
    ++g_probe_frames;
    // block +6 (the key word) lands at record +0xE (unkE)
    if (memcmp(mine + 6, rec + 0xE, 2) == 0 && memcmp(mine + 0xB, rec + 0x10, 2) == 0)
        ++g_probe_same;
    else
        ++g_probe_diff;
}

} // namespace

extern "C" {

int rb_replaying(void) { return g_replaying; }
int rb_skip_render(void)
{
    return g_replaying && !g_no_render_skip && port::comms_rb_replaying();
}
// The tick-only re-sim: the actors' Render bodies stand down too. Off with
// SM64DS_ROLLBACK_ACTOR_RENDER=1 (the conservative re-sim of the spike, which
// keeps every Render body and drops only the rasteriser and the present).
int rb_skip_actor_render(void)
{
    return g_replaying && !g_no_render_skip && !g_keep_actor_render &&
           port::comms_rb_replaying();
}
void rb_replay_phase(int idx, double ms)
{
    if (idx >= 0 && idx < 8) note(g_rp[idx], ms);
}
int rb_owns_det(void) { read_env(); return port::comms_rb_mode() && g_det_frame >= 0; }

void rb_frame_end(int *frame, int selftest)
{
    if (!port::comms_rb_mode()) return;
    read_env();
    const bool up = port::comms_rb_enabled();
    if (!up) {
        if (g_ring_live) { ring_flush("the session ended"); g_ring_live = false; }
        if (g_replaying) end_replay(false);
        if (selftest && *frame + 1 >= selftest) report();
        return;
    }
    if (!g_ring_live) {
        if (!ring_init()) return;
        ring_flush(0);
        g_ring_live = true;
        atexit(report);
    }
    if (g_pause_frame >= 0 && *frame == g_pause_frame && !g_pause_done) {
        g_pause_done = 1;
        fprintf(stderr, "[rollback] PAUSE: sleeping %d ms at frame %d with the "
                "pump stopped\n", g_pause_ms, *frame);
        Sleep((DWORD)g_pause_ms);
        ++g_stalls_paused;
    }
    const unsigned tag = port::comms_rb_round();
    if (g_local_probe) local_probe(tag);

    if (snapshot(tag, *frame)) {
        ring_flush("the hardware stores changed wholesale (an area change)");
        // the slot for THIS tag stays valid with an empty log: the shadow
        // was just rebuilt from the live stores
        Slot &s = g_ring[tag % kSlots];
        s.valid = true; s.tag = tag; s.undo_n = 0;
        port::comms_rb_flush("area change");
    }

    if (g_replaying && !port::comms_rb_replaying()) {
        end_replay();
        if (g_det_phase == 2 && *frame == g_det_frame + g_det_n) {
            uint64_t h[3];
            hash_world(h);
            const bool same = h[0] == g_det_straight[0] && h[1] == g_det_straight[1] &&
                              h[2] == g_det_straight[2];
            fprintf(stderr, "[rb-det] f%d re-run: arena %016llx dsstate %016llx hw "
                    "%016llx -> %s\n", *frame, (unsigned long long)h[0],
                    (unsigned long long)h[1], (unsigned long long)h[2],
                    same ? "BYTE-IDENTICAL to the straight run" : "DIVERGED");
            if (!same) {
                char *hw = g_hw_n ? big_alloc(port_hw_regions_size()) : 0;
                if (hw) port_hw_regions_copy_out(hw);
                size_t da = 0, dd = 0, dh = 0, dx = 0;
                if (h[0] != g_det_straight[0])
                    da = diff_region("arena", g_det_arena, (const char *)port_arena_base(), g_arena_size);
                if (h[1] != g_det_straight[1])
                    dd = diff_region("dsstate", g_det_ds, &dsstate_lo, g_ds_size, &dx);
                if (h[2] != g_det_straight[2] && hw)
                    dh = diff_region("hw", g_det_hw, hw, port_hw_regions_size());
                fprintf(stderr, "[rb-det] arena %zu bytes, dsstate %zu bytes (%zu outside the "
                        "sound command queue), hw %zu bytes differ\n", da, dd, dx, dh);
                // The verdict the rig reads. IDENTICAL-EXCEPT-SOUNDQUEUE is
                // the pass: the game's world came back byte-for-byte and only
                // the re-seeded audio queue moved (see in_sound_queue).
                printf("rb-det: %s arena=%zu dsstate=%zu hw=%zu soundqueue=%zu\n",
                       (da == 0 && dh == 0 && dx == 0) ? "IDENTICAL-EXCEPT-SOUNDQUEUE"
                       : (da == 0 && dh == 0) ? "DSSTATE-DIFFERS" : "DIVERGED",
                       da, dd, dh, dd - dx);
            } else {
                fprintf(stderr, "[rb-det] arena 0 bytes, dsstate 0 bytes, hw 0 bytes differ\n");
                printf("rb-det: IDENTICAL arena=0 dsstate=0 hw=0\n");
            }
            g_det_phase = 3;
        }
    }
    if (g_replaying) return;             // mid-replay: no scan, no new rewind

    if (g_det_frame >= 0) {
        if (g_det_phase == 0 && *frame == g_det_frame) {
            g_det_tag = tag;
            g_det_phase = 1;
            fprintf(stderr, "[rb-det] f%d: round %u marked; %d frames on, the "
                    "world is hashed and rolled back here through the live "
                    "rollback path\n", *frame, tag, g_det_n);
        } else if (g_det_phase == 1 && *frame == g_det_frame + g_det_n) {
            hash_world(g_det_straight);
            keep_world(&g_det_arena, &g_det_ds, &g_det_hw);
            fprintf(stderr, "[rb-det] f%d straight run: arena %016llx dsstate "
                    "%016llx hw %016llx; rolling back to round %u (%u frames) "
                    "and replaying with the served blocks\n", *frame,
                    (unsigned long long)g_det_straight[0],
                    (unsigned long long)g_det_straight[1],
                    (unsigned long long)g_det_straight[2], g_det_tag, tag - g_det_tag);
            if (rollback_to(g_det_tag, frame, tag, true)) g_det_phase = 2;
            else { fprintf(stderr, "[rb-det] rollback refused; check off\n"); g_det_phase = 3; }
            return;
        }
    }

    const unsigned D = port::comms_rb_scan();
    if (D != ~0u) rollback_to(D, frame, tag, false);
    else if (g_force_every > 0 && *frame > 0 && *frame % g_force_every == 0 &&
             (int)g_force_depth < (int)tag && ring_find(tag - g_force_depth))
        rollback_to(tag - g_force_depth, frame, tag, true);

    if (selftest && *frame + 1 >= selftest && !g_replaying) report();
}

} // extern "C"
