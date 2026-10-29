//cpp
/**
 * Bob-omb Battlefield's switch-operated shutter.
 *
 * InitResources / CleanupResources / Behavior hand this overlay's model
 * and collision files to daObjSwdoor_c's shared ov002 helpers. Init then
 * Enable()s the mesh. Behavior always re-syncs collision after the shared
 * step.
 *
 * daObjBSwdoor_c_classInit is reconstructed (RTTI daObjBSwdoor_c,
 * SWITCHDOOR / SHUTTER_BOB registry). Retail does not store that spelling.
 *
 * S41: RTTI daObjBSwdoor_c; D1 0x021111a0 size 0x50, D0 0x021111f0 size 0x64;
 * inline ~daObjBSwdoor_c() in daObjBSwdoor_c.h emits D1 then D0, no D2.
 *
 * Notes:
 * - daObjSwdoor_c::InitResourcesFrom / CleanupResourcesFrom / UpdateShutter
 *   are the base's setup / teardown / step (the base leaves those slots
 *   pure virtual), defined in d_a_obj_swdoor.cpp. The setup helper loads
 *   data_ov014_021145c4 slot 0 with
 *   dExtSimpleModel_c::LoadFile, slot 1 with dBgW_Kc::LoadFile, slot 2 as
 *   CLPS into SetFile, and writes daObjSwdoor_c's four byte fields from
 *   param1.
 * - data_ov014_021145c4 is overlay data; this TU is text-only so the table
 *   stays an extern. g_profile_SWITCHDOOR is the same.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_b_swdoor_AssetId {
    ASSET_B_ANA_SHUTTER_BMD = 0x05a3,
    ASSET_B_ANA_SHUTTER_KCL = 0x05a4
};

#include "daObjBSwdoor_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct BSwdoorModelFilePtr : SharedFilePtr {
    BSwdoorModelFilePtr(unsigned int fileID);
    ~BSwdoorModelFilePtr();
};

struct BSwdoorClsnFilePtr : SharedFilePtr {
    BSwdoorClsnFilePtr(unsigned int fileID);
    ~BSwdoorClsnFilePtr();
};

typedef char BSwdoorModelFilePtr_size_must_be_8[sizeof(BSwdoorModelFilePtr) == 8 ? 1 : -1];
typedef char BSwdoorClsnFilePtr_size_must_be_8[sizeof(BSwdoorClsnFilePtr) == 8 ? 1 : -1];

extern ResourceDescriptor data_ov014_021145c4;

// @symbol daObjBSwdoor_c_classInit
extern "C" daObjBSwdoor_c *daObjBSwdoor_c_classInit() {
    return new daObjBSwdoor_c();
}

// @symbol _ZN14daObjBSwdoor_c13InitResourcesEv
s32 daObjBSwdoor_c::InitResources() {
    s32 r = InitResourcesFrom(&data_ov014_021145c4);
    mMeshCollider.Enable(this);
    return r;
}

// @symbol _ZN14daObjBSwdoor_c8BehaviorEv
s32 daObjBSwdoor_c::Behavior() {
    s32 r = UpdateShutter();
    UpdateClsnPosAndRot();
    return r;
}

// @symbol _ZN14daObjBSwdoor_c16CleanupResourcesEv
/* Cross-overlay tail-call. #pragma long_calls is positional in 2004/b56. */
#pragma long_calls on
s32 daObjBSwdoor_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov014_021145c4);
}
#pragma long_calls off
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_cD0Ev
/* vtable: VT0 = _ZTV14daObjBSwdoor_c; VT1 = _ZTV13daObjSwdoor_c.
 * No definition here: the inline ~daObjBSwdoor_c() emits D0 and D1
 * together. */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_cD1Ev
/* No definition here either: `virtual ~daObjBSwdoor_c() {}` is in
 * include/daObjBSwdoor_c.h. Defined out of line in this file, the pair came
 * out D0-before-D1 -- the reverse of the cartridge -- with a third, homeless
 * D2, and objisolate refused the TU. The inline definition emits both
 * variants here, in ROM order, and no D2. */

/* Source order is construction order: model file 1443, then the collision
 * file 1444. The compiler registers each destructor beside the object. */
BSwdoorModelFilePtr gBAnaShutterModelFile(ASSET_B_ANA_SHUTTER_BMD);
BSwdoorClsnFilePtr gBAnaShutterCollisionFile(ASSET_B_ANA_SHUTTER_KCL);
