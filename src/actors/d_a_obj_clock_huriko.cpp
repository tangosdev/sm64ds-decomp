//cpp
/* daObjClockHuriko_c -- the swinging pendulum inside the clock painting.
 * ov013/daObjClockHuriko_c, one genuine translation unit, 8 functions,
 * .text 0x021111a0..0x021113bc.
 *
 * THE CLASS NAME COMES OUT OF THE CARTRIDGE, not from a coinage. The vtable
 * header at ov013 0x02112120 is {0, 0x021120e0}. That _ZTI is three words --
 * {0x0209a764, 0x021120ec, 0x0208e390} -- whose first word is
 * __si_class_type_info's vtable+8, marking a single-inheritance record; whose
 * second points at 0x021120ec, holding the length-prefixed mangled string
 * "18daObjClockHuriko_c" (18 characters, exactly "daObjClockHuriko_c"); and
 * whose third names the DIRECT BASE, 0x0208e390 = _ZTI8dActor_c in
 * config/arm9/symbols.txt. So the ROM states both the name and `: dActor_c`.
 * This file was src/*ClockPaintingPendulum* until that read renamed it.
 *
 * That rename is what makes this TU verifiable rather than merely tidy. The
 * TU defines the class's key function, so mwccarm emits the whole
 * _ZTV/_ZTI/_ZTS group as vague linkage, and every one of those records is a
 * length-prefixed mangled string or a table keyed on one. Under the coined
 * spelling they could not match the cartridge at any address, so none of them
 * could be licensed. Under the ROM spelling romdata_check word-compares them:
 * _ZTV18daObjClockHuriko_c VERIFIED at 124 bytes -- 31 slots, the full table,
 * neither cut short nor overrun -- plus _ZTI and _ZTS for this class and for
 * dActor_c/dBase_c/fBase_c, 6 VERIFIED and 3 PARTIAL and 0 DIFFERS. The 3
 * PARTIAL are the shared base _ZTS strings, whose ROM extent is longer than
 * anything this TU emits.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 * The destructor is the documented exception and is not written here at all:
 * it is inline in include/daObjClockHuriko_c.h so that D1/D0 come out in ROM
 * order with no homeless D2 -- see the comment at the D1/D0 marker below.
 *
 * Consolidated from these 8 one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN18daObjClockHuriko_cD1Ev.cpp
 *   [1] 0x021111d0  src/_ZN18daObjClockHuriko_cD0Ev.c
 *   [2] 0x02111214  src/_ZN18daObjClockHuriko_c16CleanupResourcesEv.cpp
 *   [3] 0x02111238  src/func_ov013_02111238.c
 *   [4] 0x02111280  src/_ZN18daObjClockHuriko_c6RenderEv.cpp
 *   [5] 0x021112a8  src/_ZN18daObjClockHuriko_c8BehaviorEv.cpp
 *   [6] 0x0211133c  src/_ZN18daObjClockHuriko_c13InitResourcesEv.cpp
 *   [7] 0x02111384  src/daObjClockHuriko_c_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjClockHuriko_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
#include "decl_ActorBase.h"

/* The legacy per-function sources carried two shadow structs here -- a
 * six-slot `Base` and a `Derived {char pad[0xd4]; Base base;}` -- purely so
 * Render could reach the embedded Model's vtable slot 5 without a real
 * declaration. Both are gone: include/Model.h declares that slot as
 * Model::Render(const Vector3 *), and mModel is at +0xd4 by construction. */

