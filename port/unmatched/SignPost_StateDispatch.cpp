/* HOST COPIES of src/func_ov002_020bbd5c.cpp and src/func_ov002_020bbda4.cpp
 * -- the sign's own five-state machine, with the mwcc pointer-to-member pairs
 * read as plain function pointers and seated with host addresses.
 *
 * data_ov002_0210e084 is five {Init, Main} PMF pairs, written by
 * __sinit_ov002_02101738 out of ten statics at ov002 0x02109a64..0x02109ab4.
 * All ten are the nonvirtual { function, 0 } form:
 *
 *     0  planted   init 020bba24 (empty)   main 020bb9fc  talk/grab checks
 *     1  read      init 020bb9f0           main 020bb614  UNMATCHED
 *     2  carried   init 020bbd50           main 020bbcb8
 *     3  thrown    init 020bbc78           main 020bbb14
 *     4  dropped   init 020bbac8           main 020bba28
 *
 * Two host-only steps, the same pair OneUpMushroom_Behavior.cpp needs.
 *
 * MSVC forms a pointer-to-member of an INCOMPLETE class as the most general
 * representation (four words, not one), which would stride the table at 0x20
 * instead of 0x10 and dispatch a neighbour's Init as a Main. Both source TUs
 * declare `struct C;` before the typedef, so both hit it.
 *
 * And the words the sinit copied are the ov002 image's own -- DS CODE
 * ADDRESSES, which is the ovdata contract. The seat below rewrites each with
 * its host body after checking the stored word against the ROM address that
 * body was compiled from, so a mount pointing at the wrong bytes says so
 * instead of calling into the overlay image.
 *
 * STATE 1'S MAIN IS A LOGIC-VERIFIED NEAR-MISS HOST COPY (gate 181). ov002
 * 0x020bb614 (the read loop, 0x3dc) sat unmatched; a div=7 attempt proved a
 * hard ordering floor -- the entire body 0x44..0x3dc is byte-identical to the
 * ROM and all seven divergences are one prologue regalloc/schedule
 * permutation (nearmiss/db.jsonl carries the floor evidence: the
 * conditional-cast shift-pair split vs load-hoist collision). Semantics are
 * exact, so the body below is the near-miss C, the cannon-lid precedent.
 * It walks the player to the sign, turns both, then hands off to
 * Player::ShowMessage2. Replace with matched src the day the floor breaks.
 *
 * THE OLD NOTE HERE SAID ShowMessage2 IS DECLINED AND THAT A SIGN READ
 * "APPROACHES AND IDLES". THAT HAS BEEN FALSE SINCE 2026-08-08: the decline
 * was removed, the matched src is back in slice_gate10, and hal/message_pump.cpp
 * ticks Message::UpdateWindow + Message::Update every frame, so the box really
 * opens. Traced live (SM64DS_SIGN_TRIGGER=1 SM64DS_TRACE_SIGN=1, level 3): the
 * read state runs sub 0 turn for 15 frames, sub 1 walk, sub 2 face for 15
 * frames, then ShowMessage2 fires and the box reaches state 7 and stays there
 * until dismissed. Anyone debugging the reported sign soft-lock should not
 * start from "the message is declined", because it is not.
 *
 * SUB-STATE 1 HAS NO TIMEOUT AND NO ABORT. It drives the PLAYER'S POSITION
 * (Vec3_ApproachHorz on player+0x5c, 0xa000 per frame toward a point 0x78000
 * in front of the sign), and Vec3_ApproachHorz writes x/z DIRECTLY and is
 * collision-blind -- its arrival test is horizontal length only. So the walk
 * can only fail to arrive if something writes the player's position back
 * between sign ticks.
 *
 * MEASURED, AND IT DOES NOT -- but read the rig before trusting the number.
 *
 * FIRST ATTEMPT, WHICH DOES NOT COUNT: eight approach directions at radius 700
 * around the level-3 sign, walk arrived every time, achieved step always the
 * full 0xa000. That measurement was taken on a player who never leaves
 * St_LevelEnter (0x020c7838) on level 3: speed 0 every frame, no movement
 * state, full stick ignored. He could not have contended for his position
 * under any conditions, so "nothing contests the write" was true of a case
 * incapable of contesting it. A rig that cannot fail the test does not pass it.
 *
 * REDONE WHERE THE PLAYER REALLY MOVES (level 1, SM64DS_SELFTEST_DASH, speed
 * 131072 = 32 units/frame in the frame before entry): ENTERING THE TALK ZEROES
 * HIS SPEED. spd goes 131072 -> 0 on the entry frame and stays 0 for the whole
 * walk, and the achieved step is the full 0xa000 every frame. So residual
 * momentum cannot fight the walk either: the talk takes his velocity before
 * sub 1 starts. Making the no-timeout loop fire needs a third party that moves
 * him WITHOUT going through his speed (a moving platform, a moving collider),
 * not momentum and not terrain.
 *
 * THE REPORTED SOFT-LOCK IS THEREFORE STILL UNEXPLAINED, and specifically the
 * ROTATION is: sub 0 turns for 15 frames and converges, sub 2 turns for 15 and
 * converges, and sub 0 cannot chase an unstable angle because the read point
 * is 0x78000 (120 units) out and the proximity escape fires at 0x32000 (200),
 * so the player can never be near enough to that point to destabilise
 * Vec3_HorzAngle while still in sub 0. Three hypotheses tested, all negative.
 * Do not treat this machine as the known cause.
 *
 * STILL UNTESTED, and it is what the third reporter actually described
 * ("quickly trying to read"): RE-ENTRANCY. Triggering the read repeatedly, or
 * during the transition, or cancelling and re-entering. The probe in
 * hal/input_probe.cpp latches on `entered` and fires once, so it cannot
 * produce that case; testing it needs a rig that can re-arm.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

void func_ov002_020bba24(void *);   /* 0 Init  */
void func_ov002_020bb9fc(void *);   /* 0 Main  */
void func_ov002_020bb9f0(void *);   /* 1 Init  */
void func_ov002_020bbd50(void *);   /* 2 Init  */
void func_ov002_020bbcb8(void *);   /* 2 Main  */
void func_ov002_020bbc78(void *);   /* 3 Init  */
void func_ov002_020bbb14(void *);   /* 3 Main  */
void func_ov002_020bbac8(void *);   /* 4 Init  */
void func_ov002_020bba28(void *);   /* 4 Main  */

