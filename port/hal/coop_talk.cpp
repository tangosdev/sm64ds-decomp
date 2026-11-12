// COOP TALKS: one talker at a time per NPC, the NPC turned to the talker on every screen, and the talker's head look.
// Run coopfix1, lane NPCTALK1 (Tango's 11-11 play-test bugs 13, 14 and 17).
//
// WHAT WAS WRONG.
//   13. A player in a talk tilts his head toward the NPC: Player::func_ov002_020c47f4 writes the look angles (+0x762 / +0x764)
//       and the look mode +0x742 = 3, and the body pose (func_ov002_020e640c, inside func_ov002_020e4768) puts them on the head
//       and neck bones. A friend's body on my console never runs its own Behavior (comms_sync.cpp, the follower), so its look
//       block stayed at whatever it held when it was seated and the head stayed straight.
//   14. Every console runs every NPC. A Toad talking to Mario on Mario's console is a Toad standing idle on Luigi's, so Luigi
//       could start his own talk with the same Toad at the same time, and each screen showed the Toad facing its own talker.
//   17. King Bob-omb's intro talk is the same thing on a boss: two consoles, two intro talks, and two starts of one fight.
//
// WHAT THIS FILE DOES (coop rooms only; every entry returns at once outside a live coop room, so solo is the cartridge):
//   * Every frame it looks at MY body's talk partner (Player +0x368, set by Player::StartTalk, cleared by St_Talk_Cleanup).
//     While it is a placed NPC of a lock class (death-table slot +0xce, class +0x0c) I hold that NPC's TALK LOCK, and I send
//     one SYNW TALK datagram at 15 Hz: the slot, the class, the NPC's facing (+0x8e) as my screen shows it, and my head look
//     block (+0x742 mode, +0x75c..+0x767 the six look angles). The look block is also sent while my look mode is not 0 after
//     the talk (the ROM eases it back in mode 4), and three end datagrams follow when both are done.
//   * Player::StartTalk on MY body for an NPC whose lock a friend in my group holds is refused (the hook is the existing
//     StartTalk guard in coop_target.cpp, coop_talk_refuse below). A refused talk is simply not started: the NPC retries next
//     frame (an NPC that starts the talk itself, King Bob-omb) or waits for the next button press (Toad), the same as a
//     player who is not in reach. The lock outlives the friend's talk by kLingerMs (King Bob-omb: kLingerBossMs), so the
//     friend's result (the fight his talk started, through coop_boss.cpp) reaches my copy before my copy may talk again.
//   * While a friend holds the lock, my copy of a standing NPC of a facing class is turned to the facing his screen shows
//     (0x800 a frame, the step Toad turns with in his own talk). King Bob-omb is left to the boss sync (lane KBO2).
//   * The friend's look block is put on his body right before the follower poses it (coop_talk_ghost_look, one line in
//     comms_sync.cpp), so his head tilts toward the NPC on my screen as it does on his. With no fresh block the body's look
//     goes back to neutral once.
//
// WIRE. SYNW 0x74 TALK, payload after the 12-byte head: TalkWire (28 B). A 1.0.3 console drops it as an unknown sub-type
// (coop_world.cpp on_world, drop_unknown_sub) and never sends it: with a 1.0.3 friend nothing is locked, turned or tilted for
// him (1.0.3 behaviour), and he still sees nothing of mine, but my console keeps working as before.
//
// RACE. Two players who start a talk with the same NPC inside one network delay both get it (the lock is first come, and
// a talk the ROM has started is never cut). Both consoles log it ("race").
//
// ENVIRONMENT:
//   SM64DS_COOP_TALK=0        MUST-FAIL control: nothing is sent, applied or refused (the 1.0.3 behaviour). Detection and the
//                             log still run.
//   SM64DS_COOP_TALK_LOG=1    [cotalk] lines (talk start / end, lock in / out, refusals, facing, look)
//   SM64DS_COOP_TALK_CLS=a,b  extra lock classes
//   SM64DS_COOP_OWL_GRAB=F:me|friend[,...]  (NPCFIX1 proof) at frame F after Hoot is first seen this visit, run Hoot's own
//                             grab as if my body (or the first friend's drawn body) touched him; logs what it took
//   SM64DS_COOP_OWL_TRACE=N   (NPCFIX1 proof) every N frames one [cotalk] hoot trace line (state, rider, who holds him)

