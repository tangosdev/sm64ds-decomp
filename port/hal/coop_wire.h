// COOP WORLD WIRE. Run mp1 wave 56, lane COOPWORLD1.
//
// The 'SYNW' (coop world) and 'SYNB' (coop control) sub-types the world core
// uses, their payload layouts, the validators and the sub-type dispatcher the
// other coop lanes plug into. The 12-byte head (tag + the 8-byte common
// header) is WIRE1's, in hub_presence.h (HubCoopHead, hub_coop_validate,
// hub_coop_encode); this file never redefines it. Everything is little-endian
// and packed. A payload is judged only by exact or bounded length plus ranges:
// nothing here trusts a field it did not range-check.
//
// PORT_HOST_ABI. Nothing in src/ knows this exists.
//
// SUB-TYPE OWNERSHIP (W56):
//   SYNB 0x00-0x1f  COOPWORLD1   (this file)
//   SYNB 0x20-0x2f  COOPSAVE1    (coop_save_wire.h, included below when it exists)
//   SYNB 0x30-0x3f  COOPBOOT1    (coop_boot_wire.h, included below when it exists)
//   SYNW 0x00-0x7f  COOPWORLD1
// Only ONE consumer can sit on a hub kind (hub_on_kind), so coop_world.cpp owns
// both handlers and fans a datagram out by sub-type to whoever called
// coop_ctl_register / coop_world_register for that range. A sub-type nobody
// registered is dropped and counted, never guessed at.
//
// GROUP KEY. A group is (sublevel id data_0209f2f8, act data_0209f220[0]): every
// area of a level loads together, and the act picks the placed-object filter, so
// that pair fixes the death-table slot meaning. Four bytes on the wire.

#ifndef PORT_HAL_COOP_WIRE_H
#define PORT_HAL_COOP_WIRE_H

#include "hub_presence.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#if defined(__has_include)
#if __has_include("coop_save_wire.h")
#include "coop_save_wire.h"
#endif
#if __has_include("coop_boot_wire.h")
#include "coop_boot_wire.h"
#endif
#endif

