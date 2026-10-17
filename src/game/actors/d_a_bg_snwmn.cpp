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
 *   read the BMD/BTP at gdaBgSnwmnBigSnowmanHeadModelFile / 02122c50 +4;
 *   SharedFilePtr.h has no fields.
 * - gdaBgSnwmnBigSnowmanBodyModelFile / 02122c48 / 02122c50 model/BTP handles and
 *   data_ov072_02122c70 cylinder offset are defined at the end of this file.
 *   Their ctors are what emit __sinit_d_a_bg_snwmn.cpp.
 * - InitResources ground probe stays a POD Position: a local Vector3
 *   emits unlicensed _ZN7Vector3D1Ev (empty dtor, size 0x4).
 * - func_ov072_021208d8 keeps its address label -- the ROM has no English
 *   name for it.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daBgSnwmn_c_AssetId {
    ASSET_BIG_SNOWMAN_BODY_BMD = 0x02ad,
    ASSET_BIG_SNOWMAN_HEAD_BMD = 0x02af,
    ASSET_BIG_SNOWMAN_SMILE_BTP = 0x02b0
};

#include "daBgSnwmn_c.h"
#include "SpawnInfo.h"
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
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    void *self, dExtShadowModel_c *shadow, Matrix4x3 *matrix, int radius,
    int height, u32 flags);

}

/* Four file-scope objects. mwcc emits __sinit_d_a_bg_snwmn.cpp from them.
 * The two model handles share one veneer pair, the BTP handle is the texseq
 * pair, and the cylinder offset's stores are inline. */
struct SnwmnModelFilePtr : SharedFilePtr {
    u32 words[2];

    SnwmnModelFilePtr(u32 fileID);
    ~SnwmnModelFilePtr();
};

struct SnwmnTexSequenceFilePtr : SharedFilePtr {
    u32 words[2];

    SnwmnTexSequenceFilePtr(u32 fileID);
    ~SnwmnTexSequenceFilePtr();
};

struct SnwmnCylOffset : Vector3 {
    SnwmnCylOffset(Fix12i x_, Fix12i y_, Fix12i z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    ~SnwmnCylOffset();
};

extern "C" SnwmnModelFilePtr gdaBgSnwmnBigSnowmanBodyModelFile;
extern "C" SnwmnModelFilePtr gdaBgSnwmnBigSnowmanHeadModelFile;
extern "C" SnwmnTexSequenceFilePtr gdaBgSnwmnBigSnowmanSmileTextureSequenceFile;
extern "C" SnwmnCylOffset data_ov072_02122c70;

/* Reconstructed source-style names. SM64DS directly preserves the class RTTI,
 * BIG_SNOWMAN ID, descriptor relationship, and factory behavior. */
// @symbol daBgSnwmn_c_classInit
extern "C" daBgSnwmn_c *daBgSnwmn_c_classInit()
{
    return new daBgSnwmn_c();
}

extern "C" SpawnInfo<daBgSnwmn_c> g_profile_BIG_SNOWMAN = {
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
        dActor_c::Spawn(0x111, 0, mPos, &mAngle, mAreaId, -1);
        MarkForDestruction();
    }

    model = Model::LoadFile(gdaBgSnwmnBigSnowmanHeadModelFile);
    mModel1.SetFile((BMD_File *)model, 1, 1);
    model = Model::LoadFile(gdaBgSnwmnBigSnowmanBodyModelFile);
    mModel2.SetFile((BMD_File *)model, 1, 1);

    TextureSequence::LoadFile(gdaBgSnwmnBigSnowmanSmileTextureSequenceFile);
    TextureSequence::Prepare(**(BMD_File **)((char *)&gdaBgSnwmnBigSnowmanHeadModelFile + 4),
                             **(BTP_File **)((char *)&gdaBgSnwmnBigSnowmanSmileTextureSequenceFile + 4));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTexSeq, *(void **)((char *)&gdaBgSnwmnBigSnowmanSmileTextureSequenceFile + 4), 0, 0x1000, 0);

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
    func_ov072_021208d8();
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
    mModel1.Render(&mScale);
    mModel2.Render(&mScale);
    return 1;
}

// @symbol _ZN11daBgSnwmn_c16OnPendingDestroyEv
void daBgSnwmn_c::OnPendingDestroy()
{
}

// @symbol _ZN11daBgSnwmn_c16CleanupResourcesEv
s32 daBgSnwmn_c::CleanupResources()
{
    gdaBgSnwmnBigSnowmanHeadModelFile.Release();
    gdaBgSnwmnBigSnowmanBodyModelFile.Release();
    gdaBgSnwmnBigSnowmanSmileTextureSequenceFile.Release();
    return 1;
}

// @symbol _ZN11daBgSnwmn_c19func_ov072_021208d8Ev
void daBgSnwmn_c::func_ov072_021208d8()
{
    Matrix4x3_FromRotationY(&mModel1.mat4x3, mAngleY);
    mModel1.mat4x3.t.x = mPosX >> 3;
    mModel1.mat4x3.t.y = (mPosY + 0x13b000) >> 3;
    mModel1.mat4x3.t.z = mPosZ >> 3;

    Matrix4x3_FromRotationY(&mModel2.mat4x3, mAngleY);
    mModel2.mat4x3.t.x = mPosX >> 3;
    mModel2.mat4x3.t.y = mPosY >> 3;
    mModel2.mat4x3.t.z = mPosZ >> 3;

    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadow, &mModel2.mat4x3, 0xe6000, 0x12c000, 0xf);
}

SnwmnModelFilePtr gdaBgSnwmnBigSnowmanBodyModelFile(ASSET_BIG_SNOWMAN_BODY_BMD);
SnwmnModelFilePtr gdaBgSnwmnBigSnowmanHeadModelFile(ASSET_BIG_SNOWMAN_HEAD_BMD);
SnwmnTexSequenceFilePtr gdaBgSnwmnBigSnowmanSmileTextureSequenceFile(ASSET_BIG_SNOWMAN_SMILE_BTP);
SnwmnCylOffset data_ov072_02122c70(0, -0x50000, 0);
