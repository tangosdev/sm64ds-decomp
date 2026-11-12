// GHOSTDOOR1 (run mp1, wave 56): the door half of the hub ghost layer. See
// ghost_door.h for the wire meaning. The sender reads its own body and the
// door it is using; the watcher plays that door's MODEL animation, nothing
// else.
//
// WHY ONLY THE MODEL. The ROM's door runs on the local player alone:
// Door::Behavior -> func_ov100_02145370 reads data_0209f394[data_0209f250]
// and hands THAT body to the door's state callbacks, which call
// Player::OpenBigDoor, SetRealCharacter, ChangeArea and ShowArea. Pointed at a
// ghost's door those would move the watcher (his area, his character, his
// camera). The door's drawn pose is a pure function of its Animation cursor
// (ModelAnim::Render -> UpdateVerts -> UpdateBones at currFrame), so writing
// the cursor is the whole visual, with no game logic run at all.
#include "ghost_door.h"
#include "comms_hub.h"       // comms_hub_now_ms(): the probe lines' clock
#include "coop_doorkey.h"     // DOORKEY1: the 1.0.3 two-leaf arm (SM64DS_DOORKEY_OFF=leaf)

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
void *_ZN8dActor_c4NextEPKS_(const void *prev);
extern int _ZN6Player7IsStateERNS_5StateE(void *p, void *st);
extern char data_ov002_0211022c;      // Player's no-control state
// daStarGate_c::St_OpenClose_Main's own rotation of the leaf offset (pure math)
void Vec3_RotateYAndTranslate(void *out, void *base, short angle, void *v);
// TEST ONLY (SM64DS_GHOST_DOOR_UNSAFE): the door's own touch, below
extern int _ZN8daDoor_c19func_ov100_021451c4EPvP6Player(char *door, void *node, char *player);
extern int data_ov100_021488f4[];
extern char data_020a0ebc[];          // the matrix St_OpenClose_Main passes (opaque here)
extern int data_ov100_02148948[3];    // Vector3 x,y,z; St_OpenClose_Main reads .y/.z
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern signed char data_02092120;     // the area shown (-1 none)
extern signed char data_0209f2f8;     // the level
extern signed char data_02092110;     // PAINTTAG1: the pending entrance (-1 none): every exit's gate
void MulVec3Mat4x3(const void *in, const void *m, void *out);   // PAINTTAG1: the exit's local frame
}

namespace port {
namespace ghostdoor {
namespace {

enum {
    kDoorActorId = 353,                   // g_profile_DOOR (ov100), the plain door
    kGateActorId = 354,                   // g_profile_STAR_GATE (ov100), daStarGate_c
    kGateSlide   = 0x98,                  // dActor_c::mHorzSpeed: the leaf offset, 0 .. -0x8a000
    kGateOffs    = 0xa4,                  // dActor_c 0xa4/0xa8/0xac: the offset Render adds / subtracts
    kGateAngle   = 0x8e,                  // mAngleY
    kGateMax     = 0x8a000,               // the fully open slide (St_OpenClose_Init)
    kCursor      = 0x12c,                 // Door: ModelAnim 0xd4 + Animation 0x50 + currFrame 8
    kNumFrames   = 0x128,                 // ...numFrames << 12, flags in the top two bits
    kNode        = 0x140,                 // the state callback node
    kSearchR     = 300 << 12,             // a door this near the body (x, z)
    kSearchY     = 220 << 12,
    kMaxSeats    = 32,   // HUB32
};

Stats g_stats;

inline int rd(const void *p, int off) { return *(const int *)((const char *)p + off); }
inline unsigned char rb(const void *p, int off) {
    return *((const unsigned char *)p + off);
}

bool is_door(const void *a) {
    return *(const unsigned short *)((const char *)a + 0x0c) == kDoorActorId;
}

// The nearest door actor to (x, y, z), or null. Horizontal distance first;
// ties by the first found.
void *nearest_door(int x, int y, int z) {
    void *best = 0;
    long long bd = 0;
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a)) {
        if (!is_door(a)) continue;
        const long long dx = (long long)(rd(a, 0x5c) - x),
                        dz = (long long)(rd(a, 0x64) - z);
        const int dy = rd(a, 0x60) - y;
        if (dx > kSearchR || dx < -kSearchR || dz > kSearchR || dz < -kSearchR ||
            dy > kSearchY || dy < -kSearchY)
            continue;
        const long long d = (dx >> 4) * (dx >> 4) + (dz >> 4) * (dz >> 4);
        if (!best || d < bd) { best = a; bd = d; }
    }
    return best;
}

bool is_gate(const void *a) {
    return *(const unsigned short *)((const char *)a + 0x0c) == kGateActorId;
}

/* The nearest door-like actor to (x, y, z): a plain Door (353, animated by its
   model cursor) or a star gate (354, two leaves that slide apart). Same metric
   as nearest_door; the sender and the watcher use this one so they pick the
   same actor. */
void *nearest_any(int x, int y, int z) {
    void *best = 0;
    long long bd = 0;
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a)) {
        if (!is_door(a) && !is_gate(a)) continue;
        const long long dx = (long long)(rd(a, 0x5c) - x),
                        dz = (long long)(rd(a, 0x64) - z);
        const int dy = rd(a, 0x60) - y;
        if (dx > kSearchR || dx < -kSearchR || dz > kSearchR || dz < -kSearchR ||
            dy > kSearchY || dy < -kSearchY)
            continue;
        const long long d = (dx >> 4) * (dx >> 4) + (dz >> 4) * (dz >> 4);
        if (!best || d < bd) { best = a; bd = d; }
    }
    return best;
}

