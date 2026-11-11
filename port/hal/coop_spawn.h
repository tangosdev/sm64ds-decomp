// COOP RUNTIME SPAWNS. Run mp1 coop phase 2, lane COOPSPAWN1.
//
// What it keeps the same on every console standing in the same (level, act):
//   * ? / ! / cap block hits (classes 20..25, the QuestionBlock family). The
//     console whose player hit a block tells the others; each of them runs
//     the block's own hit entry on its copy, with ITS OWN player's character
//     as the hitter, so every player gets their own cap / feather / coins
//     out of a block anyone hit (the one thing the room keeps per player).
//   * red coins. The ROM already sums one red-coin counter per body
//     (NumRedCoins) and spawns the red-coin star at the 8th. A red coin that a
//     friend collected is removed quietly by the death-bit sync and never
//     counted; this module counts it on a friend's counter slot and lets the
//     ROM's own marker code spawn the star, on every console, at the marker.
//   * ANNOUNCED runtime actors: stars (PowerStar 178, Silver Star 179) and the
//     brick-block prizes (1-up 276, mega mushroom 277) a console spawns at run
//     time, with no placed slot, are announced to the group and spawned on the
//     others once, with a network id (spawner seat, counter). Whoever takes or
//     loses one tells the others, who delete their copy quietly ("gone").
//     A late joiner gets the owner's live ones. Enemy coin drops, bubbles,
//     particles and every other class are NEVER announced (per console).
//   * silver stars (SILVERCOOP1). The ROM counts them per player
//     (data_0209f310[player], GiveVsStars) and grows the act's gold star when
//     the sum (NumVsStarsObtained) is 5. A friend's take is seen here the way
//     a red coin is (the placed star's death bit, an announced star's GONE)
//     and credited to MY count, the one the HUD draws, so every screen shows
//     the room's count and grows the gold star at the room's fifth. A hurt
//     drop is sent (SILVER DROP); a joiner gets the count from the owner.
//   * a star that every console spawns natively (the red-coin star, linked to
//     a placed star marker) is not announced; its collect is sent as a "gone
//     for marker slot S" so the other copies vanish.
//   * switch stars (STARCASE2). The star a star switch (class 12) grows in its
//     glass case is spawned natively on every console by a placed kind-6 marker
//     at level load, with no death slot and a runtime case: neither path above
//     sees it. Its take is sent as a "gone for switch star N" (kSpGoneSwitch);
//     every other console turns its copy see-through, waits for its camera
//     cutscene to end, ends its switch the ROM's way (the timer runs out), breaks
//     the case and removes the star. Only the console whose player pressed the
//     switch plays the star's camera: on a friend's console (the press came over
//     the wire) the star still grows and can be taken, but its cutscene request
//     is taken back every frame and the switch timer is held for as long as the
//     presser's camera would have held it.
//
// OTHER LANES CALL (documented API, below): coop_spawn_announce(). The boss
// lane spawns its defeat star through it.
//
// INERT unless SM64DS_COOP is set in a coop room (the callbacks are only
// invoked by coop_world). Sub-types (plan 7.0): SYNB 0x12-0x17, SYNW 0x40-0x4f.
//
// ENVIRONMENT (proof knobs, all optional, all inert when unset):
//   SM64DS_COOP_LOG=1            [coopspawn] lines
//   SM64DS_SPAWN_OFF=a,b,..      must-fail controls, ignore received: block,
//                                red (do not count friends' red coins),
//                                spawn, gone, snap
//   SM64DS_SPAWN_BLOCK=F:cls[:nth][;..]  at group frame F hit the nth live
//                                block of the class the ROM's way
//   SM64DS_SPAWN_RED=F:n[;..]    at group frame F put the local player on the
//                                nth live red coin (the ROM's collect runs)
//   SM64DS_SPAWN_STAR=F:cls:param[;..]   at group frame F spawn a runtime
//                                actor above the local player (the ROM's spawn)
//   SM64DS_SPAWN_CENSUS=1        log every new runtime actor class (proof)
//   SM64DS_SPAWN_FORGE=F:name[,..]  one forged datagram: cls, kind, param,
//                                counter, pos, short, long, gone, goneslot,
//                                blockslot, blockcls, redsum, key
//   SM64DS_SWSTAR_OFF=a,b,..     must-fail controls for switch stars: cam (friends
//                                play the presser's camera), gone (no take sent or
//                                applied), look (no see-through when the bit lands)
//   SM64DS_SWSTAR_TAKE=F[:W][;..] armed at group frame F: once the live switch star
//                                is full size, wait W frames (default 0), then walk
//                                my player onto it every frame (up to 180 frames)
//                                until the ROM's own touch takes it

#ifndef PORT_HAL_COOP_SPAWN_H
#define PORT_HAL_COOP_SPAWN_H

#include "coop_wire.h"

#include <cstdint>
#include <cstring>

