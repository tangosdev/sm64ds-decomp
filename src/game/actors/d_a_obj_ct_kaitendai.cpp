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
 * 3 the stopped hand. The speed lives in mPrevAngleZ, the random-hand
 * target in mPrevAngleX, and the accumulated yaw in mPrevAngleY, which
 * is copied to mAngleY for the model and the mesh.
 *
 * The destructor pair comes off the inline ~daObjCtKaitendai_c() in the
 * header. Source order is reverse ROM order (2004/b56 emits .text back
 * to front); do not reorder.
 *
 * Notes:
 * - dBgW_KcMbg::SetFile, dActor_c::DropShadowRadHeight and
 *   dBgActor_c::IsClsnInRange are header member calls; their headers
 *   spell the Fix12 parameters as Fix12i, a plain s32. Passing a
 *   Fix12<int> by value size-DIFFs this TU: InitResources 0x14c against
 *   0x140, and UpdateShadow 0x80 against 0x70 (notes/mwccarm-codegen.md
 *   6az).
 * - func_020393d4 is the store of dBgW::beforeClsnCallback (+0x18).
 *   This InitResources calls it with &dBgW::UpdatePosAndAngs. No setter.
 * - The sign flip and the yaw add go through a pointer. Writing
 *   mPrevAngleX = mPrevAngleX * -1 comes out 0x12c, and
 *   mPrevAngleY = mPrevAngleY + mPrevAngleZ comes out 0x134, against
 *   Behavior's 0x138. The pointer is what keeps the halfword multiply
 *   and the reload into mAngleY.
 * - data_ov065_0211d334 (the 200/400 speeds) and data_ov065_0211d35c
 *   (the two resource rows) are not this TU's data. Neither are
 *   g_profile_CT_MECHA06 / CT_MECHA07.
 * - common.h stays first. Matrix4x3's flat 12-word spelling is what
 *   puts the shadow's ground height in m[10].
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

struct CLPS_Block;

/* One row is {model, collision, clps}. InitResources indexes it by
 * mVariant; CleanupResources releases the same row. */
struct KaitendaiResources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char KaitendaiResources_size_must_be_0x0c[sizeof(KaitendaiResources) == 0x0c ? 1 : -1];

/* File-scope objects at the end of this file construct the four resource
 * handles (model files 0x5c4/0x5c6, collision files 0x5c5/0x5c7). mwcc
 * emits __sinit_d_a_obj_ct_kaitendai.cpp from those definitions. The wrapper
 * names are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. Nothing in this TU
 * names the handles directly; the KaitendaiResources rows above point at
 * them. */
struct KaitendaiModelFile : SharedFilePtr {
    KaitendaiModelFile(u32 fileID);
    ~KaitendaiModelFile();
};

struct KaitendaiCollisionFilePtr : SharedFilePtr {
    KaitendaiCollisionFilePtr(u32 fileID);
    ~KaitendaiCollisionFilePtr();
};

/* .bss proves each handle is one 0x8 word pair (ov065 0x0211d92c..0x0211d94c). */
typedef char KaitendaiModelFile_size_must_be_0x08[sizeof(KaitendaiModelFile) == 0x08 ? 1 : -1];
typedef char
    KaitendaiCollisionFilePtr_size_must_be_0x08[sizeof(KaitendaiCollisionFilePtr) == 0x08 ? 1 : -1];

extern u8 data_0209f2c0;           /* clock setting: 0 slow, 1 fast, 2 random, 3 stopped */
extern int data_0209e650;          /* shared RNG state */
extern s16 data_ov065_0211d334[2]; /* yaw speed for the slow and fast hands: 200, 400 */
extern KaitendaiResources data_ov065_0211d35c[];

/* local extern: func_020393d4 is the dBgW callback store (see the banner)
 * and has no shared header. RandomIntInternal is declared unsigned here:
 * math/CalcLib.h's int return turns Behavior's `>> 16` into an asr and
 * changes its bytes. */