/* Wire byte for a star gate: kGateBase + the slide in 2-unit steps (0..69).
   Plain-door frames are clamped below kGateBase, so the byte says which class
   it is. */
unsigned char gate_frame(const void *g) {
    int slide = -rd(g, kGateSlide);
    if (slide < 0) slide = 0;
    if (slide > kGateMax) slide = kGateMax;
    return (unsigned char)(kGateBase + (slide >> 13));
}

// The leaf offset Render uses for a slide of `slide` (Fix12, >= 0): the same
// rotation St_OpenClose_Main applies to {mHorzSpeed, data_ov100_02148948.y,
// data_ov100_02148948.z} (d_a_star_gate.cpp), with the ROM's own matrix.
void gate_offset(const void *g, int slide, int out[3]) {
    int v[3] = {-slide, data_ov100_02148948[1], data_ov100_02148948[2]};
    Vec3_RotateYAndTranslate(out, data_020a0ebc,
                             *(const short *)((const char *)g + kGateAngle), v);
}

bool door_alive(const void *d) {
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a))
        if (a == d) return true;
    return false;
}

int door_frames(const void *d) {
    return (rd(d, kNumFrames) & ~(int)0xc0000000) >> 12;
}

}  // namespace

const Stats &stats() { return g_stats; }

unsigned char pack_area(int area) {
    if (area < 0) return 0;
    if (area >= 14) return 15;
    return (unsigned char)(area + 1);
}

/* DOORKEY1: the door the sender's current door step goes through (sender_frame), forgotten whenever he is in no door step,
   so two steps of the same kind within 500 ms do not reuse the first step's door. */
static const void *s_door = 0;
static unsigned char s_kind = kDoorNone;
static unsigned s_t = 0;

static unsigned char sender_kind_raw(const void *body) {
    if (!body) return kDoorNone;
    const char *c = (const char *)body;
    if (!_ZN6Player7IsStateERNS_5StateE((void *)body, &data_ov002_0211022c))
        return kDoorNone;
    switch (rb(c, 0x70a)) {                 // mNoCtrlKind
    case 7:                                  return kDoorPlain;
    case 0xa: case 0xb: case 0xe:            return kDoorTalk;
    case 0x10: case 0x11:                    return kDoorBig;
    default: break;
    }
    if (rb(c, 0x6e3) == 0x13 && rb(c, 0x70b) != 0)   // the big-door step
        return kDoorBig;
    return kDoorNone;
}

unsigned char sender_kind(const void *body) {
    const unsigned char kind = sender_kind_raw(body);
    if (kind == kDoorNone) { s_door = 0; s_kind = kDoorNone; }
    return kind;
}

