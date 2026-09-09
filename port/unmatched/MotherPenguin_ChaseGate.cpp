/* HOST COPY of src/func_ov018_02111b3c.c -- MOTHER_PENGUIN's chase gate (id
 * 257, ov018, level 10). Run link100, lane MPG, gate mpg.
 *
 * WHY A HOST COPY: the r0-passthrough seam, the sixth instance of the shape
 * port/unmatched/Actor_ClosestPlayer_OverlayReaders.cpp hosts four of and
 * port/unmatched/Bubba_ChaseGate.cpp the fifth. That file's LATENT registry
 * names this exact TU -- "src/func_ov018_02111b3c.c (ov018, calls
 * ClosestPlayer() no arg)" -- as a bug that goes live the instant the body is
 * in the build, which is what this gate does. port/tools/closestplayer_guard.py
 * failed the build rather than letting it through, and it did.
 *
 * The matched src declares `extern char *_ZN5Actor13ClosestPlayerEv(void);` and
 * calls it with no argument. That is byte-identical on ARM because
 * Actor::ClosestPlayer reads `this` from r0 and the caller's r0 is still live
 * across the `bl` -- ROM 0x02111b3c:
 *     push {r4,r5,lr} ; sub sp,#4 ; mov r4,r0 ; bl 0x02010ad8   <- r0 = `this`
 * On the host the definition is `(void *self)` cdecl, so a zero-argument call
 * passes stack garbage and ClosestPlayer's Vec3_Dist((char *)self + 0x5c, ...)
 * reads a garbage base -- the null-this +0x5c fault, the rabbit-crash class.
 *
 * THE FIX passes the body's own first parameter, which is exactly the value the
 * ROM leaves in r0. Everything else is the matched source line for line; only
 * the ClosestPlayer declaration (now one-arg) and its call site change.
 *
 * The matched src TU stays in src/ as the byte proof and is commented out of
 * port/slice_mpg.txt.
 *
 * What the gate answers: "should the mother penguin start chasing?" -- it is
 * the state-0 tick's first call (func_ov018_02112234 makes it when self+0x386
 * is set). It takes the closest player, refuses if the player is further than
 * 0x5dc000 from the anchor at +0x364, reads the player's carried object from
 * +0x360 or +0x358 depending on the player's own mode word at +8, refuses
 * unless that object's id halfword at +0xc is 0x100 and its state word at +8 is
 * 0, and only then parks the player in self+0x374 and enters STATE 2 through
 * func_ov018_021123d0(c, 2). The 2 is the ROM's own `mov r1,#2` at 0x02111bd4.
 *
 * PORT_HOST_ABI: ARM r0 passthrough into a thiscall Actor::ClosestPlayer.
 */
#include "types.h"
#include "decl_common.h"
#include "common.h"

/* the real one-arg (this) shape, the same declaration the four copies in
 * Actor_ClosestPlayer_OverlayReaders.cpp and Bubba_ChaseGate.cpp share */
extern "C" char *_ZN5Actor13ClosestPlayerEv(void *self);

extern "C" {
extern int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern void func_ov018_021123d0(char *c, int i);

void func_ov018_02111b3c(char *c)
{
    char *p = _ZN5Actor13ClosestPlayerEv(c);
    char *r1;
    if (p == 0) return;
    if (Vec3_Dist((struct Vector3 *)(c + 0x364),
                  (struct Vector3 *)(p + 0x5c)) > 0x5dc000) return;
    if (*(int *)(p + 8) == 3) r1 = *(char **)(p + 0x360);
    else r1 = *(char **)(p + 0x358);
    if (r1 == 0) return;
    {
        int b = (int)(*(u16 *)(r1 + 0xc) == 0x100);
        if (b == 0) return;
    }
    if (*(int *)(r1 + 8) != 0) return;
    *(char **)(c + 0x374) = p;
    func_ov018_021123d0(c, 2);
}
}
