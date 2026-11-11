// See coop_groups.h. Pure logic; no game, no wire.

#include "coop_groups.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace port {

namespace {
long long owner_wall_ms() {   // proof lines: one clock for every console on this machine
    return (long long)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}
bool env_on(const char *name) {
    const char *v = std::getenv(name);
    return v && *v && *v != '0';
}
}  // namespace

void CoopGroups::reset(int my_seat) {
    legacy_ = env_on("SM64DS_COOP_OWNER_LEGACY");
    log_ = env_on("SM64DS_COOP_OWNERLOG");
    blind_at_ = blind_len_ = 0;
    if (const char *b = std::getenv("SM64DS_COOP_OWNERBLIND")) {
        unsigned a = 0, l = 0;
        if (std::sscanf(b, "%u,%u", &a, &l) == 2) { blind_at_ = a; blind_len_ = l; }
    }
    est_ms_ = 0;
    log_ms_ = 0;
    me_ = my_seat;
    key_ = CoopKey();
    est_ = false;
    owner_ = -1;
    announce_ = false;
    own_ms_ = 0;
    std::memset(s_, 0, sizeof s_);
    for (int i = 0; i < kSeats; ++i) { s_[i].claim = -1; s_[i].names = -1; }
}

bool CoopGroups::blind(unsigned now) const {
    if (!blind_len_ || !est_) return false;
    const unsigned t = now - est_ms_;
    return t >= blind_at_ && t - blind_at_ < blind_len_;
}

void CoopGroups::set_owner(int o, unsigned now, const char *why) {
    const int was = owner_;
    owner_ = o;
    if (o == was) return;
    if (o == me_) own_ms_ = now;
    if (o == me_ && others() > 0) announce_ = true;
    std::fprintf(stderr, "[coopowner] seat=%d owner %d -> %d (%s) votes=%d members=%u wall=%lld\n",
                 me_, was, o, why, o >= 0 ? votes(o) : 0, member_mask(), owner_wall_ms());
}

void CoopGroups::enter(const CoopKey &k, unsigned) {
    key_ = k;
    est_ = false;
    owner_ = -1;
    announce_ = false;
    // Any hello on record was for the group I came from.
    for (int i = 0; i < kSeats; ++i) {
        s_[i].hello = false;
        s_[i].est = false;
        s_[i].claim = -1;
        s_[i].pinned = false;
        s_[i].names = -1;
    }
}

void CoopGroups::leave() {
    enter(CoopKey(), 0);
}

void CoopGroups::establish(unsigned now, int owner_hint) {
    if (!key_.valid()) return;
    est_ = true;
    est_ms_ = now;
    owner_ = -1;
    if (owner_hint >= 0 && owner_hint < kSeats && owner_hint != me_ &&
        is_member(owner_hint)) {
        owner_ = owner_hint;
        s_[owner_hint].hello = true;
        s_[owner_hint].est = true;
        s_[owner_hint].claim = owner_hint;
        s_[owner_hint].claim_ms = now;
        s_[owner_hint].pinned = true;
        s_[owner_hint].names = owner_hint;
        s_[owner_hint].hello_ms = now;
    }
    // owner_ stays -1 without a hint: update() picks me when I am alone.
}

void CoopGroups::note_presence(int seat, bool present, const CoopKey &theirs,
                               unsigned) {
    if (seat < 0 || seat >= kSeats || seat == me_) return;
    Seat &s = s_[seat];
    s.present = present;
    s.theirs = present ? theirs : CoopKey();
    if (!present || s.theirs != key_) {
        s.hello = false;
        s.est = false;
        s.claim = -1;
        s.pinned = false;
        s.names = -1;
    }
}

void CoopGroups::note_hello(int seat, const CoopKey &k, bool est,
                            int owner_claim, unsigned now) {
    if (seat < 0 || seat >= kSeats || seat == me_ || k != key_) return;
    if (blind(now)) return;   // proof knob: a hello lost on the way
    Seat &s = s_[seat];
    s.hello = true;
    s.est = est;
    s.names = (est && owner_claim >= 0 && owner_claim < kSeats) ? owner_claim : -1;
    const int was = s.claim;
    s.claim = (est && owner_claim == seat) ? seat : -1;   // only a seat's claim about itself counts
    if (s.claim == seat && was != seat) s.claim_ms = now;
    if (s.claim != seat) s.pinned = false;
    s.hello_ms = now;
}

void CoopGroups::note_owner_change(int seat, const CoopKey &k, unsigned now) {
    if (seat < 0 || seat >= kSeats || seat == me_ || k != key_) return;
    if (blind(now)) return;   // proof knob
    Seat &s = s_[seat];
    s.hello = true;
    s.names = seat;
    if (s.claim != seat) s.claim_ms = now;
    s.est = true;
    s.claim = seat;
    s.hello_ms = now;
}

bool CoopGroups::is_member(int seat) const {
    if (seat < 0 || seat >= kSeats || seat == me_ || !key_.valid()) return false;
    return s_[seat].present && s_[seat].theirs == key_;
}

bool CoopGroups::is_established(int seat) const {
    return is_member(seat) && s_[seat].hello && s_[seat].est;
}

bool CoopGroups::owner_claimed_by(int seat) const {
    if (!est_ || seat < 0 || seat >= kSeats || seat == me_) return false;
    return seat == owner_ && is_member(seat);   // a claim only counts once update() has made it the owner
}

