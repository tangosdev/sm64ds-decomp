/* THE LEVEL 43 POLE WRITE-WATCH (run after093, lane POLELOG1). A field
 * diagnostic, on in every build, active only while level 43 is loaded.
 *
 * THE REPORTS. Twelve crash-ingest bundles (0.9.3.1 to 0.9.3.7), every one on
 * level 43, every one the same fault: during the level's teardown one
 * INVISIBLE_POLE (daBar_c, actor id 287) has fBase_c::heap (+0x4c) == 4, and
 * fBase_c::AfterCleanupResources reads through it (Heap_t::_Destroy, reading
 * address 4). The quarantine freezes the pole and the game carries on. In all
 * twelve the faulting pole sits exactly 0x500 below the player object. Forty
 * rows on dev and kit exes did not reproduce it (lane POLEFAULT1), so the
 * code that writes the 4 is still unnamed.
 *
 * WHAT THIS DOES. Thirty game ticks into level 43 (after every pole has been
 * constructed and has run InitResources, the only writes POLEFAULT1's
 * debugger watch ever saw), it arms one x86 hardware data breakpoint (debug
 * register, write, 4 bytes) on the heap word of each INVISIBLE_POLE, up to the
 * number of debug registers nobody else is using. The pole at player - 0x500
 * goes first; the rest follow in actor-list order. A vectored handler logs
 * one playlog line per hit (frame, writer eip as exe offset, old and new
 * value, the walker's actor id and class, two return addresses off the
 * stack), at most 8 per level load, and resumes. It never stops the game,
 * never writes game state, never quarantines.
 *
 * It disarms at the top of port_level_teardown (hal/level_change.cpp, through
 * port_polelog_disarm_hook, which the first tick sets), before
 * any pole's cleanup runs, and when the crash writer asks for its text. With
 * a debugger attached it never arms (the debugger owns the registers), and
 * under Wine / Proton it never arms (ntdll exports wine_get_version there).
 *
 * THREADS. Debug registers are per thread. Arm, disarm and the actor walk all
 * run on the game thread, which is the only thread that runs actor code; a
 * write from another thread would not be seen (none is expected).
 *
 * CRASH TEXT. port_crash_extra_text (tests/fault_probe.h) is pointed at
 * polelog_crash_text once this file is live: a crash or quarantine dump on
 * level 43, or with an INVISIBLE_POLE as the walker, gets the 0x80 bytes at
 * the walker pole, the 0x80 bytes at player - 0x500 and the player's own
 * address.
 *
 * Changes no ROM behaviour: it only reads actor memory and the CPU's debug
 * registers. */
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern signed char data_0209f2f8;     /* current level */
extern void *data_0209b468;           /* the live-actor list head node */
extern void *data_0209f394[];         /* per-player Actor* */
extern unsigned char data_0209f250;   /* local player idx */
extern int *data_020a4b68;            /* the walker's parked node */
const char *port_actor_class_name(unsigned id);   /* hal/star_flow.cpp */
/* tests/fault_probe.h: the crash writer calls this if set */
typedef void (*port_crash_extra_fn)(char *buf, unsigned *p, unsigned cap);
extern port_crash_extra_fn port_crash_extra_text;
void port_polelog_tick(int frame);
void port_polelog_disarm(void);
extern void (*port_polelog_disarm_hook)(void);   /* hal/level_change.cpp */
}

namespace {

const int kLevel = 43;
const unsigned kPoleId = 287;        /* INVISIBLE_POLE, daBar_c */
const unsigned kHeapOff = 0x4c;      /* fBase_c::heap */
const int kArmTick = 30;             /* game ticks into the level */
const int kCapPerLoad = 8;

struct Node { void *x0; Node *next; char *actor; };

int g_ticks;            /* consecutive ticks on level 43 this load */
int g_tried;            /* arm attempted this load (armed or refused) */
int g_armed;            /* our slots are live in DR7 */
int g_n;                /* watched words */
int g_slot[4];          /* debug register index per watched word */
unsigned g_addr[4];
unsigned g_last[4];
char *g_pole[4];
int g_hits;             /* lines logged this load */
int g_frame;
PVOID g_veh;
char *g_modbase;
unsigned g_text_lo, g_text_hi;

void text_range(void)
{
    if (g_modbase) return;
    g_modbase = (char *)GetModuleHandleA(0);
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)g_modbase;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(g_modbase + dos->e_lfanew);
    IMAGE_SECTION_HEADER *s = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++s)
        if (s->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
            const unsigned lo = (unsigned)(size_t)g_modbase + s->VirtualAddress;
            const unsigned hi = lo + s->Misc.VirtualSize;
            if (!g_text_lo || lo < g_text_lo) g_text_lo = lo;
            if (hi > g_text_hi) g_text_hi = hi;
        }
}