#include "coop_wire.h"
#include "coop_world.h"
#include "comms_hub.h"

#include "dActor_c.h"

#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
short Vec3_HorzAngle(const void *a, const void *b);
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, char *prev);
extern int (*port_coop_owl_refuse)(const void *owl, const void *player);
int port_owl_grab_test(void *owl, void *body);
}

namespace port {

void owl_knobs();   // NPCFIX1 proof knobs, below

namespace {

enum : uint8_t { kWorldTalk = 0x74 };
enum : int {
    kSeats = 4, kPlayerMax = 16, kSlots = 512,
    kSendMs = 66, kFreshMs = 600, kLockStaleMs = 10000, kLingerMs = 1200, kLingerBossMs = 4000, kEndSends = 3, kLookTailMs = 2000, kCoolMs = 2200,
    kClsKingBobOmb = 189, kClsHoot = 234, kOwlRider = 0x3cc,
};
enum : uint8_t { kFTalk = 1, kFLook = 2 };

#pragma pack(push, 1)
struct TalkWire {            // 28 B
    CoopKeyWire key;
    uint16_t slot;           // death-table slot 0..511 of the NPC I talk to, 0xffff = none
    uint16_t cls;            // its class id (cross-check), 0 with no NPC
    uint8_t  flags;          // kFTalk: I am in a talk with `slot`; kFLook: the look block below is live
    uint8_t  mode;           // Player +0x742, 0..4
    int16_t  ang;            // the NPC's facing (+0x8e) on my screen
    int16_t  look[6];        // Player +0x75c, +0x75e, +0x760, +0x762, +0x764, +0x766
    uint16_t seq;
    uint16_t reserved;       // 0
};
#pragma pack(pop)
static_assert(sizeof(TalkWire) == 28, "talk datagram is 28 bytes");

bool talk_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(TalkWire)) return false;
    TalkWire w; std::memcpy(&w, p, sizeof w);
    if (!coop_key_ok(w.key) || w.reserved != 0 || w.flags > 3 || w.mode > 4) return false;
    if (w.flags & kFTalk) return w.slot < kSlots && w.cls != 0 && w.cls < 1024;
    return w.slot == 0xffff && w.cls == 0;
}

// The lock classes: NPCs whose talk one player at a time should have (and King Bob-omb's intro talk). Signs, switches, doors
// and the other bosses' talks are left alone (signs are read by anyone; the boss lanes own the boss talks).
const unsigned short kLockCls[] = {
    181,  // BOB_OMB_BUDDY
    185,  // TOAD
    188,  // KOOPA_THE_QUICK (the talk before the race)
    189,  // KING_BOB_OMB (the intro talk starts the fight)
    234,  // HOOT_THE_OWL
    257,  // MOTHER_PENGUIN
    259,  // RACING_PENGUIN
    273,  // BIG_SNOWMAN_HEAD (NPCFIX1: his talk at the top of Cool Cool Mountain)
    274,  // BIG_SNOWMAN_BODY (NPCFIX1: the snowball's talk starts its roll)
};
// The classes my copy is turned to the talker for (they stand still in a talk; Hoot flies, King Bob-omb is the boss sync's).
const unsigned short kFaceCls[] = { 181, 185, 188, 257, 259, 274 };
unsigned short g_extra[8]; int g_nextra = 0;

