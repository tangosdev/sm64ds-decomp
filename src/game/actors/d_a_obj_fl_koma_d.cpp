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
// - data_ov022_02113da4 is the model/collision/CLPS row between typeinfo
//   at 02113d98 and the type name at 02113db0. Not owned here. The two
//   handles it points at are defined at the bottom of this file;
//   __sinit_d_a_obj_fl_koma_d.cpp constructs them.
// - g_profile_FL_KOMA_D lives outside this TU.
// - #pragma defer_codegen off stays on line 2. Dropping it still matches
//   each body, but the four functions emit in reverse ROM order.
// - An inline empty destructor still matches D1 and D0, but the ordinal
//   pair flips, D2 is not emitted, and 13 RTTI/vtable symbols come out
//   unlicensed.

#include "daObjFl_Koma_D_c.h"
#include "SharedFilePtr.h"

/* This descriptor matches the shared helpers in d_a_obj_kaitendai.cpp. */

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern ResourceDescriptor data_ov022_02113da4;

// @symbol _ZN16daObjFl_Koma_D_cD1Ev
// @symbol _ZN16daObjFl_Koma_D_cD0Ev
daObjFl_Koma_D_c::~daObjFl_Koma_D_c() {}

// @symbol _ZN16daObjFl_Koma_D_c16CleanupResourcesEv
int daObjFl_Koma_D_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov022_02113da4);
}

// @symbol _ZN16daObjFl_Koma_D_c13InitResourcesEv
int daObjFl_Koma_D_c::InitResources() {
    return InitResourcesFrom(&data_ov022_02113da4, 0x100);
}

// @symbol daObjFl_Koma_D_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjFl_Koma_D_c through
 * RTTI, allocation size, most-derived vtable identity, and the FL_KOMA_D
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. Historical alias: RotatingPlatformLll_Spawn.
 *
 * `new daObjFl_Koma_D_c` is the whole sequence the loose factory spelled
 * by hand: fBase_c::operator new(0x320), dBgActor_c's base constructor,
 * then daObjKaitendai_c's vptr store and this class's own. */
extern "C" daObjFl_Koma_D_c *daObjFl_Koma_D_c_classInit(void) {
    return new daObjFl_Koma_D_c;
}

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
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

#ifdef _MSC_VER
/* Host flat names. Not compiled into the cartridge object. */
extern "C" daObjFl_Koma_D_c *_ZN16daObjFl_Koma_D_cD0Ev(daObjFl_Koma_D_c *thiz) {
    thiz->daObjFl_Koma_D_c::~daObjFl_Koma_D_c();
    daObjFl_Koma_D_c::operator delete(thiz);
    return thiz;
}

extern "C" daObjFl_Koma_D_c *_ZN19RotatingPlatformLllD0Ev(daObjFl_Koma_D_c *thiz) {
    delete thiz;
    return thiz;
}
#endif
