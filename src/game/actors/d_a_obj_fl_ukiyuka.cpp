//cpp
/**
 * Lethal Lava Land's floating floor, both sizes of it.
 *
 * Two spawn IDs select the variant: 0x4b stores variant 0, 0x4c variant
 * 1. InitResources records the variant in mVariant, sets the mesh
 * collider range for the size, and hands this overlay's
 * model/collision record to daObjUkiyuka_c::InitResourcesFrom;
 * CleanupResources hands the same record to CleanupResourcesFrom.
 *
 * Notes:
 * - The file table is still data_ov022_021140d4 (two Resources rows,
 *   indexed by mVariant). The four handles it points at are defined at the
 *   bottom of this file; __sinit_d_a_obj_fl_ukiyuka.cpp constructs them.
 * - func_020393a4 is still the linker name of the dBgW range setter both
 *   Init arms call on mMeshCollider.
 * - InitResources returns nothing, faithfully: the header declares `int`,
 *   the body sets no return value and neither does the ROM. A `return`
 *   emits a real instruction and breaks the match. A host-port caller
 *   reading the result gets garbage.
 * The two registry factories (FL_UKIYUKA_L, then FL_UKIYUKA) close the
 *   unit, ov022 .text 0x02111f6c..0x02112130. The g_profile rows that
 *   name them stay in this overlay's .data, outside this TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_Ukiyuka_c_AssetId {
    ASSET_FL_SHISO_BMD = 0x0610,
    ASSET_FL_SHISO_KCL = 0x0611,
    ASSET_FL_UKI_YUKA_BMD = 0x0614,
    ASSET_FL_UKI_YUKA_KCL = 0x0615
};

#include "daObjFl_Ukiyuka_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
void func_020393a4(dBgW *bgw, int range);
}
extern daObjUkiyuka_c::Resources data_ov022_021140d4[];

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN17daObjFl_Ukiyuka_cD1Ev
// @symbol _ZN17daObjFl_Ukiyuka_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order. The body is empty: the three vptr stores (its own,
 * daObjUkiyuka_c's inlined, then dBgActor_c's) prove the two-level
 * chain, and D0's deallocation is the inline operator delete. */
daObjFl_Ukiyuka_c::~daObjFl_Ukiyuka_c() {}

// @symbol _ZN17daObjFl_Ukiyuka_c16CleanupResourcesEv
/* Indexes the ov022 parameter table with mVariant, the one field this
 * class adds. */
int daObjFl_Ukiyuka_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov022_021140d4[mVariant]);
}

/* 20.12 fixed-point amplitude InitResourcesFrom stores at mBobAmplitude. */
enum { kBobAmplitude = 0x1051 };

// @symbol _ZN17daObjFl_Ukiyuka_c13InitResourcesEv
int daObjFl_Ukiyuka_c::InitResources() {
    switch (actorID) {
    case 0x4b:
        mVariant = 0;
        func_020393a4(&mMeshCollider, 0x650000);
        break;
    case 0x4c:
        mVariant = 1;
        func_020393a4(&mMeshCollider, 0x1000000);
        break;
    }
    InitResourcesFrom(&data_ov022_021140d4[mVariant], kBobAmplitude);
}

/* 8-byte file handles. Both models use func_02017acc / func_02017ab4 and
 * both collision files func_02017b4c / SharedFilePtr_Destruct_Clsn. The
 * spellings are local; the manifest aliases the generated names to those
 * ROM symbols. */
struct UkiyukaModelFilePtr : SharedFilePtr {
    UkiyukaModelFilePtr(u32 fileID);
    ~UkiyukaModelFilePtr();
};

struct UkiyukaCollisionFilePtr : SharedFilePtr {
    UkiyukaCollisionFilePtr(u32 fileID);
    ~UkiyukaCollisionFilePtr();
};

/* Source order is construction order. __sinit_d_a_obj_fl_ukiyuka.cpp emits
 * all four constructions and registers the destructors; the registration
 * nodes are compiler temporaries. */
UkiyukaModelFilePtr gFlUkiYukaModelFile(ASSET_FL_UKI_YUKA_BMD);
UkiyukaCollisionFilePtr gFlUkiYukaCollisionFile(ASSET_FL_UKI_YUKA_KCL);
UkiyukaModelFilePtr gdaObjFl_UkiyukaFlShisoModelFile(ASSET_FL_SHISO_BMD);
UkiyukaCollisionFilePtr gdaObjFl_UkiyukaFlShisoCollisionFile(ASSET_FL_SHISO_KCL);

// @symbol daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA_L
/* Reconstructed source-style name: SM64DS proves daObjFl_Ukiyuka_c through RTTI,
 * allocation size, vtable identity, and the FL_UKIYUKA_L registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: FloatingFloorLllSmall_Spawn.
 *
 * `new daObjFl_Ukiyuka_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x330), dBgActor_c's base constructor, then
 * daObjUkiyuka_c's vptr store and this class's. */
extern "C" daObjFl_Ukiyuka_c *daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA_L(void) {
    return new daObjFl_Ukiyuka_c;
}

// @symbol daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA
/* Reconstructed source-style name: SM64DS proves daObjFl_Ukiyuka_c through RTTI,
 * allocation size, vtable identity, and the FL_UKIYUKA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: FloatingFloorLllBig_Spawn. */
extern "C" daObjFl_Ukiyuka_c *daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA(void) {
    return new daObjFl_Ukiyuka_c;
}
