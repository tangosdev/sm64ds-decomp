/* KING OF THE STAR dupe guards -- host overrides for two PowerStar dispatch
 * states, seated in place of the matched src bodies by PowerStar_States.cpp's
 * table (entries [13] and [5]). src/ stays frozen; these are the port/unmatched
 * copies the win-mode needs.
 *
 * WHY THESE TWO STATES. King mode holds a ONE-STAR invariant: the level_boot
 * star-order seat starves the marker rotation so only one star ever exists, and
 * holder detection reads the single nonzero slot of data_0209f310. Two src
 * paths can add to that array without a "does this player already hold it"
 * check, and either one pushing a holder's count past 1 would, on the next hit,
 * decrement to 1 AND spawn a fresh field star -- two stars at once, the invariant
 * broken. So both are clamped, king-gated. OFF (king_target==0) runs the ROM
 * body byte-for-byte.
 *
 *   [13] func_ov002_020e96a0  -- the toss-landing +1 at frame 30 (its src line
 *        24 has no VS-mode guard at all). Host copy below awards only when the
 *        collector is not already the holder, so a collect is worth exactly one.
 *
 *   [5]  func_ov002_020ea100  -- the collect confirm/revoke window. If the star
 *        were destroyed mid-confirm a stray count could survive; the wrapper runs
 *        the ROM body then, on the confirmed exit (state 6), clamps the holder to
 *        exactly one. The ROM's own revoke arm already returns the count to zero.
 */
#include "types.h"

extern "C" {

/* --- state 13 (func_ov002_020e96a0) externs, verbatim from the src TU ------ */
extern unsigned char data_0209f2d8;
extern signed char data_0209f310[];
extern void GiveVsStars(int idx, int delta);
extern void func_ov002_020e8244(int *out, char *c);
extern void _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(char *c, int *pos, int num, int b, int t, char *p);
extern void _ZN5Actor11UntrackStarERa(char *c, signed char *p);
extern void func_ov002_020e7e58(char *c);
extern void _ZN9ActorBase18MarkForDestructionEv(char *c);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(char *c);
extern void func_ov002_020e8098(char *c);

/* the ROM state-5 body, dispatched normally; the wrapper reconciles after it */
extern void func_ov002_020ea100(char *c);

/* the single source of truth for "is this a King of the Star match" */
extern int port_vs_king_target(void);

/* HOST COPY of src/func_ov002_020e96a0.c. Identical line for line except the
   +1 at frame 30, which in king mode only fires when the collector is not
   already holding the star, so a collect can never be worth two. */
void port_king_power_star_state13(void *selfv)
{
    char *c = (char *)selfv;
    int v[3];
    char *p;
    int *src;
    unsigned short t;

    *(unsigned short *)(((int)c + 0x490)) += 1;
    *(short *)(((int)c + 0x8e)) += 0x800;
    t = *(unsigned short *)(c + 0x490);
    if (t >= 0x1e) {
        if (t == 0x1e) {
            unsigned char kidx = *(unsigned char *)(*(char **)(c + 0x438) + 0x6d8);
            /* KING dupe guard: award only 0 -> 1. Off, this is the ROM's
               unconditional GiveVsStars(idx, 1). */
            if (port_vs_king_target() <= 0 || data_0209f310[kidx] < 1)
                GiveVsStars(kidx, 1);
            func_ov002_020e8244(v, c);
            p = *(char **)(c + 0x438);
            _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, v, data_0209f310[*(unsigned char *)(p + 0x6d8)], 1, 0, p);
            _ZN5Actor11UntrackStarERa(c, (signed char *)(c + 0x498));
        }
        *(unsigned short *)(((int)c + 0x4a2)) &= ~2;
        if (*(unsigned short *)(c + 0x490) < 0x64) return;
        func_ov002_020e7e58(c);
        if ((int)(data_0209f2d8 == 1) != 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
        } else {
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        }
    } else {
        p = *(char **)(c + 0x438);
        src = (int *)(((int)p + 0x5c));
        *(int *)(c + 0x5c) = src[0];
        *(int *)(c + 0x60) = src[1];
        *(int *)(c + 0x64) = src[2];
        *(int *)(((int)c + 0x60)) += 0x104000;
        func_ov002_020e8098(c);
    }
}

/* WRAPPER around the ROM state-5 body. Runs the matched body unchanged, then in
   king mode reconciles the confirm/revoke window: on the confirmed exit the
   holder is clamped to exactly one carried star, so any stray count that slipped
   in (a state-13 double-add, or a star destroyed mid-confirm) cannot survive
   past the moment the collect is finalized. Deterministic and identical on every
   machine: it reads only shared lockstep state and the same env-derived target.
   The body sets c+0x440 to 6 (confirmed) or 8 (revoked); it never frees the
   actor, so reading c+0x438 afterwards is safe. */
void port_king_power_star_state5(void *selfv)
{
    char *c = (char *)selfv;
    func_ov002_020ea100(c);
    if (port_vs_king_target() > 0) {
        char *common = *(char **)(c + 0x438);
        if (common != 0 && *(int *)(c + 0x440) == 6) {
            unsigned char idx = *(unsigned char *)(common + 0x6d8);
            if (idx < 16 && data_0209f310[idx] > 1)
                data_0209f310[idx] = 1;
        }
    }
}

}  /* extern "C" */
