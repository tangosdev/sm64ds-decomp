// PARTY HITS, game side. Rules: hal/party_hits_core.h. Overview: hal/party_hits.h and
// cloud_tasks/PLAYINT1_DESIGN.md. NOT YET BUILT OR RUN ON A DEVICE (written on a Linux VM): every ROM offset
// is taken from code that already runs (party.cpp, party_cyl.cpp, vs_arena_ref.cpp apply_hit_on_me), the
// guesses are marked TUNE.
//
// PORT_HOST_ABI. Reads Player fields; writes only MY OWN body (the victim's console moves its own player, the
// attacker's console never writes a friend's body except the cosmetic scale).

#include "party_hits.h"
#include "party_hits_core.h"
#include "party.h"
#include "comms_hub.h"
#include "coop_wire.h"
#include "hub_presence.h"
#include "player_fields.h"
#include "ghost_tongue.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;       // my player slot
extern unsigned char data_0209f21c;       // number of player bodies
int port_adventure_ghost_seat(int slot);  // the hub seat a ghost body in player slot `slot` shows (-1: none)
int port_adventure_ghost_drawpos(int slot, int out[3]);   // where the follower put that ghost this frame (fix12), 0 = not posed
extern signed char data_0209f2f8;         // level (sublevel id)
extern signed char data_02092120;         // area
extern int data_0209f220[];               // [0] & 0xff = act
extern unsigned char data_0209f2d8[];     // game mode: 0 adventure / party / coop, 1 VS, 2 script (cutscene)
extern unsigned char data_0209fc48[];     // (s32) cutscene
extern unsigned char data_0209d660[];     // 1 while a message box is up (coop_tp.cpp)
extern unsigned char data_ov002_0211013c[];   // the neutral state the ROM returns to (vs_arena_eat.cpp)
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const void *pos, unsigned dmg_units, int speed,
                                           unsigned char a4, unsigned char a5, unsigned char a6);
void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
int _ZN6Player7IsStateERNS_5StateE(void *self, void *state);
int _ZN6Player7TryGrabER8dActor_c(void *self, void *actor);   // holder->TryGrab(victim)
int _ZN6Player9DropActorEv(void *holder);
extern unsigned char data_ov002_02110064[];   // St_InYoshiMouth
extern unsigned char data_ov002_0211016c[];   // St_Grabbed
extern unsigned char data_ov002_02110604[];   // St_SwingPlayer
int func_ov002_020d5f34(char *c, void *eater);   // victim: into the mouth (state, +0xd0, anim 0x3f)
int _ZN6Player19func_ov002_020d5ed0Ev(char *c);                // victim: mouth step 1
int _ZN6Player7SetAnimEji5Fix12IiEj(void *c, unsigned anim, int a, int b, unsigned d);
int func_ov002_020d5cec(char *c);                // victim: the ROM's own spit
int _ZN6Player19func_ov002_020beb38Ev(char *c);                // the ROM's mash count (buttons pressed this frame, stick whirl +8)
int func_ov002_020db5f4(char *victim, char *holder);              // victim: St_Grabbed
void func_ov002_020db54c(unsigned char *victim, int speed, int vert, short yaw);   // victim: thrown
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned id, const void *pos);
int func_ov002_020d82f0(void *c);                // the ROM's "can be eaten / grabbed" gate (src/game/player/Player.cpp)
extern char data_0209f498[];                     // Stage::CheckInput's Ctrl[4], stride 0x18 (vs_arena_eat.cpp rig_press)
extern unsigned short data_020a0e58[];           // PadData[4]: {held, pressed}, the raw DS pad
extern int data_020a0e5a[];                      // the split "pressed" symbol (same memory on the DS)
extern unsigned short *data_0209214c[];          // CheckInput's three DS bit -> Ctrl bit maps
void *_ZN8dActor_c4NextEPKS_(const void *prev);   // the actor list walk (coop_actors.cpp)
}
namespace port {
int vs_arena_state_index(const void *body);
bool comms_sync_ghost_look(int seat, unsigned *anim, unsigned *character);
void vs_grab_export_link(char *holder, char *carried, bool ready);
void vs_grab_export_unlink(char *holder, char *carried);
void vs_grab_export_overlay(char *holder, char *carried);
namespace ghost_tongue { bool is_hurt_anim(unsigned anim); }
}

