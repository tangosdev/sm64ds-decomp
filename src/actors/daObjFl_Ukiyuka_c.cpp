//cpp
/**
 * Lethal Lava Land's floating floor, both sizes of it.
 *
 * Two spawn IDs select the variant: 0x4b stores variant 0, 0x4c variant
 * 1. InitResources records the variant in mVariant, sets the mesh
 * collider range for the size, and hands this overlay's
 * model/collision descriptor to daObjUkiyuka_c's shared ov002 setup
 * helper; CleanupResources hands the same descriptor to the shared
 * teardown helper.
 *
 * deslop
 * Leftover: func_ov002_020b6584 / func_ov002_020b6424 are still the
 *   linker names of daObjUkiyuka_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: the file table is still data_ov022_021140d4 (two
 *   ResourceDescriptor rows, indexed by mVariant).
 * Leftover: func_020393a4 is still the linker name of the dBgW range
 *   setter both Init arms call on mMeshCollider.
 * Leftover: InitResources returns nothing, faithfully: the header
 *   declares `int`, the body sets no return value and neither does
 *   the ROM. A `return` emits a real instruction and breaks the
 *   match. A host-port caller reading the result gets garbage.
 * Leftover: the factories live outside this TU (S14: the FL_UKIYUKA_L
 *   / FL_UKIYUKA g_profile rows stay where the registry owns them).
 */

#include "daObjFl_Ukiyuka_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

/* Same rows as the definition site in src/actors/daObjUkiyuka_c.cpp,
 * which owns the shared helpers. */
struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int func_ov002_020b6424(daObjUkiyuka_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b6584(daObjUkiyuka_c *self, ResourceDescriptor *descriptor,
                        int bobAmplitude);
void func_020393a4(dBgW *bgw, int range);
extern ResourceDescriptor data_ov022_021140d4[];
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN17daObjFl_Ukiyuka_cD1Ev
// @symbol _ZN17daObjFl_Ukiyuka_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order. The body is empty: the three vptr stores (its own,
 * daObjUkiyuka_c's inlined, then dBgActor_c's) prove the two-level
 * chain, and D0's deallocation is the inline operator delete. */
daObjFl_Ukiyuka_c::~daObjFl_Ukiyuka_c()
{
}

// @symbol _ZN17daObjFl_Ukiyuka_c16CleanupResourcesEv
/* Indexes the ov022 parameter table with mVariant, the one field this
 * class adds. */
int daObjFl_Ukiyuka_c::CleanupResources()
{
    return func_ov002_020b6424(this, &data_ov022_021140d4[mVariant]);
}

/* 20.12 fixed-point amplitude the shared helper stores at mBobAmplitude. */
enum { kBobAmplitude = 0x1051 };

// @symbol _ZN17daObjFl_Ukiyuka_c13InitResourcesEv
int daObjFl_Ukiyuka_c::InitResources()
{
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
    func_ov002_020b6584(this, &data_ov022_021140d4[mVariant], kBobAmplitude);
}