extern "C" {
extern int _ZTV18daObjClockHuriko_c[];
extern void *data_020a0eac;
extern int data_ov013_02112280[];
extern void Matrix4x3_FromRotationZXYExt(void *, int, int, int);
void func_ov013_02111238(char *t);
extern signed char data_02092110[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjClockHuriko_c_Spawn, 0x02111384, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daObjClockHuriko_c_Spawn
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV18daObjClockHuriko_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daObjClockHuriko_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(296);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)(_ZTV18daObjClockHuriko_c + 2);
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN18daObjClockHuriko_c13InitResourcesEv, 0x0211133c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c13InitResourcesEv
// recovered name: daObjClockHuriko_c::InitResources
/* daObjClockHuriko_c::InitResources -- vtable slot 0, ov013 0x0211133c.
 *
 * A REAL MEMBER ON PURPOSE, and it has to be. The destructor is defined inline
 * in include/daObjClockHuriko_c.h, so this class's key function is its first
 * DECLARED non-inline virtual -- first declared, not first slot -- which is
 * this one. While this body was still a hand-mangled `extern "C"` free
 * function, no TU anywhere defined the key function, so mwccarm emitted
 * neither the _ZTV/_ZTI/_ZTS group nor the inline destructor's D1/D0 pair, and
 * objisolate refused the whole TU with `_ZN18daObjClockHuriko_cD1Ev has 0
 * defined symbols`. Written as a member it emits both, and this entry's
 * compiler_only_output licenses the RTTI group record by record against the
 * cartridge. Same resolution as src/actors/d_a_tree.cpp's InitResources.
 *
 * The callees are reached through their real declarations -- Model::LoadFile
 * and ModelBase::SetFile from include/Model.h / include/ModelBase.h -- rather
 * than through hand-spelled `extern "C"` mangled names. In a C++ TU that older
 * spelling is not merely ugly, it is broken: mwccarm mangles the identifier a
 * SECOND time and emits a reference to _Z27_ZN5Model8LoadFile...Pv, which
 * nothing defines. include/SharedFilePtr.h documents the same trap. */
s32 daObjClockHuriko_c::InitResources()
{
    void *f = Model::LoadFile(*(SharedFilePtr *)data_ov013_02112280);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov013_02111238((char *)this);
    mAngSpeed = 0x100;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN18daObjClockHuriko_c8BehaviorEv, 0x021112a8, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c8BehaviorEv
/* daObjClockHuriko_c::Behavior -- vtable slot 6, ov013 0x021112a8. The
 * pendulum swings while the global at data_02092110 is not positive: the swing
 * speed reverses at each end of the arc, the angle integrates it, and the tick
 * sound plays as the speed passes +-0x10.
 *
 * The swing angle is dActor_c::mAngleZ, declared at 0x090 in include/dActor_c.h;
 * its separately materialized pointer below preserves the ROM's load/
 * read-modify-write order under 2004/b56.
 *
 * NOT A RAW OFFSET, AND THAT IS DELIBERATE. tubuild's generated body spelled
 * this halfword `*(short *)(c + 0x90)` under a comment asserting no header
 * named it. include/dActor_c.h:73 does name it, and the legacy source this TU
 * replaces already read it as mAngleZ -- the generator regressed the recovery
 * and tools/tiers.py duly dropped this member out of CONVERTED for a raw
 * offset. What follows is the legacy recovery restored, not a new guess. */
s32 daObjClockHuriko_c::Behavior()
{
    char *c = (char *)this;

    if (data_02092110[0] <= 0) {
        s16 *angleZ = &mAngleZ;
        /* The test reads the field instead of going through angleZ, and that is
           load-bearing under 2004/b56: the ROM tests the field directly
           (`ldrsh r0,[r4,#0x90]`) and only then materialises the pointer
           (`add r3,r4,#0x90`) for the read-modify-write below. Spelled as
           `*angleZ`, b56 folds the test into the pointer and emits the two in
           the other order. */
        if (mAngleZ > 0) {
            mAngSpeed -= 8;
        } else {
            mAngSpeed += 8;
        }
        *angleZ = (s16)(*angleZ + mAngSpeed);
        s16 speed = mAngSpeed;
        if (speed == 0x10 || speed == -0x10) {
            Sound::PlayBank3(0x16, *(const Vector3 *)&mCamSpacePosX);
        }
    }
    func_ov013_02111238(c);

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN18daObjClockHuriko_c6RenderEv, 0x02111280, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c6RenderEv
/* daObjClockHuriko_c::Render -- vtable slot 9, ov013 0x02111280. Declared
 * as an override in include/daObjClockHuriko_c.h; real C++ method here,
 * rendering the embedded Model at +0xd4 through its own vtable -- slot 5,
 * Model::Render(const Vector3 *scale), called with a null scale. */
s32 daObjClockHuriko_c::Render() { mModel.Render(0); return 1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov013_02111238, 0x02111238, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov013_02111238
/* Not a vtable slot and not provably a member, so it keeps its func_ name and
 * its raw offsets: it reaches dActor_c's angle triple at +0x8c..+0x90 and the
 * matrix at +0xf0, none of which this class owns. The marker above is not
 * decoration -- without it tools/tiers.py folds this body into the preceding
 * member's fragment and scores these offsets against Render. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov013_02111238(char *t)
{
    Matrix4x3_FromRotationZXYExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN18daObjClockHuriko_c16CleanupResourcesEv, 0x02111214, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c16CleanupResourcesEv
/* daObjClockHuriko_c::CleanupResources -- vtable slot 3, ov013 0x02111214.
 * Releases the one shared file the painting holds; it never touches `this`. */
s32 daObjClockHuriko_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov013_02112280)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN18daObjClockHuriko_cD1Ev, 0x021111a0, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_cD1Ev
// @symbol _ZN18daObjClockHuriko_cD0Ev
/* Both destructors are emitted from the INLINE `~daObjClockHuriko_c() {}` in
 * include/daObjClockHuriko_c.h -- there is deliberately no body here.
 *
 * A complete-object destructor stores this class's vtable over the one the
 * base constructor left, destroys members in reverse declaration order, then
 * runs the base subobject destructor; the deleting destructor does that and
 * calls operator delete. All of it follows from `: dActor_c` and the member
 * types, so the compiler writes both bodies and the bytes still reproduce.
 *
 * Defining ~daObjClockHuriko_c() out of line HERE instead would break the TU
 * two ways: mwccarm would emit D0 at 0x021111d0's slot before D1 at
 * 0x021111a0's, reversing ROM order so objisolate refuses the entire TU, and
 * it would emit a third symbol, D2, which has no address anywhere in the
 * cartridge. See the vtable comment in the header.
 */
