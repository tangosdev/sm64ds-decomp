// COOP SWITCHES AND LEVEL GLOBALS. Run mp1 coop phase 2, lane COOPSW1.
//
// What it keeps equal across the consoles standing in the same (level, act):
//   * the level event word (data_0209f34c): every switch, brick, gate, spawner
//     and water cover in the ROM reads this one 32-bit word, so keeping the
//     word equal moves all of them. Per-bit last-writer-wins by a Lamport
//     stamp (stamp = counter << 4 | seat, so two stamps never tie).
//   * the press of a switch (StarSwitch 11 / 12, BlueCoinSwitch 10): the
//     console whose player pressed it tells the others, who run the switch's
//     own press entry on their copy (button sinks, music, target actor).
//   * kicked blocks and chests that a remote body cannot trigger (its
//     cylinder is masked): one table, one idempotent state write per class.
//   * the WDW water target (the diamond sets WDW_WATER's target height) as a
//     watched field.
//   * a late joiner gets the word, the level globals (data_0209f2c0, the TTC
//     clock hand setting) and the list of pressed / kicked actors from the
//     group owner, applied once the level's objects have spawned.
//
// Door answer (recorded in the lane status): doors poll the SAVE record every
// frame near the player (func_ov100_02144cf8 reads data_0209caa0), so an
// unlocked door follows the coop save sync. PUZZLES9 adds the START of an
// unlock: the ROM sets the save bit only when the unlock cutscene ENDS, so a
// friend standing at the same door could start a second unlock meanwhile. A
// plain door (353) entering its star-unlock / key-unlock node, and a star gate
// (354) entering ST_UNLOCKING, is announced as a kick; the others OR the
// door's own save bit into their record at once (a flags write: no Player is
// ever handed to a door state here). The open animation of a walker is
// ghost_door.cpp's.
//
// INERT unless SM64DS_COOP is set in a coop room (the callbacks are only
// invoked by coop_world). Wire layouts below; sub-type ranges are fixed by the
// phase-2 plan: SYNB 0x0c-0x11 and SYNW 0x30-0x3f.
//
// ENVIRONMENT (proof knobs, all optional, all inert when unset):
//   SM64DS_COOP_LOG=1            [coopsw] lines
//   SM64DS_SW_OFF=a,b,..         must-fail controls, ignore received: word,
//                                press, kick, field, globals
//   SM64DS_SW_SET=F:bit  SM64DS_SW_CLEAR=F:bit   at group frame F do what a
//                                ROM setter does (data_0209f34c bit) locally
//   SM64DS_SW_FORGE=F:name       one forged datagram: bit32, slot, class,
//                                short, long, badkey, count, state, zero
//   SM64DS_SW_PRESS=F:cls[:nth] at group frame F press / kick the nth live actor of the class
//                                (the switch's own ROM entry, the same body slot 21 reaches)
//   SM64DS_SW_WATCH=cls:off:size log every change of that field of every actor
//                                of the class (the proof's view of a target)
//   SM64DS_COOP_REC=f0:f1:f2     press recorder: samples at group frames f0,
//                                f1 (stable reference) and f2 (after the
//                                press), then prints the diff

#ifndef PORT_HAL_COOP_SWITCH_H
#define PORT_HAL_COOP_SWITCH_H

#include "coop_wire.h"

#include <cstdint>
#include <cstring>

