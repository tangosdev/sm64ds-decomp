/* HOST COPY of src/func_ov085_0212b8dc.cpp -- the RABBIT's (daMip_c) state Main,
 * the closest-Player flee/idle machine that Rabbit::InitResources enters on its
 * last line (the Main half of state 0x021306cc, seated by name in
 * hal/actor_overlays.cpp).
 *
 * THE r0-PASSTHROUGH SEAM (the same class as Actor_ClosestPlayerWrappers.cpp and
 * the LakituBro state Ov085_ClosestPlayer_e4a4.cpp on the sibling lane):
 *
 * func_ov085_0212b8dc receives the rabbit's `this` as its first parameter (`c`,
 * the ROM's r0). It calls Actor::ClosestPlayer() with NO argument -- byte-
 * identical on ARM because ClosestPlayer is a nonstatic member that reads `this`
 * from r0, and the caller's own r0 (c) is still live across the `bl`.
 *
 * The matched src spells the call `_ZN5Actor13ClosestPlayerEv()` (the extern is
 * declared `()`), so on the host the zero-argument call pushes nothing and the
 * ClosestPlayer bridge (hal/reverse_bridges.cpp, `(void *self)` cdecl) reads
 * stack garbage as `this`. ClosestPlayer's body is
 *     Vec3_Dist((char*)this + 0x5c, (char*)player + 0x5c)
 * so with `this` the leftover 0, the first argument is 0x5c and the loop reads a
 * null base + 0x5c. That is the live rabbit crash, actor id 187 RABBIT:
 *     access 00000000 at 0000005c   ecx 0000005c   esi <a valid player>
 * -- Vec3_Dist(0 + 0x5c, player + 0x5c), both arguments of the loop exactly.
 * It is rare because the cache is warm on almost every frame: the loop only runs
 * when data_0209b458 is null. Making the rabbit swallowable (slot 19) put it on
 * a player's path with a cold cache, which is what made the pre-existing bare
 * call reachable.
 *
 * THE FIX passes `c` -- exactly the value the ROM leaves in r0 -- to
 * ClosestPlayer. The byte-locked src is unchanged; its slice line (gate 18) is
 * commented out in favour of this copy. The body below is the matched source's
 * control flow line for line.
 *
 * PROOF (run linkw, lane l1). The raw TU still carries the bare call on both
 * the declaration and the call line, and the port's own guard names it:
 *
 *     $ python -c "import closestplayer_guard as g; \
 *                  print(g.scan_file(P, P))"    # P = src/func_ov085_0212b8dc.cpp
 *     src/func_ov085_0212b8dc.cpp:13: zero-argument _ZN5Actor13ClosestPlayerEv
 *     src/func_ov085_0212b8dc.cpp:34: zero-argument _ZN5Actor13ClosestPlayerEv
 *
 * The host face it would land on is one-argument cdecl --
 * hal/reverse_bridges.cpp:45 declares `void *_ZN5Actor13ClosestPlayerEv(void *self)`
 * and :141 defines `Player *Actor::ClosestPlayer() { return (Player *)
 * _ZN5Actor13ClosestPlayerEv(this); }` -- so a zero-argument call hands it
 * whatever word sits at [esp+4]. There is no host-side repair for that: the
 * receiver is not merely in the wrong register, it was never materialised, and
 * MSVC has no calling convention that invents it. Only the caller can supply
 * `c`, and the caller is this TU.
 *
 * ONE CAVEAT worth writing down for whoever seats the next lane slice:
 * port/tools/closestplayer_guard.py enumerated port/slice_gate*.txt only (its
 * gate_active_files filters on `n.startswith("slice_gate")`), so the wave-1
 * lane slices port/slice_w1l*.txt are NOT covered. Adding this raw src to
 * slice_w1l1.txt and re-running the guard printed OK over an unchanged 3896
 * TUs. The guard's protection here is the doc, not the build.
 */
#include "types.h"

extern "C" {
    void* _ZN5Actor13ClosestPlayerEv(void* self);   /* real one-arg (this) shape */
    int func_ov085_0212a788(char* c);
    u32 func_02022cbc(u32 uid, u32 eid, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
    void func_ov085_0212bc78(char* c, void* p);
    int RandomIntInternal(int* seed);
    s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
    int _ZN9Animation8FinishedEv(void* a);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, Fix12i fix, u32 t);
    void ApproachAngle(short* p, s16 target, int a, int b, int step);

    extern int data_0209e650[];
    extern void* data_ov085_0213066c;
    extern void* data_ov085_021305d0[];
    extern void* data_ov085_021305b0[];
    extern void* data_ov085_021305c0[];
    extern void* data_ov085_021305c8[];
}

