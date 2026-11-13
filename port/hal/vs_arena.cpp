// ARENA MODE, the core half, implemented. Run mp1, lane ARENACORE1 (plan A0,
// runs/mp1/out/VSSYNC1/plan.txt). The contract (env, the 'SYNS' layout, the
// calls the other arena lanes make) is hal/vs_arena.h; read it first.
//
// PORT_HOST_ABI: host plumbing below the game. Nothing in src/ knows this file
// exists. It reads the local player's own body and the comms records, writes
// the per-slot key words before the ROM builds PadData, the ROM's "my comms
// slot" byte once per level boot, and the live byte / position of a body
// whose seat has no console. Every one of those writes is gated on arena mode
// being installed, so a solo or lockstep run never reaches them.
//
// ===========================================================================
// WHY THE ARENA NEEDS SO LITTLE OF THE ROM CHANGED
// ===========================================================================
//
// A VS boot with SM64DS_VS_PLAYERS=n and no transport already seats n Yoshi
// bodies in one world and runs the match (VSSYNC1 M4). The ROM's comms read,
// src/unnamed/arm9/0203/func_0203df40.c, takes its solo arm (role byte 0) and
// src/unnamed/arm9/0203/func_0203e0ac.c copies the local key word into all four per-slot
// records, so every body walks to the one pad. Arena mode changes two things
// about that world and nothing else:
//
//   1. MY SLOT IS MY SEAT. Stage::InitResources seats the local player index
//      as data_0209f250 = func_0203da9c(), which returns data_020a0f10, the
//      ROM's "my comms slot" byte. vs_arena_seat_world writes my hub seat into
//      that byte before the level boots, the way the lockstep conductor writes
//      its slot there (hal/comms_conductor.cpp comms_wait_for_session), so the
//      camera, the HUD's "me" and the pad all follow body <seat>.
//   2. MY PAD REACHES MY SLOT ONLY. vs_arena_route_pads clears the other
//      slots' key words after the ROM's solo copy and before the ROM's own
//      PadData builder (src/unnamed/arm9/0203/func_0203bc7c.c) turns them into PadData, so the
//      held / pressed / released words of every other slot are a plain zero
//      pad, built by the ROM's own code.
//
// Nothing waits on anybody: there is no transport, so the conductor answers
// "solo" everywhere and no round is ever exchanged. That is the whole reason
// the own body has zero added input delay at any ping, and the
// SM64DS_ARENA_LATENCY readout below is how a row shows it.
//
// ===========================================================================
// THE STATE STREAM
// ===========================================================================
//
// Every frame of the VS level this console samples its own body into a
// 'SYNS' frame and sends it through the hub (30 Hz while anything a puppet
// reads changes, 10 Hz while it stands still). Received frames are drained
// once per frame into a ring per seat, newest first by the sender's frame,
// with the time the hub's I/O thread took each one off the socket. A1's
// puppets read the rings; the referee keeps its own copy in the relay.

#include "vs_arena.h"

#include "comms_hub.h"
#include "ghost_tongue.h"
#include "host_settings.h"
#include "player_fields.h"
#include "vs_arena_eat.h"
#include "vs_arena_grab.h"
#include "voice_chat.h"

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
// The per-slot Player pointers, the local player index and the body count.
extern void *data_0209f394[];
extern unsigned char data_0209f250[];
extern unsigned char data_0209f21c;
// The game mode: 1 = VS.
extern unsigned char data_0209f2d8;
// The per-slot live bytes and the ROM's "seats lost" bits (src/unnamed/arm9/0203/func_02030b58.c
// writes both when a player's link goes; hal/star_flow.cpp vs_remove_body is
// the port's lockstep copy of the same two writes).
extern unsigned char data_0209fc5c[];
extern unsigned char data_0209fc58;
// MY COMMS SLOT (hosted by hal/actor_vtables.cpp; read by src/unnamed/arm9/0203/func_0203da9c.c).
extern unsigned char data_020a0f10[];
// The local comms record (key word at +0x0e, written from KEYINPUT by the
// ROM's comms read) and the per-slot records, 0x24 apiece (key word at +0x0e:
// src/unnamed/arm9/0203/func_0203dae4.c reads it for slot i at data_020a1162 + i*0x24).
extern unsigned char data_020a1040[];
extern unsigned char data_020a1154[];
// The 78 Player::State objects, ov002 bss order (hal/pmf3_player_states.cpp
// seats their cells; the order is src/unnamed/ov002/__sinit_ov002_021019d0.c's).
extern unsigned char data_ov002_0210ffec[], data_ov002_02110004[], data_ov002_0211001c[];
extern unsigned char data_ov002_02110034[], data_ov002_0211004c[], data_ov002_02110064[];
extern unsigned char data_ov002_0211007c[], data_ov002_02110094[], data_ov002_021100ac[];
extern unsigned char data_ov002_021100c4[], data_ov002_021100dc[], data_ov002_021100f4[];
extern unsigned char data_ov002_0211010c[], data_ov002_02110124[], data_ov002_0211013c[];
extern unsigned char data_ov002_02110154[], data_ov002_0211016c[], data_ov002_02110184[];
extern unsigned char data_ov002_0211019c[], data_ov002_021101b4[], data_ov002_021101cc[];
extern unsigned char data_ov002_021101e4[], data_ov002_021101fc[], data_ov002_02110214[];
extern unsigned char data_ov002_0211022c[], data_ov002_02110244[], data_ov002_0211025c[];
extern unsigned char data_ov002_02110274[], data_ov002_0211028c[], data_ov002_021102a4[];
extern unsigned char data_ov002_021102bc[], data_ov002_021102d4[], data_ov002_021102ec[];
extern unsigned char data_ov002_02110304[], data_ov002_0211031c[], data_ov002_02110334[];
extern unsigned char data_ov002_0211034c[], data_ov002_02110364[], data_ov002_0211037c[];
extern unsigned char data_ov002_02110394[], data_ov002_021103ac[], data_ov002_021103c4[];
extern unsigned char data_ov002_021103dc[], data_ov002_021103f4[], data_ov002_0211040c[];
extern unsigned char data_ov002_02110424[], data_ov002_0211043c[], data_ov002_02110454[];
extern unsigned char data_ov002_0211046c[], data_ov002_02110484[], data_ov002_0211049c[];
extern unsigned char data_ov002_021104b4[], data_ov002_021104cc[], data_ov002_021104e4[];
extern unsigned char data_ov002_021104fc[], data_ov002_02110514[], data_ov002_0211052c[];
extern unsigned char data_ov002_02110544[], data_ov002_0211055c[], data_ov002_02110574[];
extern unsigned char data_ov002_0211058c[], data_ov002_021105a4[], data_ov002_021105bc[];
extern unsigned char data_ov002_021105d4[], data_ov002_021105ec[], data_ov002_02110604[];
extern unsigned char data_ov002_0211061c[], data_ov002_02110634[], data_ov002_0211064c[];
extern unsigned char data_ov002_02110664[], data_ov002_0211067c[], data_ov002_02110694[];
extern unsigned char data_ov002_021106ac[], data_ov002_021106c4[], data_ov002_021106dc[];
extern unsigned char data_ov002_021106f4[], data_ov002_0211070c[], data_ov002_02110724[];
}  // extern "C"

// PUPPET (A1 PUPPET1) externs: what the follower below calls. The same flat
// names hal/comms_sync.cpp's ghost follower and hal/player_bridges.cpp use.
extern "C" {
// Player::SetAnim, declared void (its true return type; see comms_sync.cpp).
void _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned int, int, int, unsigned int);
// the ROM's body placement and the ROM's one per-frame pose
void func_ov002_020e444c(char *self);
void func_ov002_020e4768(char *self);
int func_ov002_020e4990(char *c);       // the carry arm layer: off, then on while +0x358 holds something and the anim is a carry anim
// the in-place character change (hal/player_bridges.cpp, the door path)
void port_player_set_character(void *player, unsigned ch);
int _ZN6Player8HasNoCapEv(void *self);
int _ZN6Player6IsAnimEj(void *self, unsigned id);
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned cls, char *prev);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b,
        const void *pos, const void *v16, int area, int f);
void _ZN15dExtFrameCtrl_c7AdvanceEv(void *self);
// V8: the Yoshi tongue is the HEAD model's animation (func_ov002_020d71ec sets
// the body anim and the head anim together; 020d71a0 puts the head back)
void func_ov002_020d71ec(char *p, int arg);
void _ZN6Player19func_ov002_020d71a0Ev(char *p);
int _ZN6Player19func_ov002_020bea7cEv(char *self);
unsigned _ZN6Player19func_ov002_020becf4Eii(char *self, unsigned idx, int flag);
// the body anim ids of the tongue (arg 0 / 1) live in a two-word table
extern unsigned char data_ov002_0210a34c[];
extern unsigned char data_ov002_0210a084[];
extern unsigned char data_ov002_0210eb88[];
// the ROM's sound range (src/_ZN5Sound4PlayEjjRK7Vector3.cpp's culling): full
// volume inside the first, linear to nothing at the second
extern int data_02099fa8, data_02099fac;
// the collision list: a body whose Behavior is skipped registers its own
// cylinder, the two calls Player::Behavior ends with
void _ZN5dCc_c5ClearEv(void *self);
void _ZN5dCc_c6UpdateEv(void *self);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, const void *v);
// world -> camera space (the vector at Player+0x74 the sound system reads)
void MulVec3Mat4x3(void *v, void *m, void *dst);
extern int data_0209b3ec[];
// the head of the active collision list (src/_ZN5dCc_c6UpdateEv.cpp)
extern void *data_0209cee8;
// the flat Sound::Play (hal/sdat/sound_abi.cpp), the one door every sound
// request goes through
void _ZN5Sound4PlayEjjRK7Vector3(unsigned kind, unsigned id, void *v);
// the voice base per character (src/_ZN5Sound13PlayCharVoiceEjjRK7Vector3.cpp)
extern unsigned char data_02075250[];
}  // extern "C"