namespace port {

// ---- SYNB (control) sub-types, 0x12-0x17 -----------------------------------
enum : uint8_t {
    kSpCtlRed  = 0x12,   // owner -> joiner: the group's red-coin sum
    kSpCtlSilver = 0x13, // SILVERCOOP1: owner -> joiner: the group's silver-star count (a 1.0.1 peer drops it as unknown)
    kSpCtlLast = 0x17,
};

// ---- SYNW (world) sub-types, 0x40-0x4f -------------------------------------
enum : uint8_t {
    kSpWorldSpawn = 0x40,   // an announced runtime actor
    kSpWorldGone  = 0x41,   // an announced actor (or a linked star) is gone
    kSpWorldBlock = 0x42,   // a block was hit
    kSpWorldSilverDrop = 0x43,  // SILVERCOOP1: my player dropped silver stars (hurt); the group's count goes down (a 1.0.1 peer drops it as unknown)
    kSpWorldSilverCount = 0x44, // COOPSILVER3: owner -> group: THE silver-star count, on change and every second (a 1.0.2 peer drops it as unknown)
    kSpWorldLast  = 0x4f,
};

enum : int {
    kSpKindActor   = 1,     // one actor
    kSpKindSilver  = 2,     // silver star + its marker (the brick block's pair)
    kSpGoneAnn     = 1,     // gone: announced actor (origin seat, counter)
    kSpGoneLinked  = 2,     // gone: native star linked to placed marker slot
    kSpGoneSwitch  = 3,     // gone: switch star, by star number (STARCASE2; a 1.0.2 peer drops it as bad)
    kSpSlots       = 512,
    kSpPosLimit    = 1 << 28,
};

#pragma pack(push, 1)
// SPAWN, 32 B.
struct SpSpawnWire {
    CoopKeyWire key;
    uint8_t  origin;      // spawner seat
    uint8_t  kind;        // kSpKindActor / kSpKindSilver
    uint16_t counter;     // non-zero, per origin
    uint16_t cls;         // whitelisted class
    int8_t   area;        // -1 .. 7
    uint8_t  reserved;    // 0
    uint32_t param;       // the spawn parameter
    int32_t  pos[3];      // Fix12 world position
    uint32_t reserved2;   // 0
};
// GONE, 12 B.
struct SpGoneWire {
    CoopKeyWire key;
    uint8_t  origin;      // announced: spawner seat; linked: 0xFF
    uint8_t  kind;        // kSpGoneAnn / kSpGoneLinked / kSpGoneSwitch
    uint16_t id;          // announced: counter; linked: marker slot; switch: star number 0..7
    uint32_t reserved;    // 0
};
// BLOCK, 16 B.
struct SpBlockWire {
    CoopKeyWire key;
    uint16_t slot;        // placed-object slot, 0..511
    uint16_t cls;         // 20..25
    uint16_t seq;         // non-zero, per sender, wrap-safe
    uint16_t reserved;    // 0
    uint32_t reserved2;   // 0
};
// RED (control), 12 B.
struct SpRedWire {
    CoopKeyWire key;
    uint32_t nonce;       // the joiner's enter nonce, echoed
    uint8_t  target;      // the joiner's seat
    uint8_t  sum;         // 0..8, the group's red-coin sum
    uint8_t  star;        // 0..7, the red coins' star number (0 = unknown)
    uint8_t  reserved;    // 0
};
// SILVER (control), 12 B. SILVERCOOP1.
struct SpSilverWire {
    CoopKeyWire key;
    uint32_t nonce;       // the joiner's enter nonce, echoed
    uint8_t  target;      // the joiner's seat
    uint8_t  sum;         // 0..5, the group's silver-star count
    uint16_t reserved;    // 0
};
// SILVER DROP (world), 12 B. SILVERCOOP1.
struct SpSilverDropWire {
    CoopKeyWire key;
    uint16_t seq;         // non-zero, per sender, wrap-safe
    uint8_t  n;           // 1..5 stars dropped
    uint8_t  reserved;    // 0
    uint32_t reserved2;   // 0
};
// SILVER COUNT (world), 12 B. COOPSILVER3.
struct SpSilverCountWire {
    CoopKeyWire key;
    uint16_t seq;         // non-zero, per sender, wrap-safe
    uint8_t  sum;         // 0..5, the group's silver-star count on the owner's console
    uint8_t  reserved;    // 0
    uint32_t reserved2;   // 0
};
#pragma pack(pop)

static_assert(sizeof(SpSpawnWire) == 32, "spawn");
static_assert(sizeof(SpSilverCountWire) == 12, "silver count");
static_assert(sizeof(SpSilverWire) == 12, "silver");
static_assert(sizeof(SpSilverDropWire) == 12, "silver drop");
static_assert(sizeof(SpGoneWire) == 12, "gone");
static_assert(sizeof(SpBlockWire) == 16, "block");
static_assert(sizeof(SpRedWire) == 12, "red");

// ---- what a datagram may name ------------------------------------------------
inline bool sp_block_class_ok(int c) { return c >= 20 && c <= 25; }
inline bool sp_actor_class_ok(int c) { return c == 178 || c == 276 || c == 277 || c == 282 || c == 283 || c == 229; }
inline bool sp_silver_class_ok(int c) { return c == 179; }
inline bool sp_class_ok(int kind, int c) {
    return kind == kSpKindActor ? sp_actor_class_ok(c) : kind == kSpKindSilver ? sp_silver_class_ok(c) : false;
}
// Spawn parameters: a star carries its star number (and the ROM's small flag bits), the prizes carry none.
// A star's number is (param & 0xf), 0..7 (bossfix1: 8..15 are not real stars).
inline bool sp_param_ok(int cls, uint32_t p) {
    if (cls == 178 || cls == 179) return p < 0x100u && (p & 0xfu) <= 7u;
    return (cls == 282 || cls == 283 || cls == 229) ? p < 0x100u : p == 0u;
}
inline bool sp_pos_ok(int32_t v) { return v > -kSpPosLimit && v < kSpPosLimit; }

inline bool sp_spawn_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SpSpawnWire)) return false;
    SpSpawnWire w; std::memcpy(&w, p, sizeof w);
    if (!coop_key_ok(w.key) || w.origin >= kCoopMaxSeats || w.counter == 0) return false;
    if (w.reserved || w.reserved2) return false;
    if (w.kind != kSpKindActor && w.kind != kSpKindSilver) return false;
    if (!sp_class_ok(w.kind, w.cls) || !sp_param_ok(w.cls, w.param)) return false;
    if (w.area < -1 || w.area > 7) return false;
    for (int i = 0; i < 3; ++i) if (!sp_pos_ok(w.pos[i])) return false;
    return true;
}
inline bool sp_gone_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SpGoneWire)) return false;
    SpGoneWire w; std::memcpy(&w, p, sizeof w);
    if (!coop_key_ok(w.key) || w.reserved) return false;
    if (w.kind == kSpGoneAnn) return w.origin < kCoopMaxSeats && w.id != 0;
    if (w.kind == kSpGoneLinked) return w.origin == 0xFF && w.id < kSpSlots;
    if (w.kind == kSpGoneSwitch) return w.origin == 0xFF && w.id < 8;
    return false;
}
inline bool sp_block_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SpBlockWire)) return false;
    SpBlockWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.slot < kSpSlots && sp_block_class_ok(w.cls) && w.seq != 0 &&
           w.reserved == 0 && w.reserved2 == 0;
}
inline bool sp_red_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SpRedWire)) return false;
    SpRedWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.target < kCoopMaxSeats && w.sum <= 8 && w.star <= 7 && w.reserved == 0;
}
inline bool sp_silver_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SpSilverWire)) return false;
    SpSilverWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.target < kCoopMaxSeats && w.sum <= 5 && w.reserved == 0;
}
inline bool sp_silver_drop_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SpSilverDropWire)) return false;
    SpSilverDropWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.seq != 0 && w.n >= 1 && w.n <= 5 && w.reserved == 0 && w.reserved2 == 0;
}
inline bool sp_silver_count_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SpSilverCountWire)) return false;
    SpSilverCountWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.seq != 0 && w.sum <= 5 && w.reserved == 0 && w.reserved2 == 0;
}
// Wrap-safe "newer" for the per-sender block sequence (0 = nothing seen yet).
inline bool sp_seq_newer(uint16_t seq, uint16_t last) { return last == 0 || (int16_t)(uint16_t)(seq - last) > 0; }

