//cpp
/* Cage Lift (CAGE_LIFT 99) -- ov029/daObjWc_Obj05_c.
 *
 * ov029 is WDW. RTTI ov029:0x02113e30 names daObjWc_Obj05_c; overlay_actors
 * maps profile 99 to CAGE_LIFT. Not daObjWc_Obj02_c (ARROW_LIFT), not
 * daObjWc_Obj03_c (WATER_DIAMOND), not daObjWc_Obj04_c
 * (SWITCH_ACTIVATED_PLANK), and not daObjWc_Mizu_c (WDW_WATER).
 *
 * common.h FIRST: dBgActor_c.h / Model.h otherwise win with math/Matrix.h's
 * nested Matrix4x3, and InitResources' SetFile matrix argument scalarizes.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. Highest-address first.
 * The destructor is inline in include/daObjWc_Obj05_c.h (D1 then D0, no D2).
 *
 * deslop leftovers:
 * - dBgW_KcMbg::SetFile 6az: InitResources passes Fix12<int> by value; the
 *   header method form size-DIFFs.
 * - dBgActor_c::IsClsnInRange 6az: Behavior; header method form refused
 *   (include/dBgActor_c.h).
 * - func_020393d4 / func_020393c4 / func_020393a4: InitResources stores
 *   dBgW::UpdatePosWithTransform and &func_ov029_02111e60 on mMeshCollider;
 *   Behavior clips at 0x250000; dBgW.h has no setter.
 * - Sound::PlayLong: Behavior recycles unk_324 at mCamSpacePosX; not in
 *   include/Sound.h.
 * - data_ov029_0211428c / 02114284 / 0211306c BMD/KCL/CLPS handles; this TU
 *   claims .text only. decl_common.h types them int[], so LoadFile/Release
 *   cast at the use site.
 * - func_ov029_02111e40 / 02111e60 stay placeholder labels (BeforeClsn
 *   veneer + player-id trigger). decl_common.h's `void(void)` view of
 *   02111e60 forces the namespaced definition.
 * - S14: g_profile_WC_OBJ05 stays outside the licensed .text.
 * - common.h first (InitResources SetFile matrix argument).
 */

#include "common.h"
#include "daObjWc_Obj05_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"

extern "C" {
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, Matrix4x3 *mat, int scale,
    short angY, CLPS_Block &clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void func_020393d4(void *p, void *v);
void func_020393c4(void *p, void *v);
void func_020393a4(int *p, int v);
int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c, void *pos, unsigned e);
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjWc_Obj05_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: CageLift_Spawn. Attribution stays on
 * daObjWc_Obj05_c_classInit. */
extern "C" daObjWc_Obj05_c *daObjWc_Obj05_c_classInit()
{
    return new daObjWc_Obj05_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// Arg-shifting tail-call veneer. Drops the first argument and forwards the next two;
// #pragma long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call
// (bracketed with `off` so it cannot leak into later members). decl_common.h
// declares this name as `void(void)` for the callback-pointer view; the definition
// and the callee's int view live in namespaces, all binding the same C symbols.
namespace tail_e40 { extern "C" int func_ov029_02111e40(void *b, void *c); }
#pragma long_calls on
namespace fndef_e60 {
extern "C" int func_ov029_02111e60(void *a, void *b, void *c)
{
    return tail_e40::func_ov029_02111e40(b, c);
}
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* BeforeClsn helper: if the other actor's actorID (dActor_c+0x0c) is PLAYER
 * (0xbf), arm unk_32a. The unsigned char temporary is load-bearing. */
extern "C" void func_ov029_02111e40(char *a, char *b)
{
    unsigned char ok = *(unsigned short *)(b + 0xc) == 0xbf;
    if (ok)
        ((daObjWc_Obj05_c *)a)->unk_32a = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_c13InitResourcesEv
s32 daObjWc_Obj05_c::InitResources()
{
    BMD_File *f = (BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov029_0211428c);
    mModel.SetFile(f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    KCL_File *mc = (KCL_File *)dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov029_02114284);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, mc, &mClsnMat, 0x1000, mAngleY, *(CLPS_Block *)data_ov029_0211306c);
    func_020393d4(&mMeshCollider, (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(&mMeshCollider, (void *)&func_ov029_02111e60);
    unk_32b = 0;
    unk_32a = unk_32b;
    unk_328 = 0;
    if (param1 & 1)
        unk_32c = 3;
    else
        unk_32c = 0;
    unk_320 = mPosY;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_c8BehaviorEv
s32 daObjWc_Obj05_c::Behavior()
{
    func_020393a4((int *)&mMeshCollider, 0x250000);

    switch (unk_32c) {
    case 0:
        if (unk_32a != 0) {
            if (unk_32b == 0) {
                u8 *p = &unk_32c;
                *p = *p + 1;
            }
        }
        unk_32b = unk_32a;
        break;
    case 1: {
        s32 *p60 = &mPosY;
        *p60 = *p60 - 0x14000;
        unk_324 = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            (unsigned)unk_324, 3, 0x8d, &mCamSpacePosX, 0);
        {
            int v = unk_320 + (int)0xff5d8000;
            if (mPosY <= v) {
                mPosY = v;
                {
                    u8 *p = &unk_32c;
                    *p = *p + 1;
                }
                unk_328 = 0;
            }
        }
        break;
    }
    case 2: {
        if ((u16)unk_328 >= 0x6e) {
            int snd = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                (unsigned)unk_324, 3, 0x8d, &mCamSpacePosX, 0);
            unk_324 = snd;
            {
                s32 *p60 = &mPosY;
                *p60 = *p60 + 0xa000;
            }
            if (mPosY >= unk_320) {
                mPosY = unk_320;
                unk_32c = 0;
                unk_328 = 0;
            }
        } else {
            u16 *p = (u16 *)&unk_328;
            *p = *p + 1;
        }
        break;
    }
    case 3:
        break;
    }

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    mMeshCollider.Transform(mClsnMat, mAngleY);
    unk_32a = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_c6RenderEv
s32 daObjWc_Obj05_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_c16CleanupResourcesEv
s32 daObjWc_Obj05_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ((SharedFilePtr *)data_ov029_0211428c)->Release();
    ((SharedFilePtr *)data_ov029_02114284)->Release();
    return 1;
}

/* daObjWc_Obj05_c's inline class-body destructor is instantiated by the
 * definitions above. mwccarm emits D1 and D0 into this object. */
