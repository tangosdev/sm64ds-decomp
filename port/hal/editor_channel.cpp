/* ---- THE EDITOR CONTROL CHANNEL (port mod) ----------------------------------
 *
 * A loopback line-protocol server on 127.0.0.1:7355, wire-compatible with the
 * Studio's editor link. It lets an external editor list the live objects of a
 * running level, move one, turn one, replace one where it stands, create one,
 * destroy one, ask what level is up, and warp.
 *
 * THE WIRE, in full, because the Studio builds against it and a protocol that
 * lives only in the parser is a protocol nobody can implement:
 *
 *   ping                                  -> pong
 *   info                                  -> level <N> char <M>
 *   objlist                               -> obj <PTR> <id> <x> <y> <z> ... end
 *   objmove <ptr> <x> <y> <z>             -> ok
 *   objrot  <ptr> <rx> <ry> <rz>          -> ok
 *   objrespawn <ptr> <x> <y> <z> [ry]     -> respawned <PTR>
 *   objspawn <actorid> <x> <y> <z> <ry> <param>
 *                                         -> spawned <PTR>
 *   objkill <ptr>                         -> ok
 *   warp <level> [entrance]               -> ok
 *   peek32 / poke32                       -> err unsupported
 *   anything else                         -> err unknown
 *
 * POINTERS ARE HEX AND EVERYTHING ELSE IS DECIMAL, with one documented
 * exception: objspawn's <param> is base 0, so 0x1f00 and 7936 both work (see
 * parse_u32any). Positions are raw Fix12 integers -- the Studio divides by 4096
 * itself -- and rotations are 16-bit angle units, 65536 to the full turn.
 * Every reply is a single line except objlist's block, which ends with `end`.
 * Every failure is `err ` followed by plain words.
 *
 * =============================================================================
 * WHY objrespawn EXISTS WHEN objmove ALREADY MOVES THINGS
 * =============================================================================
 *
 * Because for a great deal of a level, objmove does not move anything you can
 * see. A moving actor -- a Goomba, a platform on a path -- reads its position
 * out of Actor+0x5c every frame, so writing that word moves it and the picture
 * follows. FURNITURE DOES NOT. A sign, a brick, a tree: the class bakes its
 * world transform once, at InitResources, out of the position it was spawned
 * at, and afterwards nothing reads +0x5c again. objmove on one of those writes
 * three words that are true and inert -- objlist reports the new position, the
 * object stays exactly where it was drawn, and the editor looks broken.
 *
 * objrespawn is the answer that does not require knowing which classes are
 * which: DESTROY the actor and let the game BUILD A NEW ONE at the new place,
 * through the game's own spawn path, so whatever InitResources bakes gets baked
 * from the new position. The furniture case is the whole reason the verb is
 * here, and the proof for it is the pair of arms that shows a BMP moving under
 * objrespawn where the same object under objmove leaves the BMP untouched.
 *
 * The cost is honest and is stated in the reply: the object is NOT the same
 * object afterwards. It has a new heap address and a new uniqueID, so the verb
 * answers `respawned <newptr>` and the editor rebinds. Anything the old
 * instance had accumulated at runtime -- animation phase, state machine
 * progress, whatever it was carrying -- is gone. That is a rebirth, not a move,
 * and calling it anything softer would be a lie an editor would then have to
 * debug.
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
/* The ROM's own "kill this actor", src/_ZN9ActorBase18MarkForDestructionEv.cpp,
   declared for host callers exactly as hal/level_change.cpp:127 declares it.
   Idempotent and self-guarding; it marks +0x0f and runs OnPendingDestroy, and
   the game's own cleanup phase frees the object on a later frame. Nothing here
   frees an actor by hand -- that is hal/level_change.cpp's stated contract and
   this file does not get an exception to it. */
