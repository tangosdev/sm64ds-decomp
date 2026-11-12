// STARS HELD FROM LEVEL LOAD, AND THE WORLD THINGS ONLY ONE CONSOLE SAW. Run coopfix1, lane WORLDSTAR1.
//
// THE HOLES (audits AUDIT-WORLD H3 H4 H7, AUDIT-NPCS H5 H6, read on release/game-1.0.4):
//   1. Klepto with a silver star (d_a_jango.cpp): every console's Klepto holds its own silver from level load. A hit on
//      one console dropped that console's silver (announced, so friends got a copy), but the friends' Klepto still held
//      theirs, and a friend's later hit dropped a second silver: two silvers from one Klepto.
//   2. Stars held from load (Klepto's power star, the eel's tail star, the Ukiki cage's star): once the room's star was
//      out (announced by the console that released it) the held copy stayed on every friend's screen.
//   3. The JRB eel (d_a_moray.cpp) lets go of its tail star for the CLOSEST player, and that can be a friend's drawn
//      body: my console released the star and filmed it (camera + freeze) while my player was far away.
//   4. The Ukiki's cage (d_a_obj_hm_bskt.cpp, d_a_mky.cpp): the catch and the cage opening ran on the catcher's screen
//      only; a friend kept a closed cage with the star inside.
//   5. The mother penguin (d_a_pg_mthr.cpp): the hand-over ran on the returner's screen only; a friend's mother still
//      asked for her baby and a friend could do the hand-over again.
//
// WHAT THIS FILE DOES (coop rooms only; solo, party and VS never reach it, and the src hooks answer the cartridge's way
// outside a coop room):
//   * A held star is let go here once the room's star of its number is out (hal/coop_spawn.cpp coop_spawn_star_released:
//     a star of the release parameter was announced in this visit, or taken). The holder lets go the way its own code
//     does, without making a star: Klepto (mHeldActorID cleared, the star removed, the 30-frame timer), the eel
//     (mStarUniqueID cleared, the star removed), the cage (it is pushed and falls the way the Ukiki makes it fall; the
//     landing makes no star here: port_coop_ws_cage_land). The Ukiki of that cage is removed with it, as the catcher's
//     is once its cage has landed. This uses only records a 1.0.4 console already sends (SPAWN / GONE of coop_spawn).
//   * The mother penguin: once her star (number | 0x40) is out, mGaveStar is set here, so she no longer asks for her
//     baby and a hand-over here gives nothing.
//   * The eel's release asks port_coop_ws_moray_body: in a coop room the distance is measured to MY body only.
//   * Klepto hit by a FRIEND's drawn body (port_coop_ws_jango_hit): his own console runs his hit; mine keeps its cargo
//     and lets it go when his star is heard (power star) or his KLEPTO_SILVER arrives (silver).
//   * SYNW 0x6c KLEPTO_SILVER (new, 8 bytes): "the Klepto of placed slot N let go of its silver star here". Sent 3 times
//     by the console whose own hit dropped it, and again when a new member joins the group; a console that hears it lets
//     its own Klepto's silver go with no star made. A 1.0.4 console drops the unknown sub-type (each world message is
//     its own datagram, nothing else is lost): next to a 1.0.4 friend the old behaviour stays.
//
// NOT DONE HERE: two consoles hitting their Klepto inside one network delay each drop a silver (both are announced).
// The Ukiki's catch itself and the mother's walk are still drawn per console.
//
// PORT_HOST_ABI. Actor offsets are the ROM's (32-bit host): uniqueID +0x04, param1 +0x08, class +0x0c, dead +0x0e/+0x0f,
// position +0x5c, mHorzSpeed +0x98, mVertAccel +0x9c, mTerminalVelocity +0xa0, area +0xcc, placed slot +0xce; the eel's
// mVariant +0x408, mStarParam +0x414, mStarPos +0x43c, mStarUniqueID +0x49c; the cage's mStarActor +0x4dc; the mother's
// mPlayer +0x374, mMessageId +0x384, mGaveStar +0x386, mTalkStep +0x388; daStar_c mState +0x440, mCamSeq +0x496.
//
// ENVIRONMENT (proof knobs, all inert unless named):
//   SM64DS_COOP_WORLDSTAR=0        MUST-FAIL control: hooks answer the cartridge's way, nothing sent, nothing let go
//   SM64DS_COOP_WORLDSTAR_LOG=1    [coopws] lines (a census of the holders at each visit start)
//   SM64DS_COOP_WORLDSTAR_TRACE=N  every N frames one [coopws] trace line
//   SM64DS_WS_KLEPTO_SILVER=F      at frame F (before the spawn census arms, so like one placed at load) spawn a Klepto
//                                  carrying a silver star (param 0x0200, path 0; the cartridge places none in the courses
//                                  surveyed: SSL acts 1-8, THI): the ROM's own spawn and InitResources. Only in a course
//                                  whose own Klepto loaded the model (SSL act 2): a spawn where none is placed draws from
//                                  files that are not loaded
//   SM64DS_WS_KLEPTO_HIT=F[:friend]  at frame F the first Klepto runs its own contact test punched by my body (or the
//                                  first friend's drawn body): the ROM's HURT
//   SM64DS_WS_EEL=F[:friend]       from frame F for 4 frames put my body (or, inside the release check, the first friend's
//                                  drawn body) on my eel's tail star, so the ROM's own release check runs
//   SM64DS_WS_CAGE=F               at frame F the Ukiki's own cage push (spin, then the fall), as its talk does after a catch
//   SM64DS_WS_PG=F[:redo]          at frame F the mother's hand-over end (mGaveStar, her star), or with :redo her touch
//                                  test with my body holding her own baby (a second hand-over starts, or not)
//   SM64DS_WS_SHOT=f1,f2           ws_f<N>.bmp (the top screen) at those frames