namespace port {
namespace {

// ---------------------------------------------------------------------------
// THE STATE INDEX. Player+0x370 points at one of the 78 objects above.
// ---------------------------------------------------------------------------
const unsigned char *const kStateObjects[kArenaStates] = {
    data_ov002_0210ffec, data_ov002_02110004, data_ov002_0211001c,
    data_ov002_02110034, data_ov002_0211004c, data_ov002_02110064,
    data_ov002_0211007c, data_ov002_02110094, data_ov002_021100ac,
    data_ov002_021100c4, data_ov002_021100dc, data_ov002_021100f4,
    data_ov002_0211010c, data_ov002_02110124, data_ov002_0211013c,
    data_ov002_02110154, data_ov002_0211016c, data_ov002_02110184,
    data_ov002_0211019c, data_ov002_021101b4, data_ov002_021101cc,
    data_ov002_021101e4, data_ov002_021101fc, data_ov002_02110214,
    data_ov002_0211022c, data_ov002_02110244, data_ov002_0211025c,
    data_ov002_02110274, data_ov002_0211028c, data_ov002_021102a4,
    data_ov002_021102bc, data_ov002_021102d4, data_ov002_021102ec,
    data_ov002_02110304, data_ov002_0211031c, data_ov002_02110334,
    data_ov002_0211034c, data_ov002_02110364, data_ov002_0211037c,
    data_ov002_02110394, data_ov002_021103ac, data_ov002_021103c4,
    data_ov002_021103dc, data_ov002_021103f4, data_ov002_0211040c,
    data_ov002_02110424, data_ov002_0211043c, data_ov002_02110454,
    data_ov002_0211046c, data_ov002_02110484, data_ov002_0211049c,
    data_ov002_021104b4, data_ov002_021104cc, data_ov002_021104e4,
    data_ov002_021104fc, data_ov002_02110514, data_ov002_0211052c,
    data_ov002_02110544, data_ov002_0211055c, data_ov002_02110574,
    data_ov002_0211058c, data_ov002_021105a4, data_ov002_021105bc,
    data_ov002_021105d4, data_ov002_021105ec, data_ov002_02110604,
    data_ov002_0211061c, data_ov002_02110634, data_ov002_0211064c,
    data_ov002_02110664, data_ov002_0211067c, data_ov002_02110694,
    data_ov002_021106ac, data_ov002_021106c4, data_ov002_021106dc,
    data_ov002_021106f4, data_ov002_0211070c, data_ov002_02110724,
};

int state_index_of(const void *body) {
    if (!body) return 0xFF;
    const void *st = player::state_ptr(body);
    for (int i = 0; i < kArenaStates; ++i)
        if (st == (const void *)kStateObjects[i]) return i;
    return 0xFF;
}

enum : int {
    kStHurt = 7, kStHurtWater = 8,            // state_table.json
    kRecStride = 0x24, kRecKeys = 0x0e,       // per-slot comms record
    kNarrowSlots = 4,                         // the ROM's record copy width
    kArenaWide = 16,                          // the wide fan-out (comms_fanout_wide.cpp) feeds slots 4..15
    kGraceFrames = 150,                       // 5 s: a seat never heard by
                                              // then has its body held
    kParkUp = 200000 << 12,                   // DROPOUT1's parking spot
    kParkSide = 100000 << 12,
    kTeleportH = 200 << 12,                   // a step no state can make
    kTeleportV = 300 << 12,                   // (M2: 156 / 185 max)
    kRestartBack = 300,                       // a frame jump back this far
                                              // is a restarted sender
    kOffsetBuckets = 8,                       // 8 one-second minima
    kReportEveryMs = 10000,
};

struct SeatRing {
    ArenaSample s[kArenaRing];    // s[0] newest
    int n;
    unsigned epoch;               // the hub join_epoch the ring belongs to
    unsigned samples, rejected;
    int bucket_off[kOffsetBuckets];
    unsigned bucket_sec[kOffsetBuckets];
    bool ever_present;
    bool held;
    int held_pos[3];
};

struct Arena {
    bool installed = false;
    bool hub = false;             // the hub opened (relay or mesh)
    int want_seat = 0xFF;
    int seat = -1;
    int bodies = 0;
    unsigned frame = 0;           // my match frame (SYNS frame)
    bool frame_started = false;
    unsigned start_tick = 0;      // vs_arena_tick count at the first VS frame
    // sender
    ArenaState last{};            // the last SYNS sent
    bool have_last = false;
    int prev_pos[3] = {0, 0, 0};
    bool have_prev = false;
    unsigned since_send = 0;
    bool send_now = false;
    unsigned char sfx[2] = {0, 0};
    int last_state = 15;          // St_Wait until the body has a state
    int nsfx = 0;
    unsigned short ack = 0;
    bool star_lock = false;
    // receive
    SeatRing ring[kArenaSeats];
    unsigned roster_mask = 0, roster_version = 0;
    unsigned last_report = 0;
    ArenaStats st{};
    bool log = false;
};
Arena g;
CRITICAL_SECTION g_cs;            // the rings: A1 / A3 read them from the
bool g_cs_up = false;             // game thread too, but keep it honest

struct Guard {
    Guard() { if (g_cs_up) ::EnterCriticalSection(&g_cs); }
    ~Guard() { if (g_cs_up) ::LeaveCriticalSection(&g_cs); }
};

int env_int(const char *name, int def, int lo, int hi) {
    const char *s = std::getenv(name);
    if (!s || !*s) return def;
    char *end = nullptr;
    const long v = std::strtol(s, &end, 10);
    if (end == s || *end || v < lo || v > hi) {
        std::fprintf(stderr, "[vsarena] %s=%s is out of range %d..%d; using %d\n",
                     name, s, lo, hi, def);
        return def;
    }
    return (int)v;
}

bool env_on(const char *name) {
    const char *s = std::getenv(name);
    return s && *s && !(s[0] == '0' && s[1] == 0);
}

// SEAT PIN, first one present wins (vs_arena.h): the launcher's go plan
// already names role and slot for every member, so it pins the seat with no
// new knob. 0xFF = any free seat.
int seat_pin_from_env(const char **why) {
    *why = "no pin: any free seat";
    /* SEAMVS1 (REVSEAMJOIN1 note): the hub seat comes from the session record (hal/comms_hub.h), which a hand-off fills and the
       in-place end clears, the way the hub's own install reads it */
    if (const char *s = online_session_get(kSessHubSeat))
        if (*s) {
            char *end = nullptr;
            const long v = std::strtol(s, &end, 10);
            *why = "SM64DS_HUB_SEAT (the session record)";
            if (end != s && !*end && v >= 0 && v <= 15) return (int)v;
            std::fprintf(stderr, "[vsarena] SM64DS_HUB_SEAT=%s is out of range 0..15; using any free seat\n", s);
            return 0xFF;
        }
    if (const char *s = std::getenv("SM64DS_COMMS_SLOT"))
        if (*s) { *why = "SM64DS_COMMS_SLOT"; return env_int("SM64DS_COMMS_SLOT", 0xFF, 0, 15); }
    if (const char *r = std::getenv("SM64DS_COMMS_ROLE"))
        if (std::strcmp(r, "parent") == 0) { *why = "SM64DS_COMMS_ROLE=parent"; return 0; }
    return 0xFF;
}

inline unsigned short *slot_keys(int slot) {
    return (unsigned short *)(data_020a1154 + slot * kRecStride + kRecKeys);
}

inline int clamp16(int v) {
    return v > 32767 ? 32767 : v < -32767 ? -32767 : v;
}

int body_seat(const void *ptr) {
    if (!ptr) return -1;
    for (int k = 0; k < kArenaSeats && k < g.bodies; ++k)
        if (data_0209f394[k] == ptr) return k;
    return -1;
}

// ---------------------------------------------------------------------------
// SAMPLING MY BODY into a SYNS frame (vs_arena.h for every field's source).
// ---------------------------------------------------------------------------
void pup_lab_forge(ArenaState *s);   // PUPEAT2 lab knob, defined with the puppet code
void sample_body(void *body, ArenaState *s) {
    const char *b = (const char *)body;
    std::memset(s, 0, sizeof *s);
    s->tag = kArenaTagState;
    s->version = 1;
    s->seat = (unsigned char)g.seat;
    // A1 PUPPET1: the LOOK the body wears, Player param1 (+0x08), not
    // Player+0x6d9 (mCharacter, the real character, which a cap never changes:
    // Player::SetNewHatCharacter moves param1 and mHatCharacter only). 3 for a
    // Yoshi, 0..2 for a Mario / Luigi / Wario cap; the puppet puts its body in
    // the same look.
    s->character = (unsigned char)(*(const int *)(b + 8) & 7);
    s->frame = g.frame;
    s->x = *player::pos_x(body);
    s->y = *player::pos_y(body);
    s->z = *player::pos_z(body);
    s->yaw = *player::facing(body);
    unsigned short anim = player::anim_id(body);
    s->anim_id = anim < kArenaAnimLimit ? anim : 0;
    const int cur = player::anim_frame(body);
    s->anim_cursor = (unsigned short)((cur < 0 ? 0 : cur) >> 8);
    // NEVER 0xFF ON THE WIRE: the referee drops a SYNS whose state is not one
    // of the 78 (referee.py check_state), so a body between states (no state
    // object yet at level entry) repeats the last one it had.
    int st = state_index_of(body);
    if (st == 0xFF) st = g.last_state;
    else g.last_state = st;
    s->state_idx = (unsigned char)st;
    s->state_step = *(const unsigned char *)(b + 0x6e3);
    // CLAMPED TO 36, the VS hurt window (plan 2.1): the referee drops a SYNS
    // with more (referee.py check_state), and nothing reads past "still
    // invincible" anyway.
    const unsigned inv = player::invincible_timer(body);
    s->invuln = (unsigned char)(inv > kArenaInvulnMax ? kArenaInvulnMax : inv);
    unsigned char pw = 0;
    if (*(const unsigned short *)(b + 0x6ce) & 0x1000) pw |= kArenaFire;
    if (b[0x6ff]) pw |= kArenaWings;
    if (b[0x6f9]) pw |= kArenaMetal;
    if (b[0x6fb]) pw |= kArenaVanish;
    if (b[0x703]) pw |= kArenaMega;
    if (b[0x71a]) pw |= kArenaNoCap;
    s->power = pw;
    // held: who is in my mouth (mObjInMouth +0x360), else anything I carry
    // (mHeldObj +0x358); eaten_by: whose mouth I am in (mEatingPlayer +0xd0)
    void *mouth = *(void *const *)(b + 0x360);
    void *carry = *(void *const *)(b + 0x358);
    void *eater = *(void *const *)(b + 0xd0);
    const int ms = body_seat(mouth);
    s->held = ms >= 0 ? (unsigned char)(ms + 1)
            : (mouth || carry) ? (unsigned char)kArenaHeldObject : 0;
    const int es = body_seat(eater);
    s->eaten_by = es >= 0 ? (unsigned char)(es + 1) : 0;
    pup_lab_forge(s);
    unsigned char fl = 0;
    if (g.seat >= 0 && data_0209fc5c[g.seat]) fl |= kArenaLive;
    if (player::on_ground(body)) fl |= kArenaGrounded;
    if (voice_talking()) fl |= kArenaTalking;
    if (st == kStHurt || st == kStHurtWater || b[0x708]) fl |= kArenaInHurt;
    if (b[0x709]) fl |= kArenaNoControl;
    if (g.star_lock) fl |= kArenaStarLock;
    // THE STEP since the previous frame, in 1/64 unit (Fix12 >> 6). A step no
    // state can make in one frame is a teleport: snap, no velocity.
    if (g.have_prev) {
        const int dx = s->x - g.prev_pos[0];
        const int dy = s->y - g.prev_pos[1];
        const int dz = s->z - g.prev_pos[2];
        const int ax = dx < 0 ? -dx : dx, az = dz < 0 ? -dz : dz;
        const int ay = dy < 0 ? -dy : dy;
        if (ax > kTeleportH || az > kTeleportH || ay > kTeleportV) {
            fl |= kArenaTeleport;
        } else {
            s->vx = (short)clamp16(dx >> 6);
            s->vy = (short)clamp16(dy >> 6);
            s->vz = (short)clamp16(dz >> 6);
        }
    }
    s->flags = fl;
    s->ack = g.ack;
    for (int i = 0; i < g.nsfx && i < 2; ++i) s->sfx[i] = g.sfx[i];
    g.prev_pos[0] = s->x; g.prev_pos[1] = s->y; g.prev_pos[2] = s->z;
    g.have_prev = true;
}

// What a puppet reads changed since the last SYNS sent (the cursor and the
// invincibility countdown tick every frame and are left out on purpose).
bool changed(const ArenaState &a, const ArenaState &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z || a.yaw != b.yaw ||
           a.anim_id != b.anim_id || a.state_idx != b.state_idx ||
           a.state_step != b.state_step || a.flags != b.flags ||
           a.power != b.power || a.held != b.held ||
           a.eaten_by != b.eaten_by || a.character != b.character ||
           a.ack != b.ack || (a.invuln == 0) != (b.invuln == 0) ||
           a.sfx[0] != 0;
}

void send_state(void *body) {
    ArenaState s;
    sample_body(body, &s);
    const bool due = !g.have_last || g.send_now || changed(s, g.last) ||
                     g.since_send + 1 >= 3;
    g.nsfx = 0;
    g.sfx[0] = g.sfx[1] = 0;
    if (!due) { ++g.since_send; return; }
    const bool ok = comms_hub_send(&s, (int)sizeof s);
    if (ok) ++g.st.sent; else ++g.st.send_refused;
    g.last = s;
    g.have_last = true;
    g.since_send = 0;
    g.send_now = false;
    if (g.log)
        std::fprintf(stderr, "[vsarena] tx f=%u seat=%d pos=%d,%d,%d v=%d,%d,%d "
                     "yaw=%d anim=%u cur=%u st=%u step=%u fl=%02x pw=%02x "
                     "held=%u eaten=%u inv=%u%s\n", s.frame, g.seat, s.x, s.y,
                     s.z, s.vx, s.vy, s.vz, s.yaw, s.anim_id, s.anim_cursor,
                     s.state_idx, s.state_step, s.flags, s.power, s.held,
                     s.eaten_by, s.invuln, ok ? "" : " REFUSED");
}

// ---------------------------------------------------------------------------
// RECEIVING
// ---------------------------------------------------------------------------
void reset_ring(SeatRing &r, unsigned epoch) {
    r.n = 0;
    r.epoch = epoch;
    r.samples = r.rejected = 0;
    for (int i = 0; i < kOffsetBuckets; ++i) r.bucket_sec[i] = ~0u;
}

void ingest(int seat, const ArenaState &in, unsigned t) {
    if (seat < 0 || seat >= kArenaSeats || seat == g.seat) {
        ++g.st.rejected;
        return;
    }
    ArenaState s = in;
    s.seat = (unsigned char)seat;   // the carrier's stamp is the only truth
    SeatRing &r = g.ring[seat];
    HubSeatInfo hi;
    const unsigned epoch = comms_hub_seat_info(seat, &hi) ? hi.join_epoch : 0;
    if (epoch != r.epoch) reset_ring(r, epoch);
    if (!arena_state_valid(s)) {
        ++r.rejected;
        ++g.st.rejected;
        return;
    }
    if (r.n > 0) {
        const unsigned newest = r.s[0].s.frame;
        if (s.frame <= newest) {
            if (newest - s.frame > (unsigned)kRestartBack) {
                reset_ring(r, epoch);       // that console started again
            } else {
                ++r.rejected;
                ++g.st.out_of_order;
                return;
            }
        }
    }
    const int keep = r.n < kArenaRing ? r.n : kArenaRing - 1;
    std::memmove(&r.s[1], &r.s[0], sizeof(ArenaSample) * (size_t)keep);
    r.s[0].s = s;
    r.s[0].recv_ms = t;
    r.n = keep + 1;
    ++r.samples;
    ++g.st.recvd;
    r.ever_present = true;
    // the clock offset's lower envelope, one minimum per second over 8 s
    const int off = (int)(t - (unsigned)((unsigned long long)s.frame * 1000ull / 30ull));
    const unsigned sec = t / 1000u;
    const int bi = (int)(sec % kOffsetBuckets);
    if (r.bucket_sec[bi] != sec) { r.bucket_sec[bi] = sec; r.bucket_off[bi] = off; }
    else if (off < r.bucket_off[bi]) r.bucket_off[bi] = off;
    if (g.log)
        std::fprintf(stderr, "[vsarena] rx seat=%d f=%u t=%u pos=%d,%d,%d "
                     "v=%d,%d,%d yaw=%d anim=%u st=%u fl=%02x\n", seat,
                     s.frame, t, s.x, s.y, s.z, s.vx, s.vy, s.vz, s.yaw,
                     s.anim_id, s.state_idx, s.flags);
}

void drain() {
    unsigned char buf[64];
    int seat = -1;
    unsigned t = 0;
    int n;
    Guard gd;
    while ((n = comms_hub_recv_t(kHubArenaState, buf, (int)sizeof buf, &seat,
                                 &t)) > 0) {
        if (n != (int)sizeof(ArenaState)) { ++g.st.rejected; continue; }
        ArenaState s;
        std::memcpy(&s, buf, sizeof s);
        ingest(seat, s, t);
    }
}

// ---------------------------------------------------------------------------
// BODIES OF ABSENT SEATS (vs_arena.h). The two writes the cartridge makes
// for a lost player (src/unnamed/arm9/0203/func_02030b58.c: its "lost" bit for seats 0..3 and
// the live byte), plus DROPOUT1's parking spot so the body is nobody's
// nearest player and off the bottom-screen map.
// ---------------------------------------------------------------------------
void hold_body(int k, const char *why) {
    SeatRing &r = g.ring[k];
    if (r.held) return;
    char *c = (char *)data_0209f394[k];
    if (k < 4) data_0209fc58 = (unsigned char)(data_0209fc58 | (1u << k));
    data_0209fc5c[k] = 0;
    r.held = true;
    ++g.st.holds;
    if (!c) {
        std::fprintf(stderr, "[vsarena] f%u seat %d held (%s): no body\n",
                     g.frame, k, why);
        return;
    }
    int *pos = (int *)(c + 0x5c);
    r.held_pos[0] = pos[0]; r.held_pos[1] = pos[1]; r.held_pos[2] = pos[2];
    pos[0] += kParkSide;
    pos[1] += kParkUp;
    pos[2] += kParkSide;
    *(int *)(c + 0x98) = 0;                  // horizontal speed
    *(int *)(c + 0xa8) = 0;                  // vertical speed
    std::fprintf(stderr, "[vsarena] f%u seat %d held (%s): body out of play at "
                 "(%d,%d,%d), parked\n", g.frame, k, why, r.held_pos[0] >> 12,
                 r.held_pos[1] >> 12, r.held_pos[2] >> 12);
}

void release_body(int k) {
    SeatRing &r = g.ring[k];
    if (!r.held) return;
    char *c = (char *)data_0209f394[k];
    if (c) {
        int *pos = (int *)(c + 0x5c);
        pos[0] = r.held_pos[0]; pos[1] = r.held_pos[1]; pos[2] = r.held_pos[2];
    }
    if (k < 4) data_0209fc58 = (unsigned char)(data_0209fc58 & ~(1u << k));
    data_0209fc5c[k] = 1;
    r.held = false;
    ++g.st.releases;
    std::fprintf(stderr, "[vsarena] f%u seat %d back: body in play again\n",
                 g.frame, k);
}

void roster() {
    const unsigned mask = comms_hub_seat_mask() & 0xFFFFu;
    if (mask != g.roster_mask) {
        std::fprintf(stderr, "[vsarena] f%u roster 0x%04x -> 0x%04x (me %d)\n",
                     g.frame, g.roster_mask, mask, g.seat);
        g.roster_mask = mask;
        ++g.roster_version;
    }
    const unsigned since = g.frame;
    for (int k = 0; k < g.bodies && k < kArenaSeats; ++k) {
        if (k == g.seat) continue;
        SeatRing &r = g.ring[k];
        const bool present = (mask >> k) & 1u;
        if (present) {
            r.ever_present = true;
            release_body(k);
        } else if (r.ever_present) {
            hold_body(k, "its console left");
        } else if (since >= (unsigned)kGraceFrames) {
            hold_body(k, "no console in this seat");
        }
    }
}

// ---------------------------------------------------------------------------
// THE LATENCY READOUT (SM64DS_ARENA_LATENCY=1). Works in every VS netmode so
// solo, lockstep and arena can be compared on the same rung: the frame my
// local key word first carries A (the ROM's own comms read filled
// data_020a1040 from KEYINPUT this frame) and the first frame my own body's
// state object changes after it. Read only.
// ---------------------------------------------------------------------------
struct Lat {
    int on = -1;
    unsigned tick = 0;
    unsigned short prev_keys = 0;
    int prev_state = 0xFF;        // my body's state at the end of last frame
    bool armed = false;
    unsigned press = 0;
    int base_state = 0xFF;
} g_lat;

void lat_answer(int me, unsigned at, int from, int to) {
    std::fprintf(stderr, "[vsarena-lat] press A at tick %u, my body (slot %d) "
                 "answered at tick %u: %u frame(s) (state %d -> %d) "
                 "netmode=%s\n", g_lat.press, me, at, at - g_lat.press, from,
                 to, g.installed ? "arena" : "not-arena");
}

// The edge and the answer are both read at the END of a frame (this runs
// after the actor tick), so a press the body answers in the same frame's tick
// reads 0 frames: the state then differs from the one it had at the end of
// the previous frame.
void latency_probe() {
    if (g_lat.on < 0) g_lat.on = env_on("SM64DS_ARENA_LATENCY") ? 1 : 0;
    if (!g_lat.on) return;
    ++g_lat.tick;
    if (data_0209f2d8 != 1) return;
    const int me = (int)data_0209f250[0];
    if (me < 0 || me >= kArenaSeats) return;
    void *body = data_0209f394[me];
    const unsigned short keys =
        *(const unsigned short *)(data_020a1040 + kRecKeys);
    const unsigned short edge = (unsigned short)(keys & ~g_lat.prev_keys);
    g_lat.prev_keys = keys;
    if (!body) return;
    const int st = state_index_of(body);
    if (g_lat.armed) {
        if (st != g_lat.base_state) {
            lat_answer(me, g_lat.tick, g_lat.base_state, st);
            g_lat.armed = false;
        } else if (g_lat.tick - g_lat.press > 30) {
            std::fprintf(stderr, "[vsarena-lat] press A at tick %u: no answer "
                         "from my body (slot %d) in 30 frames (state %d)\n",
                         g_lat.press, me, st);
            g_lat.armed = false;
        }
    } else if (edge & 0x0001u) {
        g_lat.press = g_lat.tick;
        if (st != g_lat.prev_state) {
            lat_answer(me, g_lat.tick, g_lat.prev_state, st);
        } else {
            g_lat.armed = true;
            g_lat.base_state = st;
        }
    }
    g_lat.prev_state = st;
}

void report_line(const char *tag) {
    char rings[16 * 24] = "";
    int o = 0;
    for (int k = 0; k < kArenaSeats && o < (int)sizeof rings - 24; ++k) {
        const SeatRing &r = g.ring[k];
        if (!r.samples && !r.held) continue;
        o += std::snprintf(rings + o, sizeof rings - (size_t)o,
                           "%s%d:%u/%u%s", o ? "," : "", k, r.samples,
                           r.rejected, r.held ? "H" : "");
    }
    std::fprintf(stderr, "[vsarena] %s f=%u seat=%d bodies=%d hub=%d sent=%llu "
                 "refused=%llu recvd=%llu rejected=%llu ooo=%llu "
                 "roster=0x%04x holds=%llu releases=%llu rings=[%s]\n",
                 tag, g.frame, g.seat, g.bodies, comms_hub_state(),
                 g.st.sent, g.st.send_refused, g.st.recvd, g.st.rejected,
                 g.st.out_of_order, g.roster_mask, g.st.holds,
                 g.st.releases, rings);
}

void report_at_exit() { report_line("exit"); }

// ===========================================================================
// PUPPET (A1 PUPPET1): THE OTHER SEATS' BODIES, DRAWN FROM THEIR STATE STREAMS
// (VSSYNC1 plan sections 2.4, 2.5 and 9 R1). Run mp1, lane PUPPET1. Everything
// from here to the end of this anonymous namespace is the puppet half; the code
// above this line is the core half (A0) and is not changed by it.
// ===========================================================================
//
// WHAT A PUPPET IS. The body of every seat that is not mine stays a real
// Player actor in the world (so the ROM's own camera, map, targeting and
// collision lists see a body there), but its Behavior never runs: the walk's
// BeforeBehavior returns 0 while pauseFlags (+0x13) bit 2 is set
// (src/_ZN7fBase_c14BeforeBehaviorEv.cpp), exactly the GHOSTWIRE1 adventure
// hold. The scene pass at the end of every frame clears that bit again, so it
// is re-asserted here, right before the walk (vs_arena_before_walk).
//
// Each frame, for each puppet, this does what hal/comms_sync.cpp's ghost
// follower (gw_follow) does for an adventure ghost, driven from the SYNS ring
// instead of SYNG frames:
//   - place the body where the owner's body was B ms before it arrived (a
//     draw time 'B behind the newest packet', interpolated; dead-reckoned for
//     at most 150 ms past the newest packet, then held; a teleport frame
//     snaps, nothing is smoothed across it);
//   - install the owner's animation at the owner's cursor (Player::SetAnim,
//     the cursor reset the ghost follower documents), place the matrix and the
//     shadow (func_ov002_020e444c), run the ROM's one pose
//     (func_ov002_020e4768) and the six render latches (func_ov002_020e4bb8's
//     tail), so the head, the cap look and the VS colour row draw;
//   - THE COLLISION HALF (plan R1). With Behavior skipped nothing registers the
//     body's cylinder (Player::Behavior ends with dCc_c::Clear then Update on
//     +0x2d4, after putting the cylinder at the actor with
//     SetPosRelativeToActor). The puppet does both itself, every frame, so the
//     pair pass (CylinderClsn::Process, in Stage::Render) still finds a body
//     there: my body is pushed by it and my own attack cylinder lands its hit
//     flags on it, which is what A3's hit claim reads. The puppet's ATTACK
//     cylinder (+0x314) is switched off (flags bit 0 stops Update), so the ROM
//     can never apply a hit from a puppet by itself;
//   - SOUNDS: a sound my body asks for (Sound::Play with the vector at my own
//     Player+0x74) goes into the next SYNS (slot 0 = a bank 0 effect id, slot
//     1 = a bank 1 character voice id), and a puppet replays each one once,
//     when the drawn timeline reaches the sample that carried it, at the
//     puppet.
//
// LAB KNOBS (off by default): SM64DS_PUPPET=0 turns the puppets off (the A0
// behaviour: other bodies stand where they spawned); SM64DS_PUPPET_LOG=1 one
// line per drawn puppet per frame and one truth line for my own body per frame
// ([pup] / [pup-truth], the REMOTE ERROR VS TRUTH row's input);
// SM64DS_PUPPET_BUF_MS=<n> pins the buffer B (ms); SM64DS_PUPPET_SOLID=1 gives
// a puppet's body the solid-obstacle bit (pup_cylinder).
// ---------------------------------------------------------------------------
enum : int {
    kPupBufMin = 50,          // B at 30 Hz: 1.5 send intervals (plan 2.5)
    kPupBufMax = 100,
    kPupReckonMs = 150,
    kPupOffN = 10,            // samples the clock offset's minimum looks at
    kPupSwapAfter = 2,        // driven frames before a character swap
    kPupSwapEvery = 60,       // frames between two swap attempts
    kPlayerSound = 0x74,      // Player+0x74: the camera-space vector Player
                              // sounds are played at
    kPlayerBodyClsn = 0x2d4,  // mdCcAcPos_c
    kPlayerAttackClsn = 0x314,
    kPlayerBodyClsnOn = 0x713,
    kStLevelEnter = 51,       // state_table.json: St_LevelEnter
    kStInYoshiMouth = 5,      // state_table.json: St_InYoshiMouth
    kPlayerEatenBy = 0xd0,    // Player mEatingPlayer: whose mouth this body is in
    kPupMouthTolMs = 400,     // 12 frames: pup_eater_confirms' window
    kPlayerMouthObj = 0x360,  // Player mObjInMouth: the actor in this body's mouth
    kPlayerOpacity = 0x6f5,   // Player mOpacity (0 = Player::Render draws nothing)
    kClassMarioCap = 0x10d,   // daObjMarioCap_c
    kPupPutOnMinDriven = 30,  // a cap seen within this many frames of the take-over is not a put-on
    kPupIntroMaxFrames = 900, // 30 s: take over a seat that never leaves the intro
};

struct PupDraw {
    int x, y, z;
    short yaw;
    unsigned short anim;
    int cursor;               // 20.12
    unsigned char look;       // the character the owner's body wears
    unsigned char flags;
    unsigned char invuln;
    unsigned char power;
    unsigned char eaten_by;   // seat + 1 of the mouth this body is in, 0 = none
    unsigned char held;       // the owner's stream: seat + 1 of the player in its mouth / hands (or kArenaHeldObject)
    char mode;                // i interp, r reckon, h hold, t teleport hold,
                              // o older than the ring
    double ft;                // the sender frame the draw time maps to
    unsigned newest_frame;
    unsigned recv_ms;         // when the sample the draw stands on arrived
};

struct Pup {
    bool driven = false;
    unsigned n_driven = 0;
    unsigned epoch = ~0u;
    double buf_ms = kPupBufMin;
    bool sfx_init = false;
    unsigned sfx_frame = 0;
    unsigned swap_at = 0;
    unsigned long long frames = 0, interp = 0, reckon = 0, hold = 0, tele = 0,
                       older = 0, swaps = 0, sfx_played = 0, hits = 0;
    // VSPUP2: the face (eye) sequence, the tongue (head animation), anims the
    // ROM would refuse, and the puppet sounds the range kept out
    ghost_tongue::State tongue;            // POSE9: arg 0 / 1 / 2 while the head plays the tongue, else -1 (ghost_tongue.cpp)
    unsigned long long face_adv = 0,
                       anim_refused = 0, sfx_far = 0, sfx_near = 0;
    unsigned last_refused_id = 0xffffffffu;
    unsigned long long puton = 0, puff_changes = 0;
    unsigned puton_at = 0;                 // frame of the last put-on cap spawn (probe)
    // a puppet in somebody's mouth is not drawn (PUPEAT2)
    bool mouth_prev = false;               // in a mouth at the last drive
    unsigned long long mouth_frames = 0,   // frames driven in a mouth
                       mouth_drawn = 0,    // of them, frames the body was drawn
                       mouth_enters = 0,
                       mouth_unconfirmed = 0;   // stream says eaten, the named eater does not hold it
};
Pup g_pup[kArenaSeats];
struct PupCfg {
    int enabled = -1, log = -1, solid = -1, buf = -2, nohide = -1;
    // VSPUP2 (all default ON; the lab knobs below turn one fix off for a control row)
    int face = -1, tongue = -1, face_probe = -1, sfx_log = -1, puton = -1;
    double sfx_near = 90.0, sfx_far = 450.0;
    int sfx_range_on = -1;
} g_pc;
// my own sounds waiting for a free SYNS slot (a second sound of one kind in a
// frame goes out one frame later)
struct SfxQueue { unsigned char id[4]; int n = 0; } g_sq[2];
unsigned long long g_sfx_cap[2] = {0, 0}, g_sfx_drop = 0;

void pup_report();

// the length the ROM's own fade should see for a puppet sound at distance
// `dist` from my body: its full / none range is data_02099fa8 / data_02099fac
// (90 / 550); the near..far ramp is mapped onto it
double pup_sfx_romdist(double dist) {
    const double rn = (double)data_02099fa8, rf = (double)data_02099fac;
    if (dist <= g_pc.sfx_near) return dist < rn ? dist : rn;
    return rn + (dist - g_pc.sfx_near) * (rf - rn) / (g_pc.sfx_far - g_pc.sfx_near);
}

bool pup_enabled() {
    if (g_pc.enabled < 0) {
        const char *e = std::getenv("SM64DS_PUPPET");
        g_pc.enabled = !(e && e[0] == '0' && !e[1]);
    }
    return g_pc.enabled != 0;
}
bool pup_log() {
    if (g_pc.log < 0) g_pc.log = env_on("SM64DS_PUPPET_LOG") ? 1 : 0;
    return g_pc.log != 0;
}
bool pup_solid() {
    if (g_pc.solid < 0) g_pc.solid = env_on("SM64DS_PUPPET_SOLID") ? 1 : 0;
    return g_pc.solid != 0;
}
// LAB control: SM64DS_PUPPET_NOHIDE=1 draws an eaten puppet as build 4 did
bool pup_nohide() {
    if (g_pc.nohide < 0) g_pc.nohide = env_on("SM64DS_PUPPET_NOHIDE") ? 1 : 0;
    return g_pc.nohide != 0;
}
// LAB control: SM64DS_PUPPET_TRUSTEATEN=1 trusts a seat's own eaten_by with no
// word from the eater (the first PUPEAT2 rule)
bool pup_trust_eaten() {
    static int v = -1;
    if (v < 0) v = env_on("SM64DS_PUPPET_TRUSTEATEN") ? 1 : 0;
    return v != 0;
}
// VSPUP2 lab controls: SM64DS_PUPPET_FACE=0 (the build 5 face: SetAnim every
// frame, the eye sequence never advances), SM64DS_PUPPET_TONGUE=0 (no head
// animation), SM64DS_PUPPET_FACE_PROBE=1 (a [pup-face] line per puppet per second),
// SM64DS_PUPPET_SFX_LOG=1 (a [pup-sfx2] line per puppet sound),
// SM64DS_PUPPET_SFX_RANGE=<near>,<far> | off (off = build 5: the ROM's own
// camera-measured range)
bool pup_face_fix() {
    if (g_pc.face < 0) { const char *e = std::getenv("SM64DS_PUPPET_FACE"); g_pc.face = !(e && e[0] == '0' && !e[1]); }
    return g_pc.face != 0;
}
bool pup_tongue_fix() {
    if (g_pc.tongue < 0) { const char *e = std::getenv("SM64DS_PUPPET_TONGUE"); g_pc.tongue = !(e && e[0] == '0' && !e[1]); }
    return g_pc.tongue != 0;
}
bool pup_puton_fix() {   // SM64DS_PUPPET_PUTON=0: no cap put-on on a puppet (build 5)
    if (g_pc.puton < 0) { const char *e = std::getenv("SM64DS_PUPPET_PUTON"); g_pc.puton = !(e && e[0] == '0' && !e[1]); }
    return g_pc.puton != 0;
}
bool pup_face_probe() {
    if (g_pc.face_probe < 0) g_pc.face_probe = env_on("SM64DS_PUPPET_FACE_PROBE") ? 1 : 0;
    return g_pc.face_probe != 0;
}
bool pup_sfx_log() {
    if (g_pc.sfx_log < 0) g_pc.sfx_log = env_on("SM64DS_PUPPET_SFX_LOG") ? 1 : 0;
    return g_pc.sfx_log != 0;
}
void pup_sfx_cfg() {
    if (g_pc.sfx_range_on >= 0) return;
    g_pc.sfx_range_on = 1;
    const char *e = std::getenv("SM64DS_PUPPET_SFX_RANGE");
    if (!e || !*e) return;
    if (!std::strcmp(e, "off")) { g_pc.sfx_range_on = 0; return; }
    double n = 0, f = 0;
    if (std::sscanf(e, "%lf,%lf", &n, &f) == 2 && n >= 0 && f > n && f < 100000) {
        g_pc.sfx_near = n; g_pc.sfx_far = f;
    }
}
// LAB forge row: SM64DS_PUPPET_FORGE_EATEN=<frame>:<seat> makes THIS console
// report eaten_by = seat + 1 from that frame on, whatever its body does
void pup_lab_forge(ArenaState *s) {
    static int at = -2, seat = 0;
    if (at == -2) {
        const char *e = std::getenv("SM64DS_PUPPET_FORGE_EATEN");
        at = e && *e ? std::atoi(e) : -1;
        if (e) if (const char *c = std::strchr(e, ':')) seat = std::atoi(c + 1);
    }
    if (at >= 0 && (int)g.frame >= at && seat >= 0 && seat < kArenaSeats)
        s->eaten_by = (unsigned char)(seat + 1);
}
// A seat says it is in the mouth of `eater`: believed only while the eater's own
// stream says it holds that seat (held == seat + 1), at any sample that reached
// me within kPupMouthTolMs of the sample being drawn. The eater's hold leads the
// victim's eaten_by by the referee's round trip at the start and again at the
// spit, so the window is generous in time and narrow in what it needs: a
// console that claims a mouth nobody holds is simply drawn.
bool pup_eater_confirms(int eater_seat, int victim_seat, unsigned recv_ms) {
    if (eater_seat < 0 || eater_seat >= kArenaSeats) return false;
    ArenaSample es[kArenaRing];
    const int n = vs_arena_samples(eater_seat, es, kArenaRing);
    for (int i = 0; i < n; ++i) {
        const int dt = (int)(es[i].recv_ms - recv_ms);
        if (dt > kPupMouthTolMs || dt < -kPupMouthTolMs) continue;
        if (es[i].s.held == (unsigned char)(victim_seat + 1)) return true;
    }
    return false;
}
// GRABPOSE1: SM64DS_PUPPET_PUFF=0 turns the puffed-cheek puppet off (the base behaviour)
bool pup_puff_fix() {
    static int on = -1;
    if (on < 0) { const char *e = std::getenv("SM64DS_PUPPET_PUFF"); on = !(e && e[0] == '0' && !e[1]); }
    return on != 0;
}
// GRABPOSE1: SM64DS_PUPPET_CARRYLAYER=0 skips the carry layer call (the base behaviour)
bool pup_carry_layer_fix() {
    static int on = -1;
    if (on < 0) { const char *e = std::getenv("SM64DS_PUPPET_CARRYLAYER"); on = !(e && e[0] == '0' && !e[1]); }
    return on != 0;
}
int pup_buf_pin() {
    if (g_pc.buf == -2) g_pc.buf = env_int("SM64DS_PUPPET_BUF_MS", -1, 10, 400);
    return g_pc.buf;
}

inline int lerp_i(int a, int b, double num, double den) {
    return a + (int)((double)((long long)b - (long long)a) * num / den);
}

// The buffer B: 50 ms, raised to the seat's delivery jitter p95 + 33 ms when
// the link is noisy, never above 100 (plan 2.5). Moved by at most 1 ms a frame
// so a change never shows as a jump.
double pup_target_buf(const ArenaSample *sm, int n, int off) {
    if (n < 8) return kPupBufMin;
    int ex[kArenaRing];
    for (int i = 0; i < n; ++i) {
        const int e = (int)(sm[i].recv_ms -
                            (unsigned)((unsigned long long)sm[i].s.frame * 1000ull / 30ull)) - off;
        ex[i] = e < 0 ? 0 : e;
    }
    for (int i = 1; i < n; ++i) {               // insertion sort, n <= 32
        const int v = ex[i];
        int j = i - 1;
        while (j >= 0 && ex[j] > v) { ex[j + 1] = ex[j]; --j; }
        ex[j + 1] = v;
    }
    int b = ex[(n - 1) * 95 / 100] + 33;
    if (b < kPupBufMin) b = kPupBufMin;
    if (b > kPupBufMax) b = kPupBufMax;
    return b;
}

// The seat's body as the owner's console had it at draw time. False: nothing
// to draw yet.
bool pup_sample(int seat, unsigned now, Pup &pp, PupDraw *o, ArenaSample *ring,
                int *ring_n) {
    ArenaSeatView v;
    if (!vs_arena_seat_view(seat, &v) || !v.present || !v.has_state) return false;
    const int n = vs_arena_samples(seat, ring, kArenaRing);
    if (n <= 0) return false;
    *ring_n = n;
    // THE CLOCK OFFSET, tracked fast. A0's seat view keeps the lower envelope
    // over 8 s, which is right for a link but wrong for a SENDER that hitches
    // (its frame clock then falls behind the wall clock for good, and an 8 s
    // memory leaves the draw time that many frames ahead of what arrives: the
    // puppet dead-reckons off the newest packet for seconds, up to 200 units
    // off in the REMOTE ERROR row's tail). The offset used here is the minimum
    // over the newest kPupOffN samples (about 0.3 s at 30 Hz); the jitter that
    // sizes B is read against the minimum over the whole ring (about a second).
    long long lo_n = 0, lo_all = 0;
    for (int i = 0; i < n; ++i) {
        const long long e = (long long)ring[i].recv_ms -
                            (long long)((unsigned long long)ring[i].s.frame * 1000ull / 30ull);
        if (i == 0 || e < lo_all) lo_all = e;
        if (i < kPupOffN && (i == 0 || e < lo_n)) lo_n = e;
    }
    if (pup_buf_pin() >= 0) {
        pp.buf_ms = pup_buf_pin();
    } else {
        const double want = pup_target_buf(ring, n, (int)(unsigned)lo_all);
        if (pp.buf_ms < want) pp.buf_ms = pp.buf_ms + 1.0 > want ? want : pp.buf_ms + 1.0;
        else if (pp.buf_ms > want) pp.buf_ms = pp.buf_ms - 1.0 < want ? want : pp.buf_ms - 1.0;
    }
    const double ft = ((double)now - pp.buf_ms - (double)lo_n) * 30.0 / 1000.0;
    o->ft = ft;
    o->newest_frame = ring[0].s.frame;
    int ia = -1;
    for (int i = 0; i < n; ++i)
        if ((double)ring[i].s.frame <= ft) { ia = i; break; }
    if (ia < 0) {                          // older than anything held
        const ArenaState &s = ring[n - 1].s;
        o->x = s.x; o->y = s.y; o->z = s.z; o->yaw = s.yaw; o->anim = s.anim_id;
        o->cursor = (int)s.anim_cursor << 8; o->look = s.character;
        o->flags = s.flags; o->invuln = s.invuln; o->power = s.power;
        o->eaten_by = s.eaten_by;
        o->held = s.held;
        o->recv_ms = ring[n - 1].recv_ms;
        o->mode = 'o';
        return true;
    }
    const ArenaState &a = ring[ia].s;
    o->look = a.character;
    o->flags = a.flags;
    o->power = a.power;
    o->eaten_by = a.eaten_by;
    o->held = a.held;
    o->recv_ms = ring[ia].recv_ms;
    const double since = ft - (double)a.frame;           // frames past a
    const int inv = (int)a.invuln - (int)since;
    o->invuln = (unsigned char)(inv > 0 ? inv : 0);
    if (ia > 0) {
        const ArenaState &b = ring[ia - 1].s;
        if (b.flags & kArenaTeleport) {                   // stay on a until T reaches b
            o->x = a.x; o->y = a.y; o->z = a.z; o->yaw = a.yaw; o->anim = a.anim_id;
            o->cursor = (int)a.anim_cursor << 8;
            o->mode = 't';
            return true;
        }
        const double num = since, den = (double)(b.frame - a.frame);
        o->x = lerp_i(a.x, b.x, num, den);
        o->y = lerp_i(a.y, b.y, num, den);
        o->z = lerp_i(a.z, b.z, num, den);
        const short dy = (short)(b.yaw - a.yaw);          // the short way round
        o->yaw = (short)(a.yaw + (int)((double)dy * num / den));
        const int ac = (int)a.anim_cursor << 8, bc = (int)b.anim_cursor << 8;
        if (a.anim_id == b.anim_id && bc >= ac) {
            o->anim = a.anim_id;
            o->cursor = lerp_i(ac, bc, num, den);
        } else {
            // the cycle wrapped or the animation changed between the two: walk
            // back from b at speed 1.0 while that stays in b's animation,
            // otherwise carry a forward at speed 1.0
            const int back = (int)((den - num) * 4096.0);
            if (bc - back >= 0) { o->anim = b.anim_id; o->cursor = bc - back; }
            else { o->anim = a.anim_id; o->cursor = ac + (int)(num * 4096.0); }
        }
        o->mode = 'i';
        return true;
    }
    // PAST THE NEWEST SAMPLE: dead-reckon the ground axes for at most 150 ms at
    // its own per-frame step (y is held: a jump is a parabola), then hold.
    double ex = since;
    const double cap = kPupReckonMs * 30.0 / 1000.0;
    if (ex > cap) ex = cap;
    o->x = a.x; o->y = a.y; o->z = a.z; o->yaw = a.yaw; o->anim = a.anim_id;
    o->cursor = (int)a.anim_cursor << 8;
    if (!(a.flags & (kArenaTeleport | kArenaPaused)) && ex > 0.0) {
        o->x += (int)((double)((int)a.vx * 64) * ex);
        o->z += (int)((double)((int)a.vz * 64) * ex);
        o->cursor += (int)(ex * 4096.0);
        o->mode = 'r';
    } else {
        o->mode = 'h';
    }
    return true;
}

// dCc_c::Unlink (src/_ZN5dCc_c6UnlinkEv.cpp, which has no flat host name):
// take a node out of the active list, a no-op for one that is not in it.
void pup_unlink(char *node) {
    char *&prev = *(char **)(node + 0x28);
    char *&next = *(char **)(node + 0x2c);
    if (prev) *(char **)(prev + 0x2c) = next;
    else if ((char *)data_0209cee8 == node) data_0209cee8 = next;
    if (next) *(char **)(next + 0x28) = prev;
    prev = 0;
    next = 0;
}

// THE BODY CYLINDER, kept in the collision list by hand (see the banner). The
// radius and height are what Player::Behavior's first lines set; the cylinder
// is put at the actor with no offset (a dCcAcPos_c keeps its own position,
// written by SetPosRelativeToActor, it does not read the actor live).
void pup_cylinder(char *c, unsigned char power) {
    int scale = 0x1000;
    if (power & kArenaMega) scale = 0x3000;
    *(int *)(c + kPlayerBodyClsn + 0x04) = scale * 0x28;
    *(int *)(c + kPlayerBodyClsn + 0x08) = scale * 0x96;
    unsigned *fl = (unsigned *)(c + kPlayerBodyClsn + 0x18);
    if (power & kArenaMega) *fl |= 0x10u; else *fl &= ~0x10u;
    if (pup_solid()) *fl |= 4u;     // an immovable obstacle: pushes me, is not pushed
    *(unsigned char *)(c + kPlayerBodyClsnOn) = 1;
    static const int zero[3] = {0, 0, 0};
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + kPlayerBodyClsn, zero);
    // Unlink first: Process empties the list every frame, and a frame that did
    // not run it must not leave this node in the list when Update runs again
    // (a node linked to itself loops the pair pass for ever). Unlink of an
    // unlinked node is a no-op.
    pup_unlink(c + kPlayerBodyClsn);
    *fl &= ~1u;                     // not disabled
    _ZN5dCc_c5ClearEv(c + kPlayerBodyClsn);
    _ZN5dCc_c6UpdateEv(c + kPlayerBodyClsn);
    // the attack cylinder: off. Bit 0 of its flags makes Update return, and
    // nothing here calls Update on it; it is also taken out of the list.
    pup_unlink(c + kPlayerAttackClsn);
    *(unsigned *)(c + kPlayerAttackClsn + 0x18) |= 1u;
}

// The sound vector of a puppet: its position in camera space, what
// dActor_c::BeforeBehavior writes at +0x74 for a body that thinks.
void pup_cam_pos(char *c, const PupDraw &d) {
    int w[3] = {d.x, d.y, d.z};
    int out[3] = {0, 0, 0};
    MulVec3Mat4x3(w, data_0209b3ec, out);
    int *p = (int *)(c + kPlayerSound);
    p[0] = out[0]; p[1] = out[1]; p[2] = out[2];
}

// One puppet body, taken over and posed (see the banner).
void pup_drive(int seat, char *a, const PupDraw &d0, Pup &pp) {
    PupDraw d = d0;
    // LAB (VSPUP2 SetAnim row): SM64DS_PUPPET_ANIM_INJECT=<frame>:<id>[:<frames>] makes the
    // stream say animation <id> for <frames> (default 90) frames from <frame>: an id the
    // ROM's Player::SetAnim refuses in VS (below 0x11, from 0x72) must be skipped and logged.
    {
        static int inj_at = -2, inj_id = 0, inj_len = 90;
        if (inj_at == -2) {
            const char *e = std::getenv("SM64DS_PUPPET_ANIM_INJECT");
            inj_at = -1;
            if (e && *e) {
                int f = 0, id = 0, n = 90;
                if (std::sscanf(e, "%d:%d:%d", &f, &id, &n) >= 2) { inj_at = f; inj_id = id; inj_len = n > 0 ? n : 90; }
            }
        }
        if (inj_at >= 0 && (int)g.frame >= inj_at && (int)g.frame < inj_at + inj_len)
            d.anim = (unsigned short)inj_id;
    }
    // LAB (VSPUP2 face pictures): SM64DS_PUPPET_FACE_TURN=1 turns every puppet to face my body, so a
    // close-up always shows the face.
    {
        static int turn = -1;
        if (turn < 0) turn = env_on("SM64DS_PUPPET_FACE_TURN") ? 1 : 0;
        if (turn && g.seat >= 0 && g.seat < kArenaSeats && data_0209f394[g.seat]) {
            const char *me = (const char *)data_0209f394[g.seat];
            const double ddx = (double)(*(const int *)(me + 0x5c) - d.x), ddz = (double)(*(const int *)(me + 0x64) - d.z);
            d.yaw = (short)(int)(std::atan2(ddx, ddz) * 32768.0 / 3.14159265358979);
        }
    }
    // LAB (VSPUP2 tongue pictures): SM64DS_PUPPET_TONGUE_FREEZE=<n> holds the first puppet
    // that has driven <n> tongue frames on the sample it is posed from, so the end picture of a
    // run shows the tongue out whatever frame the run ends on.
    {
        static int frz_n = -2, frz_seat = -1;
        static PupDraw frz_d;
        if (frz_n == -2) { const char *e = std::getenv("SM64DS_PUPPET_TONGUE_FREEZE"); frz_n = e && *e ? std::atoi(e) : -1; }
        if (frz_n >= 0) {
            if (frz_seat == seat) d = frz_d;
            else if (frz_seat < 0 && pp.tongue.arg >= 0 && (int)pp.tongue.frames >= frz_n) {
                frz_seat = seat; frz_d = d;
                std::fprintf(stderr, "[pup-tongue] f%u seat %d held on its tongue frame (anim %u)\n", g.frame, seat, (unsigned)d.anim);
            }
        }
    }
    // BEHAVIOR SKIPPED, every frame (the scene pass clears the bit)
    *(a + 0x13) = (char)(*(a + 0x13) | 2);
    *player::pos_x(a) = d.x;
    *player::pos_y(a) = d.y;
    *player::pos_z(a) = d.z;
    *player::facing(a) = d.yaw;
    *(int *)(a + 0x98) = 0;                 // horizontal speed
    *(int *)(a + 0xa8) = 0;                 // vertical speed
    // THE LOOK: the character the owner's body wears (param1, +0x08), through
    // the game's own in-place swap, once the body has been driven a moment and
    // no more often than every 2 s. Cap morphs on the owner come through here.
    const unsigned want_look = d.look <= 3 ? d.look : 0u;
    const unsigned have_look = (unsigned)(*(int *)(a + 8) & 0xff);
    if (have_look != want_look && pp.n_driven >= (unsigned)kPupSwapAfter &&
        g.frame >= pp.swap_at) {
        pp.swap_at = g.frame + (unsigned)kPupSwapEvery;
        port_player_set_character(a, want_look);
        ++pp.swaps;
        // V2 THE PUT-ON. The owner's body put a cap on (its stream look went from
        // Yoshi to a cap character). On the owner's console the cap actor flies
        // to the body and plays its put-on model (the ROM's own type 0xa cap: it
        // follows the nearest player's position and angles and removes itself
        // when its animation ends, src/actors/daObjMarioCap_c.cpp func_ov002_020b7330
        // and func_ov002_020b7200). The swap above is the body half; this is the
        // cap half, spawned the way the ROM spawns it for a hat swap
        // (src/game/player/Player.cpp, the data_02092114 branch). Not the cap object on
        // the ground: who gets that cap is the referee's.
        if (pup_puton_fix() && have_look == 3u && want_look <= 2u &&
            pp.n_driven >= (unsigned)kPupPutOnMinDriven) {
            const int cpos[3] = {d.x, d.y, d.z};
            char *cap = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                kClassMarioCap, (want_look << 8) | 0xau, cpos, nullptr,
                (int)*(signed char *)(a + 0xcc), -1);
            ++pp.puton;
            pp.puton_at = g.frame;
            std::fprintf(stderr, "[puppet] f%u seat %d put-on: cap look %u spawned %s\n", g.frame, seat,
                         want_look, cap ? "ok" : "REFUSED");
        }
        std::fprintf(stderr, "[puppet] seat %d body look %u -> %u\n", seat,
                     have_look, want_look);
    }
    // THE CURSOR (comms_sync.cpp gw_follow documents all of this): SetAnim only
    // lands its start frame when the animation CHANGES; with the same id and a
    // different frame make the ROM's own reset first.
    int want_f = d.cursor >> 12;
    if (want_f < 0) want_f = 0;
    // V8 THE TONGUE. Yoshi's tongue is the HEAD model's animation, not the
    // body's: func_ov002_020d71ec sets the body anim (two ids from a table, and
    // 0x6c for the pull back) AND the head model's own animation together, and
    // nothing else ever selects it. The puppet's body is posed from the stream,
    // so when the stream says a tongue anim, make the ROM's own call once (it
    // sets both) and then follow the body frame with the head frame; leaving
    // the anims puts the head back (func_ov002_020d71a0).
    const bool is_yoshi = (*(int *)(a + 8) & 0xff) == 3u;
    // GRABPOSE1 THE CHEEKS. A Yoshi with a player in its mouth wears the puffed-cheek
    // body model: the ROM sets Player+0x714 (mUseAltBodyModel) at frame 0xc of the swallow
    // (src/_ZN6Player18St_YoshiPower_MainEv.cpp case 1) and Player::GetBodyModelID answers
    // model 3 + 4 for a Yoshi while it is set (src/game/player/Player.cpp:1241); only a
    // spit / hurt / escape clears it. A puppet runs no states, so nothing set it: the eater
    // looked as if it held nothing. The eater's own stream says whose mouth it holds
    // (held = seat + 1, sampled from mObjInMouth), the one clock every console shares. On the
    // victim's console wait for the referee's EATEN (my body's +0xd0 names this puppet), so a
    // DENY never puffs a cheek. Set before the SetAnim below, which installs the pose on
    // whichever model GetBodyModelID now names.
    if (is_yoshi) {
        bool puff = false;
        if (pup_puff_fix() && d.held >= 1 && d.held <= kArenaSeats) {
            const int vseat = (int)d.held - 1;
            char *const vb = (vseat == g.seat && g.seat < kArenaSeats) ? (char *)data_0209f394[g.seat] : nullptr;
            puff = vseat != seat && (vseat != g.seat || (vb && *(char **)(vb + kPlayerEatenBy) == a));
        }
        unsigned char &alt = *(unsigned char *)(a + 0x714);
        if ((alt != 0) != puff) {
            std::fprintf(stderr, "[puppet] f%u seat %d cheeks %s (holds seat %d)\n", g.frame, seat,
                         puff ? "puffed" : "relaxed", (int)d.held - 1);
            ++pp.puff_changes;
        }
        alt = puff ? 1 : 0;
    }
    {
        bool started = false;
        const int targ = ghost_tongue::select(pp.tongue, a, d.anim, is_yoshi && pup_tongue_fix(), &started);
        if (started && (pup_log() || pup_sfx_log()))
            std::fprintf(stderr, "[puppet] f%u seat %d tongue %d (anim %u)\n", g.frame, seat,
                         targ, (unsigned)d.anim);
    }
    // LAB: SM64DS_PUPPET_PUTON_PROBE=1, for 90 frames after a put-on cap spawn a line every 6
    // frames: this puppet's body and every cap actor (position, type +0x3f0, holder +0x3bc, follows +0x3c0)
    {
        static int pr = -1;
        if (pr < 0) pr = env_on("SM64DS_PUPPET_PUTON_PROBE") ? 1 : 0;
        if (pr && pp.puton_at && g.frame >= pp.puton_at && g.frame < pp.puton_at + 90 && (g.frame % 6u) == 0) {
            std::fprintf(stderr, "[pup-puton] f%u seat %d body %d,%d,%d look=%d anim=%u opacity=%u\n", g.frame, seat,
                         d.x >> 12, d.y >> 12, d.z >> 12, *(int *)(a + 8) & 0xff, (unsigned)d.anim,
                         (unsigned)*(unsigned char *)(a + kPlayerOpacity));
            char *c = nullptr;
            for (int guard = 0; guard < 32; ++guard) {
                c = _ZN8dActor_c15FindWithActorIDEjPS_((unsigned)kClassMarioCap, c);
                if (!c) break;
                std::fprintf(stderr, "[pup-puton]   cap type=%d at %d,%d,%d holder=%08x follows=%08x\n", *(int *)(c + 0x3f0),
                             *(int *)(c + 0x5c) >> 12, *(int *)(c + 0x60) >> 12, *(int *)(c + 0x64) >> 12,
                             *(unsigned *)(c + 0x3bc), *(unsigned *)(c + 0x3c0));
            }
        }
    }
    // V9 THE FACE. Player::SetAnim also restarts the face (eye) texture sequence
    // at frame 0 (TextureSequence::SetFile ... 0), and the puppet called it EVERY
    // frame, so the sequence never left frame 0 and never advanced. Keep its frame
    // across the call and step it once, the way Player_AdvanceAnims does for a
    // body that thinks. A change of animation still restarts it, as in the ROM.
    const int face_idx = *(int *)(a + 8);
    char *const face_seq = (face_idx >= 0 && face_idx <= 3) ? a + 0x1dc + face_idx * 0x14 : nullptr;
    const bool same_anim = player::anim_id(a) == d.anim;
    const int face_saved = face_seq ? *(int *)(face_seq + 8) : 0;
    {
        int *cur = player::anim_frame_ptr(a);
        if (same_anim && cur && (*cur >> 12) != want_f)
            if (void *m = player::body_model_anim(a)) *(int *)((char *)m + 0x60) = 0;
    }
    // Player::SetAnim refuses ids below 0x11 or from 0x72 in a VS level (it
    // sets a bit in +0x6ce and returns): the previous pose stays. Say so once
    // per id instead of letting a stream value we cannot play do it silently.
    static int guard_on = -1;      // LAB: SM64DS_PUPPET_ANIM_GUARD=0 = build 5 (the ROM refuses it by itself, silently)
    if (guard_on < 0) { const char *e = std::getenv("SM64DS_PUPPET_ANIM_GUARD"); guard_on = !(e && e[0] == '0' && !e[1]); }
    if (guard_on && (d.anim < 0x11u || d.anim >= 0x72u)) {
        ++pp.anim_refused;
        if (pp.last_refused_id != d.anim) {
            pp.last_refused_id = d.anim;
            std::fprintf(stderr, "[puppet] f%u seat %d anim %u is outside what Player::SetAnim "
                         "accepts in VS (0x11..0x71): pose kept\n", g.frame, seat, (unsigned)d.anim);
        }
    } else {
        _ZN6Player7SetAnimEji5Fix12IiEj(a, d.anim, 0, 0x1000, (unsigned)(want_f & 0xffff));
        if (face_seq && pup_face_fix() && same_anim) {
            *(int *)(face_seq + 8) = face_saved;
            static int force = -2;      // LAB: SM64DS_PUPPET_FACE_FORCE=<n> pins the face sequence at frame n
            if (force == -2) { const char *e = std::getenv("SM64DS_PUPPET_FACE_FORCE"); force = e && *e ? std::atoi(e) : -1; }
            const unsigned len = *(unsigned *)(face_seq + 4) & ~0xc0000000u;
            if (force >= 0 && len > 0) {
                *(int *)(face_seq + 8) = ((unsigned)force % (len >> 12 ? len >> 12 : 1u)) << 12;
            } else
            if (len > 0) {
                static const unsigned kStill[] = {0x9a, 0x19, 0xac, 0x6b, 0x85, 0x94, 0x98, 0x99};
                for (unsigned id : kStill)
                    if (_ZN6Player6IsAnimEj(a, id)) { *(int *)(face_seq + 8) = 0; break; }
                _ZN15dExtFrameCtrl_c7AdvanceEv(face_seq);
                ++pp.face_adv;
            }
        }
    }
    int frames = 0;
    if (char *m = (char *)player::body_model_anim(a)) {
        frames = (*(int *)(m + 0x54) & ~(int)0xc0000000) >> 12;
        if (frames > 0) *(int *)(m + 0x58) = (want_f % frames) << 12;
    }
    if (frames <= 0) return;                // nothing to pose
    {
        int hframes = 0;
        const int hf = ghost_tongue::follow(pp.tongue, a, want_f, &hframes);
        static int tp = -1;       // LAB: SM64DS_PUPPET_TONGUE_PROBE=1, a line per tongue frame
        if (hf >= 0) {
            if (tp < 0) tp = env_on("SM64DS_PUPPET_TONGUE_PROBE") ? 1 : 0;
            if (tp)
                std::fprintf(stderr, "[pup-tongue] f%u seat=%d arg=%d body_anim=%u body_frame=%d/%d head_frame=%d/%d\n",
                             g.frame, seat, pp.tongue.arg, (unsigned)d.anim, want_f % frames, frames,
                             hf, hframes);
        }
    }
    if (pup_face_probe() && (g.frame % 30u) == 0 && face_seq)
        std::fprintf(stderr, "[pup-face] f%u seat=%d look=%d idx6db=%u anim=%u same=%d "
                     "seq_len=%u seq_cur=%d seq_spd=%d t73c=%04x gate_bea7c=%d hid=%u op=%u "
                     "tongue=%d\n", g.frame, seat, face_idx, (unsigned)*(unsigned char *)(a + 0x6db),
                     (unsigned)d.anim, same_anim ? 1 : 0, *(unsigned *)(face_seq + 4) & ~0xc0000000u,
                     *(int *)(face_seq + 8) >> 12, *(int *)(face_seq + 0xc),
                     (unsigned)*(unsigned short *)(a + 0x73c), _ZN6Player19func_ov002_020bea7cEv(a),
                     _ZN6Player19func_ov002_020becf4Eii(a, (unsigned)*(unsigned char *)(a + 0x6db), 1),
                     (unsigned)*(unsigned char *)(a + kPlayerOpacity), pp.tongue.arg);
    // fully opaque to the ROM's own gates, not culled (BeforeBehavior is not
    // writing the cull bits for a body that does not think) EXCEPT while it is
    // in somebody's mouth: the ROM hides an eaten player once (func_ov002_020d5ed0
    // sets mOpacity 0 at the tongue's hit), and this puppet's own Behavior, the
    // only thing that would keep it hidden, never runs. Opacity 0 also lifts the
    // drop shadow (func_ov002_020e444c below sets the latch when opacity and
    // vanish are both 0).
    // In a mouth = the owner's stream says so (eaten_by names the eater's seat),
    // on every console but the victim's own, which has a live body. On the
    // EATER's console the stream is the wrong clock: the ROM eats the puppet at
    // once (about 130 ms before the referee's verdict and before the stream
    // shows it) and lets it go at the spit, a round trip before the victim's
    // stream stops saying eaten. There my own body's mouth (mObjInMouth, +0x360)
    // is the truth: hidden exactly while it holds this puppet.
    char *const mine_body = (g.seat >= 0 && g.seat < kArenaSeats) ? (char *)data_0209f394[g.seat] : nullptr;
    const bool in_my_mouth = mine_body && *(char **)(mine_body + kPlayerMouthObj) == a;
    const bool named_me = d.eaten_by && (int)d.eaten_by - 1 == g.seat;
    const bool named_other = d.eaten_by && !named_me;
    const bool confirmed = named_other &&
        (pup_trust_eaten() || pup_eater_confirms((int)d.eaten_by - 1, seat, d.recv_ms));
    if (named_other && !confirmed) ++pp.mouth_unconfirmed;
    const bool in_mouth = in_my_mouth || confirmed;
    // the transition frames (the ROM's own pull in and spit out) are the ROM's
    // to draw; a frame counts only when the puppet was in a mouth at the last
    // drive and still is
    if (pp.n_driven > 0 && pp.mouth_prev && in_mouth) {
        // what the last frame drew: this is read before it is overwritten
        ++pp.mouth_frames;
        if (*(unsigned char *)(a + kPlayerOpacity) != 0 && !(*(unsigned *)(a + 0xb0) & 0x10u)) {
            ++pp.mouth_drawn;
            if (pp.mouth_drawn <= 8)
                std::fprintf(stderr, "[puppet] f%u seat %d drawn in a mouth: opacity %u flags %08x "
                             "state %d eaten_by %u d0 %s mine %d\n", g.frame, seat,
                             (unsigned)*(unsigned char *)(a + kPlayerOpacity),
                             *(unsigned *)(a + 0xb0), state_index_of(a), (unsigned)d.eaten_by,
                             *(int *)(a + kPlayerEatenBy) ? "set" : "clear", in_my_mouth ? 1 : 0);
        }
    }
    if (in_mouth != pp.mouth_prev) {
        if (in_mouth) ++pp.mouth_enters;
        std::fprintf(stderr, "[puppet] f%u seat %d %s a mouth (eaten_by %u, d0 %s)\n", g.frame,
                     seat, in_mouth ? "into" : "out of", (unsigned)d.eaten_by,
                     *(int *)(a + kPlayerEatenBy) ? "set" : "clear");
        pp.mouth_prev = in_mouth;
    }
    if (in_mouth && !pup_nohide()) {
        *(unsigned char *)(a + kPlayerOpacity) = 0;
        *(unsigned *)(a + 0xb0) &= ~0x28u;
    } else {
        *(unsigned char *)(a + kPlayerOpacity) = 0x1f;
        *(unsigned *)(a + 0xb0) &= ~0x38u;
    }
    // THE BLINK: the draw gate reads the invincibility timer; the owner's value
    // at draw time reproduces its flicker
    *(unsigned short *)(a + 0x6a0) = d.invuln;
    if (d.flags & kArenaGrounded) *(int *)(a + 0x644) = d.y;
    *(unsigned char *)(a + 0x717) = 0;
    if (d.power & kArenaVanish) *(unsigned char *)(a + 0x6fb) = 1;
    else *(unsigned char *)(a + 0x6fb) = 0;
    // PLACE, THEN POSE, in Player::Behavior's own order
    func_ov002_020e444c(a);
    // GRABPOSE1: Player::Behavior's tail (func_ov002_020e4bb8) runs func_ov002_020e4990 between the
    // placement and the pose: it switches the upper-body carry layer (model layer 8) off and, while
    // +0x358 holds something and the body anim is a carry anim, back on. A puppet skipped it, so the
    // layer vs_arena_grab loads onto a holder puppet (puppet_carry_overlay) was never shown.
    if (pup_carry_layer_fix()) func_ov002_020e4990(a);
    func_ov002_020e4768(a);
    // the render latches, the tail of func_ov002_020e4bb8 (GHOSTLOOK1 A)
    *(unsigned char *)(a + 0x6db) = (unsigned char)*(int *)(a + 8);
    *(unsigned char *)(a + 0x6fe) = *(unsigned char *)(a + 0x6fd);
    *(unsigned char *)(a + 0x6fa) = *(unsigned char *)(a + 0x6f9);
    *(unsigned char *)(a + 0x6fc) = *(unsigned char *)(a + 0x6fb);
    *(unsigned char *)(a + 0x700) = *(unsigned char *)(a + 0x6ff);
    *(unsigned char *)(a + 0x701) = (unsigned char)_ZN6Player8HasNoCapEv(a);
    pup_cam_pos(a, d);
    pup_cylinder(a, d.power);
    ++pp.n_driven;
}