namespace port {
namespace party_hits {
namespace {

using namespace phit;

enum : unsigned {
    kVertSpeed = 0xa8, kHorzSpeed = 0x98,       // apply_hit_on_me writes both after Hurt (vs_arena_ref.cpp)
    kNoControl = 0x709, kInMouth = 0xd0,        // my_body_may_act reads both
    kYawAngle = 0x8e,
    kBounceUp = 0x30000,                        // TUNE: the attacker's hop off a friend's head (Fix12 / frame)
    kSendGapFrames = 30,                        // the attacker's own re-claim gap per victim (< relay cap 10/s)
    kMouthObj = 0x360,                          // mObjInMouth: the player in MY mouth
    kHeldObj = 0x358, kGrabbedBy = 0x35c,       // mHeldObj / mGrabbedByActor
    kFlags = 0xb0,
    kHoldTimer = 0x6c6,                         // the eater's mouth hold
    kSwingSpeed = 0x69c,
    kBlinkFrames = 0x24,                        // mInvincibleTimer after a knock: 36 frames of blink
    kStThrow = 63, kStSwing = 65,
    kLookWario = 2,
    kRingFrames = 16,
    kAttachLife = 150,                          // an attach nobody refreshed or cleared dies after this
    kGrabAckWait = 20,
    kAttachBeat = 30,
    kHeldBeat = 30, kHeldLife = 90,             // TONGUE1: held state repeats each second, dies unrefreshed after three
    kMouthPullFrames = 6, kParkUnits = 200000,  // a friend's mouthful: at the mouth for the pull, then out of sight                           // the victim repeats its attach this often
};
enum : unsigned {
    kFlagCarried = 0x100, kFlagHeldOut = 0x4000, kFlagGrabbable = 0x80, kFlagThrown = 0x400,
};

struct Pos3 { int x = 0, y = 0, z = 0; };
struct Attach { int by = -1; int kind = 0; unsigned until = 0; };
struct CarryV { bool active = false; int holder = -1; unsigned f0 = 0; bool synth = false; };
struct CarryH { bool active = false; int victim = -1; unsigned f0 = 0; int phase = 0; };

struct G {
    bool read = false, installed = false, on = false, announced = false;
    int log = 0;
    Judge judge;
    EatHold hold;
    EatClaimBook book;
    unsigned frame = 0;
    unsigned send_frame[kHubPartySeats] = {};
    unsigned counter = 0;
    int prev_state = 0xFF, state = 0xFF;
    unsigned in_state = 0;
    // the pose ring (INTERACT9): my own position the last 16 frames, for claims stamped with their age
    Pos3 ring[kRingFrames];
    // squash shown on the friend I stomped
    unsigned squash_until[kHubPartySeats] = {};
    // INTERACT9: who is in whose mouth / hands, from the victim's own broadcast
    Attach att[kHubPartySeats];
    // I am eaten through the ROM's own mouth state (else the synthetic hold)
    bool rom_mouth = false;
    // TONGUE1: what each friend holds (kHeldState in), what I last said I hold (out)
    struct HeldIn { int kind = 0, cls = 0, slot = -1, eggs = 0; unsigned until = 0, since = 0; char *pinned = nullptr; };
    HeldIn held_in[kHubPartySeats];
    int ho_kind = 0, ho_cls = 0, ho_slot = -1, ho_eggs = 0; unsigned ho_last = 0;
    unsigned held_sent = 0, held_recv = 0, held_pins = 0, held_miss = 0;
    unsigned mouth_miss = 0, eats_unstepped = 0;      // TONGUE1: hook passes the ROM mouth state left me off the eater's drawn body
    unsigned beat = 0;
    CarryV cv;      // I am carried
    CarryH ch;      // I carry
    // knobs
    bool rom_eat = true, rom_grab = true, grab_on = true, feedback = true;
    bool pend = false; Claim pend_claim; Effect pend_fx;      // a believed claim waits here for the frame hook
    unsigned pend_ms = 0;
    int diag_left = 0;     // frames of link diagnostics after an eat / grab begins
    // body-touching replies wait for the frame hook too (the pump runs inside the tick, around the actor walk)
    struct Later { int what = 0; int seat = 0; bool swing = false; int yaw = 0, speed = 0; };
    Later later[6]; int later_n = 0;
    int eat_ahead = 28, eat_up = 30, carry_ahead = 8, carry_up = 85;
    int held_ahead = 30, held_up = 45;      // TONGUE1: a friend's carried object, units ahead of / above his feet (SM64DS_HELD_AT)
    // lab
    char forge_kind = 0; int forge_units = 0; bool forged = false;
    unsigned probe_frame = 0;
    bool lab_done = false;      // the gather stops for good once an eat / hold has begun on this console (the spit and the throw fly free)
    int gather_dx = 0, gather_dz = 0; bool gather = false;     // LAB: stand next to the host every frame (pictures independent of frame skew)
    int lab_seat = -1;      // LAB: forge only toward this hub seat
    int lab_swing = 0; bool swing_done = false;     // LAB: force the holder's St_SwingPlayer this many frames into the hold (the pad's circular stick motion is not scripted; the arena rig does the same)
    unsigned forge_at = 0, eat_hold = 0;      // LAB: forge from this frame on; a longer eat hold for pictures
    // LAB (TONGUE1, SM64DS_PHIT_LAB_SEEK=<seat>[,<near>]): walk to that friend and use the tongue with the DS pad alone
    int seek_seat = -1, seek_near = 200;
    int seek_phase = 0; unsigned seek_t = 0; int seek_cup = 0, seek_sign = 1; unsigned seek_f = 0;
    unsigned long long seek_tongues = 0;
    // TONGUE1 counters: why a tongue did not say anything, and what the ROM's own gate said of each refused eat
    unsigned long long tongues = 0, tongue_sent = 0, unsent_far = 0, unsent_facing = 0, unsent_dy = 0, unsent_none = 0;
    unsigned long long eat_rom_ok = 0, eat_rom_no = 0;
    bool tongue_said = false;
    Stats st = {};
} g;

void read_env() {
    if (g.read) return;
    g.read = true;
    g.judge.tog.interact = env_flag(std::getenv("SM64DS_ROOM_INTERACT"), true);
    g.judge.tog.damage = env_flag(std::getenv("SM64DS_ROOM_DAMAGE"), false);
    if (const char *e = std::getenv("SM64DS_PHIT_LOG")) g.log = std::atoi(e);
    Rules &r = g.judge.rules;
    r.check_reach = !env_flag(std::getenv("SM64DS_PHIT_NOREACH"), false);
    r.check_level = !env_flag(std::getenv("SM64DS_PHIT_NOLEVEL"), false);
    r.check_cooldown = !env_flag(std::getenv("SM64DS_PHIT_NOCOOLDOWN"), false);
    r.check_pos = !env_flag(std::getenv("SM64DS_PHIT_NOPOS"), false);
    r.check_busy = !env_flag(std::getenv("SM64DS_PHIT_NOBUSY"), false);
    // TONGUE1: no port re-arm after an eat. The cartridge's own gate already refuses the spat victim while its +0x708 is
    // set; the 150-frame re-arm refused tongues the ROM takes (7 of 18 refusals in MPJANK1's recipe).
    r.check_eat_rearm = env_flag(std::getenv("SM64DS_PHIT_EAT_REARM"), false);
    // ROMSTATE9: the victim's own console runs the ROM's real St_InYoshiMouth (eat) and St_Grabbed / thrown (carry) by default;
    // SM64DS_ROOM_ROMSTATE=0 falls back to INTERACT9's synthetic holds (SM64DS_ROOM_EATVIS=0/1 still picks the eat alone)
    g.rom_eat = g.rom_grab = env_flag(std::getenv("SM64DS_ROOM_ROMSTATE"), true);
    if (const char *e = std::getenv("SM64DS_ROOM_EATVIS")) if (*e) g.rom_eat = std::atoi(e) != 0;
    g.grab_on = env_flag(std::getenv("SM64DS_ROOM_GRAB"), true);
    g.feedback = env_flag(std::getenv("SM64DS_PHIT_FEEDBACK"), true);
    if (const char *e = std::getenv("SM64DS_ATTACH_EAT")) std::sscanf(e, "%d,%d", &g.eat_ahead, &g.eat_up);
    if (const char *e = std::getenv("SM64DS_ATTACH_CARRY")) std::sscanf(e, "%d,%d", &g.carry_ahead, &g.carry_up);
    if (const char *e = std::getenv("SM64DS_HELD_AT")) std::sscanf(e, "%d,%d", &g.held_ahead, &g.held_up);
    if (const char *e = std::getenv("SM64DS_PHIT_LAB_GATHER")) { g.gather = true; g.gather_dx = 150; g.gather_dz = 0; std::sscanf(e, "%d,%d", &g.gather_dx, &g.gather_dz); }
    if (const char *e = std::getenv("SM64DS_PHIT_LAB_AT")) g.forge_at = (unsigned)std::atoi(e);
    if (const char *e = std::getenv("SM64DS_PHIT_LAB_SEAT")) g.lab_seat = std::atoi(e);
    if (const char *e = std::getenv("SM64DS_PHIT_LAB_SWING")) g.lab_swing = std::atoi(e);
    if (const char *e = std::getenv("SM64DS_PHIT_EATHOLD")) g.eat_hold = (unsigned)std::atoi(e);
    if (const char *e = std::getenv("SM64DS_PHIT_LAB_SEEK")) {
        if (!std::strncmp(e, "any", 3)) { g.seek_seat = -2; std::sscanf(e + 3, ",%d", &g.seek_near); }
        else std::sscanf(e, "%d,%d", &g.seek_seat, &g.seek_near);
    }
    if (const char *e = std::getenv("SM64DS_PHIT_LAB")) {
        char kind[16] = {0}; int units = 0;
        if (std::sscanf(e, "forge:%15[a-z]:%d", kind, &units) == 2) {
            g.forge_units = units;
            g.forge_kind = !std::strcmp(kind, "melee") ? 'm' : !std::strcmp(kind, "pound") ? 'p' :
                           !std::strcmp(kind, "stomp") ? 's' : !std::strcmp(kind, "eat") ? 'e' :
                           !std::strcmp(kind, "grab") ? 'g' : 0;
        }
    }
}

char *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}
// ROMSTATE9: the body of a HUB SEAT. Slot i of data_0209f394 is a player SLOT, not a seat: in a party / coop room my own
// body sits in slot data_0209f250 (0 on every console in the rows) and each friend's ghost body in some other slot, the
// follower's body -> seat table (port_adventure_ghost_seat) says which. (INTERACT9 indexed the array by seat, so on seat 1
// the "eater" was my OWN body: the mouth state read its own pointers, found the eater's mouth object was not itself and
// spat at once; the carry state landed in another state the same way. cdb: out/ROMSTATE9/logs/cdb_dbg11_p1.)
char *body_of_seat(int seat) {
    if (seat < 0 || seat >= kHubPartySeats) return nullptr;
    if (seat == comms_hub_my_seat()) return my_body();
    const int me = (int)data_0209f250;
    const int nb = (int)data_0209f21c < 16 ? (int)data_0209f21c : 16;
    for (int i = 0; i < nb; ++i) {
        if (i == me || !data_0209f394[i]) continue;
        if (port_adventure_ghost_seat(i) == seat) return (char *)data_0209f394[i];
    }
    return nullptr;
}
int u(int fix12) { return fix12 >> 12; }
// A clock every console on this machine shares (the rows compare one console's send with another's apply).
unsigned wall_ms() {
    return (unsigned)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() & 0x7fffffff);
}
int &s32(char *b, int off) { return *(int *)(b + off); }
unsigned &u32(char *b, int off) { return *(unsigned *)(b + off); }
char *&ptrf(char *b, int off) { return *(char **)(b + off); }
// Player::IsState without the call: mState (+0x370) is the state the body is in (a bool-returning ROM method
// declared int elsewhere can hand back garbage in the upper bits of eax)
bool state_is(const char *b, const void *st) { return *(void *const *)(b + 0x370) == st; }

// ROMSTATE9: a ghost body sits PARKED (my position + 200000 up) between the follower's pass and the next, so any ROM code that
// reads the eater's / holder's position from its body (the mouth state's follow, the spit's launch point, the hand placement)
// must see it where it is drawn: put the body at the follower's drawn position first. False when the ghost is not posed.
bool pose_ghost_at_draw(char *gb) {
    if (!gb) return false;
    const int nb = (int)data_0209f21c < 16 ? (int)data_0209f21c : 16;
    for (int i = 0; i < nb; ++i) {
        if (data_0209f394[i] != gb) continue;
        int dp[3];
        if (!port_adventure_ghost_drawpos(i, dp)) return false;
        *player::pos_x(gb) = dp[0]; *player::pos_y(gb) = dp[1]; *player::pos_z(gb) = dp[2];
        return true;
    }
    return false;
}

Place my_place() {
    Place p; p.level = (int)(unsigned char)data_0209f2f8; p.area = (int)data_02092120; p.act = data_0209f220[0] & 0xff;
    return p;
}

// Can my body be hit / held right now? (the same set of reasons vs_arena_ref.cpp's my_body_may_act and the
// referee's _hit use, plus a level in transition). `grab`: a Wario hold takes a STUNNED player, so the hurt
// states and the knock's invincibility do not count against it.
// TONGUE1: my own Player::Behavior does not run its state while a message sequence holds me (func_ov002_020c4188 returns
// early on +0x71e, e.g. Bob-omb Battlefield's first-visit tutorial), and a message box freezes the actor walk
// (actor_registry.cpp). Eaten then, the ROM mouth state never steps: hidden on my own spot until the spit.
bool me_in_message(const char *me) {
    return *(const unsigned char *)(me + 0x71e) != 0 || data_0209d660[0] != 0;
}
const char *g_busy_why = "";
int g_busy_state = 0xFF;
bool me_busy(const char *me, bool grab = false) {
    const int st = port::vs_arena_state_index(me);
    g_busy_state = st;
    g_busy_why = "state";
    if (st != 0xFF && state_blocks_hit(st) && !(grab && (st == 7 || st == 36))) return true;
    g_busy_why = "invincible";
    if (!grab && *(const unsigned short *)(me + player::kInvincibleTimer) > 0) return true;
    g_busy_why = "nocontrol";
    if (!grab && (*(const unsigned char *)(me + 0x708) || *(const unsigned char *)(me + kNoControl))) return true;
    g_busy_why = "inmouth";
    if (*(void *const *)(me + kInMouth)) return true;
    g_busy_why = "message";
    if (me_in_message(me)) return true;
    g_busy_why = "level_state";
    if (data_0209f2d8[0] == 2) return true;
    g_busy_why = "cutscene";
    if (*(const int *)data_0209fc48 != 0) return true;
    g_busy_why = "";
    return false;
}

// TONGUE1: what the cartridge itself asks before a Yoshi tongue takes a player (func_ov002_020d6c60, the VS arena's
// own path): func_ov002_020d82f0 on the victim (no blink +0x6a0, not mega +0x703, no cap power +0x73c, not vanished
// +0x6fb, both no-control bytes +0x708 / +0x709 clear), the victim holds nobody in its own mouth (+0x360), and the
// mouth entry func_ov002_020d5f34 finds +0xd0 (mEatingPlayer) empty. Nothing about the state the body is in.
// Returns 0 when the ROM would eat, else a short reason.
const char *rom_eat_refusal(char *me) {
    if (!func_ov002_020d82f0(me)) {
        if (*(const unsigned short *)(me + 0x6a0)) return "rom_blink";
        if (*(const unsigned char *)(me + 0x703)) return "rom_mega";
        if (*(const unsigned short *)(me + 0x73c)) return "rom_capflags";
        if (*(const unsigned char *)(me + 0x6fb)) return "rom_vanish";
        return "rom_nocontrol";
    }
    if (*(void *const *)(me + kMouthObj)) return "rom_mouth_full";
    if (*(void *const *)(me + kInMouth)) return "rom_in_a_mouth";
    return nullptr;
}

bool see(int seat, Seen *s) {
    HubPresence p; unsigned age = 0;
    s->have_presence = hub_presence_get(seat, &p, &age);
    if (s->have_presence) {
        s->presence_age_ms = age;
        s->in_level = p.in_level() && !p.in_menu() && !p.in_cutscene();
        s->at.level = p.level; s->at.area = p.area; s->at.act = p.act;
    }
    HubSeatInfo si;
    s->have_ghost = comms_hub_seat_info(seat, &si) && si.has_ghost;
    if (s->have_ghost) { s->ghost_age_ms = si.ghost_ms_ago; s->x = u(si.x); s->y = u(si.y); s->z = u(si.z); }
    return s->have_presence;
}

// ---- transport ------------------------------------------------------------------------------------
bool send_wire(int to_seat, unsigned ikind, int yaw, int x, int y, int z, unsigned stamp = 0) {
    const unsigned seq = pack_seq(ikind, ++g.counter, stamp);
    unsigned char d[64];
    const int n = hub_party_hit_encode(to_seat, kHitShove, seq, yaw, x << 12, y << 12, z << 12, d);
    if (!n) return false;
    if (comms_hub_room_kind() == kHubRoomCoop) {
        // coop rooms do not route SYNH (relay: party only): the same 24 bytes ride a SYNB sub-type
        unsigned char c[64];
        const int m = hub_coop_encode(kHubTagCoopCtl, kCoopCtlSubtype, seq, d, n, c, (int)sizeof c);
        return m && comms_hub_send(c, m);
    }
    return comms_hub_send(d, n);
}

// my attach state to every other seat: x = 0 off / 1 eaten / 2 carried, yaw = the holder's seat
void broadcast_attach(int holder, int state) {
    for (int seat = 0; seat < kHubPartySeats; ++seat) {
        if (seat == comms_hub_my_seat() || !party::seat_is_party(seat)) continue;
        send_wire(seat, kAttach, holder & 0xFFFF, state, 0, 0);
    }
}

// ---- TONGUE1: what I hold, to every other seat ----------------------------------------------------------
// The SYNG frame carries my body only: a friend sees my hands and mouth empty. The object's identity on every console
// is its spawn slot (+0xce) and class (+0x0c), the same pair coop_actors.cpp syncs it by. A player in my hands or mouth
// rides kAttach instead; eggs are local spawns (+0x588, five following eggs), so only their count goes out.
void held_send(char *me) {
    int kind = 0, cls = 0, slot = -1, eggs = 0;
    const char *ho = ptrf(me, kHeldObj), *mo = ptrf(me, kMouthObj);
    const char *ob = mo ? mo : ho;
    if (ob && *(const unsigned short *)(ob + 0x0c) != 0xbf) {
        kind = mo ? 2 : 1;
        cls = *(const unsigned short *)(ob + 0x0c);
        slot = *(const short *)(ob + 0xce);
        if (cls >= 1024 || slot < 0) kind = cls = 0, slot = -1;     // a runtime spawn has no slot every console shares
    }
    if (const int *q = *(int *const *)(me + 0x588)) for (int i = 0; i < 5; ++i) if (q[i]) ++eggs;
    const bool changed = kind != g.ho_kind || cls != g.ho_cls || slot != g.ho_slot || eggs != g.ho_eggs;
    if (!changed && (!(kind || eggs) || g.frame - g.ho_last < (unsigned)kHeldBeat)) return;
    g.ho_kind = kind; g.ho_cls = cls; g.ho_slot = slot; g.ho_eggs = eggs; g.ho_last = g.frame;
    if (changed && g.log > 0)
        std::fprintf(stderr, "[phit] held out f=%u: kind %d class %d slot %d eggs %d\n", g.frame, kind, cls, slot, eggs);
    for (int seat = 0; seat < kHubPartySeats; ++seat) {
        if (seat == comms_hub_my_seat() || !party::seat_is_party(seat)) continue;
        if (send_wire(seat, kHeldState, slot & 0xFFFF, kind | (eggs << 4), cls, 0)) ++g.held_sent;
    }
}

// After the follower posed every ghost: an object a friend holds sits at his DRAWN hands, one in his mouth goes into
// it (at the mouth for the tongue's pull, then out of sight, the way the cartridge hides a mouthful). My copy's own
// Behavior still runs from the pinned spot next walk; coop_actors' slave pin ran before the follower.
char *held_find(int cls, int slot) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + 0x0c) == (unsigned)cls && *(const short *)(a + 0xce) == slot) return a;
    return nullptr;
}
void held_after_follow_impl() {
    char *me = my_body();
    const int nb = (int)data_0209f21c < 16 ? (int)data_0209f21c : 16;
    for (int seat = 0; seat < kHubPartySeats; ++seat) {
        G::HeldIn &h = g.held_in[seat];
        if (!h.kind) continue;
        if (g.frame >= h.until) { std::fprintf(stderr, "[phit] held: seat %d went quiet, nothing pinned\n", seat); h = G::HeldIn(); continue; }
        int dp[3] = {0, 0, 0}, drawn = 0; char *gb = nullptr;
        for (int i = 0; i < nb && !drawn; ++i)
            if (port_adventure_ghost_seat(i) == seat && data_0209f394[i] && data_0209f394[i] != me) {
                gb = (char *)data_0209f394[i]; drawn = port_adventure_ghost_drawpos(i, dp);
            }
        if (!drawn || !gb) continue;
        char *a = held_find(h.cls, h.slot);
        if (!a || a == me) { if (++g.held_miss <= 8) std::fprintf(stderr, "[phit] held: no local class %d slot %d for seat %d\n", h.cls, h.slot, seat); continue; }
        if (a != h.pinned) {
            h.pinned = a;
            std::fprintf(stderr, "[phit] held: pinning my class %d slot %d to seat %d's %s\n", h.cls, h.slot, seat, h.kind == 1 ? "hands" : "mouth");
        }
        const int yaw = *player::facing(gb) & 0xFFFF;
        const int ahead = h.kind == 1 ? g.held_ahead : g.eat_ahead, up = h.kind == 1 ? g.held_up : g.eat_up;
        const double ang = (double)yaw * 3.14159265358979323846 / 32768.0;
        int *pos = (int *)(a + 0x5c);
        pos[0] = dp[0] + (int)std::lround(std::sin(ang) * ahead) * 0x1000;
        pos[2] = dp[2] + (int)std::lround(std::cos(ang) * ahead) * 0x1000;
        pos[1] = dp[1] + up * 0x1000 + ((h.kind == 2 && g.frame - h.since > (unsigned)kMouthPullFrames) ? kParkUnits * 0x1000 : 0);
        *(short *)(a + 0x8e) = (short)yaw;
        ++g.held_pins;
    }
}