#include "coop_wire.h"
#include "coop_world.h"
#include "coop_spawn.h"
#include "comms_hub.h"

#include "daJango_c.h"
#include "daPgMthr_c.h"

#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
char *_ZN8dActor_c10FindWithIDEj(unsigned id);
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, char *prev);
void *_ZN8dActor_c4NextEPKS_(const void *prev);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void _ZN7fBase_c18MarkForDestructionEv(char *c);
int port_adventure_ghost_seat(int slot);
extern int (*port_coop_ws_jango_hit)(void *klepto);
extern void *(*port_coop_ws_moray_body)(void *closest);
extern int (*port_coop_ws_cage_land)(void *cage);
extern const void *port_level_fb_ptr;
}
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace port {

enum : uint8_t { kWorldKleptoSilver = 0x6c };

#pragma pack(push, 1)
// KLEPTO_SILVER payload (after the 12-byte head), 8 bytes.
struct CoopWsWire {
    CoopKeyWire key;
    uint8_t  ver;        // 1
    uint8_t  kind;       // 1: the Klepto of this placed slot let go of its silver star
    uint16_t slot;       // the Klepto's placed-object slot, 0..511
};
#pragma pack(pop)
static_assert(sizeof(CoopWsWire) == 8, "klepto silver is 8 bytes");

namespace {

enum : int { kSeats = 16, kBodies = 16, kSlots = 512, kDropMax = 8 };
enum : int { kClsStar = 178, kClsSilver = 179, kClsPlayer = 191, kClsKlepto = 239, kClsEel = 242, kClsMother = 257,
             kClsBaby = 256, kClsCage = 103, kClsUkikiStar = 268 };
enum : unsigned { kResendMs = 100 };

bool g_env = false, g_off = false, g_log = false, g_regd = false;
int g_trace = 0;
unsigned g_lf = 0;
bool g_visit = false; int g_lv = -1, g_ac = -1;
unsigned g_mask = 0;

struct Stats { unsigned sent, recv, bad, foreign_hits, my_drops, klepto_let_go, eel_let_go, cage_dropped, cage_quiet, ukiki_removed,
               mother_given, eel_asked; } g_st = {};

// Klepto silver drops known in this visit (mine or heard), by the Klepto's placed slot
struct Drop { bool on; int slot; int from; int sends; unsigned next; };
Drop g_drop[kDropMax];

int rd32(const void *a, int off) { int v; std::memcpy(&v, (const char *)a + off, 4); return v; }
void wr32(void *a, int off, int v) { std::memcpy((char *)a + off, &v, 4); }
unsigned short a_cls(const char *a) { return *(const unsigned short *)(a + 0x0c); }
short a_slot(const char *a) { return *(const short *)(a + 0xce); }
unsigned a_uid(const char *a) { return *(const unsigned *)(a + 4); }
unsigned a_param(const char *a) { return *(const unsigned *)(a + 8); }
bool a_dead(const char *a) { return *(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2; }
// a Klepto's name on the wire: its placed slot (every console loads the same one), 511 for one with no slot (a spawned one)
int klepto_key(const char *a) { const int s = a_slot(a); return s >= 0 && s < kSlots - 1 ? s : kSlots - 1; }
unsigned now_ms() { return comms_hub_now_ms(); }

char *local_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < kBodies) ? (char *)data_0209f394[me] : nullptr;
}
bool is_body(const void *a) {
    const int nb = (int)data_0209f21c < kBodies ? (int)data_0209f21c : kBodies;
    for (int i = 0; i < nb; ++i) if (data_0209f394[i] == a) return true;
    return false;
}
char *first_friend_body() {
    const int nb = (int)data_0209f21c < kBodies ? (int)data_0209f21c : kBodies;
    for (int i = 0; i < nb; ++i)
        if (i != (int)data_0209f250 && data_0209f394[i] && port_adventure_ghost_seat(i) >= 0) return (char *)data_0209f394[i];
    return nullptr;
}
// a Player body holds / rides / carries this actor (the five Player links, hal/coop_spawn.cpp player_attached_to)
bool body_attached_to(const char *a) {
    const int nb = (int)data_0209f21c < kBodies ? (int)data_0209f21c : kBodies;
    for (int i = 0; i < nb; ++i) {
        const char *p = (const char *)data_0209f394[i];
        if (!p) continue;
        for (int off = 0x354; off <= 0x364; off += 4) if (*(const char *const *)(p + off) == a) return true;
    }
    return false;
}

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_WORLDSTAR");
    g_off = e && e[0] == '0' && !e[1];
    const char *l = std::getenv("SM64DS_COOP_WORLDSTAR_LOG");
    g_log = l && l[0] && l[0] != '0';
    if (const char *t = std::getenv("SM64DS_COOP_WORLDSTAR_TRACE")) g_trace = std::atoi(t);
    if (g_trace > 0) g_log = true;
}
bool live() { read_env(); return !g_off && coop_world_active(); }
bool in_group() { int lv = -1, ac = 0; return live() && coop_group_key(&lv, &ac); }

