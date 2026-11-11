// COOP BOSSES: a boss is an ENEMY that also carries HP, a phase and a defeat. Run mp1, lane BOSSCORE1.
//
// Tango's design: "Bosses shouldn't be a big deal. Just sync location (which you'll already do for enemies), then
// sync attacks and animations. The only issue is who the boss attacks: the closest player, with about a second of
// delay before switching."
//
//   * LOCATION: the boss rows in coop_actor_profiles.inc (COOPMOVE1's path, class 165 Whomp King, 189 King Bob-omb):
//     owner by proximity, pose slaved every frame, snapshot for a late joiner.
//   * WHO IT ATTACKS: coop_target.* (closest player, 1 s hold, the same seat on every console).
//   * ATTACKS / ANIMATION / HP / PHASE / DEFEAT: this file. Every console runs the boss's own Behavior. The AUTHORITY
//     is the console of the boss's target seat (coop_target_seat_of_slot, the group owner when there is none), because
//     that console's copy is the one the boss is actually chasing, so it is the one that is right. The authority
//     publishes a small record of the boss's state words (the state, HP, the sub-state and timers, the animation
//     cursor) at 10 Hz when anything changed and as a 400 ms heartbeat (that heartbeat is also how a late joiner
//     learns the current HP and phase). The others take it:
//       - HP only goes down: a console takes min(local, authority).
//       - the state: when the local state has differed from the authority's for two records in a row (or on the
//         first record a console ever hears), the console is put into the authority's state THE ROM'S OWN WAY (King
//         Bob-omb: KingBobOmb_SetState, which runs the state's enter code; Whomp King: the state word plus the
//         timers its Behavior resets on a transition).
//       - a hit that lands on a console that is NOT the authority (its HP dropped by itself) is sent as BOSS_HIT; the
//         authority takes the hitter's record whole, so a hit by either player counts once for everybody.
//     A star on defeat: every console runs the defeat code, but dActor_c::UntrackAndSpawnStar is replaced so that only the
//     boss's authority spawns the star; the others get it through COOPSPAWN1's announcement (one star for all).
//
// WIRE (SYNW 0x51-0x53, dispatched from coop_target.cpp; every field range-checked, unknown = dropped and counted):
//   0x51 BOSS_STATE  BossHead (8 B) + count * (BossRec 8 B + nf * u32)
//   0x52 BOSS_HIT    same layout, count 1, flags bit0 set
//
// INERT unless SM64DS_COOP is set in a coop room. ENVIRONMENT (proof knobs, inert unless named):
//   SM64DS_COOP_BOSS=0           MUST-FAIL control: no boss sync (two separate fights)
//   SM64DS_COOP_BOSS_LOG=1       [cbs] lines: a sample per boss every 500 ms, every hit, apply, claim, defeat
//   SM64DS_COOP_BOSS_HIT=F:slotsel   at boss tick F lower the boss's HP by one the ROM's way (sets its own hit flag,
//                                where the class has one) on THIS console: the proof's "a hit from either player"
//   SM64DS_COOP_BOSS_STAND=F:d      at boss tick F put the local player d units beside the first boss (proof only)
//   SM64DS_COOP_BOSS_GOD=1           keep the local player at full health (proof only)
//   SM64DS_COOP_BULLY=0              MUST-FAIL control (coopbully1): no bully claim, a knock stays on the hitter's console
//   SM64DS_COOP_BULLY_BUMP=F:cls[:every[:count[:angle[:speed]]]]   from boss tick F knock the nearest live bully of class 215 / 216 back
//                                    the way a punch does, on THIS console, with the local player put behind it (proof only)
//   SM64DS_COOP_BOSS_FORGE=F:name    one forged datagram (short long count cls slot nf hp key flags notboss)

#ifndef PORT_HAL_COOP_BOSS_H
#define PORT_HAL_COOP_BOSS_H

#include "coop_wire.h"

#include <cstdint>

namespace port {

enum : int {
    kBossMaxFields = 14,
    kBossMaxRecs   = 8,
    kBossHeartbeatMs = 400,
};

#pragma pack(push, 1)
struct BossHead {            // 8 B
    CoopKeyWire key;
    uint8_t count;           // 1..kBossMaxRecs
    uint8_t flags;           // 0 for STATE; bit0 for HIT
    uint16_t reserved;       // 0
};
struct BossRec {             // 8 B, then nf * u32
    uint16_t slot;           // death-table slot 0..511
    uint16_t cls;            // actor class id (cross-check)
    uint8_t  nf;             // must equal the class row's field count
    uint8_t  rflags;         // bit0 defeated / dying, bit1 claim (my copy is carried by my player: I am the authority)
    uint16_t seq;            // per sender, wrap-safe
};
#pragma pack(pop)
static_assert(sizeof(BossHead) == 8, "boss head is 8 bytes");
static_assert(sizeof(BossRec) == 8, "boss record is 8 bytes");

struct CoopBossStats {
    unsigned long long ticks, bosses, sent, recs_sent, hits_sent, rx, recs_rx, hits_rx;
    unsigned long long applied_state, applied_hp, applied_fields, state_mismatch, hit_local, hit_adopted, claims;
    unsigned long long drop_bad, drop_group, drop_member, drop_notauth, drop_cls, drop_off, drop_seat;
    unsigned long long stars_spawned, stars_suppressed, hit_spurious, dead_marked, dead_killed;
};
CoopBossStats coop_boss_stats();

// bossfix1. True while a Bowser (class 279) is in its defeat state: coop_target then answers the LOCAL player for it, so the defeat talk
// runs with each console's own player instead of a parked friend.
bool coop_boss_prefer_local(const char *actor);
// True when a boss was seen on this console within the last 4 s and another console is its authority: a reward (key, Grand Star)
// spawned here natively is then a duplicate of the authority's announced one.
bool coop_boss_reward_not_mine();

// coopbully1: the seat whose player knocked this bully (215 / 216) and owns its pose and state for now, or -1; the record slot (505..507) of
// one of the Big Bully's three runtime bullies, or -1.
int coop_boss_bully_claim_seat(int slot);
int coop_bully_virtual_slot(const void *actor);
// Validator (payload = after the 12-byte head).
bool coop_boss_validate(const void *payload, int len);
// Called by coop_target.cpp's dispatcher for sub-types 0x51-0x53.
void coop_boss_on_world(int seat, int sub, const unsigned char *payload, int len);

}  // namespace port

#endif  // PORT_HAL_COOP_BOSS_H