unsigned char sender_frame(const void *body) {
    if (!body) return kDoorFrameNone;
    /* DOORKEY1: the door of a door step is the one nearest him when the step starts, kept until the step ends. Picked
       afresh every tick, a walk through one leaf of a double door on a slant came nearer the other leaf for a few ticks,
       and its frame 0 went out (the friends' copy of the leaf closed and opened again). */
    const unsigned now = comms_hub_now_ms();
    const unsigned char kind = sender_kind(body);
    const void *d = 0;
    if (kind != kDoorNone && kind == s_kind && s_door && now - s_t < 500u && door_alive(s_door) && !coop_doorkey_off("leaf"))
        d = s_door;
    else
        d = nearest_any(rd(body, 0x5c), rd(body, 0x60), rd(body, 0x64));
    s_door = kind != kDoorNone ? d : 0;
    s_kind = kind;
    s_t = now;
    if (!d) return kDoorFrameNone;
    if (is_gate(d)) return gate_frame(d);     // a star door has no model cursor
    int f = rd(d, kCursor) >> 12;
    if (f < 0) f = 0;
    if (f > kGateBase - 1) f = kGateBase - 1;
    return (unsigned char)f;
}

bool sender_hidden(const void *body) {
    if (!body) return false;
    /* SM64DS_GHOST_TEST_HIDE=<a>-<b>: a TEST hook for the proof rows, inert
       unset. Calls a..b of this function count as "hidden" (one call per
       sent tick), standing in for a cannon or a pipe the script cannot reach. */
    static int ta = -2, tb = 0, calls = 0;
    if (ta == -2) {
        const char *e = std::getenv("SM64DS_GHOST_TEST_HIDE");
        ta = -1;
        if (e && std::sscanf(e, "%d-%d", &ta, &tb) != 2) ta = -1;
    }
    ++calls;
    if (ta >= 0 && calls >= ta && calls <= tb) return true;
    const char *c = (const char *)body;
    if (rb(c, 0x6f5) == 0) return true;                          // mOpacity
    if (*(const unsigned *)(c + 0xb0) & 0x10u) return true;      // held / eaten
    return false;
}

void sender_probe(const void *body, unsigned frame, unsigned char kind) {
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_GHOST_DOOR_PROBE") ? 1 : 0;
    if (!on || !body) return;
    static int last_level = -2, last_area = -2;
    static unsigned last_node = 0xffffffffu;
    static int last_cur = -999;
    if ((int)data_0209f2f8 != last_level || (int)data_02092120 != last_area) {
        last_level = data_0209f2f8;
        last_area = data_02092120;
        int n = 0;
        for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a)) {
            if (is_gate(a)) {
                std::fprintf(stderr,
                             "[gwdoors] level %d area %d gate %d @%p pos=%d,%d,%d "
                             "yaw=%d param=%u slide=%d\n",
                             (int)data_0209f2f8, (int)data_02092120, n++, a,
                             rd(a, 0x5c) >> 12, rd(a, 0x60) >> 12, rd(a, 0x64) >> 12,
                             (int)*(const short *)((const char *)a + kGateAngle),
                             (unsigned)rd(a, 8), rd(a, kGateSlide) >> 12);
                continue;
            }
            if (!is_door(a)) continue;
            std::fprintf(stderr,
                         "[gwdoors] level %d area %d door %d @%p pos=%d,%d,%d "
                         "yaw=%d variant=%u areas=%d,%d cursor=%d frames=%d "
                         "node=%08x\n",
                         (int)data_0209f2f8, (int)data_02092120, n++, a,
                         rd(a, 0x5c) >> 12, rd(a, 0x60) >> 12, rd(a, 0x64) >> 12,
                         (int)*(const short *)((const char *)a + 0x8e),
                         (unsigned)rd(a, 8), (int)*(const signed char *)((const char *)a + 0x8c),
                         (int)*(const signed char *)((const char *)a + 0x90),
                         rd(a, kCursor) >> 12, door_frames(a),
                         (unsigned)rd(a, kNode));
        }
        std::fprintf(stderr, "[gwdoors] level %d area %d: %d doors\n",
                     (int)data_0209f2f8, (int)data_02092120, n);
    }
    const void *d = nearest_any(rd(body, 0x5c), rd(body, 0x60), rd(body, 0x64));
    const bool gate = d && is_gate(d);
    const unsigned node = d && !gate ? (unsigned)rd(d, kNode) : 0u;
    const int cur = !d ? -1 : gate ? (int)gate_frame(d) : rd(d, kCursor) >> 12;
    if (kind || node != last_node || cur != last_cur) {
        last_node = node;
        last_cur = cur;
        std::fprintf(stderr,
                     "[gwdoor] t=%u f=%u kind=%u noctrl=%u step=%u big=%u pos=%d,%d,%d "
                     "yaw=%d anim=%u door=%p cursor=%d/%d node=%08x\n",
                     comms_hub_now_ms(), frame, (unsigned)kind, (unsigned)rb(body, 0x70a),
                     (unsigned)rb(body, 0x6e3), (unsigned)rb(body, 0x70b),
                     rd(body, 0x5c), rd(body, 0x60), rd(body, 0x64),
                     (int)*(const short *)((const char *)body + 0x8e),
                     (unsigned)(rd(body, 0x63c) >> 2), d, cur,
                     d && !gate ? door_frames(d) : -1, node);
    }
}