// ---- the attach book (every console) --------------------------------------------------------------
void att_set(int victim, int holder, int kind) {
    if (victim < 0 || victim >= kHubPartySeats) return;
    Attach &a = g.att[victim];
    if (kind == 0 || holder < 0 || holder >= kHubPartySeats || holder == victim) {
        if (a.by >= 0) std::fprintf(stderr, "[phit] attach of seat %d ends (was kind %d by seat %d)\n", victim, a.kind, a.by);
        a = Attach();
        return;
    }
    if (a.by != holder || a.kind != kind)
        std::fprintf(stderr, "[phit] attach: seat %d is %s seat %d\n", victim, kind == 1 ? "in the mouth of" : "carried by", holder);
    a.by = holder; a.kind = kind; a.until = g.frame + kAttachLife;
}

// Bystander (and holder) consoles link the carried ghost into the holder's hands the way the arena's other
// consoles do, so the ROM's hand placement and the carry pose see the pair. Run once a frame.
void att_links() {
    static int linked_h[kHubPartySeats];
    static bool init = false;
    if (!init) { for (int &l : linked_h) l = -1; init = true; }
    const int me = comms_hub_my_seat();
    for (int v = 0; v < kHubPartySeats; ++v) {
        Attach &a = g.att[v];
        if (a.by >= 0 && g.frame >= a.until) { std::fprintf(stderr, "[phit] attach of seat %d expired\n", v); a = Attach(); }
        const int want = (a.by >= 0 && a.kind == 2 && v != me && a.by != me) ? a.by : -1;
        if (want == linked_h[v]) continue;
        char *vb = body_of_seat(v);
        if (linked_h[v] >= 0) {
            char *old = body_of_seat(linked_h[v]);
            if (old && vb) port::vs_grab_export_unlink(old, vb);
        }
        if (want >= 0) {
            char *hb = body_of_seat(want);
            // the holder's raised-arms layer only. NOT the ROM's hand link (holder +0x358 / carried +0x35c): the ROM's hand
            // placement of a carried body reads the LOCAL player's hand, so on a bystander it drew the carried ghost over my own
            // head (picture ci_g1_p2 / p3, out/ROMSTATE9). The attach rule in gw_follow places the carried ghost at the holder.
            if (hb && vb) port::vs_grab_export_overlay(hb, vb);
        }
        linked_h[v] = want;
    }
}

// ---- feedback + lag compensation -----------------------------------------------------------------
Pos3 hist_pos(unsigned stamp) {
    if (stamp >= (unsigned)kRingFrames) stamp = kRingFrames - 1;
    if (stamp > g.frame) stamp = g.frame;
    return g.ring[(g.frame - stamp) % kRingFrames];
}

void attacker_feedback(int seat, const Seen &s) {
    if (!g.feedback) return;
    char *vb = body_of_seat(seat);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, s.x << 12, (s.y << 12) + 0x50000, s.z << 12);
    if (vb) _ZN5Sound9PlayBank0EjRK7Vector3(0x12, vb + 0x74);
}

// ---- victim side ----------------------------------------------------------------------------------
void log_verdict(const Claim &c, Verdict v) {
    ++g.st.claims_recv;
    if (v != kOk) { ++g.st.refused; ++g.st.refused_by[v & 15]; }
    if (g.log > 0 || v == kOk)
        std::fprintf(stderr, "[phit] t=%u f=%u %s ik=%u from seat %d (claimed %d,%d,%d) stamp=%u -> %s%s%s st=%d\n", wall_ms(), g.frame, v == kOk ? "APPLY" : "REFUSE", c.ikind,
                     c.attacker, c.x, c.y, c.z, c.stamp, verdict_name(v),
                     v == kVictimBusy ? " why=" : "", v == kVictimBusy ? g_busy_why : "", g_busy_state);
}

// Apply a believed shove / stomp to my own body: the ROM's own hurt entry with zero damage, then the
// knock speeds re-asserted the way apply_hit_on_me does after Hurt. Then the blink the ROM only arms at the
// end of a damaging hurt: 36 frames of mInvincibleTimer (the ROM's Render hides the body on odd frames).
void apply_effect(char *me, const Claim &c, const Effect &fx) {
    int v[3] = { c.x << 12, c.y << 12, c.z << 12 };
    // a5 = 0: Hurt adds 2 to the damage when a5 != 0 (the arena's variant select), which would make a "no damage" shove cost 2
    const int ok = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(me, v, (unsigned)fx.damage_units, fx.speed, 0, 0, 0);
    if (!ok) { ++g.st.refused; return; }
    *(int *)(me + kHorzSpeed) = fx.speed;
    *(int *)(me + kVertSpeed) = fx.hop;
    *(unsigned short *)(me + player::kInvincibleTimer) = kBlinkFrames;
    ++g.st.applied;
    std::fprintf(stderr, "[phit] t=%u f=%u HURT applied ik=%u, blink timer 0x%x\n", wall_ms(), g.frame, c.ikind, (unsigned)*(unsigned short *)(me + player::kInvincibleTimer));
}

