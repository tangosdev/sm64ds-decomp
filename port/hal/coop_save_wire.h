// COOP SAVE: the 'SYNB' sub-types 0x20-0x2f and the pure record rules.
// Run mp1 wave 56, lane COOPSAVE1. No dependencies on the carrier or the game,
// so the proof tools can compile and exercise it alone.
//
// THE COMMON HEADER is hub_presence.h's HubCoopHead (12 bytes: tag 'SYNB',
// ver 1, seat (relay-stamped), subtype, flags 0, seq, payload_len). The W56
// text lists the head as ver / subtype / seat; the shipped header (WIRE1,
// "ITS HEADER WINS") puts the seat at byte 5. COOPWORLD1's coop_wire.h is not
// needed for this file: only the head from hub_presence.h is, and the sub-type
// numbers below are inside the range W56 gave this lane.
//
// SUB-TYPES (0x20-0x2f is this lane's range; anything else in it is dropped
// and counted):
//
//   0x20 SAVE_REQ  guest -> everyone, host answers. payload 4 bytes, all 0
//                  (non-zero = dropped). Resent at 4 Hz until a host record
//                  arrives.
//   0x22 TALLY     (lane SMALLS9, hal/star_tally.h) each player's own star
//                  count this session: a guest's entry to the host, the host's
//                  table to everyone. Not part of the save record.
//   0x21 SAVE_REC  a file record. payload 72 bytes:
//                     u8  from_host   1 = the host's merged record (guests
//                                      take it), 0 = a guest's record (only
//                                      the host takes it); any other value
//                                      is dropped
//                     u8  flags       bit0 = BLANK: the host has no record
//                                      yet (a new file); the rec bytes are 0
//                                      and mean nothing. Other bits 0.
//                     u16 reserved    0 (non-zero = dropped)
//                     u8  rec[0x44]   one FileSaveData, as the ROM keeps it
//
// WHAT A RECORD CAN DO TO THE RECEIVER. Only the bits in kMonoMask (below)
// are ever read out of a received record, and they are only ever OR-ed IN. A
// received record cannot clear a bit, cannot set a scalar (current character,
// controller mode, coin records, the cap-lost flags), and cannot change the
// magic. So the worst a forged SAVE_REC does is set save bits that the player
// could have earned: stars, doors, cannons, unlocks. It cannot lock anyone out
// of anything or overwrite what they have.

#ifndef PORT_HAL_COOP_SAVE_WIRE_H
#define PORT_HAL_COOP_SAVE_WIRE_H

#include <stdint.h>
#include <string.h>