// ---- Klepto silver drops ---------------------------------------------------------------------------
Drop *drop_find(int slot) { for (Drop &d : g_drop) if (d.on && d.slot == slot) return &d; return nullptr; }
Drop *drop_add(int slot, int from, bool send) {
    Drop *d = drop_find(slot);
    if (!d) { for (Drop &x : g_drop) if (!x.on) { d = &x; break; } }
    if (!d) return nullptr;
    if (!d->on) { *d = Drop(); d->on = true; d->slot = slot; d->from = from; }
    if (send) { d->sends = 3; d->next = now_ms(); }
    return d;
}
void send_drops() {
    int lv = -1, ac = 0;
    if (!coop_group_key(&lv, &ac)) return;
    const unsigned t = now_ms();
    for (Drop &d : g_drop) {
        if (!d.on || d.sends <= 0 || (int)(t - d.next) < 0) continue;
        CoopWsWire w = {};
        w.key.level = (uint16_t)lv; w.key.act = (uint8_t)ac; w.key.reserved = 0;
        w.ver = 1; w.kind = 1; w.slot = (uint16_t)d.slot;
        if (coop_send_world(kWorldKleptoSilver, &w, (int)sizeof w)) ++g_st.sent;
        --d.sends; d.next = t + kResendMs;
    }
}

bool ws_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopWsWire)) return false;
    CoopWsWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.ver == 1 && w.kind == 1 && w.slot < kSlots;
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (sub != kWorldKleptoSilver || seat < 0 || seat >= kSeats || seat == comms_hub_my_seat() || !ws_validate(p, len)) { ++g_st.bad; return; }
    CoopWsWire w; std::memcpy(&w, p, sizeof w);
    int lv = -1, ac = 0;
    if (g_off || !coop_group_key(&lv, &ac) || (int)w.key.level != lv || (int)w.key.act != ac) return;
    ++g_st.recv;
    if (!drop_find((int)w.slot)) {
        drop_add((int)w.slot, seat, false);
        if (g_log) std::fprintf(stderr, "[coopws] lf=%u seat %d: the Klepto of slot %d let go of its silver star there\n", g_lf, seat, (int)w.slot);
    }
}