void eat_begin(char *me, const Claim &c) {
    g.lab_done = true;
    g.hold.begin(c.attacker, c.yaw, g.frame);
    g.mouth_miss = 0;
    if (g.eat_hold) g.hold.set_frames(g.eat_hold, g.eat_hold + 45);
    ++g.st.eats_began;
    char *ep = body_of_seat(c.attacker);
    g.rom_mouth = false;
    if (g.rom_eat && ep) {
        // the ROM's own entry into a mouth, the way the arena's EATEN event does it (vs_arena_eat.cpp v_enter): the eater's
        // body is the mEatingPlayer, its +0x360 names me, my state becomes St_InYoshiMouth (func_ov002_020d5f34), then step 1
        // (func_ov002_020d5ed0). The eater's two "cannot count down" flags are clear, and its hold timer is the ROM's 90:
        // the state counts it down by MY button presses, so the ROM's own mash-to-escape works on this console.
        s32(me, kInMouth) = 0;
        ptrf(ep, kMouthObj) = me;
        const int entered = func_ov002_020d5f34(me, ep);
        char *l2 = ptrf(me, kInMouth);
        *(unsigned short *)(ep + kHoldTimer) = g.eat_hold ? (unsigned short)(g.eat_hold + 60) : 0x5a;
        *(unsigned char *)(ep + 0x708) = 0; *(unsigned char *)(ep + 0x709) = 0;
        const int r5 = entered ? _ZN6Player19func_ov002_020d5ed0Ev(me) : 0;
        g.rom_mouth = r5 != 0 && state_is(me, data_ov002_02110064) && ptrf(me, kInMouth) == ep;
        std::fprintf(stderr, "[phit] EAT enter: d5f34 %d, eater link after the state change %p, d5ed0 %d, now %d (me %p eater %p, my slot %d, eater seat %d)\n",
                     entered, (void *)l2, r5, g.rom_mouth ? 1 : 0, (void *)me, (void *)ep, (int)data_0209f250, c.attacker);
        if (!g.rom_mouth) ptrf(ep, kMouthObj) = nullptr;
    }
    if (!g.rom_mouth) *(unsigned char *)(me + kNoControl) = 1;           // TUNE: the flag my_body_may_act reads
    send_wire(c.attacker, kEatAck, 0, 0, 0, 0);
    broadcast_attach(c.attacker, 1);
    g.beat = g.frame;
    g.diag_left = 40;
    std::fprintf(stderr, "[phit] EAT hold begins, eater seat %d%s\n", c.attacker, g.rom_mouth ? " (ROM mouth state)" : "");
}

void eat_end(char *me, bool spit, bool escaped = false) {
    const int eater = g.hold.eater();
    char *ep = body_of_seat(eater);
    *(unsigned char *)(me + kNoControl) = 0;
    if (g.rom_mouth) {
        if (spit && state_is(me, data_ov002_02110064) && ptrf(me, kInMouth)) {
            pose_ghost_at_draw(ep);                             // the spit's launch point is the eater's body position
            func_ov002_020d5cec(me);                            // the ROM's own spit
            ++g.st.eats_spat;
        } else {
            if (state_is(me, data_ov002_02110064)) _ZN6Player11ChangeStateERNS_5StateE(me, data_ov002_0211013c);
            s32(me, kInMouth) = 0;
            ++g.st.eats_aborted;
        }
        if (ep && ptrf(ep, kMouthObj) == me) ptrf(ep, kMouthObj) = nullptr;
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(me, data_ov002_0211013c);
        if (spit) {
            // func_ov002_020d5cec's own numbers: out 80 units in front of the eater and 50 up, 24 units a frame
            // forward and 24 up, the flying anim (0x56), facing the way the eater faces
            int yaw = g.hold.spit_yaw();
            Seen es;
            if (ep && see(eater, &es) && es.have_ghost) {
                yaw = *player::facing(ep) & 0xFFFF;
                const double a = (double)yaw * 3.14159265358979323846 / 32768.0;
                *player::pos_x(me) = (es.x + (int)std::lround(std::sin(a) * 80)) * 0x1000;
                *player::pos_z(me) = (es.z + (int)std::lround(std::cos(a) * 80)) * 0x1000;
                *player::pos_y(me) = (es.y + 50) * 0x1000;
            }
            *(short *)(me + kYawAngle) = (short)yaw;
            _ZN6Player7SetAnimEji5Fix12IiEj(me, 0x56, 0x40000000, 0x1000, 0);
            *(int *)(me + kHorzSpeed) = 0x18000;
            *(int *)(me + kVertSpeed) = 0x18000;
            ++g.st.eats_spat;
        } else {
            ++g.st.eats_aborted;
        }
    }
    g.rom_mouth = false;
    g.hold.force_end();
    send_wire(eater, kEatRelease, 0, escaped ? 1 : 0, 0, 0);
    broadcast_attach(eater, 0);
    std::fprintf(stderr, "[phit] EAT %s (eater seat %d)\n", escaped ? "broke free" : spit ? "spat out" : "released early", eater);
}

void diag_links(const char *where, char *me);
void eat_tick(char *me) {
    if (!g.hold.active()) return;
    diag_links("hook", me);
    const int eater = g.hold.eater();
    Seen s;
    see(eater, &s);
    EatInput in;
    in.toggle_on = g.judge.tog.interact;
    const Place mp = my_place();
    const bool lv = !g.judge.rules.check_level || (s.in_level && s.at.level == mp.level && s.at.area == mp.area);
    in.eater_present = s.have_presence && lv && s.presence_age_ms <= (unsigned)kPresenceMaxAgeMs &&
                       s.have_ghost && s.ghost_age_ms <= (unsigned)kGhostMaxAgeMs;
    if (!g.judge.rules.check_level) in.eater_present = body_of_seat(eater) != nullptr;    // LAB: the rig's host stalls for seconds
    if (!in.eater_present && g.log > 0)
        std::fprintf(stderr, "[phit] eater gone? presence %d in_level %d lvl %d/%d area %d/%d age %u ghost %d ghost_age %u\n",
                     s.have_presence ? 1 : 0, s.in_level ? 1 : 0, s.at.level, mp.level, s.at.area, mp.area, s.presence_age_ms, s.have_ghost ? 1 : 0, s.ghost_age_ms);
    in.me_dead_or_menu = data_0209f2d8[0] == 2;
    if (g.rom_mouth) {
        char *ep = body_of_seat(eater);
        const bool in_state = state_is(me, data_ov002_02110064) && ptrf(me, kInMouth);
        if (!ep) in.eater_present = false;
        else if (in_state) {                                      // keep the eater's side of the link the state reads
            ptrf(ep, kMouthObj) = me;
            *(unsigned char *)(ep + 0x708) = 0; *(unsigned char *)(ep + 0x709) = 0;
        }
        if (!in_state && in.eater_present) {                       // the ROM's hold broke free by itself (mashing)
            eat_end(me, false, true);
            return;
        }
        // TONGUE1: St_InYoshiMouth_Main copies the eater's body position into mine every walk. Off it three passes in a row
        // means my state is not stepping (a message holds me, the walk is frozen): spit now on both consoles rather than sit
        // hidden on my own spot for the whole hold.
        if (in_state && ep && g.frame - g.hold.since() >= 2) {
            int dp[3] = {0, 0, 0}, drawn = 0;
            const int nb = (int)data_0209f21c < 16 ? (int)data_0209f21c : 16;
            for (int i = 0; i < nb; ++i) if (data_0209f394[i] == ep) drawn = port_adventure_ghost_drawpos(i, dp);
            const int dx = (*player::pos_x(me) - dp[0]) >> 12, dz = (*player::pos_z(me) - dp[2]) >> 12;
            if (drawn && (dx > 8 || dx < -8 || dz > 8 || dz < -8)) ++g.mouth_miss; else g.mouth_miss = 0;
            if (g.mouth_miss >= 3) {
                std::fprintf(stderr, "[phit] EAT the mouth state is not stepping (me %d,%d off the eater, 71e %u, message %u): spit now\n",
                             dx, dz, (unsigned)*(unsigned char *)(me + 0x71e), (unsigned)data_0209d660[0]);
                ++g.eats_unstepped;
                eat_end(me, true);
                return;
            }
        }
        if (g.frame - g.beat >= (unsigned)kAttachBeat) { g.beat = g.frame; broadcast_attach(eater, 1); }
        if (g.log > 0 && ep && (g.frame % 20) == 0)
            std::fprintf(stderr, "[phit] mash f=%u count=%d phase=%d eater timer=%u step=%d state_is_mouth=%d\n", g.frame,
                         _ZN6Player19func_ov002_020beb38Ev(me), (int)*(unsigned char *)(me + 0x6f5), (unsigned)*(unsigned short *)(ep + kHoldTimer),
                         (int)*(unsigned char *)(me + 0x6e3), state_is(me, data_ov002_02110064) ? 1 : 0);
    }
    if (g.log > 0 && ((g.frame - g.hold.since()) % 5) == 0) {
        // TONGUE1: where am I, and where is the eater DRAWN on this screen (the follower's pose; 0 = not drawn this pass)
        char *ep = body_of_seat(eater);
        int dp[3] = {0, 0, 0}, drawn = 0;
        const int nb = (int)data_0209f21c < 16 ? (int)data_0209f21c : 16;
        for (int i = 0; i < nb; ++i) if (ep && data_0209f394[i] == ep) drawn = port_adventure_ghost_drawpos(i, dp);
        std::fprintf(stderr, "[phit] held f=%u +%u me (%d,%d,%d) opacity %u state %d | eater seat %d drawn %d at (%d,%d,%d) wire (%d,%d,%d)\n",
                     g.frame, g.frame - g.hold.since(), u(*player::pos_x(me)), u(*player::pos_y(me)), u(*player::pos_z(me)),
                     (unsigned)*(unsigned char *)(me + 0x6f5), port::vs_arena_state_index(me), eater, drawn,
                     u(dp[0]), u(dp[1]), u(dp[2]), s.x, s.y, s.z);
    }
    const EatStep step = g.hold.tick(g.frame, in);
    if (step == kEatHolding) {
        if (!g.rom_mouth) {
            // in the mouth: at the eater's DRAWN body (the ghost the attach rule also uses), the curled anim, no control
            char *ep = body_of_seat(eater);
            int ex = s.x << 12, ey = s.y << 12, ez = s.z << 12, eyaw = g.hold.spit_yaw();
            if (ep) eyaw = *player::facing(ep) & 0xFFFF;      // (the ghost body can sit parked: its position is not trusted, the hub's newest SYNG position is)
            const double a = (double)eyaw * 3.14159265358979323846 / 32768.0;
            *player::pos_x(me) = ex + (int)std::lround(std::sin(a) * g.eat_ahead) * 0x1000;
            *player::pos_z(me) = ez + (int)std::lround(std::cos(a) * g.eat_ahead) * 0x1000;
            *player::pos_y(me) = ey + g.eat_up * 0x1000;
            *(int *)(me + kHorzSpeed) = 0; *(int *)(me + kVertSpeed) = 0;
            *(unsigned char *)(me + kNoControl) = 1;
            _ZN6Player7SetAnimEji5Fix12IiEj(me, 0x3f, 0, 0x3000, 0);
        }
    } else if (step == kEatSpit) {
        eat_end(me, true);
    } else if (step == kEatAbort) {
        eat_end(me, false);
    }
}