void _ZN9ActorBase18MarkForDestructionEv(void *self);
/* hal/level_boot.cpp:3776. Runs the ROM's Actor::Spawn with an explicit
   position, yaw and area, allocates the death-table sequence the ROM allocates,
   and refuses (returning 0, in its own words on stdout) when the class belongs
   to an overlay this level never booted. Reused rather than re-derived: a
   second spawn path would be a second thing to keep true. */
void *port_debug_spawn_at(unsigned id, unsigned param, int x, int y, int z,
                          int yaw, int area);
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

/* ROTATION. include/Actor.h:91-93 is the authority, and it is the same kind of
   authority the position read above already leans on:

       s16 mAngleX;            / * 0x08c * /
       s16 mAngleY;            / * 0x08e * /
       s16 mAngleZ;            / * 0x090 * /

   SIXTEEN-BIT ANGLE UNITS, not degrees and not radians: the whole turn is
   65536, so 0x4000 is a quarter turn. That is the game's own convention and
   not a choice made here -- include/Actor.h:185 cites `ldrsh [r4,#0x8e]`, a
   SIGNED halfword load of mAngleY, in the matched player-proximity code, and
   port/hal/level_boot.cpp:3797 builds a spawn rotation as
   `rot.y = (short)yaw` off the same units. Reading the field signed is
   therefore right and the value wraps rather than saturating.

   WHY THE THREE *PREVIOUS* ANGLES AT 0x092/0x094/0x096 ARE NOT WRITTEN. They
   are separate fields (include/Actor.h:94-96), and the parallel with objmove
   is exact: exec_objmove writes mPosX/Y/Z and deliberately leaves unk_068,
   the previous position, alone. A verb that also rewrote history would hide
   the very frame-to-frame delta some behaviours compute. Same rule here. */
static const unsigned OFF_ACTOR_ANGLE = 0x8C;

/* SPAWN IDENTITY, for objrespawn. include/ActorBase.h:57 `u32 param1;` at
   0x08 -- and that name is not a guess by this file:
   src/_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii.c declares its own second
   parameter `param1` and hands it straight to
   `ActorDerived::Spawn(actorID, data_0209f5c0, param1, 2)`. So the word this
   file reads back at +0x08 is the same word a spawn was given. */
static const unsigned OFF_ACTOR_PARAM = 0x08;

/* The area the actor was spawned into. include/Actor.h:110 `s8 mAreaId;` at
   0x0cc, and src/_ZN5ActorC1Ev.cpp:49 is literally
   `self->mAreaId = data_0209b44c;` -- the global src/func_02010e78.c stages
   from Actor::Spawn's areaID argument. SIGNED, because the header says a
   negative value means "not area-bound" and passing 0xff back as 255 would
   turn that into a real area number. See exec_objrespawn for why the two
   nearby candidates (+0x10, +0x12) are both wrong. */
static const unsigned OFF_ACTOR_AREA = 0xCC;

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
    enum Kind { PING, INFO, OBJLIST, OBJMOVE, OBJROT, OBJRESPAWN, OBJSPAWN,
                OBJKILL, WARP, ERR } kind;
    unsigned objptr;            /* OBJMOVE, OBJROT, OBJRESPAWN, OBJKILL */
    int x, y, z;                /* OBJMOVE, OBJRESPAWN, OBJSPAWN, Fix12 */
    int rx, ry, rz;             /* OBJROT, and OBJRESPAWN/OBJSPAWN's ry */
    int has_rot;                /* OBJRESPAWN: was a yaw given at all */
    unsigned spawn_id;          /* OBJSPAWN: the actor id */
    unsigned spawn_param;       /* OBJSPAWN: ActorBase+0x08's word */
    int level, entrance;        /* WARP */
    const char *msg;            /* ERR: always a string literal (static
                                   lifetime), so the queue never owns storage */
};

