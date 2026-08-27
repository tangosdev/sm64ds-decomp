/* ---- THE EDITOR CONTROL CHANNEL (port mod) ----------------------------------
 *
 * A loopback line-protocol server on 127.0.0.1:7355, wire-compatible with the
 * Studio's editor link. It lets an external editor list the live objects of a
 * running level, move one, ask what level is up, and warp.
 *
 * =============================================================================
 * ws2_32 IS NOT LINKED. IT IS LoadLibrary'd. DO NOT "FIX" THIS.
 * =============================================================================
 *
 * This is not a style choice, it is the rule hal/comms_loopback.cpp wrote down
 * after shipping the bug and measuring it. A plain static import of ws2_32
 * BREAKS THE WHOLE PORT, on every path, with no socket ever created.
 *
 * The port survives by claiming the DS's FIXED addresses -- 0x02000000 main RAM
 * and 0x027ff000 -- from a TLS callback at process start, before the CRT,
 * because anything that maps low first wins and the game has no second choice
 * of address. The Windows loader resolves static imports BEFORE it runs TLS
 * callbacks. So the moment ws2_32 becomes a static import, it and its whole
 * dependency chain are mapped before the port can ask, and something in that
 * chain lands across 0x02000000.
 *
 * Already tried, already failed, do not retry: `#pragma comment(lib,
 * "ws2_32.lib")` is what broke it, and adding `/DELAYLOAD:ws2_32.dll` beside it
 * DID NOT FIX IT -- dumpbin still listed WS2_32.dll as a normal import and the
 * selftest still failed with LOST 02000000.
 *
 * <winsock2.h> below is included for the TYPES and CONSTANTS only -- SOCKET,
 * sockaddr_in, AF_INET, FIONBIO. None of its function declarations are used;
 * every call goes through the WS table. htons/htonl live in ws2_32 too, so the
 * byte swaps are hand-rolled, and FD_ISSET is avoided (it calls the imported
 * __WSAFDIsSet) by not using select at all: both sockets are non-blocking and
 * the socket thread polls with a short Sleep.
 *
 * =============================================================================
 * THE ENUMERATION IS THE GAME'S OWN, AND THAT IS THE POINT
 * =============================================================================
 *
 * There is no host-side registry of live actors to walk. hal/actor_registry.cpp
 * has port_actor_census(), which looks like one and is not: g_spawned[] and
 * g_skipped[] are CUMULATIVE SPAWN COUNTERS bumped at spawn time, so they say
 * how many of each class were ever created, never which objects are alive now.
 * Reporting off them would list a Goomba that died ten minutes ago.
 *
 * The live list is the ROM's, reached the way the ROM reaches it:
 *
 *     Actor *a = Actor::Next(nullptr);          // First
 *     while (a) a = Actor::Next(a);
 *
 * Actor::Next is src/_ZN5Actor4NextEPKS_.cpp -- a BYTE-MATCHED decompiled TU
 * that is ALREADY LINKED into walk_window. Verified, not assumed: walk_window
 * .map resolves __ZN5Actor4NextEPKS_ to _ZN5Actor4NextEPKS_.cpp.obj, the
 * matched object, not to a host stand-in. So this file adds no object to the
 * link and invents no traversal -- the walk an editor sees is the same one the
 * game's own Behavior loops see. Nine ROM TUs in src/ use exactly this idiom
 * (func_ov002_020b10e4, BrickBlock::Behavior, func_ov002_020f051c, ...).
 *
 * It walks data_0209b468, the actor list every Actor::Actor links itself into
 * (the node is embedded at Actor+0x50, with owner at node+8). The port's other
 * probes tend to walk data_020a4b78, the BEHAVIOUR list, which is a subset --
 * it holds what is being ticked. The editor wants everything that exists, and
 * it wants the same set the recomp's channel produced, so the ROM's own list is
 * the right one.
 *
 * WHY IT IS SAFE AT THE FRAME BOUNDARY. The list is mutated by spawns and kills
 * inside the game's own update phases. editor_channel_drain() is called from
 * walk_window's frame loop after that frame's tick AND its present are done, so
 * no spawn or kill is half-applied while we walk and no pointer we hand out is
 * one the game is mid-way through unlinking. It is also the only place this
 * file touches game state at all: the socket thread is forbidden from it.
 *
 * FIELD OFFSETS, NOT THE C++ HEADER. actorID and the position are read at +0xc
 * and +0x5c off the real Actor object. include/Actor.h is the authority for
 * both -- `u16 actorID;` at 0x00c (inherited from ActorBase), `s32 mPosX/mPosY/
 * mPosZ;` at 0x05c, with a static assert pinning sizeof(Actor) == 0xd0 -- and
 * raw-offset reads are the established convention in port/hal, where several
 * TUs deliberately declare their own local struct Actor rather than include the
 * real header. walk_window.cpp reads these very fields this very way in its
 * ambush census (`*(int *)(o + 0x5c)`). Four scalar reads is not worth dragging
 * the class hierarchy, its vtable and the MSVC/Itanium ABI question into a new
 * TU.
 *
 * NOTE: Actor+0x50 and +0x54 are the list node (unk_050/unk_054 in the header),
 * NOT spare words. This file never writes them.
 *
 * =============================================================================
 * THREADING, AND WHY A HUNG CLIENT CANNOT STALL THE GAME
 * =============================================================================
 *
 * The socket thread parses lines and enqueues; it never reads or writes game
 * state. The game thread executes the queue at the frame boundary and produces
 * the replies. A client that hangs, floods or dies mid-request therefore costs
 * the game nothing: both sockets are non-blocking, sends are attempted from the
 * frame boundary and come straight back WSAEWOULDBLOCK if the peer has stopped
 * reading, and the command queue, the outbound buffer and the per-frame work
 * are all capped.
 *
 * OPT-IN. Nothing runs unless SM64DS_EDITOR_CHANNEL=1. The env test is the
 * first thing editor_channel_init() does -- before the thread, before
 * LoadLibrary, before any socket exists -- so the shipped game does not grow a
 * listener merely by containing this file.
 *
 * NO peek32/poke32. The recomp's channel had them because it drove an emulator
 * with a guest address space. This port is NATIVE: there is no guest address
 * space, a DS address is not a host address, and honouring a poke would mean
 * writing to an arbitrary host pointer on request. Both answer `err
 * unsupported` on purpose.
 */