namespace port {

// ---- SYNB (control) sub-types, 0x0c-0x11 -----------------------------------
enum : uint8_t {
    kSwCtlGlobals = 0x0c,   // owner -> joiner: word + stamps + level globals
    kSwCtlStates  = 0x0d,   // owner -> joiner: pressed / kicked / field list
    kSwCtlLast    = 0x11,
};

// ---- SYNW (world) sub-types, 0x30-0x3f -------------------------------------
enum : uint8_t {
    kSwWorldBit   = 0x30,   // event word bit changes, 1..8 entries
    kSwWorldWord  = 0x31,   // the whole word + 32 stamps (low rate, idempotent)
    kSwWorldPress = 0x32,   // a switch was pressed
    kSwWorldKick  = 0x33,   // a kicked block / chest left its idle state
    kSwWorldField = 0x34,   // a watched field changed (WDW water target)
    kSwWorldLast  = 0x3f,
};

enum : int {
    kSwBits        = 32,
    kSwMaxBitEnt   = 8,
    kSwMaxStateEnt = 40,
    kSwSlots       = 512,
    kSwKindPress   = 1,
    kSwKindKick    = 2,
    kSwKindField   = 3,
};

#pragma pack(push, 1)
struct SwBitEntry { uint8_t bit; uint8_t value; uint16_t stamp; };   // 4 B
struct SwBitHead  { CoopKeyWire key; uint8_t count; uint8_t reserved[3]; };   // 8 B + 4 per entry
struct SwWordWire { CoopKeyWire key; uint32_t word; uint16_t stamp[kSwBits]; };   // 72 B
// press / kick / field: one object, a sender sequence so a late resend never
// undoes a newer change.
struct SwObjWire {
    CoopKeyWire key;
    uint16_t slot;       // placed-object slot (actor +0x0ce), 0..511
    uint16_t cls;        // actor class (actor +0x0c): must match the actor in that slot
    uint8_t  state;      // press: 1; kick: the new state; field: the field id
    uint8_t  reserved;   // 0
    uint16_t seq;        // non-zero, per sender, wrap-safe
    int32_t  value;      // field value, 0 for press / kick
};                       // 16 B
struct SwGlobalsWire {
    CoopKeyWire key;
    uint32_t nonce;      // the joiner's enter nonce, echoed
    uint8_t  target;     // the joiner's seat
    uint8_t  reserved[3];
    uint32_t word;
    uint8_t  globals[4]; // data_0209f2c0[0..3]
    uint16_t stamp[kSwBits];
};                       // 84 B
struct SwStateEntry { uint16_t slot; uint16_t cls; uint8_t kind; uint8_t state; int32_t value; };   // 10 B
struct SwStatesHead {
    CoopKeyWire key;
    uint32_t nonce;
    uint8_t  target;
    uint8_t  count;      // 0..kSwMaxStateEnt
    uint8_t  reserved[2];
};                       // 12 B + 10 per entry
#pragma pack(pop)

static_assert(sizeof(SwBitEntry) == 4, "bit entry");
static_assert(sizeof(SwBitHead) == 8, "bit head");
static_assert(sizeof(SwWordWire) == 72, "word");
static_assert(sizeof(SwObjWire) == 16, "obj");
static_assert(sizeof(SwGlobalsWire) == 84, "globals");
static_assert(sizeof(SwStateEntry) == 10, "state entry");
static_assert(sizeof(SwStatesHead) == 12, "states head");

// Classes a message may name. A forged datagram naming anything else is dropped.
inline bool sw_press_class_ok(int c) { return c == 10 || c == 11 || c == 12; }
inline bool sw_kick_class_ok(int c) { return c == 14 || c == 26 || c == 45 || c == 66 || c == 83 || c == 139 || c == 126 || c == 353 || c == 354; }
// PUZZLES9: 13 TreasureChest (value = open | last << 1), 27 Stump (value = the pound counter).
inline bool sw_field_class_ok(int c) { return c == 101 || c == 13 || c == 27 || c == 158; }
inline bool sw_class_ok(int kind, int c) {
    return kind == kSwKindPress ? sw_press_class_ok(c) :
           kind == kSwKindKick ? sw_kick_class_ok(c) :
           kind == kSwKindField ? sw_field_class_ok(c) : false;
}
// Plausible range of the one field we move (WDW water target height, Fix12 world units).
inline bool sw_field_value_ok(int32_t v) { return v > -(1 << 28) && v < (1 << 28); }
inline bool sw_field_value_ok_cls(int c, int32_t v) {
    if (c == 13) return v >= 0 && v <= 3;     // 0 closed, 1 open, 3 open and it is the last chest
    if (c == 27) return v >= 0 && v <= 8;     // the post's pound counter (3 on a fresh post)
    return sw_field_value_ok(v);
}

inline bool sw_stamp_ok(uint16_t s) { return s == 0 || (s & 15) < kCoopMaxSeats; }

inline bool sw_bit_validate(const void *p, int len) {
    if (!p || len < (int)sizeof(SwBitHead) + (int)sizeof(SwBitEntry)) return false;
    SwBitHead h; std::memcpy(&h, p, sizeof h);
    if (!coop_key_ok(h.key) || h.count < 1 || h.count > kSwMaxBitEnt) return false;
    if (h.reserved[0] || h.reserved[1] || h.reserved[2]) return false;
    if (len != (int)sizeof h + (int)h.count * (int)sizeof(SwBitEntry)) return false;
    const unsigned char *s = static_cast<const unsigned char *>(p) + sizeof h;
    for (int i = 0; i < (int)h.count; ++i) {
        SwBitEntry e; std::memcpy(&e, s + i * sizeof e, sizeof e);
        if (e.bit >= kSwBits || e.value > 1 || e.stamp == 0 || !sw_stamp_ok(e.stamp)) return false;
    }
    return true;
}
inline bool sw_word_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SwWordWire)) return false;
    SwWordWire w; std::memcpy(&w, p, sizeof w);
    if (!coop_key_ok(w.key)) return false;
    for (int i = 0; i < kSwBits; ++i) if (!sw_stamp_ok(w.stamp[i])) return false;
    return true;
}
inline bool sw_obj_validate(int kind, const void *p, int len) {
    if (!p || len != (int)sizeof(SwObjWire)) return false;
    SwObjWire w; std::memcpy(&w, p, sizeof w);
    if (!coop_key_ok(w.key) || w.slot >= kSwSlots || w.reserved != 0 || w.seq == 0) return false;
    if (!sw_class_ok(kind, w.cls)) return false;
    if ((w.cls == 353 && w.slot >= 64) || (w.cls == 354 && w.slot >= 4)) return false;   // a door is named by its table index
    if (kind == kSwKindPress) return w.state == 1 && w.value == 0;
    if (kind == kSwKindKick) return w.state >= 1 && w.state <= 3 && w.value == 0;
    return w.state == 0 && sw_field_value_ok_cls(w.cls, w.value);
}
inline bool sw_globals_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(SwGlobalsWire)) return false;
    SwGlobalsWire w; std::memcpy(&w, p, sizeof w);
    if (!coop_key_ok(w.key) || w.target >= kCoopMaxSeats) return false;
    if (w.reserved[0] || w.reserved[1] || w.reserved[2]) return false;
    for (int i = 0; i < 4; ++i) if (w.globals[i] > 7) return false;
    for (int i = 0; i < kSwBits; ++i) if (!sw_stamp_ok(w.stamp[i])) return false;
    return true;
}
inline bool sw_states_validate(const void *p, int len) {
    if (!p || len < (int)sizeof(SwStatesHead)) return false;
    SwStatesHead h; std::memcpy(&h, p, sizeof h);
    if (!coop_key_ok(h.key) || h.target >= kCoopMaxSeats || h.count > kSwMaxStateEnt) return false;
    if (h.reserved[0] || h.reserved[1]) return false;
    if (len != (int)sizeof h + (int)h.count * (int)sizeof(SwStateEntry)) return false;
    const unsigned char *s = static_cast<const unsigned char *>(p) + sizeof h;
    for (int i = 0; i < (int)h.count; ++i) {
        SwStateEntry e; std::memcpy(&e, s + i * sizeof e, sizeof e);
        if (e.slot >= kSwSlots || e.kind < kSwKindPress || e.kind > kSwKindField) return false;
        if (!sw_class_ok(e.kind, e.cls)) return false;
        if ((e.cls == 353 && e.slot >= 64) || (e.cls == 354 && e.slot >= 4)) return false;
        if (e.kind == kSwKindField ? (e.state != 0 || !sw_field_value_ok_cls(e.cls, e.value))
                                   : (e.state < 1 || e.state > 3 || e.value != 0)) return false;
    }
    return true;
}

