// PARTIES INSIDE THE WORLD. Run mp1, lane PINVITE1.
// What it is, the party word and the keys: hal/world_party.h.

#include "hal/world_party.h"

#include "hal/comms_hub.h"
#include "hal/hub_presence.h"
#include "hal/party.h"
#include "hal/player_list.h"
#include "hal/adv_chat.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern signed char data_0209f2f8;
extern signed char data_02092120;
extern int data_0209f220[];
extern int data_0209fc48;
extern unsigned char data_0209d660[];   // message window
extern unsigned char data_0209f2c4[];   // pause menu
extern signed char data_02092110[];     // < 0 while the level is not being left
}
/* hal/comms_sync.cpp (PAUSEMARK2): the seat is drawn here, fresh, in my level
   and area by its own ghost frame, not leaving, hidden or paused */
extern "C" int port_adventure_seat_invitable(int seat);

namespace port {
namespace {

const unsigned kDefaultRgb[4] = {0xE04040u, 0x40C060u, 0xE0C030u, 0x60A0F0u};
const unsigned kPadUpDflt = 0x0001u, kPadDownDflt = 0x0002u;   // XInput DPAD_UP / DPAD_DOWN

struct State {
    bool env_checked = false;
    bool off = false;
    int  vk_invite = 'B', vk_leave = 'N';          // what the keys resolve to this frame
    unsigned pad_invite = kPadUpDflt, pad_leave = kPadDownDflt;
    // the player's bindings (host_settings, pushed by world_party_set_binds) and
    // whether an env override owns each one (the proofs set those; env wins)
    int  set_vk_invite = 'B', set_vk_leave = 'N';
    unsigned set_pad_invite = kPadUpDflt, set_pad_leave = kPadDownDflt;
    bool env_vk_invite = false, env_vk_leave = false;
    bool env_pad_invite = false, env_pad_leave = false;
    int  log = -1;
    // my side of the word
    int  my_id = 0;
    int  inv_to = -1;
    int  nonce = 0;
    unsigned inv_until = 0;
    bool req_out = false;         // my outgoing inv_to is a REQUEST to join their party (they are in one)
    unsigned cooldown_until = 0;
    // leave confirm
    unsigned confirm_until = 0;
    // an invite for me (the newest unanswered)
    int  in_seat = -1, in_nonce = -1, in_id = 0;
    unsigned in_first_ms = 0, in_until = 0;
    int  dismissed_seat = -1, dismissed_nonce = -1;
    // membership as last pushed into party::set_member
    bool pushed[kHubMaxSeats] = {};
    bool member[kHubMaxSeats] = {};
    // edge detection
    unsigned prev_pad = 0;
    bool prev_inv_key = false, prev_leave_key = false;
    bool primed = false;
    // text
    char toast[kWparLineMax] = {0};
    unsigned toast_until = 0;
    WparStats st = {};
    unsigned rng = 0;
    char name_of[kHubMaxSeats][kWparNameMax + 1] = {};   // last name seen of a member
    unsigned accept_ms = 0;       // when I last accepted (suppresses the join chatter)
    bool auto_accept = false;     // test seam (not in a PORT_ROM_CLEAN build)
    unsigned test_invite_ms = 0;  // test seam: one UP press this long after the party went active (CLOCK1)
    unsigned test_t0 = 0;
    bool test_invite_done = false;
    bool announced = false;
};
State g;

bool logging() {
    if (g.log < 0) {
        const char *e = std::getenv("SM64DS_WPARTY_LOG");
        g.log = (e && *e && *e != '0') ? 1 : 0;
    }
    return g.log == 1;
}

int parse_vk(const char *e, int dflt) {
    if (!e || !*e) return dflt;
    char *end = nullptr;
    const long v = std::strtol(e, &end, 0);
    if (end == e || v < 0 || v > 255) return dflt;
    return (int)v;
}

long parse_mask(const char *e, unsigned dflt) {
    if (!e || !*e) return (long)dflt;
    char *end = nullptr;
    const long v = std::strtol(e, &end, 0);
    if (end == e || v < 0 || v > 0x3ffff) return (long)dflt;
    return v;
}

void check_env() {
    if (g.env_checked) return;
    g.env_checked = true;
    const char *o = std::getenv("SM64DS_WORLD_PARTY");
    if (o && *o == '0') g.off = true;
    const char *ki = std::getenv("SM64DS_PARTY_KEY_INVITE");
    const char *kl = std::getenv("SM64DS_PARTY_KEY_LEAVE");
    const char *pi = std::getenv("SM64DS_PARTY_PAD_INVITE");
    const char *pl = std::getenv("SM64DS_PARTY_PAD_LEAVE");
    g.env_vk_invite = ki && *ki;
    g.env_vk_leave = kl && *kl;
    g.env_pad_invite = pi && *pi;
    g.env_pad_leave = pl && *pl;
    if (g.env_vk_invite) g.vk_invite = parse_vk(ki, 'B');
    if (g.env_vk_leave) g.vk_leave = parse_vk(kl, 'N');
    if (g.env_pad_invite) g.pad_invite = (unsigned)parse_mask(pi, kPadUpDflt);
    if (g.env_pad_leave) g.pad_leave = (unsigned)parse_mask(pl, kPadDownDflt);
#ifndef PORT_ROM_CLEAN
    {   /* TEST ONLY: press UP by itself a moment after an invite prompt shows (the row driver cannot see a prompt) */
        const char *a = std::getenv("SM64DS_WPARTY_TEST_AUTOACCEPT");
        g.auto_accept = a && *a && *a != '0';
        /* TEST ONLY (CLOCK1): SM64DS_WPARTY_TEST_INVITE=<ms> presses UP once, that long after the party went active */
        const char *ti = std::getenv("SM64DS_WPARTY_TEST_INVITE");
        g.test_invite_ms = ti ? (unsigned)std::strtoul(ti, nullptr, 0) : 0u;
    }
#endif
}

unsigned next_rand() {
    if (g.rng == 0) {   /* every console its own id stream: two founders must not pick one id */
        std::random_device rd;
        g.rng = rd() ^ (unsigned)(comms_hub_now_ms() * 2654435761u) ^ ((unsigned)comms_hub_my_seat() << 20);
        if (g.rng == 0) g.rng = 0x9E3779B9u;
    }
    g.rng ^= g.rng << 13;
    g.rng ^= g.rng >> 17;
    g.rng ^= g.rng << 5;
    return g.rng;
}

void say(const char *fmt, const char *a, unsigned now) {
    std::snprintf(g.toast, sizeof g.toast, fmt, a ? a : "");
    g.toast_until = now + kWparToastMs;
    if (logging())
        std::fprintf(stderr, "[wparty] t=%u SAY \"%s\"\n", now, g.toast);
}

void clean_name(const unsigned char *s, int n, char *out) {
    int o = 0;
    for (int i = 0; i < n && o < kWparNameMax; ++i)
        out[o++] = (s[i] >= 0x20 && s[i] <= 0x7e) ? (char)s[i] : '?';
    while (o > 0 && out[o - 1] == ' ') --o;
    out[o] = 0;
}

struct Info {
    bool present = false;
    char name[kWparNameMax + 1] = {0};
    int character = 0;
    unsigned rgb = 0;
    WparWord word = {false, 0, -1, 0};
    bool in_level = false;
    int level = 0, area = -1, act = 0;
    bool have_ghost = false;
    int x = 0, y = 0, z = 0;
    unsigned ghost_age = ~0u;
};

void read_seat(int seat, Info *o) {
    *o = Info();
    HubSeatInfo si;
    if (!comms_hub_seat_info(seat, &si) || !si.present) return;
    o->present = true;
    o->rgb = kDefaultRgb[seat & 3];
    o->character = seat & 3;
    unsigned char id[64];
    const int n = comms_hub_latest(kHubIdent, seat, id, (int)sizeof id, nullptr);
    if (n >= (int)sizeof(HubIdentHead)) {
        HubIdentHead h;
        std::memcpy(&h, id, sizeof h);
        int nl = h.name_len;
        if (nl > kHubIdentNameMax) nl = kHubIdentNameMax;
        if ((int)sizeof h + nl > n) nl = n - (int)sizeof h;
        clean_name(id + sizeof h, nl, o->name);
        o->rgb = ((unsigned)h.body_rgb[0] << 16) | ((unsigned)h.body_rgb[1] << 8) |
                 h.body_rgb[2];
        if (h.character <= 3) o->character = h.character;
    }
    if (!o->name[0]) { *o = Info(); return; }   // still on the menu: no name sent yet, not a player
    HubPresence hp;
    if (hub_presence_get(seat, &hp, nullptr)) {
        o->word = wpar_decode(hp.level_clock);
        o->in_level = hp.in_level();
        o->level = hp.level;
        o->area = hp.area;
        o->act = hp.act;
        if (hp.character >= 0 && hp.character <= 3) o->character = hp.character;
    }
    o->have_ghost = si.has_ghost;
    o->x = si.x; o->y = si.y; o->z = si.z;
    o->ghost_age = si.ghost_ms_ago;
}

bool my_pos(int *x, int *y, int *z) {
    const int me = (int)data_0209f250;
    const char *a = (me >= 0 && me < 16) ? (const char *)data_0209f394[me] : nullptr;
    if (!a) return false;
    *x = *(const int *)(a + 0x5c);
    *y = *(const int *)(a + 0x60);
    *z = *(const int *)(a + 0x64);
    return true;
}

bool world_room() {
    return comms_hub_active() && !comms_hub_arena() &&
           comms_hub_state() == kHubLive &&
           comms_hub_room_kind() == kHubRoomWorld;
}

// The nearest player I could invite: present, with a fresh ghost, in my level and
// area, within range, not already with me. -1 when nobody. Shared by the UP press
// and by the D-pad icon's hint so the icon never promises what UP will refuse.
int nearest_target(const Info info[kHubMaxSeats], int my_seat) {
    int mx, my, mz;
    int best = -1;
    long long best_d2 = 0;
    if (my_pos(&mx, &my, &mz)) {
        const long long lim = (long long)kWparRange * 4096;
        const long long limy = (long long)kWparRangeY * 4096;
        for (int s = 0; s < kHubMaxSeats; ++s) {
            if (s == my_seat || !info[s].present || !info[s].have_ghost) continue;
            if (info[s].ghost_age > 1500u) continue;
            const bool place = info[s].level == (int)(unsigned char)data_0209f2f8 &&
                               info[s].area == (int)data_02092120;
            const long long dx = info[s].x - mx, dz = info[s].z - mz;
            const long long dy = info[s].y - my;
            const long long d2 = dx * dx + dz * dz;
            const bool close_by = !(dy > limy || -dy > limy) && d2 <= lim * lim;
            /* PAUSEMARK2: the place above is the seat's SYNL presence and the
               position its ghost frame, two messages; the seat must also be
               drawn here by its own frame, at full strength and not paused */
            const bool drawn = port_adventure_seat_invitable(s) != 0;
            if (logging() && close_by) {
                static unsigned said[kHubMaxSeats];
                const unsigned key = (place ? 1u : 0u) | (drawn ? 2u : 0u) | 4u |
                                     ((unsigned)(info[s].level & 0xff) << 8) |
                                     ((unsigned)(info[s].area & 0xff) << 16);
                if (said[s] != key) {
                    said[s] = key;
                    std::fprintf(stderr, "[wparty] near seat %d (%s): presence level %d area %d "
                                 "in_level %d (mine %d %d) ghost age %u at (%d,%d,%d) me (%d,%d,%d) "
                                 "place %d drawn %d\n", s, info[s].name, info[s].level, info[s].area,
                                 info[s].in_level ? 1 : 0, (int)(unsigned char)data_0209f2f8,
                                 (int)data_02092120, info[s].ghost_age, info[s].x >> 12,
                                 info[s].y >> 12, info[s].z >> 12, mx >> 12, my >> 12, mz >> 12,
                                 place ? 1 : 0, drawn ? 1 : 0);
                }
            }
            if (!place || !close_by || !drawn) continue;
            if (g.member[s]) continue;               // already with me
            if (best < 0 || d2 < best_d2) { best = s; best_d2 = d2; }
        }
    }
    return best;
}

// TEST ONLY: aim the invite at a chosen seat (the row driver cannot place players).
// A PORT_ROM_CLEAN build has no such seam and returns the search's answer as is.
int test_target(const Info info[kHubMaxSeats], int my_seat, int best) {
#ifndef PORT_ROM_CLEAN
    const char *t = std::getenv("SM64DS_WPARTY_TEST_TARGET");
    const int ts = (t && *t) ? std::atoi(t) : -1;
    if (ts >= 0 && ts < kHubMaxSeats && ts != my_seat && info[ts].present && !g.member[ts]) best = ts;
#else
    (void)info; (void)my_seat;
#endif
    return best;
}

// CLOCK1: the invite cooldown is still running. 0 is "no cooldown": a bare
// (int)(now - 0) < 0 holds once the clock passes 2^31 ms (24.9 days of PC
// uptime), and then every press said "Wait a moment" and Invite was hidden.
bool cooling(unsigned now) {
    return g.cooldown_until != 0 && (int)(now - g.cooldown_until) < 0;
}

int party_size() {
    int n = g.my_id ? 1 : 0;
    for (int s = 0; s < kHubMaxSeats; ++s) n += g.member[s] ? 1 : 0;
    return n;
}

void push_members(const bool want[kHubMaxSeats], unsigned now) {
    for (int s = 0; s < kHubMaxSeats; ++s) {
        g.member[s] = want[s];
        if (g.pushed[s] != want[s]) {
            g.pushed[s] = want[s];
            party::set_member(s, want[s]);
            if (logging())
                std::fprintf(stderr, "[wparty] t=%u member seat=%d %s\n", now, s,
                             want[s] ? "ON" : "off");
        }
    }
}

void drop_all(unsigned now) {
    bool none[kHubMaxSeats] = {};
    push_members(none, now);
    g.my_id = 0; g.inv_to = -1; g.inv_until = 0; g.confirm_until = 0; g.req_out = false;
    g.in_seat = -1;
}

// The pad and keys count only in a level the player is steering: not paused,
// no message box, no cutscene word, not typing, not changing level.
bool input_live() {
    if (data_0209f2f8 <= 0) return false;
    if ((signed char)data_02092110[0] >= 0) return false;
    if (data_0209f2c4[0] || data_0209d660[0]) return false;
    if (data_0209fc48 != 0) return false;
    if (chat_typing()) return false;
    return true;
}

}  // namespace

bool world_party_active() {
    check_env();
    return !g.off && world_room();
}

bool world_party_dpad_reserved() {
    return world_party_active() && input_live();
}

void world_party_set_binds(int vk_invite, int vk_leave, unsigned pad_invite, unsigned pad_leave) {
    check_env();
    g.set_vk_invite = vk_invite; g.set_vk_leave = vk_leave;
    g.set_pad_invite = pad_invite; g.set_pad_leave = pad_leave;
    if (!g.env_vk_invite) g.vk_invite = vk_invite;
    if (!g.env_vk_leave) g.vk_leave = vk_leave;
    if (!g.env_pad_invite) g.pad_invite = pad_invite;
    if (!g.env_pad_leave) g.pad_leave = pad_leave;
}
int world_party_vk_invite() { check_env(); return g.vk_invite; }
int world_party_vk_leave() { check_env(); return g.vk_leave; }

uint32_t world_party_word() {
    if (!world_party_active()) return 0;
    if (!g.my_id && g.inv_to < 0 && g.nonce == 0) return 0;   // never used: stay silent
    return wpar_word(g.my_id, g.inv_to, g.nonce);
}

bool world_party_in_party() { return g.my_id != 0; }
bool world_party_member(int seat) {
    return seat >= 0 && seat < kHubMaxSeats && g.member[seat];
}

unsigned world_party_input(unsigned pad_buttons, bool key_invite, bool key_leave) {
    if (!world_party_active()) {
        if (g.primed || g.my_id) {
            drop_all(comms_hub_now_ms());
            g.primed = false;
        }
        return 0;
    }
    const unsigned now = comms_hub_now_ms();
    const int my_seat = comms_hub_my_seat();
    if (my_seat < 0 || my_seat >= kHubMaxSeats) return 0;
    if (!g.announced && logging()) {
        g.announced = true;
        std::fprintf(stderr, "[wparty] active seat=%d keys invite=0x%x leave=0x%x pad invite=0x%x leave=0x%x\n",
                     my_seat, g.vk_invite, g.vk_leave, g.pad_invite, g.pad_leave);
    }

    // edges (rising only; calling twice in a frame is harmless)
    // a pad mask of 0 is unbound and never reads down
    const unsigned kPadUp = g.pad_invite, kPadDown = g.pad_leave;
    const bool up_now = (pad_buttons & kPadUp) != 0 || key_invite;
    const bool dn_now = (pad_buttons & kPadDown) != 0 || key_leave;
    const bool up_prev = (g.prev_pad & kPadUp) != 0 || g.prev_inv_key;
    const bool dn_prev = (g.prev_pad & kPadDown) != 0 || g.prev_leave_key;
    // raw edges are tracked every frame (a button held through a menu closing
    // is not a fresh press); they count only while the player has control
    const bool live = input_live();
    bool up_edge = live && g.primed && up_now && !up_prev;
#ifndef PORT_ROM_CLEAN
    if (g.auto_accept && g.in_seat >= 0 && (int)(now - g.in_first_ms) > 800) up_edge = true;
    if (g.test_invite_ms && !g.test_invite_done) {
        if (!g.test_t0) g.test_t0 = now | 1u;
        else if (now - g.test_t0 >= g.test_invite_ms) {
            g.test_invite_done = true;
            up_edge = true;
            std::fprintf(stderr, "[wparty] t=%u test: UP pressed (cooldown_until=%u inv_to=%d)\n", now, g.cooldown_until, g.inv_to);
        }
    }
#endif
    const bool dn_edge = live && g.primed && dn_now && !dn_prev;
    unsigned consumed = 0;   // pad bits of a press this call acted on: the caller masks them
    if (up_edge && (pad_buttons & kPadUp)) consumed |= kPadUp;
    if (dn_edge && (pad_buttons & kPadDown)) consumed |= kPadDown;
    g.prev_pad = pad_buttons;
    g.prev_inv_key = key_invite;
    g.prev_leave_key = key_leave;
    g.primed = true;

    // everybody else
    Info info[kHubMaxSeats];
    for (int s = 0; s < kHubMaxSeats; ++s) {
        if (s == my_seat) continue;
        read_seat(s, &info[s]);
    }

    // ---- membership: seats that carry my party id (at most four with me)
    bool want[kHubMaxSeats] = {};
    if (g.my_id) {
        int count = 1;
        for (int s = 0; s < kHubMaxSeats && count < kWparMaxMembers; ++s) {
            if (s == my_seat || !info[s].present) continue;
            if (info[s].word.ok && info[s].word.party_id == g.my_id) {
                want[s] = true;
                ++count;
            }
        }
    }
    // joins and leaves, said once
    for (int s = 0; s < kHubMaxSeats; ++s) {
        if (s == my_seat) continue;
        if (want[s] && !g.member[s]) {
            ++g.st.joined;
            if (g.inv_to == s) { g.inv_to = -1; g.inv_until = 0; g.cooldown_until = now; g.req_out = false; }
            std::memcpy(g.name_of[s], info[s].name, sizeof g.name_of[s]);
            /* the people already in the party I just joined are not news */
            if ((int)(now - g.accept_ms) > 2500 || !g.accept_ms)
                say("%s joined the party", info[s].name, now);
        } else if (!want[s] && g.member[s] && g.my_id) {
            ++g.st.left_by_other;
            say("%s left the party", g.name_of[s][0] ? g.name_of[s] : "A player", now);
        } else if (want[s]) {
            std::memcpy(g.name_of[s], info[s].name, sizeof g.name_of[s]);
        }
    }
    push_members(want, now);

    // an invite that nobody took ends quietly; a party of one dissolves
    if (g.inv_to >= 0 && (int)(now - g.inv_until) >= 0) {
        if (logging())
            std::fprintf(stderr, "[wparty] t=%u invite to seat %d ended (no answer)\n",
                         now, g.inv_to);
        g.inv_to = -1;
        g.req_out = false;
        g.cooldown_until = now + kWparCooldownMs;
    }
    if (g.my_id && g.inv_to < 0 && party_size() <= 1) {
        g.my_id = 0;
        if (logging()) std::fprintf(stderr, "[wparty] t=%u party of one dissolved\n", now);
    }
    if (g.confirm_until && (int)(now - g.confirm_until) >= 0) g.confirm_until = 0;

    // ---- my request was accepted: they answer it with an invite to me carrying their party id
    if (g.req_out && g.inv_to >= 0 && g.inv_to != my_seat && g.inv_to < kHubMaxSeats) {
        const int b = g.inv_to;
        if (info[b].present && info[b].word.ok && info[b].word.inv_to == my_seat &&
            info[b].word.party_id != 0) {
            if (g.my_id && g.my_id != info[b].word.party_id) {
                bool none[kHubMaxSeats] = {};
                push_members(none, now);
            }
            g.my_id = info[b].word.party_id;
            g.inv_to = -1; g.inv_until = 0; g.req_out = false;
            g.accept_ms = now ? now : 1;
            g.cooldown_until = now;
            ++g.st.accepts;
            say("You joined %s's party", info[b].name, now);
            if (logging())
                std::fprintf(stderr, "[wparty] t=%u REQUEST ACCEPTED by seat %d id=%d\n", now, b, g.my_id);
        }
    }

    // ---- an invite for me (or, when I am in a party, a request to join mine)
    int cand = -1;
    for (int s = 0; s < kHubMaxSeats; ++s) {
        if (s == my_seat || !info[s].present || !info[s].word.ok) continue;
        if (info[s].word.inv_to != my_seat) continue;
        if (info[s].word.party_id == 0 && g.my_id == 0) continue;   // an invite needs their party
        if (g.my_id != 0 && info[s].word.party_id == g.my_id) continue;   // already my party
        if (g.req_out && g.inv_to == s) continue;                  // that is the answer to my own request
        if (s == g.dismissed_seat && info[s].word.nonce == g.dismissed_nonce) continue;
        cand = s;
        break;
    }
    if (cand >= 0) {
        if (g.in_seat != cand || g.in_nonce != info[cand].word.nonce ||
            g.in_id != info[cand].word.party_id) {
            g.in_seat = cand;
            g.in_nonce = info[cand].word.nonce;
            g.in_id = info[cand].word.party_id;
            g.in_first_ms = now;
            g.in_until = now + kWparInviteMs;
            if (logging())
                std::fprintf(stderr, "[wparty] t=%u INVITED by seat %d (%s) id=%d\n",
                             now, cand, info[cand].name, g.in_id);
        }
        if ((int)(now - g.in_until) >= 0) {
            g.dismissed_seat = g.in_seat;
            g.dismissed_nonce = g.in_nonce;
            g.in_seat = -1;                                 // timed out: quietly
        }
    } else {
        g.in_seat = -1;
    }

    // ---- UP: accept, else invite
    if (up_edge) {
        g.confirm_until = 0;
        if (g.in_seat >= 0 && g.my_id != 0) {
            /* a REQUEST to join my party: UP lets them in. I answer it with an invite carrying
               my party id; their console sees it and moves them over. */
            if (party_size() >= kWparMaxMembers) {
                say("That party is full", nullptr, now);
            } else {
                g.inv_to = g.in_seat;
                g.req_out = false;
                g.nonce = (g.nonce + 1) & 15;
                g.inv_until = now + kWparInviteMs;
                g.cooldown_until = now;
                ++g.st.accepts;
                if (logging())
                    std::fprintf(stderr, "[wparty] t=%u LET IN seat %d id=%d\n", now, g.in_seat, g.my_id);
            }
            g.dismissed_seat = g.in_seat;
            g.dismissed_nonce = g.in_nonce;
            g.in_seat = -1;
        } else if (g.in_seat >= 0) {
            int size = 0;
            for (int s = 0; s < kHubMaxSeats; ++s)
                if (s != my_seat && info[s].present && info[s].word.ok &&
                    info[s].word.party_id == g.in_id) ++size;
            if (size >= kWparMaxMembers) {
                say("That party is full", nullptr, now);
            } else {
                if (g.my_id) {   /* switching parties: the old one is left without a word about each member */
                    bool none[kHubMaxSeats] = {};
                    push_members(none, now);
                }
                g.my_id = g.in_id;
                g.inv_to = -1;
                g.accept_ms = now ? now : 1;
                ++g.st.accepts;
                say("You joined %s's party", info[g.in_seat].name, now);
                if (logging())
                    std::fprintf(stderr, "[wparty] t=%u ACCEPT seat %d id=%d\n", now,
                                 g.in_seat, g.my_id);
            }
            g.dismissed_seat = g.in_seat;
            g.dismissed_nonce = g.in_nonce;
            g.in_seat = -1;
        } else if (g.inv_to >= 0) {
            say("Invite already sent", nullptr, now);
        } else if (cooling(now)) {
            say("Wait a moment", nullptr, now);
        } else {
            int best = nearest_target(info, my_seat);
            best = test_target(info, my_seat, best);
            if (best < 0) {
                say("Walk up to someone first", nullptr, now);
            } else if (info[best].word.ok && info[best].word.party_id != 0) {
                /* they are in a party: this is a REQUEST from me to join theirs */
                int size = 1;
                for (int s = 0; s < kHubMaxSeats; ++s)
                    if (s != my_seat && s != best && info[s].present && info[s].word.ok &&
                        info[s].word.party_id == info[best].word.party_id) ++size;
                if (size >= kWparMaxMembers) {
                    say("That party is full", nullptr, now);
                } else {
                    g.inv_to = best;
                    g.req_out = true;
                    g.nonce = (g.nonce + 1) & 15;
                    g.inv_until = now + kWparInviteMs;
                    ++g.st.invites;
                    say("Request sent to %s", info[best].name, now);
                    if (logging())
                        std::fprintf(stderr, "[wparty] t=%u REQUEST seat %d (%s) their id=%d\n",
                                     now, best, info[best].name, info[best].word.party_id);
                }
            } else if (party_size() >= kWparMaxMembers) {
                say("The party is full", nullptr, now);
            } else {
                if (!g.my_id) {
                    /* founder seat (high nibble) + a nonce 1..15 (low nibble): two parties
                       founded by different seats can never share an id */
                    g.my_id = ((my_seat & 15) << 4) | (int)(next_rand() % 15u + 1u);
                }
                g.inv_to = best;
                g.nonce = (g.nonce + 1) & 15;
                g.inv_until = now + kWparInviteMs;
                ++g.st.invites;
                say("Invite sent to %s", info[best].name, now);
                if (logging())
                    std::fprintf(stderr, "[wparty] t=%u INVITE seat %d (%s) id=%d nonce=%d\n",
                                 now, best, info[best].name, g.my_id, g.nonce);
            }
        }
    }

    // ---- DOWN: ask, then confirm
    if (dn_edge) {
        if (g.in_seat >= 0 && g.my_id) {
            /* a request to switch parties: DOWN says no, and I stay where I am */
            g.dismissed_seat = g.in_seat;
            g.dismissed_nonce = g.in_nonce;
            g.in_seat = -1;
            g.confirm_until = 0;
            say("Declined", nullptr, now);
            if (logging()) std::fprintf(stderr, "[wparty] t=%u DECLINE request\n", now);
        } else if (!g.my_id && g.inv_to < 0) {
            say("You are not in a party", nullptr, now);
        } else if (g.confirm_until) {
            g.confirm_until = 0;
            drop_all(now);
            g.inv_to = -1;
            g.cooldown_until = now + kWparCooldownMs;
            ++g.st.leaves;
            say("You left the party", nullptr, now);
            if (logging()) std::fprintf(stderr, "[wparty] t=%u LEAVE\n", now);
        } else {
            g.confirm_until = now + kWparConfirmMs;
        }
    } else if (up_edge && g.confirm_until) {
        g.confirm_until = 0;
    }
    return consumed;
}

void world_party_view(WparView *out) {
    std::memset(out, 0, sizeof *out);
    if (!world_party_active()) return;
    const unsigned now = comms_hub_now_ms();
    if (g.confirm_until && (int)(g.confirm_until - now) > 0)
        std::snprintf(out->prompt, sizeof out->prompt, "Leave party? DOWN again");
    else if (g.in_seat >= 0) {
        Info inf;
        read_seat(g.in_seat, &inf);
        std::snprintf(out->prompt, sizeof out->prompt, g.my_id ? "%s requests to join: UP"
                                                               : "%s invites you: UP to join",
                      inf.name);
    }
    if (g.toast[0] && (int)(g.toast_until - now) > 0)
        std::snprintf(out->toast, sizeof out->toast, "%s", g.toast);

    // the members' list: me first, then the others; only while in a party
    if (!g.my_id || data_0209fc48 != 0) return;
    const int my_seat = comms_hub_my_seat();
    if (my_seat < 0 || my_seat >= kHubMaxSeats) return;
    int mx, my, mz;
    if (!my_pos(&mx, &my, &mz)) return;
    int n = 0;
    {
        WparRow &r = out->row[n++];
        std::memset(&r, 0, sizeof r);
        r.seat = my_seat;
        /* SEAMCLEAN1: name and colour from the session record (hal/comms_hub.h), not getenv */
        if (const char *e = online_session_get(kSessPlayerName)) clean_name((const unsigned char *)e, (int)std::strlen(e), r.name);
        if (!r.name[0]) std::snprintf(r.name, sizeof r.name, "PLAYER %d", my_seat + 1);
        r.body_rgb = kDefaultRgb[my_seat & 3];
        if (const char *e = online_session_get(kSessPlayerColor)) {
            char *end = nullptr;
            const unsigned long v = std::strtoul(e, &end, 16);
            if (end && *end == 0 && std::strlen(e) == 6) r.body_rgb = (unsigned)v;
        }
        const int me = (int)data_0209f250;
        const char *a = (me >= 0 && me < 16) ? (const char *)data_0209f394[me] : nullptr;
        r.character = a ? (int)(*(const int *)(a + 8) & 3) : 0;
        r.same_place = true;
        player_list_stage_code((int)(unsigned char)data_0209f2f8, data_0209f220[0] & 0xff, r.code);
    }
    for (int s = 0; s < kHubMaxSeats && n < kWparMaxMembers; ++s) {
        if (s == my_seat || !g.member[s]) continue;
        Info inf;
        read_seat(s, &inf);
        if (!inf.present) continue;
        WparRow &r = out->row[n++];
        std::memset(&r, 0, sizeof r);
        r.seat = s;
        std::memcpy(r.name, inf.name, sizeof r.name);
        r.character = inf.character;
        r.body_rgb = inf.rgb;
        if (inf.in_level) player_list_stage_code(inf.level, inf.act, r.code);
        else std::strcpy(r.code, "--");
        r.same_place = inf.in_level && inf.level == (int)(unsigned char)data_0209f2f8 &&
                       inf.area == (int)data_02092120;
    }
    out->n = n;
}

void world_party_hint(WparHint *out) {
    std::memset(out, 0, sizeof *out);
    if (!world_party_dpad_reserved()) return;
    const int my_seat = comms_hub_my_seat();
    if (my_seat < 0 || my_seat >= kHubMaxSeats) return;
    out->shown = true;
    /* the live player count under the D-pad (Tango 10-28): every seat the hub has present, me included */
    out->online = 1;
    for (int s = 0; s < kHubMaxSeats; ++s) {
        if (s == my_seat) continue;
        HubSeatInfo si;
        if (comms_hub_seat_info(s, &si) && si.present) ++out->online;
    }
    const unsigned now = comms_hub_now_ms();
    const bool confirming = g.confirm_until && (int)(g.confirm_until - now) > 0;
    const bool incoming = g.in_seat >= 0 && (int)(g.in_until - now) > 0;

    // UP
    if (confirming) {
        std::strcpy(out->up, "Cancel");
    } else if (incoming) {
        if (g.my_id == 0 || party_size() < kWparMaxMembers) std::strcpy(out->up, "Accept");
    } else if (g.inv_to < 0 && !cooling(now)) {
        Info info[kHubMaxSeats];
        for (int s = 0; s < kHubMaxSeats; ++s) {
            if (s == my_seat) continue;
            read_seat(s, &info[s]);
        }
        const int best = test_target(info, my_seat, nearest_target(info, my_seat));
        if (best >= 0) {
            if (info[best].word.ok && info[best].word.party_id != 0) {
                int size = 1;
                for (int s = 0; s < kHubMaxSeats; ++s)
                    if (s != my_seat && s != best && info[s].present && info[s].word.ok &&
                        info[s].word.party_id == info[best].word.party_id) ++size;
                if (size < kWparMaxMembers) std::strcpy(out->up, "Join");
            } else if (party_size() < kWparMaxMembers) {
                std::strcpy(out->up, "Invite");
            }
        }
    }

    // DOWN
    if (incoming && g.my_id) std::strcpy(out->down, "Decline");
    else if (!g.my_id && g.inv_to < 0) { /* nothing to leave */ }
    else if (confirming) std::strcpy(out->down, "Confirm");
    else std::strcpy(out->down, "Leave");
}

void world_party_test_reset() { g = State(); }
WparStats world_party_stats() { return g.st; }

}  // namespace port
