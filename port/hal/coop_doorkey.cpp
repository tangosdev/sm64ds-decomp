// DOORKEY1 (run coopfix1, bug 12): a friend using a KEY DOOR, as this screen shows it.
//
// ON THE CARTRIDGE (src/game/actors/d_a_door.cpp). The closed door's execute member (func_ov100_02144cf8) finds the player in its
// box with the key, Player::TryTalkToKeyDoor takes him, and the key-door state's enter member (func_ov100_02144a38) does three
// things on the spot: Player::OpenBigDoor, places him at the middle of the double door (75 units sideways and 110 units out from
// the leaf he touched, func_ov100_02145170, facing the door), and spawns an OBJ_KEY (class 282) 76 units to his side and 109
// ahead, turned with him, playing the key's open animation (daObjKey_c::func_ov089_0213115c(2)). The key is its own actor, so
// "the key in his hand" is that actor, not part of the player's model. When the player's animation is over the door writes
// its save bit and goes back to closed, and the player, still in its box, goes through it like any door: only THAT leaf swings.
//
// ON A FRIEND'S SCREEN nothing of this ran: the key actor is made only on the key user's console (coop_spawn announces it,
// but a key is refused outside the boss arenas, key_ok), and the friend's ghost is drawn where his frames put him. Here a
// friend's key-door kick (coop_switch, DOOR state 2, sent the moment his unlock starts, 1.0.3 sends it too) builds the same
// key prop with the same ROM call at the same place relative to the spot the ROM stands him on. His body needs no help: his
// frames carry the spot (the door step goes out as a snap) and every step of his walk to the keyhole. The key prop is never announced (coop_spawn skips a
// key that is playing an animation from its first frame) and nobody can take it: a key in an animation has no collider
// (daObjKey_c::Behavior returns before it updates one) and deletes itself when the animation ends, as on the cartridge.
//
// Nothing here runs outside a coop room: the kick only comes from a coop friend and the knobs are proof-only.
#include "coop_doorkey.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
void *_ZN8dActor_c4NextEPKS_(const void *prev);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void _ZN10daObjKey_c19func_ov089_0213115cEi(void *key, int anim);   // start the key's collect / open animation (2: the door's)
void Vec3_RotateYAndTranslate(void *out, void *base, short angle, void *v);
int _ZN8daDoor_c19func_ov100_021451c4EPvP6Player(char *door, void *node, char *player);   // the door's own "send him through"
extern int data_ov100_021488f4[];        // the door's swing state
extern signed char data_ov100_02148204[];   // door table, 16 bytes a variant: +8 stars, +9 key id, +0xa save bit
extern int data_ov089_02132894[];        // key kinds 0..6: their SharedFilePtr (coop_spawn's key_model_ready reads the same)
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern signed char data_0209f2f8;
int port_adventure_ghost_seat(int slot);
int port_adventure_ghost_drawpos(int slot, int out[3]);
}