void exit_line() {
    std::fprintf(stderr, "[coopws:exit] sent %u received %u (bad %u); friend-body Klepto hits kept %u, my Klepto silver drops %u, "
                 "held stars let go: Klepto %u eel %u; cages dropped %u (quiet landings %u), Ukiki removed %u, mothers given %u, "
                 "eel release checks on my body %u%s\n", g_st.sent, g_st.recv, g_st.bad, g_st.foreign_hits, g_st.my_drops, g_st.klepto_let_go,
                 g_st.eel_let_go, g_st.cage_dropped, g_st.cage_quiet, g_st.ukiki_removed, g_st.mother_given, g_st.eel_asked,
                 g_off ? " (CONTROL SM64DS_COOP_WORLDSTAR=0)" : "");
}

void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    std::atexit(exit_line);
    if (g_off) return;
    const bool ok = coop_world_register(kWorldKleptoSilver, kWorldKleptoSilver, on_world);
    if (g_log || !ok) std::fprintf(stderr, "[coopws] SYNW sub-type 0x%02x %s\n", kWorldKleptoSilver, ok ? "registered" : "NOT registered (range taken)");
}

// ---- the src hooks -----------------------------------------------------------------------------------
// HURT enter of a Klepto: 1 = a friend's drawn body hit it (keep the cargo here). A hit by my body on a silver carrier is
// the room's drop: told to the group.
int jango_hit(void *kv) {
    if (!kv || !in_group()) return 0;
    daJango_c *k = (daJango_c *)kv;
    char *who = _ZN8dActor_c10FindWithIDEj((unsigned)k->mdCcAc_c1.otherOwner);
    const bool cargo = k->mHeldActorID != 0 && k->mCarriedItem == 1;
    if (who && is_body(who) && who != local_body()) {
        ++g_st.foreign_hits;
        if (g_log) std::fprintf(stderr, "[coopws] lf=%u Klepto slot %d hit by a FRIEND's body: %s kept here, his console drops it\n", g_lf,
                                (int)a_slot((char *)kv), cargo ? (k->mSilverStarFlag == 2 ? "its silver star" : "its star") : "nothing held,");
        return 1;
    }
    if (cargo && k->mSilverStarFlag == 2) {
        ++g_st.my_drops;
        drop_add(klepto_key((char *)kv), comms_hub_my_seat(), true);
        if (g_log) std::fprintf(stderr, "[coopws] lf=%u my hit: Klepto slot %d drops the room's silver star here, told to the group\n", g_lf, (int)a_slot((char *)kv));
    }
    return 0;
}

// the eel's release check: whose distance counts (and the SM64DS_WS_EEL proof knob's friend body)
struct EelKnob { int at = -2; bool fr = false; } g_eel;
void *moray_body(void *closest) {
    read_env();
    if (g_eel.fr && g_eel.at >= 0 && g_lv > 5 && g_lf >= (unsigned)g_eel.at && g_lf < (unsigned)g_eel.at + 4u && coop_world_active()) {
        char *fb = first_friend_body();
        char *eel = _ZN8dActor_c15FindWithActorIDEjPS_(kClsEel, nullptr);
        if (fb && eel && rd32(eel, 0x49c)) {
            wr32(fb, 0x5c, rd32(eel, 0x43c)); wr32(fb, 0x60, rd32(eel, 0x440)); wr32(fb, 0x64, rd32(eel, 0x444));
            closest = fb;
            if (g_lf == (unsigned)g_eel.at)
                std::fprintf(stderr, "[coopws] KNOB eel lf=%u: a FRIEND's drawn body put on my eel's tail star at (%d,%d,%d), the closest player now\n",
                             g_lf, rd32(eel, 0x43c) >> 12, rd32(eel, 0x440) >> 12, rd32(eel, 0x444) >> 12);
        }
    }
    if (!live()) return closest;
    char *me = local_body();
    if (closest && closest != me) {
        ++g_st.eel_asked;
        if (g_log && (g_st.eel_asked <= 3 || (g_st.eel_asked % 600) == 0))
            std::fprintf(stderr, "[coopws] lf=%u eel release check: the closest is a friend's body, my body is asked instead (%u so far)\n", g_lf, g_st.eel_asked);
    }
    return me ? me : closest;
}

// the cage's landing: 1 = the room's star is already out, make none here
int cage_land(void *c) {
    if (!c || !in_group()) return 0;
    const unsigned rel = (a_param((char *)c) & 0xf) | 0x20;
    if (!coop_spawn_star_released(rel)) return 0;
    ++g_st.cage_quiet;
    if (g_log) std::fprintf(stderr, "[coopws] lf=%u my cage landed: the room's star %#x is already out, none made here\n", g_lf, rel);
    return 1;
}

