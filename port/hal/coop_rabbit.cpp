// COOP RABBIT HOLD: a rabbit a friend holds is held on my screen too, and a rabbit a friend caught is caught here too.
// Run coopfix1, lane RABBIT2 (Tango's play-test bugs 2, 6 and 8). See coop_rabbit.h.
//
// What went wrong (castle grounds, Mario holding the rabbit in its talk, Luigi watching):
//   1. The holder stops publishing a carried actor (coop_actors.cpp skip_state: +0x45c set), so after kStaleMs his records
//      went stale on the friend's console, the friend's coop sync adopted the rabbit as its own (decide_owner: "the owner
//      went silent"), and party_hits' held pin then left it alone as "MINE" (held_after_follow_impl mode 2). From then on
//      the friend's copy ran its own Behavior: a free rabbit hopping around while Mario held it, and the friend PUBLISHED
//      it, which the holder threw away (bug 2: guest cls 187 pub=3189, owner_ch=4; host applied 0).
//   2. Nothing told the friend's copy that it was caught. The holder's rabbit ends its catch in the ROM's Released state
//      (daMip_c::StateCaughtMain: DropActor, SetState(Released); StateReleasedInit sets unk_426 = 1, the tame mark), while
//      the friend's copy stayed in its flee / idle states and coop sync pinned it to the holder's still rabbit: a rabbit
//      hopping on the spot for ever (bug 6).
//   3. Because the friend's copy was never tame, the friend could catch it again on his own screen and the ROM gave HIS
//      character's key for it (daMip_c::UpdateGrab sends a tame rabbit to Released, a wild one to Caught; Caught asks
//      func_02013890(rabbit, catcher's character), a bit the first catch never set) (bug 8).
//
// The fix, in the ROM's own terms, on a friend's copy only (the holder's console runs the cartridge's flow untouched):
//   * while a friend's hands hold the rabbit (party_hits kHeldState, kind 1), my copy sits in the ROM's Caught state with
//     the hold animation (daMip_c::StateCaughtInit), its talker the friend's ghost body and its talk sequence closed, so
//     its Behavior neither moves it, nor runs its grab check, nor offers a message; party_hits pins it to his drawn hands
//     and its draw matrix is rebuilt right after that pin, so this frame draws it there;
//   * coop sync keeps the holder as the owner for the whole hold (coop_rabbit_owner below), so nobody adopts a held rabbit;
//   * when the hold ends, my copy goes to the ROM's Released state exactly as the holder's did (unk_426 = 1, tame): it
//     stands, can be talked to, and a grab gives Released again, never a key;
//   * the castle grounds key rabbit (rabbit 7, the one rabbit every character's console has; the others spawn only for their
//     own character) is also made tame when the room's save already has its key (flags1 0x40) and my copy still runs wild,
//     which covers a friend who came in after the hold or lost its messages.
//
// Solo play: nothing here runs outside a coop room (coop_world_active), so a single player game is the cartridge's.
// Mixed rooms: no new message. A 1.0.3 friend sends the same kHeldState, so my copy follows his hold; a 1.0.3 friend
// watching me hold keeps 1.0.3's behaviour on his screen.
// Knobs: SM64DS_COOP_RABBIT_HOLD=0 switches all of it off (the before row); SM64DS_COOP_RABBIT2_LOG=1 logs each change
// and a [coopr2] line per rabbit every 120 frames.
// PORT_HOST_ABI: raw offsets into daMip_c are the header's named members; nothing in src/ changes.

#include "coop_rabbit.h"
#include "coop_wire.h"
#include "coop_world.h"
#include "comms_hub.h"
#include "party_hits.h"

#include "daMip_c.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;       // my player slot
extern unsigned char data_0209f21c;       // number of player bodies
extern int data_0209caa0[];               // the live save record: [1] flags1 (0x40 = rabbit 7's key was taken)
void *_ZN8dActor_c4NextEPKS_(const void *prev);
int port_adventure_ghost_seat(int slot);  // the hub seat a ghost body in player slot `slot` shows (-1: none)
// the rabbit's state records (ov085 .bss; C names from portsync3_flat_aliases.cpp)
extern char data_ov085_021306ac;          // Caught
extern char data_ov085_021306bc;          // Released
}

