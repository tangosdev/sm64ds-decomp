//cpp
/**
 * Tick Tock Clock rotating gear and triangle (CT_MECHA06 / CT_MECHA07,
 * actors 0x72 / 0x73).
 *
 * One class behind two profiles. InitResources loads the row of the
 * model / collision / clps table selected by the actor id, probes the
 * floor ten units under the spawn point, and hands the mesh
 * dBgW::UpdatePosAndAngs. Behavior is the cog. Clock setting 0 is the
 * slow hand (yaw speed 200), 1 the fast hand (400), 2 the random hand,
 * 3 the stopped hand. The speed lives in mPrevAngle.z, the random-hand
 * target in mPrevAngle.x, and the accumulated yaw in mPrevAngle.y, which
 * is copied to mAngle.y for the model and the mesh.
 *
 * The destructor pair comes off the inline ~daObjCtKaitendai_c() in the
 * header. Source order is reverse ROM order (2004/b56 emits .text back
 * to front); do not reorder.
 *
 * Notes:
 * - dBgW::SetBeforeClsnCallback is the store of dBgW::beforeClsnCallback (+0x18).
 *   This InitResources calls it with &dBgW::UpdatePosAndAngs.
 * - The sign flip and the yaw add are the compound forms (`*= -1`, `+=`).
 *   Writing mPrevAngle.x = mPrevAngle.x * -1 comes out 0x12c, and
 *   mPrevAngle.y = mPrevAngle.y + mPrevAngle.z comes out 0x134, against
 *   Behavior's 0x138.
 * - sTTCCogNormalSpeeds (the 200/400 speeds) and data_ov065_0211d35c
 *   (the two resource rows) are not this TU's data. Neither are
 *   g_profile_CT_MECHA06 / CT_MECHA07.
 * - The actor-id nest stays `!= 0x72` then `== 0x73`, and the floor
 *   probe subtracts 0xa000 on its own statement. Either an else-if or
 *   folding the subtract into the y copy comes out 0x13c against
 *   InitResources' 0x140.
 * - return new emits a homeless _ZN10dBgActor_cD2Ev. The manifest
 *   deadstrips it.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtKaitendai_c_AssetId {
    ASSET_CT_MECHA_OBJ06_BMD = 0x05c4,
    ASSET_CT_MECHA_OBJ06_KCL = 0x05c5,
    ASSET_CT_MECHA_OBJ07_BMD = 0x05c6,
    ASSET_CT_MECHA_OBJ07_KCL = 0x05c7
};

#include "common.h"
#include "daObjCtKaitendai_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "math/Approach.h"
#include "LevelRng.h"
#include "math/CalcLib.h"

struct CLPS_Block;

/* One row is {model, collision, clps}. InitResources indexes it by
 * mVariant; CleanupResources releases the same row. */
struct KaitendaiResources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char KaitendaiResources_size_must_be_0x0c[sizeof(KaitendaiResources) == 0x0c ? 1 : -1];

extern u8 data_0209f2c0; /* clock setting: 0 slow, 1 fast, 2 random, 3 stopped */
s16 sTTCCogNormalSpeeds[2] = {
    200,
    400,
};
extern KaitendaiResources data_ov065_0211d35c[];

daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA06() {
    return new daObjCtKaitendai_c();
}

daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA07() {
    return new daObjCtKaitendai_c();
}

int daObjCtKaitendai_c::InitResources() {
    Vector3 pos;

    /* Gear is 0x72 and takes row 0; triangle is 0x73 and takes row 1. */
    if (actorID != 0x72) {
        if (actorID == 0x73)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    mModel.SetFile(dExtSimpleModel_c::LoadFile(*data_ov065_0211d35c[mVariant].model), 1, -1);

    mShadowModel.InitCylinder();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*data_ov065_0211d35c[mVariant].collision), mClsnMat,
                          0x199, mAngle.y, *data_ov065_0211d35c[mVariant].clps);

    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);

    pos.x = mPos.x;
    pos.y = mPos.y;
    pos.z = mPos.z;
    pos.y = pos.y - 0xa000;

    {
        dBgCh_Gnd ground;

        ground.SetObjAndPos(pos, 0);
        mGroundY = pos.y;
        if (ground.DetectClsn() != 0)
            mGroundY = ground.clsnY;
    }

    return 1;
}

int daObjCtKaitendai_c::Behavior() {
    switch (data_0209f2c0) {
    case 0: /* slow hand */
    case 1: /* fast hand */
        mPrevAngle.z = sTTCCogNormalSpeeds[data_0209f2c0];
        break;

    case 2: /* random hand */
        if (ApproachLinear(mPrevAngle.z, mPrevAngle.x, 0x32) != 0) {
            u16 roll = ((u32)RandomIntInternal(&data_0209e650.value) >> 16);

            mPrevAngle.x = ((roll % 7) * 0xc8);
            if (roll >= 0x7fff) {
                mPrevAngle.x *= -1;
            }
        }
        break;

    case 3: /* stopped hand */
        mPrevAngle.z = 0;
        break;
    }

    mPrevAngle.y += mPrevAngle.z;
    mAngle.y = mPrevAngle.y;

    UpdateModelPosAndRotY();
    UpdateShadow();

    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();

    return 1;
}

int daObjCtKaitendai_c::Render() {
    mModel.Render(0);
    return 1;
}

int daObjCtKaitendai_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d35c[mVariant].model->Release();
    data_ov065_0211d35c[mVariant].collision->Release();
    return 1;
}

/* Copy the model matrix, plant its translation Y on the floor probe,
 * and drop the shadow. */
void daObjCtKaitendai_c::UpdateShadow() {
    mShadowMat = mModel.mat4x3;
    mShadowMat.t.y = (mGroundY + 0x32000) >> 3;
    DropShadowRadHeight(mShadowModel, mShadowMat, 0x258000, 0xc8000, 15);
}

/* The four resource handles (model files 0x5c4/0x5c6, collision files
 * 0x5c5/0x5c7), in the retail initializer's construction order: mwcc emits
 * __sinit_d_a_obj_ct_kaitendai.cpp from these definitions. Nothing in this
 * TU names them directly; the KaitendaiResources rows point at them. .bss
 * holds each as one 0x8 word pair (ov065 0x0211d92c..0x0211d94c). */
ModelFile gCtMechaObj06ModelFile(ASSET_CT_MECHA_OBJ06_BMD);
ModelFile gCtMechaObj07ModelFile(ASSET_CT_MECHA_OBJ07_BMD);
ClsnFile gCtMechaObj06CollisionFile(ASSET_CT_MECHA_OBJ06_KCL);
ClsnFile gCtMechaObj07CollisionFile(ASSET_CT_MECHA_OBJ07_KCL);