// ---------------------------------------------------------------------------
// The watcher.
namespace {
/* A door is often two leaves (the front door, the star doors, the basement
   doors): each leaf is its own Door actor, 150 units apart. DOORKEY1: the ROM
   opens ONE: the leaf whose own Behavior found the player in its box
   (func_ov100_02144cf8 -> func_ov100_021451c4), and nothing links a leaf to
   its neighbour. Driving the neighbour with the same frame swung it the wrong
   way on its own (it faces the other way, Tango's bug 12). Only the leaf the
   sender read its frame from is driven, and it is picked again whenever his
   door kind changes: a key door's talk stands him at the middle of the pair
   (75 units from either leaf), its swing then puts him 100 units in front of
   the leaf he goes through. SM64DS_DOORKEY_OFF=leaf drives both, as 1.0.3. */
enum { kLeaves = 2, kPairR = 200 << 12 };
struct Drive {
    void *door[kLeaves];   // the doors this seat is driving (door[0] null: none)
    bool  gate;            // door[0] is a star gate
    unsigned stamp;        // the pass that last drove it (watcher_end_pass)
    unsigned char kind;    // DOORKEY1: the door kind it was picked for
    bool moved;            // DOORKEY1: a frame above 0 was applied since the pick
};
Drive g_drive[kMaxSeats];
unsigned g_pass = 1;

/* DOORS9C: WHAT THE DOOR LOOKED LIKE BEFORE ANY FRIEND TOUCHED IT, kept per
   DOOR and shared by every seat. It used to be kept per seat, so a second
   friend who took the same door while the first friend's last frame was still
   in it recorded that frame (half open) as "the door's own", and gave it back
   at the end: the leaves were left open with nobody near, forever. `last` is
   the value anyone last wrote, so a write by another friend is not mistaken
   for the local game moving the door. */
struct Held {
    void *door;            // null: free
    bool  gate;
    bool  yielded;         // the local game moved it: not ours to write or restore
    int   users;           // drives holding it
    int   base;            // plain door: the cursor before the first write (Fix12)
    int   last;            // ...and as last written
    int   gbase[3];        // star gate: the offset 0xa4/0xa8/0xac before the first write
    int   glast[3];
};
enum { kHeld = 24 };
Held g_held[kHeld];

Held *held_find(const void *d) {
    for (int i = 0; i < kHeld; ++i)
        if (g_held[i].door == d) return &g_held[i];
    return 0;
}
void held_free(Held *h) { std::memset(h, 0, sizeof *h); }

// One more drive holds `d`; the first one records the door's own state.
Held *held_take(void *d, bool gate) {
    Held *h = held_find(d);
    if (h) { ++h->users; return h; }
    for (int i = 0; i < kHeld; ++i)    // free entries of doors a level change took
        if (g_held[i].door && !door_alive(g_held[i].door)) held_free(&g_held[i]);
    for (int i = 0; i < kHeld; ++i) {
        if (g_held[i].door) continue;
        h = &g_held[i];
        h->door = d;
        h->gate = gate;
        h->users = 1;
        if (gate)
            for (int k = 0; k < 3; ++k) h->gbase[k] = h->glast[k] = rd(d, kGateOffs + 4 * k);
        else
            h->base = h->last = rd(d, kCursor);
        return h;
    }
    return 0;
}

// A drive lets go of `d`; the last one puts the door back as it was, unless the local game moved it.
void held_drop(void *d) {
    Held *h = held_find(d);
    if (!h) return;
    if (--h->users > 0) return;
    if (door_alive(d) && !h->yielded) {
        if (h->gate) {
            if (rd(d, kGateOffs) == h->glast[0] && rd(d, kGateOffs + 4) == h->glast[1] &&
                rd(d, kGateOffs + 8) == h->glast[2])
                for (int k = 0; k < 3; ++k) *(int *)((char *)d + kGateOffs + 4 * k) = h->gbase[k];
        } else if (rd(d, kCursor) == h->last) {
            *(int *)((char *)d + kCursor) = h->base;
        }
        ++g_stats.restored;
    }
    held_free(h);
}

// The Door actor nearest to `d` (other than d) within kPairR, or null.
void *pair_leaf(const void *d) {
    void *best = 0;
    long long bd = 0;
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a)) {
        if (a == d || !is_door(a)) continue;
        const long long dx = (long long)(rd(a, 0x5c) - rd(d, 0x5c)),
                        dz = (long long)(rd(a, 0x64) - rd(d, 0x64));
        const int dy = rd(a, 0x60) - rd(d, 0x60);
        if (dx > kPairR || dx < -kPairR || dz > kPairR || dz < -kPairR ||
            dy > (100 << 12) || dy < -(100 << 12))
            continue;
        const long long q = (dx >> 4) * (dx >> 4) + (dz >> 4) * (dz >> 4);
        if (!best || q < bd) { best = a; bd = q; }
    }
    return best;
}