struct Hooks { Hooks() { port_coop_ws_jango_hit = jango_hit; port_coop_ws_moray_body = moray_body; port_coop_ws_cage_land = cage_land; } } g_hooks;

// ---- letting go of held copies -------------------------------------------------------------------------
void klepto_tick() {
    for (char *a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsKlepto, nullptr); a; a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsKlepto, a)) {
        if (a_dead(a)) continue;
        daJango_c *k = (daJango_c *)a;
        if (!k->mHeldActorID || k->mCarriedItem != 1) continue;
        char *held = _ZN8dActor_c10FindWithIDEj((unsigned)k->mHeldActorID);
        if (!held) continue;
        const char *why = nullptr;
        if (k->mSilverStarFlag == 2) {
            if (a_cls(held) == kClsSilver && drop_find(klepto_key(a))) why = "the room's silver star was dropped by a friend";
        } else if (a_cls(held) == kClsStar) {
            const unsigned rel = (a_param(held) & 0xf) | 0x40;
            if (coop_spawn_star_released(rel)) why = "the room's star is out";
        }
        if (!why) continue;
        // HURT enter's own let-go, with no star made and no sound
        _ZN7fBase_c18MarkForDestructionEv(held);
        k->mHeldActorID = 0;
        if (k->mSilverStarFlag == 2) k->mCarriedItem = 0;
        k->mTimer = 0x1e;
        ++g_st.klepto_let_go;
        if (g_log) std::fprintf(stderr, "[coopws] lf=%u my Klepto slot %d lets go of its %s (%s): removed here, none made\n", g_lf,
                                (int)a_slot(a), k->mSilverStarFlag == 2 ? "silver star" : "star", why);
    }
}

void eel_tick() {
    for (char *a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsEel, nullptr); a; a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsEel, a)) {
        if (a_dead(a)) continue;
        const unsigned uid = (unsigned)rd32(a, 0x49c);
        if (!uid) continue;
        char *star = _ZN8dActor_c10FindWithIDEj(uid);
        if (!star) continue;
        const unsigned rel = (*(const unsigned char *)(a + 0x414) & 0xf) | 0x40;
        if (!coop_spawn_star_released(rel)) continue;
        _ZN7fBase_c18MarkForDestructionEv(star);
        wr32(a, 0x49c, 0);
        ++g_st.eel_let_go;
        if (g_log) std::fprintf(stderr, "[coopws] lf=%u my eel lets go of its tail star: the room's star %#x is out, mine removed, none made\n", g_lf, rel);
    }
}

void cage_tick() {
    for (char *a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsCage, nullptr); a; a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsCage, a)) {
        if (a_dead(a) || rd32(a, 0x9c) != 0 || !rd32(a, 0x4dc)) continue;          // already falling, or nothing held
        const unsigned rel = (a_param(a) & 0xf) | 0x20;
        if (!coop_spawn_star_released(rel)) continue;
        // the Ukiki's own push (d_a_mky.cpp func_ov030_02112578 steps 6 and 9): spin, then fall; the landing removes the cage
        // and its star (func_ov030_0211124c), and port_coop_ws_cage_land keeps it from making a second star
        wr32(a, 0x98, 0x400);
        wr32(a, 0x9c, -0x2000);
        wr32(a, 0xa0, -0x3c000);
        ++g_st.cage_dropped;
        if (g_log) std::fprintf(stderr, "[coopws] lf=%u my cage drops: the room's star %#x is out (a friend's Ukiki opened it)\n", g_lf, rel);
        for (char *u = _ZN8dActor_c15FindWithActorIDEjPS_(kClsUkikiStar, nullptr); u; u = _ZN8dActor_c15FindWithActorIDEjPS_(kClsUkikiStar, u)) {
            if (a_dead(u)) continue;
            if (body_attached_to(u)) {
                if (g_log) std::fprintf(stderr, "[coopws] lf=%u my cage's Ukiki is held by a player: left, it goes once its cage is gone\n", g_lf);
                continue;
            }
            _ZN7fBase_c18MarkForDestructionEv(u);
            ++g_st.ukiki_removed;
            if (g_log) std::fprintf(stderr, "[coopws] lf=%u my cage's Ukiki removed (the catcher's goes once its cage lands)\n", g_lf);
        }
    }
}