/* The largest actor id this channel will pass to the spawn path. 512 is not a
   guess: hal/actor_registry.cpp:145 sizes its per-id counters
   `enum { PORT_ACTOR_IDS = 512 }`, and the LEVEL DATA agrees from the other
   side -- a simple sub-table record packs the actor-table index into nine bits
   (`raw & 0x1ff`, src/_Z17LoadSimpleObjectsRN11LVL_Overlay11ObjSubTableEij and
   hal/stage_mods.cpp both say so). An id past that is not a class the game can
   name, and handing it to Actor::Spawn would index the spawn-info table out of
   bounds inside matched ROM code, which is a crash this channel must not be
   able to cause on a typo. */
const unsigned MAX_ACTOR_ID = 511;

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

/* VALIDATE THE POINTER AGAINST THE LIVE LIST FIRST -- every verb that writes
   through a client-supplied pointer starts here, and there is exactly one copy
   of the rule so there is exactly one place for it to be right.

   The client's pointer came from an objlist that may be many frames old, and
   the actor it named may have been killed and its memory reused since. Writing
   through a stale pointer would corrupt whatever now occupies that address -- a
   game crash caused by an editor holding a slightly old list, which is the
   normal case and not an exotic one. So a write is applied only to an object
   still in the list THIS frame. (The recomp's channel wrote unconditionally; it
   could afford to be wrong about an emulator's RAM in a way this cannot be
   about the host's heap.)

   Returns 0 when the pointer is stale; every caller answers `err no such
   object`, which is the reply objmove has always given and the one objrot and
   objrespawn give too. */
struct Actor *find_live(unsigned ptr)
{
    std::vector<struct Actor *> live;
    walk_actors(live);
    for (size_t i = 0; i < live.size(); ++i)
        if ((unsigned)(uintptr_t)live[i] == ptr)
            return live[i];
    return 0;
}

void exec_objmove(const Cmd &c)
{
    struct Actor *a = find_live(c.objptr);
    if (!a) {
        push_reply("err no such object\n");
        return;
    }
    int *p = (int *)((char *)a + OFF_ACTOR_POS);
    p[0] = c.x;
    p[1] = c.y;
    p[2] = c.z;
    push_reply("ok\n");
}

void exec_objrot(const Cmd &c)
{
    struct Actor *a = find_live(c.objptr);
    if (!a) {
        push_reply("err no such object\n");
        return;
    }
    /* Three signed halfwords, exactly as the header lays them out. The casts
       truncate to 16 bits, which is the correct behaviour for an angle and not
       a lost error: the unit wraps at a full turn, so 0x14000 and 0x4000 name
       the same facing and both mean a quarter turn. */
    short *r = (short *)((char *)a + OFF_ACTOR_ANGLE);
    r[0] = (short)c.rx;
    r[1] = (short)c.ry;
    r[2] = (short)c.rz;
    push_reply("ok\n");
}