// A puppet's sounds: every sample whose frame the drawn timeline has reached
// since the last pass, once.
//
// V5 NEAR ONLY. The ROM plays a sound at a vector in camera space and fades it
// by that vector's length (full under data_02099fa8 = 90, none from
// data_02099fac = 550), so the listener is the CAMERA, which sits well behind
// the player. A puppet's sound is judged here from MY body instead: farther
// than the far range (450) it is not played at all, nearer it is handed to the
// ROM with the same direction (the left / right pan) and a length that makes
// the ROM's own fade reproduce the near..far ramp from my body.
void pup_play_sounds(int seat, char *a, const PupDraw &d, Pup &pp,
                     const ArenaSample *ring, int n) {
    if (!pp.sfx_init) {
        // start at what is already behind the timeline: no replay of history
        pp.sfx_init = true;
        pp.sfx_frame = 0;
        for (int i = 0; i < n; ++i)
            if ((double)ring[i].s.frame <= d.ft) { pp.sfx_frame = ring[i].s.frame; break; }
        return;
    }
    pup_sfx_cfg();
    const char *const mine = (g.seat >= 0 && g.seat < kArenaSeats) ? (const char *)data_0209f394[g.seat] : nullptr;
    for (int i = n - 1; i >= 0; --i) {          // oldest first
        const ArenaState &s = ring[i].s;
        if (s.frame <= pp.sfx_frame) continue;
        if ((double)s.frame > d.ft) break;
        pp.sfx_frame = s.frame;
        for (int k = 0; k < 2; ++k) {
            if (!s.sfx[k]) continue;
            int vec[3];
            std::memcpy(vec, a + kPlayerSound, sizeof vec);
            double dist = -1.0, gain = 1.0, romd = -1.0;
            if (!g_pc.sfx_range_on && mine && pup_sfx_log()) {   // range off (build 5): measure for the log only
                const int me[3] = {*(const int *)(mine + 0x5c), *(const int *)(mine + 0x60),
                                   *(const int *)(mine + 0x64)};
                const int src[3] = {d.x, d.y, d.z};
                dist = vs_puppet_sfx_distance(me, src);
            }
            if (g_pc.sfx_range_on && mine) {
                const int me[3] = {*(const int *)(mine + 0x5c), *(const int *)(mine + 0x60),
                                   *(const int *)(mine + 0x64)};
                const int src[3] = {d.x, d.y, d.z};
                dist = vs_puppet_sfx_distance(me, src);
                gain = vs_puppet_sfx_gain(me, src);
                if (dist >= g_pc.sfx_far) {
                    ++pp.sfx_far;
                    if (pup_sfx_log())
                        std::fprintf(stderr, "[pup-sfx2] skip seat=%d f=%u kind=%d id=%u mydist=%.0f "
                                     "gain=0.00 (beyond %.0f)\n", seat, s.frame, k, (unsigned)s.sfx[k],
                                     dist, g_pc.sfx_far);
                    continue;
                }
                // the length the ROM should see: same direction, a length whose
                // ROM fade equals the near..far ramp from my body
                romd = pup_sfx_romdist(dist);
                const double tx = (double)(vec[0] >> 12), ty = (double)(vec[1] >> 12) * 2.0,
                             tz = (double)(vec[2] >> 12);
                const double len = std::sqrt(tx * tx + ty * ty + tz * tz);
                if (len >= 1.0) {
                    const double f = romd / len;
                    vec[0] = (int)(vec[0] * f); vec[1] = (int)(vec[1] * f); vec[2] = (int)(vec[2] * f);
                } else {
                    vec[0] = 0; vec[1] = 0; vec[2] = (int)(romd * 4096.0);
                }
                ++pp.sfx_near;
            }
            ++pp.sfx_played;
            if (pup_log()) {
                std::fprintf(stderr, "[pup-sfx] play seat=%d f=%u kind=%d id=%u "
                             "cam=%d,%d,%d\n", seat, s.frame, k, (unsigned)s.sfx[k],
                             vec[0] >> 12, vec[1] >> 12, vec[2] >> 12);
            }
            if (pup_sfx_log()) {
                const double tx = (double)(vec[0] >> 12), ty = (double)(vec[1] >> 12) * 2.0,
                             tz = (double)(vec[2] >> 12);
                std::fprintf(stderr, "[pup-sfx2] play seat=%d f=%u kind=%d id=%u mydist=%.0f gain=%.2f "
                             "romvec_len=%.0f\n", seat, s.frame, k, (unsigned)s.sfx[k], dist, gain,
                             std::sqrt(tx * tx + ty * ty + tz * tz));
            }
            _ZN5Sound4PlayEjjRK7Vector3((unsigned)k, s.sfx[k], vec);
        }
    }
}

