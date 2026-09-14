//cpp
/* Production translation unit for ov043/daObjKm1_Ukishima_c, hand-curated.
 * 7 function(s), .text 0x021111a0..0x021113fc.
 *
 * BitDW floating island of the KM1_UKISHIMA profile (DIAMOND_LIFT 135): a
 * dBgActor_c that drifts forward along its own yaw and turns a quarter
 * turn every 60 frames. ROM RTTI spells the class daObjKm1_Ukishima_c
 * (_ZTS at ov043 0x0211227c); _ZTI+8 at 0x02112270 points at
 * _ZTI10dBgActor_c. ov043 is Bowser in the Dark World.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled
 *   in this TU -- both take Fix12<int> by value (wall 6az); a method call
 *   homes the argument and size-DIFFs InitResources / Behavior.
 * Leftover: func_020393d4 is a 4-byte store into dBgW+0x18
 *   (beforeClsnCallback). This TU calls it; naming belongs with dBgW in arm9.
 * Leftover: data_ov043_021125e0 / 021125e8 are this overlay's KCL/BMD
 *   handles. data_ov043_02111c00 is the CLPS block in overlay .data this
 *   TU does not own.
 * Leftover: g_profile_KM1_UKISHIMA lives outside this TU (S14).
 * Leftover: inline destructor (out-of-line emits D0 before D1).
 */

#include "daObjKm1_Ukishima_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;

/* DecIfAbove0_Byte (arm9 0x0203add4) has no shared header anywhere in the
 * tree -- every caller declares it locally. */
extern "C" {
extern SharedFilePtr data_ov043_021125e0;   /* the KCL  */
extern SharedFilePtr data_ov043_021125e8;   /* the BMD  */
extern CLPS_Block    data_ov043_02111c00;   /* the CLPS block SetFile is handed */

unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_020393d4(dBgW *bgw, void *fn);

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjKm1_Ukishima_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjKm1_Ukishima_c through
   RTTI, allocation size, vtable identity, and the KM1_UKISHIMA registry
   profile; later EAD lineage supplies classInit. Exact original spelling is
   not preserved. Historical alias: DiamondLift_Spawn.

   Every instruction the cartridge has here falls out of the one `new`.
   0x320 is the whole object: dBgActor_c's own size, this class's one byte
   living in its tail padding at 0x31e. */
extern "C" daObjKm1_Ukishima_c *daObjKm1_Ukishima_c_classInit()
{
    return new daObjKm1_Ukishima_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm1_Ukishima_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so this one call deliberately retains the measured register-level view. */
s32 daObjKm1_Ukishima_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov043_021125e8), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov043_021125e0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov043_02111c00);

    /* The mesh moves with the actor's velocity rather than its transform --
       the island slides, it does not carry a rotating platform's matrix. */
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);

    mPrevAngleY = mAngleY;
    mHorzSpeed = 0xa000;
    mTurnTimer = 0x3c;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm1_Ukishima_c8BehaviorEv
/* IsClsnInRange takes two Fix12<int> by value; see InitResources for why that
 * one call keeps the measured register-level spelling. */
s32 daObjKm1_Ukishima_c::Behavior()
{
    if (!DecIfAbove0_Byte(&mTurnTimer)) {
        mTurnTimer = 0x3c;
        /* A quarter turn, applied to the heading UpdatePos integrates along.
           MEASURED: it has to be the compound assign. Spelled out as
           `mPrevAngleY = mPrevAngleY + 0x4000` mwcc reloads the field address
           for the store, the block gets short enough to if-convert, and the
           ROM's `bne` around it becomes moveq/strbeq/ldrsheq -- 0x70 bytes
           against the cartridge's 0x78. */
        mPrevAngleY += 0x4000;
    }

    UpdatePos(0);
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm1_Ukishima_c6RenderEv
s32 daObjKm1_Ukishima_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm1_Ukishima_c16CleanupResourcesEv
s32 daObjKm1_Ukishima_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov043_021125e8.Release();
    data_ov043_021125e0.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm1_Ukishima_cD1Ev
// @symbol _ZN19daObjKm1_Ukishima_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKm1_Ukishima_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2, which is the order
   and the set the ROM carries. Written out of line here instead, mwcc emits D0
   ahead of D1 and rombuild refuses the object outright. */
