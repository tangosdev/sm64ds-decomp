// IN-GAME TEXT CHAT. Run mp1, lane CHATUI1. The contract is in adv_chat.h;
// the wire is design.txt 6.3. This file owns the message rules (three copies,
// dedupe, both rate limits, the cleaner) and the box's text; the window
// (tests/walk_window.cpp) owns the keys and the pixels.

#include "adv_chat.h"
#include "comms_hub.h"
#include "comms_loopback.h"
#include "comms_seam.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" const char *port_vs_slot_name(int slot);   // hal/star_flow.cpp
// run after093 lane MPSOUND1: a friend's sound frame goes here (hal/coop_sound.h). A
// pointer, not a call: this file links into targets that do not take coop_sound.cpp.
extern "C" { void (*port_chat_sound_rx)(int seat, const unsigned char *buf, int n) = 0; }

namespace port {
namespace {

enum : int {
    kHeadBytes    = 12,
    kWireMax      = kHeadBytes + kChatTextMax,   // 132
    kCopies       = 3,
    kPending      = 8,        // messages still sending their copies
    kDedupe       = 64,
    kSendGiveUpMs = 10000,    // no carrier for this long: the line is dropped
    kTestMax      = 16,
};
const unsigned kTagChat = 0x544e5953u;           // 'S','Y','N','T'
// The 6.3 addendum's typing frame: flags bit0, exactly the 12-byte head.
const unsigned char kFlagTyping = 0x01;
const unsigned char kFlagSound = 0x80;    // MPSOUND1: a sound frame (hal/coop_sound.h)
const unsigned kCopyAtMs[kCopies] = {0, 150, 500};

// Sender: 1 message per 1.5 s, burst 3. Receiver: 1 per second per seat,
// burst 3 (so an honest sender's burst is never cut). Token buckets in
// thousandths of a message.
const int kSendRefillMs = 1500, kRecvRefillMs = 1000, kBurst = 3;
// Typing frames: their own bucket per seat, 4 a second, burst 4. An honest
// typer sends about one a second (two state-0 copies on a close), so it is
// never cut; a flood is, and it never touches the chat bucket above.
const int kTypeRefillMs = 250, kTypeBurst = 4;
// The sender's typing schedule: state 1 on open and every second while open,
// the second state-0 copy 150 ms after the first. A sequence that does not move
// forward is a late copy, unless the sender has been quiet this long (a
// restarted console starts a new sequence anywhere).
const unsigned kTypeRefreshMs = 1000, kTypeZeroAgainMs = 150,
               kTypeSeqFreshMs = 5000;

enum Carrier : int { kNone = 0, kVs = 1, kHub = 2 };

unsigned now_ms() {
    static LARGE_INTEGER f = {};
    if (!f.QuadPart) ::QueryPerformanceFrequency(&f);
    LARGE_INTEGER c;
    ::QueryPerformanceCounter(&c);
    return (unsigned)((unsigned long long)c.QuadPart * 1000ull /
                      (unsigned long long)f.QuadPart);
}

struct Bucket {
    int      milli;       // tokens x 1000
    unsigned last;
    bool     init;
};
bool bucket_take(Bucket &b, unsigned now, int refill_ms, int burst = kBurst) {
    if (!b.init) { b.init = true; b.milli = burst * 1000; b.last = now; }
    const unsigned dt = now - b.last;
    b.last = now;
    long long m = (long long)b.milli + (long long)dt * 1000 / refill_ms;
    if (m > burst * 1000) m = burst * 1000;
    b.milli = (int)m;
    if (b.milli < 1000) return false;
    b.milli -= 1000;
    return true;
}

struct Pending {
    bool          used;
    unsigned char buf[kWireMax];
    int           len;
    int           copies;     // sent so far
    unsigned      first_ms;   // when copy 0 left (valid once copies > 0)
    unsigned      queued_ms;
    unsigned short id;
    bool          failed;     // a copy had to wait for the carrier
};

struct Line {
    bool     used;
    char     name[kChatNameMax + 1];
    char     text[kChatTextMax + 1];
    unsigned rgb;
    unsigned until;
};

struct TestSend { int frame; int len; unsigned char text[kChatTextMax * 2]; };

// What one seat shows over its head (lane BUBBLE1). The receiver's side of the
// typing frame and the newest line; this console's own seat uses the line half
// only (its own dots are g.typing).
struct SeatBubble {
    bool           typing;
    unsigned       typing_until;
    bool           have_seq;
    unsigned short last_seq;
    unsigned       last_seq_ms;
    Bucket         type_bucket;
    bool           has_text;
    char           text[kChatTextMax + 1];
    unsigned       text_until;
};

struct State {
    bool     env_read;
    int      carrier;          // Carrier, re-read every tick
    int      said_carrier;     // the carrier the "on" line was printed for
    int      my_seat;
    unsigned tick;
    unsigned short next_id;

    Pending  pend[kPending];
    Bucket   send_bucket;
    Bucket   recv_bucket[kHubMaxSeats];
    unsigned dd_key[kDedupe];  // (seat << 16) | id, 0xFFFFFFFF = empty
    int      dd_head;

    Line     lines[kChatShowLines];
    int      line_head;        // next slot to fill (oldest when all used)

    SeatBubble sb[kHubMaxSeats];
    unsigned short type_seq;
    unsigned type_next_ms;     // next state-1 refresh while the box is open
    bool     type_zero_due;    // the second state-0 copy is still owed
    unsigned type_zero_ms;

    bool     typing;
    unsigned char box[kChatTextMax];
    int      box_len;
    unsigned char dbcs_lead;

    // knobs
    int      drop_pct;
    bool     log_text;
    TestSend tests[kTestMax];
    int      ntests;
    int      type_frame;
    unsigned char type_text[kChatTextMax];
    int      type_len, type_pos;
    int      flood_frame, flood_n;
    int      type_hold;        // frames between the last character and Enter
    bool     type_esc;         // cancel instead of sending
    int      tflood_frame, tflood_n;
    bool     selftest;
    unsigned rng;

