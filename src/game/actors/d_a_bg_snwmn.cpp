//cpp
/* ov072/daBgSnwmn_c -- BIG_SNOWMAN (272), the snowman manager/head assembly.
 *
 * ov072 is mixed (BABY_PENGUIN / BIG_SNOWMAN / SNOWMAN_HEAD / SNOWMAN_BODY).
 * RTTI names this class daBgSnwmn_c; the debug table names BIG_SNOWMAN.
 * This is the manager, not daBgSnmBdy_c (body) and not the penguin.
 *
 * deslop leftovers:
 * - TextureSequence::SetFile / dCcAcPos_c::Init / DropShadowRadHeight 6az:
 *   InitResources and func_ov072_021208d8 pass Fix12<int> by value
 *   (notes/mwccarm-codegen.md 6az).
 * - SharedFilePtr +4: InitResources' TextureSequence::Prepare / SetFile
 *   read the BMD/BTP at data_ov072_02122c48 / 02122c50 +4;
 *   SharedFilePtr.h has no fields.
 * - data_ov072_02122c40 / 02122c48 / 02122c50 model/BTP handles and
 *   data_ov072_02122c70 cylinder offset; this TU consumes them, overlay
 *   .bss owns them.
 * - InitResources ground probe stays a POD Position: a local Vector3
 *   emits unlicensed _ZN7Vector3D1Ev (empty dtor, size 0x4).
 */

#include "daBgSnwmn_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

extern "C" {
extern int IsStarCollectedInLevel(s8 levelID, int starID);
void Matrix4x3_FromRotationY(void *, short);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void *self, void *textureFile, int animation, int speed, u32 flags);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, const void *offset, int radius, int height,
    u32 flags, u32 unk);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, ShadowModel *shadow, Matrix4x3 *matrix, int radius,
    int height, u32 flags);
void func_ov072_021208d8(daBgSnwmn_c *self);

extern SharedFilePtr data_ov072_02122c40;
extern SharedFilePtr data_ov072_02122c48;
extern SharedFilePtr data_ov072_02122c50;
extern const Vector3 data_ov072_02122c70;
}

/* The typed 0x1c actor profile: fBase_c reads the halfwords at +4/+6 as
 * behavior/render priorities. dActor_c reads actor flags at +8 and passes
 * the words at +0xc/+0x10/+0x14/+0x18 to SetRanges as clip offset Y, clip
 * radius, clip distance and far distance. */
struct SnwmnProfile {
    daBgSnwmn_c *(*classInit)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    u32 clipDistance;
    u32 farDistance;
};

typedef char SnwmnProfile_size_must_be_0x1c[
    sizeof(SnwmnProfile) == 0x1c ? 1 : -1];

/* Reconstructed source-style names. SM64DS directly preserves the class RTTI,
 * BIG_SNOWMAN ID, descriptor relationship, and factory behavior. */
// @symbol daBgSnwmn_c_classInit
extern "C" daBgSnwmn_c *daBgSnwmn_c_classInit()
{
    return new daBgSnwmn_c();
}

extern "C" SnwmnProfile g_profile_BIG_SNOWMAN = {
    daBgSnwmn_c_classInit,
    0x0110,
    0x0083,
    3,
    1,
    0x0012c000,
    0x01000000,
    0x00001000
};

// @symbol _ZN11daBgSnwmn_c13InitResourcesEv
s32 daBgSnwmn_c::InitResources()
{
    /* POD, not Vector3: a local Vector3 emits _ZN7Vector3D1Ev (empty dtor,
       size 0x4) as unlicensed EXTRA. */
    struct Position {
        s32 x;
        s32 y;
        s32 z;
    } pos;
    void *model;

    if (IsStarCollectedInLevel(0xa, 5) == 0) {
        dActor_c::Spawn(0x111, 0, *(Vector3 *)&mPosX, (Vector3_16 *)&mAngleX,
                        mAreaId, -1);
        MarkForDestruction();
    }

    model = Model::LoadFile(data_ov072_02122c48);
    mModel1.SetFile((BMD_File *)model, 1, 1);
    model = Model::LoadFile(data_ov072_02122c40);
    mModel2.SetFile((BMD_File *)model, 1, 1);

    TextureSequence::LoadFile(data_ov072_02122c50);
    TextureSequence::Prepare(**(BMD_File **)((char *)&data_ov072_02122c48 + 4),
                             **(BTP_File **)((char *)&data_ov072_02122c50 + 4));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTexSeq, *(void **)((char *)&data_ov072_02122c50 + 4), 0, 0x1000, 0);

    if (mShadow.InitCylinder() == 0)
        return 0;

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mCylClsn, this, &data_ov072_02122c70, 0xc3000, 0x17c000,
        0x800004, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(*reinterpret_cast<Vector3 *>(&pos), 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mPosY += 0xc3000;
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mScaleX = 0x1800;
    mScaleY = 0x1800;
    mScaleZ = 0x1800;
    func_ov072_021208d8(this);
    return 1;
}

// @symbol _ZN11daBgSnwmn_c8BehaviorEv
s32 daBgSnwmn_c::Behavior()
{
    mTexSeq.Advance();
    mCylClsn.SetPosRelativeToActor(data_ov072_02122c70);
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

// @symbol _ZN11daBgSnwmn_c6RenderEv
s32 daBgSnwmn_c::Render()
{
    mTexSeq.Update(mModel1.data);
    mModel1.Render((Vector3 *)&mScaleX);
    mModel2.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN11daBgSnwmn_c16OnPendingDestroyEv
void daBgSnwmn_c::OnPendingDestroy()
{
}

// @symbol _ZN11daBgSnwmn_c16CleanupResourcesEv
s32 daBgSnwmn_c::CleanupResources()
{
    data_ov072_02122c48.Release();
    data_ov072_02122c40.Release();
    data_ov072_02122c50.Release();
    return 1;
}

// @symbol func_ov072_021208d8
extern "C" void func_ov072_021208d8(daBgSnwmn_c *self)
{
    Matrix4x3_FromRotationY(&self->mModel1.mat4x3, self->mAngleY);
    self->mModel1.mat4x3.t.x = self->mPosX >> 3;
    self->mModel1.mat4x3.t.y = (self->mPosY + 0x13b000) >> 3;
    self->mModel1.mat4x3.t.z = self->mPosZ >> 3;

    Matrix4x3_FromRotationY(&self->mModel2.mat4x3, self->mAngleY);
    self->mModel2.mat4x3.t.x = self->mPosX >> 3;
    self->mModel2.mat4x3.t.y = self->mPosY >> 3;
    self->mModel2.mat4x3.t.z = self->mPosZ >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadow, &self->mModel2.mat4x3, 0xe6000, 0x12c000, 0xf);
}