void mother_tick() {
    for (char *a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsMother, nullptr); a; a = _ZN8dActor_c15FindWithActorIDEjPS_(kClsMother, a)) {
        if (a_dead(a) || *(const unsigned char *)(a + 0x386)) continue;
        const unsigned rel = (a_param(a) & 0xf) | 0x40;
        if (!coop_spawn_star_released(rel)) continue;
        *(unsigned char *)(a + 0x386) = 1;
        ++g_st.mother_given;
        if (g_log) std::fprintf(stderr, "[coopws] lf=%u my mother penguin: her star %#x is out (a friend's hand-over), she gave it here too\n", g_lf, rel);
    }
}

// ---- census, trace, knobs ------------------------------------------------------------------------------
void census() {
    if (!g_log) return;
    int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned c = a_cls(a);
        if (c != kClsKlepto && c != kClsEel && c != kClsCage && c != kClsMother && c != kClsUkikiStar && c != kClsBaby) continue;
        ++n;
        char extra[96] = "";
        if (c == kClsKlepto) { daJango_c *k = (daJango_c *)a; std::snprintf(extra, sizeof extra, " carried %d silver %d held %d", k->mCarriedItem, k->mSilverStarFlag, k->mHeldActorID); }
        else if (c == kClsEel) std::snprintf(extra, sizeof extra, " variant %d starparam %d star %d", rd32(a, 0x408), *(const unsigned char *)(a + 0x414), rd32(a, 0x49c));
        else if (c == kClsCage) std::snprintf(extra, sizeof extra, " star %p", (void *)(intptr_t)rd32(a, 0x4dc));
        else if (c == kClsMother) std::snprintf(extra, sizeof extra, " gave %d", *(const unsigned char *)(a + 0x386));
        std::fprintf(stderr, "[coopws] census lv=%d act=%d cls %u slot %d uid %u param %#x at (%d,%d,%d)%s\n", g_lv, g_ac, c, (int)a_slot(a),
                     a_uid(a), a_param(a), rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12, extra);
    }
    std::fprintf(stderr, "[coopws] census lv=%d act=%d: %d holders\n", g_lv, g_ac, n);
}

void trace(char *me) {
    if (g_trace <= 0 || (g_lf % (unsigned)g_trace) != 0) return;
    char line[900]; int k = 0;
    k += std::snprintf(line + k, sizeof line - k, "[coopws] trace lf=%u me at (%d,%d,%d) nocontrol %d", g_lf, rd32(me, 0x5c) >> 12,
                       rd32(me, 0x60) >> 12, rd32(me, 0x64) >> 12, (int)*(const unsigned char *)(me + 0x709));
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && k < (int)sizeof line - 100; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (a_dead(a)) continue;
        const unsigned c = a_cls(a);
        if (c == kClsKlepto) { daJango_c *kl = (daJango_c *)a; k += std::snprintf(line + k, sizeof line - k, " | klepto carried %d silver %d holds %d", kl->mCarriedItem, kl->mSilverStarFlag, kl->mHeldActorID); }
        else if (c == kClsEel) k += std::snprintf(line + k, sizeof line - k, " | eel star %d", rd32(a, 0x49c));
        else if (c == kClsCage) k += std::snprintf(line + k, sizeof line - k, " | cage y %d falling %d", rd32(a, 0x60) >> 12, rd32(a, 0x9c) != 0);
        else if (c == kClsMother) k += std::snprintf(line + k, sizeof line - k, " | mother gave %d talkstep %d", *(const unsigned char *)(a + 0x386), *(const unsigned char *)(a + 0x388));
        else if (c == kClsUkikiStar) k += std::snprintf(line + k, sizeof line - k, " | ukiki268");
        else if (c == kClsStar || c == kClsSilver)
            k += std::snprintf(line + k, sizeof line - k, " | %s %#x st %u cam %#x at (%d,%d,%d)", c == kClsStar ? "star" : "silver", a_param(a) & 0xff,
                               *(const unsigned *)(a + 0x440), (unsigned)*(const unsigned short *)(a + 0x496), rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12);
    }
    std::fprintf(stderr, "%s\n", line);
}

int knob_at(const char *name, bool *flag, const char *flagname) {
    const char *e = std::getenv(name);
    if (!e || !*e) return -1;
    if (flag) *flag = std::strstr(e, flagname) != nullptr;
    return std::atoi(e);
}

