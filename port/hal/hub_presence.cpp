// PRESENCE: who is where, for party and coop rooms. Run mp1 wave 56, lane
// WIRE1. Layouts and rules: hal/hub_presence.h. Carrier: hal/comms_hub.h.
//
// WHAT IT DOES. Once a frame (port_hub_presence_frame, one hook line in
// tests/walk_window.cpp) it
//   * drains the hub's queues for the kinds that have a handler
//     (comms_hub_pump), so the party and coop lanes' hub_on_kind consumers run
//     on the game thread, and
//   * sends this console's 'SYNL' presence: the level (sublevel id), the area,
//     the act (mission), the character, a few state bits and the star total,
//     at 2 Hz and at once on any change (never faster than every 150 ms, the
//     relay holds 'SYNL' to 6 a second).
// and it keeps the newest 'SYNL' of every other seat for hub_presence_get().
//
// INERT unless the hub is installed in a live non-arena session. A world room
// sends it too (the relay accepts 'SYNL' in a hub); nothing reads it there
// today, and it costs 2 packets a second.
//
// WHERE THE FIELDS COME FROM (see the header comment for the reasoning):
//   level      data_0209f2f8, the sublevel id (SYNG's level_id)
//   area       data_02092120, the area shown now (-1 none)
//   act        data_0209f220[0], the mission latched at stage boot. NOT
//              data_0209f1f0, which is only the pending choice
//   character  the low byte of param1 (+0x08) of this console's body
//   stars      NumStars()
//   flags      in a level: my body exists; menu: Stage::Behavior has not run
//              for 8 frames (the host menu, the ROM pause menu, a loading
//              hold); cutscene: data_0209fc48; room: from the hub
//
// PROBE: SM64DS_PRESENCE_LOG=1 prints one "[presence]" line for every
// datagram sent and every one received that changes anything, with the act
// sources side by side (mission data_0209f220[0] and pending data_0209f1f0),
// so a proof row can read the whole path.

#include "hub_presence.h"
#include "comms_hub.h"
#include "world_party.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern signed char data_0209f2f8;
extern signed char data_02092120;
extern int data_0209f220[];
extern unsigned char data_0209f1f0;
extern int data_0209fc48;
unsigned port_stage_behavior_calls(void);
unsigned char NumStars(void);
}

