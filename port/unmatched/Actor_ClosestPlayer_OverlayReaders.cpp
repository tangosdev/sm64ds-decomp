/* HOST COPIES of four overlay bodies that each call Actor::ClosestPlayer() with
 * NO argument and rely on `this` riding ARM r0:
 *
 *     [src/func_ov084_02129cf4.c RETIRED at SEAT15B -- the src passes `c`]
 *     src/actors/daPkn_c.cpp    (ov084 range cache;       receiver `r4`)
 *     [src/func_ov094_02136024.cpp RETIRED at SEAT15B -- the src passes `c`]
 *     src/func_ov102_02149078.c    (ov102 refusal test;      receiver `self`)
 *
 * THE r0-PASSTHROUGH SEAM -- identical to Actor_ClosestPlayerWrappers.cpp: each
 * body's own `this` is its first parameter (the ROM's r0), and it calls
 * Actor::ClosestPlayer() with no argument. Byte-identical on ARM because
 * ClosestPlayer reads `this` from r0 and the caller's r0 is still live across the
 * `bl`. On the host the ClosestPlayer definition is `(void *self)` cdecl, so a
 * zero-argument call passes stack garbage and ClosestPlayer's
 * Vec3_Dist((char*)self + 0x5c, ...) reads a garbage base + 0x5c -- the null-this
 * +0x5c fault, the same class as the live rabbit crash.
 *
 * THE FIX passes each body's own first parameter to ClosestPlayer -- exactly the
 * value the ROM leaves in r0. The bodies are the matched sources line for line;
 * only the ClosestPlayer declaration (now one-arg) and its call site change.
 *
 * The remaining byte-locked sources are commented out of their slice gates (32,
 * 70, 203) in favour of these copies; the sources are unchanged. Gate 194's row
 * is uncommented again: that source no longer drops the receiver.
 */

/* STILL-RAW Actor::ClosestPlayer zero-argument readers (LATENT, not yet hosted).
 *
 * Eight more readers open the same r0 passthrough seam but are NOT in the build
 * today: their overlays are not currently mounted in the port (map hits 0), so
 * host copying them now would only add dead code. Each is a latent live bug the
 * instant its overlay gets hosted. A dropped receiver makes ClosestPlayer read
 * off a null base (Vec3_Dist from garbage + 0x5c), the null this + 0x5c fault,
 * the same class as the live rabbit crash (see Actor_ClosestPlayerWrappers.cpp,
 * corpus id 9e164e92).
 *
 * NONE of the eight is listed in any port/slice_gate*.txt manifest (checked on
 * cons commit 24c2dcf75), so there is no arrival line to annotate; this block is
 * the registry instead. BEFORE un-commenting or otherwise hosting the overlay
 * that carries one of these, host copy the reader to PASS the receiver first
 * (exactly the value the ROM leaves in r0), the way the four copies below and
 * the wrappers file do. Per seat gate item 4: a newly hosted overlay carrying
 * known raw readers.
 *
 *   src/game/actors/d_a_pg_mthr.cpp                 (ov018, calls ClosestPlayer() no arg)
 *   src/func_ov020_02111fc4.cpp               (ov020, calls ClosestPlayer() no arg)
 *   [RETIRED, run rel0215 wave 2 lane cast-sweep2: ov032 is hosted now and this
 *    reader went live. Host copy in port/unmatched/Bubba_ChaseGate.cpp; the src
 *    TU is out of port/slice_sweep2_ov032.txt.]
 *   src/game/actors/d_a_bakubaku.cpp                 (ov032, HOSTED -- Bubba_ChaseGate.cpp)
 *   [src/func_ov060_02111f08.c SEATED at SEAT15B -- the src passes `arg0`]
 *   src/actors/Eyerok.cpp               (ov066, calls ClosestPlayer() no arg)
 *   src/unnamed/ov063/func_ov063_02117650.c   (ov063, calls ClosestPlayer() no arg)
 *   src/_ZN8dActor_c23HorzAngleToCPlayerOrAngEv.cpp (Actor::HorzAngleToCPlayerOrAng, no arg;
 *                                              already noted in-slice as "in no slice")
 *   src/game/actors/daTrs_c/_ZN7daTrs_c13InitResourcesEv.cpp (Boo::InitResources: its two call sites
 *                                              already pass c, so it is safe on the host
 *                                              as written, but its extern is declared
 *                                              zero-arg style; keep the c argument if the
 *                                              declaration is ever reprototyped.)
 */