struct PortStatePair { unsigned fn; int delta; };
struct PortSignPostState { PortStatePair init, main_; };
extern PortSignPostState data_ov002_0210e084[];

}  /* extern "C" */

enum { PORT_SIGNPOST_STATES = 5 };

/* ---- what the read-state main closes over (all linked; the two 020bec
   helpers join slice_gate16) ---- */
extern "C" {
extern short data_02082214[];
extern unsigned char data_0209d660, data_0209d6bc, data_0209f284;
int _ZN6Player12GetTalkStateEv(void *player);
int Vec3_HorzDist(void *a, void *b);
short Vec3_HorzAngle(void *a, void *b);
int _Z14ApproachLinearRsss(short *val, short target, short step);
int Vec3_ApproachHorz(void *pos, void *target, int step);
int func_ov002_020bec84(void *player, unsigned int i);
int func_ov002_020bec9c(void *player, unsigned int a, int b, int d, unsigned short e);
int _ZN6Player12FinishedAnimEv(void *player);
void _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(
    void *player, void *actor, unsigned int msg, void *pos, unsigned int a,
    unsigned int b);
void func_02012790(int id);
void func_ov002_020bbd5c(void *selfv, int i);   /* defined below */
}

/* LOGIC-VERIFIED NEAR-MISS host copy of ov002 0x020bb614 (SignPost state 1
   Main, the read loop), div=7, prologue-only regalloc residue, body
   byte-identical; floor evidence in nearmiss/db.jsonl. */