namespace port {
namespace {

enum : int { kClsMip = 187, kSlots = 512, kSeats = 4 };

bool g_env = false, g_on = true, g_log = false;
unsigned g_frame = 0;
struct Track { const char *a = nullptr; int seat = -1; bool mirrored = false; };
Track g_t[kSlots];
unsigned g_mirrors = 0, g_releases = 0, g_rebuilds = 0, g_both = 0;

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_RABBIT_HOLD");
    g_on = !(e && *e == '0');
    const char *l = std::getenv("SM64DS_COOP_RABBIT2_LOG");
    g_log = l && *l && *l != '0';
    if (!g_on) std::fprintf(stderr, "[coopr2] OFF (SM64DS_COOP_RABBIT_HOLD=0): a friend's held / caught rabbit runs free here\n");
}

bool is_body(const void *a) {
    for (int i = 0; i < 16; ++i) if (data_0209f394[i] == a) return true;
    return false;
}
char *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}
char *ghost_body(int seat) {
    const int nb = (int)data_0209f21c < 16 ? (int)data_0209f21c : 16;
    char *me = my_body();
    for (int i = 0; i < nb; ++i)
        if (port_adventure_ghost_seat(i) == seat && data_0209f394[i] && data_0209f394[i] != me) return (char *)data_0209f394[i];
    return nullptr;
}
bool in_state(const daMip_c *m, const void *rec) { return (const void *)(size_t)(unsigned)m->mState == rec; }
const char *state_name(const daMip_c *m) {
    if (in_state(m, &data_ov085_021306ac)) return "caught";
    if (in_state(m, &data_ov085_021306bc)) return "released";
    return "free";
}

// A friend's hands hold this rabbit: my copy is caught, held by his ghost body, and quiet.
void mirror_hold(int slot, daMip_c *m, char *gb, int seat) {
    Track &t = g_t[slot];
    if (!in_state(m, &data_ov085_021306ac)) {
        m->mHorzSpeed = 0;
        m->SetState(&data_ov085_021306ac);              // StateCaughtInit: step 0, the hold animation, the small cylinder
        ++g_mirrors;
        std::fprintf(stderr, "[coopr2] f=%u slot %d: seat %d holds the rabbit, my copy is caught in his hands (was %s, tame %d)\n",
                     g_frame, slot, seat, t.mirrored ? "caught" : "free", (int)m->unk_426);
    }
    m->mTalkingPlayer = (Player *)gb;                   // StateCaughtMain needs a talker; the carry flag (+0xb0 0x4000) stays clear
    m->mTalkState = 2;                                  // so neither StateCaughtMain nor Behavior's talk block opens a message here
    m->mdCcAc_c.flags |= 2;                             // what UpdateGrab sets on a real catch
    t.mirrored = true; t.seat = seat;
}

// The friend's hold ended: the holder's rabbit is in Released now (after the key, or a tame one let go). Mine follows.
void mirror_release(int slot, daMip_c *m) {
    Track &t = g_t[slot];
    m->mTalkingPlayer = 0;
    m->mTalkState = 0;
    m->mHorzSpeed = 0;
    m->mdCcAc_c.flags &= ~2u;
    m->SetState(&data_ov085_021306bc);                  // StateReleasedInit: unk_426 = 1 (tame), gravity, the wait animation
    ++g_releases;
    std::fprintf(stderr, "[coopr2] f=%u slot %d: seat %d let the rabbit go, my copy is released and tame (unk_426 %d) at %d,%d,%d\n",
                 g_frame, slot, t.seat, (int)m->unk_426,  // (seat -1: the room's key flag, no hold seen here) *(const int *)((const char *)m + 0x5c) >> 12, *(const int *)((const char *)m + 0x60) >> 12, *(const int *)((const char *)m + 0x64) >> 12);
    t.mirrored = false; t.seat = -1;
}

}  // namespace

