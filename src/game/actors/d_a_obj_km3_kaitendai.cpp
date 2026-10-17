//cpp
/**
 * daKpa_c in the Sky's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers. Start angle is data_ov047_02112320, or
 * data_ov047_02112324 when param1's low byte is 1.
 * func_ov002_020b676c loads slot 0 with Model::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. This TU's static
 * initializer constructs those SharedFilePtrs as file IDs 1661 / 1662.
 *
 * The three-word file table is defined in this TU. Retail .data
 * order is typeinfo, descriptor, type-name, profile, vtable.
 *
 * daObjKm3_Kaitendai_c_classInit is reconstructed (RTTI
 * daObjKm3_Kaitendai_c, KM3_KAITENDAI registry). Retail does not
 * store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b676c / func_ov002_020b66a8 are still
 *   linker names. Naming belongs in ov002.
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov047_*. The two start-angle halfwords 02112320 / 02112324
 *   sit 8 bytes before this TU's claimed .data (typeinfo at
 *   02112328); they are not owned here.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km3_kaitendai_AssetId {
    ASSET_KM3_KAITENDAI_BMD = 0x067d,
    ASSET_KM3_KAITENDAI_KCL = 0x067e
};

#include "SharedFilePtr.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct Km3KaitendaiModelFilePtr : SharedFilePtr {
    unsigned int words[2];

    Km3KaitendaiModelFilePtr(unsigned int fileID);
    ~Km3KaitendaiModelFilePtr();
};

struct Km3KaitendaiClsnFilePtr : SharedFilePtr {
    unsigned int words[2];

    Km3KaitendaiClsnFilePtr(unsigned int fileID);
    ~Km3KaitendaiClsnFilePtr();
};

typedef char Km3KaitendaiModelFilePtr_size_must_be_8[
    sizeof(Km3KaitendaiModelFilePtr) == 8 ? 1 : -1];
typedef char Km3KaitendaiClsnFilePtr_size_must_be_8[
    sizeof(Km3KaitendaiClsnFilePtr) == 8 ? 1 : -1];

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *kcl;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern Km3KaitendaiModelFilePtr data_ov047_02112610;
extern Km3KaitendaiClsnFilePtr data_ov047_02112608;
extern CLPS_Block data_ov047_02111bf4;
}

/* Defined here because this TU owns the descriptor in overlay .data. */
extern "C" ResourceDescriptor data_ov047_02112334 = {
    &data_ov047_02112610,
    &data_ov047_02112608,
    &data_ov047_02111bf4
};

#include "daObjKm3_Kaitendai_c.h"
#include "SpawnInfo.h"

extern "C" {
s32 func_ov002_020b676c(daObjKaitendai_c *self, ResourceDescriptor *descriptor,
                        s16 angle);
s32 func_ov002_020b66a8(daObjKaitendai_c *self, ResourceDescriptor *descriptor);
extern s16 data_ov047_02112320;
extern s16 data_ov047_02112324;
}

// @symbol daObjKm3_Kaitendai_c_classInit
extern "C" daObjKm3_Kaitendai_c *daObjKm3_Kaitendai_c_classInit()
{
    return new daObjKm3_Kaitendai_c();
}

// @symbol g_profile_KM3_KAITENDAI
extern "C" SpawnInfo<daObjKm3_Kaitendai_c> g_profile_KM3_KAITENDAI = {
    daObjKm3_Kaitendai_c_classInit,
    0x009c, /* KM3_KAITENDAI registry id 156 */
    0x00e3,
    2,
    0,
    0x00380000,
    0x02000000,
    0
};

// @symbol _ZN20daObjKm3_Kaitendai_c13InitResourcesEv
s32 daObjKm3_Kaitendai_c::InitResources()
{
    s16 angle = data_ov047_02112320;
    if ((param1 & 0xff) == 1)
        angle = data_ov047_02112324;
    return func_ov002_020b676c(this, &data_ov047_02112334, angle);
}

// @symbol _ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv
s32 daObjKm3_Kaitendai_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov047_02112334);
}

/* Source order is construction order: model file 1661, then the collision
 * file 1662. The compiler registers each destructor beside the object. */
Km3KaitendaiModelFilePtr data_ov047_02112610(ASSET_KM3_KAITENDAI_BMD);
Km3KaitendaiClsnFilePtr data_ov047_02112608(ASSET_KM3_KAITENDAI_KCL);