bool g_env = false, g_on = true, g_log = false, g_regd = false;
bool g_kbo_once = true;      // SM64DS_COOP_TALK_KBO_ONCE=0: control, a friend's finished King talk does not end mine for the visit
unsigned g_clock_add = 0;    // SM64DS_COOP_TALK_CLOCK_ADD=ms (proof only): the clock this file reads, pushed forward
unsigned now_ms() { return comms_hub_now_ms() + g_clock_add; }
// King Bob-omb's intro is one talk per level visit for the whole group: once a friend's King talk ended, mine is refused
// until the level changes or that friend leaves the group (the fight itself is coop_boss.cpp's).
int g_kbo_seat = -1, g_kbo_slot = -1; int g_kbo_lv = -1, g_kbo_act = -1;
// My own cooldown: after my talk with an NPC ends I may not start the next one with it for kCoolMs, one second longer than
// the wait a friend gets (kLingerMs), so a friend who presses A in that second gets the NPC before the one who talked last.
int g_cool_slot = -1; unsigned g_cool_until = 0;
int g_my = -1;
unsigned g_frame = 0;

struct Mine {               // my own talk and look
    const char *actor; int slot; unsigned cls;
    unsigned last_send; int end_left; uint16_t seq;
    bool look_live; unsigned talk_end_t;
};
Mine g_me = { nullptr, -1, 0, 0, 0, 0, false, 0 };

struct Friend {             // what a friend last told me
    bool have; unsigned t;
    bool talk; int slot; unsigned cls; int16_t ang;
    bool look; uint8_t mode; int16_t look6[6];
    int linger_slot; unsigned linger_until; unsigned linger_cls; bool linger_set;   // linger_set: a real end time was taken
    bool look_applied;      // his body carries a look block from me (put back to neutral once it goes stale)
    bool raced;
};
Friend g_fr[kSeats];

struct Stats { unsigned long long sent, rx, bad, group, refused, faced, looks, races, starts, ends; };
Stats g_st = {};
unsigned g_refuse_log_t = 0;

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}
inline int16_t rd16(const char *a, int off) { int16_t v; std::memcpy(&v, a + off, 2); return v; }
inline void wr16(char *a, int off, int16_t v) { std::memcpy(a + off, &v, 2); }
inline int32_t rd32(const char *a, int off) { int32_t v; std::memcpy(&v, a + off, 4); return v; }

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_TALK");
    g_on = !(e && *e == '0');
    const char *l = std::getenv("SM64DS_COOP_TALK_LOG");
    g_log = l && *l && *l != '0';
    const char *ko = std::getenv("SM64DS_COOP_TALK_KBO_ONCE");
    g_kbo_once = !(ko && *ko == '0');
    if (const char *ca = std::getenv("SM64DS_COOP_TALK_CLOCK_ADD")) g_clock_add = (unsigned)std::strtoul(ca, nullptr, 0);
    if (const char *c = std::getenv("SM64DS_COOP_TALK_CLS")) {
        for (const char *p = c; *p && g_nextra < 8;) {
            char *end = nullptr; unsigned long v = std::strtoul(p, &end, 0);
            if (end == p) break;
            if (v > 0 && v < 1024) g_extra[g_nextra++] = (unsigned short)v;
            p = end; while (*p == ',' || *p == ' ') ++p;
        }
    }
    std::fprintf(stderr, "[cotalk] talk lock %s%s\n", g_on ? "on" : "OFF (control SM64DS_COOP_TALK=0)", g_log ? " log" : "");
}

bool lock_cls(unsigned c) {
    for (unsigned short k : kLockCls) if (k == c) return true;
    for (int i = 0; i < g_nextra; ++i) if (g_extra[i] == c) return true;
    return false;
}
// King Bob-omb's defeat talk (HP +0x500 at 0 or below) is never locked: coop_boss.cpp runs it on every console on purpose,
// and a lock there held the star back until the first talker closed his box.
bool kbo_defeat(const char *a) {
    return a && *(const unsigned short *)(a + 0x0c) == kClsKingBobOmb && rd32(a, 0x500) <= 0;
}
bool face_cls(unsigned c) {
    for (unsigned short k : kFaceCls) if (k == c) return true;
    return false;
}

char *local_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < kPlayerMax) ? (char *)data_0209f394[me] : nullptr;
}

bool member(int seat) {
    return seat >= 0 && seat < kSeats && seat != g_my && ((coop_group_member_mask() >> seat) & 1u);
}