/* ---- objrespawn ------------------------------------------------------------
 *
 * Kill the actor at `ptr` and let the game build a fresh one of the same class,
 * with the same spawn param, at the new position. Everything below is read off
 * the LIVE object, so the identity is recovered rather than remembered.
 *
 * WHERE EACH FIELD COMES FROM, traced rather than assumed:
 *
 *   actorID  ActorBase+0x0c. Already what exec_objlist reports.
 *
 *   param    ActorBase+0x08. src/func_02043180.c stores Spawn's `param1`
 *            (r2) to data_020a4b60, and src/_ZN9ActorBaseC1Ev.cpp loads that
 *            global and does `str r2, [r4, #8]`. Copied 32 bits wide with no
 *            mask, shift or merge anywhere on that path -- see OFF_ACTOR_PARAM.
 *            THE TWO EXCEPTIONS ARE HANDLED BELOW; they are real.
 *
 *   area     Actor+0xcc. src/_ZN5ActorC1Ev.cpp:49 is
 *            `self->mAreaId = data_0209b44c;`, and data_0209b44c is exactly
 *            what src/func_02010e78.c staged from Spawn's areaID argument.
 *            include/Actor.h:110 types it `s8 mAreaId` and says a negative
 *            value means "not area-bound", so it is read SIGNED.
 *
 *            NOT +0x10 and NOT +0x12, both of which look plausible and are
 *            wrong. +0x12 is the 4th ActorDerived::Spawn argument, which
 *            Actor::Spawn hardcodes to 2, so it carries no area at all. +0x10
 *            is a boolean written by ActorBase::AfterInitResources meaning
 *            "init'd while data_02099f24[0] == 3". port/hal/level_boot.cpp:3809
 *            says the area is the byte at +0x10 and reads it at :3825; that is
 *            a live bug in a file this lane does not own, so it is reported
 *            rather than edited. It is invisible in most testing because area
 *            0 is the common case and that boolean is usually 0 too.
 *
 *   yaw      Actor+0x8e when the caller did not give one, so a plain
 *            reposition keeps the facing it had.
 *
 * WHY ONLY THE YAW IS CARRIED. port_debug_spawn_at takes a single yaw and
 * builds `rot.x = 0; rot.y = yaw; rot.z = 0` (hal/level_boot.cpp:3797), which
 * is the game's own host spawn shape. Writing the other two angles onto the
 * new actor AFTERWARDS would be the exact inert write this verb exists to
 * avoid -- InitResources has already baked its transform by then -- so a
 * pitch or roll is not silently half-applied. Callers that want those use
 * objrot and accept that it may not move a baked actor.
 *
 * THE KILL is ActorBase::MarkForDestruction, the ROM's own
 * (src/_ZN9ActorBase18MarkForDestructionEv.cpp), declared for host callers at
 * hal/level_change.cpp:127. It is idempotent and self-guarding: it returns
 * early if shouldBeKilled is already set or aliveState is 2. Nothing here
 * frees an actor by hand -- marking sets +0x0f, and the game's own cleanup
 * phase moves the actor onto the cleanup list and dispatches its teardown on a
 * later frame, which is hal/level_change.cpp's stated contract.
 *
 * MarkForDestruction calls OnPendingDestroy SYNCHRONOUSLY through the vptr,
 * and an OnPendingDestroy may mark OTHER actors, relinking list nodes. That is
 * why this runs at the frame drain and why the pointer was revalidated against
 * a fresh walk immediately before. It is also why the spawn happens AFTER the
 * mark and reads nothing off the old object afterwards.
 *
 * THE OLD POINTER IS STILL IN THE LIST when this returns -- marking is not
 * freeing -- so the reply's new pointer is guaranteed distinct from it, and an
 * objlist a few frames later shows the old one gone and the new one present.
 * That gap is the editor's cue to rebind, which is why the reply carries the
 * new pointer at all.
 */