namespace port {

enum : int {
    kCoopSaveSubReq = 0x20,
    kCoopSaveSubRec = 0x21,
    kCoopSaveSubTally = 0x22,   // SMALLS9: per-player star tally, see star_tally.h
    kCoopSaveSubLo  = 0x20,
    kCoopSaveSubHi  = 0x2f,
};

enum : int {
    kCoopRecBytes     = 0x44,            // FileSaveData
    kCoopRecPayload   = 4 + kCoopRecBytes,   // 72
    kCoopReqPayload   = 4,
    kCoopRecFlagBlank = 0x01,
};

// FileSaveData, as the ROM spells it (src/func_02013c84.c, SaveData.h and the
// readers named next to each row):
//   +0x00 u32 magic "8000"                      scalar, never merged
//   +0x04 u32 flags1                            MIXED, see the mask
//   +0x08 u32 flags2                            MIXED, see the mask
//   +0x0c u32 level / door unlock bits (28)     monotonic (func_02013868)
//   +0x10 u32 cannons open                      monotonic (OpenCannon)
//   +0x14 u8[30] stars, 8 bits per course       monotonic (CollectStar)
//   +0x32 u8[15] coin records                   per console (best coins; not
//                                               merged: a scalar, and each
//                                               player's own best)
//   +0x41 u8 current character                  per console
//   +0x42 u8 controller mode                    per console
//   +0x43 u8 unknown                            per console
//
// flags1 (+0x04) bits, OR-ed: bit 0 in-use, bits 1..23 (key / switch / gate
// flags set by their actors; star gate 0x8000<<n, ov100 0x100<<n, ov089 2<<n)
// and bit 31 (the ? switch, d_a_obj_hatena_switch). NOT merged: bits 24..30
// (cap lost / cap available per character, set AND cleared by play, and
// 0x40000000 shares the range). Bits 6 and 7 (0x40, 0x80, the Mip rabbit
// flags) are monotonic.
// KEYDOOR10: bits 2, 4 and 9 are merged too. They are key door bits
// (d_a_door.cpp func_ov100_02144cf8; the door table data_ov100_02148204 gives
// key door k the "key held" bit 2 << k and the "door unlocked" bit 0x100 << k):
// 0x4 = key 1 held (the Fire Sea key, the upstairs Bowser door), 0x200 = that
// door unlocked, 0x10 = key 3 held. Left out, a friend on a reloaded coop save
// never had them and that door stayed shut for him. Every ROM write of 0x10
// sets it (d_a_obj_key.cpp:727). 0x204 is set by the key pickup and the door
// (d_a_obj_key.cpp:727, d_a_door.cpp:649) and cleared in one place, the
// castle fall cutscene (FALL_IN_FRONT_OF_CASTLE_CUTSCENE,
// ContinueKuppaScriptIfNecessary.c:20);
// sharing it can at most keep that door open and that key held for someone,
// the state the cartridge has before that cutscene, so it can never lock
// anyone out.
// So the mask is 0x80FFFFFF.
//
// flags2 (+0x08) monotonic bits: 0..3 (character intro / unlock, bit 3 is the
// default), 7 (the opening was seen, LakituBro), 9 and 10..14 (Player's
// one-shot cutscene bits 0x200, 0x400..0x4000), 20..27 (the eight glowing
// rabbits, 0x100000 << n) and 28 (0x10000000, a one-shot). NOT merged: bit 17
// (0x20000) and bit 19 (0x80000), which daMip clears and sets as state.
inline const uint8_t *coop_mono_mask() {
    static uint8_t m[kCoopRecBytes];
    static bool built = false;
    if (!built) {
        memset(m, 0, sizeof m);
        const uint32_t f1 = 0x80FFFFFFu;   // KEYDOOR10: + 0x214 (key door bits)
        const uint32_t f2 = 0x0000000Fu | 0x80u | 0x200u | 0x400u | 0x800u |
                            0x1000u | 0x2000u | 0x4000u | 0x0FF00000u |
                            0x10000000u;
        const uint32_t wd = 0x0FFFFFFFu;     // +0x0c: 28 bits
        const uint32_t cn = 0xFFFFFFFFu;     // +0x10: cannons
        memcpy(m + 0x04, &f1, 4);            // little endian host
        memcpy(m + 0x08, &f2, 4);
        memcpy(m + 0x0c, &wd, 4);
        memcpy(m + 0x10, &cn, 4);
        for (int i = 0x14; i < 0x32; ++i) m[i] = 0xFF;   // 30 star bytes
        built = true;
    }
    return m;
}

// OR the monotonic bits of `src` into `dst`. Returns how many bits were newly
// set (0 = nothing to do). Touches nothing else.
inline int coop_merge_record(uint8_t *dst, const uint8_t *src) {
    const uint8_t *mk = coop_mono_mask();
    int added = 0;
    for (int i = 0; i < kCoopRecBytes; ++i) {
        const uint8_t add = (uint8_t)(src[i] & mk[i] & (uint8_t)~dst[i]);
        if (add) {
            dst[i] = (uint8_t)(dst[i] | add);
            for (uint8_t b = add; b; b = (uint8_t)(b & (b - 1))) ++added;
        }
    }
    return added;
}

// Is every monotonic bit of `src` already in `dst`?
inline bool coop_record_covers(const uint8_t *dst, const uint8_t *src) {
    const uint8_t *mk = coop_mono_mask();
    for (int i = 0; i < kCoopRecBytes; ++i)
        if (src[i] & mk[i] & (uint8_t)~dst[i]) return false;
    return true;
}

// Stars set in a record (the 30 bytes at +0x14).
inline int coop_record_stars(const uint8_t *rec) {
    int n = 0;
    for (int i = 0x14; i < 0x32; ++i)
        for (uint8_t b = rec[i]; b; b = (uint8_t)(b & (b - 1))) ++n;
    return n;
}

// The ROM's record checksum (backup.cpp section 4): the eight tag bytes summed
// as a u16, then per payload byte rotate left one and xor.
inline uint16_t coop_record_checksum(const uint8_t tag[8], const uint8_t *p,
                                     int n) {
    uint16_t s = 0;
    for (int i = 0; i < 8; ++i) s = (uint16_t)(s + tag[i]);
    for (int i = 0; i < n; ++i)
        s = (uint16_t)((uint16_t)((s << 1) | (s >> 15)) ^ p[i]);
    return s;
}

// Payload encode / decode. The caller wraps the payload with
// hub_coop_encode(kHubTagCoopCtl, ...) and validates the head with
// hub_coop_validate first.
inline int coop_rec_encode(uint8_t *out72, bool from_host, bool blank,
                           const uint8_t *rec68) {
    memset(out72, 0, kCoopRecPayload);
    out72[0] = from_host ? 1 : 0;
    out72[1] = blank ? kCoopRecFlagBlank : 0;
    if (!blank && rec68) memcpy(out72 + 4, rec68, kCoopRecBytes);
    return kCoopRecPayload;
}

// -1 = malformed. Else 0 = guest record, 1 = host record.
inline int coop_rec_decode(const uint8_t *pl, int len, bool *blank,
                           uint8_t *rec68) {
    if (!pl || len != kCoopRecPayload) return -1;
    if (pl[0] > 1) return -1;
    if (pl[1] & ~kCoopRecFlagBlank) return -1;
    if (pl[2] || pl[3]) return -1;
    const bool b = (pl[1] & kCoopRecFlagBlank) != 0;
    if (b && pl[0] == 0) return -1;      // a guest never sends "blank"
    if (blank) *blank = b;
    if (rec68) {
        if (b) memset(rec68, 0, kCoopRecBytes);
        else memcpy(rec68, pl + 4, kCoopRecBytes);
    }
    return pl[0];
}

}  // namespace port

#endif  // PORT_HAL_COOP_SAVE_WIRE_H