char *local_player(void)
{
    if (data_0209f250 >= 8) return 0;
    return (char *)data_0209f394[data_0209f250];
}

unsigned walker_id(char **actor)
{
    *actor = 0;
    if (!data_020a4b68 || IsBadReadPtr(data_020a4b68, 12)) return 0xffffu;
    char *a = (char *)(size_t)data_020a4b68[2];
    *actor = a;
    if (!a || IsBadReadPtr(a, 0x10)) return 0xffffu;
    return *(unsigned short *)(a + 0xc);
}

/* One line, formatted into a static buffer first: the handler runs inside
   whatever code did the write, so nothing here allocates. */
char g_line[512];

LONG WINAPI polelog_veh(EXCEPTION_POINTERS *ep)
{
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP || !g_armed)
        return EXCEPTION_CONTINUE_SEARCH;
    const DWORD dr6 = ep->ContextRecord->Dr6;
    int mine = 0;
    for (int i = 0; i < g_n; ++i) {
        const unsigned now = *(volatile unsigned *)(size_t)g_addr[i];
        const int fired = (dr6 >> g_slot[i]) & 1;
        if (!fired && now == g_last[i]) continue;
        mine = 1;
        if (g_hits < kCapPerLoad) {
            ++g_hits;
            char *wa;
            const unsigned wid = walker_id(&wa);
            const char *cls = wid != 0xffffu ? port_actor_class_name(wid) : 0;
            unsigned ret[2] = {0, 0};
            int nr = 0;
            const unsigned *sp = (const unsigned *)(size_t)ep->ContextRecord->Esp;
            for (int k = 0; k < 256 && nr < 2; ++k) {
                if (IsBadReadPtr(sp + k, 4)) break;
                const unsigned v = sp[k];
                if (v >= g_text_lo && v < g_text_hi)
                    ret[nr++] = v - (unsigned)(size_t)g_modbase;
            }
            std::snprintf(g_line, sizeof g_line,
                "[polelog] f%d HIT pole %p +0x4c (watch %d of %d, dr%d) %08x -> %08x "
                "eip=+0x%08x ret=+0x%08x +0x%08x walker %p id %d (%s) hit %d of %d\n",
                g_frame, (void *)g_pole[i], i + 1, g_n, g_slot[i], g_last[i], now,
                (unsigned)((char *)ep->ExceptionRecord->ExceptionAddress - g_modbase),
                ret[0], ret[1], (void *)wa, wid == 0xffffu ? -1 : (int)wid,
                cls ? cls : "?", g_hits, kCapPerLoad);
            std::fputs(g_line, stderr);
            std::fflush(stderr);
        }
        g_last[i] = now;
    }
    if (!mine) {
        /* A data trap whose Dr6 came back empty and whose value did not
           change (a same-value write on a host that does not report Dr6).
           Nothing else in a player build uses the debug registers or the trap
           flag, so while armed, a single step with TF clear and no Dr6 bit is
           ours: resume it here, or the quarantine filter would see it and
           freeze the walker. */
        if ((ep->ContextRecord->EFlags & 0x100) || (dr6 & 0xf))
            return EXCEPTION_CONTINUE_SEARCH;   /* somebody else's breakpoint */
        static int said;
        if (!said) {
            said = 1;
            std::snprintf(g_line, sizeof g_line,
                "[polelog] f%d unattributed single step (Dr6 %08lx, TF clear) eip=+0x%08x: resumed\n",
                g_frame, (unsigned long)dr6,
                (unsigned)((char *)ep->ExceptionRecord->ExceptionAddress - g_modbase));
            std::fputs(g_line, stderr);
            std::fflush(stderr);
        }
    }
    ep->ContextRecord->Dr6 = 0;
    ep->ContextRecord->EFlags |= 0x10000;   /* RF; harmless for a data trap */
    return EXCEPTION_CONTINUE_EXECUTION;
}

void clear_slots(CONTEXT *ctx)
{
    DWORD *dr[4] = {&ctx->Dr0, &ctx->Dr1, &ctx->Dr2, &ctx->Dr3};
    for (int i = 0; i < g_n; ++i) {
        const int s = g_slot[i];
        *dr[s] = 0;
        ctx->Dr7 &= ~(3u << (2 * s));
        ctx->Dr7 &= ~(0xfu << (16 + 4 * s));
    }
}