struct CoopSpawnStats {
    unsigned long long sent_spawn, sent_gone, sent_block, sent_red;
    unsigned long long recv_spawn, recv_gone, recv_block, recv_red;
    unsigned long long block_local, block_applied, block_busy, block_notfound;
    unsigned long long spawn_local, spawn_applied, spawn_adopted, spawn_dup, spawn_failed, spawn_queued;
    unsigned long long gone_local, gone_applied, gone_unknown;
    unsigned long long red_remote, red_snap_applied, star_spawn_native;
    unsigned long long drop_bad, drop_group, drop_member, drop_class, drop_old, drop_off, drop_rate, drop_full;
};
CoopSpawnStats coop_spawn_stats();

// ---- API for other lanes ----------------------------------------------------
// Announce a runtime actor the caller has ALREADY spawned on this console (or
// pass the actor you want the others to get a copy of). The group gets one
// SPAWN; the actor is registered under a network id, so its take / loss is
// sent as GONE and a late joiner receives it. `cls` must be an announce-list
// class (178 PowerStar, 179 Silver Star, 276 1-up, 277 mega mushroom, 282 Key); a
// Silver Star announces with its marker (+0x434). Returns false when not in a
// coop group, the class is off the list, or the table is full. Safe to call
// from any frame hook; inert outside a coop room.
bool coop_spawn_announce(void *local_actor, int cls, unsigned param, const int pos[3], int area);
// Spawn on THIS console AND announce, in one call: the ROM's own spawn, then
// coop_spawn_announce. Returns the local actor (null = spawn failed, nothing sent).
void *coop_spawn_and_announce(int cls, unsigned param, const int pos[3], int area);
// STARCASE2: coop_switch calls this when it applied a FRIEND's press to a star switch (class 12) here, so the star it
// grows is not this console's to film: its camera cutscene is skipped here. A press by my own player never calls it.
void coop_spawn_note_remote_press(void *switch_actor);

}  // namespace port

#endif  // PORT_HAL_COOP_SPAWN_H