// One game frame's worth of puppet work, right before the actor walk.
void pup_before_walk() {
    if (!pup_enabled() || g.seat < 0 || data_0209f2d8 != 1) return;
    {
        // LAB (VSPUP2 rows): SM64DS_PUPPET_SFX_RIG=<frame> makes MY body play one effect
        // (kind 0, id 65) and one character voice (kind 1, id 252) at itself every 24
        // frames from that frame, so a puppet sound is always in the stream.
        static int rig = -2;
        if (rig == -2) { const char *e = std::getenv("SM64DS_PUPPET_SFX_RIG"); rig = e && *e ? std::atoi(e) : -1; }
        if (rig >= 0 && (int)g.frame >= rig && ((int)g.frame - rig) % 24 == 0 && g.seat < kArenaSeats &&
            data_0209f394[g.seat]) {
            char *me = (char *)data_0209f394[g.seat];
            _ZN5Sound4PlayEjjRK7Vector3(0u, 65u, me + kPlayerSound);
            _ZN5Sound4PlayEjjRK7Vector3(1u, 252u, me + kPlayerSound);
        }
    }
    if (pup_sfx_log()) {
        static bool table_done = false;      // the distance rule at 50 / 200 / 400 / 600 units, once
        if (!table_done) {
            table_done = true;
            pup_sfx_cfg();
            static const int dists[] = {50, 200, 400, 600};
            for (int i = 0; i < 4; ++i) {
                const int me[3] = {0, 0, 0}, src[3] = {dists[i] << 12, 0, 0};
                const double dd = vs_puppet_sfx_distance(me, src);
                std::fprintf(stderr, "[pup-sfx2] table dist=%d gain=%.2f %s romvec_len=%.0f (rule %.0f..%.0f, range %s)\n",
                             dists[i], vs_puppet_sfx_gain(me, src), dd >= g_pc.sfx_far ? "SKIPPED" : "played",
                             dd >= g_pc.sfx_far ? -1.0 : pup_sfx_romdist(dd), g_pc.sfx_near, g_pc.sfx_far,
                             g_pc.sfx_range_on ? "on" : "OFF");
            }
        }
    }
    // my own sounds that did not fit in last frame's SYNS go in this one's
    for (int k = 0; k < 2; ++k)
        if (g_sq[k].n > 0 && !g.sfx[k]) {
            g.sfx[k] = g_sq[k].id[0];
            for (int i = 1; i < g_sq[k].n; ++i) g_sq[k].id[i - 1] = g_sq[k].id[i];
            --g_sq[k].n;
            g.nsfx = 2;
            g.send_now = true;
        }
    // THE RING AT THIS INSTANT. A0 drains the hub once a frame, at the end of
    // the tick, so a puppet drawn here would read a ring that is up to a whole
    // frame stale: the sample that arrived since is in the hub's queue, and the
    // draw time (which counts from the packet's own arrival stamp) sits ahead of
    // every sample the ring holds (the REMOTE ERROR row: lag 0.5 frame behind
    // ft in 'r' mode at every rtt, B never in play). Drain again first; an
    // empty queue is a no-op.
    drain();
    const unsigned now = comms_hub_now_ms();
    if (pup_log() && g.seat < kArenaSeats && data_0209f394[g.seat] && g.have_last) {
        // THE TRUTH LINE: my own body as the SYNS for frame g.frame carried it
        // (this runs before the walk, so the body is where the tick left it)
        const char *b = (const char *)data_0209f394[g.seat];
        std::fprintf(stderr, "[pup-truth] t=%u f=%u pos=%d,%d,%d yaw=%d anim=%u\n",
                     now, g.frame, *(const int *)(b + 0x5c), *(const int *)(b + 0x60),
                     *(const int *)(b + 0x64), (int)*(const short *)(b + 0x8e),
                     (unsigned)player::anim_id(b));
    }
    // R1 PROBE (plan section 9) and the lab fixture beside it. What the pair
    // pass (CylinderClsn::Process, in Stage::Render) wrote on the bodies last
    // frame is still on them here: the body cylinder's hitFlags +0x2f4 and
    // otherOwner +0x2f8 (dCc_c +0x20 / +0x24), and its pushback +0x2e0.
    // SM64DS_PUPPET_NEAR=<frame>[:<units>] puts MY body <units> (default 30)
    // beside the first driven puppet once at that frame, so two real bodies
    // overlap and the pair pass has something to do.
    if (pup_log() && g.seat < kArenaSeats && data_0209f394[g.seat]) {
        const char *b = (const char *)data_0209f394[g.seat];
        const unsigned hf = *(const unsigned *)(b + kPlayerBodyClsn + 0x20);
        const unsigned ow = *(const unsigned *)(b + kPlayerBodyClsn + 0x24);
        const int *pb = (const int *)(b + kPlayerBodyClsn + 0x0c);
        if (hf || ow || pb[0] || pb[1] || pb[2])
            std::fprintf(stderr, "[pup-hit] f=%u me seat=%d body hitflags=%08x owner=%u "
                         "push=%d,%d,%d\n", g.frame, g.seat, hf, ow, pb[0], pb[1], pb[2]);
    }
    {
        static int near_at = -2, near_units = 30, near_dz = 0;
        static bool near_done = false;
        if (near_at == -2) {
            const char *e = std::getenv("SM64DS_PUPPET_NEAR");
            near_at = e && *e ? std::atoi(e) : -1;
            if (e) if (const char *c = std::strchr(e, ':')) {
                near_units = std::atoi(c + 1);
                if (const char *c2 = std::strchr(c + 1, ':')) near_dz = std::atoi(c2 + 1);   // VSPUP2: optional dz
            }
        }
        if (near_at >= 0 && !near_done && (int)g.frame >= near_at && g.seat < kArenaSeats &&
            data_0209f394[g.seat]) {
            for (int k = 0; k < g.bodies && k < kArenaSeats; ++k) {
                if (k == g.seat || !g_pup[k].driven || !data_0209f394[k]) continue;
                char *me = (char *)data_0209f394[g.seat];
                const char *pk = (const char *)data_0209f394[k];
                *(int *)(me + 0x5c) = *(const int *)(pk + 0x5c) + (near_units << 12);
                *(int *)(me + 0x60) = *(const int *)(pk + 0x60);
                *(int *)(me + 0x64) = *(const int *)(pk + 0x64) + (near_dz << 12);
                near_done = true;
                std::fprintf(stderr, "[pup-near] f=%u my body placed %d units from the "
                             "puppet of seat %d\n", g.frame, near_units, k);
                break;
            }
        }
    }
    // LAB (VSPUP2 put-on row): SM64DS_PUPPET_CAPRIG=<frame> puts MY body on the first cap in the
    // level (the ROM's own touch then runs the pickup and the hat morph), once.
    {
        static int cap_at = -2, cap_hold = 0, cap_seen = 0;
        static int cap_pos[3];
        static bool cap_done = false;
        if (cap_at == -2) { const char *e = std::getenv("SM64DS_PUPPET_CAPRIG"); cap_at = e && *e ? std::atoi(e) : -1; }
        if (cap_at >= 0 && g.seat >= 0 && g.seat < kArenaSeats && data_0209f394[g.seat]) {
            char *me = (char *)data_0209f394[g.seat];
            static const bool swap_rig = env_on("SM64DS_PUPPET_CAPRIG_SWAP");
            if (swap_rig && !cap_done && (int)g.frame >= cap_at) {
                // the ROM's own character swap (the door path): the stream look goes 3 -> 0 as it does on a real pickup
                cap_done = true;
                port_player_set_character(me, 0);
                std::fprintf(stderr, "[pup-caprig] f=%u my body swapped to look %d (swap rig)\n", g.frame, *(int *)(me + 8) & 0xff);
            }
            if (!swap_rig && !cap_done && (int)g.frame >= cap_at) {
                char *cap = nullptr;
                for (int guard = 0; guard < 64; ++guard) {
                    cap = _ZN8dActor_c15FindWithActorIDEjPS_((unsigned)kClassMarioCap, cap);
                    if (!cap || *(int *)(cap + 0x3f0) == 4) break;    // a placed (type 4) cap
                }
                if (cap && *(int *)(cap + 0x3f0) == 4) {
                    cap_done = true;
                    cap_hold = 12;      // a one-frame contact can be missed: stay on it for 12 frames
                    for (int i = 0; i < 3; ++i) cap_pos[i] = *(int *)(cap + 0x5c + 4 * i);
                    std::fprintf(stderr, "[pup-caprig] f=%u my body put on the cap at %d,%d,%d (look %d)\n", g.frame,
                                 cap_pos[0] >> 12, cap_pos[1] >> 12, cap_pos[2] >> 12, *(int *)(me + 8) & 0xff);
                }
            }
            if (cap_hold > 0) {
                --cap_hold;
                for (int i = 0; i < 3; ++i) *(int *)(me + 0x5c + 4 * i) = cap_pos[i];
                *(int *)(me + 0x98) = 0;
                *(int *)(me + 0xa8) = 0;
            }
            if (cap_done && cap_seen < 400 && (int)g.frame % 6 == 0) {
                ++cap_seen;
                std::fprintf(stderr, "[pup-caprig] f=%u look=%d state=%d\n", g.frame, *(int *)(me + 8) & 0xff,
                             vs_arena_state_index(me));
            }
        }
    }
    // LAB (VSPUP2 pictures): SM64DS_PUPPET_LOOK=<frame>[:<dx>:<dz>] pins MY body
    // from that frame on, every frame, and turns it to face the first driven
    // puppet, so my camera settles on a puppet that stands in front of it. With
    // dx / dz (units) my body is first placed at puppet + (dx, dz); without them it
    // stays where it stands.
    {
        static int look_at = -2, look_dx = 0, look_dz = 0, look_yaw = 0;
        static bool look_off = false, look_set = false;
        static int look_pos[3];
        if (look_at == -2) {
            const char *e = std::getenv("SM64DS_PUPPET_LOOK");
            look_at = e && *e ? std::atoi(e) : -1;
            if (e) if (const char *c = std::strchr(e, ':')) {
                look_off = std::sscanf(c + 1, "%d:%d:%d", &look_dx, &look_dz, &look_yaw) >= 2;   // optional :yaw (degrees) turns my body off the puppet, for a side view
            }
        }
        int pk = -1;
        for (int k = 0; k < g.bodies && k < kArenaSeats; ++k)
            if (k != g.seat && g_pup[k].driven && data_0209f394[k]) { pk = k; break; }
        if (look_at >= 0 && (int)g.frame >= look_at && g.seat < kArenaSeats && data_0209f394[g.seat] && pk >= 0) {
            char *me = (char *)data_0209f394[g.seat];
            const char *pb = (const char *)data_0209f394[pk];
            if (!look_set) {
                look_set = true;
                for (int i = 0; i < 3; ++i) look_pos[i] = *(int *)(me + 0x5c + 4 * i);
                if (look_off) {
                    look_pos[0] = *(const int *)(pb + 0x5c) + (look_dx << 12);
                    look_pos[1] = *(const int *)(pb + 0x60);
                    look_pos[2] = *(const int *)(pb + 0x64) + (look_dz << 12);
                }
                std::fprintf(stderr, "[pup-look] f=%u my body pinned at %d,%d,%d facing the puppet of seat %d\n", g.frame,
                             look_pos[0] >> 12, look_pos[1] >> 12, look_pos[2] >> 12, pk);
            }
            for (int i = 0; i < 3; ++i) *(int *)(me + 0x5c + 4 * i) = look_pos[i];
            *(int *)(me + 0x98) = 0;
            *(int *)(me + 0xa8) = 0;
            const double ddx = (double)(*(const int *)(pb + 0x5c) - look_pos[0]);
            const double ddz = (double)(*(const int *)(pb + 0x64) - look_pos[2]);
            const int yaw = (int)(std::atan2(ddx, ddz) * 32768.0 / 3.14159265358979) + (int)(look_yaw * 65536.0 / 360.0);
            *(short *)(me + 0x8e) = (short)yaw;
            *(short *)(me + 0x94) = (short)yaw;
        }
    }
    for (int k = 0; k < g.bodies && k < kArenaSeats; ++k) {
        if (k == g.seat) continue;
        char *a = (char *)data_0209f394[k];
        if (!a || !data_0209fc5c[k] || g.ring[k].held) continue;
        Pup &pp = g_pup[k];
        // THE INTRO RUNS FIRST. The ROM's VS level-entry state (St_LevelEnter,
        // state_table.json idx 51) is released for the bodies by the level's own
        // start, and a joiner's own body never left it when slot 0 was held from
        // frame 0 (real entry path, ARENAGATE1 finding 1: St_LevelEnter for 1500+
        // frames, A0 alone does not do it). So a puppet is left to the ROM, with
        // its zero pad, until my own body AND this body are out of that state;
        // the hold starts at the first frame both are. A seat that never leaves
        // it (or a fixture without the intro) is taken over anyway after
        // kPupIntroMaxFrames.
        if (!pp.driven && g.frame < (unsigned)kPupIntroMaxFrames) {
            const int mine = state_index_of(data_0209f394[g.seat]);
            const int its = state_index_of(a);
            if (mine == kStLevelEnter || its == kStLevelEnter) {
                if (pup_log() && (g.frame % 30u) == 0)
                    std::fprintf(stderr, "[pup-intro] f=%u seat=%d waits: my state %d, its "
                                 "state %d\n", g.frame, k, mine, its);
                continue;
            }
        }
        if (pup_log() && pp.driven) {
            const unsigned hf = *(const unsigned *)(a + kPlayerBodyClsn + 0x20);
            const unsigned ow = *(const unsigned *)(a + kPlayerBodyClsn + 0x24);
            if (hf || ow) {
                ++pp.hits;
                std::fprintf(stderr, "[pup-hit] f=%u puppet seat=%d body hitflags=%08x "
                             "owner=%u\n", g.frame, k, hf, ow);
            }
        }
        ArenaSeatView sv;
        if (vs_arena_seat_view(k, &sv) && sv.join_epoch != pp.epoch) {
            // the seat's console (re)joined: its timeline starts again
            pp.epoch = sv.join_epoch;
            pp.sfx_init = false;
            pp.buf_ms = kPupBufMin;
        }
        ArenaSample ring[kArenaRing];
        int rn = 0;
        PupDraw d;
        if (!pup_sample(k, now, pp, &d, ring, &rn)) continue;
        pup_drive(k, a, d, pp);
        pup_play_sounds(k, a, d, pp, ring, rn);
        if (!pp.driven) {
            static bool reg = false;
            if (!reg) { reg = true; std::atexit(pup_report); }
            std::fprintf(stderr, "[puppet] f%u seat %d taken over: Behavior skipped, "
                         "body cylinder kept in the list by the puppet hold\n", g.frame, k);
        }
        pp.driven = true;
        ++pp.frames;
        switch (d.mode) {
            case 'i': ++pp.interp; break;
            case 'r': ++pp.reckon; break;
            case 't': ++pp.tele; break;
            case 'o': ++pp.older; break;
            default: ++pp.hold; break;
        }
        if (pup_log())
            std::fprintf(stderr, "[pup] t=%u f=%u seat=%d ft=%.3f mode=%c pos=%d,%d,%d "
                         "yaw=%d anim=%u cur=%d buf=%.0f newest=%u\n", now, g.frame, k,
                         d.ft, d.mode, d.x, d.y, d.z, (int)d.yaw, (unsigned)d.anim,
                         d.cursor >> 12, pp.buf_ms, d.newest_frame);
    }
}

