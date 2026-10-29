//cpp
/**
 * Hazy Maze Cave's switch-operated shutter.
 *
 * InitResources / CleanupResources / Behavior hand this overlay's model
 * and collision files to daObjSwdoor_c's shared ov002 helpers. Behavior
 * then re-syncs collision when the mesh is in range.
 *
 * daObjCvShutter_c_classInit is reconstructed (RTTI daObjCvShutter_c,
 * CV_SHUTTER / SHUTTER_HMC registry). Retail does not store that spelling.
 *
 * Notes:
 * - daObjSwdoor_c::InitResourcesFrom / CleanupResourcesFrom / UpdateShutter
 *   are the base's setup / teardown / step (the base leaves those slots
 *   pure virtual), defined in d_a_obj_swdoor.cpp.
 *   The setup helper loads data_ov021_021148d0 slot 0 with dExtSimpleModel_c::LoadFile,
 *   slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, and writes
 *   daObjSwdoor_c's four byte fields from param1.
 * - data_ov021_021148d0 is overlay data; this TU is text-only so
 *   the table stays an extern. g_profile_CV_SHUTTER is the same.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_cv_shutter_AssetId { ASSET_CV_SHUTTER_BMD = 0x05d9, ASSET_CV_SHUTTER_KCL = 0x05da };

#include "daObjCvShutter_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern ResourceDescriptor data_ov021_021148d0;

/* File 1497 / 1498 resource handles. The static initializer registers
 * their destructors; the spellings are this TU's, mapped onto
 * func_02017acc / func_02017ab4 and func_02017b4c /
 * SharedFilePtr_Destruct_Clsn. */
struct CvShutterModelFilePtr : SharedFilePtr {
    CvShutterModelFilePtr(u32 fileID);
    ~CvShutterModelFilePtr();
};

struct CvShutterCollisionFilePtr : SharedFilePtr {
    CvShutterCollisionFilePtr(u32 fileID);
    ~CvShutterCollisionFilePtr();
};

// @symbol daObjCvShutter_c_classInit
extern "C" daObjCvShutter_c *daObjCvShutter_c_classInit() {
    return new daObjCvShutter_c();
}

// @symbol _ZN16daObjCvShutter_c13InitResourcesEv
s32 daObjCvShutter_c::InitResources() {
    return InitResourcesFrom(&data_ov021_021148d0);
}

// @symbol _ZN16daObjCvShutter_c8BehaviorEv
s32 daObjCvShutter_c::Behavior() {
    s32 r = UpdateShutter();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    return r;
}

// @symbol _ZN16daObjCvShutter_c16CleanupResourcesEv
s32 daObjCvShutter_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov021_021148d0);
}
// @symbol _ZN16daObjCvShutter_cD1Ev
// @symbol _ZN16daObjCvShutter_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjCvShutter_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are THREE vptr stores, and the middle one is the finding:
   `daObjCvShutter_c : daObjSwdoor_c : dBgActor_c` emits this class's vptr,
   then daObjSwdoor_c's -- inlined, because that destructor is defined in its
   class body -- then dBgActor_c's, then dBgActor_c's dBgW_KcMbg and dExtSimpleModel_c,
   then dActor_c. A one-level chain would emit two. This class adds no member
   with a destructor of its own, and D0's trailing deallocation is the inline
   `operator delete` it inherits, which is why nothing here names a heap. */

/* Definitions stay after the last .text function so they do not insert
 * a function into the ROM-ascending run. Construction order is the model
 * handle, then the collision handle. */
CvShutterModelFilePtr gCvShutterModelFile(ASSET_CV_SHUTTER_BMD);
CvShutterCollisionFilePtr gCvShutterCollisionFile(ASSET_CV_SHUTTER_KCL);
