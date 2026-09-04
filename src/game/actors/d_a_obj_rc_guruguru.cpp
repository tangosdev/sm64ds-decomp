//cpp
/* Production translation unit for ov036/daObjRc_Guruguru_c, hand-curated.
 * 6 function(s), .text 0x02111d14..0x02111f5c.
 *
 * The RC_GURUGURU profile's powered turntable: a dBgActor_c that adds a fixed
 * yaw step to its own heading every frame and drags its model and collision
 * mesh round with it, until the actor is flagged and it parks itself with the
 * mesh switched off. The ROM's own RTTI spells the class daObjRc_Guruguru_c
 * (_ZTS at ov036 0x02113d90); the coined ArmedRotatingPlatform alias that
 * named vtable 0x02113dcc in ov036's symbols.txt has been renamed away.
 * _ZTI+8 at 0x02113d84 points at _ZTI10dBgActor_c, so dBgActor_c is the direct
 * and only base.
 *
 * It fills the four vtable slots its base leaves to the derived class --
 * InitResources (0), CleanupResources (3), Behavior (6) and Render (9) -- and
 * adds one s16 of its own storage in dBgActor_c's tail padding at 0x31e.
 *
 * The .text run ends at 0x02111f5c: daObjRc_Guruguru_c_classInit lives there
 * in its own C file and _ZN16daObjRc_Dorifu_cD1Ev at 0x02111f8c opens the next
 * class, so nothing outside this TU's six functions belongs to it.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111d14  src/_ZN18daObjRc_Guruguru_cD1Ev.cpp
 *   [1] 0x02111d58  src/_ZN18daObjRc_Guruguru_cD0Ev.cpp
 *   [2] 0x02111db0  src/_ZN18daObjRc_Guruguru_c16CleanupResourcesEv.cpp
 *   [3] 0x02111df8  src/_ZN18daObjRc_Guruguru_c6RenderEv.cpp
 *   [4] 0x02111e20  src/_ZN18daObjRc_Guruguru_c8BehaviorEv.cpp
 *   [5] 0x02111eb0  src/_ZN18daObjRc_Guruguru_c13InitResourcesEv.cpp
 */

#include "daObjRc_Guruguru_c.h"
#include "SharedFilePtr.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * data_ov036_02113d78 is a REAL three-word data object, not a literal pool.
 * MEASURED: InitResources loads its address ONCE (the literal at 0x02111f54)
 * and then indexes it -- ldr r0,[r1]; ldr r0,[r0,#4]; ldr r3,[r2,#8] -- and
 * CleanupResources loads that same one literal twice, for [0] and [1].
 * Naming the three targets instead (the SharedFilePtrs at 0x021140ac and
 * 0x021140b4 and the CLPS block at 0x02112ac8) builds a three-entry pool and
 * moves every call site, so the indirection is kept.
 *
 * func_020393d4 (arm9 0x020393d4) has no shared header anywhere in the tree --
 * every caller declares it locally, so this matches the house spelling rather
 * than inventing an include. */
extern "C" {
/* [0] the BMD's SharedFilePtr (0x021140ac), [1] the KCL's (0x021140b4),
   [2] the CLPS block dBgW_KcMbg::SetFile is handed (0x02112ac8). */
extern void *data_ov036_02113d78[];

void func_020393d4(void *bgw, void *fn);
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];

bool _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, short angleY, void *clps);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN18daObjRc_Guruguru_c13InitResourcesEv, 0x02111eb0, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so this one call deliberately retains the measured register-level view. */
s32 daObjRc_Guruguru_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(
        *(SharedFilePtr *)data_ov036_02113d78[0]), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov036_02113d78[1]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
        data_ov036_02113d78[2]);

    /* The mesh follows the actor's position AND angles -- this platform turns,
       so the collision has to turn with it. */
    func_020393d4(&mMeshCollider,
        (void *)_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    /* The default spin is clockwise at 0x80 units of yaw a frame; a level
       author overrides both rate and direction through the spawn's mAngleZ. */
    mAngVelY = -0x80;
    if (mAngleZ != 0)
        mAngVelY = mAngleZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN18daObjRc_Guruguru_c8BehaviorEv, 0x02111e20, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c8BehaviorEv
/* IsClsnInRange takes two Fix12<int> by value; see InitResources for why that
 * one call keeps the measured register-level spelling. */
s32 daObjRc_Guruguru_c::Behavior()
{
    /* `+=`, not `mAngleY = mAngleY + ...`: the compound form CSEs the field
       address into a register and reuses it for the load and the store, which
       is what the ROM does here. */
    mAngleY += mAngVelY;

    /* The temporary is load-bearing: the ROM materialises the predicate with
       movne/moveq and then tests it, where a direct `if` folds the two. */
    int flagged = (int)((mFlags & 8) != 0);
    if (flagged != 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 1;
    }

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN18daObjRc_Guruguru_c6RenderEv, 0x02111df8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c6RenderEv
s32 daObjRc_Guruguru_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN18daObjRc_Guruguru_c16CleanupResourcesEv, 0x02111db0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_c16CleanupResourcesEv
s32 daObjRc_Guruguru_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    ((SharedFilePtr *)data_ov036_02113d78[0])->Release();
    ((SharedFilePtr *)data_ov036_02113d78[1])->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN18daObjRc_Guruguru_cD0Ev, 0x02111d58, size 0x58        */
/* ROM ordinal 0 -- _ZN18daObjRc_Guruguru_cD1Ev, 0x02111d14, size 0x44        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjRc_Guruguru_cD1Ev
// @symbol _ZN18daObjRc_Guruguru_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjRc_Guruguru_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2, which is the order
   and the set the ROM carries. Written out of line here instead, mwcc emits D0
   ahead of D1 and rombuild refuses the object outright. */
