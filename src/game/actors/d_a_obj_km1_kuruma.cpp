//cpp
/**
 * daKpa_c in the Dark World's rickshaw cart (`kuruma`).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKuruma_c's shared ov002 helpers.
 * func_ov002_020b6958 loads slot 0 with Model::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov043 sinit
 * constructs those SharedFilePtrs as file IDs 1619 / 1620.
 *
 * daObjKm1_Kuruma_c_classInit is reconstructed (RTTI daObjKm1_Kuruma_c,
 * KM1_KURUMA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6958 / func_ov002_020b68b0 are still the
 *   linker names of daObjKuruma_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: data_ov043_02112418 points at the two handles below and at
 *   data_ov043_02111b80. symbols.txt names the middle word
 *   data_ov043_0211241c.
 * Leftover: data_ov043_02111b80, the CLPS block this row points at
 *   (0x02111b80..0x02111ba0; the symbols.txt row names only the 8-byte
 *   header), stays the ROM copy. A TU's .data emits as one contribution
 *   and the manifest claims one contiguous range per input section, so
 *   this file's .data slot is the descriptor at 0x02112418; a second
 *   disjoint .data claim cannot be expressed, and input-section
 *   retargeting is unimplemented for enrolled intact objects. The block
 *   sits in a pooled run of sibling CLPS blocks at the head of .data
 *   that other km1 files' descriptors also point at.
 * Leftover: g_profile_KM1_KURUMA is overlay data; its definition here
 *   is a deadstripped duplicate.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km1_kuruma_AssetId {
    ASSET_KM1_KURUMA_BMD = 0x0653,
    ASSET_KM1_KURUMA_KCL = 0x0654
};

#include "daObjKm1_Kuruma_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x653, collision file 0x654). mwcc emits
 * __sinit_d_a_obj_km1_kuruma.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. Nothing in this TU
 * names the handles directly; the ResourceDescriptor row above points at
 * them. */
struct Km1KurumaModelFile : SharedFilePtr {

    Km1KurumaModelFile(u32 fileID);
    ~Km1KurumaModelFile();
};

struct Km1KurumaCollisionFilePtr : SharedFilePtr {

    Km1KurumaCollisionFilePtr(u32 fileID);
    ~Km1KurumaCollisionFilePtr();
};

extern "C" {
int func_ov002_020b6958(daObjKuruma_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b68b0(daObjKuruma_c *self, ResourceDescriptor *descriptor);
extern SharedFilePtr *data_ov043_02112418;
}

struct KurumaSpawnInfo {
    daObjKm1_Kuruma_c *(*classInit)();
    s16 executePriority; /* +4: also KM1_KURUMA registry id 0x0088 = 136 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char KurumaSpawnInfo_size_must_be_0x1c[
    sizeof(KurumaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daObjKm1_Kuruma_c_classInit
extern "C" daObjKm1_Kuruma_c *daObjKm1_Kuruma_c_classInit()
{
    return new daObjKm1_Kuruma_c();
}

// @symbol g_profile_KM1_KURUMA
extern "C" KurumaSpawnInfo g_profile_KM1_KURUMA = {
    daObjKm1_Kuruma_c_classInit,
    0x0088,
    0x00b1,
    2,
    0,
    0x00200000,
    0x01000000,
    0
};

// @symbol _ZN17daObjKm1_Kuruma_c13InitResourcesEv
s32 daObjKm1_Kuruma_c::InitResources()
{
    return func_ov002_020b6958(this, (ResourceDescriptor *)&data_ov043_02112418);
}

// @symbol _ZN17daObjKm1_Kuruma_c16CleanupResourcesEv
s32 daObjKm1_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0(this, (ResourceDescriptor *)&data_ov043_02112418);
}

/* Static-init globals (was the handwritten __sinit_ov043_021118d4 shard).
 * Definition order is the retail initializer's construction order. */
Km1KurumaModelFile gKm1KurumaModelFile(ASSET_KM1_KURUMA_BMD);
Km1KurumaCollisionFilePtr gKm1KurumaCollisionFile(ASSET_KM1_KURUMA_KCL);

/* symbols.txt names the middle word. The row is still those three pointers. */
extern CLPS_Block data_ov043_02111b80;
extern "C" SharedFilePtr *data_ov043_02112418 = &gKm1KurumaModelFile;
extern "C" struct {
    SharedFilePtr *collision;
    CLPS_Block *clps;
} data_ov043_0211241c = {
    &gKm1KurumaCollisionFile,
    &data_ov043_02111b80,
};