void give_back(Drive &d) {
    for (int k = 0; k < kLeaves; ++k) {
        if (d.door[k]) held_drop(d.door[k]);
        d.door[k] = 0;
    }
    d.gate = false;
}
/* [gwdoors] pick: every door a friend opens. The friend's position, the door
   the watcher picked (actor id at +0x0c, param / variant at +8, position) and
   every other door-like actor in range, so a key-door variant that the pick
   missed (or a neighbouring plain door that it took instead) shows in one row. */
void probe_pick(int seat, unsigned char kind, unsigned char frame,
                const int pos[3], const void *picked) {
    std::fprintf(stderr,
                 "[gwdoors] pick t=%u seat=%d kind=%u frame=%u friend=%d,%d,%d "
                 "picked=%p class=%u variant=%u pos=%d,%d,%d\n",
                 comms_hub_now_ms(), seat, (unsigned)kind, (unsigned)frame,
                 pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, picked,
                 (unsigned)*(const unsigned short *)((const char *)picked + 0x0c),
                 (unsigned)rd(picked, 8),
                 rd(picked, 0x5c) >> 12, rd(picked, 0x60) >> 12, rd(picked, 0x64) >> 12);
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a)) {
        if (!is_door(a) && !is_gate(a)) continue;
        const int dx = rd(a, 0x5c) - pos[0], dz = rd(a, 0x64) - pos[2];
        if (dx > 2 * kSearchR || dx < -2 * kSearchR || dz > 2 * kSearchR || dz < -2 * kSearchR)
            continue;
        std::fprintf(stderr,
                     "[gwdoors] pick   %s %p class=%u variant=%u pos=%d,%d,%d "
                     "d=%d,%d\n", a == picked ? "PICKED" : "other ", a,
                     (unsigned)*(const unsigned short *)((const char *)a + 0x0c),
                     (unsigned)rd(a, 8), rd(a, 0x5c) >> 12, rd(a, 0x60) >> 12,
                     rd(a, 0x64) >> 12, dx >> 12, dz >> 12);
    }
}
}  // namespace

void watcher_release(int seat) {
    if (seat < 0 || seat >= kMaxSeats) return;
    Drive &d = g_drive[seat];
    if (!d.door[0]) return;
    give_back(d);
    ++g_stats.released;
}

// Once per follow pass, after every seat: a seat that was not driven this pass or the one before (it left, its
// frames stopped, the pass skipped it by a path that has no release) gives its door back. Nothing used to.
void watcher_end_pass() {
    for (int s = 0; s < kMaxSeats; ++s) {
        Drive &d = g_drive[s];
        if (d.door[0] && g_pass - d.stamp >= 2) {
            give_back(d);
            ++g_stats.released;
            ++g_stats.swept;
        }
    }
    ++g_pass;
}