namespace port {
namespace {

enum : unsigned {
    kSendEveryMs = 500,     // 2 Hz steady
    kMinGapMs    = 150,     // never faster on a change (relay cap 6/s)
    kMenuFrames  = 8,       // frames without a Stage::Behavior call = menu
};

struct Slot {
    bool have;
    unsigned epoch;         // the hub seat's join_epoch when it arrived
    unsigned t;             // comms_hub_now_ms() at arrival
    HubPresence p;
};

SRWLOCK g_lock = SRWLOCK_INIT;
Slot g_slot[kHubMaxSeats];
HubPresence g_mine;
bool g_have_mine = false;          // hub_presence_publish() override
bool g_registered = false;
bool g_sent_have = false;
HubPresence g_sent;
unsigned g_sent_t = 0;
unsigned g_seq = 0;
unsigned g_beh_last = 0, g_beh_still = 0;
bool g_beh_seeded = false;
int g_log = -1;
HubPresenceStats g_st = {};

struct Lock {
    Lock() { ::AcquireSRWLockExclusive(&g_lock); }
    ~Lock() { ::ReleaseSRWLockExclusive(&g_lock); }
};

bool logging() {
    if (g_log < 0) {
        const char *e = std::getenv("SM64DS_PRESENCE_LOG");
        g_log = (e && *e && *e != '0') ? 1 : 0;
    }
    return g_log == 1;
}

// The consumer for 'SYNL': newest by sequence wins (wrap compared), and a
// seat that left and came back starts fresh (the hub's join_epoch).
void on_presence(int seat, const unsigned char *d, int len, void *) {
    HubPresence p;
    if (seat < 0 || seat >= kHubMaxSeats ||
        !hub_presence_decode(d, len, &p)) {
        Lock l;
        ++g_st.drop_bad;
        return;
    }
    p.seat = seat;
    HubSeatInfo si;
    const bool si_ok = comms_hub_seat_info(seat, &si);
    bool changed = false;
    {
        Lock l;
        Slot &s = g_slot[seat];
        const unsigned epoch = si_ok ? si.join_epoch : 0;
        if (s.have && s.epoch == epoch && !hub_seq_newer(p.seq, s.p.seq) &&
            p.seq != s.p.seq) {
            ++g_st.drop_stale;
            return;
        }
        changed = !s.have || s.epoch != epoch || s.p.level != p.level ||
                  s.p.area != p.area || s.p.act != p.act ||
                  s.p.character != p.character || s.p.flags != p.flags ||
                  s.p.stars_total != p.stars_total;
        s.have = true;
        s.epoch = epoch;
        s.t = comms_hub_now_ms();
        s.p = p;
        ++g_st.recvd;
    }
    if (changed && logging())
        std::fprintf(stderr, "[presence] t=%u RECV seat=%d level=%d area=%d "
                     "act=%d char=%d flags=0x%02x stars=%d seq=%u\n",
                     comms_hub_now_ms(), seat, p.level, p.area, p.act,
                     p.character, p.flags, p.stars_total, p.seq);
}

// What this console is, from the ROM's own globals (game thread).
bool sample(HubPresence *out) {
    std::memset(out, 0, sizeof *out);
    out->seat = comms_hub_my_seat();
    out->area = -1;
    const int me = (int)data_0209f250;
    void *a = (me >= 0 && me < 32) ? data_0209f394[me] : nullptr;
    const unsigned beh = port_stage_behavior_calls();
    if (g_beh_seeded && beh == g_beh_last) {
        if (g_beh_still < 1000) ++g_beh_still;
    } else {
        g_beh_still = 0;
    }
    g_beh_last = beh;
    g_beh_seeded = true;

    unsigned flags = 0;
    if (a) {
        flags |= kPresInLevel;
        out->level = (int)(unsigned char)data_0209f2f8;
        out->area = (int)data_02092120;          // -1 = none
        out->act = data_0209f220[0] & 0xff;
        const unsigned c = (unsigned)(*(const int *)((const char *)a + 8) & 0xff);
        out->character = c <= 3 ? (int)c : 0;
    }
    if (g_beh_still >= kMenuFrames) flags |= kPresMenu;
    if (data_0209fc48 != 0) flags |= kPresCutscene;
    const HubRoomKind room = comms_hub_room_kind();
    if (room == kHubRoomParty) flags |= kPresPartyRoom;
    if (room == kHubRoomCoop) flags |= kPresCoopRoom;
    out->flags = flags;
    out->stars_total = (int)NumStars();
    /* hook: PINVITE1, the party word rides the unused clock of a world room's SYNL */
    out->level_clock = room == kHubRoomWorld ? world_party_word() : 0;
    return true;
}

bool differs(const HubPresence &a, const HubPresence &b) {
    return a.level != b.level || a.area != b.area || a.act != b.act ||
           a.character != b.character || a.flags != b.flags ||
           a.stars_total != b.stars_total ||
           (a.level_clock != b.level_clock && comms_hub_room_kind() == kHubRoomWorld);
}

void send_if_due() {
    HubPresence now_p;
    {
        Lock l;
        if (g_have_mine) now_p = g_mine;
        else sample(&now_p);
    }
    const unsigned now = comms_hub_now_ms();
    const bool changed = !g_sent_have || differs(now_p, g_sent);
    if (!changed && (now - g_sent_t) < kSendEveryMs) return;
    if (g_sent_have && (now - g_sent_t) < kMinGapMs) return;
    now_p.seq = ++g_seq & 0xffffu;
    unsigned char buf[kHubPresenceBytes];
    if (hub_presence_encode(now_p, buf) != kHubPresenceBytes) return;
    if (!comms_hub_send(buf, kHubPresenceBytes)) return;
    g_sent = now_p;
    g_sent_have = true;
    g_sent_t = now;
    {
        Lock l;
        ++g_st.sent;
    }
    if (logging())
        std::fprintf(stderr, "[presence] t=%u SEND seat=%d level=%d area=%d "
                     "act=%d (pending data_0209f1f0=%d) char=%d flags=0x%02x "
                     "stars=%d seq=%u\n", now, comms_hub_my_seat(),
                     now_p.level, now_p.area, now_p.act,
                     (int)data_0209f1f0, now_p.character, now_p.flags,
                     now_p.stars_total, now_p.seq);
}

}  // namespace

bool hub_presence_get(int seat, HubPresence *out, unsigned *age_ms) {
    if (seat < 0 || seat >= kHubMaxSeats || !out) return false;
    HubSeatInfo si;
    if (!comms_hub_seat_info(seat, &si) || !si.present) return false;
    Lock l;
    const Slot &s = g_slot[seat];
    if (!s.have || s.epoch != si.join_epoch) return false;
    *out = s.p;
    if (age_ms) *age_ms = comms_hub_now_ms() - s.t;
    return true;
}

void hub_presence_publish(const HubPresence &mine) {
    Lock l;
    g_mine = mine;
    g_have_mine = true;
}

HubPresenceStats hub_presence_stats() {
    Lock l;
    return g_st;
}

}  // namespace port