namespace port {
namespace {

enum : int {
    kDoorCls   = 353,
    kKeyCls    = 282,
    kKeyAnimOpen = 2,
    kSeats     = 32,
    kBodies    = 16,
    kBoxX      = 75 << 12,    // func_ov100_021452e4's box, inclusive
    kBoxY      = 50 << 12,
    kBoxZ      = 110 << 12,
};

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

inline int rd(const void *p, int off) { return *(const int *)((const char *)p + off); }
inline unsigned short cls_of(const void *a) { return *(const unsigned short *)((const char *)a + 0x0c); }
inline bool dead(const void *a) { return *((const unsigned char *)a + 0x0f) || *((const unsigned char *)a + 0x0e) == 2; }
inline short yaw_of(const void *a) { return *(const short *)((const char *)a + 0x8e); }

bool g_log = false, g_probe = false;
const char *g_off = nullptr;
bool g_env = false;
void read_env() {
    if (g_env) return;
    g_env = true;
    const char *l = std::getenv("SM64DS_COOP_LOG");
    g_log = l && *l && *l != '0';
    g_probe = std::getenv("SM64DS_DOORKEY_PROBE") != nullptr;
    g_off = std::getenv("SM64DS_DOORKEY_OFF");
}

char *local_player() {
    const int mi = (int)data_0209f250;
    return (mi >= 0 && mi < 16) ? (char *)data_0209f394[mi] : nullptr;
}

// The friend's drawn place on this screen (the follower's), or false.
bool seat_drawpos(int seat, int out[3]) {
    for (int b = 0; b < kBodies; ++b)
        if (port_adventure_ghost_seat(b) == seat && port_adventure_ghost_drawpos(b, out)) return true;
    return false;
}

// p in the door's own frame (the offset rotated by minus its facing), as func_ov100_02145370 stores it.
void door_local(const char *door, const int p[3], int out[3]) {
    int d[3] = {p[0] - rd(door, 0x5c), p[1] - rd(door, 0x60), p[2] - rd(door, 0x64)};
    int zero[3] = {0, 0, 0};
    Vec3_RotateYAndTranslate(out, zero, (short)-yaw_of(door), d);
}

bool key_model_ready(int kind) {
    if (kind < 0 || kind > 6) return false;
    const int sfp = data_ov089_02132894[kind];
    return sfp != 0 && *(void *const *)((const char *)(intptr_t)sfp + 4) != nullptr;
}

struct TouchKnob { unsigned frame; int idx; bool done; };
TouchKnob g_touch[4]; int g_ntouch = 0;
bool g_touch_read = false;
int g_touch_lvl = -2;

// cursor probe: every Door near me whose model frame moves
struct Cur { const void *a; int cur; };
Cur g_cur[32];

}  // namespace

bool coop_doorkey_off(const char *what) {
    read_env();
    return g_off && std::strstr(g_off, what) != nullptr;
}

void coop_doorkey_friend_unlock(int seat, int idx) {
    read_env();
    if (seat < 0 || seat >= kSeats || idx < 0 || idx >= 64) return;
    const signed char *e = data_ov100_02148204 + idx * 16;
    if (e[8] >= 0 || e[9] < 0) return;            // not a key door
    int fp[3];
    if (!seat_drawpos(seat, fp)) {
        if (g_log) std::fprintf(stderr, "[doorkey] seat %d key door idx=%d: his body is not drawn here, nothing to show wall=%lld\n", seat, idx, wall_ms());
        return;
    }
    // the leaf he touched: the one whose box he stands in (the ROM's own test, func_ov100_021452e4, without the facing), else
    // the nearest leaf of the door
    char *best = nullptr; long long bd = 0; bool in_box = false;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != kDoorCls || dead(a) || rd(a, 8) != idx) continue;
        int l[3]; door_local(a, fp, l);
        const bool box = (l[0] <= kBoxX && l[0] >= -kBoxX && l[1] <= kBoxY && l[1] >= -kBoxY && l[2] <= kBoxZ && l[2] >= -kBoxZ);
        const long long dx = (long long)(fp[0] - rd(a, 0x5c)) >> 4, dz = (long long)(fp[2] - rd(a, 0x64)) >> 4;
        const long long d = dx * dx + dz * dz;
        if (!best || (box && !in_box) || (box == in_box && d < bd)) { best = a; bd = d; in_box = box; }
    }
    if (!best) {
        if (g_log) std::fprintf(stderr, "[doorkey] seat %d key door idx=%d: no leaf of it here wall=%lld\n", seat, idx, wall_ms());
        return;
    }
    // func_ov100_02145170 with the key door's offsets (75, 0, 110) / (75, 0, -110): the side he is on decides
    int l[3]; door_local(best, fp, l);
    int off[3] = {75 << 12, 0, (l[2] > 0 ? 110 : -110) << 12};
    const short yaw = (short)(l[2] > 0 ? yaw_of(best) + 0x8000 : yaw_of(best));
    int spot[3];
    Vec3_RotateYAndTranslate(spot, best + 0x5c, yaw_of(best), off);
    if (g_log)
        std::fprintf(stderr, "[doorkey] seat %d key door idx=%d leaf at %d,%d,%d (%s): the ROM stands him at %d,%d,%d yaw=%d; drawn at %d,%d,%d "
                     "(off by %d,%d,%d) wall=%lld\n", seat, idx, rd(best, 0x5c) >> 12, rd(best, 0x60) >> 12, rd(best, 0x64) >> 12,
                     in_box ? "he is in its box" : "nearest", spot[0] >> 12, spot[1] >> 12, spot[2] >> 12, (int)yaw,
                     fp[0] >> 12, fp[1] >> 12, fp[2] >> 12, (fp[0] - spot[0]) >> 12, (fp[1] - spot[1]) >> 12, (fp[2] - spot[2]) >> 12,
                     wall_ms());
    if (coop_doorkey_off("key")) return;
    // func_ov100_02144a38's key: (-76, 0, 109) from him, turned with him, his area, the open animation
    const int kind = e[9] & 7;
    if (!key_model_ready(kind)) {
        if (g_log) std::fprintf(stderr, "[doorkey] key kind %d model not loaded here: no key prop wall=%lld\n", kind, wall_ms());
        return;
    }
    int koff[3] = {-76 << 12, 0, 109 << 12};
    int kpos[3];
    Vec3_RotateYAndTranslate(kpos, spot, yaw, koff);
    const short rot[3] = {0, yaw, 0};
    char *me = local_player();
    const int area = me ? (int)*(const signed char *)(me + 0xcc) : -1;
    char *key = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(kKeyCls, (unsigned)e[9], kpos, rot, area, -1);
    if (key) _ZN10daObjKey_c19func_ov089_0213115cEi(key, kKeyAnimOpen);
    if (g_log)
        std::fprintf(stderr, "[doorkey] seat %d key prop %s kind=%d at %d,%d,%d area=%d wall=%lld\n", seat, key ? "built" : "NOT built", kind,
                     kpos[0] >> 12, kpos[1] >> 12, kpos[2] >> 12, area, wall_ms());
}

