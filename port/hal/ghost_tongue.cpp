// POSE9: see ghost_tongue.h. The logic is the VS puppet's V8 block, moved here
// unchanged so the adventure ghost follower (comms_sync.cpp gw_follow) can run
// the same calls.
#include "ghost_tongue.h"
#include "hitch_probe.h"

#include <cstdio>
#include <cstdlib>

extern "C" {
void func_ov002_020d71ec(char *p, int arg);
void _ZN6Player19func_ov002_020d71a0Ev(char *p);
extern unsigned char data_ov002_0210a34c[];
extern unsigned char data_ov002_0210a084[];
extern unsigned char data_ov002_0210eb88[];
extern unsigned data_ov002_0210a6d4[];       // St_Hurt_Init's eight hurt animation ids
}

namespace port {
namespace ghost_tongue {
namespace {
enum : unsigned { kRetractAnim = 0x6c };
unsigned g_id[2] = {0x6e, 0x6f};
}  // namespace

// The body anim ids of the two tongue anims are the first two words of a table
// in the overlay (func_ov002_020d71ec's arr[0..1]); 0x6c is the pull back.
// Ready = the ids make sense and the head files the ROM call reads exist, so
// the call cannot read a null file.
bool ready() {
    static int ok = -1;
    if (ok >= 0) return ok != 0;
    const unsigned *w = (const unsigned *)data_ov002_0210a34c;
    const unsigned *h = (const unsigned *)data_ov002_0210a084;
    ok = 1;
    if (w[0] < 0x11u || w[0] >= 0x72u || w[1] < 0x11u || w[1] >= 0x72u) ok = 0;
    for (int i = 0; ok && i < 2; ++i) {
        const unsigned p = h[i];
        if (!p || !*(const unsigned *)(p + 4)) ok = 0;
    }
    if (!*(const unsigned *)(data_ov002_0210eb88 + 4)) ok = 0;
    if (ok) { g_id[0] = w[0]; g_id[1] = w[1]; }
    std::fprintf(stderr, "[puppet] tongue anims %u %u + %u: %s\n", g_id[0], g_id[1],
                 (unsigned)kRetractAnim, ok ? "head files present" : "NOT USABLE here, tongue off");
    return ok != 0;
}

int arg_for(unsigned anim) {
    if (anim == g_id[0]) return 0;
    if (anim == g_id[1]) return 1;
    if (anim == kRetractAnim) return 2;
    return -1;
}

int select(State &st, char *body, unsigned anim, bool is_yoshi, bool *started) {
    if (started) *started = false;
    int targ = -1;
    if (is_yoshi && ready()) targ = arg_for(anim);
    if (targ != st.arg) {
        if (targ >= 0) {
            HITCH_SCOPE(HK_TONGUE);   // HITCH9C
            func_ov002_020d71ec(body, targ);
            ++st.starts;
            if (started) *started = true;
        } else if (st.arg >= 0) {
            HITCH_SCOPE(HK_TONGUE);
            _ZN6Player19func_ov002_020d71a0Ev(body);
            ++st.stops;
        }
        st.arg = targ;
    }
    return st.arg;
}

int follow(State &st, char *body, int want_f, int *head_frames) {
    if (st.arg < 0) return -1;
    char *hm = *(char **)(body + 0x160);
    if (!hm) return -1;
    const int hframes = (*(int *)(hm + 0x54) & ~(int)0xc0000000) >> 12;
    if (head_frames) *head_frames = hframes;
    if (hframes <= 0) return -1;
    *(int *)(hm + 0x58) = (want_f % hframes) << 12;
    ++st.frames;
    return *(int *)(hm + 0x58) >> 12;
}

unsigned retract_anim() { return kRetractAnim; }

void forget(State &st) { st.arg = -1; }

bool is_hurt_anim(unsigned anim) {
    if (anim < 0x11u || anim >= 0x72u) return false;
    for (int i = 0; i < 8; ++i)
        if (data_ov002_0210a6d4[i] == anim) return true;
    return false;
}

// FRIENDLOOK1 (bug 9): THE BLINK AT THE END OF THE HURT. St_Hurt_Init arms nothing;
// the hurt state's own end does, once its animation has played out (St_Hurt_Main:
// FinishedAnim, then mInvincibleTimer = 0x24 as it changes state, when the hurt
// took health; the land and water versions the same). A friend's hit on me
// (party_hits apply_effect) also arms 0x24 at the START, so a hurt that costs
// health blinks from the knock to 36 frames after it ends. The ghost only blinked
// the first 36 frames from the start, so on a friend's screen it stopped a whole
// knockback early while the player was still invincible on his own screen. Now:
// 36 frames from the start as before, and when the hurt animation ends the count
// is set to 36 again (end_rearm; the caller says false for a hurt known to carry no
// health). The shock (Player::St_Electrocute_Init, animation 0x11) blinks from its
// first frame (0x1e) and again at its end (St_Electrocute_Main, 0x24).
// SM64DS_GHOST_BLINK_AT_START=1 puts back the old start-only count.
namespace {
enum : unsigned { kShockAnim = 0x11 };
bool blink_at_start() {
    static int v = -1;
    if (v < 0) { const char *e = std::getenv("SM64DS_GHOST_BLINK_AT_START"); v = (e && e[0] == '1') ? 1 : 0; }
    return v == 1;
}
}  // namespace

unsigned blink_step(Blink &b, unsigned anim, bool *started, bool end_rearm) {
    if (started) *started = false;
    const bool hurt = is_hurt_anim(anim);
    if (hurt && !b.prev_hurt) {
        b.left = 0x24;
        ++b.starts;
        if (started) *started = true;
    }
    if (!blink_at_start()) {
        const bool shock = anim == kShockAnim, was_shock = b.prev_anim == kShockAnim;
        if (shock && !was_shock) {
            b.left = 0x1e;
            ++b.starts;
            if (started) *started = true;
        }
        if ((b.prev_hurt && !hurt && end_rearm) || (was_shock && !shock)) {
            b.left = 0x24;
            ++b.ends;
            if (started) *started = true;
        }
    }
    b.prev_hurt = hurt;
    b.prev_anim = anim;
    if (b.left <= 0) return 0;
    const unsigned v = (unsigned)b.left--;    // 0x24 down to 1, bit 0 alternates every pass
    ++b.shown;
    return v;
}

void blink_forget(Blink &b) { b.left = 0; b.prev_hurt = false; b.prev_anim = 0xffffffffu; }

}  // namespace ghost_tongue
}  // namespace port
