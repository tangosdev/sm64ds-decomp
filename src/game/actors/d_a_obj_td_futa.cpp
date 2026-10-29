//cpp
/* Tiny-Huge Island's lid (HUGE_COVER, actor 106): a mesh collider that
 * sits over the huge water until event bit 0xe is set. This class adds
 * no fields and no methods beyond the six slots it overrides.
 *
 * Order is load-bearing: source runs REVERSE of ROM (highest address
 * first), and the destructor stays inline in the header, declared
 * last -- out of line it emits D0 ahead of D1 plus a homeless D2,
 * which the cartridge refuses.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_td_futa_AssetId { ASSET_TD_OBJ_FUTA_BMD = 0x06b5, ASSET_TD_OBJ_FUTA_KCL = 0x06b6 };

#include "daObjTdFuta_c.h"
#include "SharedFilePtr.h"
#include "Event.h"

struct KCL_File;
struct CLPS_Block;

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The manifest aliases those
 * undefined members onto the ROM symbols. */
struct TdFutaModelFilePtr : SharedFilePtr {
    TdFutaModelFilePtr(u32 fileID);
    ~TdFutaModelFilePtr();
};

struct TdFutaClsnFileHandle : SharedFilePtr {
    TdFutaClsnFileHandle(u32 fileID);
    ~TdFutaClsnFileHandle();
};

extern TdFutaClsnFileHandle data_ov032_02113acc;
extern TdFutaModelFilePtr data_ov032_02113ad4;
extern CLPS_Block data_ov032_02112f98;

// @symbol daObjTdFuta_c_classInit
/* Every instruction the cartridge has here falls out of the one `new`.
 * 0x0211266c loads 0x320 -- the class's own size -- into the header's leaf
 * operator new; the following call is dBgActor_c's C2 and the store lays
 * down this class's vptr. The null check is the one `new` itself emits.
 * The implicit constructor is what keeps the intervening dBgActor_c
 * construction inlined; declaring a constructor of our own would emit a `bl`
 * the factory does not have. */
extern "C" daObjTdFuta_c *daObjTdFuta_c_classInit() {
    return new daObjTdFuta_c();
}

// @symbol _ZN13daObjTdFuta_c13InitResourcesEv
s32 daObjTdFuta_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov032_02113ad4), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov032_02113acc);
    mMeshCollider.SetFile(kcl, mClsnMat, 0x199, mAngleY, data_ov032_02112f98);

    return Event::GetBit(0xe) == 0;
}

// @symbol _ZN13daObjTdFuta_c8BehaviorEv
s32 daObjTdFuta_c::Behavior() {
    IsClsnInRange(0, 0);
    return 1;
}

// @symbol _ZN13daObjTdFuta_c6RenderEv
s32 daObjTdFuta_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjTdFuta_c16CleanupResourcesEv
s32 daObjTdFuta_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov032_02113ad4.Release();
    data_ov032_02113acc.Release();
    return 1;
}

// @symbol _ZN13daObjTdFuta_cD1Ev
// @symbol _ZN13daObjTdFuta_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjTdFuta_c.h and declared LAST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it last, with the factory's `new` instantiating the class, is
 * what emits the used D1/D0 pair in cartridge order and keeps this TU the
 * vtable's home.
 *
 * Both bodies are short because the chain is short and this class adds no
 * member of its own: this class's vptr store, then dBgActor_c's -- inlined,
 * which destroys the dExtSimpleModel_c at 0xd4 and the dBgW_KcMbg at 0x124 -- then
 * dActor_c. */

/* File-scope objects, in __sinit_d_a_obj_td_futa.cpp construction order:
 * the model's SharedFilePtr, then the collision file's. */
TdFutaModelFilePtr data_ov032_02113ad4(ASSET_TD_OBJ_FUTA_BMD);
TdFutaClsnFileHandle data_ov032_02113acc(ASSET_TD_OBJ_FUTA_KCL);
