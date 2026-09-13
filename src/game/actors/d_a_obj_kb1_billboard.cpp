//cpp
/* ov044/daObjKb1Billboard_c -- orange ball billboard.
 *
 * RTTI ov044:0x021115bc/_ZTS at 0x021115c8 names daObjKb1Billboard_c.
 * overlay_actors: ORANGE_BALL_BILLBOARD(301). Profile g_profile_KB1_BILLBOARD.
 * ov044 is this actor only.
 *
 * mwccarm emits function sections in reverse source order. Keep the factory
 * first and UpdateModelTransform last. InitResources is the key function; the
 * inline destructor emits retail D1 then D0 with this class's RTTI and vtable.
 *
 * Leftover:
 * - data_ov044_02111680 model handle; SharedFilePtr.h has no fields
 * - Matrix4x3_FromRotationY stays a free function (no header method)
 * - pad_0d0 unused (not live)
 * - common.h not needed (no 12-word Matrix4x3 copy)
 * - no Player.h / Camera.h
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

extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angY);
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

/* Every instruction the cartridge has here falls out of the one `new`.
 * The header's inline operator new keeps the allocation on fBase_c::operator
 * new; without it the call relocates to the unavailable global `_Znwm`. */
// @symbol daObjKb1Billboard_c_classInit
extern "C" daObjKb1Billboard_c *daObjKb1Billboard_c_classInit()
{
    return new daObjKb1Billboard_c();
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

// @symbol _ZN19daObjKb1Billboard_c13InitResourcesEv
int daObjKb1Billboard_c::InitResources()
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov044_02111680);
    mModel.SetFile(file, 1, -1);
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN19daObjKb1Billboard_c6RenderEv
int daObjKb1Billboard_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN19daObjKb1Billboard_c16CleanupResourcesEv
int daObjKb1Billboard_c::CleanupResources()
{
    data_ov044_02111680.Release();
    return 1;
}

// @symbol _ZN19daObjKb1Billboard_c20UpdateModelTransformEv
void daObjKb1Billboard_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}