void coop_doorkey_frame(unsigned frames) {
    read_env();
    if ((int)data_0209f2f8 != g_touch_lvl) {
        g_touch_lvl = (int)data_0209f2f8;
        std::memset(g_cur, 0, sizeof g_cur);
        for (int i = 0; i < g_ntouch; ++i) g_touch[i].done = false;
    }
    if (!g_touch_read) {
        g_touch_read = true;
        // SM64DS_DOORKEY_TOUCH=F:idx[;F:idx] (proof): at group frame F the leaf of door idx nearest to me does to me what its own
        // Behavior does to a player in its box (func_ov100_021451c4 into the swing state): a plain walk through a door.
        if (const char *t = std::getenv("SM64DS_DOORKEY_TOUCH")) {
            const char *p = t;
            while (*p && g_ntouch < 4) {
                unsigned f = 0; int idx = 0, used = 0;
                if (std::sscanf(p, "%u:%i%n", &f, &idx, &used) < 2 || used <= 0) break;
                g_touch[g_ntouch++] = {f, idx, false};
                p += used;
                while (*p == ';' || *p == ',' || *p == ' ') ++p;
            }
        }
    }
    char *me = local_player();
    for (int i = 0; i < g_ntouch; ++i) {
        TouchKnob &k = g_touch[i];
        if (k.done || frames < k.frame || !me) continue;
        k.done = true;
        char *best = nullptr; long long bd = 0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (cls_of(a) != kDoorCls || dead(a) || rd(a, 8) != k.idx) continue;
            const long long dx = (long long)(rd(me, 0x5c) - rd(a, 0x5c)) >> 4, dz = (long long)(rd(me, 0x64) - rd(a, 0x64)) >> 4;
            if (!best || dx * dx + dz * dz < bd) { best = a; bd = dx * dx + dz * dz; }
        }
        const int did = best ? _ZN8daDoor_c19func_ov100_021451c4EPvP6Player(best, data_ov100_021488f4, me) : -1;
        std::fprintf(stderr, "[doorkey-knob] touch door idx=%d leaf=%d,%d,%d did=%d me=%d,%d,%d frame=%u wall=%lld\n", k.idx,
                     best ? rd(best, 0x5c) >> 12 : 0, best ? rd(best, 0x60) >> 12 : 0, best ? rd(best, 0x64) >> 12 : 0, did,
                     rd(me, 0x5c) >> 12, rd(me, 0x60) >> 12, rd(me, 0x64) >> 12, frames, wall_ms());
    }
    // SM64DS_DOORKEY_PROBE=1: one line per Door leaf near me whose model frame changed, so a row shows how many leaves move
    if (g_probe && me) {
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (cls_of(a) != kDoorCls || dead(a)) continue;
            const int dx = (rd(a, 0x5c) - rd(me, 0x5c)) >> 12, dz = (rd(a, 0x64) - rd(me, 0x64)) >> 12;
            if (dx > 900 || dx < -900 || dz > 900 || dz < -900) continue;
            const int cur = rd(a, 0x12c) >> 12;
            Cur *c = nullptr;
            for (Cur &q : g_cur) if (q.a == a) { c = &q; break; }
            if (!c) for (Cur &q : g_cur) if (!q.a) { q.a = a; q.cur = cur; c = &q; break; }
            if (!c || c->cur == cur) continue;
            std::fprintf(stderr, "[doorkey-probe] leaf idx=%d at %d,%d,%d yaw=%d cursor %d->%d node=%08x frame=%u\n", rd(a, 8),
                         rd(a, 0x5c) >> 12, rd(a, 0x60) >> 12, rd(a, 0x64) >> 12, (int)yaw_of(a), c->cur, cur, (unsigned)rd(a, 0x140), frames);
            c->cur = cur;
        }
    }
}

}  // namespace port
