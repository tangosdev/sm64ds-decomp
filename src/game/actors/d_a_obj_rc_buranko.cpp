//cpp
/* Production translation unit for ov036/daObjRcBuranko_c, hand-curated.
 * 8 function(s), .text 0x021111a0..0x02111444.
 *
 * The Rainbow Ride swinging platform (profile RC_BURANKO, SWINGING_PLATFORM
 * 127): a mesh collider hung like a pendulum, tipped 0x2000 to one side at
 * spawn and swinging back and forth about its Z axis forever.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov036 0x02113a60  "16daObjRcBuranko_c"
 *   _ZTI  ov036 0x02113a54  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov036 0x02113a98  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x0211137c). The vptr store is
 *                           addend-0 against this symbol.
 *   size  0x320             daObjRcBuranko_c_classInit's own literal (800),
 *                           the same 0x320 dBgActor_c already rounds up to:
 *                           this class's one field lives in that tail padding.
 * The coined SwingingPlatform alias that used to sit on this vtable is gone;
 * the ROM's own type string is where the class name now comes from.
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's 32-word table point
 * inside ov036; every other slot still holds dBgActor_c's arm9 word, including
 * slot 31 (Kill, 0x020ee55c), so nothing else is overridden.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in the header and so
 * emits last, which is where the cartridge has it.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled
 *   in this TU -- both take Fix12<int> by value (wall 6az); a method call
 *   homes the argument and size-DIFFs InitResources / Behavior.
 * Leftover: func_020393d4 is a 4-byte store into dBgW+0x18
 *   (beforeClsnCallback). This TU stores dBgW::UpdatePosWithTransform;
 *   dBgW.h has no setter.
 * Leftover: Matrix4x3_FromRotationXYZExt has no shared header; declared
 *   locally like every other caller.
 * Leftover: data_ov036_02114020 / 02114028 are this overlay's KCL/BMD
 *   handles; data_ov036_02112b68 is the CLPS block. This TU consumes them,
 *   overlay .bss/.data owns them.
 * Leftover: g_profile_RC_BURANKO lives outside this TU (S14).
 * Leftover: func_ov036_0211123c keeps C linkage and its cartridge name;
 *   other config still names it.
 * Leftover: common.h first so func_ov036_0211123c's Matrix4x3 translation
 *   stores use the flat s32 m[12] spelling (math/Matrix.h nested form
 *   scalarises).
 */

#include "common.h"
#include "daObjRcBuranko_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov036_02114020;   /* the collision KCL  */
extern SharedFilePtr data_ov036_02114028;   /* the platform's BMD */
extern CLPS_Block    data_ov036_02112b68;

void func_ov036_0211123c(daObjRcBuranko_c *self);
void func_020393d4(dBgW *bgw, void *fn);
void Matrix4x3_FromRotationXYZExt(void *mtx, int x, int y, int z);

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjRcBuranko_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjRcBuranko_c through
   RTTI, allocation size, vtable identity, and the RC_BURANKO registry profile;
   later EAD lineage supplies classInit. Exact original spelling is not
   preserved. Historical alias: SwingingPlatform_Spawn. */
extern "C" daObjRcBuranko_c *daObjRcBuranko_c_classInit()
{
    return new daObjRcBuranko_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c13InitResourcesEv
int daObjRcBuranko_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov036_02114028), 1, -1);

    /* The pendulum starts a quarter of the way over, not hanging straight
       down, so it is already moving on the frame the player first sees it. */
    mAngleZ = 0x2000;
    func_ov036_0211123c(this);
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov036_02114020);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov036_02112b68);

    /* A rider is carried by the collider's whole transform, not by a position
       and a pair of angles: the platform tilts underfoot as it swings. */
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c8BehaviorEv
int daObjRcBuranko_c::Behavior()
{
    /* The whole pendulum is these four lines: a constant 4-unit acceleration
       always pointed back at zero, integrated into the angle. There is no
       damping term, so the swing never decays and never needs re-kicking. */
    if (mAngleZ < 0)
        mSwingVelZ += 4;
    else
        mSwingVelZ -= 4;

    mAngleZ += mSwingVelZ;

    func_ov036_0211123c(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c6RenderEv
int daObjRcBuranko_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c16CleanupResourcesEv
int daObjRcBuranko_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov036_02114028.Release();
    data_ov036_02114020.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov036_0211123c
/* Private "put the model where the actor is" helper: rebuilds mModel.mat4x3
 * from the actor's three angles, then writes the actor position >> 3 into
 * that matrix's translation row. */
extern "C" {
void func_ov036_0211123c(daObjRcBuranko_c *self)
{
    Matrix4x3_FromRotationXYZExt(&self->mModel.mat4x3,
        self->mAngleX, self->mAngleY, self->mAngleZ);

    self->mModel.mat4x3.m[9]  = self->mPosX >> 3;
    self->mModel.mat4x3.m[10] = self->mPosY >> 3;
    self->mModel.mat4x3.m[11] = self->mPosZ >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_cD1Ev
// @symbol _ZN16daObjRcBuranko_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjRcBuranko_c.h and declared FIRST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it first is what makes this TU the vtable's home, so _ZTV and the
 * RTTI pair land here rather than in whichever other TU happens to name them.
 *
 * Both bodies are short because the chain is short: this class's vptr store,
 * then dBgActor_c's -- inlined, its destructor is defined in its class body --
 * then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds only
 * an s16, which has no destructor of its own. D0's trailing deallocation is
 * the inherited inline operator delete, which is why nothing here names a
 * heap.
 */