int coop_rabbit_owner(int slot, const char *a) {
    read_env();
    if (!g_on || !a || slot < 0 || slot >= kSlots || *(const unsigned short *)(a + 0x0c) != kClsMip) return -1;
    const char *me = my_body();
    if (me && *(const char *const *)(a + 0x45c) == me) return comms_hub_my_seat();     // my hands: I own it for the whole hold
    const int s = party_hits::held_by(kClsMip, slot);
    return (s >= 0 && s < kSeats && s != comms_hub_my_seat()) ? s : -1;
}

void coop_rabbit_after_follow() {
    read_env();
    if (!g_on || !coop_world_active()) return;
    ++g_frame;
    const int my = comms_hub_my_seat();
    char *me = my_body();
    bool seen[kSlots]; std::memset(seen, 0, sizeof seen);
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != kClsMip || is_body(a)) continue;
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kSlots) continue;
        if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) continue;   // dying / deleted
        Track &t = g_t[slot];
        if (t.a != a) t = Track(), t.a = a;
        seen[slot] = true;
        daMip_c *m = (daMip_c *)a;
        const int seat = party_hits::held_by(kClsMip, slot);
        const bool mine = me && (char *)m->mTalkingPlayer == me;
        if (seat >= 0 && seat != my) {
            if (mine) {                                     // we both grabbed it on our own screens at once: mine runs the ROM
                if (++g_both <= 8) std::fprintf(stderr, "[coopr2] f=%u slot %d: seat %d and I both hold the rabbit, mine stays mine\n", g_frame, slot, seat);
                continue;
            }
            char *gb = ghost_body(seat);
            if (!gb) continue;                              // his ghost is not drawn here: nothing to hold it
            mirror_hold(slot, m, gb, seat);
            m->UpdateMatrixAndShadow();                     // party_hits pinned it to his hands after my Behavior drew it: draw it there
            ++g_rebuilds;
        } else if (t.mirrored) {
            mirror_release(slot, m);
        } else if (m->mRabbitId == 7 && (data_0209caa0[1] & 0x40) && !m->mTalkingPlayer && !m->unk_426 &&
                   !in_state(m, &data_ov085_021306ac) && !in_state(m, &data_ov085_021306bc)) {
            // The castle grounds key rabbit (rabbit 7, the only rabbit every character's console has) whose key the room
            // already has (flags1 0x40, set by its key and merged into every console's save): a copy still running wild here
            // missed the hold (I came in later, or the hold's messages were lost). On the catcher's screen it stands tame;
            // here too, so no second key comes out of it. (A reloaded level never spawns it: InitResources checks 0x40.)
            t.seat = -1;
            std::fprintf(stderr, "[coopr2] f=%u slot %d: the room already has rabbit 7's key, my wild copy is made tame\n", g_frame, slot);
            mirror_release(slot, m);
        }
        if (g_log && (g_frame % 120u) == 0)
            std::fprintf(stderr, "[coopr2] f=%u slot %d id %d char %d state %s tame %d held_by %d talker %s at %d,%d,%d\n", g_frame, slot,
                         m->mRabbitId, m->mCharacterId, state_name(m), (int)m->unk_426, seat,
                         !m->mTalkingPlayer ? "none" : (char *)m->mTalkingPlayer == me ? "me" : "friend", *(const int *)(a + 0x5c) >> 12,
                         *(const int *)(a + 0x60) >> 12, *(const int *)(a + 0x64) >> 12);
    }
    for (int i = 0; i < kSlots; ++i) if (!seen[i] && g_t[i].a) g_t[i] = Track();
    if (g_log && (g_frame % 600u) == 0 && (g_mirrors || g_releases))
        std::fprintf(stderr, "[coopr2] f=%u summary: held mirrors %u, releases %u, draws at the hands %u, both-held %u\n", g_frame, g_mirrors,
                     g_releases, g_rebuilds, g_both);
}

}  // namespace port