// A sound my body asked for (the flat Sound::Play, hal/sdat/sound_abi.cpp).
void pup_note_sound(unsigned kind, unsigned id, const void *v) {
    if (!g.installed || g.seat < 0 || g.seat >= kArenaSeats || kind > 1 || !id ||
        id > 255 || !pup_enabled())
        return;
    const void *body = data_0209f394[g.seat];
    if (!body || v != (const char *)body + kPlayerSound) return;
    ++g_sfx_cap[kind];
    if (pup_log())
        std::fprintf(stderr, "[pup-sfx] cap f=%u kind=%u id=%u\n", g.frame, kind, id);
    if (!g.sfx[kind]) {
        g.sfx[kind] = (unsigned char)id;
    } else if (g_sq[kind].n < 4) {
        g_sq[kind].id[g_sq[kind].n++] = (unsigned char)id;
    } else {
        ++g_sfx_drop;
    }
    g.nsfx = 2;               // both slots are always read (A0's sample_body)
    g.send_now = true;        // a sound is never held for the 10 Hz rule
}

void pup_report() {
    for (int k = 0; k < kArenaSeats; ++k) {
        const Pup &p = g_pup[k];
        if (!p.frames) continue;
        std::fprintf(stderr, "[puppet] exit seat=%d frames=%llu interp=%llu reckon=%llu "
                     "hold=%llu teleport=%llu older=%llu swaps=%llu sfx_played=%llu hits=%llu "
                     "buf=%.0f\n", k, p.frames, p.interp, p.reckon, p.hold, p.tele,
                     p.older, p.swaps, p.sfx_played, p.hits, p.buf_ms);
        std::fprintf(stderr, "[puppet] exit seat=%d face_adv=%llu tongue_starts=%llu tongue_frames=%llu "
                     "anim_refused=%llu sfx_near=%llu sfx_far=%llu puton=%llu\n", k, p.face_adv,
                     p.tongue.starts, p.tongue.frames, p.anim_refused, p.sfx_near, p.sfx_far, p.puton);
        std::fprintf(stderr, "[puppet] exit seat=%d mouths=%llu mouth_frames=%llu "
                     "mouth_drawn=%llu unconfirmed=%llu\n", k, p.mouth_enters, p.mouth_frames,
                     p.mouth_drawn, p.mouth_unconfirmed);
    }
    std::fprintf(stderr, "[puppet] exit sfx_captured=%llu,%llu dropped=%llu\n",
                 g_sfx_cap[0], g_sfx_cap[1], g_sfx_drop);
}

}  // namespace

