//cpp
/* Lethal Lava Land's floating block (registry profile FL_BLOCK). It sinks
 * 2 units a frame while the player stands on it, down to 200 below its
 * spawn height, and rises back to that height once it is left alone.
 *
 * daObjFl_Block_c is the cartridge's RTTI spelling: _ZTS at ov022
 * 0x02113e74 is "15daObjFl_Block_c", and _ZTI at 0x02113e68 names
 * dBgActor_c. The empty destructor is inline in the class header. It is
 * the key function, so this TU emits the vtable and the typeinfo records
 * and has no destructor body of its own. mwccarm emits one .text section
 * per function in reverse source order. Do not reorder these seven: the
 * registry factory, the collision callback and its touch helper sit at the
 * top because they are the highest-address .text in the run
 * (.text 0x021116c4..0x02111980).
 */

// Notes (each measured with mwccarm 2004/b56):
// - mMeshCollider.clipRadius = 0x150000 size-DIFFs Behavior 0xa4->0xa0.
//   func_020393a4 is the call the ROM makes.
// - Assigning beforeClsnCallback size-DIFFs InitResources 0xac->0xa8.
//   Assigning stoodOnCallback from StoodOnCallback size-DIFFs 0xac->0xa8.
//   func_020393d4 and func_020393c4 stay; d_bg_w.cpp defines them and no
//   header declares them.
// - Putting the limit on the left of the compare
//   (`mMaxPosY - 0xc8000 > mPosY`, `mMaxPosY < mPosY`) keeps Behavior at
//   0xa4 and changes its words. mPosY on the left matches.
// - g_profile_FL_BLOCK lives outside this TU.
// - data_ov064_0211bb0c is an unnamed row in its own module.
// - The two file handles live at the bottom of this file; the compiler's
//   __sinit_d_a_obj_fl_block.cpp constructs them at overlay load. Their
//   wrapper names are local -- the constructors are the ROM
//   resource-family functions, recorded as aliases in the manifest.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_Block_c_AssetId { ASSET_FL_BLOCK_BMD = 0x05f1, ASSET_FL_BLOCK_KCL = 0x05f2 };

#include "daObjFl_Block_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct FlBlockModelFilePtr : SharedFilePtr {
    FlBlockModelFilePtr(u32 fileID);
    ~FlBlockModelFilePtr();
};

struct FlBlockCollisionFilePtr : SharedFilePtr {
    FlBlockCollisionFilePtr(u32 fileID);
    ~FlBlockCollisionFilePtr();
};

/* This TU's two file handles: the block model and its collision file.
 * InitResources loads both, CleanupResources releases them. Defined at the
 * end of this file so the constructors do not enter .text. */
extern FlBlockModelFilePtr data_ov022_02114558;
extern FlBlockCollisionFilePtr data_ov022_02114550;
extern char data_ov064_0211bb0c[];

extern "C" {
/* local extern: stores at dBgW+0x0c, +0x18 and +0x1c. d_bg_w.cpp defines
 * them and no header declares them. */
void func_020393a4(int *mesh, int range);
void func_020393d4(int *mesh, int callback);
void func_020393c4(int *mesh, int callback);
}

// @symbol daObjFl_Block_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjFl_Block_c through RTTI,
 * allocation size, vtable identity, and the FL_BLOCK registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: FloatOnLavaPlatform_Spawn.
 *
 * `new daObjFl_Block_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x328), dBgActor_c's base constructor, then the
 * vptr store. */
extern "C" daObjFl_Block_c *daObjFl_Block_c_classInit(void) {
    return new daObjFl_Block_c;
}

/* The collision callback InitResources installs in mMeshCollider's slot. The
 * slot passes three arguments; the touch helper wants the last two.
 * long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
// @symbol _ZN15daObjFl_Block_c15StoodOnCallbackEP4dBgWPS_P8dActor_c
void daObjFl_Block_c::StoodOnCallback(dBgW *collider, daObjFl_Block_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}
#pragma pop

// @symbol _ZN15daObjFl_Block_c9OnStoodOnEP8dActor_c
/* The player (actor 0xbf) touched the block: raise mHadClsn, which Behavior
 * reads to decide whether to sink. */
void daObjFl_Block_c::OnStoodOn(dActor_c *other) {
    u8 isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        mHadClsn = 1;
}

// @symbol _ZN15daObjFl_Block_c13InitResourcesEv
s32 daObjFl_Block_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov022_02114558), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    char *kcl = dBgW_Kc::LoadFile(data_ov022_02114550);
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile((KCL_File *)(kcl), mClsnMat, 0x199, mAngleY,
                              *(CLPS_Block *)(data_ov064_0211bb0c));

    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    func_020393c4((int *)&mMeshCollider, (int)&StoodOnCallback);

    mMaxPosY = mPosY;
    return 1;
}

// @symbol _ZN15daObjFl_Block_c8BehaviorEv
s32 daObjFl_Block_c::Behavior() {
    func_020393a4((int *)&mMeshCollider, 0x150000);

    if (mHadClsn) {
        /* Sink, but no lower than 200 below the spawn height. */
        mPosY -= 0x2000;
        if (mPosY < mMaxPosY - 0xc8000)
            mPosY = mMaxPosY - 0xc8000;
        mHadClsn = 0;
    } else {
        /* Rise back to the spawn height. */
        mPosY += 0x2000;
        if (mPosY > mMaxPosY)
            mPosY = mMaxPosY;
    }

    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN15daObjFl_Block_c6RenderEv
s32 daObjFl_Block_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjFl_Block_c16CleanupResourcesEv
s32 daObjFl_Block_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov022_02114558.Release();
    data_ov022_02114550.Release();
    return 1;
}

/* Source order is construction order: model file 1521, collision file 1522.
 * __sinit_d_a_obj_fl_block.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
FlBlockModelFilePtr data_ov022_02114558(ASSET_FL_BLOCK_BMD);
FlBlockCollisionFilePtr data_ov022_02114550(ASSET_FL_BLOCK_KCL);