void diag_links(const char *where, char *me) {
    if (g.diag_left <= 0) return;
    --g.diag_left;
    char *ep = g.hold.active() ? body_of_seat(g.hold.eater()) : (g.cv.active ? body_of_seat(g.cv.holder) : nullptr);
    std::fprintf(stderr, "[phit] diag %s f=%u me=%p state=%p mouthState=%d grabbedState=%d step=%d spd=%x pos=%d,%d,%d d0=%p 35c=%p other.360=%p other.358=%p other=%p other.pos=%d,%d,%d\n",
                 where, g.frame, (void *)me, *(void **)(me + 0x370), state_is(me, data_ov002_02110064) ? 1 : 0, state_is(me, data_ov002_0211016c) ? 1 : 0,
                 (int)*(unsigned char *)(me + 0x6e3), *(unsigned *)(me + kHorzSpeed), u(*player::pos_x(me)), u(*player::pos_y(me)), u(*player::pos_z(me)),
                 (void *)ptrf(me, kInMouth), (void *)ptrf(me, kGrabbedBy), ep ? (void *)ptrf(ep, kMouthObj) : nullptr,
                 ep ? (void *)ptrf(ep, kHeldObj) : nullptr, (void *)ep,
                 ep ? u(*player::pos_x(ep)) : 0, ep ? u(*player::pos_y(ep)) : 0, ep ? u(*player::pos_z(ep)) : 0);
}

// ---- the Wario hold: victim side -----------------------------------------------------------------
void carry_v_clear(char *me, bool broke) {
    const int h = g.cv.holder;
    if (g.cv.synth && me) *(unsigned char *)(me + kNoControl) = 0;
    char *hb = body_of_seat(h);
    if (hb) port::vs_grab_export_unlink(hb, me);
    if (me) u32(me, kFlags) &= ~(kFlagHeldOut | kFlagCarried);
    if (broke) send_wire(h, kGrabBreak, 0, 0, 0, 0);
    broadcast_attach(h, 0);
    g.cv = CarryV();
}

void grab_begin(char *me, const Claim &c) {
    char *hb = body_of_seat(c.attacker);
    if (!hb || g.cv.active || ptrf(me, kGrabbedBy)) { ++g.st.refused; return; }
    // the ROM's own order (the arena's GRABBED event): the holder's side first, then the victim's entry
    ptrf(hb, kHeldObj) = me;
    {
        unsigned &f = u32(me, kFlags);
        f &= ~(0x400u | 0x2000u);
        f |= kFlagCarried | kFlagHeldOut;
    }
    port::vs_grab_export_overlay(hb, me);
    // the ROM's gate in St_Grabbed's entry (func_ov002_020d82f0) refuses a body that is still in its knock (the
    // blink timer, the damage byte); the hold takes a STUNNED player, so those are down for the one call
    const unsigned short inv = *(unsigned short *)(me + player::kInvincibleTimer);
    *(unsigned short *)(me + player::kInvincibleTimer) = 0;
    *(unsigned char *)(me + 0x708) = 0;
    void *st_before = *(void **)(me + 0x370);
    const int ok = g.rom_grab ? func_ov002_020db5f4(me, hb) : 1;        // ROMSTATE9: =0 skips the ROM state (synthetic hold below)
    char *gb0 = ptrf(me, kGrabbedBy);
    ptrf(me, kGrabbedBy) = hb;       // the state change clears the link on this port (as it does the mouth link): set it again
    *(unsigned short *)(me + player::kInvincibleTimer) = inv;
    std::fprintf(stderr, "[phit] GRAB enter: db5f4 %d, holder link after the call %p (holder %p), state %p -> %p (St_Grabbed is %p)\n", ok, (void *)gb0, (void *)hb, st_before, *(void **)(me + 0x370), (void *)data_ov002_0211016c);
    if (!ok) {
        ptrf(hb, kHeldObj) = nullptr;
        u32(me, kFlags) &= ~(kFlagHeldOut | kFlagCarried);
        std::fprintf(stderr, "[phit] GRAB: the ROM refused St_Grabbed (state %d)\n", port::vs_arena_state_index(me));
        ++g.st.refused;
        return;
    }
    g.lab_done = true;
    g.cv.active = true; g.cv.holder = c.attacker; g.cv.f0 = g.frame;
    // the ROM's St_Grabbed did not take (the state change landed elsewhere on this port): hold the body by hand,
    // pinned to the holder's hands with no control, and let the holder's throw launch it
    g.cv.synth = !state_is(me, data_ov002_0211016c);
    if (g.cv.synth) {
        ptrf(me, kGrabbedBy) = nullptr;
        u32(me, kFlags) &= ~(kFlagHeldOut | kFlagCarried);
        *(unsigned char *)(me + kNoControl) = 1;
        std::fprintf(stderr, "[phit] GRAB: St_Grabbed did not take, synthetic hold\n");
    }
    g.beat = g.frame;
    send_wire(c.attacker, kGrabAck, 0, 0, 0, 0);
    broadcast_attach(c.attacker, 2);
    g.diag_left = 40;
    std::fprintf(stderr, "[phit] GRAB: held by seat %d (St_Grabbed entered)\n", c.attacker);
}

void carry_v_tick(char *me) {
    if (!g.cv.active) return;
    diag_links("hook", me);
    char *hb = body_of_seat(g.cv.holder);
    Seen s;
    see(g.cv.holder, &s);
    const Place mp = my_place();
    bool holder_ok = hb && s.have_presence && s.in_level && s.presence_age_ms <= (unsigned)kPresenceMaxAgeMs &&
                     s.at.level == mp.level && s.at.area == mp.area && s.have_ghost && s.ghost_age_ms <= (unsigned)kGhostMaxAgeMs;
    if (!g.judge.rules.check_level) holder_ok = hb != nullptr;         // LAB: the rig's host stalls for seconds
    if (!holder_ok) {
        std::fprintf(stderr, "[phit] GRAB: the holder is gone, out of the hold\n");
        if (ptrf(me, kGrabbedBy)) { pose_ghost_at_draw(hb); func_ov002_020db54c((unsigned char *)me, 0x10000, 0x10000, *(short *)(me + kYawAngle)); }
        carry_v_clear(me, true);
        return;
    }
    if (g.cv.synth) {
        // pinned to the holder's hands, no control; a hold nobody ends lets go after ten seconds
        const double a = (double)(*(short *)(hb + kYawAngle) & 0xFFFF) * 3.14159265358979323846 / 32768.0;
        // the hub's newest holder position (the holder's ghost body can sit parked, so its own position is not used)
        *player::pos_x(me) = (s.x + (int)std::lround(std::sin(a) * g.carry_ahead)) * 0x1000;
        *player::pos_z(me) = (s.z + (int)std::lround(std::cos(a) * g.carry_ahead)) * 0x1000;
        *player::pos_y(me) = (s.y + g.carry_up) * 0x1000;
        *(int *)(me + kHorzSpeed) = 0; *(int *)(me + kVertSpeed) = 0;
        *(unsigned char *)(me + kNoControl) = 1;
        if (g.frame - g.cv.f0 > 300) { *(unsigned char *)(me + kNoControl) = 0; carry_v_clear(me, true); }
        if (g.frame - g.beat >= (unsigned)kAttachBeat) { g.beat = g.frame; broadcast_attach(g.cv.holder, 2); }
        return;
    }
    if (ptrf(hb, kHeldObj) != me) ptrf(hb, kHeldObj) = me;
    u32(me, kFlags) |= kFlagCarried | kFlagHeldOut;
    if (g.log > 0 && (g.frame % 20) == 0)
        std::fprintf(stderr, "[phit] mash f=%u count=%d step=%d state_is_grabbed=%d\n", g.frame, _ZN6Player19func_ov002_020beb38Ev(me),
                     (int)*(unsigned char *)(me + 0x6e3), state_is(me, data_ov002_0211016c) ? 1 : 0);
    if (!ptrf(me, kGrabbedBy) || !state_is(me, data_ov002_0211016c)) {
        // my own St_Grabbed_Main broke free (the mash count ran out: it dropped the holder and threw me off)
        std::fprintf(stderr, "[phit] GRAB: broke free after %u frames\n", g.frame - g.cv.f0);
        if (ptrf(hb, kHeldObj) == me) ptrf(hb, kHeldObj) = nullptr;
        carry_v_clear(me, true);
        return;
    }
    if (g.frame - g.beat >= (unsigned)kAttachBeat) { g.beat = g.frame; broadcast_attach(g.cv.holder, 2); }
}

void grab_launch(char *me, int holder, bool swing, int yaw, int speed) {
    if (!g.cv.active || g.cv.holder != holder) return;
    char *hb = body_of_seat(holder);
    const int vert = swing ? 0x20000 : 0x10000;
    std::fprintf(stderr, "[phit] GRAB: %s by seat %d: speed 0x%x yaw %d\n", swing ? "swung off" : "thrown", holder, (unsigned)speed, yaw & 0xFFFF);
    u32(me, kFlags) &= ~(kFlagHeldOut | kFlagCarried);
    if (g.cv.synth) {
        *(unsigned char *)(me + kNoControl) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(me, data_ov002_0211013c);
        *(short *)(me + kYawAngle) = (short)yaw;
        *(int *)(me + kHorzSpeed) = speed;
        *(int *)(me + kVertSpeed) = vert;
    } else {
    if (!swing && speed >= 0x30000) u32(me, kFlags) |= kFlagThrown;
    pose_ghost_at_draw(hb);      // func_ov002_020db54c launches me from the holder's body position (a Player holder: +100 up): not from where its ghost is parked
    func_ov002_020db54c((unsigned char *)me, speed, vert, (short)yaw);
    }
    if (hb && ptrf(hb, kHeldObj) == me) ptrf(hb, kHeldObj) = nullptr;
    carry_v_clear(me, false);
    ++g.st.applied;
}

// ---- the Wario hold: holder side -----------------------------------------------------------------
void carry_h_end(const char *why, bool drop) {
    char *me = my_body();
    char *vb = body_of_seat(g.ch.victim);
    if (me && vb) {
        if (drop && ptrf(me, kHeldObj) == vb) _ZN6Player9DropActorEv(me);
        port::vs_grab_export_unlink(me, vb);
    }
    att_set(g.ch.victim, -1, 0);
    std::fprintf(stderr, "[phit] GRAB: hold of seat %d over (%s)\n", g.ch.victim, why);
    g.ch = CarryH();
}