bool my_key(CoopKeyWire *k) {
    int lv = 0, ac = 0;
    if (!coop_group_key(&lv, &ac)) return false;
    k->level = (uint16_t)lv; k->act = (uint8_t)ac; k->reserved = 0;
    return true;
}

// The actor of a placed slot and class on my console, null when it is not loaded.
char *find_slot(int slot, unsigned cls) {
    for (dActor_c *a = dActor_c::Next(nullptr); a; a = dActor_c::Next(a)) {
        const char *c = (const char *)a;
        if (rd16(c, 0xce) == slot && *(const unsigned short *)(c + 0x0c) == cls) return (char *)a;
    }
    return nullptr;
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (sub != kWorldTalk) return;
    read_env();
    if (!talk_validate(p, len)) { ++g_st.bad; return; }
    TalkWire w; std::memcpy(&w, p, sizeof w);
    ++g_st.rx;
    if (!g_on) return;
    CoopKeyWire k;
    if (!member(seat) || !my_key(&k) || k.level != w.key.level || k.act != w.key.act) { ++g_st.group; return; }
    Friend &f = g_fr[seat];
    const unsigned now = now_ms();
    const bool was_talk = f.have && f.talk;
    const int was_slot = f.slot;
    const unsigned was_cls = f.cls;
    f.have = true; f.t = now;
    f.talk = (w.flags & kFTalk) != 0;
    f.slot = f.talk ? (int)w.slot : -1;
    f.cls = f.talk ? w.cls : 0u;
    f.ang = w.ang;
    f.look = (w.flags & kFLook) != 0;
    f.mode = w.mode;
    std::memcpy(f.look6, w.look, sizeof f.look6);
    if (was_talk && (!f.talk || f.slot != was_slot)) {
        f.linger_slot = was_slot; f.linger_cls = was_cls;
        f.linger_until = (now + (was_cls == kClsKingBobOmb ? (unsigned)kLingerBossMs : (unsigned)kLingerMs)) | 1u;   // never 0: 0 is "no linger"
        f.linger_set = true;
        if (was_cls == kClsKingBobOmb && g_kbo_once && g_kbo_seat < 0) {
            int lv = -1, ac = -1; coop_group_key(&lv, &ac);
            g_kbo_seat = seat; g_kbo_slot = was_slot; g_kbo_lv = lv; g_kbo_act = ac;
            std::fprintf(stderr, "[cotalk] king talk done by seat %d (slot %d): my King intro is refused for this level visit w=%lld\n",
                         seat, was_slot, wall_ms());
        }
        if (g_log) std::fprintf(stderr, "[cotalk] lock out: seat %d done with slot %d cls %u (held for me %u ms more) w=%lld\n",
                                seat, was_slot, was_cls, f.linger_until - now, wall_ms());
    }
    if (f.talk && (!was_talk || f.slot != was_slot)) {
        if (g_log) std::fprintf(stderr, "[cotalk] lock in: seat %d talks to slot %d cls %u (npc facing %d) w=%lld\n",
                                seat, f.slot, f.cls, (int)f.ang, wall_ms());
        if (g_me.actor && g_me.slot == f.slot) {
            ++g_st.races; f.raced = true;
            std::fprintf(stderr, "[cotalk] race: seat %d and I both talk to slot %d cls %u (both talks started inside one delay) w=%lld\n",
                         seat, f.slot, f.cls, wall_ms());
        }
    }
}

void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    const bool ok = coop_world_register(kWorldTalk, kWorldTalk, on_world);
    if (g_log || !ok) std::fprintf(stderr, "[cotalk] SYNW sub-type 0x%02x %s\n", kWorldTalk, ok ? "registered" : "NOT registered (range taken)");
}

// A talk lock stands until the holder says his talk is over (or leaves my group): a friend whose game hitches or sits in a
// menu for a second keeps it. kLockStaleMs only covers a holder whose end datagrams were all lost.
bool friend_fresh(const Friend &f, unsigned now) { return f.have && (unsigned)(now - f.t) < (unsigned)kLockStaleMs; }