// ===========================================================================
// PUBLIC
// ===========================================================================
// V5: the distance rule for a remote player's character sounds. The ROM's own
// metric (src/func_02049018.c: x and z as they are, y doubled, in world units),
// measured from my body.
double vs_puppet_sfx_distance(const int me[3], const int src[3]) {
    const double dx = (double)((me[0] - src[0]) >> 12), dy = (double)((me[1] - src[1]) >> 12) * 2.0,
                 dz = (double)((me[2] - src[2]) >> 12);
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
float vs_puppet_sfx_gain(const int me[3], const int src[3]) {
    pup_sfx_cfg();
    if (!g_pc.sfx_range_on) return 1.0f;
    const double d = vs_puppet_sfx_distance(me, src);
    if (d <= g_pc.sfx_near) return 1.0f;
    if (d >= g_pc.sfx_far) return 0.0f;
    return (float)((g_pc.sfx_far - d) / (g_pc.sfx_far - g_pc.sfx_near));
}
// MPSOUND1 (party / coop / world-party sounds, hal/coop_sound.cpp): the same rule,
// read through the same config, so both paths fade alike. Nothing in VS calls these.
bool vs_puppet_sfx_rule(double *near_out, double *far_out) {
    pup_sfx_cfg();
    if (near_out) *near_out = g_pc.sfx_near;
    if (far_out) *far_out = g_pc.sfx_far;
    return g_pc.sfx_range_on != 0;
}
double vs_puppet_sfx_romdist(double dist) {
    pup_sfx_cfg();
    return pup_sfx_romdist(dist);
}

bool arena_state_valid(const ArenaState &s) {
    if (s.tag != kArenaTagState || s.version != 1) return false;
    if (s.seat >= kArenaSeats) return false;
    const int lim = kArenaPosLimit << 12;
    if (s.x > lim || s.x < -lim || s.y > lim || s.y < -lim || s.z > lim ||
        s.z < -lim) return false;
    if (s.anim_id >= kArenaAnimLimit) return false;
    if (s.state_idx >= kArenaStates && s.state_idx != 0xFF) return false;
    if (s.invuln > kArenaInvulnMax) return false;
    if (s.character > 7) return false;
    if (s.held > kArenaSeats && s.held < kArenaHeldObject) return false;
    if (s.eaten_by > kArenaSeats) return false;
    if (s.power & 0xC0) return false;
    if (s.reserved[0] || s.reserved[1]) return false;
    return true;
}

bool vs_arena_requested() {
    const char *nm = std::getenv("SM64DS_VS_NETMODE");
    if (!nm || !*nm || std::strcmp(nm, "lockstep") == 0) return false;
    if (std::strcmp(nm, "arena") != 0) {
        static bool said = false;
        if (!said) {
            said = true;
            std::fprintf(stderr, "[vsarena] SM64DS_VS_NETMODE=%s is not a "
                         "netmode (arena or lockstep); lockstep it is\n", nm);
        }
        return false;
    }
    const char *vs = std::getenv("SM64DS_VS_MAP");
    if (!vs || !*vs) {
        static bool said = false;
        if (!said) {
            said = true;
            std::fprintf(stderr, "[vsarena] SM64DS_VS_NETMODE=arena without "
                         "SM64DS_VS_MAP: arena mode is VS only; ignored\n");
        }
        return false;
    }
    return true;
}

extern "C" void port_fanout_force_wide(int on);   // hal/comms_fanout_wide.cpp (VS9)
extern "C" unsigned char data_0209f250[];        // the world's local player index (hal/auto_bss.cpp)
#define port_vs_local_index data_0209f250
extern "C" int port_vs_force_nobody = 0;          // SEAMVS1: the "seat has no body" drill, set only by a dev knob (hal/handoff_poll.cpp)
extern "C" int port_vs_leave_to_title(const char *why);   // hal/star_flow.cpp (SEAMVS1): the match leaves to the title in place
extern "C" int port_scene_live_id(void);                   // hal/scene_boot.cpp: 1 = the title is the live scene

// SEAMVS1: the install's latch at file scope, so a VS match that ended in place (vs_arena_leave_reset) can forget it and the next
// match handed to this process installs the arena again (vs_arena_install_in_place). g_in_place: this process's arena was installed
// by a hand-off at the title, not by main() at boot.
namespace {
bool g_tried = false;
bool g_in_place = false;
ULONGLONG g_in_place_t0 = 0;   // when the hand-off installed the arena (the time to the level's seat is printed from it)
bool g_report_hooked = false;
}  // namespace

bool vs_arena_install_from_env() {
    bool &tried = g_tried;
    if (tried) return g.installed;
    tried = true;
    if (!vs_arena_requested()) return false;

    // PHASE A IS 2..4 PLAYERS. The ROM's solo record copy fills four slot
    // records (kNarrowSlots), so a seat of 4 or more never gets its pad and
    // its body cannot move (review F1). Refuse before anything is touched, so
    // the process falls back exactly as if the env had not been set (the
    // lockstep carrier installs next). B1 widens this. The line avoids the
    // launcher's "NOT installing" watchdog words on purpose.
    {
        const char *ps = std::getenv("SM64DS_VS_PLAYERS");
        char *pend = nullptr;
        const long pn = (ps && *ps) ? std::strtol(ps, &pend, 10) : 0;
        if (ps && *ps && pend != ps && !*pend && pn > kArenaWide) {
            std::fprintf(stderr, "[vsarena] ERROR: SM64DS_VS_PLAYERS=%ld is "
                         "more than the %d seats the arena can "
                         "drive; arena mode refused, this console plays "
                         "without it\n", pn, kArenaWide);
            return false;
        }
        const char *pwhy = "";
        const int ppin = seat_pin_from_env(&pwhy);
        if (ppin >= kArenaWide && ppin < kArenaSeats) {
            std::fprintf(stderr, "[vsarena] ERROR: seat pin %d (%s) is past "
                         "the %d seats phase A of the arena can drive; arena "
                         "mode refused, this console plays without it\n",
                         ppin, pwhy, kArenaWide);
            return false;
        }
    }
    if (!g_cs_up) {   // SEAMVS1: once per process; a second install in place keeps the section
        ::InitializeCriticalSection(&g_cs);
        g_cs_up = true;
    }
    for (int k = 0; k < kArenaSeats; ++k) reset_ring(g.ring[k], 0);

    // AN ARENA CONSOLE IS NEVER AN ADVENTURE CONSOLE. The adventure ghost
    // layer (hal/comms_sync.cpp, hal/player_bridges.cpp) keys on
    // adventure_ghost_mode() && comms_hub_active(), and the hub is active
    // here: a player whose settings file has AdventureGhosts on would hand
    // the arena's bodies to the ghost follower. The settings read honours
    // SM64DS_ADVENTURE=0 as "off" and latches it at its first call.
    ::_putenv_s("SM64DS_ADVENTURE", "0");
    if (adventure_ghost_mode())
        std::fprintf(stderr, "[vsarena] WARNING: adventure mode was latched ON "
                     "before the arena install; the ghost layer may touch "
                     "arena bodies\n");

    g.bodies = env_int("SM64DS_VS_PLAYERS", 0, 1, kArenaSeats);
    if (!g.bodies) {
        g.bodies = 4;
        std::fprintf(stderr, "[vsarena] SM64DS_VS_PLAYERS is not set; the arena "
                     "boots 4 bodies\n");
    }
    // VS9: past four bodies the PadData / TouchInfo tails of the wide fan-out
    // must run, and comms_session_players() is 0 here (no comms session).
    port_fanout_force_wide(g.bodies > kNarrowSlots ? 1 : 0);
    const char *why = "";
    g.want_seat = seat_pin_from_env(&why);
    // THE VS CARRIER MUST NEVER OPEN IN AN ARENA PROCESS, and main() is not
    // its only installer: the VS env boot (hal/scene_vs_menu.cpp) calls
    // comms_loopback_install_from_env() again, and that installer keys on
    // SM64DS_COMMS_ROLE alone. Measured on the first build: the arena joiner
    // seated a lockstep child there, was "retired" by a host it never had and
    // exited 5. The role has done its one arena job (the seat pin, read
    // above), so it leaves this process's environment here.
    if (const char *role = std::getenv("SM64DS_COMMS_ROLE")) {
        std::fprintf(stderr, "[vsarena] SM64DS_COMMS_ROLE=%s pins the seat "
                     "only; removed from the environment so no VS carrier "
                     "opens in arena mode\n", role);
        ::_putenv_s("SM64DS_COMMS_ROLE", "");
    }
    g.log = env_on("SM64DS_ARENA_LOG");
    g.installed = true;
    g.hub = comms_hub_install_arena(g.want_seat);
    std::fprintf(stderr, "[vsarena] installed: arena mode, %d bodies, seat pin "
                 "%d (%s), hub %s. No DS transport, no rounds: my body takes "
                 "my pad with no added delay.\n", g.bodies, g.want_seat, why,
                 g.hub ? "open" : "NOT OPEN (no SM64DS_COMMS_RELAY or "
                                  "SM64DS_HUB_PORT): this console plays the "
                                  "arena alone");
    if (!g_report_hooked) { g_report_hooked = true; std::atexit(report_at_exit); }
    return true;
}

bool vs_arena_active() { return g.installed; }

void vs_ref_match_reset();    // hal/vs_arena_ref.cpp (SEAMVS1)
void vs_eat_match_reset();    // hal/vs_arena_eat.cpp
void vs_grab_match_reset();   // hal/vs_arena_grab.cpp
bool vs_arena_leave_reset(const char *why);

// SEAMVS1: THE ARENA INTO A LIVE PROCESS, for the launcher hand-off of a VS match at the title. The install main() makes at boot
// (vs_arena_install_from_env: the seat pin, the body count, the hub opened through comms_hub_install_arena), made again from the
// environment the hand-off has just written. Refuses (false, *why) when an arena is still installed, when the env does not ask for
// one, or when the hub does not open (the arena with no relay is a lab shape; a launcher match always names one); a failed install is
// taken down again so the caller refuses with nothing of it left.
bool vs_arena_install_in_place(const char **why) {
    *why = nullptr;
    if (g.installed) { *why = "a VS arena of an earlier match is still installed"; return false; }
    g_tried = false;
    if (!vs_arena_install_from_env() || !g.installed) { *why = "the arena install refused the request's names"; return false; }
    if (!g.hub || !comms_hub_active()) {
        *why = "the arena's hub did not open (no relay)";
        vs_arena_leave_reset("the in-place arena install did not open its hub");
        return false;
    }
    g_in_place = true;
    g_in_place_t0 = ::GetTickCount64();
    std::fprintf(stderr, "[seamless] VS arena installed in this process: %d bodies, seat pin %d\n", g.bodies, g.want_seat);
    std::fflush(stderr);
    return true;
}

// SEAMVS1: THE ARENA OUT OF A LIVE PROCESS, at the title after a VS match ended in place: the hub torn down (comms_hub_teardown:
// the I/O thread joined, the socket closed, the arena latch forgotten), the arena, the referee client, the eat and grab state and
// the puppets back to a process that never installed one, the wide fan-out off. False when the hub's thread did not stop (nothing
// else changed; the caller closes the game as before).
bool vs_arena_leave_reset(const char *why) {
    if (comms_hub_active() || comms_hub_state() != kHubOff)
        if (!comms_hub_teardown(why)) return false;
    const int bodies = g.bodies, seat = g.seat;
    {
        Guard l;
        g = Arena();
        for (int k = 0; k < kArenaSeats; ++k) reset_ring(g.ring[k], 0);
    }
    for (int k = 0; k < kArenaSeats; ++k) g_pup[k] = Pup();
    {
        const int on = g_lat.on;
        g_lat = Lat();
        g_lat.on = on;
    }
    for (int k = 0; k < 2; ++k) { g_sq[k] = SfxQueue(); g_sfx_cap[k] = 0; }
    vs_ref_match_reset();
    vs_eat_match_reset();
    vs_grab_match_reset();
    port_fanout_force_wide(0);
    g_tried = false;
    g_in_place = false;
    // my body's number back to the solo game's 0 (vs_arena_seat_world wrote the seat; a process that never joined has 0 in this ROM
    // word, and the solo level that follows spawns player 0 only: a seat-1 console's first door read the missing player 1 and faulted)
    // and the world's local player index the Stage seated from it (Stage::InitResources:178, data_0209f250 = func_0203da9c()): back to
    // the 0 a process that never joined holds until its first Stage
    const unsigned me_was = data_020a0f10[0], local_was = port_vs_local_index[0];
    data_020a0f10[0] = 0;
    port_vs_local_index[0] = 0;
    std::fprintf(stderr, "[seamless] VS arena taken down (%s): it was seat %d of %d bodies; referee client, eat, grab and puppets reset, "
                 "the install latch forgotten, my body number data_020a0f10 %u -> 0, local player index data_0209f250 %u -> 0\n",
                 why ? why : "?", seat, bodies, me_was, local_was);
    std::fflush(stderr);
    return true;
}

bool vs_arena_in_place() { return g_in_place; }

void vs_arena_seat_world() {
    if (!g.installed) return;
    int seat = comms_hub_my_seat();
    unsigned waited = 0;
    if (seat < 0 && g.hub && comms_hub_active()) {
        const int wait_ms = env_int("SM64DS_ARENA_PAIR_MS", 8000, 0, 60000);
        const unsigned t0 = comms_hub_now_ms();
        while (seat < 0 && comms_hub_now_ms() - t0 < (unsigned)wait_ms) {
            ::Sleep(4);
            seat = comms_hub_my_seat();
        }
        waited = comms_hub_now_ms() - t0;
        if (seat < 0)
            std::fprintf(stderr, "[vsarena] the relay did not name a seat "
                         "after %u ms\n", waited);
    }
    // The launcher's watchdog widens its window on this line (line start,
    // out/ARENALOBBY1/contract.txt section 10), so it prints once, ALWAYS,
    // whenever the hub settled the seat: waited for, or known at the boot.
    if (seat >= 0) {
        char src[48];
        if (waited) std::snprintf(src, sizeof src, "waited %u ms", waited);
        else std::snprintf(src, sizeof src, "%s", seat == g.want_seat ?
                           "known at boot, env pin" : "known at boot, hub");
        std::fprintf(stderr, "[vsarena] the relay named my seat %d (%s)\n",
                     seat, src);
    }
    if (seat < 0) {
        seat = g.want_seat < kArenaSeats ? g.want_seat : 0;
        std::fprintf(stderr, "[vsarena] no seat from the hub; taking seat %d "
                     "(%s)\n", seat, g.want_seat < kArenaSeats ? "the pin"
                                                              : "default");
    }
    if (g_in_place && port_vs_force_nobody) {   // SEAMVS1 drill (dev knob, hal/handoff_poll.cpp): a seat past the bodies
        port_vs_force_nobody = 0;
        std::fprintf(stderr, "[vsarena] the no-body drill: seat %d taken as %d, past the %d bodies\n", seat, g.bodies, g.bodies);
        seat = g.bodies;
    }
    if (seat >= g.bodies) {
        // A seat with no body faults the first frame that reaches for it.
        // Leave the match instead, the way a full room does (exit 4,
        // comms_hub.cpp): the launcher reads the code, the log has the line.
        std::fprintf(stderr, "[vsarena] ERROR: seat %d has no body (the arena "
                     "boots %d); this console leaves the match\n", seat,
                     g.bodies);
        std::fflush(stderr);
        std::fflush(stdout);
        // SEAMVS1: a match handed to a running game leaves it back at the title in place (the way its end does); my body word
        // stays where it was and my pad goes nowhere for the fade. A process that booted into the match still exits 4.
        if (g_in_place && port_scene_live_id() == 1) {   // seated by the hand-off at the title: the hand-off refuses (scene_vs_menu)
            g.seat = -1;
            return;
        }
        if (g_in_place && port_vs_leave_to_title("seat has no body")) {
            g.seat = -1;
            return;
        }
        ::ExitProcess(4);
    }
    if (g.seat >= 0 && g.seat != seat)
        std::fprintf(stderr, "[vsarena] my seat moved %d -> %d at a level "
                     "boot\n", g.seat, seat);
    g.seat = seat;
    data_020a0f10[0] = (unsigned char)seat;
    if (g_in_place && g_in_place_t0) {   // SEAMVS1: the hand-off's time to the world (REVSEAMJOIN1 note 4)
        std::fprintf(stderr, "[seamless] the handed-off VS match's level is seated %llu ms after the hand-off installed the arena\n",
                     (unsigned long long)(::GetTickCount64() - g_in_place_t0));
        g_in_place_t0 = 0;
    }
    std::fprintf(stderr, "[vsarena] seated: seat %d is my body (data_020a0f10 = "
                 "%d), %d bodies\n", seat, seat, g.bodies);
}

int vs_arena_my_seat() { return g.installed ? g.seat : -1; }

void vs_arena_route_pads() {
    if (!g.installed || g.seat < 0) return;
    // The ROM's solo copy fills records 0..3 only. A seat of 4..15 gets my
    // key word from record 0 first (the wide fan-out then feeds PadData[seat]).
    // VS9: the WHOLE record, not only the key word. The ROM's solo copy
    // (func_0203e0ac) fills records 0..3 with the local record: key word,
    // touch, and the camera heading GetAngleToCamera(seat) reads at +0x10 (the
    // direction the stick is turned by). Records 4..15 were zero, so a body
    // past seat 3 walked relative to heading 0, not to its camera (the
    // "inverted" movement of the open test).
    if (g.seat >= kNarrowSlots && g.seat < kArenaWide)
        std::memcpy(data_020a1154 + g.seat * kRecStride, data_020a1154,
                    kRecStride);
    for (int i = 0; i < kArenaWide; ++i)
        if (i != g.seat) *slot_keys(i) = 0;
}

void vs_arena_before_walk(int frame) {
    if (!g.installed) return;
    // THE PUPPET HOLD (A1 PUPPET1): the other seats' bodies are held and posed
    // before the actor walk ticks them (see the PUPPET section above).
    pup_before_walk();
    // EAT1: the tongue never eats a player in an arena (hal/vs_arena_eat.cpp)
    vs_eat_before_walk(frame);
    // GRAB1: the Wario hold (hal/vs_arena_grab.cpp)
    vs_grab_before_walk(frame);
}

void vs_arena_tick(int frame) {
    latency_probe();
    if (!g.installed || g.seat < 0) return;
    if (data_0209f2d8 != 1) return;          // the VS level only
    vs_eat_after_walk(frame);                // EAT1: a held puppet stays on its eater
    vs_grab_after_walk(frame);               // GRAB1: did the ROM let go of the puppet this frame
    if (!g.frame_started) {
        g.frame_started = true;
        std::fprintf(stderr, "[vsarena] first VS frame: seat %d, %d bodies "
                     "(ROM count %d, me %d)\n", g.seat, g.bodies,
                     (int)data_0209f21c, (int)data_0209f250[0]);
    } else {
        ++g.frame;
    }
    void *body = (g.seat < kArenaSeats) ? data_0209f394[g.seat] : nullptr;
    if (body) send_state(body);
    drain();
    roster();
    const unsigned now = comms_hub_now_ms();
    if (!g.last_report) g.last_report = now;
    if (now - g.last_report >= (unsigned)kReportEveryMs) {
        g.last_report = now;
        report_line("periodic");
    }
}

unsigned vs_arena_frame() { return g.have_last ? g.last.frame : 0; }

int vs_arena_samples(int seat, ArenaSample *out, int cap) {
    if (!g.installed || !out || cap <= 0 || seat < 0 || seat >= kArenaSeats ||
        seat == g.seat) return 0;
    Guard gd;
    const SeatRing &r = g.ring[seat];
    const int n = r.n < cap ? r.n : cap;
    std::memcpy(out, r.s, sizeof(ArenaSample) * (size_t)n);
    return n;
}

bool vs_arena_seat_view(int seat, ArenaSeatView *out) {
    if (!out || seat < 0 || seat >= kArenaSeats) return false;
    std::memset(out, 0, sizeof *out);
    if (!g.installed) return false;
    HubSeatInfo hi;
    if (comms_hub_seat_info(seat, &hi)) {
        out->present = hi.present;
        out->join_epoch = hi.join_epoch;
        out->rtt_ms = hi.rtt_ms;
    } else {
        out->rtt_ms = -1;
    }
    Guard gd;
    const SeatRing &r = g.ring[seat];
    out->has_state = r.n > 0 && r.epoch == out->join_epoch;
    out->samples = r.samples;
    out->rejected = r.rejected;
    out->body_held = r.held;
    if (r.n > 0) {
        out->newest_frame = r.s[0].s.frame;
        out->newest_age_ms = comms_hub_now_ms() - r.s[0].recv_ms;
        const unsigned now_sec = comms_hub_now_ms() / 1000u;
        bool have = false;
        int best = 0;
        for (int i = 0; i < kOffsetBuckets; ++i) {
            if (r.bucket_sec[i] == ~0u || now_sec - r.bucket_sec[i] >= 8u)
                continue;
            if (!have || r.bucket_off[i] < best) { best = r.bucket_off[i]; have = true; }
        }
        out->clock_offset_ms = have ? best : 0;
    }
    return true;
}

unsigned vs_arena_roster_mask() { return g.roster_mask; }
unsigned vs_arena_roster_version() { return g.roster_version; }

void vs_arena_note_sfx(unsigned id) {
    if (!g.installed || !id || id > 255 || g.nsfx >= 2) return;
    g.sfx[g.nsfx++] = (unsigned char)id;
}
void vs_arena_set_ack(unsigned short seq) { g.ack = seq; }
void vs_arena_set_star_lock(bool on) { g.star_lock = on; }
void vs_arena_send_now() { g.send_now = true; }

bool vs_arena_referee_now(unsigned *ref_ms, int *rtt_ms) {
    if (!g.installed) return false;
    return comms_hub_referee_clock(ref_ms, rtt_ms, nullptr);
}

int vs_arena_state_index(const void *body) { return state_index_of(body); }

void vs_arena_sound_request(unsigned kind, unsigned id, const void *v) {
    pup_note_sound(kind, id, v);
}

bool vs_arena_body_held(int seat) {
    return g.installed && seat >= 0 && seat < kArenaSeats && g.ring[seat].held;
}

ArenaStats vs_arena_stats() {
    ArenaStats s = g.st;
    s.installed = g.installed;
    s.seat = g.seat;
    s.bodies = g.bodies;
    s.frame = g.frame;
    return s;
}

void vs_arena_report(const char *tag) {
    if (!g.installed) return;
    report_line(tag ? tag : "report");
}

}  // namespace port

// The flat door hal/sdat/sound_abi.cpp calls for every sound request (A1): a
// sound whose vector is my own body's Player+0x74 goes out in my next SYNS.
extern "C" void port_vs_arena_sound(unsigned kind, unsigned id, const void *v) {
    port::vs_arena_sound_request(kind, id, v);
}