void exec_objrespawn(const Cmd &c)
{
    struct Actor *a = find_live(c.objptr);
    if (!a) {
        push_reply("err no such object\n");
        return;
    }
    char *o = (char *)a;

    /* NEVER THE PLAYER. Two independent reasons, either one sufficient: the
       Player's +0x08 is not a spawn param at all but a character bitfield
       (src/_ZN6Player13InitResourcesEv.cpp:93 writes it, and four more methods
       rewrite it), so the "same param" this verb promises cannot be honoured;
       and killing the Player out from under the camera and the controller is
       not an edit, it is a crash with extra steps. data_0209f394 is the
       per-slot Player pointer array, declared int[8] in hal/cxxname_bridge.cpp
       with at most kCommsMaxPlayers = 4 slots used, so this walk is in
       bounds. */
    for (int s = 0; s < 4; ++s) {
        if (data_0209f394[s] == (void *)o) {
            push_reply("err that is a player, not scenery\n");
            return;
        }
    }

    unsigned id = *(const unsigned short *)(o + OFF_ACTOR_ID);
    unsigned param = *(const unsigned *)(o + OFF_ACTOR_PARAM);
    int area = *(const signed char *)(o + OFF_ACTOR_AREA);
    int yaw = c.has_rot ? c.ry
                        : *(const short *)(o + OFF_ACTOR_ANGLE + 2);

    /* GOOMBA EDITS ITS OWN PARAM. src/_ZN6Goomba13InitResourcesEv.cpp:59-60
       does `*(int *)(c + 8) &= 0xf0ff` under a condition, so the word read
       back above is the MASKED one and not what the goomba was spawned with.
       The respawn still works and still produces a goomba; what it cannot
       promise is that the new one carries the original param bits. Said out
       loud on the game's log rather than silently, and NOT folded into the
       wire reply, which stays the single line the protocol promises. */
    if (id == 200)
        std::fprintf(stderr, "[editor] objrespawn: actor 200 (GOOMBA) rewrites "
                             "its own param at InitResources, so the param "
                             "carried over is the masked one, not necessarily "
                             "the one it was first spawned with\n");

    _ZN9ActorBase18MarkForDestructionEv(o);

    void *n = port_debug_spawn_at(id, param, c.x, c.y, c.z, yaw, area);
    if (!n) {
        /* port_debug_spawn_at refuses, on stdout and in its own words, when
           the class belongs to an overlay this level did not boot. The old
           actor is already marked at this point and that is not undone: it
           stays dead, which is the honest outcome of "replace this" when the
           replacement cannot be built, and saying so is better than pretending
           the object survived. */
        push_reply("err the game would not spawn that class here; the old "
                   "object is gone\n");
        return;
    }
    char ln[48];
    std::snprintf(ln, sizeof ln, "respawned %08X\n",
                  (unsigned)(uintptr_t)n);
    push_reply(ln);
}

/* ---- objspawn ---------------------------------------------------------------
 *
 * objrespawn's second half with the first half removed: no object is read, no
 * object is killed, the game simply builds one where it is told. Everything it
 * needs comes off the wire except the AREA, which does not, and that is the one
 * decision in this verb worth writing down.
 *
 * THE AREA IS READ FROM THE PLAYER'S Actor+0xCC, AND THAT IS DELIBERATELY NOT
 * WHAT hal/level_boot.cpp's port_debug_spawn DOES.
 *
 * The obvious move is to copy the base's own convenience helper, which spawns
 * "at the local player, facing the way he faces, in his area" and reads that
 * area at hal/level_boot.cpp:3825 as `*(const unsigned char *)(p + 0x10)`. That
 * read is WRONG, and this file already says so in exec_objrespawn's banner
 * above rather than having discovered it here: Actor+0x10 is a BOOLEAN written
 * by ActorBase::AfterInitResources meaning "init'd while data_02099f24[0] == 3",
 * not an area. The area is Actor+0xcc -- include/Actor.h:110 types it
 * `s8 mAreaId`, and src/_ZN5ActorC1Ev.cpp:49 is literally
 * `self->mAreaId = data_0209b44c;`, the global src/func_02010e78.c stages from
 * Actor::Spawn's areaID argument. exec_objrespawn reads +0xcc for exactly this
 * reason and has done since it shipped.
 *
 * So this verb resolves the area the way the file's OWN spawn verb already
 * does, not the way the shared helper does. Copying a read that the same file
 * documents as a bug would put the bug in a second place and make the two spawn
 * verbs disagree about which area an object lands in. hal/level_boot.cpp is not
 * this lane's file to fix, so the divergence is stated here instead of edited
 * there -- and it is a divergence in the correct direction.
 *
 * SIGNED, because include/Actor.h says a negative mAreaId means "not
 * area-bound"; reading it unsigned would turn that into area 255.
 *
 * WITH NO PLAYER there is no area to inherit and no level worth spawning into,
 * so the verb refuses before it gets here -- same guard exec_warp uses, and for
 * the same reason: data_0209f2f8 <= 0 means menus or boot, not a level.
 */