namespace port {

// ---- SYNB (control) sub-types, 0x00-0x1f ----------------------------------
enum : uint8_t {
    kCtlGroupHello  = 0x01,   // 1 Hz: "I am in this group" + established + owner belief
    kCtlLevelEnter  = 0x02,   // "I am entering this group" (asks for the snapshot)
    kCtlSnapPart    = 0x03,   // owner -> joiner, n of m, the death bits
    kCtlSnapAck     = 0x04,   // joiner -> owner, "got all of it"
    kCtlOwnerChange = 0x05,   // "I am now the owner of this group"
    kCtlWorldLast   = 0x1f,
};

// ---- SYNW (world) sub-types, 0x00-0x7f ------------------------------------
enum : uint8_t {
    kWorldDeathDelta = 0x01,  // slot ids newly dead
    kWorldDeathFull  = 0x02,  // the whole 512-bit part, idempotent
    // 0x03 is coop_world.cpp's swallow message (kWorldSwallowGone)
    kWorldCoinCount  = 0x04,  // COOPCOUNT1: my level coin count + "100-coin star is out" (an old build drops it as unknown)
    kWorldSecret     = 0x05,  // COOPCOUNT1: I took this secret coin (class 329); an old build drops it as unknown
    kWorldRacePose   = 0x06,  // KTQ1: the racer's Koopa the Quick pose (hal/coop_race.cpp); an old build drops it as unknown
    kWorldCoinLoose  = 0x07,  // COINS10: my running total of coins with no slot this visit; an old build drops it as unknown
    kWorldRedCount   = 0x08,  // AREACOIN1: my red coin count, for friends in another part of the same course; an old build drops it as unknown
    kWorldLast       = 0x7f,
};

enum : int {
    kCoopSeats        = 4,     // rooms hold seats 0..3 today; masks are 16 bits wide
    kCoopMaxSeats     = 16,
    kCoopSlots        = 512,   // death-table slots per level part
    kCoopWords        = 16,    // 32-bit words per part
    kCoopMaxDelta     = 64,    // slots in one DEATH_DELTA
    kCoopSnapChunkMax = 64,
    kCoopNoSeat       = 0xFF,
};

#pragma pack(push, 1)
struct CoopKeyWire {
    uint16_t level;      // sublevel id
    uint8_t  act;        // mission / star number
    uint8_t  reserved;   // must be 0
};

// GROUP_HELLO payload, 8 B.
struct CoopHelloWire {
    CoopKeyWire key;
    uint8_t established;     // 0/1: I hold the group's state
    uint8_t owner;           // the seat I believe owns the group, 0xFF none
    uint8_t rank;            // how many established members were here when I became one (1 = first in)
    uint8_t reserved;        // 0
};

// LEVEL_ENTER payload, 8 B.
struct CoopEnterWire {
    CoopKeyWire key;
    uint32_t nonce;          // the joiner's enter counter; the snapshot and the ack echo it
};

// SNAPSHOT_PART payload, 20 B + chunk. The owner sends the current level part
// (64 bytes) in `total` chunks of 64/total bytes; resent until the joiner's ack.
struct CoopSnapWire {
    uint8_t  target;         // the joiner's seat (others ignore it)
    uint8_t  n;              // chunk index 0..total-1
    uint8_t  total;          // 1, 2 or 4
    uint8_t  reserved;       // 0
    CoopKeyWire key;
    uint32_t nonce;          // echo of the joiner's LEVEL_ENTER nonce
    uint32_t level_clock;    // phase 2; 0 until then
    uint32_t rng_seed;       // phase 2; 0 until then
    // uint8_t chunk[64 / total];
};

// SNAPSHOT_ACK payload, 12 B.
struct CoopAckWire {
    CoopKeyWire key;
    uint32_t nonce;
    uint8_t  owner;          // the seat whose snapshot I took
    uint8_t  reserved[3];
};

// OWNER_CHANGE payload, 8 B.
struct CoopOwnerWire {
    CoopKeyWire key;
    uint8_t  owner;          // the new owner; must equal the sender seat
    uint8_t  reserved[3];
};

// DEATH_DELTA payload, 8 B + 2 per slot.
struct CoopDeltaWire {
    CoopKeyWire key;
    uint8_t  count;          // 1..kCoopMaxDelta
    uint8_t  reserved[3];
    // uint16_t slot[count];  each 0..kCoopSlots-1
};

// DEATH_FULL payload, 4 + 64 B.
struct CoopFullWire {
    CoopKeyWire key;
    uint32_t words[kCoopWords];
};

// COIN_COUNT payload, 8 B (COOPCOUNT1). Every member of a group sends it at 1 Hz and when its count changes, the owner
// also right after a joiner's snapshot; a receiver takes the larger of its own count and the group's.
struct CoopCoinWire {
    CoopKeyWire key;
    uint16_t count;          // the sender's level coin counter, 0..255 (the ROM clamps it at 0xff)
    uint8_t  flags;          // bit 0: the sender's 100-coin star is out (event bit 0x1f)
    uint8_t  reserved;       // 0
};

// SECRET payload, 8 B (COOPCOUNT1). A secret coin (daSCoin_c, class 329) the sender collected: its placed slot and
// the group leader's collected count right after it on the sender (1..5, for the log; the receiver counts its own).
struct CoopSecretWire {
    CoopKeyWire key;
    uint16_t slot;           // 0..511
    uint8_t  count;          // 1..5
    uint8_t  reserved;       // 0
};
// RED_COUNT payload, 8 B (AREACOIN1). The sender's red coin count (NumRedCoins, 0..8) and the star the course's red coins
// carry (0 = not seen). Read only by friends in ANOTHER sublevel of the same course and act: the cartridge keeps one red coin count
// for the whole course, while the coop group (and its live red coin path) is one sublevel.
struct CoopRedWire {
    CoopKeyWire key;
    uint8_t  sum;            // 0..8
    uint8_t  star;           // 0..7
    uint16_t reserved;       // 0
};
#pragma pack(pop)

static_assert(sizeof(CoopKeyWire) == 4, "key is 4 bytes");
static_assert(sizeof(CoopHelloWire) == 8, "hello is 8 bytes");
static_assert(sizeof(CoopEnterWire) == 8, "enter is 8 bytes");
static_assert(sizeof(CoopSnapWire) == 20, "snap head is 20 bytes");
static_assert(sizeof(CoopAckWire) == 12, "ack is 12 bytes");
static_assert(sizeof(CoopOwnerWire) == 8, "owner change is 8 bytes");
static_assert(sizeof(CoopDeltaWire) == 8, "delta head is 8 bytes");
static_assert(sizeof(CoopFullWire) == 68, "full is 68 bytes");
// LOOSE payload, 12 B (COINS10). The coin value the sender's own player took this level visit from coins with no slot
// (enemy drops, runtime coins, the bonus coin of a Bob-omb or a mushroom). A running total: a receiver credits only the
// part above the last total it saw from that seat in that visit, so a lost or repeated message never counts twice.
struct CoopLooseWire {
    CoopKeyWire key;
    uint32_t visit;          // the sender's visit id: new each time it joins a level group, never 0
    uint16_t total;          // 0..0xffff
    uint8_t  flags;          // 0
    uint8_t  reserved;       // 0
};

static_assert(sizeof(CoopCoinWire) == 8, "coin count is 8 bytes");
static_assert(sizeof(CoopLooseWire) == 12, "loose coins is 12 bytes");
static_assert(sizeof(CoopSecretWire) == 8, "secret is 8 bytes");

// ---------------------------------------------------------------------------
// Validators. Each takes the PAYLOAD (after the 12-byte head) and its length,
// range-checks everything, and returns false for anything not exactly right.
// ---------------------------------------------------------------------------
// How long ago a hub-ms stamp was taken. Stamps are kept odd (stamp | 1) so 0 can mean "never", which puts a stamp taken on an
// even millisecond 1 ms ahead of a now read in that same millisecond; such a stamp (or any a few ms ahead) is age 0, not the
// 4294967295 an unsigned difference gives. Callers check the 0 = never case themselves.
inline unsigned coop_age_ms(unsigned now, unsigned stamp) {
    const int d = (int)(now - stamp);
    return d < 0 ? 0u : (unsigned)d;
}
inline bool coop_key_ok(const CoopKeyWire &k) {
    return k.reserved == 0 && k.level < 256 && k.act <= 8;
}
inline bool coop_hello_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopHelloWire)) return false;
    CoopHelloWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.established <= 1 && w.reserved == 0 &&
           (w.owner < kCoopMaxSeats || w.owner == kCoopNoSeat) && w.rank <= 16;
}
inline bool coop_enter_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopEnterWire)) return false;
    CoopEnterWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key);
}
inline bool coop_snap_validate(const void *p, int len) {
    if (!p || len < (int)sizeof(CoopSnapWire) + 1 ||
        len > (int)sizeof(CoopSnapWire) + kCoopSnapChunkMax) return false;
    CoopSnapWire w; std::memcpy(&w, p, sizeof w);
    if (w.target >= kCoopMaxSeats || w.reserved != 0 || !coop_key_ok(w.key))
        return false;
    if (w.total != 1 && w.total != 2 && w.total != 4) return false;
    if (w.n >= w.total) return false;
    return len - (int)sizeof(CoopSnapWire) == kCoopSnapChunkMax / w.total;
}
inline bool coop_ack_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopAckWire)) return false;
    CoopAckWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.owner < kCoopMaxSeats &&
           w.reserved[0] == 0 && w.reserved[1] == 0 && w.reserved[2] == 0;
}
inline bool coop_owner_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopOwnerWire)) return false;
    CoopOwnerWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.owner < kCoopMaxSeats &&
           w.reserved[0] == 0 && w.reserved[1] == 0 && w.reserved[2] == 0;
}
// DEATH_DELTA: length is exactly head + 2 * count, count 1..64, every slot in
// range. A slot out of range fails the whole datagram (nothing partial is applied).
inline bool coop_delta_validate(const void *p, int len) {
    if (!p || len < (int)sizeof(CoopDeltaWire) + 2) return false;
    CoopDeltaWire w; std::memcpy(&w, p, sizeof w);
    if (!coop_key_ok(w.key) || w.count < 1 || w.count > kCoopMaxDelta) return false;
    if (w.reserved[0] || w.reserved[1] || w.reserved[2]) return false;
    if (len != (int)sizeof(CoopDeltaWire) + 2 * (int)w.count) return false;
    const unsigned char *s = static_cast<const unsigned char *>(p) + sizeof w;
    for (int i = 0; i < (int)w.count; ++i) {
        uint16_t v; std::memcpy(&v, s + 2 * i, 2);
        if (v >= kCoopSlots) return false;
    }
    return true;
}
inline bool coop_full_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopFullWire)) return false;
    CoopFullWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key);
}
inline bool coop_coin_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopCoinWire)) return false;
    CoopCoinWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.count <= 0xff && w.flags <= 1 && w.reserved == 0;
}
inline bool coop_loose_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopLooseWire)) return false;
    CoopLooseWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.visit != 0 && w.flags == 0 && w.reserved == 0;
}
inline bool coop_red_validate(const void *p, int len) {   // AREACOIN1
    if (!p || len != 8) return false;
    CoopRedWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.sum <= 8 && w.star <= 7 && w.reserved == 0;
}
inline bool coop_secret_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopSecretWire)) return false;
    CoopSecretWire w; std::memcpy(&w, p, sizeof w);
    return coop_key_ok(w.key) && w.slot < kCoopSlots && w.count >= 1 && w.count <= 5 && w.reserved == 0;
}

// ---------------------------------------------------------------------------
// The sub-type dispatcher (coop_world.cpp). A handler runs on the game thread
// (inside comms_hub_pump), gets the sender seat (stamped), the sub-type, and the
// payload after the 12-byte head, already head-validated. Registering a range
// that overlaps another lane's, or one outside the owner's range, fails.
// ---------------------------------------------------------------------------
typedef void (*CoopSubFn)(int seat, int subtype, const unsigned char *payload,
                          int payload_len, void *user);
bool coop_ctl_register(int lo, int hi, CoopSubFn fn, void *user = nullptr);
bool coop_world_register(int lo, int hi, CoopSubFn fn, void *user = nullptr);
// Send a SYNB / SYNW datagram: head + payload (the head's seat is stamped by the
// hub). False when the hub is not live in a coop room or the payload is bad.
bool coop_send_ctl(int subtype, const void *payload, int payload_len);
bool coop_send_world(int subtype, const void *payload, int payload_len);
// True when this console is in a coop room with a live hub and the world core
// is on (SM64DS_COOP set).
bool coop_world_active();

}  // namespace port

#endif  // PORT_HAL_COOP_WIRE_H