#include "common.h"

/* one real one-arg (this) shape, shared by all four copies below */
extern "C" void *_ZN8dActor_c13ClosestPlayerEv(void *self);

/* ---- func_ov084_02129cf4 -- RETIRED, the matched TU has the seat ---------- */
/* Run link100 wave 15 lane SEAT15B. LINK15 read this row as "owner carries a
 * non-stale reason" and it was wrong: src/func_ov084_02129cf4.c:16 passes the
 * ClosestPlayer receiver itself (`(c)`), exactly like the seven rows of BATCH 1.
 * The src row is uncommented in port/slice_gate32.txt. func_ov084_0212f204 below
 * KEEPS its copy: its matched body lives in src/actors/daPkn_c.cpp, a whole-class
 * TU, so seating it is a different and much larger change. */

/* Vec3_Dist and Vec3_HorzAngle, kept for the copies below. */
extern "C" {
typedef int Fix12i;
Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
}

/* ---- func_ov084_0212f204 (receiver r4) ------------------------------------ */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes r4).
extern "C" void func_ov084_0212f204(char* r4)
{
    struct Vector3 v;
    *(char**)(r4 + 0x460) = (char*)_ZN8dActor_c13ClosestPlayerEv(r4);   /* <-- this, the ROM's r0 */
    {
        char* p = *(char**)(r4 + 0x460);
        if (p != 0) {
            struct Vector3* pp = (struct Vector3*)(((int)p + 0x5c));
            v.x = pp->x;
            v.y = pp->y;
            v.z = pp->z;
            *(int*)(r4 + 0x464) = Vec3_Dist((struct Vector3*)(r4 + 0x5c), &v);
            *(short*)(r4 + 0x468) = Vec3_HorzAngle((struct Vector3*)(r4 + 0x5c), &v);
            *(int*)(r4 + 0x46c) = *(unsigned char*)(*(char**)(r4 + 0x460) + 0x6de);
        } else {
            *(int*)(r4 + 0x464) = 0x7fffffff;
            *(short*)(r4 + 0x468) = *(short*)(r4 + 0x8e);
        }
    }
}

/* ---- func_ov094_02136024 -- RETIRED, the matched TU has the seat --------- */
/* Run link100 wave 15 lane SEAT15B (LINK15 BATCH 1). src/func_ov094_02136024.cpp
 * passes the ClosestPlayer receiver itself now (src:17, `(c)`), so this copy had
 * nothing left to work around. The src row is uncommented in
 * port/slice_gate194.txt and the body below is gone. */

/* ---- func_ov102_02149078 -- RETIRED, the matched TU has the seat --------- */
/* Run linkfull wave 31 lane RS5A (row R4). main #2812 passes the receiver
 * (self->ClosestPlayer() in its promoted src/actors/daObjHatenaBlock_c.cpp), and
 * the port's per-function src/func_ov102_02149078.c takes that one fix as
 * `_ZN8dActor_c13ClosestPlayerEv(self)` (route (a), byte-identical under
 * mwccarm), so this copy had nothing left to work around. The src row is back
 * on port/slice_gate203.txt and the body below is gone. */

/* ---- func_ov060_02111f08 -- RETIRED, the matched TU has the seat --------- */
/* Run link100 wave 15 lane SEAT15B (LINK15 BATCH 1). src/func_ov060_02111f08.c
 * passes the ClosestPlayer receiver itself now (src:21, `(arg0)`), so this copy
 * had nothing left to work around. The src row is on port/slice_l15cp.txt and the
 * body below is gone. */
