//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_Koma_D_c_AssetId { ASSET_FL_KOMA_D_BMD = 0x05f5, ASSET_FL_KOMA_D_KCL = 0x05f6 };

#pragma defer_codegen off
/**
 * Lethal Lava Land's spinning disc (profile FL_KOMA_D).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers, with a fixed spin step of 0x100. The destructor is out
 * of line: defer_codegen off lays down D1, D0, CleanupResources,
 * InitResources and the registry factory in ROM order.
 *
 * daObjFl_Koma_D_c_classInit is reconstructed (RTTI
 * daObjFl_Koma_D_c, FL_KOMA_D registry). Retail does not store that
 * spelling.
 */

// What stays:
// - g_profile_FL_KOMA_D, the typeinfo at 0x02113d98, and the type name at
//   0x02113db0 stay compiler-emitted. The model/collision/CLPS row between
//   them is defined at the bottom of this file. The two handles it points
//   at are this file's .bss; __sinit_d_a_obj_fl_koma_d.cpp constructs them.
// - #pragma defer_codegen off stays on line 2. Dropping it still matches
//   each body, but the four functions emit in reverse ROM order.
// - An inline empty destructor still matches D1 and D0, but the ordinal
//   pair flips, D2 is not emitted, and 13 RTTI/vtable symbols come out
//   unlicensed.

#include "SpawnInfo.h"
#include "daObjFl_Koma_D_c.h"
#include "SharedFilePtr.h"

#include "ResourceDescriptor.h"

/* This descriptor matches the shared helpers in d_a_obj_kaitendai.cpp.
 * Defined at the bottom, after the handles it points at. */
extern "C" ResourceDescriptor data_ov022_02113da4;

daObjFl_Koma_D_c::~daObjFl_Koma_D_c() {}

int daObjFl_Koma_D_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov022_02113da4);
}

int daObjFl_Koma_D_c::InitResources() {
    return InitResourcesFrom(&data_ov022_02113da4, 0x100);
}

/* Reconstructed source-style name: SM64DS proves daObjFl_Koma_D_c through
 * RTTI, allocation size, most-derived vtable identity, and the FL_KOMA_D
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. Historical alias: RotatingPlatformLll_Spawn.
 *
 * `new daObjFl_Koma_D_c` is the whole sequence the loose factory spelled
 * by hand: fBase_c::operator new(0x320), dBgActor_c's base constructor,
 * then daObjKaitendai_c's vptr store and this class's own. */
daObjFl_Koma_D_c *daObjFl_Koma_D_c_classInit(void) {
    return new daObjFl_Koma_D_c;
}

/* 8-byte file handles. The model uses ModelFile::ModelFile / ModelFile::~ModelFile and the
 * collision file ClsnFile::ClsnFile / ClsnFile::~ClsnFile. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct KomaDModelFilePtr : SharedFilePtr {
    KomaDModelFilePtr(u32 fileID);
    ~KomaDModelFilePtr();
};

struct KomaDCollisionFilePtr : SharedFilePtr {
    KomaDCollisionFilePtr(u32 fileID);
    ~KomaDCollisionFilePtr();
};

/* Source order is construction order: model file 1525, collision file 1526.
 * __sinit_d_a_obj_fl_koma_d.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
KomaDModelFilePtr gFlKomaDModelFile(ASSET_FL_KOMA_D_BMD);
KomaDCollisionFilePtr gFlKomaDCollisionFile(ASSET_FL_KOMA_D_KCL);

/* 0x02113da4..0x02113db0. Words at 0x02113da8 and 0x02113dac are the
 * collision handle and the ov064 CLPS block; both are kind:load pointers. */
extern CLPS_Block data_ov064_0211bb4c;

ResourceDescriptor data_ov022_02113da4 = {
    &gFlKomaDModelFile,
    &gFlKomaDCollisionFile,
    &data_ov064_0211bb4c,
};

extern "C" SpawnInfo<daObjFl_Koma_D_c> g_profile_FL_KOMA_D = {daObjFl_Koma_D_c_classInit,
                                                              80,
                                                              288,
                                                              0x00000002,
                                                              0x00060000,
                                                              0x00380000,
                                                              0x02000000,
                                                              0x00000000};