void arm(void)
{
    g_tried = 1;
    g_hits = 0;
    /* Wine / Proton: its debug-register support is not the same as Windows'
       (Dr6 may come back empty), so never arm there and make no context
       call at all. */
    if (HMODULE nt = GetModuleHandleA("ntdll.dll"))
        if (GetProcAddress(nt, "wine_get_version")) {
            std::fprintf(stderr, "[polelog] wine: not armed\n");
            return;
        }
    if (IsDebuggerPresent()) {
        std::fprintf(stderr, "[polelog] level %d: debugger attached, watch not armed\n", kLevel);
        return;
    }
    text_range();
    char *pl = local_player();
    char *want = pl ? pl - 0x500 : 0;
    char *poles[16];
    int np = 0, total = 0;
    for (Node *n = *(Node **)&data_0209b468; n; n = n->next) {
        if (IsBadReadPtr(n, sizeof *n)) break;
        char *a = n->actor;
        if (!a || IsBadReadPtr(a, 0x50)) continue;
        if (*(unsigned short *)(a + 0xc) != kPoleId) continue;
        ++total;
        if (np < 16) poles[np++] = a;
    }
    /* the player - 0x500 pole first, then actor-list order */
    for (int i = 1; i < np; ++i)
        if (poles[i] == want) {
            char *t = poles[i];
            std::memmove(poles + 1, poles, i * sizeof *poles);
            poles[0] = t;
            break;
        }
    CONTEXT ctx;
    std::memset(&ctx, 0, sizeof ctx);
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    HANDLE th = GetCurrentThread();
    if (!GetThreadContext(th, &ctx)) {
        std::fprintf(stderr, "[polelog] level %d: GetThreadContext failed %lu, not armed\n",
                     kLevel, GetLastError());
        return;
    }
    DWORD *dr[4] = {&ctx.Dr0, &ctx.Dr1, &ctx.Dr2, &ctx.Dr3};
    g_n = 0;
    for (int s = 0; s < 4 && g_n < np; ++s) {
        if (ctx.Dr7 & (3u << (2 * s))) continue;      /* in use by someone else */
        char *p = poles[g_n];
        g_pole[g_n] = p;
        g_slot[g_n] = s;
        g_addr[g_n] = (unsigned)(size_t)(p + kHeapOff);
        g_last[g_n] = *(volatile unsigned *)(size_t)g_addr[g_n];
        *dr[s] = (DWORD)g_addr[g_n];
        ctx.Dr7 |= 1u << (2 * s);            /* local enable */
        ctx.Dr7 |= 0x1u << (16 + 4 * s);     /* break on write */
        ctx.Dr7 |= 0x3u << (18 + 4 * s);     /* 4 bytes */
        ++g_n;
    }
    if (!g_n) {
        std::fprintf(stderr, "[polelog] level %d: %d pole(s), no free debug register, not armed\n",
                     kLevel, total);
        return;
    }
    if (!g_veh)
        g_veh = AddVectoredExceptionHandler(1, polelog_veh);
    g_armed = 1;
    const BOOL ok = SetThreadContext(th, &ctx);
    CONTEXT chk;
    std::memset(&chk, 0, sizeof chk);
    chk.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(th, &chk);
    if (!ok) g_armed = 0;
    std::fprintf(stderr, "[polelog] f%d level %d: armed %d of %d pole(s), player %p, "
                 "first %s player-0x500, set=%d dr7=%08lx",
                 g_frame, kLevel, g_n, total, (void *)pl,
                 (g_n && g_pole[0] == want) ? "IS" : "is NOT", (int)ok, (unsigned long)chk.Dr7);
    for (int i = 0; i < g_n; ++i)
        std::fprintf(stderr, " | dr%d %p+0x4c=%08x", g_slot[i], (void *)g_pole[i], g_last[i]);
    std::fprintf(stderr, "\n");
    std::fflush(stderr);
}

/* SM64DS_POLELOG_POKE=<tick>[,keep] (test builds only, compiled out of the
   kit): on that level-43 tick, write 4 into the first watched word, the value
   the field reports found. Without ",keep" the old value goes back on the next
   statement (two hits: the 4 and the restore); with ",keep" the 4 stays, so the
   level's teardown faults the way the reports do and the crash text shows. */
