//cpp
/* Genuine-TU reconstruction for ov044/daObjKb1Billboard_c.
 *
 * SM64DS preserves the daObjKb1Billboard_c RTTI class and the KB1_BILLBOARD
 * registry ID. daObjKb1Billboard_c_classInit, g_profile_KB1_BILLBOARD, and
 * this probable Nintendo-style filename are lineage-supported reconstructions;
 * their exact original SM64DS spellings do not survive. Historical project
 * aliases are OrangeBallBillboard_Spawn and OrangeBallBillboard_SpawnInfo.
 * UpdateModelTransform is an inferred private name; its ownership, body, call
 * site, ordering, and relocations are direct binary evidence.
 *
 * mwccarm emits function sections in reverse source order. Keep the factory
 * first and UpdateModelTransform last. InitResources is the key function; the
 * inline destructor in the class header lets the compiler emit retail D1 then
 * D0 together with this class's RTTI and vtable.
 */

#include "daObjKb1Billboard_c.h"
#include "SharedFilePtr.h"

struct Kb1BillboardSpawnInfo {
    daObjKb1Billboard_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char Kb1BillboardSpawnInfo_size_must_be_0x1c[
    sizeof(Kb1BillboardSpawnInfo) == 0x1c ? 1 : -1];

extern int _ZTV19daObjKb1Billboard_c[];

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void *);
extern void _ZN5ModelC1Ev(Model *);
extern void Matrix4x3_FromRotationY(void *, s16);
}

/* The retail static initializer constructs this 8-byte model resource handle
 * and registers its destructor. The wrapper spelling is reconstructed; the
 * constructor/destructor destinations, file ID, size, and BSS order are ROM
 * evidence. The manifest maps these undefined members to the recovered engine
 * symbols without claiming that the wrapper name was original. */
struct Kb1BillboardModelFilePtr : SharedFilePtr {
    u32 words[2];

    Kb1BillboardModelFilePtr(u32 fileID);
    ~Kb1BillboardModelFilePtr();
};

extern "C" Kb1BillboardModelFilePtr data_ov044_02111680;
Kb1BillboardModelFilePtr data_ov044_02111680(1570);

extern "C" daObjKb1Billboard_c *daObjKb1Billboard_c_classInit();

/* ROM ordinal 6: class initializer, 0x021112dc, size 0x38. */
// @symbol daObjKb1Billboard_c_classInit
extern "C" daObjKb1Billboard_c *daObjKb1Billboard_c_classInit() {
    daObjKb1Billboard_c *actor =
        (daObjKb1Billboard_c *)_ZN7fBase_cnwEj(sizeof(daObjKb1Billboard_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV19daObjKb1Billboard_c[2];
        _ZN5ModelC1Ev(&actor->mModel);
    }
    return actor;
}

extern "C" Kb1BillboardSpawnInfo g_profile_KB1_BILLBOARD = {
    daObjKb1Billboard_c_classInit,
    0x012d,
    0x00be,
    0,
    0,
    0x00010000,
    0,
    0
};

/* ROM ordinal 5: InitResources, 0x021112a0, size 0x3c. */
// @symbol _ZN19daObjKb1Billboard_c13InitResourcesEv
int daObjKb1Billboard_c::InitResources() {
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov044_02111680);
    mModel.SetFile(file, 1, -1);
    UpdateModelTransform();
    return 1;
}

/* ROM ordinal 4: Render, 0x02111278, size 0x28. */
// @symbol _ZN19daObjKb1Billboard_c6RenderEv
int daObjKb1Billboard_c::Render() {
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 3: CleanupResources, 0x02111254, size 0x24. */
// @symbol _ZN19daObjKb1Billboard_c16CleanupResourcesEv
int daObjKb1Billboard_c::CleanupResources() {
    data_ov044_02111680.Release();
    return 1;
}

/* ROM ordinal 2: inferred UpdateModelTransform, 0x02111214, size 0x40. */
// @symbol _ZN19daObjKb1Billboard_c20UpdateModelTransformEv
void daObjKb1Billboard_c::UpdateModelTransform() {
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}