// The seat (not me) that holds the lock on `slot` now, -1 none.
int holder_of(int slot, unsigned now) {
    for (int s = 0; s < kSeats; ++s) {
        if (!member(s)) continue;
        const Friend &f = g_fr[s];
        if (friend_fresh(f, now) && f.talk && f.slot == slot) return s;
        if (f.linger_set && f.linger_slot == slot && (int)(f.linger_until - now) > 0) return s;
    }
    return -1;
}

void send(const char *body, bool talk, bool look) {
    TalkWire w = {};
    if (!my_key(&w.key)) return;
    w.slot = talk ? (uint16_t)g_me.slot : (uint16_t)0xffff;
    w.cls = talk ? (uint16_t)g_me.cls : (uint16_t)0;
    w.flags = (uint8_t)((talk ? kFTalk : 0) | (look ? kFLook : 0));
    w.ang = talk && g_me.actor ? rd16(g_me.actor, 0x8e) : (int16_t)0;
    if (look) {
        const unsigned char m = *(const unsigned char *)(body + 0x742);
        w.mode = m <= 4 ? m : (uint8_t)0;
        for (int i = 0; i < 6; ++i) w.look[i] = rd16(body, 0x75c + 2 * i);
    }
    w.seq = ++g_me.seq;
    if (coop_send_world(kWorldTalk, &w, sizeof w)) ++g_st.sent;
}

// NPCFIX1: the Hoot my body rides (daOwl_c mRider +0x3cc), null when none. A ride holds Hoot's lock like a talk, so a
// friend's console refuses his own grab of his copy (port_coop_owl_refuse) and his talk with it while I ride.
const char *my_ride(const char *body) {
    if (!body) return nullptr;
    for (char *a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsHoot, nullptr); a; a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsHoot, a))
        if (*(const char *const *)(a + kOwlRider) == body) return a;
    return nullptr;
}

void my_frame(unsigned now) {
    char *body = local_body();
    const char *ta = body ? *(const char *const *)(body + 0x368) : nullptr;
    if (!ta) ta = my_ride(body);
    int slot = -1; unsigned cls = 0;
    if (ta) {
        cls = *(const unsigned short *)(ta + 0x0c);
        slot = rd16(ta, 0xce);
        if (!lock_cls(cls) || slot < 0 || slot >= kSlots || kbo_defeat(ta)) { ta = nullptr; slot = -1; cls = 0; }
    }
    if (ta != g_me.actor) {
        if (g_me.actor) {
            ++g_st.ends;
            g_cool_slot = g_me.slot; g_cool_until = (now + (unsigned)kCoolMs) | 1u; g_me.talk_end_t = now;
            if (g_log) std::fprintf(stderr, "[cotalk] my talk ended: slot %d cls %u w=%lld\n", g_me.slot, g_me.cls, wall_ms());
        }
        if (ta) {
            ++g_st.starts;
            const int h = holder_of(slot, now);
            if (g_log) std::fprintf(stderr, "[cotalk] my talk started: slot %d cls %u%s w=%lld\n", slot, cls,
                                    h >= 0 ? " (a friend holds it: race or control)" : "", wall_ms());
            if (h >= 0 && g_on) { ++g_st.races; std::fprintf(stderr, "[cotalk] race: I started a talk with slot %d cls %u that seat %d holds w=%lld\n", slot, cls, h, wall_ms()); }
        }
        g_me.actor = ta; g_me.slot = slot; g_me.cls = cls;
        g_me.last_send = now - kSendMs;
    }
    if (!g_on || !body) return;
    const bool look = *(const unsigned char *)(body + 0x742) != 0 && (ta || g_me.look_live);
    // the look stays live from a talk until the ROM's own ease (mode 4) is over and the mode is 0 again, at most kLookTailMs
    if (ta) g_me.look_live = true;
    else if (*(const unsigned char *)(body + 0x742) == 0 || (unsigned)(now - g_me.talk_end_t) >= (unsigned)kLookTailMs) g_me.look_live = false;
    if (ta || look) {
        g_me.end_left = kEndSends;
        if ((unsigned)(now - g_me.last_send) >= (unsigned)kSendMs) { g_me.last_send = now; send(body, ta != nullptr, look); }
    } else if (g_me.end_left > 0 && (unsigned)(now - g_me.last_send) >= (unsigned)kSendMs) {
        g_me.last_send = now; --g_me.end_left; send(body, false, false);
    }
}