void watcher_apply(int seat, unsigned char kind, unsigned char frame,
                   const int pos[3]) {
    if (seat < 0 || seat >= kMaxSeats) return;
    Drive &d = g_drive[seat];
    if (kind == kDoorNone || kind >= kDoorKinds || frame == kDoorFrameNone) {
        if (kind != kDoorNone) ++g_stats.no_door;
        watcher_release(seat);
        return;
    }
    if (d.door[0] && !door_alive(d.door[0])) {      // a level change took it
        give_back(d);
    }
    if (d.door[0] && d.kind != kind && !coop_doorkey_off("leaf")) {   // DOORKEY1: a new step of his door sequence, pick again
        give_back(d);
    }
    /* DOORKEY1: until the door has been moved once in this step (a frame above 0 applied) the pick follows him: a sender
       with no step flag (1.0.3) slides from the middle of a double door to the leaf he goes through, and the first frames of
       the swing can find him still halfway, as near one leaf as the other. Once it moved it is kept: a 1.0.3 sender's own
       pick flips to the other leaf for a few ticks while he walks through on a slant (it reads that leaf's frame 0). The key
       talk never moves a leaf (frame 0 throughout) and he is placed at the middle of the double door, so there the first pick
       is kept for the whole talk and the swing that follows picks again. */
    if (d.door[0] && !d.moved && !d.gate && kind != kDoorTalk && !coop_doorkey_off("leaf")) {
        void *nd = nearest_any(pos[0], pos[1], pos[2]);
        if (nd && nd != d.door[0]) give_back(d);
    }
    d.stamp = g_pass;
    if (!d.door[0]) {
        void *nd = nearest_any(pos[0], pos[1], pos[2]);
        if (!nd) { ++g_stats.no_door; return; }
        d.gate = is_gate(nd);
        d.door[0] = nd;
        d.door[1] = d.gate || !coop_doorkey_off("leaf") ? 0 : pair_leaf(nd);
        d.kind = kind;
        d.moved = false;
        for (int k = 0; k < kLeaves; ++k)
            if (d.door[k] && !held_take(d.door[k], d.gate)) d.door[k] = 0;   // table full: not driven
        if (!d.door[0]) { d.door[0] = d.door[1]; d.door[1] = 0; }
        if (!d.door[0]) { d.gate = false; return; }
        if (std::getenv("SM64DS_GHOST_DOOR_PROBE")) probe_pick(seat, kind, frame, pos, d.door[0]);
        /* THE MUST-FAIL CONTROL for the proof rows, inert unset: what the door
           visual would be if it ran the door's REAL touch (the state logic the
           header says it must not): the watcher's OWN body is taken into the
           door sequence, snapped to the door and turned, and his area / level
           follows. A row that holds the watcher's own position, level and
           camera fixed fails on this arm. */
        static int unsafe = -1;
        if (unsafe < 0) unsafe = std::getenv("SM64DS_GHOST_DOOR_UNSAFE") ? 1 : 0;
        if (unsafe) {
            void *me = data_0209f394[data_0209f250];
            std::fprintf(stderr, "[gwdoorw] UNSAFE: door touch on the LOCAL body\n");
            if (me && !d.gate)
                _ZN8daDoor_c19func_ov100_021451c4EPvP6Player((char *)d.door[0], data_ov100_021488f4, (char *)me);
        }
    }
    /* has the LOCAL game moved one? (a write by another friend is not that: held.last is shared) */
    for (int k = 0; k < kLeaves; ++k) {
        if (!d.door[k]) continue;
        Held *h = held_find(d.door[k]);
        if (!h) continue;
        if (!h->yielded) {
            const bool moved = h->gate
                ? (rd(d.door[k], kGateOffs) != h->glast[0] || rd(d.door[k], kGateOffs + 4) != h->glast[1] ||
                   rd(d.door[k], kGateOffs + 8) != h->glast[2])
                : rd(d.door[k], kCursor) != h->last;
            if (moved) { h->yielded = true; ++g_stats.yielded; }
        }
        if (h->yielded) return;    // the local player is using this door himself. Not ours to write.
    }
    static int probe = -1;
    if (probe < 0) probe = std::getenv("SM64DS_GHOST_DOOR_PROBE") ? 1 : 0;
    if (d.gate) {
        /* A star door: no model cursor. Its two leaves are the same model drawn
           at +offset and -offset (daStarGate_c::Render); the offset is set by
           St_OpenClose_Main, which only runs for the local player. Write it. */
        int slide = frame >= kGateBase ? ((int)frame - kGateBase) << 13 : 0;
        if (slide > kGateMax) slide = kGateMax;
        Held *h = held_find(d.door[0]);
        if (!h) return;
        gate_offset(d.door[0], slide, h->glast);
        for (int i = 0; i < 3; ++i)
            *(int *)((char *)d.door[0] + kGateOffs + 4 * i) = h->glast[i];
        ++g_stats.applied;
        if (probe)
            std::fprintf(stderr, "[gwdoorw] t=%u seat=%d kind=%u frame=%u gate=%p "
                         "slide=%d\n", comms_hub_now_ms(), seat, (unsigned)kind,
                         (unsigned)frame, d.door[0], slide >> 12);
        return;
    }
    for (int k = 0; k < kLeaves; ++k) {
        if (!d.door[k]) continue;
        Held *h = held_find(d.door[k]);
        if (!h) continue;
        int f = (int)frame;
        const int n = door_frames(d.door[k]);
        if (n > 0 && f >= n) f = n - 1;
        if (f > 0) d.moved = true;
        if (f == 0 && d.moved && !coop_doorkey_off("leaf")) continue;   // DOORKEY1: a 1.0.3 sender's stray 0 (see the pick)
        h->last = f << 12;
        *(int *)((char *)d.door[k] + kCursor) = h->last;
        ++g_stats.applied;
        if (probe)
            std::fprintf(stderr, "[gwdoorw] t=%u seat=%d kind=%u frame=%u door=%p "
                         "cursor=%d/%d leaf=%d\n", comms_hub_now_ms(), seat,
                         (unsigned)kind, (unsigned)frame, d.door[k], f, n, k);
    }
}