// SEAMHUB1: the in-place leave (hal/online_join.cpp, after comms_hub_teardown, which also dropped the
// presence handler with every other hub_on_kind registration): back to a process that never sent one.
namespace port {
void hub_presence_leave_reset() {
    Lock l;
    for (int s = 0; s < kHubMaxSeats; ++s) g_slot[s] = Slot();
    g_mine = HubPresence();
    g_have_mine = false;
    g_registered = false;
    g_sent_have = false;
    g_sent = HubPresence();
    g_sent_t = 0;
    g_seq = 0;
    g_beh_last = 0;
    g_beh_still = 0;
    g_beh_seeded = false;
    g_st = HubPresenceStats();
}
}  // namespace port

// THE ONE HOOK LINE (tests/walk_window.cpp, once a frame). Global, no
// namespace, like the other port_* frame calls.
extern "C" void port_hub_presence_frame(void) {
    using namespace port;
    if (!comms_hub_active() || comms_hub_arena() ||
        comms_hub_state() != kHubLive) return;
    if (!g_registered) {
        hub_on_kind(kHubPresence, on_presence, nullptr);
        g_registered = true;
    }
    comms_hub_pump();
    send_if_due();
}

// THE STAR SELECT'S PRESENCE (run coopfix1, lane HITCH1; tests/walk_window.cpp's
// port_interlude_frame, once a star-select frame). The star select runs inside
// one level frame (hal/level_change.cpp's interlude), so the frame hooks above
// do not run while a player picks an act: the friends' presence piled up in the
// hub's 16-deep queue (the "drop queue" count) and this console stopped saying
// where it is, so the friends kept counting it in the course it had just left
// until its presence went stale. Only the presence kind is drained here: its
// consumer only stores the newest record, while the world, boss and save
// consumers act on the level, and the level is torn down during the select
// (they are drained as before once the next level is up). The send says what
// the ROM says: no body, so not in a level.
extern "C" void port_hub_presence_interlude(void) {
    using namespace port;
    static int off = -1;   // SM64DS_STARSEL_PRESENCE=0: the select as 1.0.3 had it (the proof's before arm)
    if (off < 0) { const char *e = std::getenv("SM64DS_STARSEL_PRESENCE"); off = (e && *e == '0') ? 1 : 0; }
    if (off) return;
    if (!comms_hub_active() || comms_hub_arena() ||
        comms_hub_state() != kHubLive) return;
    unsigned char buf[kHubMaxDatagram];
    for (int i = 0; i < 64; ++i) {
        int seat = -1;
        const int n = comms_hub_recv(kHubPresence, buf, (int)sizeof buf, &seat);
        if (n <= 0) break;
        on_presence(seat, buf, n, nullptr);
    }
    send_if_due();
}
