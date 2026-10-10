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

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct BSwdoorModelFilePtr : SharedFilePtr {
    unsigned int words[2];

    BSwdoorModelFilePtr(unsigned int fileID);
    ~BSwdoorModelFilePtr();
};

struct BSwdoorClsnFilePtr : SharedFilePtr {
    unsigned int words[2];

    BSwdoorClsnFilePtr(unsigned int fileID);
    ~BSwdoorClsnFilePtr();
};

typedef char BSwdoorModelFilePtr_size_must_be_8[
    sizeof(BSwdoorModelFilePtr) == 8 ? 1 : -1];
typedef char BSwdoorClsnFilePtr_size_must_be_8[
    sizeof(BSwdoorClsnFilePtr) == 8 ? 1 : -1];

extern "C" {
int func_ov002_020bad10(daObjSwdoor_c *self, ResourceDescriptor *descriptor);
int func_ov002_020baba8(daObjSwdoor_c *self, ResourceDescriptor *descriptor);
int func_ov002_020bac18(daObjSwdoor_c *self);
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
/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjBSwdoor_cD0Ev, 0x021111f0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_cD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV14daObjBSwdoor_c; VT1 = _ZTV13daObjSwdoor_c */
/* (no separate definition: the single ~daObjBSwdoor_c() below emits the D0 and D1
 * variants together -- keeping the hand-mangled body alongside a real destructor
 * is the known mwccarm ICE, ELFgen.c:483.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN14daObjBSwdoor_cD1Ev, 0x021111a0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * (no definition here either: `virtual ~daObjBSwdoor_c() {}` is in
 * include/daObjBSwdoor_c.h, which is where the reasoning lives. Defined out of
 * line in this file, the pair came out D0-before-D1 -- the reverse of the
 * cartridge -- with a third, homeless D2, and objisolate refused the TU. The
 * inline definition emits both variants here, in ROM order, and no D2.)
 */

/* Source order is construction order: model file 1443, then the collision
 * file 1444. The compiler registers each destructor beside the object. */
BSwdoorModelFilePtr data_ov014_02114948(1443);
BSwdoorClsnFilePtr data_ov014_02114940(1444);