#include "editor_channel.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>       /* TYPES AND CONSTANTS ONLY -- see the banner */
#include <ws2tcpip.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/* ---- what this file borrows from the game ---------------------------------
 *
 * Actor::Next is the matched TU. The level globals are the ROM's own, spelled
 * by address the way every hosted global in port/hal is, and declared here
 * exactly as hal/level_change.cpp and hal/actor_registry.cpp declare them.
 *
 * LoadLevelNoReturn is src/LoadLevelNoReturn.c, the ROM's own level-change
 * entry. Calling it STAGES a change -- it writes data_02092110 (next level) and
 * data_0209f268 (next entrance) -- and walk_window's frame loop then applies it
 * through port_level_change_pending() / port_level_change_poll().
 *
 * That is precisely how the debug menu's MENU_LEVEL row gets an IN-PROCESS
 * handoff. Its enter branch calls port_title_select(), whose operative line is
 * this same LoadLevelNoReturn call, then closes the menu and lets the frame
 * loop's poll do the rest. We deliberately do NOT copy the rest of
 * port_title_select (SetPlayerGlobals / SetNumPlayers / StartSceneFade / the
 * white fade): that is the title screen handing off to the star select. An
 * editor warp is the warp-pipe shape, which the frame loop applies with no
 * scene fade. We also do NOT call port_level_change_poll() ourselves -- the
 * loop's own call at the poll site re-seats player/camera afterwards, and
 * driving the handoff from here would leave those stale.
 */
extern "C" {
struct Actor;
struct Actor *_ZN5Actor4NextEPKS_(struct Actor *prev);
void LoadLevelNoReturn(int level, unsigned entrance, unsigned star,
                       unsigned reason);
int port_level_is_mounted(int level);
int port_actor_live_count(void);        /* hal/level_change.cpp, cross-check */
extern signed char data_0209f2f8;       /* current level */
extern signed char data_02092110;       /* staged next level, -1 = none */
extern unsigned char data_02092128[];   /* per-player character; [0] is active */
extern void *data_0209f394[];           /* per-player Actor* */
}

