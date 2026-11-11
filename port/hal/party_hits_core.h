// PARTY HITS: THE PURE CORE (lane PLAYINT1). No Windows, no ROM, no globals: every rule that decides
// whether a friend's punch / stomp / eat is believed, and what it does to the victim, lives here as a
// function of plain numbers, so it can be compiled and driven on any host (port/tools/party_hits_proof.py
// does, with g++). hal/party_hits.cpp is the glue that feeds it from the live game.
//
// THE MODEL (cloud_tasks/PLAYINT1_DESIGN.md has the long form). Nobody refereed: the VICTIM'S console
// decides, against its own truth, and moves its own player. The attacker's console only says "I think I
// touched you" (a claim); the claim is believed only if the victim itself saw the attacker in reach, in
// the same level / area / act, and the victim could be hit right now.
//
// Units: positions are WHOLE game units (Fix12 >> 12), angles are the game's 16-bit angle, time is
// FRAMES of the victim's own 30 Hz game clock.
#ifndef PORT_HAL_PARTY_HITS_CORE_H
#define PORT_HAL_PARTY_HITS_CORE_H

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace port {
namespace phit {

// ---- the interaction kinds (4 bits, packed into the top of SYNH.seq) -----------------------------
// The relay checks SYNH.hit_kind < 1 (relay.py HIT_KINDS) and nothing about seq, so the wire stays
// untouched: hit_kind is always kHitShove (0) and the interaction rides in seq bits 12..15.
enum IKind : unsigned {
    kMelee = 0,        // punch / kick / spin / dive / slide kick: shove + stun
    kPound = 1,        // ground pound landing next to the friend: bigger shove + stun
    kStomp = 2,        // landed on the friend's head: squash, small hop for the victim
    kEatClaim = 3,     // Yoshi tongue touched the friend (eater -> victim)
    kEatAck = 4,       // victim -> eater: "I am in your mouth"
    kEatRelease = 5,   // victim -> eater: "spat out / gave up" (x = 1: broke free by itself, the eater is hurt)
    // INTERACT9: a Wario hold of a friend and the attach broadcast (same SYNH / SYNB 0x41 channel).
    kGrabCatch = 6,    // holder -> victim: a claim, judged like a melee but a stun is fine (victim side)
    kGrabThrow = 7,    // holder -> victim: let go with a throw (yaw = launch yaw, x = speed >> 8)
    kGrabSwing = 8,    // holder -> victim: let go off the spin (same fields)
    kGrabBreak = 9,    // victim -> holder: "I broke free" (the holder drops me)
    kAttach = 10,      // victim -> every other seat: x = 0 off / 1 eaten / 2 carried, yaw = the holder's seat
    kGrabAck = 11,     // victim -> holder: "I am in your hands"
    // TONGUE1: what I hold, to every other seat (on change and every second while I hold something): x = kind (0 nothing,
    // 1 an object in my hands, 2 an object in my mouth) | eggs following me << 4, yaw = the object's spawn slot (+0xce),
    // y = its class id. Builds before it refuse kind 12 as a bad kind and draw nothing, as before.
    kHeldState = 12,
    // FRIENDVIS1: one record of a Yoshi egg I threw, to every other seat in a PARTY room (every 2 frames while it flies, a
    // burst three times): yaw = egg id (low byte of its actor id) | flags << 8 (1 in hand, 2 flying, 4 burst), x / y / z =
    // its position. Builds before it judge kind 13 as a claim and refuse it (kBadKind: not a victim-side kind), drawing nothing.
    kEggFlight = 13,
    // HEADBOUNCE1: a friend landed on my head and I took it (squash only, no knock): to every other seat so each console
    // draws my squash on my ghost. yaw = the bouncer's seat. Builds before it refuse kind 14 as a bad kind and draw nothing.
    kSquish = 14,
    kKinds = 15,
};
// seq = kind (4 bits) | stamp (4 bits) | counter (8 bits). The stamp is the attacker's estimate, in frames,
// of how old its view of the victim was when it struck (INTERACT9: the victim judges against its own pose
// that many frames ago).
inline unsigned pack_seq(unsigned ikind, unsigned counter, unsigned stamp = 0) {
    return ((ikind & 0xFu) << 12) | ((stamp & 0xFu) << 8) | (counter & 0xFFu);
}
inline unsigned seq_ikind(unsigned seq) { return (seq >> 12) & 0xFu; }
inline unsigned seq_stamp(unsigned seq) { return (seq >> 8) & 0xFu; }
inline unsigned seq_counter(unsigned seq) { return seq & 0xFFu; }
inline bool ikind_is_claim(unsigned k) { return k <= kEatClaim || k == kGrabCatch; }   // victim-side kinds
inline bool ikind_is_reply(unsigned k) { return k >= kEatAck && k != kGrabCatch && k < kKinds; }   // everything that is not a claim

// ---- the numbers (all tunable on a device; the proof pins the rules, not the values) -------------
enum : int {
    kBodyRadius = 56,            // 40 body + 16 pad (party_cyl.cpp)
    kLagAllowStamped = 90,       // INTERACT9: the slack left once the claim says how old my pose was
    kLagAllow = 150,             // how far a friend can have moved between his frame and my reading (units)
    kPosSlack = 400,             // claimed position vs the position I saw him at
    kCooldownFrames = 30,        // one believed claim per attacker per this
    kPresenceMaxAgeMs = 3000,    // SYNL older than this = no word (same as party::gate)
    kGhostMaxAgeMs = 1500,       // newest SYNG older than this: I do not know where he is, refuse
    kEatRearmFrames = 150,       // nobody is eaten twice inside this
    kEatAckWaitFrames = 20,      // eater: no ack by then, the claim is dead
    kEatHoldFrames = 45,         // victim: held in the mouth this long, then spat
    kEatMaxFrames = 90,          // victim: hard stop whatever else happens
    kEatSpitUnits = 140,         // how far the spit travels (units of horizontal speed*frames, see speed)
    kMouthAhead = 60,            // units in front of the eater's claimed yaw
};

// Reach per kind. dy = attacker.y - victim.y (units).
struct ReachRule { int horiz; int dy_lo; int dy_hi; };
inline ReachRule reach_rule(unsigned ikind) {
    switch (ikind) {
    case kMelee:    return { 2 * kBodyRadius + 70 + kLagAllow, -120, 150 };
    case kPound:    return { 220 + kLagAllow, -120, 150 };
    case kStomp:    return { 2 * kBodyRadius + kLagAllow, 115, 260 };   // HEADBOUNCE1: feet over feet; under 115 he passes the waist
    case kEatClaim: return { 420 + kLagAllow, -150, 200 };
    case kGrabCatch: return { 2 * kBodyRadius + 70 + kLagAllow, -120, 150 };
    }
    return { 0, 0, -1 };         // empty window: nothing reaches
}

// ---- the room toggles ---------------------------------------------------------------------------
struct Toggles {
    bool interact = true;        // SM64DS_ROOM_INTERACT (default 1)
    bool damage = false;         // SM64DS_ROOM_DAMAGE   (default 0)
    bool room = false;           // a party / coop room is up and this console is in it
};
inline bool env_flag(const char *v, bool dflt) {
    if (!v || !*v) return dflt;
    return std::atoi(v) != 0;
}

// ---- the verdicts --------------------------------------------------------------------------------
enum Verdict : int {
    kOk = 0,
    kNoRoom,           // not a party / coop room (solo, VS, world): nothing is ever believed
    kToggleOff,        // SM64DS_ROOM_INTERACT=0
    kBadKind,          // not a victim-side kind, or an unknown one
    kBadSeat,          // attacker is me / out of range / not a member
    kNoPresence,       // no (fresh) SYNL from him
    kWrongLevel,
    kWrongArea,
    kWrongAct,
    kNotInLevel,       // he is in a menu / cutscene / no level
    kNoGhost,          // no fresh SYNG position for him
    kPosMismatch,      // the position he claims is far from where I saw him
    kOutOfReach,
    kVictimBusy,       // my body cannot be hit now (hurt, invulnerable, star-get, cutscene, mouth...)
    kCooldown,         // a believed claim from him inside kCooldownFrames
    kEatBusy,          // already eaten, or eaten too recently
    kVerdicts,
};
inline const char *verdict_name(int v) {
    static const char *const n[kVerdicts] = {
        "ok", "no_room", "toggle_off", "bad_kind", "bad_seat", "no_presence", "wrong_level", "wrong_area",
        "wrong_act", "not_in_level", "no_ghost", "pos_mismatch", "out_of_reach", "victim_busy", "cooldown",
        "eat_busy" };
    return (v >= 0 && v < kVerdicts) ? n[v] : "?";
}

// ---- what the victim knows when a claim arrives ---------------------------------------------------
struct Place { int level = 0, area = 0, act = 0; };       // act 0 = unknown / none (not judged)
struct Seen {                                             // one friend as this console sees him
    bool have_presence = false; unsigned presence_age_ms = 0;
    bool in_level = true;                                 // SYNL: in a level, not a menu / cutscene
    Place at;
    bool have_ghost = false; unsigned ghost_age_ms = 0;
    int x = 0, y = 0, z = 0;                              // newest SYNG position, units
};
struct Me {
    Place at;
    int x = 0, y = 0, z = 0;
    bool busy = false;           // the caller's "cannot be hit now" test (state / invuln / mouth / cutscene)
    bool eaten = false;          // I am already in somebody's mouth
    // HITLAG3: my poses one frame either side of the stamped one (the stamp is the claim's age rounded to whole frames, so
    // the pose the attacker tested can sit a frame off it). A claim reaches when any of them does.
    int near_n = 0;
    int near_x[2] = {}, near_y[2] = {}, near_z[2] = {};
};
struct Claim {
    int attacker = 0, victim = 0;
    unsigned ikind = 0, counter = 0;
    unsigned stamp = 0;          // frames the attacker's view of me lagged (0 = unknown)
    int yaw = 0;                 // attacker yaw, 16-bit angle
    int x = 0, y = 0, z = 0;     // attacker's claimed position, units
};

// The lab's must-fail switches. Production: all true. Each one turns ONE check off so the proof can show
// the matching forged claim then lands (the row is only meaningful if the control fails).
struct Rules {
    bool check_toggle = true, check_room = true, check_level = true, check_reach = true,
         check_cooldown = true, check_busy = true, check_pos = true, check_eat_rearm = true;
    bool reach_claimed = true;   // INTERACT9: reach is tested from the position the attacker CLAIMS (it passed the
                                 // position check against where I saw him), against my pose `stamp` frames ago
};

inline long long sq(long long v) { return v * v; }
inline long long horiz2(int ax, int az, int bx, int bz) { return sq((long long)ax - bx) + sq((long long)az - bz); }

// What the victim does with a believed claim.
struct Effect {
    unsigned ikind = 0;
    int away_yaw = 0;            // direction the victim is pushed (16-bit angle), from attacker to victim
    int speed = 0;               // horizontal speed, Fix12 units / frame
    int hop = 0;                 // vertical speed, Fix12 (0 = none)
    int damage_units = 0;        // health units, 0 unless SM64DS_ROOM_DAMAGE
    int squash_frames = 0;       // visual squash on the victim's body (stomp)
    bool hurt_state = false;     // enter the ROM's hurt state (the stun)
    bool eat = false;            // start the eat hold instead
    bool grab = false;           // enter the Wario hold instead
};

inline int angle_from_to(int ax, int az, int bx, int bz) {
    // the game's convention: 0 = +z, 0x4000 = +x (Vec3_HorzAngle is atan2(dx, dz))
    const double a = std::atan2((double)(bx - ax), (double)(bz - az));
    return (int)(((long long)std::llround(a * 32768.0 / 3.14159265358979323846)) & 0xFFFF);
}

inline Effect effect_for(const Claim &c, const Me &me, const Toggles &t) {
    Effect e;
    e.ikind = c.ikind;
    e.away_yaw = angle_from_to(c.x, c.z, me.x, me.z);
    switch (c.ikind) {
    case kMelee: e.speed = 0x1C000; e.hop = 0x18000; e.hurt_state = true; break;
    case kPound: e.speed = 0x24000; e.hop = 0x20000; e.hurt_state = true; break;
    case kStomp: e.speed = 0x04000; e.hop = 0x10000; e.hurt_state = true; e.squash_frames = 20; break;
    case kEatClaim: e.eat = true; break;
    case kGrabCatch: e.grab = true; break;
    }
    if (t.damage && !e.eat && !e.grab) e.damage_units = 1;
    return e;
}

// ---- the judge ------------------------------------------------------------------------------------
// One pose of mine against the attacker's position: inside the kind's horizontal reach and its height window.
inline bool pose_in_reach(const ReachRule &r, int ax, int ay, int az, int mx, int my, int mz) {
    const long long dy = (long long)ay - my;
    return horiz2(ax, az, mx, mz) <= sq(r.horiz) && dy >= r.dy_lo && dy <= r.dy_hi;
}

struct Counters {
    unsigned long long verdict[kVerdicts] = {};
    unsigned long long applied[kKinds] = {};
};

class Judge {
public:
    Counters ct;
    Rules rules;
    Toggles tog;

    // Decide one claim. `now` = my game frame. On kOk the cooldown is consumed and *fx is filled.
    Verdict judge(const Claim &c, const Seen &s, const Me &me, unsigned now, Effect *fx) {
        const Verdict v = decide(c, s, me, now);
        ++ct.verdict[v];
        if (v == kOk) {
            if (c.attacker >= 0 && c.attacker < kSeats) {
                last_[c.attacker] = now ? now : 1;
                if (c.ikind == kEatClaim) last_eat_ = now ? now : 1;
            }
            ++ct.applied[c.ikind];
            if (fx) *fx = effect_for(c, me, tog);
        }
        return v;
    }
    void reset() { std::memset(last_, 0, sizeof last_); last_eat_ = 0; }
    enum { kSeats = 4 };

private:
    unsigned last_[kSeats] = {};
    unsigned last_eat_ = 0;

    Verdict decide(const Claim &c, const Seen &s, const Me &me, unsigned now) const {
        if (rules.check_room && !tog.room) return kNoRoom;
        if (rules.check_toggle && !tog.interact) return kToggleOff;
        if (!ikind_is_claim(c.ikind)) return kBadKind;
        if (c.attacker < 0 || c.attacker >= kSeats || c.attacker == c.victim) return kBadSeat;
        if (!s.have_presence || s.presence_age_ms > (unsigned)kPresenceMaxAgeMs) return kNoPresence;
        if (rules.check_level) {
            if (!s.in_level) return kNotInLevel;
            if (s.at.level != me.at.level) return kWrongLevel;
            if (s.at.area != me.at.area) return kWrongArea;
            if (s.at.act > 0 && me.at.act > 0 && s.at.act != me.at.act) return kWrongAct;
        }
        if (!s.have_ghost || s.ghost_age_ms > (unsigned)kGhostMaxAgeMs) return kNoGhost;
        if (rules.check_pos) {
            // his claimed position must be where I saw him, within what a few frames of travel allow
            if (horiz2(c.x, c.z, s.x, s.z) > sq(kPosSlack) || std::llabs((long long)c.y - s.y) > kPosSlack)
                return kPosMismatch;
        }
        if (rules.check_reach) {
            // judged on MY truth: where I saw him (not where he says he is), against where I am now
            ReachRule r = reach_rule(c.ikind);
            if (c.stamp > 0 && r.horiz > kLagAllow) r.horiz -= (kLagAllow - kLagAllowStamped);   // a stamped claim needs less slack
            const int ax = rules.reach_claimed ? c.x : s.x, ay = rules.reach_claimed ? c.y : s.y, az = rules.reach_claimed ? c.z : s.z;
            if (!pose_in_reach(r, ax, ay, az, me.x, me.y, me.z)) {
                bool any = false;
                for (int i = 0; i < me.near_n && i < 2 && !any; ++i) any = pose_in_reach(r, ax, ay, az, me.near_x[i], me.near_y[i], me.near_z[i]);
                if (!any) return kOutOfReach;
            }
        }
        if (rules.check_busy && (me.busy || me.eaten)) return kVictimBusy;
        if (c.ikind == kEatClaim && rules.check_eat_rearm && last_eat_ && now - last_eat_ < (unsigned)kEatRearmFrames)
            return kEatBusy;
        if (rules.check_cooldown && last_[c.attacker] && now - last_[c.attacker] < (unsigned)kCooldownFrames)
            return kCooldown;
        return kOk;
    }
};

// ---- the eat hold (victim side): one session at a time --------------------------------------------
enum EatStep : int { kEatIdle = 0, kEatHolding, kEatSpit, kEatAbort };
struct EatInput {
    bool eater_present = true;   // fresh SYNL + SYNG, same level / area / act
    bool me_dead_or_menu = false;
    bool toggle_on = true;       // SM64DS_ROOM_INTERACT still on
};
class EatHold {
public:
    bool active() const { return step_ == kEatHolding; }
    int eater() const { return eater_; }
    unsigned since() const { return since_; }
    int spit_yaw() const { return yaw_; }
    void begin(int eater, int yaw, unsigned now) { step_ = kEatHolding; eater_ = eater; yaw_ = yaw; since_ = now ? now : 1; }
    // LAB (pictures): a longer hold. Production keeps kEatHoldFrames / kEatMaxFrames.
    void set_frames(unsigned hold, unsigned max) { hold_ = hold; max_ = max; }
    // Called once a frame while active. kEatSpit = hold time is up (launch yaw()), kEatAbort = let go
    // with no launch (eater gone, level changed, menu), kEatHolding = keep holding.
    EatStep tick(unsigned now, const EatInput &in) {
        if (step_ != kEatHolding) return step_;
        if (!in.toggle_on || !in.eater_present || in.me_dead_or_menu || now - since_ >= max_) {
            step_ = kEatIdle; return kEatAbort;
        }
        if (now - since_ >= hold_) { step_ = kEatIdle; return kEatSpit; }
        return kEatHolding;
    }
    void force_end() { step_ = kEatIdle; }
    // Where the held body is put each frame: in front of the eater's last known position.
    static void mouth_pos(int ex, int ey, int ez, int yaw, int *ox, int *oy, int *oz) {
        const double a = (double)yaw * 3.14159265358979323846 / 32768.0;
        *ox = ex + (int)std::lround(std::sin(a) * kMouthAhead);
        *oz = ez + (int)std::lround(std::cos(a) * kMouthAhead);
        *oy = ey + 20;
    }
private:
    EatStep step_ = kEatIdle;
    int eater_ = -1, yaw_ = 0;
    unsigned since_ = 0;
    unsigned hold_ = (unsigned)kEatHoldFrames, max_ = (unsigned)kEatMaxFrames;
};

// ---- the eater side: remember a claim until the ack (or its time-out) -----------------------------
class EatClaimBook {
public:
    // Eater sent a claim at frame `now` to `victim`.
    void sent(int victim, unsigned now) { victim_ = victim; at_ = now ? now : 1; open_ = true; held_ = false; }
    // An ack / release arrives from `seat`: believed only from the seat claimed, while the claim is live.
    bool on_ack(int seat, unsigned now) {
        if (!open_ || seat != victim_ || now - at_ > (unsigned)kEatAckWaitFrames) return false;
        held_ = true; return true;
    }
    bool on_release(int seat) {
        if (!held_ || seat != victim_) return false;
        held_ = false; open_ = false; return true;
    }
    void tick(unsigned now) {
        if (open_ && !held_ && now - at_ > (unsigned)kEatAckWaitFrames) open_ = false;
        if (held_ && now - at_ > (unsigned)(kEatMaxFrames + kEatAckWaitFrames)) { held_ = false; open_ = false; }
    }
    bool open() const { return open_; }
    bool holding() const { return held_; }
    int victim() const { return victim_; }
private:
    int victim_ = -1; unsigned at_ = 0; bool open_ = false, held_ = false;
};

// ---- the attacker side: pose -> claim -------------------------------------------------------------
// Player::State indices (port/tools/relay/arena_contract.py STATE_NAMES; the proof cross-checks this table).
enum : int {
    kStYoshiPower = 3, kStSwallow = 4, kStInYoshiMouth = 5, kStHurt = 7, kStHurtWater = 8, kStDeadHit = 12, kStDeadPit = 13,
    kStJump = 18, kStFall = 19, kStSideFlip = 21, kStLongJump = 58, kStBackFlip = 56, kStSlopeJump = 43,
    kStSpin = 47, kStPunchKick = 59, kStGroundPound = 61, kStDive = 62, kStSweepKick = 66, kStSlideKick = 67,
    kStLand = 45, kStNoControl = 24,
};
// Can a body in this state be hit / eaten? (referee.py DISABLED_STATES plus the states no friendly
// contact should interrupt: a level enter, a teleport, a cannon, a conversation, a respawn.)
inline bool state_blocks_hit(int st) {
    switch (st) {
    case 5: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 16: case 20: case 24: case 29:
    case 31: case 32: case 36: case 48: case 51: case 54:
        return true;
    }
    return false;
}

// What the attacker's own pose says it is doing this frame. `frames_in_state` counts follower passes
// since the state was entered; `vy` is Fix12 vertical speed (negative = falling).
struct Pose { int state = 0xFF; int prev_state = 0xFF; unsigned frames_in_state = 0; int vy = 0; };
// The windows are conservative first guesses; tune them on a device (the proof pins the logic only).
enum : unsigned { kMeleeFirst = 2, kMeleeLast = 14, kPoundFirst = 0, kPoundLast = 12, kTongueFirst = 6, kTongueLast = 24 };
inline int attack_for_pose(const Pose &p) {   // returns an IKind, or -1
    switch (p.state) {
    case kStPunchKick: case kStSpin: case kStDive: case kStSweepKick: case kStSlideKick:
        return (p.frames_in_state >= kMeleeFirst && p.frames_in_state <= kMeleeLast) ? (int)kMelee : -1;
    case kStLand:
        // only the landing that follows a ground pound shoves (any other landing is just a landing)
        return (p.prev_state == kStGroundPound && p.frames_in_state <= kPoundLast) ? (int)kPound : -1;
    case kStJump: case kStFall: case kStSideFlip: case kStLongJump: case kStBackFlip: case kStSlopeJump:
        return p.vy < 0 ? (int)kStomp : -1;
    case kStYoshiPower:
        return (p.frames_in_state >= kTongueFirst && p.frames_in_state <= kTongueLast) ? (int)kEatClaim : -1;
    }
    return -1;
}
// Is a friend (at tx,ty,tz) where my attack of `ikind` could have touched him? Looser than the victim's own
// check on purpose: the attacker only decides whether to SAY it; the victim decides.
inline bool attacker_contact(unsigned ikind, int mx, int my, int mz, int myaw, int tx, int ty, int tz) {
    const ReachRule r = reach_rule(ikind);
    const long long dy = (long long)my - ty;        // attacker.y - target.y, as the victim reads it
    const int h = r.horiz - kLagAllow;               // no lag allowance on my own fresh numbers
    if (horiz2(mx, mz, tx, tz) > sq(h) || dy < r.dy_lo || dy > r.dy_hi) return false;
    if (ikind == kMelee || ikind == kEatClaim || ikind == kGrabCatch) {     // in front of me: within +-0x3000 of my facing
        const int want = angle_from_to(mx, mz, tx, tz);
        int d = (want - myaw) & 0xFFFF;
        if (d > 0x8000) d -= 0x10000;
        if (d > 0x3000 || d < -0x3000) return false;
    }
    return true;
}

}  // namespace phit
}  // namespace port

#endif  // PORT_HAL_PARTY_HITS_CORE_H