// ---- the stamp rule, pure so a table-top test can drive it -------------------
inline unsigned sw_stamp_make(unsigned counter, int seat) { return ((counter & 0xFFFu) << 4) | ((unsigned)seat & 15u); }
inline unsigned sw_stamp_counter(unsigned stamp) { return stamp >> 4; }
// One bit's record and the single rule that keeps every console's copy equal:
// a change is taken only with a strictly higher stamp (stamps carry the sender
// seat in the low nibble, so two different senders never tie).
struct SwBitModel { unsigned stamp; uint8_t val; };
inline bool sw_adopt(SwBitModel &b, unsigned stamp, int v) {
    if (stamp == 0 || stamp <= b.stamp) return false;
    b.stamp = stamp; b.val = (uint8_t)(v ? 1 : 0);
    return true;
}
// Wrap-safe "newer" for the per-sender sequence (0 = nothing seen yet).
inline bool sw_seq_newer(uint16_t seq, uint16_t last) { return last == 0 || (int16_t)(uint16_t)(seq - last) > 0; }

struct CoopSwitchStats {
    unsigned long long sent_bit, sent_word, sent_press, sent_kick, sent_field, sent_globals, sent_states;
    unsigned long long recv_bit, recv_word, recv_press, recv_kick, recv_field, recv_globals, recv_states;
    unsigned long long bits_applied, bits_local, press_applied, kick_applied, field_applied,
                       globals_applied, states_applied, states_pending;
    unsigned long long drop_bad, drop_group, drop_member, drop_class, drop_old, drop_off, drop_notfound;
};
CoopSwitchStats coop_switch_stats();

}  // namespace port

#endif  // PORT_HAL_COOP_SWITCH_H
