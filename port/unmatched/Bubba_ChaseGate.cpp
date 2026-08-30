/* HOST COPY of src/func_ov032_02111350.c -- BUBBA's chase gate (id 228, ov032,
 * level 24). Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY: the r0-passthrough seam, the fifth instance of the shape
 * port/unmatched/Actor_ClosestPlayer_OverlayReaders.cpp hosts four of. That
 * file's LATENT registry names this exact TU -- "src/func_ov032_02111350.c
 * (ov032, calls ClosestPlayer() no arg)" -- as a bug that goes live the instant
 * ov032 is hosted, which is what this lane does. port/tools/closestplayer_guard
 * .py fails the build rather than letting it through, and it did.
 *
 * The matched src declares `extern char *_ZN5Actor13ClosestPlayerEv(void);` and
 * calls it with no argument. That is byte-identical on ARM because
 * Actor::ClosestPlayer reads `this` from r0 and the caller's r0 is still live
 * across the `bl` -- ROM 0x02111350:
 *     push {r4,lr} ; mov r4, r0 ; bl 0x02010ad8   <- r0 is still `this`
 * On the host the definition is `(void *self)` cdecl, so a zero-argument call
 * passes stack garbage and ClosestPlayer's Vec3_Dist((char *)self + 0x5c, ...)
 * reads a garbage base -- the null-this +0x5c fault, the rabbit-crash class.
 *
 * THE FIX passes the body's own first parameter, which is exactly the value the
 * ROM leaves in r0. Everything else is the matched source line for line; only
 * the ClosestPlayer declaration (now one-arg) and its call site change.
 *
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov032.txt.
 *
 * What the gate answers: "keep chasing?" -- 1 (stop) if there is no player, if
 * the WithMeshClsn at +0x190 is on a wall or on the ground, if the horizontal
 * distance from the anchor at +0x40c exceeds 0x4b0000, or -- unless BUBBA is in
 * the state cell at 0x02113abc -- if the water surface at data_0209f32c is below
 * its own Y. The 0x40c anchor and the 0x4b0000 bound are read off the ROM's own
 * literal pool at 0x021113f0 / the immediate at 0x021113a8.
 *
 * PORT_HOST_ABI: ARM r0 passthrough into a thiscall Actor::ClosestPlayer.
 */
#include "common.h"

/* the real one-arg (this) shape, the same declaration the four copies in
 * Actor_ClosestPlayer_OverlayReaders.cpp share */
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *self);

extern "C" {
int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
int Vec3_HorzDist(const struct Vector3 *a, const struct Vector3 *b);
extern char data_ov032_02113abc[];
extern char data_0209f32c[];

/* PORT_HOST_ABI: ARM r0 passthrough into a thiscall Actor::ClosestPlayer. */
int func_ov032_02111350(char *c)
{
    if (_ZN5Actor13ClosestPlayerEv(c) == 0)
        return 1;
    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x190) != 0 ||
        _ZNK12WithMeshClsn10IsOnGroundEv(c + 0x190) != 0)
        return 1;
    if (Vec3_HorzDist((struct Vector3 *)(c + 0x40c),
                      (struct Vector3 *)(c + 0x5c)) > 0x4b0000)
        return 1;
    if (*(char **)(c + 0x3b0) != (char *)data_ov032_02113abc) {
        if (*(int *)data_0209f32c < *(int *)(c + 0x60))
            return 1;
    }
    return 0;
}
}