/* PORT_HOST_ABI: implicit-register-arg -- Actor::ClosestPlayer is a nonstatic member whose `this` rode the caller's own r0 into the bl, so the raw src calls it with zero arguments; the host face is one-argument cdecl (hal/reverse_bridges.cpp:45,141) and reads stack garbage as `this`, the live RABBIT crash (null base + 0x5c in Vec3_Dist). Only the caller can supply the receiver. */
extern "C" int func_ov085_0212b8dc(char* c)
{
    char* player = (char*)_ZN5Actor13ClosestPlayerEv(c);   /* <- this rode r0 */
    if (player == 0) return 1;

    if (func_ov085_0212a788(c) == 1) {
        Vector3 sp;
        s32 pair[2];
        pair[0] = *((s32*)(c + 0x5c));
        pair[1] = *((s32*)(c + 0x464));
        s32 z = *((s32*)(c + 0x64));
        s32 y = pair[1] + 0x3000;
        s32 x = pair[0];
        *((volatile s32*)(&sp.x)) = x;
        *((volatile s32*)(&sp.y)) = y;
        *((volatile s32*)(&sp.z)) = z;
        *((s32*)(c + 0x46c)) = func_02022cbc(*((s32*)(c + 0x46c)), 0xe8,
            *((volatile s32*)(&sp.x)), *(&sp.y), z, 0);
    }

    Vector3 pp;
    Vector3* ppp = (Vector3*)(player + 0x5c);
    pp.x = ppp->x;
    pp.y = ppp->y;
    pp.z = ppp->z;
    int lim = 0x3e8000;
    if (*(int*)(c + 0x43c) == 7) lim = 0x2ee000;

    if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < lim) {
        int t = *(int*)(c + 0x43c);
        int cond = 0;
        if (t == 7 || t == 1 ||
            (t == 4 && *(int*)(c + 0x440) == 1) ||
            (t == 4 && *(int*)(c + 0x440) == 3)) {
            if (*(int*)(c + 0x60) + 0x64000 <= pp.y) cond = 1;
        }
        if (!cond) {
            *(int*)(c + 0x98) = 0;
            func_ov085_0212bc78(c, &data_ov085_0213066c);
            return 1;
        }
    }

    u32 r = (u32)RandomIntInternal(data_0209e650) >> 8;
    if (*(int*)(c + 0x43c) == 1) {
        if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < 0x4b0000) {
            *(s16*)(c + 0x424) = Vec3_HorzAngle((Vector3*)(c + 0x5c), &pp) + 0x8000;
            *(s16*)(c + 0x100) = 0x1e;
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        switch (*(int*)(c + 0x41c)) {
        case 1:
            *(int*)(c + 0x98) = 0x4000;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305d0[1], 0x40000000, 0x1000, 0);
            (*(int*)(c + 0x41c))++;
            break;
        case 2:
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305b0[1], 0x40000000, 0x1000, 0);
            *(int*)(c + 0x98) = 0;
            (*(int*)(c + 0x41c))++;
            break;
        case 3:
            *(s16*)(c + 0x100) = (r & 0x1f) + 0x1e;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c0[1], 0, 0x1000, 0);
            *(int*)(c + 0x41c) = 0;
            break;
        }
    }

    if (*(int*)(c + 0x41c) == 0 && *(u16*)(c + 0x100) == 0) {
        *(s16*)(c + 0x424) = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x42c));
        s16* ang = (s16*)(c + 0x424);
        *ang = *ang + (0x1800 - ((r & 3) << 12));
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c8[1], 0x40000000, 0x1000, 0);
        *(int*)(c + 0x41c) = 1;
    }

    ApproachAngle((short*)(c + 0x94), *(s16*)(c + 0x424), 1, 0x500, 0x500);
    return 1;
}

/* CORRECTION TO THE MERGE MESSAGE ABOVE THIS FILE'S ARRIVAL.
   The commit that merged this fix claims "the rabbit keeps its key, and its
   dialogue comes back". The first half is right and the second is WRONG, and
   the wrong half was mine rather than this file's author's.

   This fix stops the rabbit FAULTING, so it is no longer frozen and no longer
   vanishes. It does NOT by itself restore the dialogue: that needs a player
   pointer at rabbit+0x45c, which two places READ (_ZN6Rabbit8BehaviorEv.c:85
   gating the whole talk block, and func_ov085_0212ae08.c:44, the caught-dialog
   state that ends by spawning the key).

   WHAT THIS NOTE USED TO SAY AND WHY IT WAS WRONG. It claimed NOTHING in the
   tree writes rabbit+0x45c, that the only writer was _ZN6Rabbit8BehaviorEv.c:150
   behind the Enemy::UpdateYoshiEat host stub, and therefore that "the key spawn
   is unreachable" dead code. The first clause is false and the conclusion with
   it. There is a SECOND writer, on a different path entirely:

     src/func_ov085_0212a828.cpp:19   *(void**)(c + 0x45c) = o;

   after a successful Player::TryGrab, and func_ov085_0212a828 is called from
   _ZN6Rabbit8BehaviorEv.c:204 -- OUTSIDE the UpdateYoshiEat block, which opens
   at line 130 and returns at 155. The call is in the ROM's own relocs:
   config/arm9/overlays/ov085/relocs.txt:456,
   from:0x0212c778 kind:arm_call to:0x0212a828 module:overlay(85).
   The natural catch is GRABBING the rabbit, not Yoshi eating it. Only the
   line-150 half of the old claim was ever right.

   The old note's "measured both ways: every run ends with zero keys in the
   world" measured the Yoshi arm only, and read that as the whole actor.

   TWO INDEPENDENT PLAYERS COLLECTED THIS KEY on release 0.2.13 (built
   2026-08-22, source tip 9a4bd47b8), which is proof enough on its own that the
   spawn is reachable: report eacd7977ed564011a31a752119943fbb ("Bunny key was
   floating after I collected it") and Erableto's 16:59:35 level-6 note in the
   same corpus ("Rabbit key is still on top of the player head after collecting
   it"). Both are written up as report 2 of runs/mg15/status/TRIAGE14.md,
   triaged 2026-08-27.

   Re-measured here on 2026-08-27 rather than argued: driving the rabbit's own
   grab check with its collision-detect fields armed (SM64DS_RABBIT_TRIGGER,
   hal/input_probe.cpp) runs the real Player::TryGrab and the real
   func_ov085_0212a828, and rabbit+0x45c comes out written --
     [rabbit] f92 GRABBED: Player::TryGrab succeeded, rabbit+0x45c = 30039F38
   on SM64DS_LEVEL=1. UpdateYoshiEat is still a real open item; it is just not
   the only way in, and this key spawn is not dead code. A comment that says a
   live bug is impossible costs whoever reads it next a session. */
