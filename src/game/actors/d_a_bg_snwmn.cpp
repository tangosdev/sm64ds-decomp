//cpp
/* ov072/daBgSnwmn_c -- BIG_SNOWMAN (272), the snowman manager/head assembly.
 *
 * ov072 is mixed (BABY_PENGUIN / BIG_SNOWMAN / SNOWMAN_HEAD / SNOWMAN_BODY).
 * RTTI names this class daBgSnwmn_c; the debug table names BIG_SNOWMAN.
 * This is the manager, not daBgSnmBdy_c (body) and not the penguin.
 *
 * Notes:
 * - InitResources' dExtAnmTexPat_c::Prepare / SetFile read the BMD/BTP
 *   through gdaBgSnwmnBigSnowmanHeadModelFile / the smile BTP handle's filePtr.
 * - The two model handles, the smile BTP handle and data_ov072_02122c70
 *   (the cylinder offset) are defined at the end of this file. Their ctors
 *   are what emit __sinit_d_a_bg_snwmn.cpp.
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

#include "math/MtxStack.h"

/* arm9 C helper with no shared header. */
extern "C" int IsStarCollectedInLevel(s8 levelID, int starID);

/* Four file-scope objects. mwcc emits __sinit_d_a_bg_snwmn.cpp from them.
 * The two model handles share one veneer pair, the BTP handle is the texseq
 * pair, and the cylinder offset's stores are inline. */
struct SnwmnModelFilePtr : SharedFilePtr {
    SnwmnModelFilePtr(u32 fileID);
    ~SnwmnModelFilePtr();
};

struct SnwmnTexSequenceFilePtr : SharedFilePtr {
    SnwmnTexSequenceFilePtr(u32 fileID);
    ~SnwmnTexSequenceFilePtr();
};

extern SnwmnModelFilePtr gdaBgSnwmnBigSnowmanBodyModelFile;
extern SnwmnModelFilePtr gdaBgSnwmnBigSnowmanHeadModelFile;
extern SnwmnTexSequenceFilePtr gdaBgSnwmnBigSnowmanSmileTextureSequenceFile;
extern Vector3 data_ov072_02122c70;

/* Reconstructed source-style names. SM64DS directly preserves the class RTTI,
 * BIG_SNOWMAN ID, descriptor relationship, and factory behavior. */
// @symbol daBgSnwmn_c_classInit
extern "C" daBgSnwmn_c *daBgSnwmn_c_classInit() {
    return new daBgSnwmn_c();
}

extern "C" SpawnInfo<daBgSnwmn_c> g_profile_BIG_SNOWMAN = {
    daBgSnwmn_c_classInit, 0x0110, 0x0083, 3, 1, 0x0012c000, 0x01000000, 0x00001000};

// @symbol _ZN11daBgSnwmn_c13InitResourcesEv
s32 daBgSnwmn_c::InitResources() {
    Vector3 pos;
    void *model;

    if (IsStarCollectedInLevel(0xa, 5) == 0) {
        dActor_c::Spawn(0x111, 0, mPos, &mAngle, mAreaId, -1);
        MarkForDestruction();
    }

    model = dExtSimpleModel_c::LoadFile(gdaBgSnwmnBigSnowmanHeadModelFile);
    mModel1.SetFile((BMD_File *)model, 1, 1);
    model = dExtSimpleModel_c::LoadFile(gdaBgSnwmnBigSnowmanBodyModelFile);
    mModel2.SetFile((BMD_File *)model, 1, 1);

    dExtAnmTexPat_c::LoadFile(gdaBgSnwmnBigSnowmanSmileTextureSequenceFile);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)gdaBgSnwmnBigSnowmanHeadModelFile.filePtr,
                             *(BTP_File *)gdaBgSnwmnBigSnowmanSmileTextureSequenceFile.filePtr);
    mTexSeq.SetFile(*(BTP_File *)gdaBgSnwmnBigSnowmanSmileTextureSequenceFile.filePtr, 0, 0x1000,
                    0);

    if (mShadow.InitCylinder() == 0)
        return 0;

    mCylClsn.Init(this, data_ov072_02122c70, 0xc3000, 0x17c000, 0x800004, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
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
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN11daBgSnwmn_c8BehaviorEv
s32 daBgSnwmn_c::Behavior() {
    mTexSeq.Advance();
    mCylClsn.SetPosRelativeToActor(data_ov072_02122c70);
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

// @symbol _ZN11daBgSnwmn_c6RenderEv
s32 daBgSnwmn_c::Render() {
    mTexSeq.Update(mModel1.data);
    mModel1.Render(&mScale);
    mModel2.Render(&mScale);
    return 1;
}

// @symbol _ZN11daBgSnwmn_c16OnPendingDestroyEv
void daBgSnwmn_c::OnPendingDestroy() {}

// @symbol _ZN11daBgSnwmn_c16CleanupResourcesEv
s32 daBgSnwmn_c::CleanupResources() {
    gdaBgSnwmnBigSnowmanHeadModelFile.Release();
    gdaBgSnwmnBigSnowmanBodyModelFile.Release();
    gdaBgSnwmnBigSnowmanSmileTextureSequenceFile.Release();
    return 1;
}

// @symbol _ZN11daBgSnwmn_c20UpdateModelTransformEv
void daBgSnwmn_c::UpdateModelTransform() {
    Matrix4x3_FromRotationY(&mModel1.mat4x3, mAngleY);
    mModel1.mat4x3.t.x = mPosX >> 3;
    mModel1.mat4x3.t.y = (mPosY + 0x13b000) >> 3;
    mModel1.mat4x3.t.z = mPosZ >> 3;

    Matrix4x3_FromRotationY(&mModel2.mat4x3, mAngleY);
    mModel2.mat4x3.t.x = mPosX >> 3;
    mModel2.mat4x3.t.y = mPosY >> 3;
    mModel2.mat4x3.t.z = mPosZ >> 3;

    DropShadowRadHeight(mShadow, mModel2.mat4x3, 0xe6000, 0x12c000, 0xf);
}

SnwmnModelFilePtr gdaBgSnwmnBigSnowmanBodyModelFile(ASSET_BIG_SNOWMAN_BODY_BMD);
SnwmnModelFilePtr gdaBgSnwmnBigSnowmanHeadModelFile(ASSET_BIG_SNOWMAN_HEAD_BMD);
SnwmnTexSequenceFilePtr gdaBgSnwmnBigSnowmanSmileTextureSequenceFile(ASSET_BIG_SNOWMAN_SMILE_BTP);
Vector3 data_ov072_02122c70(0, -0x50000, 0);