unsigned CoopGroups::member_mask() const {
    unsigned m = 0;
    for (int i = 0; i < kSeats; ++i)
        if (is_member(i)) m |= 1u << i;
    return m;
}

int CoopGroups::others() const {
    int n = 0;
    for (int i = 0; i < kSeats; ++i) n += is_member(i) ? 1 : 0;
    return n;
}

int CoopGroups::established_count() const {
    int n = 0;
    for (int i = 0; i < kSeats; ++i) n += is_established(i) ? 1 : 0;
    return n;
}

int CoopGroups::votes(int seat) const {
    if (seat < 0 || seat >= kSeats) return 0;
    int n = (est_ && owner_ == seat) ? 1 : 0;   // my own belief
    for (int j = 0; j < kSeats; ++j)
        if (is_established(j) && s_[j].names == seat) ++n;
    return n;
}

bool CoopGroups::beats(int a, int b) const {
    if (b < 0) return a >= 0;
    if (a < 0) return false;
    const int va = votes(a), vb = votes(b);
    if (va != vb) return va > vb;
    return a < b;
}

void CoopGroups::update(unsigned now) {
    if (!key_.valid()) return;
    // Hello evidence goes stale: an established member that stopped saying so.
    for (int i = 0; i < kSeats; ++i) {
        Seat &s = s_[i];
        if (s.hello && (unsigned)(now - s.hello_ms) > (unsigned)kEvidenceMs) {
            s.hello = false;
            s.est = false;
            s.claim = -1;
            s.pinned = false;
            s.names = -1;
        }
    }
    if (log_ && est_ && (unsigned)(now - log_ms_) >= 1000u) {
        log_ms_ = now;
        std::fprintf(stderr, "[coopowner] seat=%d owner=%d est=1 members=%u claims=", me_, owner_, member_mask());
        for (int i = 0; i < kSeats; ++i)
            if (is_established(i) && s_[i].claim == i) std::fprintf(stderr, "%d,", i);
        std::fprintf(stderr, " wall=%lld\n", owner_wall_ms());
    }
    if (legacy_) { update_legacy(now); return; }
    if (!est_) { owner_ = -1; return; }

    // The owner I believe in, still a candidate? A member owner must still claim the group: one that
    // yielded to another claimant names that one in its hello, and I follow it there.
    const bool owner_ok = (owner_ == me_) ||
                          (owner_ >= 0 && is_established(owner_) && s_[owner_].claim == owner_);
    if (owner_ok) {
        // The owner I took my state from keeps me while it still claims the group.
        if (owner_ != me_ && s_[owner_].pinned) return;
        // Every other standing claim is weighed the same way on every console: the one more of us
        // name as owner wins, a draw goes to the lower seat.
        int best = owner_;
        for (int i = 0; i < kSeats; ++i)
            if (i != best && is_established(i) && s_[i].claim == i && beats(i, best)) best = i;
        if (best != owner_) set_owner(best, now, owner_ == me_ ? "yield" : "follow");
        return;
    }

    // Nobody, or a gone owner: the best standing claim first, else the lowest
    // established seat, counting myself.
    int pick = -1;
    for (int i = 0; i < kSeats; ++i)
        if (is_established(i) && s_[i].claim == i && beats(i, pick)) pick = i;
    if (pick < 0) {
        pick = me_;
        for (int i = 0; i < me_; ++i)
            if (is_established(i)) { pick = i; break; }
    }
    set_owner(pick, now, pick == me_ ? "take" : "pick");
}

// The rule before coopfix1, kept for the before/after proof (SM64DS_COOP_OWNER_LEGACY=1).
void CoopGroups::update_legacy(unsigned now) {
    if (!est_) { owner_ = -1; return; }

    // The owner I believe in, still a candidate?
    const bool owner_ok = (owner_ == me_) ||
                          (owner_ >= 0 && is_established(owner_));
    if (owner_ok) {
        // Two claimants that showed up together: the lower seat keeps it. A
        // claimant that turns up later than that never displaces a standing owner.
        if (owner_ != me_ && s_[owner_].pinned) return;
        const unsigned inc = (owner_ == me_) ? own_ms_ : s_[owner_].claim_ms;
        for (int i = 0; i < owner_; ++i) {
            if (!is_established(i) || s_[i].claim != i) continue;
            if ((int)(s_[i].claim_ms - inc) > (int)kTieMs) continue;
            // An owner that already has an established member besides the challenger is not in a tie any more:
            // two consoles that entered together are each alone until they see each other's claim.
            if (owner_ == me_) {
                bool served = false;
                for (int j = 0; j < kSeats; ++j) if (j != i && is_established(j)) served = true;
                if (served) continue;
            }
            set_owner(i, now, "legacy tie");
            break;
        }
        return;
    }

    // Nobody, or a gone owner: a standing claim first, else the lowest
    // established seat, counting myself.
    int pick = -1;
    for (int i = 0; i < kSeats; ++i)
        if (is_established(i) && s_[i].claim == i) { pick = i; break; }
    if (pick < 0) {
        pick = me_;
        for (int i = 0; i < me_; ++i)
            if (is_established(i)) { pick = i; break; }
    }
    set_owner(pick, now, "legacy pick");
}

bool CoopGroups::take_owner_announce() {
    const bool a = announce_;
    announce_ = false;
    return a;
}

}  // namespace port