void carry_h_tick(char *me) {
    if (!g.ch.active) return;
    {
        static int last_st = -1;
        if (g.log > 0 && g.state != last_st) {
            std::fprintf(stderr, "[phit] holder state %d -> %d (prev %d) f=%u\n", last_st, g.state, g.prev_state, g.frame);
            last_st = g.state;
        }
    }
    char *vb = body_of_seat(g.ch.victim);
    if (!vb) { g.ch = CarryH(); return; }
    Seen s;
    see(g.ch.victim, &s);
    if (g.judge.rules.check_level && (!s.have_presence || s.presence_age_ms > (unsigned)kPresenceMaxAgeMs || !s.have_ghost || s.ghost_age_ms > (unsigned)kGhostMaxAgeMs)) {
        carry_h_end("the victim is gone", true);
        return;
    }
    if (g.ch.phase == 0 && g.frame - g.ch.f0 > (unsigned)kGrabAckWait) { carry_h_end("no ack", true); return; }
    if (g.lab_swing > 0 && !g.swing_done && g.ch.phase == 1 && g.frame - g.ch.f0 >= (unsigned)g.lab_swing && ptrf(me, kHeldObj) == vb) {
        g.swing_done = true;
        _ZN6Player11ChangeStateERNS_5StateE(me, data_ov002_02110604);
        *(short *)(me + kSwingSpeed) = 0x1800;
        std::fprintf(stderr, "[phit] LAB: St_SwingPlayer forced on the holder\n");
    }
    if (ptrf(me, kHeldObj) != vb) {
        // the ROM let go: how? (St_Throw -> the throw, St_SwingPlayer -> the spin's release, else a soft drop)
        if (g.ch.phase == 0) { carry_h_end("let go before the ack", false); return; }
        unsigned speed = 0x10000;
        int angle = *(short *)(me + kYawAngle) & 0xFFFF;
        bool swing = false;
        if (g.prev_state == kStThrow || g.state == kStThrow) {
            speed = 0x30000;
        } else if (g.prev_state == kStSwing || g.state == kStSwing) {
            const int v = *(short *)(me + kSwingSpeed);
            const int av = v < 0 ? (short)(-v) : v;
            speed = (unsigned)(int)(((long long)av * 0x28000 + 0x800) >> 12);
            const short b = (short)(int)(((long long)av * 0x1c71 + 0x800) >> 12);
            const short yaw = *(short *)(me + kYawAngle);
            angle = (unsigned short)(short)(v < 0 ? (short)(yaw - b) : (short)(yaw + b));
            swing = true;
        }
        send_wire(g.ch.victim, swing ? kGrabSwing : kGrabThrow, angle, (int)(speed >> 8), 0, 0);
        std::fprintf(stderr, "[phit] GRAB: %s sent to seat %d (yaw %d, speed 0x%x)\n", swing ? "swing" : "throw", g.ch.victim, angle, speed);
        carry_h_end("let go", false);
    }
}

// ---- claims / replies in -------------------------------------------------------------------------
void handle_claim(int attacker, unsigned ikind, unsigned counter, unsigned stamp, int yaw, int x, int y, int z) {
    char *me = my_body();
    if (!me) return;
    Claim c;
    c.attacker = attacker; c.victim = comms_hub_my_seat(); c.ikind = ikind; c.counter = counter; c.stamp = stamp; c.yaw = yaw;
    c.x = u(x); c.y = u(y); c.z = u(z);
    Seen s;
    see(attacker, &s);
    Me m;
    m.at = my_place();
    const Pos3 hp = hist_pos(stamp);            // where I was when the attacker saw me (the claim's age in frames)
    m.x = stamp ? hp.x : u(*player::pos_x(me)); m.y = stamp ? hp.y : u(*player::pos_y(me)); m.z = stamp ? hp.z : u(*player::pos_z(me));
    m.busy = me_busy(me, ikind == kGrabCatch);
    m.eaten = g.hold.active() || g.cv.active;
    const char *rom_no = ikind == kEatClaim ? rom_eat_refusal(me) : nullptr;
    if (ikind == kEatClaim) {
        // TONGUE1: a tongue asks what the cartridge asks (func_ov002_020d6c60's gate, rom_eat_refusal), plus the port-only
        // reasons my body cannot step a mouth state at all (a message, a script, a cutscene) and a hold I am already in.
        // The state list is not the ROM's: it refused tongues the cartridge takes (MPJANK1).
        m.busy = true;
        if (rom_no) g_busy_why = rom_no;
        else if (me_in_message(me)) g_busy_why = "message";
        else if (data_0209f2d8[0] == 2) g_busy_why = "level_state";
        else if (*(const int *)data_0209fc48 != 0) g_busy_why = "cutscene";
        else { g_busy_why = ""; m.busy = false; }
        g_busy_state = port::vs_arena_state_index(me);
    }
    Effect fx;
    const Verdict v = g.judge.judge(c, s, m, g.frame, &fx);
    log_verdict(c, v);
    if (ikind == kEatClaim) {
        if (rom_no) ++g.eat_rom_no; else ++g.eat_rom_ok;
        std::fprintf(stderr, "[phit] eat claim f=%u verdict %s, the ROM's own gate: %s (state %d, blink %u, 708 %u, 709 %u, 71e %u, message %u)\n", g.frame,
                     verdict_name(v), rom_no ? rom_no : "would eat", port::vs_arena_state_index(me),
                     (unsigned)*(unsigned short *)(me + 0x6a0), (unsigned)*(unsigned char *)(me + 0x708), (unsigned)*(unsigned char *)(me + 0x709),
                     (unsigned)*(unsigned char *)(me + 0x71e), (unsigned)data_0209d660[0]);
    }
    if (v != kOk) return;
    // A believed claim waits for the frame hook. The pump runs inside the frame's tick, BEFORE and around the
    // actor walk, so changing my body's state from here is unsafe (measured: the ROM's mouth state found its
    // eater pointer cleared and faulted; a hold dropped after one frame). The hook is later in the SAME frame, after
    // the walk, so the wait is under a frame; the lag compensation is in the pose history, not in skipping it.
    if (g.pend) { std::fprintf(stderr, "[phit] claim dropped: one is already waiting for the frame hook\n"); return; }
    g.pend = true; g.pend_claim = c; g.pend_fx = fx;
}

enum : int { kLaterHurtSelf = 1, kLaterBreak = 2, kLaterLaunch = 3 };
void later_push(int what, int seat, bool swing = false, int yaw = 0, int speed = 0) {
    if (g.later_n >= 6) return;
    G::Later &l = g.later[g.later_n++];
    l.what = what; l.seat = seat; l.swing = swing; l.yaw = yaw; l.speed = speed;
}
void hurt_self_from_front(char *me) {            // the ROM hurts the eater when its mouthful breaks free
    int v[3] = { *player::pos_x(me), *player::pos_y(me), *player::pos_z(me) };
    const double a = (double)(*(short *)(me + kYawAngle) & 0xFFFF) * 3.14159265358979323846 / 32768.0;
    v[0] += (int)std::lround(std::sin(a) * 100) * 0x1000; v[2] += (int)std::lround(std::cos(a) * 100) * 0x1000;
    if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(me, v, 0, 0x8000, 0, 0, 0))
        *(unsigned short *)(me + player::kInvincibleTimer) = kBlinkFrames;
}
void later_run(char *me) {
    for (int i = 0; i < g.later_n; ++i) {
        const G::Later l = g.later[i];
        if (l.what == kLaterHurtSelf) hurt_self_from_front(me);
        else if (l.what == kLaterBreak) { if (g.ch.active && g.ch.victim == l.seat) carry_h_end("it broke free", true); }
        else if (l.what == kLaterLaunch) grab_launch(me, l.seat, l.swing, l.yaw, l.speed);
    }
    g.later_n = 0;
}

void handle_reply(int seat, unsigned ik, int yaw, int x, int y, int z) {
    (void)y; (void)z;
    switch (ik) {
    case kEatAck:
        if (g.book.on_ack(seat, g.frame)) std::fprintf(stderr, "[phit] EAT ack from seat %d\n", seat);
        break;
    case kEatRelease:
        if (g.book.on_release(seat)) {
            std::fprintf(stderr, "[phit] EAT release from seat %d%s\n", seat, u(x) == 1 ? " (it broke free)" : "");
            if (u(x) == 1) later_push(kLaterHurtSelf, seat);
        }
        att_set(seat, -1, 0);
        break;
    case kGrabAck:
        if (g.ch.active && g.ch.victim == seat && g.ch.phase == 0) {
            g.ch.phase = 1;
            att_set(seat, comms_hub_my_seat(), 2);
            std::fprintf(stderr, "[phit] GRAB: seat %d is in my hands\n", seat);
        }
        break;
    case kGrabBreak:
        later_push(kLaterBreak, seat);
        break;
    case kGrabThrow: case kGrabSwing:
        {   // MPSAFE1: the holder names the launch speed. Real throws are 0x30000, a spin at most ~0x40000: clamp the rest
            int sp = u(x);
            if (sp < 0) sp = 0;
            if (sp > (0x80000 >> 8)) sp = 0x80000 >> 8;
            later_push(kLaterLaunch, seat, ik == kGrabSwing, yaw, sp << 8);
        }
        break;
    case kAttach: {
        const int st = u(x);
        if (st < 0 || st > 2) break;
        att_set(seat, yaw & 0xFFFF, st);
        break;
    }
    case kHeldState: {
        if (seat < 0 || seat >= kHubPartySeats) break;
        const int v = u(x), kind = v & 0xF, eggs = (v >> 4) & 0xF, cls = u(y), slot = (short)yaw;
        if (kind > 2 || eggs > 5 || cls < 0 || cls >= 1024) break;
        G::HeldIn &h = g.held_in[seat];
        ++g.held_recv;
        if (h.kind != kind || h.cls != cls || h.slot != slot || h.eggs != eggs) {
            std::fprintf(stderr, "[phit] held: seat %d now holds %s (class %d slot %d), eggs %d\n", seat,
                         kind == 1 ? "an object in its hands" : kind == 2 ? "an object in its mouth" : "nothing", cls, slot, eggs);
            h.since = g.frame; h.pinned = nullptr;
        }
        h.kind = kind; h.cls = cls; h.slot = slot; h.eggs = eggs; h.until = g.frame + kHeldLife;
        break;
    }
    default: break;
    }
}

void on_hit(int seat, const unsigned char *d, int len, void *) {
    if (!g.on || len != (int)sizeof(HubPartyHitWire)) return;
    HubPartyHitWire w; std::memcpy(&w, d, sizeof w);
    const unsigned ik = seq_ikind(w.seq), ctr = seq_counter(w.seq), stamp = seq_stamp(w.seq);
    if (ikind_is_reply(ik)) { handle_reply(seat, ik, w.yaw, w.x, w.y, w.z); return; }
    handle_claim(seat, ik, ctr, stamp, w.yaw, w.x, w.y, w.z);
}