// PORT_HOST_ABI: near-miss host copy; the matched-src replacement waits on the floor.
static void port_signpost_read_main(void *selfv)
{
    char *c = (char *)selfv;
    int msgPos[3], tgt[3], plPos[3];
    char *player;
    unsigned short msgId;
    int scale, talk, ang, param;
    short sinV, cosV;
    unsigned char st;

    msgId = 0;
    param = *(int *)(c + 8);
    if (param != 0xffff)
        msgId = (unsigned short)param;

    player = *(char **)(c + 0x598);
    msgPos[0] = *(int *)(c + 0x5c);
    msgPos[1] = *(int *)(c + 0x60) + 0x50000;
    msgPos[2] = *(int *)(c + 0x64);

    scale = 0x5a000;
    if (*(unsigned char *)(c + 0x58e) == 1)
        scale = 0x78000;
    tgt[0] = *(int *)(c + 0x5c);
    tgt[1] = *(int *)(c + 0x60);
    tgt[2] = *(int *)(c + 0x64);
    ang = (int)*(unsigned short *)(c + 0x8e);
    sinV = data_02082214[(ang >> 4) * 2];
    tgt[0] += (int)(((long long)scale * sinV + 0x800) >> 12);
    cosV = data_02082214[(ang >> 4) * 2 + 1];
    tgt[2] += (int)(((long long)scale * cosV + 0x800) >> 12);

    plPos[0] = *(int *)(player + 0x5c);
    plPos[1] = *(int *)(player + 0x60);
    plPos[2] = *(int *)(player + 0x64);

    talk = _ZN6Player12GetTalkStateEv(player);
    switch (talk) {
    case 0:
        st = *(unsigned char *)(c + 0x58d);
        switch (st) {
        case 0:
            if (Vec3_HorzDist(plPos, tgt) < 0x32000) {
                *(unsigned char *)(c + 0x58d) += 1;
            } else if (_Z14ApproachLinearRsss(
                           (short *)(player + 0x8e),
                           Vec3_HorzAngle(plPos, tgt), 0x800) != 0) {
                *(unsigned char *)(c + 0x58d) += 1;
                func_ov002_020bec9c(player, 1, 0, 0x1000, 0);
            }
            break;
        case 1:
            if (Vec3_ApproachHorz(player + 0x5c, tgt, 0xa000) != 0)
                *(unsigned char *)(c + 0x58d) += 1;
            break;
        case 2:
            if (_Z14ApproachLinearRsss(
                    (short *)(player + 0x8e),
                    (short)(*(short *)(c + 0x8e) + 0x8000), 0x800) != 0) {
                if (*(unsigned char *)(c + 0x58e) == 1) {
                    if (func_ov002_020bec84(player, 1) != 0
                        || func_ov002_020bec84(player, 0) != 0) {
                        func_ov002_020bec9c(player, 2, 0x40000000, 0x1000, 0);
                    } else if (func_ov002_020bec84(player, 2) != 0
                               && _ZN6Player12FinishedAnimEv(player) != 0) {
                        func_ov002_020bec9c(player, 3, 0x40000000, 0x1000, 0);
                    } else if (func_ov002_020bec84(player, 3) != 0
                               && _ZN6Player12FinishedAnimEv(player) != 0) {
                        _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(
                            player, c, (short)msgId, msgPos, 0, 1);
                    }
                } else {
                    func_ov002_020bec9c(player, 0, 0, 0x1000, 0);
                    _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(
                        player, c, (short)msgId, msgPos, 0, 1);
                }
            }
            break;
        }
        break;
    case 1:
        break;
    default:
        func_ov002_020bbd5c(c, 0);
        break;
    }

    if (data_0209d660 != 0 && msgId == 0x74a) {
        switch (data_0209d6bc) {
        case 3: data_0209f284 = 1; break;
        case 9: data_0209f284 = 0; break;
        }
    }
    if (*(unsigned char *)(c + 0x594) != data_0209f284 && data_0209f284 != 0)
        func_02012790(0x24);
    *(unsigned char *)(c + 0x594) = data_0209f284;
}

static const struct { unsigned rom; void (*host)(void *); } g_states[10] = {
    {0x020bba24, func_ov002_020bba24}, {0x020bb9fc, func_ov002_020bb9fc},
    {0x020bb9f0, func_ov002_020bb9f0}, {0x020bb614, port_signpost_read_main},
    {0x020bbd50, func_ov002_020bbd50}, {0x020bbcb8, func_ov002_020bbcb8},
    {0x020bbc78, func_ov002_020bbc78}, {0x020bbb14, func_ov002_020bbb14},
    {0x020bbac8, func_ov002_020bbac8}, {0x020bba28, func_ov002_020bba28},
};

extern "C" void port_sign_post_states_seat(void)
{
    PortStatePair *p = (PortStatePair *)data_ov002_0210e084;
    for (int i = 0; i < 10; ++i) {
        if (p[i].fn != g_states[i].rom) {
            std::fprintf(stderr, "FATAL: SignPost state %d %s: the sinit left "
                         "%08x, the ROM's own table says %08x -- WRONG "
                         "BYTES\n", i / 2, (i & 1) ? "Main" : "Init", p[i].fn,
                         g_states[i].rom);
            std::abort();
        }
        p[i].fn = (unsigned)(size_t)g_states[i].host;
        p[i].delta = 0;
    }
}

/* func_ov002_020bbd5c: enter state `i` -- store it, then run its Init. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
extern "C" void func_ov002_020bbd5c(void *selfv, int i)
{
    char *c = (char *)selfv;
    *(int *)(c + 0x354) = i;
    if ((unsigned)i >= PORT_SIGNPOST_STATES) {
        std::fprintf(stderr, "FATAL: SignPost state %d out of range\n", i);
        std::abort();
    }
    ((void (*)(void *))(size_t)data_ov002_0210e084[i].init.fn)(c);
}

/* func_ov002_020bbda4: run the current state's Main. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
extern "C" void func_ov002_020bbda4(void *selfv)
{
    char *c = (char *)selfv;
    int i = *(int *)(c + 0x354);
    if ((unsigned)i >= PORT_SIGNPOST_STATES) {
        std::fprintf(stderr, "FATAL: SignPost state %d out of range\n", i);
        std::abort();
    }
    ((void (*)(void *))(size_t)data_ov002_0210e084[i].main_.fn)(c);
}
