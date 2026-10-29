//cpp
/**
 * Whomp's Fortress rotating bar.
 *
 * Pauses 0x3c frames, then turns in yaw until a half turn
 * (mPrevAngleY & 0x7fff == 0) and pauses again.
 *
 * daObjBk_Rotebar_c_classInit is reconstructed (RTTI
 * daObjBk_Rotebar_c, BK_ROTEBAR registry). Retail does not store
 * that spelling.
 *
 * S41: RTTI daObjBk_Rotebar_c; D1 0x02112944 size 0x44, D0 0x02112988 size 0x58;
 * inline dtor in daObjBk_Rotebar_c.h declared first emits D1 then D0, no D2.
 *
 * Notes:
 * - func_020393a4 / func_020393d4 are 4-byte stores into dBgW+0x0c and
 *   dBgW+0x18 (beforeClsnCallback), defined in d_bg_w.cpp with no header.
 * - data_ov015_02114a84 / 02114a8c are this overlay's KCL/BMD handles.
 * - data_ov015_02113654 is the CLPS block in overlay .data this TU does
 *   not own.
 * - g_profile_BK_ROTEBAR lives outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_bk_rotebar_AssetId { ASSET_BK_ROTEBAR_BMD = 0x059b, ASSET_BK_ROTEBAR_KCL = 0x059c };

#include "daObjBk_Rotebar_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"

struct KCL_File;
struct CLPS_Block;

/* The constructors and destructors are the ROM resource-family veneers,
 * aliased in the manifest. */
struct BkRotebarModelFilePtr : SharedFilePtr {
    BkRotebarModelFilePtr(u32 fileID);
    ~BkRotebarModelFilePtr();
};

struct BkRotebarCollisionFilePtr : SharedFilePtr {
    BkRotebarCollisionFilePtr(u32 fileID);
    ~BkRotebarCollisionFilePtr();
};

extern BkRotebarCollisionFilePtr data_ov015_02114a84; /* collision KCL */
extern BkRotebarModelFilePtr data_ov015_02114a8c;     /* bar BMD */
extern CLPS_Block data_ov015_02113654;

/* arm9 C helpers with no shared header. */
extern "C" {
unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_020393a4(dBgW *bgw, int range);
void func_020393d4(dBgW *bgw, void *fn);
}

// @symbol daObjBk_Rotebar_c_classInit
extern "C" daObjBk_Rotebar_c *daObjBk_Rotebar_c_classInit() {
    return new daObjBk_Rotebar_c();
}

// @symbol _ZN17daObjBk_Rotebar_c13InitResourcesEv
s32 daObjBk_Rotebar_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov015_02114a8c), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov015_02114a84);
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile(kcl, *(&mClsnMat), 0x199, mAngleY, *(&data_ov015_02113654));

    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);

    mPauseTimer = 0x3c;
    return 1;
}

// @symbol _ZN17daObjBk_Rotebar_c8BehaviorEv
s32 daObjBk_Rotebar_c::Behavior() {
    if (DecIfAbove0_Byte((unsigned char *)&mPauseTimer) == 0) {
        mPrevAngleY += 0x100;
        mAngleY = mPrevAngleY;
        mTurnSound = Sound::PlayLong(mTurnSound, 3, 0x88, mCamSpacePos, 0);
        if ((mPrevAngleY & 0x7fff) == 0)
            mPauseTimer = 0x3c;
    }

    UpdateModelPosAndRotY();
    func_020393a4(&mMeshCollider, 0x320000);
    if (IsClsnInRange(0x320000, 0))
        UpdateClsnPosAndRot();

    return 1;
}

// @symbol _ZN17daObjBk_Rotebar_c6RenderEv
s32 daObjBk_Rotebar_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN17daObjBk_Rotebar_c16CleanupResourcesEv
s32 daObjBk_Rotebar_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_02114a8c.Release();
    data_ov015_02114a84.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
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

/* The ROM's initializer constructs the model handle with file ID 1435 and
 * the collision handle with file ID 1436, registering a destructor node for
 * each. mwcc emits __sinit_d_a_obj_bk_rotebar.cpp from these definitions. */
BkRotebarModelFilePtr data_ov015_02114a8c(ASSET_BK_ROTEBAR_BMD);
BkRotebarCollisionFilePtr data_ov015_02114a84(ASSET_BK_ROTEBAR_KCL);
