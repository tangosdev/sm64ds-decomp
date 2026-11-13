/* naming_lab.c -- the live half of the naming lab (plugin API v1).

   The lab is for naming the decomp while playing the port: which actors exist,
   which functions each one runs this frame, what its fields hold, which ROM
   files it loads. This DLL only observes and reports, plus a few lab controls;
   the names, headers, ROM addresses and the dashboard live in lab_server.py,
   which talks to this over UDP on 127.0.0.1:<port> (setting "port", 47621).

   WHAT IT READS
   * The actor list, through the game's own dActor_c::Next.
   * Every function call, through the port's /Gh function-entry trace
     (port/hal/fn_trace.cpp) in LAB MODE (ntr_fn_trace_on = 2): four words a
     call -- callee+5, call site, ecx, first stack argument. Only a TRACING
     build has the hook (port/build-port-trace.cmd); on a plain build the lab
     still lists actors and fields and says "trace=0".
   * Memory, for the fields view, through a fault-guarded copy.

   PORT FACTS ARE CANDIDATES. What this sees is the port running the decomp's
   own source, so it can suggest a name; the ROM (melonDS) has to confirm it.
   The server's export says so on every record.

   COMMANDS (one per datagram; replies go to the sender, streams to every peer
   heard from in the last 10 s):
     hello                      -> "hi ..." (build facts)
     status                     -> "status tick= level= ... calls= dropped= maxcalls=<n>@<tick>
                                   dropframes= lastdrop=" (the trace's fill, this frame and since boot)
     actors                     -> "actors <tick> <n>" then "a ..." lines
     watch <ptr> [size]         stream "snap" of that object every frame (<= 4)
     unwatch <ptr|all>
     trace 0|1                  lab-mode function trace on/off
     focus <ptr> [ptr...]       stream "hits" (callee, kind, count) for these
     hot <frames>               every <frames> frames, the 400 hottest callees
     cap <addr> [addr...]       stream every call to these callees ("cap")
     uncap all
     peek <addr> <len>          -> "mem <addr> <hex>"
     poke <addr> <hex>          write bytes (guarded) -> "poked <addr> <n>"
     tp <x> <y> <z>             move player 0 (fix12 whole-unit ints)
     pause | resume | step [n]  freeze the game loop in this callback
     detour <addr> <dll> <sym>  hot-patch: jmp from <addr> to <sym> in <dll>
     undetour <addr|all>
     wwatch <addr> [1|2|4]      data breakpoint: record every write there (4 at once)
     wwatch clear
     writers                    -> "writers <tick> <frames> <lost>", "w slot addr len", "h slot eip ret1 ret2 n"
*/
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../sm64ds_mod.h"

typedef unsigned int u32;

static const SM64DSModApi *g_api;
static void *(__cdecl *g_next)(const void *after);   /* dActor_c::Next */
static u32 *g_player_table;                          /* data_0209f394 */
static u32 *g_player_count;                          /* data_0209f21c (u8 in the ROM; low byte used) */
static signed char *g_cur_area;                      /* data_02092120 */

/* the port's trace hook globals (tracing builds only) */
static volatile u32 *g_tr_on, *g_tr_head, *g_tr_cap;
static u32 **g_tr_buf;
static u32 *g_buf;
#define TRACE_WORDS (16u * 1024u * 1024u)  /* 4M calls a frame: a level load is one long frame */

static u32 g_tick;
static int g_level = -1;
static int g_trace;
static int g_paused, g_step;

/* ---------------------------------------------------------------- net */
#define MAX_PEERS 4
static SOCKET g_sock = INVALID_SOCKET;
static struct { struct sockaddr_in addr; u32 seen; int used; } g_peers[MAX_PEERS];

static void net_init(int port)
{
    WSADATA wsa;
    struct sockaddr_in a;
    u_long nb = 1;
    int big = 4 << 20;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;
    g_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_sock == INVALID_SOCKET) return;
    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_port = htons((u_short)port);
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    setsockopt(g_sock, SOL_SOCKET, SO_SNDBUF, (const char *)&big, sizeof big);
    if (bind(g_sock, (struct sockaddr *)&a, sizeof a) != 0 || ioctlsocket(g_sock, FIONBIO, &nb) != 0) {
        closesocket(g_sock);
        g_sock = INVALID_SOCKET;
        return;
    }
    {
        /* A datagram streamed to a client that has exited comes back as ICMP
           port-unreachable, and Windows then fails the NEXT recvfrom on this
           socket with WSAECONNRESET. Streams go to every remembered peer each
           frame, so one closed client used to starve the command queue: the
           errors arrived faster than poll() consumed them. Turn that off. */
        BOOL no = FALSE;
        DWORD ret = 0;
        WSAIoctl(g_sock, _WSAIOW(IOC_VENDOR, 12) /* SIO_UDP_CONNRESET */, &no, sizeof no, 0, 0, &ret, 0, 0);
    }
}

static void send_to(const struct sockaddr_in *to, const char *msg, int len)
{
    sendto(g_sock, msg, len, 0, (const struct sockaddr *)to, sizeof *to);
}

static void broadcast(const char *msg, int len)
{
    int i;
    if (g_sock == INVALID_SOCKET) return;
    for (i = 0; i < MAX_PEERS; ++i)
        if (g_peers[i].used && g_tick - g_peers[i].seen < 600)
            send_to(&g_peers[i].addr, msg, len);
}

