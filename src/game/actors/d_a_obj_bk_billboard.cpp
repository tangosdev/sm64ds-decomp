//cpp
/**
 * Whomp's Fortress pole billboard.
 *
 * Scenery model on a pole. Direct base is dActor_c; the owned
 * member is dExtSimpleModel_c at 0xd4, which closes the class on the factory's
 * literal 0x124.
 *
 * daObjBkBillboard_c_classInit is reconstructed (RTTI
 * daObjBkBillboard_c, BK_BILLBOARD registry). Retail does not store
 * that spelling.
 *
 * Notes:
 * - g_profile_BK_BILLBOARD lives outside this TU (S14).
 * - pad_0d0[4] is unread in this TU. dActor_c is 0xd0 and
 *   dExtSimpleModel_c needs no more than 4-byte alignment, so those four bytes
 *   are an unknown field, not alignment of mModel.
 * - Inline destructor (out-of-line emits D0 before D1).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_bk_billboard_AssetId { ASSET_BK_BILLBORD_BMD = 0x0588 };

#include "daObjBkBillboard_c.h"
#include "SharedFilePtr.h"
#include "math/MtxStack.h"

/* The constructor and destructor are the ROM veneers, aliased in the
 * manifest. */
struct BkBillboardModelFilePtr : SharedFilePtr {
    BkBillboardModelFilePtr(u32 fileID);
    ~BkBillboardModelFilePtr();
};

extern BkBillboardModelFilePtr gBkBillbordModelFile;

// @symbol daObjBkBillboard_c_classInit
extern "C" daObjBkBillboard_c *daObjBkBillboard_c_classInit() {
    return new daObjBkBillboard_c();
}

// @symbol _ZN18daObjBkBillboard_c13InitResourcesEv
s32 daObjBkBillboard_c::InitResources() {
    void *file = dExtSimpleModel_c::LoadFile(gBkBillbordModelFile);
    mModel.SetFile((BMD_File *)file, 1, -1);
    UpdateModelMatrix();
    return 1;
}

// @symbol _ZN18daObjBkBillboard_c6RenderEv
s32 daObjBkBillboard_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjBkBillboard_c16CleanupResourcesEv
s32 daObjBkBillboard_c::CleanupResources() {
    gBkBillbordModelFile.Release();
    return 1;
}

// @symbol _ZN18daObjBkBillboard_c17UpdateModelMatrixEv
void daObjBkBillboard_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

// @symbol _ZN18daObjBkBillboard_cD1Ev
// @symbol _ZN18daObjBkBillboard_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjBkBillboard_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are one vptr store, then mModel destroyed, then ~dActor_c --
   the direct-base chain the RTTI states. D0's trailing deallocation is the
   inline `operator delete` it inherits, which is why nothing here names a
   heap. */

/* The ROM's initializer constructs the BMD handle with file ID 1416, then
 * registers its destructor with the node that follows it in .bss. mwcc
 * emits __sinit_d_a_obj_bk_billboard.cpp from this definition. */
BkBillboardModelFilePtr gBkBillbordModelFile(ASSET_BK_BILLBORD_BMD);