// ---- PAINTTAG1: a ghost that went through a painting ------------------------
// ON THE CARTRIDGE the player who jumps into a painting is taken by the EXIT
// actor (349, daChScene_c): its Behavior (src/game/actors/d_a_ch_scene.cpp)
// fires on the frame his position, in the exit's own local frame (the inverse
// matrix at +0xd4), is inside the box (|x| <= +0x80, 0 <= y <= +0x84) and his
// local z has changed sign since the frame before (+0x88 keeps the last z).
// A wall painting then pins him up to 768 units behind its plane while he
// falls, a floor hole lets him drop; either way the wall or the floor hides
// him and the camera stays put until the load. Online the watcher still drew
// the name tag over him, on top of everything, sliding down behind the wall.
// The watcher runs that same test on the ghost's drawn position (the sender's
// own body went through it 100 ms earlier) and keeps the answer until the
// ghost is back on the side it came from (a refused course puts him back in
// front of the painting) or the seat is released. The ROM's own gate comes
// along: with an entrance pending (data_02092110 >= 0) no exit fires, so none
// starts a hide either.
namespace {
enum { kExitActorId = 349, kPaintExits = 32, kExInv = 0xd4, kExHalfW = 0x80, kExHigh = 0x84,
       /* THE SLACK. The ghost is drawn 100 ms behind its sender, between two of
          his frames, so the sample that first shows him past the plane is not
          the position the sender's own exit tested: measured on the castle's
          Bob-omb painting (exit 0 of level 2), the jumper crossed at local
          y 0..4 and one watcher's first sample past the plane read y -8, under
          the box, and the hide never started. 64 units (about a stride) either
          way on the box's sides and floor takes the in-between sample; the
          plane itself and the sign test are the ROM's. */
       kExSlack = 64 << 12 };
struct PaintSeat {
    int lvl, area;
    const void *ex[kPaintExits];
    int lastz[kPaintExits];
    bool have[kPaintExits];
    int in;          // the exit index he went through, -1 none
};
PaintSeat g_paint[kMaxSeats];
bool g_paint_init = false;

void paint_clear(PaintSeat &p) {
    std::memset(&p, 0, sizeof p);
    p.in = -1;
    p.lvl = (int)data_0209f2f8;
    p.area = (int)data_02092120;
}

bool paint_off_knob() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_GHOST_PAINTTAG_OFF");
        v = (e && std::atoi(e) != 0) ? 1 : 0;
    }
    return v != 0;
}
bool paint_probe() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_GHOST_PAINT_PROBE") ? 1 : 0;
    return v != 0;
}
}  // namespace