int current_area(void)
{
    const char *p = (const char *)data_0209f394[0];
    return p ? *(const signed char *)(p + OFF_ACTOR_AREA) : 0;
}

void exec_objspawn(const Cmd &c)
{
    /* Not in a level: Actor::Spawn would run with no Stage under it. The
       reference the ROM hands ActorDerived::Spawn (data_0209f5c0) is only
       meaningful inside a booted level. */
    if (data_0209f2f8 <= 0 || !data_0209f394[0]) {
        push_reply("err not in a level\n");
        return;
    }

    /* port_debug_spawn_at is REUSED rather than re-derived, exactly as
       exec_objrespawn reuses it: it runs the ROM's Actor::Spawn with an
       explicit position, yaw and area, allocates the death-table sequence the
       ROM allocates, and refuses in its own words when the class belongs to an
       overlay this level never booted. A second spawn path would be a second
       thing to keep true. */
    void *n = port_debug_spawn_at(c.spawn_id, c.spawn_param,
                                  c.x, c.y, c.z, c.ry, current_area());
    if (!n) {
        push_reply("err the game would not spawn that class here\n");
        return;
    }
    char ln[48];
    std::snprintf(ln, sizeof ln, "spawned %08X\n", (unsigned)(uintptr_t)n);
    push_reply(ln);
}

/* ---- objkill ---------------------------------------------------------------
 *
 * The kill half of objrespawn on its own. Same pointer rule as every other verb
 * that writes through a client-supplied pointer -- revalidated against a walk
 * of the live list THIS frame, because the client's pointer came from an
 * objlist that may be many frames old and the memory may have been reused.
 *
 * Same player guard, too, and it is not paranoia: killing the Player out from
 * under the camera and the controller is not an edit.
 *
 * MARKING IS NOT FREEING, and the reply says `ok` rather than pretending the
 * object is already gone. ActorBase::MarkForDestruction sets +0x0f and runs
 * OnPendingDestroy synchronously through the vptr; the game's own cleanup phase
 * moves the actor onto the cleanup list and frees it on a LATER frame. So an
 * objlist taken immediately after this still shows the object, and one taken a
 * frame or two later does not. That is the ROM's own lifecycle and this file
 * does not get to shortcut it -- hal/level_change.cpp's stated contract is that
 * nothing frees an actor by hand.
 *
 * It is idempotent by construction: MarkForDestruction returns early if
 * shouldBeKilled is already set or aliveState is 2, so a client that sends the
 * same kill twice in one frame gets two `ok`s and the game does the work once.
 */
void exec_objkill(const Cmd &c)
{
    struct Actor *a = find_live(c.objptr);
    if (!a) {
        push_reply("err no such object\n");
        return;
    }
    char *o = (char *)a;
    for (int s = 0; s < 4; ++s) {
        if (data_0209f394[s] == (void *)o) {
            push_reply("err that is a player, not scenery\n");
            return;
        }
    }
    _ZN9ActorBase18MarkForDestructionEv(o);
    push_reply("ok\n");
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
    case Cmd::OBJROT:   exec_objrot(c);   return;
    case Cmd::OBJRESPAWN: exec_objrespawn(c); return;
    case Cmd::OBJSPAWN: exec_objspawn(c); return;
    case Cmd::OBJKILL:  exec_objkill(c);  return;
    case Cmd::WARP:     exec_warp(c);     return;
    case Cmd::ERR:
        /* A refusal the socket thread decided on, emitted HERE so it takes its
           turn in command order. See enqueue_err's comment. */
        push_reply(c.msg);
        return;
    }
}