/* include/Actor.h: u16 actorID at 0x00c, s32 mPosX/mPosY/mPosZ at 0x05c.
   include/Player.h: the live character is Player+0x6d9, low 2 bits. */
static const unsigned OFF_ACTOR_ID = 0x0C;
static const unsigned OFF_ACTOR_POS = 0x5C;
static const unsigned OFF_PLAYER_CHARACTER = 0x6D9;

namespace {

const unsigned short EDITOR_PORT = 7355;

/* A line longer than this is not a command, it is noise. Cap it, drop it, keep
   reading -- the fuzz arm of this lane's proof blasts random bytes with no
   newline in them at all, and the buffer must not grow to meet them. */
const size_t MAX_LINE = 512;
/* Bounded per-frame work: a client that pipelines a thousand objlists must not
   turn one frame into a thousand list walks. */
const size_t MAX_CMDS_PER_FRAME = 16;
/* A flooding client must not grow memory without bound either. */
const size_t MAX_QUEUE = 256;
/* Nor may a peer that stopped reading: past this the client is dropped. */
const size_t MAX_OUTBUF = 1u << 20;
/* And one frame writes at most this much, so a huge objlist cannot monopolise
   the frame boundary on a slow socket. */
const size_t MAX_SEND_PER_FRAME = 64u * 1024u;
/* The socket thread's idle poll. It is its own thread, so sleeping here costs
   the game nothing; 10 ms is well under one frame of added latency. */
const DWORD POLL_SLEEP_MS = 10;

/* ---- ws2_32, by hand ------------------------------------------------------*/

struct WsTable {
    int    (WSAAPI *WSAStartup)(WORD, LPWSADATA);
    SOCKET (WSAAPI *socket)(int, int, int);
    int    (WSAAPI *ioctlsocket)(SOCKET, long, u_long *);
    int    (WSAAPI *bind)(SOCKET, const struct sockaddr *, int);
    int    (WSAAPI *listen)(SOCKET, int);
    SOCKET (WSAAPI *accept)(SOCKET, struct sockaddr *, int *);
    int    (WSAAPI *recv)(SOCKET, char *, int, int);
    int    (WSAAPI *send)(SOCKET, const char *, int, int);
    int    (WSAAPI *setsockopt)(SOCKET, int, int, const char *, int);
    int    (WSAAPI *closesocket)(SOCKET);
    int    (WSAAPI *WSAGetLastError)(void);
};
WsTable WS;
int g_ws_loaded = 0;            /* 0 not tried, 1 loaded, -1 refused */

bool ws_load()
{
    if (g_ws_loaded) return g_ws_loaded > 0;
    HMODULE m = ::LoadLibraryA("ws2_32.dll");
    if (!m) {
        g_ws_loaded = -1;
        std::fprintf(stderr, "[editor] ws2_32.dll would not load; channel "
                             "stays off\n");
        return false;
    }
#define WS_GET(n) \
    *(FARPROC *)&WS.n = ::GetProcAddress(m, #n); \
    if (!WS.n) g_ws_loaded = -1;
    WS_GET(WSAStartup);  WS_GET(socket);   WS_GET(ioctlsocket);
    WS_GET(bind);        WS_GET(listen);   WS_GET(accept);
    WS_GET(recv);        WS_GET(send);     WS_GET(setsockopt);
    WS_GET(closesocket); WS_GET(WSAGetLastError);
#undef WS_GET
    if (g_ws_loaded < 0) {
        std::fprintf(stderr, "[editor] ws2_32.dll is missing an entry point; "
                             "channel stays off\n");
        return false;
    }
    g_ws_loaded = 1;
    return true;
}

/* htons/htonl are ws2_32 imports too; hand-rolled the way comms_loopback does. */
inline unsigned short hton16(unsigned short v)
{
    return (unsigned short)((v >> 8) | (v << 8));
}
inline unsigned long hton32(unsigned long v)
{
    return ((v & 0xFFul) << 24) | ((v & 0xFF00ul) << 8) |
           ((v & 0xFF0000ul) >> 8) | ((v & 0xFF000000ul) >> 24);
}

/* ---- shared state ---------------------------------------------------------*/

struct Cmd {
    enum Kind { PING, INFO, OBJLIST, OBJMOVE, WARP } kind;
    unsigned objptr;            /* OBJMOVE */
    int x, y, z;                /* OBJMOVE, Fix12 */
    int level, entrance;        /* WARP */
};

std::mutex g_mtx;               /* guards everything below */
std::deque<Cmd> g_queue;
std::string g_out;              /* pending reply bytes, flushed at the boundary */
SOCKET g_client = INVALID_SOCKET;
bool g_drop = false;            /* game thread asks, socket thread closes */

bool g_armed = false;
bool g_started = false;

/* Call with g_mtx held. */
void push_reply(const char *s)
{
    if (g_out.size() + std::strlen(s) > MAX_OUTBUF) {
        g_drop = true;          /* peer has stopped reading; let it go */
        return;
    }
    g_out += s;
}

/* ---- the live actor walk --------------------------------------------------
 *
 * The guard is not decoration. If a corrupted list ever pointed at itself this
 * would spin inside the frame boundary and hang the game -- precisely the
 * failure a debug channel must not be able to cause. 8192 matches the bound the
 * reference channel and the port's own list probes use.
 */
void walk_actors(std::vector<struct Actor *> &out)
{
    struct Actor *a = _ZN5Actor4NextEPKS_(0);
    for (int guard = 0; a && guard < 8192; ++guard) {
        out.push_back(a);
        a = _ZN5Actor4NextEPKS_(a);
    }
}

unsigned actor_id_of(struct Actor *a)
{
    return *(const unsigned short *)((const char *)a + OFF_ACTOR_ID);
}

const int *actor_pos_of(struct Actor *a)
{
    return (const int *)((const char *)a + OFF_ACTOR_POS);
}

/* ---- command execution, at the frame boundary -----------------------------*/

void exec_objlist()
{
    std::vector<struct Actor *> live;
    walk_actors(live);
    char ln[96];
    for (size_t i = 0; i < live.size(); ++i) {
        const int *p = actor_pos_of(live[i]);
        /* `obj <ptr> <actorID> <x> <y> <z>`: ptr HEX (the Studio parses it with
           from_str_radix(.., 16)), actorID DECIMAL, and the three positions as
           the raw Fix12 integers -- the Studio divides by 4096 itself. Pinned
           against app/src/viewer_app.rs parse_obj_line and its own unit test
           ("obj 02188A38 191 -25497600 0 26005504"), not guessed. */
        std::snprintf(ln, sizeof ln, "obj %08X %u %d %d %d\n",
                      (unsigned)(uintptr_t)live[i], actor_id_of(live[i]),
                      p[0], p[1], p[2]);
        push_reply(ln);
    }
    push_reply("end\n");
}

void exec_objmove(const Cmd &c)
{
    /* VALIDATE THE POINTER AGAINST THE LIVE LIST FIRST.
       The client's pointer came from an objlist that may be many frames old,
       and the actor it named may have been killed and its memory reused since.
       Writing three words through a stale pointer would corrupt whatever now
       occupies that address -- a game crash caused by an editor holding a
       slightly old list, which is the normal case and not an exotic one. So a
       move is applied only to an object still in the list THIS frame. (The
       recomp's channel wrote unconditionally; it could afford to be wrong about
       an emulator's RAM in a way this cannot be about the host's heap.) */
    std::vector<struct Actor *> live;
    walk_actors(live);
    for (size_t i = 0; i < live.size(); ++i) {
        if ((unsigned)(uintptr_t)live[i] != c.objptr)
            continue;
        int *p = (int *)((char *)live[i] + OFF_ACTOR_POS);
        p[0] = c.x;
        p[1] = c.y;
        p[2] = c.z;
        push_reply("ok\n");
        return;
    }
    push_reply("err no such object\n");
}

void exec_warp(const Cmd &c)
{
    if (c.level < 0 || c.level > 51) {
        push_reply("err level 0..51\n");
        return;
    }
    /* Level 0 is the Test Map AND the "not in gameplay" sentinel the ROM's own
       globals use, so a current level of 0 means menus or boot, not a level.
       The reference channel refuses to inject a warp there for a concrete
       reason: warping out of a menu wedges the scene state machine in a fade.
       Refuse and say so rather than stage something that will never apply. */
    if (data_0209f2f8 <= 0 || !data_0209f394[0]) {
        push_reply("err not in a level\n");
        return;
    }
    if (!port_level_is_mounted(c.level)) {
        push_reply("err level not mounted in this build\n");
        return;
    }
    if (data_02092110 >= 0) {
        push_reply("err a level change is already pending\n");
        return;
    }
    /* Stage it. star = 1 is what the debug menu's row passes (the ROM's own
       confirm branch); the frame loop's port_level_change_poll() applies the
       staged request on a later frame and re-seats everything itself. */
    LoadLevelNoReturn(c.level, (unsigned)c.entrance, 1u, 0u);
    push_reply("ok\n");
}

void exec(const Cmd &c)
{
    switch (c.kind) {
    case Cmd::PING:
        /* Answered HERE, on the game thread, not on the socket thread. That is
           deliberate: a pong then proves the FRAME LOOP is still turning, which
           is exactly what a liveness check after a fuzz blast has to establish.
           A socket-thread pong would prove only that the socket thread is
           alive, which is not the interesting question. */
        push_reply("pong\n");
        return;
    case Cmd::INFO: {
        char buf[64];
        /* The live character is read off the Player the spawn actually produced
           rather than off the save byte -- walk_window.cpp does the same and
           says why: zeroed storage reads as 0, which IS Mario, so agreement
           there proves nothing. Falls back to the save-side array when there is
           no Player. `level <N> char <M>` is the shape the Studio parses (it
           strips "level " and reads the first field as i32). */
        const char *p = (const char *)data_0209f394[0];
        int chr = p ? (*(const unsigned char *)(p + OFF_PLAYER_CHARACTER) & 3)
                    : (data_02092128[0] & 3);
        std::snprintf(buf, sizeof buf, "level %d char %d\n",
                      (int)data_0209f2f8, chr);
        push_reply(buf);
        return;
    }
    case Cmd::OBJLIST:  exec_objlist();   return;
    case Cmd::OBJMOVE:  exec_objmove(c);  return;
    case Cmd::WARP:     exec_warp(c);     return;
    }
}

/* ---- line parsing, on the socket thread -----------------------------------
 *
 * Nothing here touches game state. Verbs that need it are enqueued; the ones
 * that do not (an unknown verb, a malformed argument list, and the two
 * deliberately-unsupported memory pokes) are answered straight into the
 * outbound buffer.
 *
 * Hand-rolled tokenising rather than sscanf, because the input is hostile by
 * assumption: a fuzz blast puts NUL bytes mid-line, and any NUL-terminated read
 * would silently see a shorter string than actually arrived.
 */
bool next_tok(const std::string &s, size_t &i, std::string &out)
{
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r')) ++i;
    if (i >= s.size()) return false;
    size_t b = i;
    while (i < s.size() && s[i] != ' ' && s[i] != '\t' && s[i] != '\r') ++i;
    out.assign(s, b, i - b);
    return true;
}

bool parse_int(const std::string &t, int &v)
{
    if (t.empty() || t.size() > 20) return false;
    if (t.find('\0') != std::string::npos) return false;
    errno = 0;
    char *end = 0;
    long r = std::strtol(t.c_str(), &end, 10);
    if (errno || !end || *end) return false;
    v = (int)r;
    return true;
}

bool parse_u32hex(const std::string &t, unsigned &v)
{
    if (t.empty() || t.size() > 16) return false;
    if (t.find('\0') != std::string::npos) return false;
    errno = 0;
    char *end = 0;
    unsigned long r = std::strtoul(t.c_str(), &end, 16);
    if (errno || !end || *end) return false;
    v = (unsigned)r;
    return true;
}

/* Call with g_mtx held. */
void enqueue(const Cmd &c)
{
    if (g_queue.size() >= MAX_QUEUE) return;   /* flooding: drop, never grow */
    g_queue.push_back(c);
}

void handle_line(const std::string &line)
{
    size_t i = 0;
    std::string verb;
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!next_tok(line, i, verb)) return;      /* blank line: ignore silently */

    Cmd c;
    std::memset(&c, 0, sizeof c);

    if (verb == "ping")    { c.kind = Cmd::PING;    enqueue(c); return; }
    if (verb == "info")    { c.kind = Cmd::INFO;    enqueue(c); return; }
    if (verb == "objlist") { c.kind = Cmd::OBJLIST; enqueue(c); return; }

    if (verb == "objmove") {
        std::string a, b, d, e;
        unsigned ptr = 0;
        int x = 0, y = 0, z = 0;
        if (!next_tok(line, i, a) || !next_tok(line, i, b) ||
            !next_tok(line, i, d) || !next_tok(line, i, e) ||
            !parse_u32hex(a, ptr) || !parse_int(b, x) ||
            !parse_int(d, y) || !parse_int(e, z)) {
            push_reply("err objmove <ptr> <x> <y> <z>\n");
            return;
        }
        c.kind = Cmd::OBJMOVE;
        c.objptr = ptr; c.x = x; c.y = y; c.z = z;
        enqueue(c);
        return;
    }

    if (verb == "warp") {
        std::string a, b;
        int level = 0, entrance = 0;
        if (!next_tok(line, i, a) || !parse_int(a, level)) {
            push_reply("err warp <level> [entrance]\n");
            return;
        }
        if (next_tok(line, i, b) && !parse_int(b, entrance)) {
            push_reply("err warp <level> [entrance]\n");
            return;
        }
        c.kind = Cmd::WARP;
        c.level = level; c.entrance = entrance;
        enqueue(c);
        return;
    }

    /* The port is native: there is no guest address space to peek or poke, and
       a DS address is not a host address. Named explicitly so a client written
       against the recomp's channel gets a real answer instead of "unknown". */
    if (verb == "peek32" || verb == "poke32") {
        push_reply("err unsupported\n");
        return;
    }

    push_reply("err unknown\n");
}

/* ---- the socket thread ----------------------------------------------------*/

void set_nonblocking(SOCKET s)
{
    u_long one = 1;
    WS.ioctlsocket(s, FIONBIO, &one);
}

void serve_client(SOCKET c)
{
    set_nonblocking(c);
    {
        std::lock_guard<std::mutex> lk(g_mtx);
        g_client = c;
        g_drop = false;
        g_out.clear();
        g_queue.clear();
    }

    std::string buf;
    char rd[1024];
    for (;;) {
        {
            std::lock_guard<std::mutex> lk(g_mtx);
            if (g_drop) break;
        }
        int n = WS.recv(c, rd, (int)sizeof rd, 0);
        if (n == 0) break;                      /* peer closed */
        if (n < 0) {
            if (WS.WSAGetLastError() == WSAEWOULDBLOCK) {
                ::Sleep(POLL_SLEEP_MS);
                continue;
            }
            break;
        }
        buf.append(rd, (size_t)n);

        /* Split on '\n' BY LENGTH, not with strchr: a fuzz blast contains NUL
           bytes and strchr would stop at the first one, losing everything after
           it in the same read. */
        for (;;) {
            size_t nl = buf.find('\n');
            if (nl == std::string::npos) {
                /* No newline yet. If what we hold already exceeds a plausible
                   command it is noise -- drop it and resynchronise rather than
                   buffer it forever. */
                if (buf.size() > MAX_LINE) buf.clear();
                break;
            }
            std::string line = buf.substr(0, nl);
            buf.erase(0, nl + 1);
            if (line.size() <= MAX_LINE)
                handle_line(line);
            /* an over-long line is dropped: no reply, no crash, keep going */
        }
    }

    {
        std::lock_guard<std::mutex> lk(g_mtx);
        g_client = INVALID_SOCKET;
        g_out.clear();
        g_queue.clear();
        g_drop = false;
    }
    WS.closesocket(c);
}

void server_thread()
{
    if (!ws_load()) return;

    WSADATA wsa;
    /* WSAStartup is refcounted per process, so a second call is correct and
       harmless if hal/comms_loopback.cpp already started Winsock. Never call
       WSACleanup -- it would tear Winsock down under any other user of it, and
       process exit does the job. (comms_loopback.cpp's rule, same reason.) */
    if (WS.WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::fprintf(stderr, "[editor] WSAStartup failed; channel stays off\n");
        return;
    }
    SOCKET srv = WS.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srv == INVALID_SOCKET) {
        std::fprintf(stderr, "[editor] socket() failed; channel stays off\n");
        return;
    }
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = hton16(EDITOR_PORT);
    /* LOOPBACK ONLY. Not INADDR_ANY: this channel hands out host pointers and
       accepts writes through them, with no authentication of any kind. It must
       not be reachable from off this machine. */
    addr.sin_addr.s_addr = hton32(INADDR_LOOPBACK);
    if (WS.bind(srv, (sockaddr *)&addr, sizeof addr) != 0 ||
        WS.listen(srv, 1) != 0) {
        std::fprintf(stderr, "[editor] 127.0.0.1:%u unavailable; channel stays "
                             "off\n", (unsigned)EDITOR_PORT);
        WS.closesocket(srv);
        return;
    }
    set_nonblocking(srv);
    std::fprintf(stderr, "[editor] listening on 127.0.0.1:%u\n",
                 (unsigned)EDITOR_PORT);
    std::fflush(stderr);