static void remember(const struct sockaddr_in *from)
{
    int i, slot = -1;
    u32 oldest = 0;
    for (i = 0; i < MAX_PEERS; ++i)
        if (g_peers[i].used && g_peers[i].addr.sin_port == from->sin_port &&
            g_peers[i].addr.sin_addr.s_addr == from->sin_addr.s_addr) {
            g_peers[i].seen = g_tick;
            return;
        }
    for (i = 0; i < MAX_PEERS; ++i) {
        if (!g_peers[i].used) { slot = i; break; }
        if (slot < 0 || g_tick - g_peers[i].seen > oldest) { slot = i; oldest = g_tick - g_peers[i].seen; }
    }
    g_peers[slot].addr = *from;
    g_peers[slot].seen = g_tick;
    g_peers[slot].used = 1;
}

/* A growing text message that flushes itself as a datagram near 60 KB. Every
   datagram starts with the same header line so the server can stitch them. */
typedef struct { char buf[62000]; int len; char head[96]; const struct sockaddr_in *to; } Msg;

static void msg_flush(Msg *m)
{
    if (m->len <= (int)strlen(m->head)) return;
    if (m->to) send_to(m->to, m->buf, m->len); else broadcast(m->buf, m->len);
    m->len = sprintf(m->buf, "%s", m->head);
}

static void msg_begin(Msg *m, const struct sockaddr_in *to, const char *head)
{
    m->to = to;
    _snprintf(m->head, sizeof m->head - 1, "%s\n", head);
    m->head[sizeof m->head - 1] = 0;
    m->len = sprintf(m->buf, "%s", m->head);
}

static void msg_add(Msg *m, const char *line)
{
    int n = (int)strlen(line);
    if (m->len + n + 2 > (int)sizeof m->buf) msg_flush(m);
    memcpy(m->buf + m->len, line, n);
    m->len += n;
    m->buf[m->len++] = '\n';
}

static Msg g_msg;