void face_frame(unsigned now) {
    for (int s = 0; s < kSeats; ++s) {
        Friend &f = g_fr[s];
        if (!member(s) || !friend_fresh(f, now) || !f.talk || !face_cls(f.cls)) continue;
        if (g_me.actor && g_me.slot == f.slot) continue;     // a race: my own talk turns my copy
        char *a = find_slot(f.slot, f.cls);
        if (!a || rd32(a, 0x98) != 0) continue;             // not loaded, or walking: its own steering stands
        const int16_t cur = rd16(a, 0x8e);
        const int16_t d = (int16_t)(f.ang - cur);
        if (!d) continue;
        const int16_t step = d > 0x800 ? (int16_t)0x800 : d < -0x800 ? (int16_t)-0x800 : d;
        wr16(a, 0x8e, (int16_t)(cur + step));
        ++g_st.faced;
        if (g_log && (d > 0x800 || d < -0x800 || !(g_st.faced % 30)))
            std::fprintf(stderr, "[cotalk] facing: slot %d cls %u turned toward seat %d's talk %d -> %d (want %d) w=%lld\n",
                         f.slot, f.cls, s, (int)cur, (int)(int16_t)(cur + step), (int)f.ang, wall_ms());
    }
}

void on_frame(unsigned) {
    read_env();
    const unsigned now = now_ms();
    ensure_registered();
    ++g_frame;
    g_my = comms_hub_my_seat();
    for (int s = 0; s < kSeats; ++s)
        if (!member(s) && (g_fr[s].have || g_fr[s].linger_until)) { const bool la = g_fr[s].look_applied; g_fr[s] = Friend{}; g_fr[s].linger_slot = -1; g_fr[s].look_applied = la; }
    if (g_kbo_seat >= 0) {
        int lv = -1, ac = -1;
        const bool k = coop_group_key(&lv, &ac);
        const char *why = !k || lv != g_kbo_lv || ac != g_kbo_act ? "level changed" : !member(g_kbo_seat) ? "that friend left" : nullptr;
        if (why) {
            std::fprintf(stderr, "[cotalk] king talk refusal cleared (%s) w=%lld\n", why, wall_ms());
            g_kbo_seat = -1; g_kbo_slot = -1;
        }
    }
    my_frame(now);
    if (g_on) face_frame(now);
    if (coop_world_active()) owl_knobs();
    if (g_log && g_frame % 1800u == 0)
        std::fprintf(stderr, "[cotalk] stats: sent %llu rx %llu bad %llu other-group %llu refused %llu faced %llu looks %llu "
                     "races %llu my talks %llu/%llu\n", g_st.sent, g_st.rx, g_st.bad, g_st.group, g_st.refused, g_st.faced,
                     g_st.looks, g_st.races, g_st.starts, g_st.ends);
}

struct Reg {
    Reg() { if (!coop_on_frame(on_frame)) std::fprintf(stderr, "[cotalk] frame callback NOT registered (table full): talk lock off\n"); }
} g_reg;

}  // namespace

