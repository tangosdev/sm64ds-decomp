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
 * deslop
 * Leftover: func_ov002_020bad10 / func_ov002_020baba8 / func_ov002_020bac18
 *   are still the linker names of daObjSwdoor_c setup / teardown / step
 *   (the base leaves those slots pure virtual). Naming belongs in ov002.
 *   The setup helper loads data_ov014_021145c4 slot 0 with Model::LoadFile,
 *   slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, and writes
 *   mTimer / mEventBit from param1.
 * Leftover: data_ov014_021145c4 is overlay data; this TU is text-only so
 *   the table stays an extern. g_profile_SWITCHDOOR is the same.
 */

#include "daObjBSwdoor_c.h"
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
int func_ov002_020bad10(daObjBSwdoor_c *self, ResourceDescriptor *descriptor);
int func_ov002_020baba8(daObjBSwdoor_c *self, ResourceDescriptor *descriptor);
int func_ov002_020bac18(daObjBSwdoor_c *self);
extern ResourceDescriptor data_ov014_021145c4;
}

// @symbol daObjBSwdoor_c_classInit
extern "C" daObjBSwdoor_c *daObjBSwdoor_c_classInit()
{
    return new daObjBSwdoor_c();
}

// @symbol _ZN14daObjBSwdoor_c13InitResourcesEv
s32 daObjBSwdoor_c::InitResources()
{
    s32 r = func_ov002_020bad10(this, &data_ov014_021145c4);
    mMeshCollider.Enable(this);
    return r;
}

// @symbol _ZN14daObjBSwdoor_c8BehaviorEv
s32 daObjBSwdoor_c::Behavior()
{
    s32 r = func_ov002_020bac18(this);
    UpdateClsnPosAndRot();
    return r;
}

// @symbol _ZN14daObjBSwdoor_c16CleanupResourcesEv
/* Cross-overlay tail-call. #pragma long_calls is positional in 2004/b56. */
#pragma long_calls on
s32 daObjBSwdoor_c::CleanupResources()
{
    return func_ov002_020baba8(this, &data_ov014_021145c4);
}
#pragma long_calls off
