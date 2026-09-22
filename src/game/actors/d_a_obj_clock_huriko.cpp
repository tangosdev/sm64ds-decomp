//cpp
/* daObjClockHuriko_c -- the swinging pendulum inside the clock painting.
 * ov013/daObjClockHuriko_c, one genuine translation unit, 8 functions,
 * .text 0x021111a0..0x021113bc.
 *
 * ov013 is the clock painting overlay (not TTC / ov065): CLOCK_PAINTING_PENDULUM
 * (294) is this class; CLOCK_PAINTING_HAND_SHORT (292) and
 * CLOCK_PAINTING_HAND_LONG (293) share the sibling daObjClock_c.
 *
 * THE CLASS NAME COMES OUT OF THE CARTRIDGE. The vtable header at ov013
 * 0x02112120 is {0, 0x021120e0}. That _ZTI is three words --
 * {0x0209a764, 0x021120ec, 0x0208e390} -- whose first word is
 * __si_class_type_info's vtable+8; whose second points at the length-prefixed
 * mangled string "18daObjClockHuriko_c"; and whose third names the DIRECT BASE,
 * 0x0208e390 = _ZTI8dActor_c.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 * The destructor is the documented exception and is not written here at all:
 * it is inline in include/daObjClockHuriko_c.h so that D1/D0 come out in ROM
 * order with no homeless D2.
 *
 * Members, in ROM address order:
 *   [0] 0x021111a0  daObjClockHuriko_c::~daObjClockHuriko_c  (D1)
 *   [1] 0x021111d0  daObjClockHuriko_c::~daObjClockHuriko_c  (D0)
 *   [2] 0x02111214  daObjClockHuriko_c::CleanupResources
 *   [3] 0x02111238  func_ov013_02111238
 *   [4] 0x02111280  daObjClockHuriko_c::Render
 *   [5] 0x021112a8  daObjClockHuriko_c::Behavior
 *   [6] 0x0211133c  daObjClockHuriko_c::InitResources
 *   [7] 0x02111384  daObjClockHuriko_c_classInit  (factory, actor 294)
 *
 * deslop leftovers:
 * - data_ov013_02112280: SharedFilePtr handle. This TU consumes it; overlay
 *   .bss owns it. symbols.txt has no recovered name, so it is not coined.
 * - Matrix4x3_FromRotationZXYExt stays the C helper (no class method).
 * - func_ov013_02111238 keeps its func_ name: not a vtable slot, and nothing
 *   in the cartridge says it is a member.
 * - g_profile_CLOCK_HURIKO is overlay .data, not licensed to this TU.
 */

#include "daObjClockHuriko_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"

extern "C" {
extern void Matrix4x3_FromRotationZXYExt(void *, int, int, int);
void func_ov013_02111238(daObjClockHuriko_c *self);
extern signed char data_02092110[];
extern SharedFilePtr data_ov013_02112280;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjClockHuriko_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjClockHuriko_c through RTTI,
 * allocation size, vtable identity, and the CLOCK_HURIKO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daObjClockHuriko_c_Spawn. */
extern "C" daObjClockHuriko_c *daObjClockHuriko_c_classInit()
{
    return new daObjClockHuriko_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c13InitResourcesEv
// recovered name: daObjClockHuriko_c::InitResources
/* daObjClockHuriko_c::InitResources -- vtable slot 0, ov013 0x0211133c.
 * A REAL MEMBER ON PURPOSE: the destructor is inline in the header, so this
 * class's key function is its first declared non-inline virtual. */
s32 daObjClockHuriko_c::InitResources()
{
    void *f = Model::LoadFile(data_ov013_02112280);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov013_02111238(this);
    mAngSpeed = 0x100;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c8BehaviorEv
/* daObjClockHuriko_c::Behavior -- vtable slot 6, ov013 0x021112a8. The
 * pendulum swings while the global at data_02092110 is not positive: the swing
 * speed reverses at each end of the arc, the angle integrates it, and the tick
 * sound plays as the speed passes +-0x10.
 *
 * The swing angle is dActor_c::mAngleZ. Its separately materialized pointer
 * below preserves the ROM's load/read-modify-write order under 2004/b56. */
s32 daObjClockHuriko_c::Behavior()
{
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
    func_ov013_02111238(this);

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c6RenderEv
/* daObjClockHuriko_c::Render -- vtable slot 9, ov013 0x02111280. */
s32 daObjClockHuriko_c::Render() { mModel.Render(0); return 1; }

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov013_02111238
/* Not a vtable slot and not provably a member, so it keeps its func_ name.
 * The marker above is not decoration -- without it tools/tiers.py folds this
 * body into the preceding member's fragment. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov013_02111238(daObjClockHuriko_c *self)
{
    Matrix4x3_FromRotationZXYExt(&self->mModel.mat4x3, self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModel.mat4x3.t.x = self->mPosX >> 3;
    self->mModel.mat4x3.t.y = self->mPosY >> 3;
    self->mModel.mat4x3.t.z = self->mPosZ >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_c16CleanupResourcesEv
/* daObjClockHuriko_c::CleanupResources -- vtable slot 3, ov013 0x02111214.
 * Releases the one shared file the painting holds; it never touches `this`. */
s32 daObjClockHuriko_c::CleanupResources()
{
    data_ov013_02112280.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjClockHuriko_cD1Ev
// @symbol _ZN18daObjClockHuriko_cD0Ev
/* Both destructors are emitted from the INLINE `~daObjClockHuriko_c() {}` in
 * include/daObjClockHuriko_c.h -- there is deliberately no body here.
 *
 * Defining ~daObjClockHuriko_c() out of line HERE instead would break the TU
 * two ways: mwccarm would emit D0 at 0x021111d0's slot before D1 at
 * 0x021111a0's, reversing ROM order so objisolate refuses the entire TU, and
 * it would emit a third symbol, D2, which has no address anywhere in the
 * cartridge. See the vtable comment in the header.
 */