// ---- attacker side --------------------------------------------------------------------------------
int g_tongue_why = 0;   // TONGUE1: the last reason a scan of this tongue did not send (1 far, 2 height, 3 facing)
void track_pose(const char *me) {
    const int st = port::vs_arena_state_index(me);
    if (st != g.state) {
        if (g.state == kStYoshiPower && g.in_state >= kTongueFirst && !g.tongue_said) {
            if (g_tongue_why == 1) ++g.unsent_far; else if (g_tongue_why == 2) ++g.unsent_dy;
            else if (g_tongue_why == 3) ++g.unsent_facing; else ++g.unsent_none;
        }
        g.prev_state = g.state; g.state = st; g.in_state = 0;
        if (st == kStYoshiPower) { ++g.tongues; g.tongue_said = false; g_tongue_why = 0; }
    } else { ++g.in_state; }
}
// attacker_contact's own tests, with which one failed (0 = it reaches)
int contact_why(unsigned ikind, int mx, int my, int mz, int myaw, int tx, int ty, int tz) {
    const ReachRule r = reach_rule(ikind);
    const long long dy = (long long)my - ty;
    const int h = r.horiz - kLagAllow;
    if (horiz2(mx, mz, tx, tz) > sq(h)) return 1;
    if (dy < r.dy_lo || dy > r.dy_hi) return 2;
    return attacker_contact(ikind, mx, my, mz, myaw, tx, ty, tz) ? 0 : 3;
}

unsigned stamp_for(const HubSeatInfo &si) {
    const int rtt = si.rtt_ms >= 0 ? si.rtt_ms : 100;
    const unsigned ms = si.ghost_ms_ago + 75u + (unsigned)(rtt / 2);     // my view of him + the draw delay + the claim's flight
    unsigned f = ms / 33u;
    return f > 15u ? 15u : f;
}

// Try the ROM's own grab on the friend's ghost body: true = I am holding it now.
bool holder_try_grab(char *me, char *vb, int seat) {
    if (g.ch.active || ptrf(me, kHeldObj) || ptrf(vb, kGrabbedBy)) return false;
    u32(vb, kFlags) |= kFlagGrabbable;                 // the stun's bit, set for this one call (a ghost's Behavior never runs)
    const int r = _ZN6Player7TryGrabER8dActor_c(me, vb);
    u32(vb, kFlags) &= ~kFlagGrabbable;
    if (!r) { if (g.log > 0) std::fprintf(stderr, "[phit] GRAB: TryGrab refused (my state %d)\n", g.state); return false; }
    port::vs_grab_export_link(me, vb, false);
    g.ch.active = true; g.ch.victim = seat; g.ch.f0 = g.frame; g.ch.phase = 0;
    return true;
}

// LAB (SM64DS_PHIT_LAB_GATHER=dx,dz): every console but the host's puts its body next to the host's, seat * (dx, dz)
// away, every frame. The consoles run at different frame rates, so a picture at a fixed frame needs the bodies to
// be together at ANY frame. Never set outside a proof.
void lab_gather(char *me) {
    const int seat = comms_hub_my_seat();
    if (!g.gather || g.lab_done || seat <= 0 || g.hold.active() || g.cv.active || g.ch.active) return;
    Seen s;
    if (!see(0, &s) || !s.have_ghost || !s.in_level) return;
    const Place mp = my_place();
    if (s.at.level != mp.level || s.at.area != mp.area) return;
    *player::pos_x(me) = (s.x + g.gather_dx * seat) << 12;
    *player::pos_y(me) = s.y << 12;
    *player::pos_z(me) = (s.z + g.gather_dz * seat) << 12;
    *(int *)(me + kHorzSpeed) = 0; *(int *)(me + kVertSpeed) = 0;
}

void attacker_scan(char *me) {
    Pose p;
    p.state = g.state; p.prev_state = g.prev_state; p.frames_in_state = g.in_state;
    p.vy = *(const int *)(me + kVertSpeed);
    int ik = attack_for_pose(p);
    const int mx = u(*player::pos_x(me)), my = u(*player::pos_y(me)), mz = u(*player::pos_z(me));
    const int yaw = *(const short *)(me + kYawAngle) & 0xFFFF;
    const bool lab = g.forge_kind && !g.forged && g.frame >= g.forge_at;
    if (lab) ik = g.forge_kind == 'm' ? kMelee : g.forge_kind == 'p' ? kPound : g.forge_kind == 's' ? kStomp :
                  g.forge_kind == 'g' ? kGrabCatch : kEatClaim;
    if (ik < 0 || (ik == kEatClaim && g.book.open())) return;
    const bool wario = (s32(me, 8) & 7) == kLookWario;
    for (int seat = 0; seat < kHubPartySeats; ++seat) {
        if (seat == comms_hub_my_seat() || !party::seat_is_party(seat) || party::gate(seat) != party::kGateOk) continue;
        if (lab && g.lab_seat >= 0 && seat != g.lab_seat) continue;
        if (g.send_frame[seat] && g.frame - g.send_frame[seat] < (unsigned)kSendGapFrames) continue;
        Seen s;
        if (!see(seat, &s) || !s.have_ghost) continue;
        HubSeatInfo si; comms_hub_seat_info(seat, &si);
        const unsigned stamp = stamp_for(si);
        int kind = ik;
        if (lab) {
            // LAB: forge a claim from `units` away from the friend, whatever the real positions are. A forged
            // grab still needs the ROM's own TryGrab to take (my punch state), so it retries each frame.
            if (ik == kGrabCatch) {
                char *vb = body_of_seat(seat);
                if (!vb || !holder_try_grab(me, vb, seat)) return;
            }
            send_wire(seat, (unsigned)ik, yaw, s.x + g.forge_units, s.y, s.z, stamp);
            g.forged = true; g.send_frame[seat] = g.frame ? g.frame : 1;
            std::fprintf(stderr, "[phit] LAB forged ik=%d claim %d units from seat %d\n", ik, g.forge_units, seat);
            return;
        }
        // a Wario's punch on a STUNNED friend (his ghost shows a hurt animation) picks him up instead of shoving
        unsigned fanim = 0, fchar = 0;
        port::comms_sync_ghost_look(seat, &fanim, &fchar);
        if (kind == kMelee && g.grab_on && wario && !g.ch.active && ghost_tongue::is_hurt_anim(fanim)) kind = kGrabCatch;
        if (!attacker_contact((unsigned)kind, mx, my, mz, yaw, s.x, s.y, s.z)) {
            if (ik == kEatClaim) g_tongue_why = contact_why((unsigned)kind, mx, my, mz, yaw, s.x, s.y, s.z);
            if (ik == kEatClaim && g.log > 0 && g.frame - g.probe_frame >= 4) {
                // PROBE (item 7): why a tongue scan did not send, with the friend's character
                g.probe_frame = g.frame;
                const int want = angle_from_to(mx, mz, s.x, s.z);
                int fe = (want - yaw) & 0xFFFF; if (fe > 0x8000) fe -= 0x10000;
                std::fprintf(stderr, "[phit] eat-scan miss seat %d: his char %u, my (%d,%d,%d) his (%d,%d,%d) dh %d dy %d face_err 0x%x\n",
                             seat, fchar, mx, my, mz, s.x, s.y, s.z,
                             (int)std::lround(std::sqrt((double)horiz2(mx, mz, s.x, s.z))), my - s.y, fe);
            }
            continue;
        }
        if (kind == kGrabCatch) {
            char *vb = body_of_seat(seat);
            if (!vb || !holder_try_grab(me, vb, seat)) continue;
        }
        send_wire(seat, (unsigned)kind, yaw, mx, my, mz, stamp);
        g.send_frame[seat] = g.frame ? g.frame : 1;
        ++g.st.claims_sent;
        if (kind == kEatClaim) { g.book.sent(seat, g.frame); g.tongue_said = true; ++g.tongue_sent; }
        if (kind == kStomp) {
            *(int *)(me + kVertSpeed) = kBounceUp;             // my own bounce, whatever the friend's console decides
            g.squash_until[seat] = g.frame + 20;
            ++g.st.bounces;
        }
        if (kind == kMelee || kind == kPound) attacker_feedback(seat, s);     // instant feedback on my screen
        if (g.log > 0) std::fprintf(stderr, "[phit] claim sent ik=%d -> seat %d stamp=%u (my %d,%d,%d; his %d,%d,%d)\n", kind, seat, stamp, mx, my, mz, s.x, s.y, s.z);
        return;                                                // one claim per frame
    }
}