// The StartTalk guard (coop_target.cpp) asks before my own body starts a talk: true = refuse, a friend holds this NPC.
bool coop_talk_refuse(void *body, void *actor) {
    if (!actor || !body || !coop_world_active()) return false;
    read_env();
    if (!g_on) return false;
    const char *a = (const char *)actor;
    const unsigned cls = *(const unsigned short *)(a + 0x0c);
    const int slot = rd16(a, 0xce);
    if (!lock_cls(cls) || slot < 0 || slot >= kSlots || kbo_defeat(a)) return false;
    if (*(void *const *)((const char *)body + 0x368) == actor) return false;   // already mine: the ROM answers
    g_my = comms_hub_my_seat();
    const unsigned now = now_ms();
    if (cls == kClsKingBobOmb && g_kbo_once && g_kbo_seat >= 0 && slot == g_kbo_slot && rd32(a, 0x500) > 0) {   // intro only (HP > 0), never the defeat talk
        ++g_st.refused;
        if (g_log && (unsigned)(now - g_refuse_log_t) >= 1000u) {
            g_refuse_log_t = now;
            std::fprintf(stderr, "[cotalk] refused: my King intro (seat %d's King talk ended this visit) w=%lld\n", g_kbo_seat, wall_ms());
        }
        return true;
    }
    if (g_cool_slot == slot && (int)(g_cool_until - now) > 0 && coop_group_member_mask()) {
        ++g_st.refused;
        if (g_log && (unsigned)(now - g_refuse_log_t) >= 1000u) {
            g_refuse_log_t = now;
            std::fprintf(stderr, "[cotalk] refused: my own cooldown on slot %d cls %u (%d ms left) w=%lld\n", slot, cls,
                         (int)(g_cool_until - now), wall_ms());
        }
        return true;
    }
    const int h = holder_of(slot, now);
    if (h < 0) return false;
    ++g_st.refused;
    if (g_log && (unsigned)(now - g_refuse_log_t) >= 1000u) {
        g_refuse_log_t = now;
        std::fprintf(stderr, "[cotalk] refused: my talk with slot %d cls %u (seat %d holds it; %llu refusals so far) w=%lld\n",
                     slot, cls, h, g_st.refused, wall_ms());
    }
    return true;
}

// NPCFIX1: Hoot's grab (d_a_owl.cpp, port_coop_owl_refuse) asks before it takes a player: 1 = refuse. A friend's drawn body is
// never taken on my console (his own console runs his grab), and my own body is not taken while a friend in my group
// holds Hoot (he talks to it or rides it).
struct OwlStats { unsigned foreign, held; } g_owl = {};

// ---- NPCFIX1 proof knobs (inert unless set) ----
struct OwlKnob { unsigned f; bool friend_body; bool done; };
OwlKnob g_owlk[6]; int g_nowlk = -1; int g_owl_trace = 0;
const char *g_owl_seen = nullptr; unsigned g_owl_lf = 0;

const char *body_name(const char *b) {
    if (!b) return "none";
    if (b == local_body()) return "MY body";
    return "a FRIEND's body";
}