    for (;;) {
        SOCKET c = WS.accept(srv, 0, 0);
        if (c == INVALID_SOCKET) {
            ::Sleep(POLL_SLEEP_MS);
            continue;
        }
        /* One client at a time, served on this thread. A second connection
           waits in the listen backlog until the first goes away. */
        serve_client(c);
    }
}

}  /* namespace */

/* ---- the two entry points -------------------------------------------------*/

void editor_channel_init(void)
{
    if (g_started) return;
    g_started = true;
    const char *e = std::getenv("SM64DS_EDITOR_CHANNEL");
    /* atoi, so SM64DS_EDITOR_CHANNEL=0 is a real opt-out rather than an
       accidental opt-in -- the shape walk_window.cpp settled on for
       SM64DS_NO_FOCUS, and for the same reason. */
    g_armed = e && std::atoi(e) != 0;
    if (!g_armed) return;    /* no thread, no LoadLibrary, no socket, no listen */
    std::thread(server_thread).detach();
}

void editor_channel_drain(void)
{
    if (!g_armed) return;

    SOCKET c;
    std::string out;
    {
        std::lock_guard<std::mutex> lk(g_mtx);
        c = g_client;
        if (c == INVALID_SOCKET) {
            g_queue.clear();    /* nobody to answer */
            g_out.clear();
            return;
        }
        for (size_t n = 0; n < MAX_CMDS_PER_FRAME && !g_queue.empty(); ++n) {
            Cmd cmd = g_queue.front();
            g_queue.pop_front();
            exec(cmd);          /* appends to g_out */
        }
        if (g_out.empty()) return;
        size_t take = g_out.size() < MAX_SEND_PER_FRAME ? g_out.size()
                                                        : MAX_SEND_PER_FRAME;
        out.assign(g_out, 0, take);
    }

    /* Non-blocking send, OUTSIDE the lock. A peer that has stopped reading
       returns WSAEWOULDBLOCK immediately; the unsent tail stays buffered for
       the next frame. Nothing here can block the frame loop. */
    int sent = WS.send(c, out.data(), (int)out.size(), 0);
    std::lock_guard<std::mutex> lk(g_mtx);
    if (sent > 0)
        g_out.erase(0, (size_t)sent);
    else if (sent < 0 && WS.WSAGetLastError() != WSAEWOULDBLOCK)
        g_drop = true;          /* real error: the socket thread will close it */
}

#else  /* !_WIN32 */

void editor_channel_init(void) {}
void editor_channel_drain(void) {}

#endif
