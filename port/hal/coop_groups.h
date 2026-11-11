// COOP LEVEL GROUPS AND OWNERS. Run mp1 wave 56, lane COOPWORLD1.
//
// Pure bookkeeping, no game and no wire in here, so it can be driven by a
// table-top test (and is). The game side is coop_world.cpp.
//
// A group is every console standing in the same (sublevel, act). Each console
// tracks the membership of ITS OWN group from three kinds of evidence: the
// 'SYNL' presence of the other seats (who is in the same level and act), the
// 1 Hz GROUP_HELLO ("I hold this group's state", the seat I believe owns it),
// and OWNER_CHANGE.
//
// OWNER. The first console in the group (the one that found nobody to ask).
// It stays owner until it leaves the group or drops out; then the LOWEST
// remaining established seat takes over and announces it. A console that is
// still loading (not established) is never an owner and never counted as one.
//
// Two standing claims (two consoles that entered in the same breath, a joiner
// whose snapshot timed out, or a console that lost the owner's hello for a few
// seconds and took over) are settled the same way on every console, from the
// hellos alone: the claimant more established consoles name as owner wins,
// counting each console's own belief. On a draw, a claimant that followed the
// other one earlier in this visit gives way to it (so a joiner that lost the
// hellos for a moment hands the group back to the console it took its state
// from); only when neither ever followed the other does the lower seat win.
// A console's own follow record can know more than the others' copies of it (a
// follow not yet heard in a hello). That is still safe: the follower always
// yields to the one it followed, so the worst case is both claimants yielding
// for one hello cycle, after which neither claims, the lowest established seat
// takes the group and the rest follow it. Follow records (mine and the copies)
// are dropped whenever a seat stops being a member of my group, which is also how
// a new player in that seat shows up (the seat leaves, then joins again). Every
// console sees the same hellos, so they all reach the same answer within about
// a hello period; a claimant that loses stops claiming, and whoever followed it
// moves on with it. (Before coopfix1 a late claim never displaced a standing
// owner, but the late claimant never yielded to a higher seat either, so the
// two kept owning the group side by side for the whole visit.) The owner a
// joiner took its state from keeps the joiner while it still claims the group;
// an owner with that joiner behind it outvotes a lone claimant. Everything the
// owner does is idempotent, so a moment of disagreement costs nothing.
//
// Proof knobs (off by default): SM64DS_COOP_OWNER_LEGACY=1 runs the old rule,
// SM64DS_COOP_OWNERBLIND=<start ms>,<length ms> deafens this console to the
// others' hellos for that window after each establish (a lost hello), and
// SM64DS_COOP_OWNERLOG=1 prints the owner once a second.

#ifndef PORT_HAL_COOP_GROUPS_H
#define PORT_HAL_COOP_GROUPS_H

#include <cstdint>

namespace port {

struct CoopKey {
    int level;     // sublevel id, -1 = no group
    int act;
    CoopKey() : level(-1), act(0) {}
    CoopKey(int l, int a) : level(l), act(a) {}
    bool valid() const { return level >= 0; }
    bool operator==(const CoopKey &o) const { return level == o.level && act == o.act; }
    bool operator!=(const CoopKey &o) const { return !(*this == o); }
};

class CoopGroups {
public:
    enum { kSeats = 16, kEvidenceMs = 3500, kTieMs = 2000 };

    void reset(int my_seat);
    int  my_seat() const { return me_; }

    // MY group.
    void enter(const CoopKey &k, unsigned now);       // I changed group; not established
    void leave();                                     // I left every group
    void establish(unsigned now, int owner_hint);     // I hold the state now (hint -1 = none)
    bool established() const { return est_; }
    const CoopKey &key() const { return key_; }

    // Evidence about the others.
    void note_presence(int seat, bool present, const CoopKey &theirs, unsigned now);
    void note_hello(int seat, const CoopKey &k, bool est, int owner_claim, unsigned now);
    void note_owner_change(int seat, const CoopKey &k, unsigned now);

    // Recompute membership and owner. Call once a frame.
    void update(unsigned now);

    int      owner() const { return owner_; }                 // -1 none
    bool     i_am_owner() const { return est_ && owner_ == me_; }
    bool     is_member(int seat) const;                       // present, same key
    bool     is_established(int seat) const;                  // member whose hello says so
    // True only for the owner I believe in (a present member). A seat that merely
    // claims ownership is not trusted until update() makes it the owner. FULL bitmaps trust this.
    bool     owner_claimed_by(int seat) const;
    unsigned member_mask() const;                             // others in my group, bit = seat
    int      others() const;                                  // count of those
    int      established_count() const;                       // established others
    // True once each time I become owner by claim or failover while others are
    // here: the caller sends OWNER_CHANGE.
    bool     take_owner_announce();

private:
    struct Seat {
        bool present;
        CoopKey theirs;      // from presence
        bool hello;          // a hello for my key is on record
        bool est;
        int  claim;          // this seat said it owns the group, else -1
        unsigned hello_ms;
        unsigned claim_ms;   // when this seat's standing claim was first heard
        bool pinned;         // the owner I took my state from: nobody displaces it while it lives
        int  names;          // the owner this seat's last established hello named, else -1
        unsigned followed;   // every owner this seat's established hellos named during this visit, bit = seat
    };
    unsigned followed_ = 0;                   // every owner I followed during this visit, bit = seat
    bool deferred(int a, int b) const;        // claimant a followed b earlier in this visit
    int  votes(int seat) const;               // established consoles (me included) that name this seat owner
    bool beats(int a, int b) const;           // claimant a wins over claimant b
    void update_legacy(unsigned now);         // the rule before coopfix1 (proof knob)
    void set_owner(int o, unsigned now, const char *why);
    int me_ = -1;
    CoopKey key_;
    bool est_ = false;
    int owner_ = -1;
    bool announce_ = false;
    unsigned own_ms_ = 0;     // when I became owner
    unsigned est_ms_ = 0;     // when I established in this group
    unsigned log_ms_ = 0;     // last once-a-second owner line
    bool legacy_ = false;     // proof knob SM64DS_COOP_OWNER_LEGACY
    bool log_ = false;        // proof knob SM64DS_COOP_OWNERLOG
    unsigned blind_at_ = 0, blind_len_ = 0;   // proof knob SM64DS_COOP_OWNERBLIND
    bool blind(unsigned now) const;
    Seat s_[kSeats] = {};
};

}  // namespace port

#endif  // PORT_HAL_COOP_GROUPS_H
