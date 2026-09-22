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
 * deslop
 * Leftover: func_ov002_020bad10 / func_ov002_020baba8 / func_ov002_020bac18
 *   are still the linker names of daObjSwdoor_c setup / teardown / step
 *   (the base leaves those slots pure virtual). Naming belongs in ov002.
 *   The setup helper loads data_ov021_021148d0 slot 0 with Model::LoadFile,
 *   slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, and writes
 *   mTimer / mEventBit from param1.
 * Leftover: data_ov021_021148d0 is overlay data; this TU is text-only so
 *   the table stays an extern. g_profile_CV_SHUTTER is the same.
 * Leftover: IsClsnInRange stays mangled (by-value Fix12<int>, wall 6az).
 */

#include "daObjCvShutter_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int func_ov002_020bad10(daObjCvShutter_c *self, ResourceDescriptor *descriptor);
int func_ov002_020baba8(daObjCvShutter_c *self, ResourceDescriptor *descriptor);
int func_ov002_020bac18(daObjCvShutter_c *self);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern ResourceDescriptor data_ov021_021148d0;
}

// @symbol daObjCvShutter_c_classInit
extern "C" daObjCvShutter_c *daObjCvShutter_c_classInit()
{
    return new daObjCvShutter_c();
}

// @symbol _ZN16daObjCvShutter_c13InitResourcesEv
s32 daObjCvShutter_c::InitResources()
{
    return func_ov002_020bad10(this, &data_ov021_021148d0);
}

// @symbol _ZN16daObjCvShutter_c8BehaviorEv
s32 daObjCvShutter_c::Behavior()
{
    s32 r = func_ov002_020bac18(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return r;
}

// @symbol _ZN16daObjCvShutter_c16CleanupResourcesEv
s32 daObjCvShutter_c::CleanupResources()
{
    return func_ov002_020baba8(this, &data_ov021_021148d0);
}
