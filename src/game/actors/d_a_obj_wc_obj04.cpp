//cpp
/* Switch-activated plank (SWITCH_ACTIVATED_PLANK 98) -- ov029/daObjWc_Obj04_c.
 *
 * ov029 is WDW. RTTI ov029:0x0211415c names daObjWc_Obj04_c; overlay_actors
 * maps profile 98 to SWITCH_ACTIVATED_PLANK. Not daObjWc_Mizu_c (WDW_WATER),
 * not daObjWc_Obj02_c (ARROW_LIFT), not daObjWc_Obj03_c (WATER_DIAMOND), not
 * daObjWc_Obj05_c (CAGE_LIFT).
 *
 * common.h FIRST (via dBgActor_c.h): Model.h would otherwise win with
 * math/Matrix.h's nested Matrix4x3, and InitResources' SetFile matrix
 * argument scalarizes. The two helpers write translation as m[9..11].
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. Highest-address first.
 * The destructor is inline in include/daObjWc_Obj04_c.h (D1 then D0, no D2).
 *
 * deslop leftovers:
 * - dBgW_KcMbg::SetFile 6az: InitResources passes Fix12<int> by value; the
 *   header method form size-DIFFs.
 * - func_020393d4: InitResources stores dBgW::UpdatePosWithTransform on
 *   mMeshCollider; dBgW.h has no setter.
 * - func_020393a4: Behavior pokes mMeshCollider with 0x100000 (no setter).
 * - Event::GetBit stays the scalar mangled ABI (Behavior).
 * - func_ov029_021126dc / 02112710 stay placeholder labels (Y-rot + pos
 *   into mClsnMat2 / mModel2.mat4x3). Not coined.
 * - data_ov029_0211432c / 02114324 / 0211304c BMD/KCL/CLPS handles; this TU
 *   claims .text only.
 * - Behavior case 1 keeps the unsigned short* increment pointer plus the
 *   raw 0x3a0 reload (`*p > 0x168` CSEs and size-DIFFs).
 * - S14: g_profile_WC_OBJ04 stays outside the licensed .text.
 * - common.h first via dBgActor_c.h (SetFile matrix argument).
 */

#include "common.h"
#include "daObjWc_Obj04_c.h"
#include "SharedFilePtr.h"

extern "C" {
void func_020393a4(void *p, int v);
void func_020393d4(int *p, int v);
int _ZN5Event6GetBitEj(unsigned int);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, Matrix4x3 *mat, int scale,
    short angY, CLPS_Block &clps);
void Matrix4x3_FromRotationY(Matrix4x3 *m, int angY);
void func_ov029_021126dc(daObjWc_Obj04_c *c);
void func_ov029_02112710(daObjWc_Obj04_c *t);
extern SharedFilePtr data_ov029_0211432c;
extern SharedFilePtr data_ov029_02114324;
extern CLPS_Block data_ov029_0211304c;
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjWc_Obj04_c_classInit
/* Historical alias: SwitchActivatedPlank_Spawn -- this entry point carried the
 * project's coined class name until the ROM's RTTI name was adopted.
 * The ROM proves the class through RTTI (_ZTS15daObjWc_Obj04_c at
 * 0x02114168), the 0x3a8 allocation, the vtable at 0x021141a0 and
 * the WC_OBJ04 registry profile at 0x0211417c.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02112968 loads 0x3a8 -- the class's own size -- into the header's leaf
 * operator new; 0x02112978 calls dBgActor_c's C2, the store at 0x02112984
 * lays down this class's vptr, and Model's C1 at 0x320 follows. The null
 * check is the one `new` itself emits. The implicit constructor is what
 * keeps the intervening construction inlined; declaring a constructor of
 * our own would emit a `bl` the factory does not have. */
extern "C" daObjWc_Obj04_c *daObjWc_Obj04_c_classInit()
{
    return new daObjWc_Obj04_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj04_c13InitResourcesEv
int daObjWc_Obj04_c::InitResources()
{
    mModel2.SetFile((BMD_File *)Model::LoadFile(data_ov029_0211432c), 1, -1);
    func_ov029_02112710(this);
    func_ov029_021126dc(this);
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov029_02114324);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat2, 0x199, mAngleY, data_ov029_0211304c);
    }
    func_020393d4(
        (int *)&mMeshCollider,
        (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mState = 0;
    mStateTimer = 0;
    mVisible = 0;
    mEventID = (u8)(param1 & 0x1f);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj04_c8BehaviorEv
#pragma optimize_for_size on
int daObjWc_Obj04_c::Behavior()
{
    func_020393a4(&mMeshCollider, 0x100000);

    switch (mState) {
    case 0:
        if (_ZN5Event6GetBitEj(mEventID) == 0)
            break;

        {
            u8 *st_ptr = &mState;
            *st_ptr = *st_ptr + 1;
        }

        mStateTimer = 0;
        mVisible = 1;

        mMeshCollider.Enable(this);
        func_ov029_021126dc(this);
        mMeshCollider.Transform(mClsnMat2, mAngleY);
        break;

    case 1: {
        /* MATCH: `*p > 0x168` CSEs the timer load and size-DIFFs. The
         * unsigned short* increment pointer plus the raw 0x3a0 reload are
         * the cartridge form. */
        unsigned short *p = (unsigned short *)((char *)&mStateTimer);
        if (*(unsigned short *)(((char *)this) + 0x300 + 0xa0) > 0x168) {
            mVisible = *(unsigned short *)(((char *)this) + 0x300 + 0xa0) & 1;
        }
        *p = *p + 1;
        if (_ZN5Event6GetBitEj(mEventID) != 0)
            break;
        mMeshCollider.Disable();
        mState = 0;
        mVisible = 0;
        break;
    }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj04_c6RenderEv
int daObjWc_Obj04_c::Render()
{
    if (mVisible != 0) {
        mModel2.Render(0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj04_c16CleanupResourcesEv
int daObjWc_Obj04_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov029_0211432c.Release();
    data_ov029_02114324.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov029_02112710
extern "C" void func_ov029_02112710(daObjWc_Obj04_c *t)
{
    Matrix4x3_FromRotationY(&t->mModel2.mat4x3, t->mAngleY);
    t->mModel2.mat4x3.m[9] = t->mPosX >> 3;
    t->mModel2.mat4x3.m[10] = t->mPosY >> 3;
    t->mModel2.mat4x3.m[11] = t->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov029_021126dc
extern "C" void func_ov029_021126dc(daObjWc_Obj04_c *c)
{
    Matrix4x3_FromRotationY(&c->mClsnMat2, c->mAngleY);
    c->mClsnMat2.m[9] = c->mPosX;
    c->mClsnMat2.m[10] = c->mPosY;
    c->mClsnMat2.m[11] = c->mPosZ;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj04_cD1Ev
// @symbol _ZN15daObjWc_Obj04_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjWc_Obj04_c.h and declared LAST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it last, with the factory's `new` instantiating the class, is
 * what emits the used D1/D0 pair in cartridge order and keeps this TU the
 * vtable's home.
 *
 * Both bodies destroy Model at +0x320 first, then the inlined dBgActor_c
 * step stores _ZTV10dBgActor_c, destroys dBgW_KcMbg at +0x124 and Model at
 * +0xd4, and chains to dActor_c. D0 then returns the object to the actor
 * heap through dActor_c's inline operator delete.
 */

/* Closes the `optimize_for_size on` opened above. */
#pragma optimize_for_size off