/* ---- line parsing, on the socket thread -----------------------------------
 *
 * Nothing here touches game state. EVERY line ends up on the queue -- the real
 * verbs, and equally the refusals this thread could answer by itself (unknown
 * verb, malformed argument list, the two unsupported memory pokes). Answering a
 * refusal immediately would let it overtake an earlier command's reply on the
 * same socket, and the Studio's router is strict FIFO; see enqueue_err.
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

/* objspawn's param, the ONE field on this channel that is neither a pointer nor
   a plain quantity. Base 0, so both `7936` and `0x1f00` are accepted and mean
   the same word.

   The channel's rule everywhere else is "pointers are hex, everything else is
   decimal", and this does not break it: a spawn param is a BITFIELD -- the
   level data prints it as `param 0x%04x` and every actor that reads one masks
   and shifts it -- so a caller copying a value out of a level dump has a hex
   string in hand and a caller computing one has a number. Refusing one of those
   two spellings would be refusing the format the value is normally written in.
   The refusal text says base 0 rather than leaving it to be discovered. */
bool parse_u32any(const std::string &t, unsigned &v)
{
    if (t.empty() || t.size() > 20) return false;
    if (t.find('\0') != std::string::npos) return false;
    errno = 0;
    char *end = 0;
    unsigned long r = std::strtoul(t.c_str(), &end, 0);
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

/* EVERY REPLY LEAVES FROM THE FRAME DRAIN, INCLUDING THE REFUSALS.
   Call with g_mtx held.

   The socket thread can decide some answers by itself -- a malformed argument
   list, an unknown verb, the two unsupported memory pokes -- and the obvious
   thing is to write them straight into the outbound buffer, which is what this
   file did first. That is a REPLY-ORDERING BUG, and a nasty one.

   Commands that need game state answer from editor_channel_drain() a frame
   later, so a refusal written immediately OVERTAKES a reply that was asked for
   first. Both arrive on the same socket, and the Studio's reply router
   (viewer_app.rs game_link_thread) is strict FIFO -- it pops end_kinds in
   order -- so an inverted pair silently misroutes: one write of
   "ping\npeek32 0" came back ["err unsupported", "pong"], and "objlist\nzzz"
   put the err ahead of the whole obj block, which the router then reads as the
   answer to a different request.

   Queueing the refusal too makes the queue the single ordering authority: every
   reply, refusal or not, is emitted from the drain in the order the commands
   arrived. A refusal dropped when the queue is full is the same bound every
   other command is under, and a flooding client is already being shed. */
void enqueue_err(const char *msg)
{
    Cmd c;
    std::memset(&c, 0, sizeof c);
    c.kind = Cmd::ERR;
    c.msg = msg;                /* string literal: static lifetime, no copy */
    enqueue(c);
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
            enqueue_err("err objmove <ptr> <x> <y> <z>\n");
            return;
        }
        c.kind = Cmd::OBJMOVE;
        c.objptr = ptr; c.x = x; c.y = y; c.z = z;
        enqueue(c);
        return;
    }

    if (verb == "objrot") {
        std::string a, b, d, e;
        unsigned ptr = 0;
        int rx = 0, ry = 0, rz = 0;
        if (!next_tok(line, i, a) || !next_tok(line, i, b) ||
            !next_tok(line, i, d) || !next_tok(line, i, e) ||
            !parse_u32hex(a, ptr) || !parse_int(b, rx) ||
            !parse_int(d, ry) || !parse_int(e, rz)) {
            /* THE UNIT IS IN THE REFUSAL, because it is the one thing a caller
               cannot guess and objlist never tells them: the obj line carries
               position and no rotation at all, so an editor writing its first
               objrot has nothing to copy the convention from. 65536 to the
               turn, and the message says so rather than making them read this
               file. */
            enqueue_err("err objrot <ptr> <rx> <ry> <rz> "
                        "(16-bit angle units, 65536 = one full turn, "
                        "so 16384 = a quarter turn)\n");
            return;
        }
        c.kind = Cmd::OBJROT;
        c.objptr = ptr; c.rx = rx; c.ry = ry; c.rz = rz;
        enqueue(c);
        return;
    }

    if (verb == "objrespawn") {
        std::string a, b, d, e, f;
        unsigned ptr = 0;
        int x = 0, y = 0, z = 0, ry = 0;
        if (!next_tok(line, i, a) || !next_tok(line, i, b) ||
            !next_tok(line, i, d) || !next_tok(line, i, e) ||
            !parse_u32hex(a, ptr) || !parse_int(b, x) ||
            !parse_int(d, y) || !parse_int(e, z)) {
            enqueue_err("err objrespawn <ptr> <x> <y> <z> [ry] "
                        "(position in Fix12, ry in 16-bit angle units)\n");
            return;
        }
        /* The yaw is optional, and a PRESENT-BUT-BROKEN yaw is an error rather
           than a silently ignored word -- the same shape warp uses for its
           optional entrance just below. */
        if (next_tok(line, i, f)) {
            if (!parse_int(f, ry)) {
                enqueue_err("err objrespawn <ptr> <x> <y> <z> [ry] "
                            "(position in Fix12, ry in 16-bit angle units)\n");
                return;
            }
            c.has_rot = 1;
        }
        c.kind = Cmd::OBJRESPAWN;
        c.objptr = ptr; c.x = x; c.y = y; c.z = z; c.ry = ry;
        enqueue(c);
        return;
    }

    if (verb == "objspawn") {
        /* Six arguments, all required. objrespawn's yaw is optional because it
           has an old actor to inherit a facing from; this verb has nothing to
           inherit, so leaving one out would mean picking a default silently. */
        static const char *usage =
            "err objspawn <actorid> <x> <y> <z> <ry> <param> "
            "(actorid 0..511 decimal, position in Fix12, ry in 16-bit angle "
            "units with 65536 = one full turn, param decimal or 0x-prefixed)\n";
        std::string a, b, d, e, f, g;
        int id = 0, x = 0, y = 0, z = 0, ry = 0;
        unsigned param = 0;
        if (!next_tok(line, i, a) || !next_tok(line, i, b) ||
            !next_tok(line, i, d) || !next_tok(line, i, e) ||
            !next_tok(line, i, f) || !next_tok(line, i, g) ||
            !parse_int(a, id) || !parse_int(b, x) || !parse_int(d, y) ||
            !parse_int(e, z) || !parse_int(f, ry) || !parse_u32any(g, param)) {
            enqueue_err(usage);
            return;
        }
        /* Bounded HERE, on the socket thread, so a typo never reaches the frame
           boundary at all. See MAX_ACTOR_ID for why 511 and not something
           larger: past it, Actor::Spawn indexes its own table out of range
           inside matched ROM code. */
        if (id < 0 || (unsigned)id > MAX_ACTOR_ID) {
            enqueue_err("err objspawn actorid is 0..511\n");
            return;
        }
        c.kind = Cmd::OBJSPAWN;
        c.spawn_id = (unsigned)id;
        c.spawn_param = param;
        c.x = x; c.y = y; c.z = z; c.ry = ry;
        enqueue(c);
        return;
    }

    if (verb == "objkill") {
        std::string a;
        unsigned ptr = 0;
        if (!next_tok(line, i, a) || !parse_u32hex(a, ptr)) {
            enqueue_err("err objkill <ptr>\n");
            return;
        }
        c.kind = Cmd::OBJKILL;
        c.objptr = ptr;
        enqueue(c);
        return;
    }

    if (verb == "warp") {
        std::string a, b;
        int level = 0, entrance = 0;
        if (!next_tok(line, i, a) || !parse_int(a, level)) {
            enqueue_err("err warp <level> [entrance]\n");
            return;
        }
        if (next_tok(line, i, b) && !parse_int(b, entrance)) {
            enqueue_err("err warp <level> [entrance]\n");
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
        enqueue_err("err unsupported\n");
        return;
    }

    enqueue_err("err unknown\n");
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
