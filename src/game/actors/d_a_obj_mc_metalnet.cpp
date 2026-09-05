//cpp
/* Production translation unit for ov009/daObjMc_Metalnet_c, hand-curated.
 * 8 function(s), .text 0x02111dc4..0x02112078.
 *
 * The MC_METALNET profile's chain-link net: a climbable mesh with no state of
 * its own. It decides ONCE, in InitResources, whether it belongs in the level
 * at all, and after that it does nothing but keep its collider where its model
 * is. The WL_POLELIFT profile at ov009 0x02113abc points at the same descriptor
 * and so spawns the same class.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov009 0x02113aa4  "18daObjMc_Metalnet_c"
 *   _ZTI  ov009 0x02113a98  __si_class_type_info (vptr 0x0209a764); +8 ->
 *                           _ZTI10dBgActor_c (ov002 0x021089ec), so the DIRECT
 *                           base is dBgActor_c and nothing else.
 *   _ZTV  ov009 0x02113ae0  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x02111f40). The vptr store is
 *                           addend-0 against this symbol.
 *   size  0x320             daObjMc_Metalnet_c_classInit's own literal (800).
 *                           This class adds no field, so that is dBgActor_c's
 *                           own size unchanged.
 * The coined MetalNet alias that used to sit on this vtable is gone; the ROM's
 * own type string is where the class name now comes from. MetalNetLift (ov064,
 * _ZTV12MetalNetLift at 0x0211bc68) is a DIFFERENT class and is untouched.
 *
 * Six slots of the cartridge's 33-word table point inside ov009 -- 0
 * (InitResources), 3 (CleanupResources), 6 (Behavior), 9 (Render), 12
 * (OnPendingDestroy) and the 16/17 destructor pair. Every other slot still
 * holds dBgActor_c's own word, including slot 31 (Kill, 0x020ee55c), so
 * nothing else is overridden. Slot 12 is the one this family usually leaves
 * alone: fBase_c::OnPendingDestroy at 0x02043ac0 is replaced by a 4-byte
 * empty body at 0x02111ea4.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in
 * include/daObjMc_Metalnet_c.h and so emits last, which is where the cartridge
 * has it.
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

#include "daObjMc_Metalnet_c.h"
#include "SharedFilePtr.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * The two SharedFilePtrs are real bss objects in ov009 and carry no coined
 * alias, so the address-true names stand. data_0209f2d8 and data_0209caa0 are
 * arm9 globals config still leaves unnamed; the spelling below is the one every
 * other caller in the tree uses (src/actors/d_a_set_se.cpp,
 * src/actors/daObjPathLift_c.cpp).
 *
 * dBgW_KcMbg::SetFile keeps its mangled free spelling: it carries Fix12<int> by
 * value, so an ordinary member call would trigger mwccarm's by-value-class
 * parameter homing and change the ROM ABI. */
struct KCL_File;
struct CLPS_Block;

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

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mtx, int scale, s16 angleY, void *clps);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjMc_Metalnet_c_classInit, 0x02112048, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daObjMc_Metalnet_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* resolved: VT0 = _ZTV18daObjMc_Metalnet_c */
/* Reconstructed source-style name; the exact original SM64DS spelling is not
 * preserved. Historical alias: MetalNet_Spawn.
 *
 * No member of this class needs constructing beyond what dBgActor_c's own
 * constructor does, which is why the factory is three calls and a store. */
int *daObjMc_Metalnet_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* This TU defines every virtual of the class, so mwccarm emits the vtable
           HERE and its `_ZTV` names the emitted OBJECT -- the two-word ABI preamble
           at 0x02113ad8 -- not the address point. `[2]` steps over the preamble onto
           the address point the cartridge actually stores, 0x02113ae0. A TU that only
           IMPORTS someone else's vtable writes the bare symbol instead. */
        p[0] = (int)&_ZTV18daObjMc_Metalnet_c[2];
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN18daObjMc_Metalnet_c13InitResourcesEv, 0x02111f40, size 0x108 */
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

    void *kcl = dBgW_Kc::LoadFile(data_ov009_02113e88);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov009_02112bf8);

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
/* ROM ordinal 5 -- _ZN18daObjMc_Metalnet_c8BehaviorEv, 0x02111ed0, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c8BehaviorEv
/* The net never moves, but it still re-derives its collider transform every
 * frame -- that is what lets a moving parent carry it.
 *
 * The mode-1 branch force-enables the collider and leaves it enabled; the
 * ordinary branch calls IsClsnInRange purely for its side effect, which is to
 * enable the mesh near the player and switch it off again far away. Neither
 * branch looks at the result, so nothing here reads the return value. */
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
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN18daObjMc_Metalnet_c6RenderEv, 0x02111ea8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c6RenderEv
s32 daObjMc_Metalnet_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN18daObjMc_Metalnet_c16OnPendingDestroyEv, 0x02111ea4, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_c16OnPendingDestroyEv
/* Four bytes: a bare `bx lr`. The override exists to occupy slot 12 so that
 * fBase_c's own OnPendingDestroy does not run for this class. */
void daObjMc_Metalnet_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN18daObjMc_Metalnet_c16CleanupResourcesEv, 0x02111e60, size 0x44 */
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
/* ROM ordinal 1 -- _ZN18daObjMc_Metalnet_cD0Ev, 0x02111e08, size 0x58        */
/* ROM ordinal 0 -- _ZN18daObjMc_Metalnet_cD1Ev, 0x02111dc4, size 0x44        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjMc_Metalnet_cD1Ev
// @symbol _ZN18daObjMc_Metalnet_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjMc_Metalnet_c.h and declared FIRST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it first is what makes this TU the vtable's home, so _ZTV and the
 * RTTI pair land here rather than in whichever other TU happens to name them.
 *
 * Both bodies are short because the chain is short: this class's vptr store,
 * then dBgActor_c's -- inlined, its destructor is defined in its class body --
 * then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member of its own at all. D0's trailing deallocation is the inherited inline
 * operator delete, which is why nothing here names a heap.
 */