bool paint_watch(int seat, const int pos[3], bool teleport) {
    if (seat < 0 || seat >= kMaxSeats || !pos || paint_off_knob()) return false;
    if (!g_paint_init) {
        for (int s = 0; s < kMaxSeats; ++s) paint_clear(g_paint[s]);
        g_paint_init = true;
    }
    PaintSeat &p = g_paint[seat];
    if (p.lvl != (int)data_0209f2f8 || p.area != (int)data_02092120) paint_clear(p);
    const bool armed = data_02092110 < 0;
    int n = 0;
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a && n < kPaintExits;
         a = _ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)((const char *)a + 0x0c) != kExitActorId) continue;
        if (p.ex[n] != a) {              // a new exit in this slot of the list
            p.ex[n] = a;
            p.have[n] = false;
            if (p.in == n) p.in = -1;
        }
        int local[3];
        MulVec3Mat4x3(pos, (const char *)a + kExInv, local);
        if (paint_probe() && (local[2] >> 12) < 200 && (local[2] >> 12) > -200 &&
            (local[0] >> 12) < 600 && (local[0] >> 12) > -600)
            std::fprintf(stderr, "[gwpaint] t=%u seat=%d near exit %d local=%d,%d,%d "
                         "last z %d have %d armed %d box %d x %d\n", comms_hub_now_ms(),
                         seat, n, local[0] >> 12, local[1] >> 12, local[2] >> 12,
                         p.lastz[n] >> 12, p.have[n] ? 1 : 0, armed ? 1 : 0,
                         rd(a, kExHalfW) >> 12, rd(a, kExHigh) >> 12);
        if (p.in == n) {
            if (local[2] >= 0) {                 // back on the side he came from
                p.in = -1;
                ++g_stats.paint_out;
                if (paint_probe())
                    std::fprintf(stderr, "[gwpaint] t=%u seat=%d back out of exit %d "
                                 "local=%d,%d,%d\n", comms_hub_now_ms(), seat, n,
                                 local[0] >> 12, local[1] >> 12, local[2] >> 12);
            }
        } else if (p.in < 0 && armed && p.have[n] && !teleport) {
            int ax = local[0] < 0 ? -local[0] : local[0];
            if (ax <= rd(a, kExHalfW) + kExSlack && local[1] >= -kExSlack &&
                local[1] <= rd(a, kExHigh) + kExSlack &&
                (local[2] ^ p.lastz[n]) < 0 && local[2] < 0) {
                /* INTO the exit only: whichever side the ROM's trigger saw him
                   come from, its pull then drags him to local z -0x300000
                   (mHorzSpeed), so "through" is the negative side. A sample
                   that steps back out to the positive side (the level entry
                   that puts a ghost in front of a painting) is no entry. */
                p.in = n;
                ++g_stats.paint_in;
                if (paint_probe())
                    std::fprintf(stderr, "[gwpaint] t=%u seat=%d through exit %d @%p "
                                 "param=%08x local=%d,%d,%d last z %d box %d x %d\n",
                                 comms_hub_now_ms(), seat, n, a, (unsigned)rd(a, 8),
                                 local[0] >> 12, local[1] >> 12, local[2] >> 12,
                                 p.lastz[n] >> 12, rd(a, kExHalfW) >> 12,
                                 rd(a, kExHigh) >> 12);
            }
        }
        p.lastz[n] = local[2];
        p.have[n] = true;
        ++n;
    }
    if (p.in >= n) p.in = -1;            // that exit is gone from the list
    if (p.in >= 0) ++g_stats.paint_frames;
    return p.in >= 0;
}

void paint_release(int seat) {
    if (seat < 0 || seat >= kMaxSeats || !g_paint_init) return;
    paint_clear(g_paint[seat]);
}

}  // namespace ghostdoor
}  // namespace port
