//cpp
/* Peach's Castle chain-link net -- a climbable mesh with no state of its
 * own. ov009/daObjMc_Metalnet_c, profile MC_METALNET (METAL_NET 339).
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / FLAG / BIRD); this is the net,
 * not castle water or the flag. MetalNetLift (ov064) is a different class.
 *
 * 8 function(s), .text 0x02111dc4..0x02112078.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x02111dc4  src/_ZN18daObjMc_Metalnet_cD1Ev.cpp
 *   [1] 0x02111e08  src/_ZN18daObjMc_Metalnet_cD0Ev.cpp
 *   [2] 0x02111e60  src/_ZN18daObjMc_Metalnet_c16CleanupResourcesEv.cpp
 *   [3] 0x02111ea4  src/_ZN18daObjMc_Metalnet_c16OnPendingDestroyEv.cpp
 *   [4] 0x02111ea8  src/_ZN18daObjMc_Metalnet_c6RenderEv.cpp
 *   [5] 0x02111ed0  src/_ZN18daObjMc_Metalnet_c8BehaviorEv.cpp
 *   [6] 0x02111f40  src/_ZN18daObjMc_Metalnet_c13InitResourcesEv.cpp
 *   [7] 0x02112048  src/daObjMc_Metalnet_c_classInit.c
 */

/* daObjMc_Metalnet_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
#include "daObjMc_Metalnet_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov009_02113e90;   /* the net's BMD        */
extern SharedFilePtr data_ov009_02113e88;   /* its collision KCL    */
extern CLPS_Block    data_ov009_02112bf8;

/* The global mode byte. Unnamed in config; compared against 1 in fifty-odd
   places across the tree, always to mean "not the ordinary single-player run". */
extern unsigned char data_0209f2d8;
/* The one global SaveData instance (include/SaveData.h). Indexed as int[] here,
   the way d_a_set_se.cpp does it; [2] is the 0x008 unlock word. */
extern int data_0209caa0[];

unsigned char NumStars(void);

/* dBgActor_c::IsClsnInRange(Fix12<int>, Fix12<int>) -- reached through the
   mangled name because the two by-value Fix12<int> parameters are wall 6az
   on ov009/daObjMc_Metalnet_c::Behavior. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

/* dBgW_KcMbg::SetFile -- same wall on ov009/daObjMc_Metalnet_c::InitResources:
   its by-value Fix12<int> scale makes a member call cost stack the ROM does
   not spend. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, Matrix4x3 *mat, int scale, s16 angY,
    void *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjMc_Metalnet_c_classInit
/* Reconstructed source-style name. Historical alias: MetalNet_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x0211204c loads 800 = 0x320 -- the class's own size -- into the header's
 * inline operator new; 0x0211205c calls dBgActor_c's C2 and the store at
 * 0x02112068 lays down this class's vptr. The null check is the one `new`
 * itself emits. */
extern "C" daObjMc_Metalnet_c *daObjMc_Metalnet_c_classInit(void)
{
    return new daObjMc_Metalnet_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c13InitResourcesEv
/* Load the model, place it, hand the collision mesh to the collider -- and then
 * decide whether this net should exist at all. Returning 0 from InitResources
 * destroys the actor before it is ever drawn.
 *
 * Two gates, chosen by the level author's parameter byte:
 *   param1 & 0xff == 0xff   the author left it unset, so the net asks the save
 *                           file instead: bit 19 of SaveData's 0x008 unlock
 *                           word must be SET for the net to be removed.
 *   otherwise               the net is removed once the player has 150 stars.
 * Either way, mode 1 short-circuits both and the net always stays. */
s32 daObjMc_Metalnet_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov009_02113e90), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov009_02113e88);
    /* MEASURED on ov009/daObjMc_Metalnet_c::InitResources: this one call
       has to keep the mangled spelling. Its third parameter is a by-value
       Fix12<int> -- wall 6az -- and materialising one costs stack traffic
       the ROM does not have. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
        &data_ov009_02112bf8);

    if ((param1 & 0xff) == 0xff) {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) goto keep;
        if ((data_0209caa0[2] & 0x80000) == 0) goto keep;
        return 0;
    } else {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) goto keep;
        if (NumStars() >= 0x96) return 0;
    }
keep:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c8BehaviorEv
/* The net never moves, but it still re-derives its collider transform every
 * frame -- that is what lets a moving parent carry it.
 *
 * The mode-1 branch force-enables the collider and leaves it enabled; the
 * ordinary branch calls IsClsnInRange purely for its side effect, which is to
 * enable the mesh near the player and switch it off again far away. */
s32 daObjMc_Metalnet_c::Behavior()
{
    int b;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (mMeshCollider.IsEnabled() == 0)
            mMeshCollider.Enable(this);
    } else {
        /* MEASURED on ov009/daObjMc_Metalnet_c::Behavior: the two by-value
           Fix12<int> parameters are wall 6az. */
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c6RenderEv
s32 daObjMc_Metalnet_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c16OnPendingDestroyEv
/* Four bytes: a bare `bx lr`. The override exists to occupy slot 12 so that
 * fBase_c's own OnPendingDestroy does not run for this class. */
void daObjMc_Metalnet_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c16CleanupResourcesEv
s32 daObjMc_Metalnet_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov009_02113e90.Release();
    data_ov009_02113e88.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_cD1Ev
// @symbol _ZN18daObjMc_Metalnet_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjMc_Metalnet_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are two vptr stores and the member destructions, every one a
   consequence of `daObjMc_Metalnet_c : dBgActor_c`: this class's vptr, then
   dBgActor_c's -- inlined, because that destructor is defined in its class
   body -- then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class
   adds no member with a destructor of its own. */
