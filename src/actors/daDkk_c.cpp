//cpp
/* Production translation unit for ov025/daDkk_c -- the Grindel/DONKAKU leaf of
 * the daDsnBase_c family (its siblings are daDsn_c and Thwomp, both in ov091).
 *
 * mwccarm emits ordinary functions in reverse source order, so the six
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group. The destructor pair is written
 * by nobody: include/daDkk_c.h defines ~daDkk_c() in the class body, and that
 * alone makes mwccarm emit D1 (0x021118c8) then D0 (0x02111928) at the bottom
 * of the section list -- which is the cartridge's own order. See the header for
 * why the in-class form is load-bearing.
 *
 * func_ov025_021119a4, func_ov025_021119f4 and func_ov025_02111a84 keep their
 * address-derived names: nothing in config/ or include/ spells them any other
 * way, so they stay C-linkage free functions here rather than becoming invented
 * members.
 *
 * Superseded one-function sources (ROM address order):
 *   [0] 0x021118c8  src/_ZN7daDkk_cD1Ev.cpp              (now the header's inline body)
 *   [1] 0x02111928  src/_ZN7daDkk_cD0Ev.cpp              (now the header's inline body)
 *   [2] 0x0211199c  src/_ZN7daDkk_c16OnAimedAtWithEggEv.cpp
 *   [3] 0x021119a4  src/func_ov025_021119a4.cpp
 *   [4] 0x021119f4  src/func_ov025_021119f4.c
 *   [5] 0x02111a84  src/func_ov025_02111a84.cpp
 *   [6] 0x02111b64  src/_ZN7daDkk_c8BehaviorEv.cpp
 *   [7] 0x02111c24  src/_ZN7daDkk_c13InitResourcesEv.cpp
 */

/* INCLUDE ORDER IS LOAD-BEARING, the same way it is in the base class's own TU:
 * daDkk_c.h reaches daDsnBase_c.h -> dBgActor_c.h, which includes common.h
 * BEFORE Model.h, fixing Matrix4x3 to common.h's flat `s32 m[12]' spelling --
 * the one every shard here compiled against. Do not hoist math/Matrix.h, and do
 * not move dBgCh_Lin.h above this line. */
#include "daDkk_c.h"
#include "common.h"
#include "decl_common.h"
#include "dBgCh_Lin.h"

/* decl_common.h already declares every address-named symbol this TU touches --
 * func_ov025_021119a4/021119f4/02111a84, func_ov091_02132dc0/02132e64/02132e98/
 * 02132f04/02132ff4/02133020/02133098 and data_ov025_02113814 -- all taking
 * char*, where several shards had spelled them void*. The real header wins;
 * their shadow declarations are gone and the definitions below match it.
 *
 * These are the ones no header declares. func_0201267c is spelled `void
 * func_0201267c(int, void*)' by eight shards elsewhere in the tree and `int' by
 * one; the two shards merged here disagreed the same way. Neither call site
 * reads the result, so the return type cannot move a byte and the majority
 * spelling is kept. tubuild's conflict detector does not compare return types
 * and reported nothing about it. */
extern "C" {
extern void func_0201267c(int a, void *b);
extern int func_ov091_02133254(char *c);
extern int _Z14ApproachLinearRsss(short &a, short b, short c);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *p);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *p, Fix12i a, Fix12i b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *cc);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *thiz, const Vector3 &v, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int x, int y, int z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN7daDkk_c13InitResourcesEv, 0x02111c24, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDkk_c13InitResourcesEv
/* Vtable slot 0, override of a slot daDsnBase_c leaves pure, and this class's
 * ABI key function -- the first declared virtual that is neither inline nor
 * pure -- so defining it here is what makes this TU emit _ZTV7daDkk_c.
 *
 * Stores the file table pointer at 0x320 (daDsnBase_c's own mFileTable), calls
 * a shared ov091 helper, then either forces mState to a fixed "already
 * airborne" value or runs a downward raycast from the actor's own position to
 * set mProbeHeight from the collision point it finds. The flag test at +0x8 and
 * the raycast machinery are read through raw offsets and opaque locals,
 * matching the un-migrated file this replaces -- neither belongs to this
 * class's own layout.
 */
