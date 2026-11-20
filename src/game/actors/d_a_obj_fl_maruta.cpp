//cpp
/**
 * Lethal Lava Land's rolling log (daObjFlMaruta_c).
 *
 * .text 0x02112498..0x021125e0. The volcano flame is the next unit,
 * src/game/actors/d_a_obj_volcano_cannon.cpp, and does not share a function
 * with this one. The 0x80-byte vtable ends at 0x02114424; the two words
 * there are the flame's state handlers, not slots of this vtable.
 *
 * _ZTS15daObjFlMaruta_c 0x0211436c. _ZTI15daObjFlMaruta_c 0x02114344,
 * one base daObjMaruta_c at 0x02114350. _ZTV15daObjFlMaruta_c 0x021143a4.
 * Key function is the out-of-line destructor: D1 at 0x02112498, D0 at
 * 0x021124e8, under defer_codegen off. D2 is not in the ROM.
 */

#include "SpawnInfo.h"
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"

#include "daObjFlMaruta_c.h"
#include "SharedFilePtr.h"
#include "ResourceDescriptor.h"

extern CLPS_Block data_ov064_0211bbcc;
extern "C" const int data_ov022_02112c98[];
extern "C" const ResourceDescriptor data_ov022_02112c9c;

#pragma defer_codegen off

/* One out-of-line definition; D1 and D0 in cartridge order. THREE vptr
 * stores: its own, daObjMaruta_c's inlined, then dBgActor_c's, then
 * dBgActor_c's dBgW_KcMbg and dExtSimpleModel_c, then dActor_c. */
daObjFlMaruta_c::~daObjFlMaruta_c()
{
}

int daObjFlMaruta_c::CleanupResources()
{
    return this->CleanupResourcesFrom((ResourceDescriptor *)&data_ov022_02112c9c);
}

int daObjFlMaruta_c::Behavior()
{
    mMeshCollider.SetClipRadius(0x500000);
    return this->func_ov080_0212714c(data_ov022_02112c98) & 0xff;
}

int daObjFlMaruta_c::InitResources()
{
    return this->InitResourcesFrom((ResourceDescriptor *)&data_ov022_02112c9c);
}

daObjFlMaruta_c *daObjFlMaruta_c_classInit(void) {
    return new daObjFlMaruta_c;
}

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x05fb, collision file 0x05fc). Those definitions
 * are this file's static initializer. The wrapper names are local; the
 * handle constructors and destructors are the ROM resource-family
 * functions, aliased in the manifest. */
struct FlLogModelFile : SharedFilePtr {
    FlLogModelFile(u32 fileID);
    ~FlLogModelFile();
};

struct FlLogCollisionFile : SharedFilePtr {
    FlLogCollisionFile(u32 fileID);
    ~FlLogCollisionFile();
};

/* Definition order is the retail initializer's construction order. */
FlLogModelFile gFlLogModelFile(0x05fb);
FlLogCollisionFile gFlLogCollisionFile(0x05fc);
extern "C" {
extern const int data_ov022_02112c98[1] = {0x7d0000};
extern const ResourceDescriptor data_ov022_02112c9c = {&gFlLogModelFile, &gFlLogCollisionFile,
                                                       &data_ov064_0211bbcc};
}

extern "C" SpawnInfo<daObjFlMaruta_c> g_profile_FL_MARUTA = {
    daObjFlMaruta_c_classInit, 70, 278, 0x00000002, 0x00000000, 0x00500000, 0x02000000, 0x00000000};