    // counters
    unsigned long long sent_msgs, sent_copies, send_fail, send_limited,
        send_given_up, rx_dgrams, rx_shown, rx_dup, rx_bad_len, rx_bad_ver,
        rx_bad_seat, rx_self, rx_rate, rx_test_drop, rx_invalid_utf8,
        rx_ctrl_stripped, rx_unknown_glyph, box_refused, tx_typing,
        tx_typing_fail, rx_typing, rx_typing_bad, rx_typing_rate,
        rx_typing_stale, rx_bad_flags;
    bool     reported;
};
State g;

void report_at_exit() { chat_report("exit"); }

bool env_on(const char *name) {
    const char *e = std::getenv(name);
    return e && *e && *e != '0';
}

int hexv(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// "\xHH" -> one byte, "\\" -> backslash; everything else verbatim.
int unescape(const char *s, const char *end, unsigned char *out, int cap) {
    int n = 0;
    while (s < end && n < cap) {
        if (s[0] == '\\' && end - s >= 2 && s[1] == '\\') {
            out[n++] = '\\';
            s += 2;
            continue;
        }
        if (s[0] == '\\' && end - s >= 4 && s[1] == 'x' &&
            hexv(s[2]) >= 0 && hexv(s[3]) >= 0) {
            out[n++] = (unsigned char)(hexv(s[2]) * 16 + hexv(s[3]));
            s += 4;
            continue;
        }
        out[n++] = (unsigned char)*s++;
    }
    return n;
}

void read_env() {
    if (g.env_read) return;
    g.env_read = true;
    for (int i = 0; i < kDedupe; ++i) g.dd_key[i] = 0xFFFFFFFFu;
    g.rng = (unsigned)::GetCurrentProcessId() * 2654435761u ^ now_ms();
    g.next_id = (unsigned short)(g.rng >> 7);
    g.type_frame = -1;
    g.flood_frame = -1;
    g.tflood_frame = -1;
    g.drop_pct = -1;

    if (const char *e = std::getenv("SM64DS_CHAT_DROP")) {
        int v = std::atoi(e);
        g.drop_pct = v < 0 ? 0 : v > 90 ? 90 : v;
    }
    if (const char *e = std::getenv("SM64DS_CHAT_TEST")) {
        const char *p = e;
        while (*p && g.ntests < kTestMax) {
            const char *semi = std::strchr(p, ';');
            const char *end = semi ? semi : p + std::strlen(p);
            const char *colon = (const char *)std::memchr(p, ':', (size_t)(end - p));
            if (colon) {
                TestSend &t = g.tests[g.ntests];
                t.frame = std::atoi(p);
                t.len = unescape(colon + 1, end, t.text, (int)sizeof t.text);
                if (t.len > 0) ++g.ntests;
            }
            p = semi ? semi + 1 : end;
        }
        std::fprintf(stderr, "[chat] TEST: %d scripted message(s) "
                     "(SM64DS_CHAT_TEST)\n", g.ntests);
    }
    if (const char *e = std::getenv("SM64DS_CHAT_TYPE_TEST")) {
        const char *colon = std::strchr(e, ':');
        if (colon) {
            g.type_frame = std::atoi(e);
            g.type_len = unescape(colon + 1, colon + 1 + std::strlen(colon + 1),
                                  g.type_text, (int)sizeof g.type_text);
            std::fprintf(stderr, "[chat] TEST: typing %d byte(s) into the box "
                         "from frame %d (SM64DS_CHAT_TYPE_TEST)\n",
                         g.type_len, g.type_frame);
        }
    }
    if (const char *e = std::getenv("SM64DS_CHAT_FLOOD")) {
        const char *colon = std::strchr(e, ':');
        if (colon) {
            g.flood_frame = std::atoi(e);
            g.flood_n = std::atoi(colon + 1);
            if (g.flood_n < 0) g.flood_n = 0;
            if (g.flood_n > 200) g.flood_n = 200;
            std::fprintf(stderr, "[chat] TEST: flooding %d message(s) at frame "
                         "%d, sender limit off (SM64DS_CHAT_FLOOD)\n",
                         g.flood_n, g.flood_frame);
        }
    }
    if (const char *e = std::getenv("SM64DS_CHAT_TYPE_HOLD")) {
        g.type_hold = std::atoi(e);
        if (g.type_hold < 0) g.type_hold = 0;
        if (g.type_hold > 100000000) g.type_hold = 100000000;
    }
    g.type_esc = env_on("SM64DS_CHAT_TYPE_ESC");
    if (const char *e = std::getenv("SM64DS_CHAT_TYPING_FLOOD")) {
        const char *colon = std::strchr(e, ':');
        if (colon) {
            g.tflood_frame = std::atoi(e);
            g.tflood_n = std::atoi(colon + 1);
            if (g.tflood_n < 0) g.tflood_n = 0;
            if (g.tflood_n > 200) g.tflood_n = 200;
            std::fprintf(stderr, "[chat] TEST: flooding %d typing frame(s) + 4 "
                         "malformed at frame %d (SM64DS_CHAT_TYPING_FLOOD)\n",
                         g.tflood_n, g.tflood_frame);
        }
    }
    g.selftest = env_on("SM64DS_CHAT_SELFTEST");
    g.log_text = env_on("SM64DS_CHAT_LOG") || g.ntests || g.type_frame >= 0 ||
                 g.flood_frame >= 0 || g.tflood_frame >= 0 || g.selftest;
}

// ---------------------------------------------------------------------------
// THE CARRIER. The hub when it is installed (adventure); otherwise the VS
// carrier when a VS session was opened with an aux-capable transport. Nothing
// else: a solo run has neither and chat stays off.
// ---------------------------------------------------------------------------
int carrier_now(int *seat) {
    if (comms_hub_active()) {
        *seat = comms_hub_my_seat();
        return comms_hub_state() == kHubLive && *seat >= 0 ? kHub : kNone;
    }
    const CommsTransport *t = comms_transport();
    if (t && t->send_aux && comms_session_players() > 0) {
        *seat = t->slot ? t->slot() : 0;
        return kVs;
    }
    *seat = -1;
    return kNone;
}

bool carrier_send(const unsigned char *buf, int len) {
    if (g.carrier == kHub) return comms_hub_send(buf, len);
    if (g.carrier == kVs) {
        const CommsTransport *t = comms_transport();
        return t && t->send_aux && t->send_aux(buf, len) == len;
    }
    return false;
}

int carrier_recv(unsigned char *buf, int cap, int *seat) {
    if (g.carrier == kHub) return comms_hub_recv(kHubChat, buf, cap, seat);
    if (g.carrier == kVs) return comms_recv_kind(kCommsTagChat, buf, cap, seat);
    return 0;
}

// ---------------------------------------------------------------------------
// THE CLEANER: UTF-8 in, printable ASCII the font can draw out.
// ---------------------------------------------------------------------------
struct CleanStats { int invalid, ctrl, unknown; };

// Decode one code point strictly (no overlongs, no surrogates, nothing past
// U+10FFFF). Returns the bytes consumed, or 0 for an invalid sequence (the
// caller then takes one byte as '?').
int utf8_one(const unsigned char *s, int n, unsigned *cp) {
    if (n <= 0) return 0;
    const unsigned c = s[0];
    if (c < 0x80) { *cp = c; return 1; }
    int len; unsigned v, min;
    if ((c & 0xE0) == 0xC0)      { len = 2; v = c & 0x1F; min = 0x80; }
    else if ((c & 0xF0) == 0xE0) { len = 3; v = c & 0x0F; min = 0x800; }
    else if ((c & 0xF8) == 0xF0) { len = 4; v = c & 0x07; min = 0x10000; }
    else return 0;
    if (n < len) return 0;
    for (int i = 1; i < len; ++i) {
        if ((s[i] & 0xC0) != 0x80) return 0;
        v = (v << 6) | (s[i] & 0x3F);
    }
    if (v < min || v > 0x10FFFF || (v >= 0xD800 && v <= 0xDFFF)) return 0;
    *cp = v;
    return len;
}

bool is_control(unsigned cp) {
    if (cp < 0x20 || cp == 0x7F) return true;
    if (cp >= 0x80 && cp <= 0x9F) return true;             // C1
    if (cp >= 0x200B && cp <= 0x200F) return true;         // zero-width, LRM/RLM
    if (cp == 0x2028 || cp == 0x2029) return true;         // line/para separators
    if (cp >= 0x202A && cp <= 0x202E) return true;         // bidi embeddings
    if (cp >= 0x2060 && cp <= 0x2069) return true;         // joiners, bidi isolates
    if (cp == 0xFEFF) return true;                         // BOM
    return false;
}

int clean(const unsigned char *s, int n, char *out, int cap, CleanStats *st) {
    int o = 0, i = 0;
    while (i < n && o < cap - 1) {
        unsigned cp = 0;
        const int k = utf8_one(s + i, n - i, &cp);
        if (k == 0) { out[o++] = '?'; ++i; if (st) ++st->invalid; continue; }
        i += k;
        if (is_control(cp)) { if (st) ++st->ctrl; continue; }
        if (cp < 0x80 && chat_font_code((unsigned char)cp) >= 0)
            out[o++] = (char)cp;
        else { out[o++] = '?'; if (st) ++st->unknown; }
    }
    out[o] = 0;
    return o;
}

// ---------------------------------------------------------------------------
// NAMES AND COLOURS
// ---------------------------------------------------------------------------
const unsigned kPalette[16] = {
    0xFFFF6060u, 0xFF60A0FFu, 0xFF60E060u, 0xFFFFE040u,
    0xFFFF80FFu, 0xFF40E0E0u, 0xFFFFA040u, 0xFFC090FFu,
    0xFFFFFFFFu, 0xFFA0FF80u, 0xFFFF9090u, 0xFF80C0FFu,
    0xFFE0C080u, 0xFF80FFC0u, 0xFFFFC0E0u, 0xFFC0C0C0u,
};

bool rgb6(const char *p, unsigned *out) {
    unsigned v = 0;
    for (int i = 0; i < 6; ++i) {
        const int h = hexv(p[i]);
        if (h < 0) return false;
        v = (v << 4) | (unsigned)h;
    }
    *out = 0xFF000000u | v;
    return true;
}

// Text on a dark shade has to stay readable: a very dark body colour is
// lifted toward white rather than drawn near-black.
unsigned readable(unsigned rgb) {
    const unsigned r = (rgb >> 16) & 0xFF, gg = (rgb >> 8) & 0xFF, b = rgb & 0xFF;
    if (r * 3 + gg * 6 + b < 70 * 10) {
        return 0xFF000000u | (((r + 255) / 2) << 16) | (((gg + 255) / 2) << 8) |
               ((b + 255) / 2);
    }
    return 0xFF000000u | (rgb & 0xFFFFFFu);
}

// SM64DS_VS_COLORS field `seat`, "bbbbbb:ssssss" or empty (hal/fs_mods.cpp is
// the authority on the grammar; this only reads the body half of a field).
bool vs_color(int seat, unsigned *out) {
    const char *e = std::getenv("SM64DS_VS_COLORS");
    if (!e) return false;
    int field = 0;
    const char *p = e;
    while (*p && field < seat) { if (*p == ',') ++field; ++p; }
    if (field != seat) return false;
    const char *end = std::strchr(p, ',');
    const size_t n = end ? (size_t)(end - p) : std::strlen(p);
    if (n != 13 || p[6] != ':') return false;
    return rgb6(p, out);
}

void seat_identity(int seat, char *name, unsigned *rgb) {
    name[0] = 0;
    *rgb = kPalette[seat & 15];
    const bool mine = seat == g.my_seat;
    if (g.carrier == kHub) {
        if (mine) {
            /* SEAMCLEAN1: the name and colour come off the session record (hal/comms_hub.h), not getenv */
            if (const char *e = online_session_get(kSessPlayerName))
                clean((const unsigned char *)e, (int)std::strlen(e), name,
                      kChatNameMax + 1, nullptr);
            unsigned c;
            if (const char *e = online_session_get(kSessPlayerColor))
                if (std::strlen(e) == 6 && rgb6(e, &c)) *rgb = c;
        } else {
            // The newest identity from that seat, peeked (W2 GHOSTWIRE1 is the
            // identity's consumer; comms_hub_latest never takes from its FIFO).
            unsigned char id[64];
            const int n = comms_hub_latest(kHubIdent, seat, id, (int)sizeof id,
                                           nullptr);
            if (n >= (int)sizeof(HubIdentHead)) {
                HubIdentHead h;
                std::memcpy(&h, id, sizeof h);
                int nl = h.name_len;
                if (nl > kHubIdentNameMax) nl = kHubIdentNameMax;
                if ((int)sizeof h + nl > n) nl = n - (int)sizeof h;
                clean(id + sizeof h, nl, name, kChatNameMax + 1, nullptr);
                *rgb = 0xFF000000u | ((unsigned)h.body_rgb[0] << 16) |
                       ((unsigned)h.body_rgb[1] << 8) | h.body_rgb[2];
            }
        }
    } else if (g.carrier == kVs) {
        if (const char *nick = port_vs_slot_name(seat))
            clean((const unsigned char *)nick, (int)std::strlen(nick), name,
                  kChatNameMax + 1, nullptr);
        unsigned c;
        if (vs_color(seat, &c)) *rgb = c;
    }
    if (!name[0]) std::snprintf(name, kChatNameMax + 1, "PLAYER %d", seat + 1);
    *rgb = readable(*rgb);
}

void add_line(int seat, const char *text) {
    Line &l = g.lines[g.line_head];
    g.line_head = (g.line_head + 1) % kChatShowLines;
    l.used = true;
    if (seat >= 0) {
        seat_identity(seat, l.name, &l.rgb);
    } else {
        l.name[0] = 0;                       // a note from this program
        l.rgb = 0xFFC0C0C0u;
    }
    std::snprintf(l.text, sizeof l.text, "%s", text);
    l.until = now_ms() + kChatShowMs;
}

// ---------------------------------------------------------------------------
// THE BUBBLE STATE (lane BUBBLE1)
// ---------------------------------------------------------------------------
void typing_off(int seat, const char *why, unsigned now) {
    SeatBubble &s = g.sb[seat];
    if (!s.typing) return;
    s.typing = false;
    std::fprintf(stderr, "[chat] typing seat=%d off (%s) t=%u\n", seat, why, now);
}

void set_bubble(int seat, const char *shown, unsigned now) {
    if (seat < 0 || seat >= kHubMaxSeats) return;
    SeatBubble &s = g.sb[seat];
    typing_off(seat, "line", now);          // the message ends the typing
    s.has_text = true;
    std::snprintf(s.text, sizeof s.text, "%s", shown);
    s.text_until = now + kChatBubbleMs;
    std::fprintf(stderr, "[chat] bubble seat=%d bytes=%d t=%u\n", seat,
                 (int)std::strlen(s.text), now);
}

void bubble_gone(int seat, unsigned now) {
    SeatBubble &s = g.sb[seat];
    if (!s.has_text) return;
    s.has_text = false;
    std::fprintf(stderr, "[chat] bubble seat=%d gone t=%u\n", seat, now);
}

// Ages every seat's dots and line, and clears a seat that left: in VS the
// round's seat mask (the same on every console; 0 = no information yet), on
// the hub the seat's presence.
void bubbles_age(unsigned now) {
    unsigned live = 0xFFFFFFFFu;
    if (g.carrier == kVs) {
        const unsigned m = port_vs_seat_live_mask();
        if (m) live = m;
    } else if (g.carrier == kHub) {
        live = comms_hub_seat_mask();
    }
    if (g.my_seat >= 0 && g.my_seat < kHubMaxSeats) live |= 1u << g.my_seat;
    for (int seat = 0; seat < kHubMaxSeats; ++seat) {
        SeatBubble &s = g.sb[seat];
        if (!((live >> seat) & 1u)) {
            typing_off(seat, "left", now);
            bubble_gone(seat, now);
            s.have_seq = false;
            continue;
        }
        if (s.typing && (int)(s.typing_until - now) <= 0)
            typing_off(seat, "timeout", now);
        if (s.has_text && (int)(s.text_until - now) <= 0) bubble_gone(seat, now);
    }
}

// flags / text_len / state / length are parameters only so the flood knob can
// build the malformed kinds a hostile sender would; the real path sends
// (kFlagTyping, 0, state, 12).
int build_typing(unsigned char *buf, unsigned short seq, int state,
                 unsigned char flags, unsigned char text_len, int len) {
    HubChatHead h = {};
    h.tag = kTagChat;
    h.version = 1;
    h.seat = 0xFF;
    h.flags = flags;
    h.text_len = text_len;
    h.msg_id = seq;
    h.reserved = (unsigned short)(state & 0xFF);   // byte 10 state, byte 11 0
    std::memset(buf, 0, (size_t)len);
    std::memcpy(buf, &h, sizeof h);
    return len;
}

void send_typing(int state) {
    if (g.carrier == kNone) return;
    unsigned char w[kHeadBytes];
    build_typing(w, g.type_seq++, state, kFlagTyping, 0, kHeadBytes);
    const unsigned now = now_ms();
    if (carrier_send(w, kHeadBytes)) {
        ++g.tx_typing;
        std::fprintf(stderr, "[chat] typing tx state=%d t=%u\n", state, now);
    } else {
        ++g.tx_typing_fail;
    }
}

// The box closed and no message went out (Esc, an empty Enter, a refused
// send): tell the room now and once more 150 ms later.
void typing_closed_quiet() {
    send_typing(0);
    g.type_zero_due = true;
    g.type_zero_ms = now_ms() + kTypeZeroAgainMs;
}

// ---------------------------------------------------------------------------
// SENDING
// ---------------------------------------------------------------------------
int build_wire(unsigned char *buf, unsigned short id, const unsigned char *text,
               int len) {
    HubChatHead h = {};
    h.tag = kTagChat;
    h.version = 1;
    h.seat = 0xFF;                  // the carrier / relay writes the real one
    h.flags = 0;
    h.text_len = (unsigned char)len;
    h.msg_id = id;
    h.reserved = 0;
    std::memcpy(buf, &h, sizeof h);
    std::memcpy(buf + kHeadBytes, text, (size_t)len);
    return kHeadBytes + len;
}

bool queue_message(const unsigned char *text, int len) {
    if (len <= 0) return false;
    if (len > kChatTextMax) len = kChatTextMax;
    const unsigned now = now_ms();
    if (!bucket_take(g.send_bucket, now, kSendRefillMs)) {
        ++g.send_limited;
        add_line(-1, "slow down: one message every 1.5 seconds");
        return false;
    }
    Pending *p = nullptr;
    for (int i = 0; i < kPending && !p; ++i)
        if (!g.pend[i].used) p = &g.pend[i];
    if (!p) { ++g.send_limited; return false; }
    std::memset(p, 0, sizeof *p);
    p->used = true;
    p->id = g.next_id++;
    p->len = build_wire(p->buf, p->id, text, len);
    p->queued_ms = now;
    ++g.sent_msgs;

    char shown[kChatTextMax + 1];
    clean(text, len, shown, (int)sizeof shown, nullptr);
    if (g.log_text)
        std::fprintf(stderr, "[chat] tx id=%u bytes=%d text=\"%s\"\n",
                     (unsigned)p->id, len, shown);
    else
        std::fprintf(stderr, "[chat] tx id=%u bytes=%d\n", (unsigned)p->id, len);
    add_line(g.my_seat, shown);      // the sender sees their own line at once
    set_bubble(g.my_seat, shown, now);  // and over their own head (Tango, Q1)
    return true;
}

void pump_sends() {
    const unsigned now = now_ms();
    for (int i = 0; i < kPending; ++i) {
        Pending &p = g.pend[i];
        if (!p.used) continue;
        if (p.copies == 0 && now - p.queued_ms > (unsigned)kSendGiveUpMs) {
            p.used = false;
            ++g.send_given_up;
            add_line(-1, "message not sent: no connection");
            continue;
        }
        while (p.used && p.copies < kCopies &&
               (p.copies == 0 || now - p.first_ms >= kCopyAtMs[p.copies])) {
            if (!carrier_send(p.buf, p.len)) {
                // counted once per message, not once per frame it waits
                if (!p.failed) { p.failed = true; ++g.send_fail; }
                break;
            }
            if (p.copies == 0) p.first_ms = now;
            ++p.copies;
            ++g.sent_copies;
        }
        if (p.copies >= kCopies) p.used = false;
    }
}

// ---------------------------------------------------------------------------
// RECEIVING
// ---------------------------------------------------------------------------
bool dedupe_seen(unsigned key) {
    for (int i = 0; i < kDedupe; ++i)
        if (g.dd_key[i] == key) return true;
    g.dd_key[g.dd_head] = key;
    g.dd_head = (g.dd_head + 1) % kDedupe;
    return false;
}

unsigned rnd() {
    g.rng = g.rng * 1664525u + 1013904223u;
    return g.rng >> 8;
}

void pump_recv() {
    unsigned char buf[kHubMaxDatagram];
    int seat = -1;
    int n;
    int budget = 64;                     // a frame never spins on a flood
    while (budget-- > 0 &&
           (n = carrier_recv(buf, (int)sizeof buf, &seat)) > 0) {
        ++g.rx_dgrams;
        if (g.drop_pct > 0 && (int)(rnd() % 100u) < g.drop_pct) {
            ++g.rx_test_drop;
            continue;
        }
        if (n < kHeadBytes || n > kWireMax) { ++g.rx_bad_len; continue; }
        HubChatHead h;
        std::memcpy(&h, buf, sizeof h);
        if (h.tag != kTagChat || h.version != 1) { ++g.rx_bad_ver; continue; }
        if (h.flags != 0) {
            // THE TYPING FRAME (6.3 addendum). Only bit0 exists; it is exactly
            // the 12-byte head with text_len 0, state 0 or 1 in byte 10 and 0
            // in byte 11. Anything else is dropped and counted.
            // MPSOUND1: flags 0x80 is a friend's sound frame (hal/coop_sound.h); a build
            // without that file leaves the hook null and counts it here as before
            if (h.flags == kFlagSound && port_chat_sound_rx) { port_chat_sound_rx(seat, buf, n); continue; }
            if (h.flags != kFlagTyping) { ++g.rx_bad_flags; continue; }
            if (n != kHeadBytes || h.text_len != 0 || buf[10] > 1 || buf[11] != 0) {
                ++g.rx_typing_bad;
                continue;
            }
            if (seat < 0 || seat >= kHubMaxSeats) { ++g.rx_bad_seat; continue; }
            if (seat == g.my_seat) { ++g.rx_self; continue; }
            const unsigned now = now_ms();
            SeatBubble &s = g.sb[seat];
            if (!bucket_take(s.type_bucket, now, kTypeRefillMs, kTypeBurst)) {
                ++g.rx_typing_rate;
                continue;
            }
            if (s.have_seq && (short)(h.msg_id - s.last_seq) <= 0 &&
                now - s.last_seq_ms < kTypeSeqFreshMs) {
                ++g.rx_typing_stale;               // a late copy never undoes
                continue;                          // a newer state
            }
            s.have_seq = true;
            s.last_seq = h.msg_id;
            s.last_seq_ms = now;
            ++g.rx_typing;
            if (buf[10]) {
                if (!s.typing)
                    std::fprintf(stderr, "[chat] typing seat=%d on t=%u\n", seat, now);
                s.typing = true;
                s.typing_until = now + kChatTypingMs;
            } else {
                typing_off(seat, "closed", now);
            }
            continue;
        }
        if (h.text_len < 1 || h.text_len > kChatTextMax ||
            kHeadBytes + (int)h.text_len != n) {
            ++g.rx_bad_len;
            continue;
        }
        if (seat < 0 || seat >= kHubMaxSeats) { ++g.rx_bad_seat; continue; }
        if (seat == g.my_seat) { ++g.rx_self; continue; }
        if (dedupe_seen(((unsigned)seat << 16) | h.msg_id)) {
            ++g.rx_dup;
            continue;
        }
        if (!bucket_take(g.recv_bucket[seat], now_ms(), kRecvRefillMs)) {
            ++g.rx_rate;
            continue;
        }
        char shown[kChatTextMax + 1];
        CleanStats st = {};
        clean(buf + kHeadBytes, h.text_len, shown, (int)sizeof shown, &st);
        g.rx_invalid_utf8 += (unsigned)st.invalid;
        g.rx_ctrl_stripped += (unsigned)st.ctrl;
        g.rx_unknown_glyph += (unsigned)st.unknown;
        ++g.rx_shown;
        if (g.log_text)
            std::fprintf(stderr, "[chat] rx seat=%d id=%u bytes=%d text=\"%s\"\n",
                         seat, (unsigned)h.msg_id, (int)h.text_len, shown);
        else
            std::fprintf(stderr, "[chat] rx seat=%d id=%u bytes=%d\n", seat,
                         (unsigned)h.msg_id, (int)h.text_len);
        add_line(seat, shown);
        set_bubble(seat, shown, now_ms());
    }
}

// ---------------------------------------------------------------------------
// THE BOX
// ---------------------------------------------------------------------------
void box_send() {
    const bool sent = g.box_len > 0 && queue_message(g.box, g.box_len);
    g.box_len = 0;
    g.typing = false;
    if (!sent) typing_closed_quiet();    // a sent line ends the dots by itself
}

void box_append_cp(unsigned cp) {
    if (is_control(cp)) return;
    unsigned char u[4];
    int k;
    if (cp < 0x80) { u[0] = (unsigned char)cp; k = 1; }
    else if (cp < 0x800) {
        u[0] = (unsigned char)(0xC0 | (cp >> 6));
        u[1] = (unsigned char)(0x80 | (cp & 0x3F)); k = 2;
    } else if (cp < 0x10000) {
        if (cp >= 0xD800 && cp <= 0xDFFF) return;
        u[0] = (unsigned char)(0xE0 | (cp >> 12));
        u[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        u[2] = (unsigned char)(0x80 | (cp & 0x3F)); k = 3;
    } else return;
    if (g.box_len + k > kChatTextMax) { ++g.box_refused; return; }
    std::memcpy(g.box + g.box_len, u, (size_t)k);
    g.box_len += k;
}

// ---------------------------------------------------------------------------
// THE CLEANER'S UNIT CASES (SM64DS_CHAT_SELFTEST)
// ---------------------------------------------------------------------------
void run_selftest() {
    struct Case { const char *in; int len; const char *want; };
    const Case cases[] = {
        {"Hello 42", 8, "Hello 42"},
        {"a\x07" "b\x1b" "c\x7f" "d", 7, "abcd"},             // C0 + DEL stripped
        {"\xc3\xa9t\xc3\xa9", 5, "?t?"},                       // U+00E9 has no glyph
        {"x\xff" "y", 3, "x?y"},                               // invalid byte
        {"\xc0\xaf", 2, "??"},                                 // overlong '/'
        {"\xed\xa0\x80" "z", 4, "???z"},                       // a surrogate
        {"\xe2\x80\xae" "ab", 5, "ab"},                        // RLO stripped
        {"\xc2\x85" "q", 3, "q"},                              // C1 NEL stripped
        {"\xe2\x80\xa8" "r", 4, "r"},                          // U+2028 stripped
        {"\xf0\x9f\x98\x80", 4, "?"},                          // emoji: one '?'
        {"\xe3\x81", 2, "??"},                                 // truncated
        {"ok, it's 5:30 - go! 100% ?", 26, "ok, it's 5:30 - go! 100% ?"},
        {"a<b>=(c)", 8, "a?b???c?"},                           // no glyphs
    };
    int bad = 0;
    for (const Case &c : cases) {
        char out[64];
        clean((const unsigned char *)c.in, c.len, out, (int)sizeof out, nullptr);
        if (std::strcmp(out, c.want) != 0) {
            ++bad;
            std::fprintf(stderr, "[chat] selftest FAIL: \"%s\" wanted \"%s\"\n",
                         out, c.want);
        }
    }
    std::fprintf(stderr, "[chat] selftest %s: %d of %d cleaner cases\n",
                 bad ? "FAIL" : "PASS", (int)(sizeof cases / sizeof cases[0]) - bad,
                 (int)(sizeof cases / sizeof cases[0]));
}

}  // namespace

// ---------------------------------------------------------------------------
// THE FONT TABLE. The ROM message font's character codes. Letters, digits and
// space are tests/nametag.h's ranges (digits 0x00..0x09, A..Z 0x0a..0x23, a..z
// 0x2d..0x46, space 0x4d). The punctuation below was READ OFF THE FONT SHEET
// ITSELF (file 0x980e = c2d.narc member 14, decoded with Message::AddChar's
// tile walk), each glyph checked for shape AND height in its 8x16 cell: '.'
// sits on the baseline and ',' hangs below it, the apostrophe is high, the
// hyphen mid-height. Only ASCII characters with an unmistakable glyph are
// mapped. The sheet has no ( ) [ ] { } < > = + * # @ $ _ ^ ` | \ glyph (0x56
// is a dagger, 0x5b a guillemet), so those clean to '?'.
// ---------------------------------------------------------------------------
int chat_font_code(unsigned char a) {
    if (a >= '0' && a <= '9') return a - '0';
    if (a >= 'A' && a <= 'Z') return 0x0a + (a - 'A');
    if (a >= 'a' && a <= 'z') return 0x2d + (a - 'a');
    switch (a) {
    case ' ':  return 0x4d;
    case '?':  return 0x26;
    case '!':  return 0x27;
    case '~':  return 0x28;
    case ',':  return 0x29;
    case '"':  return 0x2b;      // the closing double quote, cap height
    case '-':  return 0x47;
    case '.':  return 0x48;
    case '\'': return 0x49;
    case ':':  return 0x4a;
    case ';':  return 0x4b;
    case '&':  return 0x4c;
    case '/':  return 0x4e;
    case '%':  return 0x59;
    default:   return -1;
    }
}

void chat_tick() {
    // SOLO: no hub and no VS transport, so there is nobody to talk to and
    // nothing here runs (not even the env read).
    if (!comms_hub_active() && !comms_transport()) return;
    read_env();
    ++g.tick;
    const int f = (int)g.tick;
    int seat = -1;
    g.carrier = carrier_now(&seat);
    if (g.carrier == kNone) {
        if (g.typing) { g.typing = false; g.box_len = 0; }
        return;
    }
    g.my_seat = seat;
    if (g.said_carrier != g.carrier) {
        g.said_carrier = g.carrier;
        std::fprintf(stderr, "[chat] on: carrier=%s seat=%d\n",
                     g.carrier == kHub ? "hub" : "vs", seat);
        if (g.drop_pct < 0) {
            g.drop_pct = 0;
            if (g.carrier == kVs)
                if (const char *e = std::getenv("SM64DS_COMMS_DROP")) {
                    const int v = std::atoi(e);
                    g.drop_pct = v < 0 ? 0 : v > 90 ? 90 : v;
                }
        }
        if (g.drop_pct > 0)
            std::fprintf(stderr, "[chat] TEST: dropping %d%% of received chat "
                         "datagrams\n", g.drop_pct);
        if (g.selftest) run_selftest();
        std::atexit(report_at_exit);
    }

    // Scripted sends fire on the first frame at or past their own while chat
    // is up (a hub that pairs late still sends them).
    for (int i = 0; i < g.ntests; ++i)
        if (g.tests[i].frame >= 0 && f >= g.tests[i].frame) {
            queue_message(g.tests[i].text, g.tests[i].len);
            g.tests[i].frame = -1;
        }
    if (g.flood_frame >= 0 && f >= g.flood_frame) {
        // one copy each, straight to the carrier: the sender's own limiter and
        // the three-copy schedule are what a flooder would leave out
        for (int i = 0; i < g.flood_n; ++i) {
            char t[32];
            const int n = std::snprintf(t, sizeof t, "flood %d", i);
            unsigned char w[kWireMax];
            const int len = build_wire(w, g.next_id++,
                                       (const unsigned char *)t, n);
            if (carrier_send(w, len)) { ++g.sent_msgs; ++g.sent_copies; }
            else ++g.send_fail;
        }
        std::fprintf(stderr, "[chat] TEST: flood of %d sent\n", g.flood_n);
        g.flood_frame = -1;
    }
    if (g.type_frame >= 0 && f >= g.type_frame) {
        if (g.type_pos == 0) g.type_pos = f;      // the frame the box opens
        const int step = f - g.type_pos;
        if (step == 0) {
            chat_open();
            std::fprintf(stderr, "[chat] TEST: box open at frame %d\n", f);
        }
        else if (step <= g.type_len) {
            const unsigned char b = g.type_text[step - 1];
            if (b < 0x80) box_append_cp(b);
            else if (g.box_len < kChatTextMax) g.box[g.box_len++] = b;  // raw
        } else if (step >= g.type_len + 1 + g.type_hold) {
            std::fprintf(stderr, "[chat] TEST: %s at frame %d\n",
                         g.type_esc ? "Esc" : "Enter", f);
            chat_key(g.type_esc ? VK_ESCAPE : VK_RETURN);
            g.type_frame = -1;
        }
    }
    if (g.tflood_frame >= 0 && f >= g.tflood_frame) {
        // straight to the carrier, like SM64DS_CHAT_FLOOD: the receivers' typing
        // cap and checks are what is under test. Then one of each malformed
        // kind: a flag bit that does not exist, a 13-byte typing frame, text_len
        // on a typing frame, a state that does not exist.
        unsigned char w[kHeadBytes + 1];
        int sent = 0;
        for (int i = 0; i < g.tflood_n; ++i) {
            build_typing(w, g.type_seq++, 1, kFlagTyping, 0, kHeadBytes);
            if (carrier_send(w, kHeadBytes)) ++sent;
        }
        build_typing(w, g.type_seq++, 1, 0x02, 0, kHeadBytes);
        if (carrier_send(w, kHeadBytes)) ++sent;
        build_typing(w, g.type_seq++, 1, kFlagTyping, 0, kHeadBytes + 1);
        if (carrier_send(w, kHeadBytes + 1)) ++sent;
        build_typing(w, g.type_seq++, 1, kFlagTyping, 5, kHeadBytes);
        if (carrier_send(w, kHeadBytes)) ++sent;
        build_typing(w, g.type_seq++, 7, kFlagTyping, 0, kHeadBytes);
        if (carrier_send(w, kHeadBytes)) ++sent;
        std::fprintf(stderr, "[chat] TEST: typing flood of %d + 4 malformed, %d "
                     "left this console\n", g.tflood_n, sent);
        g.tflood_frame = -1;
    }

    // THE TYPING FRAMES this console owes: the refresh while the box is open,
    // and the second state-0 copy after a quiet close.
    const unsigned now = now_ms();
    if (g.typing && (int)(now - g.type_next_ms) >= 0) {
        send_typing(1);
        g.type_next_ms = now + kTypeRefreshMs;
    }
    if (g.type_zero_due && (int)(now - g.type_zero_ms) >= 0) {
        g.type_zero_due = false;
        if (!g.typing) send_typing(0);
    }

    pump_sends();
    pump_recv();
    bubbles_age(now_ms());
}

bool chat_available() {
    int seat;
    return carrier_now(&seat) != kNone;
}

bool chat_typing() { return g.typing; }

bool chat_open() {
    if (!chat_available()) return false;
    const bool was = g.typing;
    g.typing = true;
    g.box_len = 0;
    g.dbcs_lead = 0;
    if (!was) {
        // the room sees the dots from now (and every second while it stays open)
        g.type_zero_due = false;
        send_typing(1);
        g.type_next_ms = now_ms() + kTypeRefreshMs;
    }
    return true;
}

void chat_key(int vk) {
    if (!g.typing) return;
    if (vk == VK_RETURN) { box_send(); return; }
    if (vk == VK_ESCAPE) {
        g.typing = false;
        g.box_len = 0;
        typing_closed_quiet();
        return;
    }
    if (vk == VK_BACK) {
        // one whole UTF-8 character
        while (g.box_len > 0) {
            const unsigned char b = g.box[--g.box_len];
            if ((b & 0xC0) != 0x80) break;
        }
    }
}

void chat_char_ansi(unsigned char b) {
    if (!g.typing) return;
    char mb[2];
    int n = 0;
    if (g.dbcs_lead) { mb[0] = (char)g.dbcs_lead; mb[1] = (char)b; n = 2; g.dbcs_lead = 0; }
    else if (::IsDBCSLeadByte(b)) { g.dbcs_lead = b; return; }
    else { mb[0] = (char)b; n = 1; }
    wchar_t w[2] = {};
    const int k = ::MultiByteToWideChar(CP_ACP, 0, mb, n, w, 2);
    if (k == 1) box_append_cp((unsigned)w[0]);
}

void chat_view(ChatView *out) {
    std::memset(out, 0, sizeof *out);
    const unsigned now = now_ms();
    for (int i = 0; i < kChatShowLines; ++i) {
        const Line &l = g.lines[(g.line_head + i) % kChatShowLines];
        if (!l.used || (int)(l.until - now) <= 0) continue;
        ChatViewLine &v = out->line[out->n++];
        std::memcpy(v.name, l.name, sizeof v.name);
        std::memcpy(v.text, l.text, sizeof v.text);
        v.rgb = l.rgb;
        const int left = (int)(l.until - now);
        v.alpha = left >= 1000 ? 255 : left * 255 / 1000;
    }
    out->open = g.typing;
    if (g.typing) {
        clean(g.box, g.box_len, out->box, (int)sizeof out->box, nullptr);
        out->box_bytes = g.box_len;
        out->caret = ((now / 500u) & 1u) == 0;
    }
}

bool chat_bubble(int seat, ChatBubble *out) {
    std::memset(out, 0, sizeof *out);
    if (seat < 0 || seat >= kHubMaxSeats || g.carrier == kNone) return false;
    const unsigned now = now_ms();
    const SeatBubble &s = g.sb[seat];
    out->typing = seat == g.my_seat ? g.typing
                                    : s.typing && (int)(s.typing_until - now) > 0;
    if (s.has_text && (int)(s.text_until - now) > 0) {
        out->has_text = true;
        std::memcpy(out->text, s.text, sizeof out->text);
        const int left = (int)(s.text_until - now);
        out->alpha = left >= 1000 ? 255 : left * 255 / 1000;
    }
    // dots win over an older line: that seat is typing again
    if (out->typing) out->has_text = false;
    if (!out->typing && !out->has_text) return false;
    out->dots = 1 + (int)((now / 400u) % 3u);
    char name[kChatNameMax + 1];
    seat_identity(seat, name, &out->rgb);
    return true;
}

int chat_my_seat() { return g.carrier == kNone ? -1 : g.my_seat; }

// A note from this program in the overlay (gray, no name): hal/party_stars.cpp
// says which friend's star just landed. Printable ASCII is passed through the
// same cleaner as chat text. Works whether or not a chat carrier is up.
void chat_system_line(const char *text) {
    if (!text) return;
    char shown[kChatTextMax + 1];
    clean((const unsigned char *)text, (int)std::strlen(text), shown,
          (int)sizeof shown, nullptr);
    add_line(-1, shown);
}

void chat_report(const char *tag) {
    if (!g.said_carrier) return;
    std::fprintf(stderr,
                 "[chat] report %s: carrier=%s seat=%d sent=%llu copies=%llu "
                 "send_fail=%llu limited=%llu given_up=%llu rx=%llu shown=%llu "
                 "dup=%llu rate=%llu bad_len=%llu bad_ver=%llu bad_seat=%llu "
                 "self=%llu test_drop=%llu invalid_utf8=%llu ctrl=%llu "
                 "unknown_glyph=%llu box_refused=%llu typing_tx=%llu "
                 "typing_tx_fail=%llu typing_rx=%llu typing_bad=%llu "
                 "typing_rate=%llu typing_stale=%llu bad_flags=%llu\n",
                 tag ? tag : "-", g.said_carrier == kHub ? "hub" : "vs",
                 g.my_seat, g.sent_msgs, g.sent_copies, g.send_fail,
                 g.send_limited, g.send_given_up, g.rx_dgrams, g.rx_shown,
                 g.rx_dup, g.rx_rate, g.rx_bad_len, g.rx_bad_ver, g.rx_bad_seat,
                 g.rx_self, g.rx_test_drop, g.rx_invalid_utf8,
                 g.rx_ctrl_stripped, g.rx_unknown_glyph, g.box_refused,
                 g.tx_typing, g.tx_typing_fail, g.rx_typing, g.rx_typing_bad,
                 g.rx_typing_rate, g.rx_typing_stale, g.rx_bad_flags);
    std::fflush(stderr);
}

}  // namespace port

// The C name hal/party_stars.cpp links to (resolved there through /alternatename
// until this exists).
extern "C" void port_chat_system_line(const char *text) {
    port::chat_system_line(text);
}