#ifndef PORT_ROM_CLEAN
void test_poke(void)
{
    static int read, at = -1, keep;
    if (!read) {
        read = 1;
        if (const char *e = std::getenv("SM64DS_POLELOG_POKE")) {
            at = std::atoi(e);
            keep = std::strstr(e, "keep") != 0;
        }
    }
    if (at < 0 || g_ticks != at || !g_armed || !g_n) return;
    volatile unsigned *w = (volatile unsigned *)(size_t)g_addr[0];
    const unsigned old = *w;
    std::fprintf(stderr, "[polelog] f%d TEST POKE 4 into %p (keep=%d)\n", g_frame, (void *)w, keep);
    *w = 4;
    if (!keep) *w = old;
}
#endif

void disarm_now(const char *why)
{
    if (!g_armed) return;
    g_armed = 0;
    CONTEXT ctx;
    std::memset(&ctx, 0, sizeof ctx);
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    HANDLE th = GetCurrentThread();
    if (GetThreadContext(th, &ctx)) {
        clear_slots(&ctx);
        SetThreadContext(th, &ctx);
    }
    std::fprintf(stderr, "[polelog] f%d level %d: disarmed (%s), %d hit line(s)", g_frame,
                 kLevel, why, g_hits);
    for (int i = 0; i < g_n; ++i) {
        const unsigned v = IsBadReadPtr((void *)(size_t)g_addr[i], 4)
                           ? 0xffffffffu : *(volatile unsigned *)(size_t)g_addr[i];
        std::fprintf(stderr, " | %p+0x4c=%08x", (void *)g_pole[i], v);
    }
    std::fprintf(stderr, "\n");
    std::fflush(stderr);
}

/* PORT_RD-style appends: the crash writer's buffer, no CRT. */
void put_str(char *buf, unsigned *p, unsigned cap, const char *s)
{
    while (*s && *p + 16 < cap) buf[(*p)++] = *s++;
}
void put_hex(char *buf, unsigned *p, unsigned cap, unsigned v, int w)
{
    static const char hx[] = "0123456789abcdef";
    if (*p + 16 >= cap) return;
    for (int k = w - 1; k >= 0; --k) buf[(*p)++] = hx[(v >> (4 * k)) & 15];
}
void put_bytes(char *buf, unsigned *p, unsigned cap, const char *tag, const char *at)
{
    put_str(buf, p, cap, "\r\npolelog  ");
    put_str(buf, p, cap, tag);
    put_str(buf, p, cap, " ");
    put_hex(buf, p, cap, (unsigned)(size_t)at, 8);
    if (!at || IsBadReadPtr(at, 0x80)) {
        put_str(buf, p, cap, " (unreadable)");
        return;
    }
    for (int r = 0; r < 0x80; r += 0x10) {
        put_str(buf, p, cap, "\r\n  +");
        put_hex(buf, p, cap, (unsigned)r, 2);
        for (int c = 0; c < 0x10; c += 4) {
            put_str(buf, p, cap, " ");
            put_hex(buf, p, cap, *(const unsigned *)(at + r + c), 8);
        }
    }
}

void polelog_crash_text(char *buf, unsigned *p, unsigned cap)
{
    char *wa;
    const unsigned wid = walker_id(&wa);
    if (data_0209f2f8 != kLevel && wid != kPoleId) return;
    disarm_now("crash text");
    char *pl = local_player();
    put_str(buf, p, cap, "\r\npolelog  player ");
    put_hex(buf, p, cap, (unsigned)(size_t)pl, 8);
    put_str(buf, p, cap, " watch hits this load ");
    put_hex(buf, p, cap, (unsigned)g_hits, 2);
    if (wid == kPoleId) put_bytes(buf, p, cap, "walker pole", wa);
    if (pl) put_bytes(buf, p, cap, "player-0x500", pl - 0x500);
}

}  // namespace

extern "C" void port_polelog_tick(int frame)
{
    g_frame = frame;
    if (!port_crash_extra_text) port_crash_extra_text = polelog_crash_text;
    if (!port_polelog_disarm_hook) port_polelog_disarm_hook = port_polelog_disarm;
    if (data_0209f2f8 != kLevel) {
        if (g_armed) disarm_now("level left");
        g_ticks = 0;
        g_tried = 0;
        return;
    }
    ++g_ticks;
    if (!g_tried && g_ticks >= kArmTick) arm();
#ifndef PORT_ROM_CLEAN
    test_poke();
#endif
}

/* hal/level_change.cpp port_level_teardown, first statement: before any
   pole's CleanupResources / AfterCleanupResources runs. */
extern "C" void port_polelog_disarm(void)
{
    disarm_now("teardown");
    g_ticks = 0;
    g_tried = 0;
}
