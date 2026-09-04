//cpp
/* Production translation unit for ov043/daObjKm1_Ukishima_c, hand-curated.
 * 6 function(s), .text 0x021111a0..0x021113cc.
 *
 * The floating island of the KM1_UKISHIMA profile: a dBgActor_c that drifts
 * forward along its own yaw and turns a quarter turn every 60 frames. The
 * ROM's own RTTI spells the class daObjKm1_Ukishima_c (_ZTS at ov043
 * 0x0211227c); the coined DiamondLift alias that named vtable 0x021122b8 in
 * ov043's symbols.txt has been renamed away. _ZTI+8 at 0x02112270 points at
 * _ZTI10dBgActor_c, so dBgActor_c is the direct and only base.
 *
 * It fills the four vtable slots its base leaves to the derived class --
 * InitResources (0), CleanupResources (3), Behavior (6) and Render (9) -- and
 * adds one byte of its own storage in dBgActor_c's tail padding at 0x31e.
 *
 * The .text run ends at 0x021113cc: daObjKm1_Ukishima_c_classInit lives there
 * in its own C file and _ZN11RickshawBdwD1Ev at 0x021113fc opens the next
 * class, so nothing outside this TU's six functions belongs to it.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN19daObjKm1_Ukishima_cD1Ev.cpp
 *   [1] 0x021111e4  src/_ZN19daObjKm1_Ukishima_cD0Ev.cpp
 *   [2] 0x0211123c  src/_ZN19daObjKm1_Ukishima_c16CleanupResourcesEv.cpp
 *   [3] 0x02111280  src/_ZN19daObjKm1_Ukishima_c6RenderEv.cpp
 *   [4] 0x021112a8  src/_ZN19daObjKm1_Ukishima_c8BehaviorEv.cpp
 *   [5] 0x02111320  src/_ZN19daObjKm1_Ukishima_c13InitResourcesEv.cpp
 */

#include "daObjKm1_Ukishima_c.h"
#include "SharedFilePtr.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 * DecIfAbove0_Byte (arm9 0x0203add4) and func_020393d4 (arm9 0x020393d4) have
 * no shared header anywhere in the tree -- every caller declares them locally,
 * so this matches the house spelling rather than inventing an include. */
extern "C" {
extern SharedFilePtr data_ov043_021125e0;   /* the KCL  */
extern SharedFilePtr data_ov043_021125e8;   /* the BMD  */
extern char data_ov043_02111c00;            /* the CLPS block SetFile is handed */

unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_020393d4(void *bgw, void *fn);
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, short angleY, void *clps);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN19daObjKm1_Ukishima_c13InitResourcesEv, 0x02111320, size 0xac */
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

    void *kcl = dBgW_Kc::LoadFile(data_ov043_021125e0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov043_02111c00);

    /* The mesh moves with the actor's velocity rather than its transform --
       the island slides, it does not carry a rotating platform's matrix. */
    func_020393d4(&mMeshCollider,
        (void *)_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    mPrevAngleY = mAngleY;
    mHorzSpeed = 0xa000;
    mTurnTimer = 0x3c;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN19daObjKm1_Ukishima_c8BehaviorEv, 0x021112a8, size 0x78 */
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
/* ROM ordinal 3 -- _ZN19daObjKm1_Ukishima_c6RenderEv, 0x02111280, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm1_Ukishima_c6RenderEv
s32 daObjKm1_Ukishima_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19daObjKm1_Ukishima_c16CleanupResourcesEv, 0x0211123c, size 0x44 */
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
/* ROM ordinal 1 -- _ZN19daObjKm1_Ukishima_cD0Ev, 0x021111e4, size 0x58       */
/* ROM ordinal 0 -- _ZN19daObjKm1_Ukishima_cD1Ev, 0x021111a0, size 0x44       */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm1_Ukishima_cD1Ev
// @symbol _ZN19daObjKm1_Ukishima_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKm1_Ukishima_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2, which is the order
   and the set the ROM carries. Written out of line here instead, mwcc emits D0
   ahead of D1 and rombuild refuses the object outright. */