int daDkk_c::InitResources()
{
    char *c = (char *)this;
    *(void **)(c + 0x320) = data_ov025_02113814;
    int r = func_ov091_02133254(c);
    if (*(int *)(c + 8) & 1) {
        mState = 6;
    } else {
        mState = 0;
        dBgCh_Lin ray;
        Vector3 va;
        Vector3 vb;
        int x = *(int *)(c + 0x5c);
        vb.x = x;
        int y = *(int *)(c + 0x60);
        vb.y = y;
        int z = *(int *)(c + 0x64);
        va.x = x;
        vb.z = z;
        va.y = y;
        va.z = z;
        vb.y = y + 0x7d0000;
        ray.SetObjAndLine(va, vb, this);
        if (ray.DetectClsn()) {
            Vector3 p1 = ray.GetClsnPos();
            Vector3 p2 = ray.GetClsnPos();
            mProbeHeight = p2.y - 0x190000;
        }
    }
    return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN7daDkk_c8BehaviorEv, 0x02111b64, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDkk_c8BehaviorEv
/* Vtable slot 6, the other slot daDsnBase_c leaves pure.
 *
 * Switches on mState to one of eight per-state step functions -- five shared
 * with the ov091 siblings, three private to this overlay -- then runs the
 * post-step housekeeping every daDsnBase_c leaf needs.
 */
int daDkk_c::Behavior()
{
    char *c = (char *)this;
    switch (mState) {
    case 0: func_ov091_02133020(c); break;
    case 1: func_ov091_02132ff4(c); break;
    case 2: func_ov091_02132f04(c); break;
    case 3: func_ov091_02132e98(c); break;
    case 4: func_ov091_02132e64(c); break;
    case 5: func_ov025_02111a84(c); break;
    case 6: func_ov025_021119f4(c); break;
    case 7: func_ov025_021119a4(c); break;
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    func_ov091_02133098(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0 ||
        func_ov091_02132dc0(c) != 0) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov025_02111a84, 0x02111a84, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov025_02111a84
/* mState 5, the fall. Integrates the drop, and on reaching the stored ground
 * height snaps to it, shakes the camera, spawns the impact particle and hands
 * over to state 6. */
extern "C" void func_ov025_02111a84(char *c)
{
    Vector3 v[2];
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
    if (*(int *)(c + 0xa8) >= 0)
        *(int *)(c + 0x9c) = -0x4000;
    else
        *(int *)(c + 0x9c) = -0x8000;
    if (*(int *)(c + 0x60) > *(int *)(c + 0x394))
        return;
    *(int *)(c + 0x60) = *(int *)(c + 0x394);
    v[1].x = *(int *)(c + 0x5c);
    v[1].y = *(int *)(c + 0x60);
    v[1].z = *(int *)(c + 0x64);
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(c, v[1], 0x7d0000);
    *(unsigned char *)(c + 0x39e) = 0x3c;
    *(unsigned char *)(((int)c + 0x39f)) =
        *(unsigned char *)(((int)c + 0x39f)) + 1;
    *(int *)(c + 0x398) = 6;
    v[0].x = *(int *)(c + 0x5c);
    v[0].y = *(int *)(c + 0x60);
    v[0].z = *(int *)(c + 0x64);
    v[0].y = v[0].y + 0x3c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, v[0].x, v[0].y, v[0].z);
    func_0201267c(0xc7, c + 0x74);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov025_021119f4, 0x021119f4, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov025_021119f4
/* mState 6, the pause after landing. Counts 0x39e down; at zero it either
 * starts the next fall (state 5) or, on the fourth pass, turns to face the
 * opposite way (state 7). */
extern "C" void func_ov025_021119f4(char *c)
{
    *(u8 *)(((int)c + 0x39e)) =
        *(u8 *)(((int)c + 0x39e)) - 1;
    if (*(u8 *)(c + 0x39e) != 0) return;
    if (*(u8 *)(c + 0x39f) != 4) {
        *(s32 *)(c + 0x398) = 5;
        *(s32 *)(c + 0xa8) = 0x3c000;
        func_0201267c(0xf4, c + 0x74);
        return;
    }
    *(s32 *)(c + 0x398) = 7;
    *(s16 *)(c + 0x39c) = (s16)(*(s16 *)(c + 0x8e) + 0x8000);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov025_021119a4, 0x021119a4, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov025_021119a4
/* mState 7, the turn. Steps mAngleY toward the target angle at 0x39c; once
 * ApproachLinear reports it has arrived, copies it into 0x94, clears the pass
 * counter and goes back to state 6. */
extern "C" int func_ov025_021119a4(char *c)
{
    int r = _Z14ApproachLinearRsss(*(short *)(c + 0x8e), *(short *)(c + 0x39c), 0x400);
    if (r == 0) return r;
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(int *)(c + 0x398) = 6;
    *(unsigned char *)(c + 0x39f) = 0;
    *(unsigned char *)(c + 0x39e) = 0x28;
    return 0x28;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN7daDkk_c16OnAimedAtWithEggEv, 0x0211199c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDkk_c16OnAimedAtWithEggEv
/* Vtable slot 29, override of dActor_c::OnAimedAtWithEgg. `mov r0,#0xce000; bx
 * lr'. 0xce000 is a Fix12i egg auto-aim lock-on radius of 206.0 -- much wider
 * than dActor_c's own default of 20.0 (0x14000). */
int daDkk_c::OnAimedAtWithEgg()
{
    return 0xce000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- _ZN7daDkk_cD0Ev (0x02111928) and _ZN7daDkk_cD1Ev
 * (0x021118c8) are deliberately NOT written here. include/daDkk_c.h defines
 * ~daDkk_c() in the class body, and that is what makes mwccarm emit the pair in
 * the cartridge's D1-then-D0 order with no D2. Owning the key function above
 * drags both variants in, so neither the `delete p' nor the `p->~daDkk_c()'
 * scaffold the two destructor shards carried is needed; both were dropped and
 * both bodies come out byte-identical without them.
 * -------------------------------------------------------------------------- */
// @symbol _ZN7daDkk_cD1Ev
// @symbol _ZN7daDkk_cD0Ev