void knobs(char *me) {
    static int hit_at = -2, cage_at = -2, pg_at = -2, sil_at = -2; static bool hit_friend = false, pg_redo = false;
    if (hit_at == -2) {
        sil_at = knob_at("SM64DS_WS_KLEPTO_SILVER", nullptr, nullptr);
        hit_at = knob_at("SM64DS_WS_KLEPTO_HIT", &hit_friend, ":friend");
        cage_at = knob_at("SM64DS_WS_CAGE", nullptr, nullptr);
        pg_at = knob_at("SM64DS_WS_PG", &pg_redo, ":redo");
        g_eel.at = knob_at("SM64DS_WS_EEL", &g_eel.fr, ":friend");
    }
    if (g_lv <= 5) return;          // the knobs act in the course the row warps to, never in the castle the room starts in
    if (sil_at >= 0 && g_lf == (unsigned)sil_at) {
        const int pos[3] = {rd32(me, 0x5c), rd32(me, 0x60) + (600 << 12), rd32(me, 0x64)};
        char *k = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(kClsKlepto, 0x0200, pos, nullptr, *((signed char *)me + 0xcc), -1);
        std::fprintf(stderr, "[coopws] KNOB klepto silver lf=%u: %s\n", g_lf, !k ? "SPAWN FAILED (no Klepto here)" : "a Klepto carrying a silver star spawned");
        if (k) { daJango_c *kk = (daJango_c *)k;
            std::fprintf(stderr, "[coopws] KNOB klepto silver lf=%u: uid %u slot %d carried %d silver %d held %d\n", g_lf, a_uid(k), (int)a_slot(k), kk->mCarriedItem, kk->mSilverStarFlag, kk->mHeldActorID); }
    }
    if (hit_at >= 0 && g_lf == (unsigned)hit_at) {
        daJango_c *k = (daJango_c *)_ZN8dActor_c15FindWithActorIDEjPS_(kClsKlepto, nullptr);
        for (char *x = (char *)k; x; x = _ZN8dActor_c15FindWithActorIDEjPS_(kClsKlepto, x))      // the silver carrier first, if any
            if (((daJango_c *)x)->mSilverStarFlag == 2) { k = (daJango_c *)x; break; }
        char *who = hit_friend ? first_friend_body() : me;
        if (!k || !who) std::fprintf(stderr, "[coopws] KNOB klepto hit lf=%u: no Klepto (%p) or no body (%p)\n", g_lf, (void *)k, (void *)who);
        else {
            const int held0 = k->mHeldActorID;
            k->mdCcAc_c1.otherOwner = a_uid(who);
            k->mdCcAc_c1.hitFlags = 0x40;                  // a punch
            k->func_ov062_0211b51c();                      // the ROM's own contact test -> HURT
            std::fprintf(stderr, "[coopws] KNOB klepto hit lf=%u: Klepto slot %d punched by %s body: held %d -> %d, carried %d silver %d\n", g_lf,
                         (int)a_slot((char *)k), hit_friend ? "a FRIEND's" : "MY", held0, k->mHeldActorID, k->mCarriedItem, k->mSilverStarFlag);
        }
    }
    if (g_eel.at >= 0 && !g_eel.fr && g_lf >= (unsigned)g_eel.at && g_lf < (unsigned)g_eel.at + 4u) {
        char *eel = _ZN8dActor_c15FindWithActorIDEjPS_(kClsEel, nullptr);
        if (eel && rd32(eel, 0x49c)) {
            wr32(me, 0x5c, rd32(eel, 0x43c)); wr32(me, 0x60, rd32(eel, 0x440)); wr32(me, 0x64, rd32(eel, 0x444));
            wr32(me, 0x98, 0); wr32(me, 0xa8, 0);
            if (g_lf == (unsigned)g_eel.at)
                std::fprintf(stderr, "[coopws] KNOB eel lf=%u: my body put on my eel's tail star at (%d,%d,%d)\n", g_lf, rd32(eel, 0x43c) >> 12,
                             rd32(eel, 0x440) >> 12, rd32(eel, 0x444) >> 12);
        } else if (g_lf == (unsigned)g_eel.at) std::fprintf(stderr, "[coopws] KNOB eel lf=%u: no eel holding a star here\n", g_lf);
    }
    if (cage_at >= 0 && g_lf == (unsigned)cage_at) {
        char *c = _ZN8dActor_c15FindWithActorIDEjPS_(kClsCage, nullptr);
        if (!c) std::fprintf(stderr, "[coopws] KNOB cage lf=%u: no cage\n", g_lf);
        else {
            wr32(c, 0x98, 0x400); wr32(c, 0x9c, -0x2000); wr32(c, 0xa0, -0x3c000);
            std::fprintf(stderr, "[coopws] KNOB cage lf=%u: my Ukiki's push: the cage at (%d,%d,%d) spins and falls\n", g_lf, rd32(c, 0x5c) >> 12,
                         rd32(c, 0x60) >> 12, rd32(c, 0x64) >> 12);
        }
    }
    if (pg_at >= 0 && g_lf == (unsigned)pg_at) {
        daPgMthr_c *m = (daPgMthr_c *)_ZN8dActor_c15FindWithActorIDEjPS_(kClsMother, nullptr);
        if (!m) std::fprintf(stderr, "[coopws] KNOB pg lf=%u: no mother penguin\n", g_lf);
        else if (!pg_redo) {
            // the hand-over's end (func_ov018_02111fac step 2 with her own baby): the flag, then her star
            const int pos[3] = {rd32(m, 0x5c), rd32(m, 0x60), rd32(m, 0x64)};
            *((unsigned char *)m + 0x386) = 1;
            char *s = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(kClsStar, (a_param((char *)m) & 0xf) | 0x40, pos, nullptr, *((signed char *)m + 0xcc), -1);
            std::fprintf(stderr, "[coopws] KNOB pg lf=%u: my hand-over: mother gave her star %#x (%s)\n", g_lf, (a_param((char *)m) & 0xf) | 0x40, s ? "spawned" : "SPAWN FAILED");
        } else {
            char *baby = nullptr;
            for (char *b = _ZN8dActor_c15FindWithActorIDEjPS_(kClsBaby, nullptr); b; b = _ZN8dActor_c15FindWithActorIDEjPS_(kClsBaby, b))
                if (!a_dead(b) && a_param(b) == 0) { baby = b; break; }
            const int gave = *((unsigned char *)m + 0x386);
            void *p0 = (void *)(intptr_t)rd32(m, 0x374);
            m->func_ov018_02111968(me, baby);                      // her touch test: my body holding her own baby
            const bool talk = (void *)(intptr_t)rd32(m, 0x374) == (void *)me && p0 != (void *)me;
            std::fprintf(stderr, "[coopws] KNOB pg lf=%u: my body touches her holding her own baby (%s): gave %d -> %s\n", g_lf, baby ? "found" : "NO BABY",
                         gave, talk ? (*((short *)((char *)m + 0x384)) == 0xad ? "SECOND HAND-OVER STARTS (own-baby message)" : "a talk starts") : "no hand-over (she already gave)");
        }
    }
    if (const char *sh = std::getenv("SM64DS_WS_SHOT")) {
        for (const char *q = sh; *q;) {
            char *end = nullptr;
            const long n = std::strtol(q, &end, 10);
            if (end == q) break;
            if (g_lf == (unsigned)n) {
                char nm[64]; std::snprintf(nm, sizeof nm, "ws_f%ld.bmp", n);
                const void *lfb = ::port_level_fb_ptr;
                const bool w = lfb && ::ntr::ppu_write_bmp(nm, *(const ::ntr::Framebuffer *)lfb);
                std::fprintf(stderr, "[coopws] picture %s %s\n", nm, w ? "written" : "FAILED");
            }
            q = end; while (*q == ',' || *q == ' ') ++q;
        }
    }
}

void on_latched(int, int) {
    g_visit = false; g_lf = 0;
    std::memset(g_drop, 0, sizeof g_drop);
}

void on_frame(unsigned) {
    read_env();
    ensure_registered();
    int lv = -1, ac = 0;
    if (!coop_group_key(&lv, &ac)) return;
    char *me = local_body();
    if (!me) return;
    if (lv != g_lv || ac != g_ac) { g_lv = lv; g_ac = ac; on_latched(lv, ac); }
    ++g_lf;
    if (!g_visit && g_lf == 30) { g_visit = true; census(); }
    if (!g_off) {
        const unsigned mask = coop_group_member_mask();
        if (mask & ~g_mask)                       // a new member: tell him the drops this visit knows
            for (Drop &d : g_drop) if (d.on) { d.sends = 3; d.next = now_ms(); }
        g_mask = mask;
        klepto_tick();
        eel_tick();
        cage_tick();
        mother_tick();
        send_drops();
    }
    knobs(me);
    trace(me);
}

struct Reg {
    Reg() {
        if (!coop_on_frame(on_frame)) std::fprintf(stderr, "[coopws] frame callback NOT registered (table full)\n");
        coop_on_group_latched(on_latched);
    }
} g_reg;

}  // namespace
}  // namespace port
