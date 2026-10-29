//cpp
/*
 * daObjFl_UkiKi_c: the FL_UKI_KI profile (actor 82), a bobbing platform in
 * Lethal Lava Land (ov022; the tree names its files FloatingFloorLllBig_*). A
 * dBgActor_c that remembers its spawn height, sets mPosY to that height plus
 * a sine term every frame, and re-syncs the mesh collider when it is in
 * range.
 *
 * ROM: ov022, 7 functions, .text 0x02112130..0x02112380. daObjFl_UkiKi_c is
 * the cartridge's RTTI spelling: _ZTS at ov022 0x021141b8 is the byte string
 * "15daObjFl_UkiKi_c", and _ZTI at 0x021141ac reads
 * [__si_class_type_info+8, that string, _ZTI10dBgActor_c].
 *
 * The destructor is this class's key function, so this TU also emits
 * _ZTV15daObjFl_UkiKi_c, _ZTI15daObjFl_UkiKi_c and _ZTS15daObjFl_UkiKi_c as
 * vague linkage, alongside the inherited bases' RTTI records. Every one of
 * them has a configured ROM home, so all of them license as deadstrip-data
 * and the seven-function object isolates cleanly. The destructor is declared
 * and defined inline and empty in the class header (see that file for why),
 * so there is deliberately no destructor text in this .cpp for an @symbol
 * marker to sit above.
 *
 * The factory is part of this run: daObjFl_UkiKi_c_classInit at 0x02112350
 * abuts the member run with a zero gap on both sides. ov022's previous
 * function is daObjFl_Ukiyuka_c's classInit ending exactly at 0x02112130, and
 * the next is daObjFl_Fall_Block_cD1Ev starting exactly at 0x02112380, so the
 * whole 0x02112130..0x02112380 window is this class's and nothing unlabelled
 * sits at either edge. The
 * ROM bytes allocate 0x328, call dBgActor_c's ctor and store
 * &_ZTV15daObjFl_UkiKi_c[2] -- exactly what `new daObjFl_UkiKi_c()` compiles
 * to, so the factory is written that way rather than by hand.
 *
 * Function order is the reverse of the ROM's: mwccarm 2004/b56 emits one
 * .text section per function in the reverse of source order, so the
 * highest-address ROM function is written first. Do not reorder. The compiler
 * chooses where the D1/D0 pair lands on its own.
 *
 * Notes:
 * - func_020393d4 is a 4-byte store into dBgW+0x18 (beforeClsnCallback).
 *   d_bg_w.cpp defines it and no header declares it.
 * - data_ov064_0211ba6c is the CLPS block, in overlay .data this TU does not
 *   own.
 * - g_profile_FL_UKI_KI, the FL_UKI_KI registry row that names this factory,
 *   is ov022 .data at 0x021141cc and lives outside this TU.
 */

#include "daObjFl_UkiKi_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daObjFl_UkiKi_c_AssetId { ASSET_FL_UKI_KI_BMD = 0x0612, ASSET_FL_UKI_KI_KCL = 0x0613 };

/* The handles reach their files through these wrappers' constructor and
 * destructor veneers; the manifest maps each to the retail destination.
 * Descriptive local types, not recovered spellings. */
struct FlUkiKiModelFile : SharedFilePtr {
    FlUkiKiModelFile(u32 fileID);
    ~FlUkiKiModelFile();
};

struct FlUkiKiCollisionFile : SharedFilePtr {
    FlUkiKiCollisionFile(u32 fileID);
    ~FlUkiKiCollisionFile();
};

/* This TU's two file handles, in the order the retail initializer
 * (0x02112f78) builds them. */
FlUkiKiModelFile FloatingFloorLllBig_ModelFile(ASSET_FL_UKI_KI_BMD);
FlUkiKiCollisionFile FloatingFloorLllBig_ClsnFile(ASSET_FL_UKI_KI_KCL);

extern CLPS_Block data_ov064_0211ba6c; /* the CLPS block SetFile is handed */
extern s16 data_02082214[];            /* shared sin/cos table, sin at [i*2] */

/* local extern: d_bg_w.cpp defines it and no header declares it. */
extern "C" void func_020393d4(dBgW *bgw, void *fn);

// @symbol daObjFl_UkiKi_c_classInit
/* The registry's allocator for FL_UKI_KI. The implicit default constructor is
   inlined into it at -O4: operator new, dBgActor_c's constructor, the vptr
   store. Historical alias in the rename ledger: LavaPlank_Spawn. */
extern "C" daObjFl_UkiKi_c *daObjFl_UkiKi_c_classInit() {
    return new daObjFl_UkiKi_c();
}

// @symbol _ZN15daObjFl_UkiKi_c13InitResourcesEv
s32 daObjFl_UkiKi_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(FloatingFloorLllBig_ModelFile), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(FloatingFloorLllBig_ClsnFile);
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile(kcl, mClsnMat, 0x1000, mAngleY, data_ov064_0211ba6c);

    /* The platform carries the mesh along by velocity rather than by its
       transform -- it slides straight up and down. */
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);

    mOriginalPosY = mPosY;
    mPhaseAngle = mAngleX;
    return 1;
}

// @symbol _ZN15daObjFl_UkiKi_c8BehaviorEv
/* Bob 30 units either side of mOriginalPosY, one cycle per 64 frames
 * (mPhaseAngle advances 0x400 of 0x10000 a frame). */
s32 daObjFl_UkiKi_c::Behavior() {
    int val = (u16)mPhaseAngle >> 4;
    mPosY = data_02082214[val * 2] * (s16)0x1e + mOriginalPosY;
    mPhaseAngle += 0x400;

    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();

    return 1;
}

// @symbol _ZN15daObjFl_UkiKi_c6RenderEv
s32 daObjFl_UkiKi_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjFl_UkiKi_c16CleanupResourcesEv
s32 daObjFl_UkiKi_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    FloatingFloorLllBig_ModelFile.Release();
    FloatingFloorLllBig_ClsnFile.Release();
    return 1;
}