void owl_knobs() {
    if (g_nowlk < 0) {
        g_nowlk = 0;
#ifndef PORT_ROM_CLEAN
        if (const char *e = std::getenv("SM64DS_COOP_OWL_GRAB")) {
            for (const char *q = e; *q && g_nowlk < 6;) {
                char *end = nullptr; const unsigned long f = std::strtoul(q, &end, 10);
                if (end == q || *end != ':') break;
                q = end + 1;
                const bool fr = std::strncmp(q, "friend", 6) == 0;
                g_owlk[g_nowlk++] = { (unsigned)f, fr, false };
                while (*q && *q != ',') ++q;
                while (*q == ',' || *q == ' ') ++q;
            }
        }
        if (const char *t = std::getenv("SM64DS_COOP_OWL_TRACE")) g_owl_trace = std::atoi(t);
#endif
    }
    if (!g_nowlk && g_owl_trace <= 0) return;
    char *owl = _ZN8dActor_c15FindWithActorIDEjPS_(kClsHoot, nullptr);
    if (owl != g_owl_seen) { g_owl_seen = owl; g_owl_lf = 0; for (int i = 0; i < g_nowlk; ++i) g_owlk[i].done = false; }
    if (!owl) return;
    ++g_owl_lf;
    for (int i = 0; i < g_nowlk; ++i) {
        OwlKnob &k = g_owlk[i];
        if (k.done || g_owl_lf != k.f) continue;
        k.done = true;
        char *body = local_body();
        if (k.friend_body) {
            body = nullptr;
            const int nb = (int)data_0209f21c < kPlayerMax ? (int)data_0209f21c : kPlayerMax;
            for (int b = 0; b < nb; ++b) if (data_0209f394[b] && data_0209f394[b] != local_body()) { body = (char *)data_0209f394[b]; break; }
        }
        if (!body) { std::fprintf(stderr, "[cotalk] KNOB hoot grab f=%u: no %s body\n", g_owl_lf, k.friend_body ? "friend's" : "own"); continue; }
        const char *before = *(const char *const *)(owl + kOwlRider);
        const int took = port_owl_grab_test(owl, body);
        const char *after = *(const char *const *)(owl + kOwlRider);
        std::fprintf(stderr, "[cotalk] KNOB hoot grab f=%u: Hoot touched by %s (rider before: %s) -> %s (rider now: %s) w=%lld\n", g_owl_lf,
                     k.friend_body ? "a FRIEND's body" : "MY body", body_name(before),
                     took ? "TAKEN, he carries it" : "not taken", body_name(after), wall_ms());
    }
    if (g_owl_trace > 0 && (g_owl_lf % (unsigned)g_owl_trace) == 0) {
        const int slot = rd16(owl, 0xce);
        const int h = slot >= 0 && slot < kSlots ? holder_of(slot, now_ms()) : -1;
        std::fprintf(stderr, "[cotalk] hoot trace f=%u: at (%d,%d,%d) rider %s | my ride lock %s | held by seat %d w=%lld\n", g_owl_lf,
                     rd32(owl, 0x5c) >> 12, rd32(owl, 0x60) >> 12, rd32(owl, 0x64) >> 12,
                     body_name(*(const char *const *)(owl + kOwlRider)), g_me.actor == owl ? "on" : "off", h, wall_ms());
    }
}
int owl_refuse(const void *owl, const void *pl) {
    if (!owl || !pl || !coop_world_active()) return 0;
    read_env();
    if (!g_on) return 0;
    const char *me = local_body();
    if (pl != me) {
        ++g_owl.foreign;
        if (g_owl.foreign <= 10 || (g_owl.foreign % 100) == 0)
            std::fprintf(stderr, "[cotalk] hoot: a friend's body touched my Hoot: not taken (only his own console takes him; %u so far) w=%lld\n",
                         g_owl.foreign, wall_ms());
        return 1;
    }
    const int slot = rd16((const char *)owl, 0xce);
    if (slot < 0 || slot >= kSlots) return 0;
    g_my = comms_hub_my_seat();
    const int h = holder_of(slot, now_ms());
    if (h < 0) return 0;
    ++g_owl.held;
    if (g_owl.held <= 10 || (g_owl.held % 100) == 0)
        std::fprintf(stderr, "[cotalk] hoot: my grab refused, seat %d holds Hoot (slot %d: his talk or his ride; %u so far) w=%lld\n",
                     h, slot, g_owl.held, wall_ms());
    return 1;
}
struct OwlHook { OwlHook() { port_coop_owl_refuse = owl_refuse; } } g_owl_hook;

// comms_sync.cpp's follower, right before it poses a friend's body: his head look from his last TALK datagram.
void coop_talk_ghost_look(int seat, void *body) {
    if (!body || seat < 0 || seat >= kSeats || !g_on || !g_env) return;
    Friend &f = g_fr[seat];
    char *b = (char *)body;
    const unsigned now = now_ms();
    if (f.have && f.look && (unsigned)(now - f.t) < (unsigned)kFreshMs && coop_world_active()) {
        *(unsigned char *)(b + 0x742) = f.mode;
        for (int i = 0; i < 6; ++i) wr16(b, 0x75c + 2 * i, f.look6[i]);
        if (!f.look_applied && g_log)
            std::fprintf(stderr, "[cotalk] look: seat %d head look on (mode %u, angles %d %d %d) w=%lld\n", seat, (unsigned)f.mode,
                         (int)f.look6[3], (int)f.look6[4], (int)f.look6[5], wall_ms());
        f.look_applied = true;
        ++g_st.looks;
    } else if (f.look_applied) {
        *(unsigned char *)(b + 0x742) = 0;
        for (int i = 0; i < 6; ++i) wr16(b, 0x75c + 2 * i, 0);
        f.look_applied = false;
        if (g_log) std::fprintf(stderr, "[cotalk] look: seat %d head look off (back to neutral) w=%lld\n", seat, wall_ms());
    }
}

}  // namespace port