/* ---------------------------------------------------------------- memory */
static int safe_copy(void *dst, const void *src, size_t n)
{
    __try { memcpy(dst, src, n); return 1; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static int safe_write(void *dst, const void *src, size_t n)
{
    DWORD old;
    __try { memcpy(dst, src, n); return 1; }
    __except (EXCEPTION_EXECUTE_HANDLER) { }
    if (!VirtualProtect(dst, n, PAGE_EXECUTE_READWRITE, &old)) return 0;
    __try { memcpy(dst, src, n); }
    __except (EXCEPTION_EXECUTE_HANDLER) { VirtualProtect(dst, n, old, &old); return 0; }
    VirtualProtect(dst, n, old, &old);
    FlushInstructionCache(GetCurrentProcess(), dst, n);
    return 1;
}

static void hex_of(char *out, const unsigned char *p, int n)
{
    static const char hx[] = "0123456789abcdef";
    int i;
    for (i = 0; i < n; ++i) { out[2 * i] = hx[p[i] >> 4]; out[2 * i + 1] = hx[p[i] & 15]; }
    out[2 * n] = 0;
}

static int unhex(unsigned char *out, const char *s, int cap)
{
    int n = 0;
    while (s[0] && s[1] && n < cap) {
        int hi, lo;
        char c = s[0], d = s[1];
        hi = c <= '9' ? c - '0' : (c | 32) - 'a' + 10;
        lo = d <= '9' ? d - '0' : (d | 32) - 'a' + 10;
        if (hi < 0 || hi > 15 || lo < 0 || lo > 15) break;
        out[n++] = (unsigned char)(hi << 4 | lo);
        s += 2;
    }
    return n;
}

/* ---------------------------------------------------------------- actors */
#define MAX_ACTORS 1024
static u32 g_actors[MAX_ACTORS];
static int g_nactors;

static void *actor_next(const void *after)
{
    void *r = 0;
    __try { r = g_next(after); }
    __except (EXCEPTION_EXECUTE_HANDLER) { r = 0; }
    return r;
}

static void collect_actors(void)
{
    void *a = 0;
    g_nactors = 0;
    if (!g_next) return;
    while ((a = actor_next(a)) != 0 && g_nactors < MAX_ACTORS)
        g_actors[g_nactors++] = (u32)(size_t)a;
}

static u32 player0(void)
{
    int i;
    if (!g_player_table) return 0;
    for (i = 0; i < 4; ++i)
        if (g_player_table[i]) return g_player_table[i];
    return 0;
}

/* per-actor call counts this frame, filled by the trace pass */
static u32 g_acalls[MAX_ACTORS];

static void send_actors(const struct sockaddr_in *to)
{
    char line[256], head[64];
    int i;
    _snprintf(head, sizeof head, "actors %u %d %u", g_tick, g_nactors, player0());
    msg_begin(&g_msg, to, head);
    for (i = 0; i < g_nactors; ++i) {
        unsigned char o[0xd0];
        u32 *w = (u32 *)o;
        if (!safe_copy(o, (void *)(size_t)g_actors[i], sizeof o)) continue;
        /* ptr vtable uniqueID actorID alive x y z angY flags area calls */
        _snprintf(line, sizeof line, "a %x %x %u %u %u %d %d %d %d %x %d %u",
                  g_actors[i], w[0], w[1], *(unsigned short *)(o + 0x0c), o[0x0e],
                  *(int *)(o + 0x5c) >> 12, *(int *)(o + 0x60) >> 12, *(int *)(o + 0x64) >> 12,
                  *(short *)(o + 0x8e), *(u32 *)(o + 0xb0), (signed char)o[0xcc], g_acalls[i]);
        msg_add(&g_msg, line);
    }
    msg_flush(&g_msg);
}

/* ---------------------------------------------------------------- watches */
#define MAX_WATCH 4
static struct { u32 ptr; int size; } g_watch[MAX_WATCH];

static void send_snaps(void)
{
    static unsigned char raw[0x2000];
    static char text[0x4100 + 128];
    int i, n;
    for (i = 0; i < MAX_WATCH; ++i) {
        if (!g_watch[i].ptr) continue;
        n = g_watch[i].size;
        if (!safe_copy(raw, (void *)(size_t)g_watch[i].ptr, n)) { g_watch[i].ptr = 0; continue; }
        n = sprintf(text, "snap %u %x ", g_tick, g_watch[i].ptr);
        hex_of(text + n, raw, g_watch[i].size);
        broadcast(text, (int)strlen(text));
    }
}

/* ---------------------------------------------------------------- trace */
#define MAX_FOCUS 8
static u32 g_focus[MAX_FOCUS];
static int g_nfocus;
static int g_hot_every;          /* 0 = off */
#define MAX_CAP 32
static u32 g_cap[MAX_CAP];       /* callee entry+5 */
static int g_ncap;

/* open-addressing tables, cleared per use */
#define HOT_SLOTS 65536
static u32 g_hot_key[HOT_SLOTS], g_hot_n[HOT_SLOTS];
static u32 g_hot_frames;
#define FH_SLOTS 4096
static u32 g_fh_key[FH_SLOTS], g_fh_ptr[FH_SLOTS], g_fh_n[FH_SLOTS];
static unsigned char g_fh_kind[FH_SLOTS];
#define AS_SLOTS 4096
static u32 g_as_key[AS_SLOTS];
static short g_as_idx[AS_SLOTS];

static u32 hash32(u32 x) { x ^= x >> 16; x *= 0x7feb352d; x ^= x >> 15; x *= 0x846ca68b; x ^= x >> 16; return x; }

static void hot_add(u32 key)
{
    u32 h = hash32(key) & (HOT_SLOTS - 1);
    while (g_hot_key[h] && g_hot_key[h] != key) h = (h + 1) & (HOT_SLOTS - 1);
    g_hot_key[h] = key;
    ++g_hot_n[h];
}

static void fh_add(u32 callee, u32 ptr, int kind)
{
    u32 h = hash32(callee * 31 + ptr) & (FH_SLOTS - 1);
    int probes = 0;
    while (g_fh_key[h] && !(g_fh_key[h] == callee && g_fh_ptr[h] == ptr)) {
        h = (h + 1) & (FH_SLOTS - 1);
        if (++probes > FH_SLOTS / 2) return;
    }
    g_fh_key[h] = callee;
    g_fh_ptr[h] = ptr;
    g_fh_kind[h] |= (unsigned char)kind;
    ++g_fh_n[h];
}

static void actor_index_build(void)
{
    int i;
    memset(g_as_key, 0, sizeof g_as_key);
    for (i = 0; i < g_nactors; ++i) {
        u32 h = hash32(g_actors[i]) & (AS_SLOTS - 1);
        while (g_as_key[h]) h = (h + 1) & (AS_SLOTS - 1);
        g_as_key[h] = g_actors[i];
        g_as_idx[h] = (short)i;
    }
}

static int actor_index(u32 p)
{
    u32 h;
    if (!p) return -1;
    h = hash32(p) & (AS_SLOTS - 1);
    while (g_as_key[h]) {
        if (g_as_key[h] == p) return g_as_idx[h];
        h = (h + 1) & (AS_SLOTS - 1);
    }
    return -1;
}

static void trace_set(int on)
{
    if (!g_tr_on) return;
    if (on) {
        if (!g_buf) g_buf = (u32 *)VirtualAlloc(0, TRACE_WORDS * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!g_buf) return;
        *g_tr_on = 0;
        *g_tr_buf = g_buf;
        *g_tr_cap = TRACE_WORDS;
        *g_tr_head = 0;
        *g_tr_on = 2;
    } else {
        *g_tr_on = 0;
        *g_tr_head = 0;
    }
    g_trace = on;
}

static u32 g_last_calls, g_last_dropped;
static u32 g_max_calls, g_max_tick, g_drop_frames, g_drop_tick;   /* since boot */
static void asset_seen(int li, u32 *r, u32 actor);
#define N_LOADER_SLOTS 10
static u32 g_loader[N_LOADER_SLOTS];
static int g_hooked[N_LOADER_SLOTS];   /* logged by a loader hook, not by trace_drain */

static void trace_drain(void)
{
    u32 n, i, k;
    int f;
    u32 last_actor = 0;
    char line[160], head[64];
    if (!g_trace || !g_tr_on) return;
    *g_tr_on = 0;                       /* our own calls below are not the game's */
    n = *g_tr_head / 4;
    g_last_dropped = (*g_tr_head + 4 > *g_tr_cap);
    g_last_calls = n;
    if (n > g_max_calls) { g_max_calls = n; g_max_tick = g_tick; }
    if (g_last_dropped) { ++g_drop_frames; g_drop_tick = g_tick; }
    memset(g_acalls, 0, sizeof g_acalls);
    if (g_nfocus) {
        memset(g_fh_key, 0, sizeof g_fh_key);
        memset(g_fh_n, 0, sizeof g_fh_n);
        memset(g_fh_kind, 0, sizeof g_fh_kind);
    }
    actor_index_build();
    if (g_ncap) msg_begin(&g_msg, 0, "cap");
    for (i = 0; i < n; ++i) {
        u32 *r = g_buf + 4 * i;
        u32 callee = r[0], ecx = r[2], arg0 = r[3];
        int ai = actor_index(ecx), bi = actor_index(arg0), li;
        if (ai >= 0) { ++g_acalls[ai]; last_actor = ecx; }
        for (li = 0; li < N_LOADER_SLOTS; ++li)
            if (callee == g_loader[li] && callee && !g_hooked[li]) { asset_seen(li, r, last_actor); break; }
        if (bi >= 0 && bi != ai) ++g_acalls[bi];
        if (g_hot_every) hot_add(callee);
        for (f = 0; f < g_nfocus; ++f) {
            int kind = (ecx == g_focus[f] ? 1 : 0) | (arg0 == g_focus[f] ? 2 : 0);
            if (kind) fh_add(callee, g_focus[f], kind);
        }
        for (k = 0; k < (u32)g_ncap; ++k)
            if (callee == g_cap[k]) {
                _snprintf(line, sizeof line, "%u %x %x %x %x", g_tick, callee, r[1], ecx, arg0);
                msg_add(&g_msg, line);
            }
    }
    if (g_ncap) msg_flush(&g_msg);
    if (g_nfocus) {
        _snprintf(head, sizeof head, "hits %u %u", g_tick, n);
        msg_begin(&g_msg, 0, head);
        for (i = 0; i < FH_SLOTS; ++i)
            if (g_fh_key[i]) {
                _snprintf(line, sizeof line, "%x %x %u %u", g_fh_ptr[i], g_fh_key[i], g_fh_kind[i], g_fh_n[i]);
                msg_add(&g_msg, line);
            }
        msg_flush(&g_msg);
        if (g_msg.len == (int)strlen(g_msg.head)) broadcast(g_msg.buf, g_msg.len);   /* empty frame still marks time */
    }
    if (g_hot_every && ++g_hot_frames >= (u32)g_hot_every) {
        /* the 400 hottest: a partial selection, cheap at this size */
        static u32 top_k[400], top_n[400];
        int nt = 0, j;
        for (i = 0; i < HOT_SLOTS; ++i) {
            if (!g_hot_key[i]) continue;
            if (nt < 400) { top_k[nt] = g_hot_key[i]; top_n[nt] = g_hot_n[i]; ++nt; continue; }
            for (j = 0, k = 0; j < 400; ++j) if (top_n[j] < top_n[k]) k = j;
            if (g_hot_n[i] > top_n[k]) { top_k[k] = g_hot_key[i]; top_n[k] = g_hot_n[i]; }
        }
        _snprintf(head, sizeof head, "hot %u %u", g_tick, g_hot_frames);
        msg_begin(&g_msg, 0, head);
        for (j = 0; j < nt; ++j) {
            _snprintf(line, sizeof line, "%x %u", top_k[j], top_n[j]);
            msg_add(&g_msg, line);
        }
        msg_flush(&g_msg);
        memset(g_hot_key, 0, sizeof g_hot_key);
        memset(g_hot_n, 0, sizeof g_hot_n);
        g_hot_frames = 0;
    }
    *g_tr_head = 0;
    *g_tr_on = 2;
}

/* ---------------------------------------------------------------- assets
   Every ROM file an actor uses goes through a SharedFilePtr (u16 fileID at +0,
   u8 refs at +2, data at +4: func_02017e0c) handed to one of the LoadFile
   functions below. The trace sees each call; the SharedFilePtr is ecx for the
   thiscall members and the first argument for the rest, and by the end of the
   frame its fileID is set. The actor is the last one seen as `this` before the
   call, which during a level load is the one running InitResources. */
static const struct { const char *name; int in_ecx; } k_loaders[] = {
    { "?LoadFile@Model@@SAPAXAAUSharedFilePtr@@@Z", 0 },
    { "__ZN5Model8LoadFileER13SharedFilePtr", 0 },
    { "?LoadFile@SharedFilePtr@@QAEPAXXZ", 1 },
    { "?LoadFile@SharedFilePtr@@QAEXXZ", 1 },
    { "__ZN13SharedFilePtr8LoadFileEv", 0 },
    { "?LoadFile@TextureSequence@@SAPAXAAUSharedFilePtr@@@Z", 0 },
    { "__ZN15TextureSequence8LoadFileER13SharedFilePtr", 0 },
    { "?LoadFile@dBgW_Kc@@SAPADAAUSharedFilePtr@@@Z", 0 },
    { "?LoadFile@dExtFrameCtrl_c@@SAPADAAUSharedFilePtr@@@Z", 0 },
    { "__ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr", 0 },
};
#define N_LOADERS (sizeof k_loaders / sizeof k_loaders[0])
#define ASSET_LOG 16384
static struct { u32 tick, loader, caller, sfp, actor; unsigned short file; } g_alog[ASSET_LOG];
static u32 g_nalog;

static void asset_seen(int li, u32 *r, u32 actor)
{
    u32 sfp = k_loaders[li].in_ecx ? r[2] : r[3];
    unsigned short id = 0xffff;
    u32 k = g_nalog % ASSET_LOG;
    safe_copy(&id, (void *)(size_t)sfp, 2);
    g_alog[k].tick = g_tick;
    g_alog[k].loader = r[0];
    g_alog[k].caller = r[1];
    g_alog[k].sfp = sfp;
    g_alog[k].actor = actor;
    g_alog[k].file = id;
    ++g_nalog;
}

/* LOADER HOOKS. A level load is one frame of more than 4M calls, so the trace
   buffer overflows (status: maxcalls=4194304@1 dropframes=1) and the loads late
   in that frame -- most of the level's models -- never reach trace_drain. So each
   loader whose entry is the traced build's `call _penter` gets its own hook: the
   5 bytes become `jmp thunk`, and the thunk is
       push li ; call loader_log ; push entry+5 ; jmp _penter
   loader_log records the call; _penter then sees exactly the call it would
   have (callee+5 = entry+5, the game's return address above it), so the trace
   and the timeline are unchanged. */
static unsigned char *g_thunks;

static void loader_hit(u32 li, u32 caller, u32 ecx, u32 arg0)
{
    u32 r[4], actor = 0;
    r[0] = g_loader[li]; r[1] = caller; r[2] = ecx; r[3] = arg0;
    /* the actor: the last `this` that is a known actor among this frame's
       newest calls, while the trace still has room (not during a level load) */
    if (g_trace && g_tr_on && *g_tr_head + 4 <= *g_tr_cap) {
        u32 h = *g_tr_head / 4, stop = h > 4096 ? h - 4096 : 0;
        while (h-- > stop) if (actor_index(g_buf[4 * h + 2]) >= 0) { actor = g_buf[4 * h + 2]; break; }
    }
    asset_seen((int)li, r, actor);
}

static void __cdecl loader_log_c(u32 li, u32 caller, u32 ecx, u32 arg0)
{
    loader_hit(li, caller, ecx, arg0);
}

/* stack on entry: [esp] thunk return, [esp+4] li, [esp+8] the game's return
   address, [esp+12] the first argument; ecx is the caller's ecx */
static __declspec(naked) void loader_log(void)
{
    __asm {
        pushad
        push    dword ptr [esp+44]      // arg0
        push    ecx
        push    dword ptr [esp+48]      // the game's return address
        push    dword ptr [esp+48]      // li
        call    loader_log_c
        add     esp, 16
        popad
        ret     4
    }
}

static void hooks_install(void)
{
    int li, n = 0;
    if (!g_tr_on) return;   /* a plain build has no _penter to hand back to */
    g_thunks = (unsigned char *)VirtualAlloc(0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!g_thunks) return;
    for (li = 0; li < (int)N_LOADERS && li < N_LOADER_SLOTS; ++li) {
        u32 entry = g_loader[li] ? g_loader[li] - 5 : 0, penter;
        unsigned char code[5], *t = g_thunks + 32 * li;
        int rel;
        if (!entry || !safe_copy(code, (void *)(size_t)entry, 5) || code[0] != 0xE8) continue;
        memcpy(&rel, code + 1, 4);
        penter = entry + 5 + (u32)rel;
        t[0] = 0x6A; t[1] = (unsigned char)li;
        t[2] = 0xE8; rel = (int)((u32)(size_t)loader_log - (u32)(size_t)(t + 7)); memcpy(t + 3, &rel, 4);
        t[7] = 0x68; rel = (int)(entry + 5); memcpy(t + 8, &rel, 4);
        t[12] = 0xE9; rel = (int)(penter - (u32)(size_t)(t + 17)); memcpy(t + 13, &rel, 4);
        code[0] = 0xE9; rel = (int)((u32)(size_t)t - (entry + 5)); memcpy(code + 1, &rel, 4);
        if (safe_write((void *)(size_t)entry, code, 5)) { g_hooked[li] = 1; ++n; }
    }
    g_api->log(g_api, "naming lab: %d loader hooks", n);
}

static void send_assets(const struct sockaddr_in *to)
{
    char head[64], line[128];
    u32 i, first = g_nalog > ASSET_LOG ? g_nalog - ASSET_LOG : 0;
    _snprintf(head, sizeof head, "assets %u %u", g_tick, g_nalog);
    msg_begin(&g_msg, to, head);
    for (i = first; i < g_nalog; ++i) {
        u32 k = i % ASSET_LOG;
        _snprintf(line, sizeof line, "%u %x %x %x %x %u", g_alog[k].tick, g_alog[k].loader, g_alog[k].caller,
                  g_alog[k].sfp, g_alog[k].actor, g_alog[k].file);
        msg_add(&g_msg, line);
    }
    msg_flush(&g_msg);
    if (g_msg.len == (int)strlen(g_msg.head)) send_to(to, g_msg.buf, g_msg.len);
}

/* ---------------------------------------------------------------- detours */
#define MAX_DETOUR 16
static struct { u32 addr; unsigned char orig[5]; HMODULE dll; char path[MAX_PATH]; } g_det[MAX_DETOUR];

static int undetour(u32 addr)
{
    int i, n = 0;
    for (i = 0; i < MAX_DETOUR; ++i)
        if (g_det[i].addr && (addr == 0 || g_det[i].addr == addr)) {
            safe_write((void *)(size_t)g_det[i].addr, g_det[i].orig, 5);
            /* the game may still be inside the old body on another path's stack:
               the DLL stays loaded (it is a copy, so the original can be rebuilt) */
            g_det[i].addr = 0;
            ++n;
        }
    return n;
}

/* The patch DLL may export  void __cdecl lab_patch_init(void *(*sym)(const char *), void *orig)
   to resolve game symbols and learn where the original body continues. On a
   tracing build every function opens with the 5-byte "call _penter", so the
   jmp replaces exactly that call and `orig` = addr + 5 is the untouched rest. */
static const SM64DSModApi *g_api_for_sym;
static void *__cdecl lab_sym(const char *name) { return g_api_for_sym->symbol(g_api_for_sym, name); }

static int detour(u32 addr, const char *dll, const char *sym, char *err, int errcap)
{
    char tmp[MAX_PATH], dir[MAX_PATH];
    HMODULE h;
    FARPROC fn, init;
    unsigned char jmp[5];
    int i, slot = -1;
    unsigned char first;
    undetour(addr);
    for (i = 0; i < MAX_DETOUR; ++i) if (!g_det[i].addr) { slot = i; break; }
    if (slot < 0) { _snprintf(err, errcap, "detour table full"); return 0; }
    if (!safe_copy(&first, (void *)(size_t)addr, 1)) { _snprintf(err, errcap, "bad address"); return 0; }
    /* load a private copy so the source DLL can be rebuilt while it is in use */
    GetTempPathA(sizeof dir, dir);
    _snprintf(tmp, sizeof tmp, "%snaming-lab-patch-%u-%u.dll", dir, GetCurrentProcessId(), GetTickCount());
    if (!CopyFileA(dll, tmp, FALSE)) { _snprintf(err, errcap, "cannot copy %s (%lu)", dll, GetLastError()); return 0; }
    h = LoadLibraryA(tmp);
    if (!h) { _snprintf(err, errcap, "LoadLibrary failed (%lu)", GetLastError()); return 0; }
    fn = GetProcAddress(h, sym);
    if (!fn) { _snprintf(err, errcap, "no export %s", sym); FreeLibrary(h); return 0; }
    init = GetProcAddress(h, "lab_patch_init");
    if (init) ((void (__cdecl *)(void *(__cdecl *)(const char *), void *))init)(lab_sym, (void *)(size_t)(addr + 5));
    safe_copy(g_det[slot].orig, (void *)(size_t)addr, 5);
    jmp[0] = 0xE9;
    *(int *)(jmp + 1) = (int)((u32)(size_t)fn - (addr + 5));
    if (!safe_write((void *)(size_t)addr, jmp, 5)) { _snprintf(err, errcap, "cannot write code"); return 0; }
    g_det[slot].addr = addr;
    g_det[slot].dll = h;
    strncpy(g_det[slot].path, tmp, MAX_PATH - 1);
    _snprintf(err, errcap, "first byte was %02x", first);
    return 1;
}

/* ---------------------------------------------------------------- who writes
   x86 data breakpoints on the game thread: DR0..DR3 each watch 1, 2 or 4
   aligned bytes for writes. The CPU traps after the writing instruction, and a
   vectored handler records where it stopped: the instruction after the write,
   and the next two return addresses up the ebp chain (the port keeps frame
   pointers), so a write done by a shared helper (a Vector3 copy, a memset)
   still names its caller. The lab server maps those addresses to functions.
   The handler only reads; it clears DR6 and lets the game continue. */
#define WW_SLOTS 4
#define WW_HITS 256
static struct { u32 addr, len; } g_ww[WW_SLOTS];
static struct { u32 slot, eip, r1, r2, n; } g_wh[WW_HITS];
static volatile LONG g_nwh;
static u32 g_ww_since, g_ww_lost;
static PVOID g_veh;

static u32 ww_read(u32 a)
{
    u32 v = 0;
    __try { v = *(u32 *)(size_t)a; } __except (EXCEPTION_EXECUTE_HANDLER) { v = 0; }
    return v;
}

static LONG CALLBACK ww_handler(EXCEPTION_POINTERS *ep)
{
    CONTEXT *c = ep->ContextRecord;
    u32 hit = c->Dr6 & 0xf, s, eip, r1, r2, ebp, i;
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP || !hit) return EXCEPTION_CONTINUE_SEARCH;
    eip = c->Eip;
    ebp = c->Ebp;
    r1 = ww_read(ebp + 4);
    r2 = ww_read(ww_read(ebp) + 4);
    for (s = 0; s < WW_SLOTS; ++s) {
        if (!(hit & (1u << s))) continue;
        for (i = 0; i < (u32)g_nwh; ++i)
            if (g_wh[i].slot == s && g_wh[i].eip == eip && g_wh[i].r1 == r1) { ++g_wh[i].n; break; }
        if (i == (u32)g_nwh) {
            if (g_nwh < WW_HITS) {
                g_wh[i].slot = s; g_wh[i].eip = eip; g_wh[i].r1 = r1; g_wh[i].r2 = r2; g_wh[i].n = 1;
                ++g_nwh;
            } else ++g_ww_lost;
        }
    }
    c->Dr6 = 0;
    return EXCEPTION_CONTINUE_EXECUTION;
}

/* called on the game thread (poll runs inside on_frame) */
static int ww_apply(void)
{
    CONTEXT c;
    int s;
    HANDLE th = GetCurrentThread();
    if (!g_veh) g_veh = AddVectoredExceptionHandler(1, ww_handler);
    memset(&c, 0, sizeof c);
    c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    if (!GetThreadContext(th, &c)) return 0;
    c.Dr7 = 0;
    for (s = 0; s < WW_SLOTS; ++s) {
        u32 len = g_ww[s].len, enc = len == 4 ? 3 : len == 2 ? 1 : 0;
        (&c.Dr0)[s] = g_ww[s].addr;
        if (g_ww[s].addr) c.Dr7 |= (1u << (2 * s)) | (1u << (16 + 4 * s)) | (enc << (18 + 4 * s));
    }
    c.Dr6 = 0;
    return SetThreadContext(th, &c) != 0;
}

/* ---------------------------------------------------------------- commands */
static void reply(const struct sockaddr_in *to, const char *fmt, ...)
{
    char b[1024];
    va_list ap;
    int n;
    va_start(ap, fmt);
    n = _vsnprintf(b, sizeof b - 1, fmt, ap);
    va_end(ap);
    if (n < 0) n = sizeof b - 1;
    b[n] = 0;
    send_to(to, b, n);
}

static void status(const struct sockaddr_in *to)
{
    char b[400];
    int n = _snprintf(b, sizeof b - 1,
                      "status tick=%u level=%d area=%d actors=%d trace=%d trace_avail=%d calls=%u dropped=%u "
                      "paused=%d focus=%d watch=%d player=%x maxcalls=%u@%u dropframes=%u lastdrop=%u",
                      g_tick, g_level, g_cur_area ? *g_cur_area : -1, g_nactors, g_trace,
                      g_tr_on != 0, g_last_calls, g_last_dropped, g_paused, g_nfocus,
                      (g_watch[0].ptr != 0) + (g_watch[1].ptr != 0) + (g_watch[2].ptr != 0) + (g_watch[3].ptr != 0),
                      player0(), g_max_calls, g_max_tick, g_drop_frames, g_drop_tick);
    b[sizeof b - 1] = 0;
    if (to) send_to(to, b, n); else broadcast(b, n);
}

static void command(char *buf, const struct sockaddr_in *from)
{
    u32 a, b;
    int v, i;
    char s1[MAX_PATH], s2[128];
    if (!strncmp(buf, "hello", 5)) {
        reply(from, "hi release=%s base=%x trace_avail=%d next=%x tick=%u", g_api->game_release,
              (u32)(size_t)GetModuleHandleA(0), g_tr_on != 0, (u32)(size_t)g_next, g_tick);
    } else if (!strncmp(buf, "assets", 6)) {
        send_assets(from);
    } else if (!strncmp(buf, "status", 6)) {
        status(from);
    } else if (!strncmp(buf, "actors", 6)) {
        collect_actors();
        send_actors(from);
    } else if (sscanf(buf, "watch %x %i", &a, &v) >= 1) {
        if (sscanf(buf, "watch %x %i", &a, &v) < 2) v = 0x400;
        if (v < 4) v = 4;
        if (v > 0x2000) v = 0x2000;
        for (i = 0; i < MAX_WATCH; ++i) if (g_watch[i].ptr == a) break;
        if (i == MAX_WATCH) for (i = 0; i < MAX_WATCH; ++i) if (!g_watch[i].ptr) break;
        if (i == MAX_WATCH) i = 0;
        g_watch[i].ptr = a;
        g_watch[i].size = v;
    } else if (!strncmp(buf, "unwatch", 7)) {
        if (sscanf(buf, "unwatch %x", &a) == 1) { for (i = 0; i < MAX_WATCH; ++i) if (g_watch[i].ptr == a) g_watch[i].ptr = 0; }
        else memset(g_watch, 0, sizeof g_watch);
    } else if (sscanf(buf, "trace %i", &v) == 1) {
        trace_set(v != 0);
        status(from);
    } else if (!strncmp(buf, "focus", 5)) {
        char *p = buf + 5;
        g_nfocus = 0;
        while (g_nfocus < MAX_FOCUS && sscanf(p, " %x%n", &a, &v) == 1) { if (a) g_focus[g_nfocus++] = a; p += v; }
    } else if (sscanf(buf, "hot %i", &v) == 1) {
        g_hot_every = v < 0 ? 0 : v;
        g_hot_frames = 0;
        memset(g_hot_key, 0, sizeof g_hot_key);
        memset(g_hot_n, 0, sizeof g_hot_n);
    } else if (!strncmp(buf, "uncap", 5)) {
        g_ncap = 0;
    } else if (!strncmp(buf, "cap", 3)) {
        char *p = buf + 3;
        while (g_ncap < MAX_CAP && sscanf(p, " %x%n", &a, &v) == 1) { g_cap[g_ncap++] = a; p += v; }
    } else if (sscanf(buf, "peek %x %i", &a, &v) == 2) {
        static unsigned char raw[4096];
        static char text[8300];
        int n;
        if (v > 4096) v = 4096;
        if (v < 1) v = 1;
        if (!safe_copy(raw, (void *)(size_t)a, v)) { reply(from, "mem %x !fault", a); return; }
        n = sprintf(text, "mem %x ", a);
        hex_of(text + n, raw, v);
        send_to(from, text, (int)strlen(text));
    } else if (sscanf(buf, "poke %x %255s", &a, s1) == 2) {
        unsigned char raw[128];
        int n = unhex(raw, s1, sizeof raw);
        reply(from, "poked %x %d", a, n && safe_write((void *)(size_t)a, raw, n) ? n : 0);
    } else if (!strncmp(buf, "tp ", 3)) {
        int x, y, z;
        u32 p = player0();
        if (p && sscanf(buf + 3, "%i %i %i", &x, &y, &z) == 3) {
            int pos[3];
            pos[0] = x << 12; pos[1] = y << 12; pos[2] = z << 12;
            safe_write((void *)(size_t)(p + 0x5c), pos, 12);
            safe_write((void *)(size_t)(p + 0x68), pos, 12);
            reply(from, "tp ok");
        } else reply(from, "tp no-player");
    } else if (!strncmp(buf, "pause", 5)) {
        g_paused = 1;
    } else if (!strncmp(buf, "resume", 6)) {
        g_paused = 0;
    } else if (!strncmp(buf, "step", 4)) {
        if (sscanf(buf, "step %i", &v) != 1) v = 1;
        g_step = v;
    } else if (sscanf(buf, "detour %x %259s %127s", &a, s1, s2) == 3) {
        char err[200];
        int ok = detour(a, s1, s2, err, sizeof err);
        g_api->log(g_api, "detour %x -> %s!%s: %s %s", a, s1, s2, ok ? "ok" : "FAILED", err);
        reply(from, "detour %x %d %s", a, ok, err);
    } else if (!strncmp(buf, "undetour", 8)) {
        a = 0;
        sscanf(buf, "undetour %x", &a);
        b = (u32)undetour(a);
        reply(from, "undetour %x %u", a, b);
    } else if (!strncmp(buf, "wwatch clear", 12)) {
        memset(g_ww, 0, sizeof g_ww);
        g_nwh = 0; g_ww_lost = 0; g_ww_since = g_tick;
        reply(from, "wwatch clear %d", ww_apply());
    } else if (sscanf(buf, "wwatch %x %i", &a, &v) >= 1) {
        int s, free_s = -1;
        if (sscanf(buf, "wwatch %x %i", &a, &v) < 2) v = 4;
        if (v != 1 && v != 2) v = 4;
        a &= ~(u32)(v - 1);                     /* the CPU needs len-aligned addresses */
        for (s = 0; s < WW_SLOTS; ++s) {
            if (g_ww[s].addr == a) { free_s = s; break; }
            if (!g_ww[s].addr && free_s < 0) free_s = s;
        }
        if (free_s < 0) { reply(from, "wwatch %x %d -1 full", a, v); return; }
        g_ww[free_s].addr = a; g_ww[free_s].len = (u32)v;
        reply(from, "wwatch %x %d %d %s", a, v, free_s, ww_apply() ? "ok" : "failed");
    } else if (!strncmp(buf, "writers", 7)) {
        char head[96], line[96];
        int i, s;
        _snprintf(head, sizeof head, "writers %u %u %u", g_tick, g_tick - g_ww_since, g_ww_lost);
        msg_begin(&g_msg, from, head);
        for (s = 0; s < WW_SLOTS; ++s)
            if (g_ww[s].addr) {
                _snprintf(line, sizeof line, "w %d %x %u", s, g_ww[s].addr, g_ww[s].len);
                msg_add(&g_msg, line);
            }
        for (i = 0; i < (int)g_nwh; ++i) {
            _snprintf(line, sizeof line, "h %u %x %x %x %u", g_wh[i].slot, g_wh[i].eip, g_wh[i].r1, g_wh[i].r2, g_wh[i].n);
            msg_add(&g_msg, line);
        }
        msg_flush(&g_msg);
        if (g_msg.len == (int)strlen(g_msg.head)) send_to(from, g_msg.buf, g_msg.len);
    } else {
        reply(from, "error unknown command");
    }
}

static void poll(void)
{
    char buf[1024];
    struct sockaddr_in from;
    int fromlen, n, k;
    if (g_sock == INVALID_SOCKET) return;
    for (k = 0; k < 64; ++k) {
        fromlen = sizeof from;
        n = recvfrom(g_sock, buf, sizeof buf - 1, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0 && WSAGetLastError() == WSAECONNRESET) continue;  /* a gone peer, not an empty queue */
        if (n <= 0) break;
        buf[n] = 0;
        while (n && (buf[n - 1] == '\n' || buf[n - 1] == '\r')) buf[--n] = 0;
        remember(&from);
        command(buf, &from);
    }
}

/* ---------------------------------------------------------------- frame */
static void pump_while_paused(void)
{
    /* The lab's freeze: the game loop waits inside this callback. Window
       messages are pumped so the window stays responsive; nothing else runs.
       A safety valve lets the game go after ten minutes. */
    DWORD start = GetTickCount();
    MSG m;
    while (g_paused && !g_step && GetTickCount() - start < 600000) {
        poll();
        while (PeekMessageA(&m, 0, 0, 0, PM_REMOVE)) { TranslateMessage(&m); DispatchMessageA(&m); }
        Sleep(4);
    }
    if (g_step) --g_step;
}

static void __cdecl on_frame(void *user)
{
    (void)user;
    ++g_tick;
    collect_actors();
    trace_drain();
    poll();
    send_snaps();
    if ((g_tick % 10) == 0) { send_actors(0); status(0); }
    if (g_paused) {
        if (g_tr_on && g_trace) *g_tr_on = 0;
        status(0);
        pump_while_paused();
        if (g_tr_on && g_trace) { *g_tr_head = 0; *g_tr_on = 2; }
    }
}

static void __cdecl on_level(int level_id, void *user)
{
    (void)user;
    g_level = level_id;
    memset(g_watch, 0, sizeof g_watch);
    g_nfocus = 0;
    collect_actors();
    g_api->log(g_api, "level %d: %d actors, player %x", level_id, g_nactors, player0());
    {
        char b[64];
        int n = _snprintf(b, sizeof b, "level %d", level_id);
        broadcast(b, n);
    }
}

SM64DS_MOD_EXPORT int __cdecl sm64ds_mod_main(const SM64DSModApi *api)
{
    const char *p;
    int port = 47621;
    if (api->api_version < SM64DS_MOD_API_VERSION || !SM64DS_MOD_API_HAS(api, free)) return 1;
    g_api = g_api_for_sym = api;
    g_next = (void *(__cdecl *)(const void *))api->symbol(api, "?Next@dActor_c@@SAPAU1@PBU1@@Z");
    g_player_table = (u32 *)api->symbol(api, "data_0209f394");
    g_player_count = (u32 *)api->symbol(api, "data_0209f21c");
    g_cur_area = (signed char *)api->symbol(api, "data_02092120");
    g_tr_on = (volatile u32 *)api->symbol(api, "ntr_fn_trace_on");
    g_tr_buf = (u32 **)api->symbol(api, "ntr_fn_trace_buf");
    g_tr_head = (volatile u32 *)api->symbol(api, "ntr_fn_trace_head");
    g_tr_cap = (volatile u32 *)api->symbol(api, "ntr_fn_trace_cap");
    if (!g_tr_buf || !g_tr_head || !g_tr_cap) g_tr_on = 0;
    {
        int li;
        for (li = 0; li < (int)N_LOADERS && li < N_LOADER_SLOTS; ++li) {
            u32 a = (u32)(size_t)api->symbol(api, k_loaders[li].name);
            g_loader[li] = a ? a + 5 : 0;
        }
    }
    p = api->setting(api, "port");
    if (p) port = atoi(p);
    net_init(port);
    api->log(api, "naming lab: Next %p, players %p, trace hook %s, UDP 127.0.0.1:%d %s", (void *)g_next,
             (void *)g_player_table, g_tr_on ? "present (tracing build)" : "absent (plain build: no call capture)",
             port, g_sock != INVALID_SOCKET ? "listening" : "NOT available");
    if (!g_next || !g_player_table) {
        api->log(api, "a required symbol is missing in this build; naming lab is off");
        return 2;
    }
    p = api->setting(api, "trace");
    if (p && g_tr_on && strcmp(p, "0") && strcmp(p, "false")) trace_set(1);
    hooks_install();
    api->on_frame(api, on_frame, 0);
    api->on_level_load(api, on_level, 0);
    return 0;
}
