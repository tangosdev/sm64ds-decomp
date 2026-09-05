//cpp
/* Production translation unit for ov015/daObjBk_Rotebar_c, hand-curated.
 * 7 function(s), .text 0x02112944..0x02112bd0.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov015 0x021146dc  "17daObjBk_Rotebar_c"
 *   _ZTI  ov015 0x021146d0  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov015 0x02114714  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x02112b04). The vptr store is
 *                           addend-0 against this symbol.
 *   size  0x324             daObjBk_Rotebar_c_classInit's own literal (804).
 * The coined RotatingBridge alias that used to sit on this vtable is gone; the
 * ROM's own type string is where the class name now comes from.
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's 31-word table point
 * inside ov015; every other slot still holds dBgActor_c's arm9 word.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x02112944  src/_ZN17daObjBk_Rotebar_cD1Ev.cpp
 *   [1] 0x02112988  src/_ZN17daObjBk_Rotebar_cD0Ev.cpp
 *   [2] 0x021129e0  src/_ZN17daObjBk_Rotebar_c16CleanupResourcesEv.cpp
 *   [3] 0x02112a24  src/_ZN17daObjBk_Rotebar_c6RenderEv.cpp
 *   [4] 0x02112a4c  src/_ZN17daObjBk_Rotebar_c8BehaviorEv.cpp
 *   [5] 0x02112b04  src/_ZN17daObjBk_Rotebar_c13InitResourcesEv.cpp
 *   [6] 0x02112ba0  src/daObjBk_Rotebar_c_classInit.c
 *
 * THE SEVENTH IS THE FACTORY. daObjBk_Rotebar_c_classInit (historical alias
 * RotatingBridge_Spawn) is the BK_ROTEBAR registry profile's spawn function and
 * sits immediately after InitResources in the ROM's own .text order, so it is
 * part of this TU. It keeps C linkage and is written first here, being the
 * highest-address member.
 */

#include "daObjBk_Rotebar_c.h"
#include "SharedFilePtr.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * The two SharedFilePtrs are real bss objects in ov015. symbols.txt carries a
 * second, coined spelling on each of the same two addresses -- TowerStep_ClsnFile
 * on 0x02114a84 and TowerStep_ModelFile on 0x02114a8c -- and nothing else in the
 * tree reads either, so this TU uses the address-true names throughout rather
 * than a prefix that names the wrong class.
 *
 * DecIfAbove0_Byte, func_020393a4 and func_020393d4 have no shared header
 * anywhere in the tree -- every caller declares them locally, so this matches
 * the house spelling rather than inventing an include. dBgW_KcMbg::SetFile and
 * dBgActor_c::IsClsnInRange keep their mangled free spelling: both carry
 * Fix12<int> by value, so an ordinary member call would trigger mwccarm's
 * by-value-class parameter homing and change the ROM ABI. */
struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov015_02114a84;   /* the collision KCL */
extern SharedFilePtr data_ov015_02114a8c;   /* the bridge's BMD  */
extern CLPS_Block    data_ov015_02113654;

unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_020393a4(void *bgw, int range);
void func_020393d4(void *bgw, void *fn);
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mtx, int scale, s16 angleY, void *clps);

/* Sound::PlayLong keeps its mangled spelling for the same reason every other
   caller in the tree does: no shared header declares it. */
int _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 id, const void *pos, s16 f);

/* The factory's own dependencies, restated here rather than pulled in through
   decl_ActorBase.h / decl_Platform.h / decl_common.h as the legacy file did --
   this TU declares in place, and pulling new decl_*.h headers in changes what
   the TU sees and can perturb members that already match. */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV17daObjBk_Rotebar_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjBk_Rotebar_c_classInit, 0x02112ba0, size 0x30        */
/* -------------------------------------------------------------------------- */
// @symbol daObjBk_Rotebar_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjBk_Rotebar_c through
   RTTI, allocation size, vtable identity, and the BK_ROTEBAR registry profile;
   later EAD lineage supplies classInit. Exact original spelling is not
   preserved. Historical alias: RotatingBridge_Spawn.

   804 = 0x324 = the whole object, which is the literal the class's size assert
   in include/daObjBk_Rotebar_c.h is taken from. One vptr store only: the base
   here is dBgActor_c and this class derives from it directly. */
extern "C" int *daObjBk_Rotebar_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV17daObjBk_Rotebar_c; }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN17daObjBk_Rotebar_c13InitResourcesEv, 0x02112b04, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Rotebar_c13InitResourcesEv
int daObjBk_Rotebar_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov015_02114a8c), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    void *kcl = dBgW_Kc::LoadFile(data_ov015_02114a84);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, &data_ov015_02113654);

    /* The bridge turns in place, so riders take the collider's own position
       and angles rather than a velocity or a full transform. */
    func_020393d4(&mMeshCollider,
        (void *)_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    mPauseTimer = 0x3c;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN17daObjBk_Rotebar_c8BehaviorEv, 0x02112a4c, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Rotebar_c8BehaviorEv
int daObjBk_Rotebar_c::Behavior()
{
    if (DecIfAbove0_Byte((unsigned char *)&mPauseTimer) == 0) {
        /* One 0x100 step of yaw per frame while the pause timer is spent, and
           a fresh 0x3c-frame pause every time the heading crosses a half turn:
           0x8000 / 0x100 is 128 frames of turning between rests. */
        mPrevAngleY += 0x100;
        mAngleY = mPrevAngleY;
        mTurnSound = _ZN5Sound8PlayLongEjjjRK7Vector3s(mTurnSound, 3, 0x88, &mCamSpacePosX, 0);
        if ((mPrevAngleY & 0x7fff) == 0)
            mPauseTimer = 0x3c;
    }

    UpdateModelPosAndRotY();
    func_020393a4(&mMeshCollider, 0x320000);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x320000, 0))
        UpdateClsnPosAndRot();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN17daObjBk_Rotebar_c6RenderEv, 0x02112a24, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Rotebar_c6RenderEv
int daObjBk_Rotebar_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN17daObjBk_Rotebar_c16CleanupResourcesEv, 0x021129e0, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Rotebar_c16CleanupResourcesEv
int daObjBk_Rotebar_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_02114a8c.Release();
    data_ov015_02114a84.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN17daObjBk_Rotebar_cD0Ev, 0x02112988, size 0x58        */
/* ROM ordinal 0 -- _ZN17daObjBk_Rotebar_cD1Ev, 0x02112944, size 0x44        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Rotebar_cD1Ev
// @symbol _ZN17daObjBk_Rotebar_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjBk_Rotebar_c.h and declared FIRST. Two measurements force that:
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
 * member with a destructor of its own. D0's trailing deallocation is the
 * inherited inline operator delete, which is why nothing here names a heap.
 */