// ---- LAB (TONGUE1): the seek pad -----------------------------------------------------------------
// SM64DS_PHIT_LAB_SEEK=<seat>[,<near>]: this console walks its own body to that friend and uses the tongue, with
// nothing but DS pad bits written into PadData before the actor walk (Stage::CheckInput builds the Ctrl block from
// them at the top of the walk: vs_arena_eat.cpp rig_press says why the Ctrl words themselves are no use). No position,
// speed or facing is written: the ROM's own walk turns and moves the body. The D-pad is camera-relative, so the
// camera's forward is learned by holding UP (the body turns to it) and the handedness by holding LEFT once.
void pad_raw(unsigned short ds, bool edge) {
    const int slot = (int)data_0209f250;
    if (slot < 0 || slot > 3) return;
    data_020a0e58[slot * 2] |= ds;
    if (edge) {
        data_020a0e58[slot * 2 + 1] |= ds;
        *(unsigned short *)((char *)data_020a0e5a + slot * 4) |= ds;
    }
}
void pad_ctrl(unsigned short ctrl_mask, bool edge) {      // a Ctrl bit (0x0001 = attack) through CheckInput's own map
    const int slot = (int)data_0209f250;
    if (slot < 0 || slot > 3) return;
    const unsigned char mode = *(unsigned char *)(data_0209f498 + slot * 0x18 + 0x16);
    const unsigned short *map = data_0209214c[mode < 3 ? mode : 0];
    unsigned short ds = 0;
    for (int j = 0; j < 16; ++j)
        if (map[j] & ctrl_mask) ds |= (unsigned short)(1u << j);
    pad_raw(ds, edge);
}
enum : unsigned short { kDsRight = 0x10, kDsLeft = 0x20, kDsUp = 0x40, kDsDown = 0x80 };
void lab_seek(char *me) {
    if (g.seek_seat < 0 && g.seek_seat != -2) return;
    if (g.hold.active() || g.cv.active) return;
    int seat = g.seek_seat;
    if (seat == -2) {                           // "any": the first friend seat with a fresh ghost (seats are not stable per run)
        Seen t;
        for (int q = 0; q < kHubPartySeats && seat == -2; ++q)
            if (q != comms_hub_my_seat() && see(q, &t) && t.have_ghost) seat = q;
        if (seat == -2) return;
    }
    Seen s;
    if (!see(seat, &s) || !s.have_ghost || !s.in_level) return;
    const Place mp = my_place();
    if (s.at.level != mp.level || s.at.area != mp.area) return;
    ++g.seek_f;
    const int mx = u(*player::pos_x(me)), mz = u(*player::pos_z(me));
    const int yaw = *(const short *)(me + kYawAngle) & 0xFFFF;
    const double dist = std::sqrt((double)horiz2(mx, mz, s.x, s.z));
    const int want = angle_from_to(mx, mz, s.x, s.z);
    switch (g.seek_phase) {
    case 0:                                     // hold UP: the body turns to the camera's forward
        pad_raw(kDsUp, g.seek_t == 0);
        if (++g.seek_t >= 10) { g.seek_cup = yaw; g.seek_t = 0; g.seek_phase = 1; }
        return;
    case 1: {                                   // hold LEFT: which way is left
        pad_raw(kDsLeft, g.seek_t == 0);
        if (++g.seek_t >= 10) {
            const short d = (short)(yaw - g.seek_cup);
            g.seek_sign = d >= 0 ? 1 : -1;
            g.seek_t = 0; g.seek_phase = 2;
            std::fprintf(stderr, "[phit] LAB seek: camera forward 0x%04x, left is %s\n", g.seek_cup, g.seek_sign > 0 ? "+0x4000" : "-0x4000");
        }
        return;
    }
    case 2: {                                   // walk to him; near and facing him: the tongue
        int fe = (want - yaw) & 0xFFFF; if (fe > 0x8000) fe -= 0x10000;
        if (dist <= g.seek_near && fe > -0x1000 && fe < 0x1000 && g.state != kStYoshiPower) {
            pad_ctrl(0x0001, true);
            ++g.seek_tongues;
            std::fprintf(stderr, "[phit] LAB seek: tongue %llu at %d units, facing error 0x%x, f=%u\n", g.seek_tongues, (int)dist, fe & 0xFFFF, g.frame);
            g.seek_t = 0; g.seek_phase = 3;
            return;
        }
        // the D-pad direction (8 of them) nearest the way to him, in the camera's frame
        const short rel = (short)((want - g.seek_cup) * g.seek_sign);
        const int k = (((int)rel + 0x1000) & 0xFFFF) / 0x2000;       // 0 up, 1 up-left, 2 left, ... 7 up-right
        static const unsigned short dirs[8] = { kDsUp, kDsUp | kDsLeft, kDsLeft, kDsDown | kDsLeft, kDsDown, kDsDown | kDsRight, kDsRight, kDsUp | kDsRight };
        // close in: a short press only turns the body (and steps it a little)
        if (dist > g.seek_near * 0.6 || (g.seek_f & 3) == 0) pad_raw(dirs[k & 7], false);
        return;
    }
    default:                                    // after a tongue: hands off for a second and a half
        if (++g.seek_t >= 45) { g.seek_t = 0; g.seek_phase = (g.seek_tongues % 6) == 0 ? 0 : 2; }
        return;
    }
}

void exit_summary() {
    std::fprintf(stderr, "[phit:exit] tongues %llu: claim sent %llu, unsent: out of reach %llu, height %llu, facing %llu, nothing to say %llu"
                 " (seek pad tongues %llu)\n", g.tongues, g.tongue_sent, g.unsent_far, g.unsent_dy, g.unsent_facing, g.unsent_none, g.seek_tongues);
    std::fprintf(stderr, "[phit:exit] claims in %llu: applied %llu, refused %llu; eat claims the ROM's own gate would take %llu, refuse %llu\n",
                 g.st.claims_recv, g.st.claims_recv - g.st.refused, g.st.refused, g.eat_rom_ok, g.eat_rom_no);
    for (int v = 1; v < kVerdicts; ++v)
        if (g.st.refused_by[v & 15]) std::fprintf(stderr, "[phit:exit]   refused %s: %llu\n", verdict_name(v), g.st.refused_by[v & 15]);
    std::fprintf(stderr, "[phit:exit] eats began %llu, spat %llu, aborted %llu, spat early (mouth state not stepping) %u\n", g.st.eats_began, g.st.eats_spat, g.st.eats_aborted, g.eats_unstepped);
    std::fprintf(stderr, "[phit:exit] held state: sent %u, received %u, pinned frames %u, no local copy %u\n", g.held_sent, g.held_recv, g.held_pins, g.held_miss);
    std::fflush(stderr);
}

}  // namespace

bool active() { read_env(); return g.on; }

void on_coop_ctl(int seat, const unsigned char *pl, int len) {
    if (!g.on || !pl || len < (int)sizeof(HubPartyHitWire)) return;
    HubPartyHitWire w; std::memcpy(&w, pl, sizeof w);
    if (w.tag != kHubTagPartyHit || w.version != 1 || w.victim != (unsigned char)comms_hub_my_seat() || seat == w.victim) return;
    const unsigned ik = seq_ikind(w.seq), ctr = seq_counter(w.seq), stamp = seq_stamp(w.seq);
    if (ikind_is_reply(ik)) { handle_reply(seat, ik, w.yaw, w.x, w.y, w.z); return; }
    handle_claim(seat, ik, ctr, stamp, w.yaw, w.x, w.y, w.z);
}

// coop_world's SYNB dispatcher hands over the payload after the 12-byte head.
static void coop_sub(int seat, int, const unsigned char *pl, int len, void *) { on_coop_ctl(seat, pl, len); }

int squash_scale_for_seat(int seat) {
    if (seat < 0 || seat >= kHubPartySeats || !g.on || g.frame >= g.squash_until[seat]) return 0x1000;
    return 0x800;                                              // TUNE: half height
}

void held_after_follow() { if (g.on) held_after_follow_impl(); }

int attached_holder(int victim_seat, int *kind) {
    if (!g.on || victim_seat < 0 || victim_seat >= kHubPartySeats) return -1;
    const Attach &a = g.att[victim_seat];
    if (a.by < 0 || g.frame >= a.until) return -1;
    if (kind) *kind = a.kind;
    return a.by;
}

void attach_offsets(int kind, int *ahead, int *up) {
    if (kind == 2) { *ahead = g.carry_ahead; *up = g.carry_up; }
    else { *ahead = g.eat_ahead; *up = g.eat_up; }
}

const Stats &stats() { return g.st; }

}  // namespace party_hits
}  // namespace port

extern "C" void port_party_hits_frame(void) {
    using namespace port;
    using namespace port::party_hits;
    read_env();
    if (!g.installed) {
        /* comms_hub_room_kind() reads kHubRoomWorld until the hub is live, so
           deciding earlier would leave interactions off for the whole run.
           Re-evaluate every frame and latch only once the hub is live. */
        if (!comms_hub_active() || comms_hub_state() != kHubLive) return;
        g.installed = true;
        const HubRoomKind rk = comms_hub_room_kind();
        g.on = g.judge.tog.interact && (rk == kHubRoomParty || rk == kHubRoomCoop);
        g.judge.tog.room = rk == kHubRoomParty || rk == kHubRoomCoop;
        if (rk == kHubRoomParty) hub_on_kind(kHubPartyHit, on_hit, nullptr);
        if (rk == kHubRoomCoop && g.on) {              // hook 3: coop receives claims on SYNB 0x41
            const bool ok = coop_ctl_register(kCoopCtlSubtype, kCoopCtlSubtype, coop_sub, nullptr);
            std::fprintf(stderr, "[phit] coop SYNB 0x%02x %s\n", (unsigned)kCoopCtlSubtype, ok ? "registered" : "REGISTER FAILED");
        }
        if (g.on) std::atexit(exit_summary);
        std::fprintf(stderr, "[phit] %s (room %d, interact %d, damage %d)\n", g.on ? "on" : "off: not a party/coop room or interactions off",
                     (int)rk, g.judge.tog.interact ? 1 : 0, g.judge.tog.damage ? 1 : 0);
    }
    if (!g.on) return;
    ++g.frame;
    char *me = my_body();
    if (!me) return;
    track_pose(me);
    held_send(me);
    {   // my position this frame, for the claims that name an age
        Pos3 &r = g.ring[g.frame % kRingFrames];
        r.x = u(*player::pos_x(me)); r.y = u(*player::pos_y(me)); r.z = u(*player::pos_z(me));
    }
    g.book.tick(g.frame);
    later_run(me);
    if (g.pend) {
        g.pend = false;
        if (g.pend_fx.eat) eat_begin(me, g.pend_claim);
        else if (g.pend_fx.grab) grab_begin(me, g.pend_claim);
        else apply_effect(me, g.pend_claim, g.pend_fx);
    }
    att_links();
    eat_tick(me);
    carry_v_tick(me);
    carry_h_tick(me);
    lab_gather(me);
    if (!g.hold.active() && !g.cv.active) attacker_scan(me);
}

// Right before the actor walk. A ghost body is held and re-posed by the follower after the frame hook, and the
// ROM's mouth / hand states read their link to it during the walk: put the links back where the hook left them.
extern "C" void port_party_hits_prewalk(void) {
    using namespace port;
    using namespace port::party_hits;
    if (!g.on) return;
    char *me = my_body();
    if (!me) return;
    diag_links("prewalk-before", me);
    lab_seek(me);                                 // LAB (TONGUE1): inert unless SM64DS_PHIT_LAB_SEEK
    if (g.hold.active() && g.rom_mouth) {
        if (char *ep = body_of_seat(g.hold.eater())) {
            if (state_is(me, data_ov002_02110064)) {
                pose_ghost_at_draw(ep);                   // the mouth state copies the eater's body position every frame
                ptrf(me, kInMouth) = ep;
                ptrf(ep, kMouthObj) = me;
                *(unsigned char *)(ep + 0x708) = 0; *(unsigned char *)(ep + 0x709) = 0;
            }
        }
    }
    if (g.cv.active && state_is(me, data_ov002_0211016c)) {
        if (char *hb = body_of_seat(g.cv.holder)) {
            pose_ghost_at_draw(hb);
            ptrf(hb, kHeldObj) = me;
            ptrf(me, kGrabbedBy) = hb;
            u32(me, kFlags) |= kFlagCarried | kFlagHeldOut;
        }
    }
}

/* SEAMCOOP1 (run after093, the one-process plan's M5b; BROADCAST-M5-1): a coop room left or entered inside a running game. The hub
   teardown forgets every hub_on_kind handler and the room's toggles come from the next room's names, so the whole state goes back to
   a process that never was in a room: the next frame with a live hub latches, registers and reads the env again. */
namespace port { namespace party_hits {
void session_reset() { g = G(); }
}  // namespace party_hits
}  // namespace port