extern "C" {
extern void func_020393d4(void *p, void *v);
extern unsigned int RandomIntInternal(int *seed);
}

// @symbol daObjCtKaitendai_c_classInit_CT_MECHA06
extern "C" daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA06() {
    return new daObjCtKaitendai_c();
}

// @symbol daObjCtKaitendai_c_classInit_CT_MECHA07
extern "C" daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA07() {
    return new daObjCtKaitendai_c();
}

// @symbol _ZN18daObjCtKaitendai_c13InitResourcesEv
int daObjCtKaitendai_c::InitResources() {
    Vector3 pos;

    /* Gear is 0x72 and takes row 0; triangle is 0x73 and takes row 1. */
    if (actorID != 0x72) {
        if (actorID == 0x73)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov065_0211d35c[mVariant].model), 1,
                   -1);

    mShadowModel.InitCylinder();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(*data_ov065_0211d35c[mVariant].collision),
                          mClsnMat, 0x199, mAngleY, *data_ov065_0211d35c[mVariant].clps);

    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y = pos.y - 0xa000;

    {
        dBgCh_Gnd ground;

        ground.SetObjAndPos(pos, (dActor_c *)0);
        mGroundY = pos.y;
        if (ground.DetectClsn() != 0)
            mGroundY = ground.clsnY;
    }

    return 1;
}

// @symbol _ZN18daObjCtKaitendai_c8BehaviorEv
int daObjCtKaitendai_c::Behavior() {
    switch (data_0209f2c0) {
    case 0: /* slow hand */
    case 1: /* fast hand */
        mPrevAngleZ = data_ov065_0211d334[data_0209f2c0];
        break;

    case 2: /* random hand */
        if (ApproachLinear(mPrevAngleZ, mPrevAngleX, 50) != 0) {
            u16 roll = (u16)(RandomIntInternal(&data_0209e650) >> 16);

            mPrevAngleX = (s16)((roll % 7) * 200);
            if (roll >= 0x7fff) {
                s16 *target = &mPrevAngleX;
                *target *= (s16)-1;
            }
        }
        break;

    case 3: /* stopped hand */
        mPrevAngleZ = 0;
        break;
    }

    {
        s16 *yaw = &mPrevAngleY;
        *yaw = (s16)(*yaw + mPrevAngleZ);
    }
    mAngleY = mPrevAngleY;

    UpdateModelPosAndRotY();
    UpdateShadow();

    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();

    return 1;
}

// @symbol _ZN18daObjCtKaitendai_c6RenderEv
int daObjCtKaitendai_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjCtKaitendai_c16CleanupResourcesEv
int daObjCtKaitendai_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d35c[mVariant].model->Release();
    data_ov065_0211d35c[mVariant].collision->Release();
    return 1;
}

// @symbol _ZN18daObjCtKaitendai_c12UpdateShadowEv
/* Copy the model matrix, plant its translation Y on the floor probe,
 * and drop the shadow. m[10] is that Y in the flat Matrix4x3. */
void daObjCtKaitendai_c::UpdateShadow() {
    mShadowMat = mModel.mat4x3;
    mShadowMat.m[10] = (mGroundY + 0x32000) >> 3;
    DropShadowRadHeight(mShadowModel, mShadowMat, 0x258000, 0xc8000, 0xf);
}

/* Static-init globals (was the handwritten __sinit_ov065_0211c8fc shard).
 * Definition order is the retail initializer's construction order. */
KaitendaiModelFile gCtMechaObj06ModelFile(ASSET_CT_MECHA_OBJ06_BMD);
KaitendaiModelFile gCtMechaObj07ModelFile(ASSET_CT_MECHA_OBJ07_BMD);
KaitendaiCollisionFilePtr gCtMechaObj06CollisionFile(ASSET_CT_MECHA_OBJ06_KCL);
KaitendaiCollisionFilePtr gCtMechaObj07CollisionFile(ASSET_CT_MECHA_OBJ07_KCL);
