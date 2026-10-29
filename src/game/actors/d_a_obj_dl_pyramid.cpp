//cpp
/* daObjDlPyramid_c and daObjDlPyramidDummy_c. ov024 0x021111a0..0x021118ac,
 * 16 functions.
 *
 * The pyramid top in Shifting Sand Land and the four invisible tags on its
 * faces. Each tag that is touched looks the top up by unique ID and bumps
 * its count; at four the top plays the secret jingle, spins its way down
 * into the sand and leaves the level-specific event flag set.
 *
 * ROM names from _ZTS16daObjDlPyramid_c and _ZTS21daObjDlPyramidDummy_c.
 * Vtables _ZTV16daObjDlPyramid_c at 0x021138c8 and
 * _ZTV21daObjDlPyramidDummy_c at 0x02113844. The two classInit factories
 * that follow InitResources (0x0211183c, 0x02111874) are zero-gap and folded
 * in here; the next run starts at 0x021118ac.
 *
 * #pragma defer_codegen off lays .text down in source order, so the file
 * reads in ROM order.
 *
 * S41: _ZTS16daObjDlPyramid_c / _ZTS21daObjDlPyramidDummy_c;
 * _ZTV16daObjDlPyramid_c 0x021138c8, _ZTV21daObjDlPyramidDummy_c 0x02113844;
 * D1 0x021111a0 / D0 0x021111ec (pyramid), D1 0x0211124c / D0 0x0211127c (dummy);
 * D0 emitted from D1 definition, defer_codegen off keeps ROM order.
 *
 * The two resource handles below (model gDlPyramidModelFile, collision
 * gDlPyramidCollisionFile) are defined by this TU; mwcc builds
 * __sinit_d_a_obj_dl_pyramid.cpp from them. data_ov024_021129f0 is the
 * collision CLPS block, overlay .data this TU does not own.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjDlPyramid_c_AssetId { ASSET_DL_PYRAMID_BMD = 0x05dd, ASSET_DL_PYRAMID_KCL = 0x05de };

#include "daObjDlPyramid_c.h"
#include "daObjDlPyramidDummy_c.h"
#include "common.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dMap_c.h"
#include "Event.h"
#include "Particle__System.h"
#include "math/MtxStack.h"

struct BMD_File;
struct KCL_File;
struct CLPS_Block;

extern s16 data_02082214[];            /* sine/cosine table, interleaved */
extern CLPS_Block data_ov024_021129f0; /* the collision CLPS block */

/* local extern: the dBgW callback store; no shared header. */
extern "C" void func_020393d4(int *collider, int callback);

/* The retail static initializer constructs these two 8-byte resource handles
 * in source order (model 1501, then collision 1502) and lets the C++ runtime
 * register their destructors. InitResources loads the model through the first
 * (dExtSimpleModel_c::LoadFile) and the collision through the second (dBgW_Kc::LoadFile);
 * CleanupResources releases both (S19/S31: typed from this TU's callees).
 * The family spellings are reconstructed; the constructor/destructor
 * addresses, file IDs, object widths, BSS order, and registration topology
 * are direct ROM evidence. The intact-TU manifest maps their
 * compiler-generated undefined member imports onto the existing
 * evidence-bounded ROM symbols. */
struct DlPyramidModelFilePtr : SharedFilePtr {
    DlPyramidModelFilePtr(u32 fileID);
    ~DlPyramidModelFilePtr();
};

struct DlPyramidCollisionFilePtr : SharedFilePtr {
    DlPyramidCollisionFilePtr(u32 fileID);
    ~DlPyramidCollisionFilePtr();
};

extern DlPyramidModelFilePtr gDlPyramidModelFile;
extern DlPyramidCollisionFilePtr gDlPyramidCollisionFile;

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjDlPyramid_cD1Ev
daObjDlPyramid_c::~daObjDlPyramid_c() {}

// @symbol _ZN16daObjDlPyramid_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjDlPyramidDummy_cD1Ev
daObjDlPyramidDummy_c::~daObjDlPyramidDummy_c() {}

// @symbol _ZN21daObjDlPyramidDummy_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* The top has finished sinking: four puffs of dust at the old summit, then
 * the top removes itself and raises the event the level script waits on. */
// @symbol _ZN16daObjDlPyramid_c13FinishDescentEv
void daObjDlPyramid_c::FinishDescent() {
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y += 0xfa000;
    Particle::System::NewSimple(0x1f, v.x, v.y, v.z);
    Particle::System::NewSimple(0x20, *(volatile int *)&v.x, *(volatile int *)&v.y,
                                *(volatile int *)&v.z);
    Particle::System::NewSimple(0x21, *(volatile int *)&v.x, *(volatile int *)&v.y,
                                *(volatile int *)&v.z);
    Particle::System::NewSimple(0x22, *(volatile int *)&v.x, *(volatile int *)&v.y,
                                *(volatile int *)&v.z);
    MarkForDestruction();
    Event::SetBit(0xe);
    dMap_c::UpdateLevelSpecific();
}

/* One frame of the descent: wobble along X off the home position, bob up
 * for the first two seconds, then spin up and sink. Keeps the dust
 * particle alive and moves to state 2 at frame 150. */
// @symbol _ZN16daObjDlPyramid_c7DescendEv
void daObjDlPyramid_c::Descend() {
    s16 *tbl = data_02082214;
    int a1 = ((int)((unsigned int)mStateTimer << 0x1e)) >> 0x10;
    mPosX = tbl[((u16)a1 >> 4) * 2] * (s16)0x28 + mHomePosX;
    if (mStateTimer < 0x3c) {
        int a2 = ((int)((unsigned int)mStateTimer << 0x1d)) >> 0x10;
        int bob = tbl[((u16)a2 >> 4) * 2] * (s16)0xa;
        if (bob < 0)
            bob = -bob;
        mPosY = mHomePosY + bob;
    } else {
        mAngVelY += 0x100;
        if (mAngVelY > 0x1800) {
            mAngVelY = 0x1800;
            mVertSpeed = 0x5000;
        }
        mAngleY += mAngVelY;
        mPosY += mVertSpeed;
    }
    mSpinParticleID = Particle::System::New(mSpinParticleID, 0x23, mPosX, mPosY, mPosZ, 0, 0);
    if (mStateTimer == 150)
        mState = 2;
}

/* The collider follows the top: yaw matrix, actor position in the
 * translation row, then hand both to the mesh collider. */
// @symbol _ZN16daObjDlPyramid_c16UpdateClsnMatrixEv
void daObjDlPyramid_c::UpdateClsnMatrix() {
    Matrix4x3_FromRotationY(&mClsnMat2, mAngleY);
    mClsnMat2.t.x = mPosX;
    mClsnMat2.t.y = mPosY;
    mClsnMat2.t.z = mPosZ;
    mMeshCollider.Transform(mClsnMat2, mAngleY);
}

/* The model follows the top: yaw matrix, position at 1/8 scale. */
// @symbol _ZN16daObjDlPyramid_c17UpdateModelMatrixEv
void daObjDlPyramid_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mTopModel.mat4x3, mAngleY);
    mTopModel.mat4x3.t.x = mPosX >> 3;
    mTopModel.mat4x3.t.y = mPosY >> 3;
    mTopModel.mat4x3.t.z = mPosZ >> 3;
}

// @symbol _ZN16daObjDlPyramid_c16CleanupResourcesEv
int daObjDlPyramid_c::CleanupResources() {
    mMeshCollider.Disable();
    gDlPyramidModelFile.Release();
    gDlPyramidCollisionFile.Release();
    return 1;
}

// @symbol _ZN16daObjDlPyramid_c6RenderEv
int daObjDlPyramid_c::Render() {
    mTopModel.Render(0);
    return 1;
}

/* 0: wait for all four tags. 1: jingle, then descend. 2: descend until the
 * jingle has finished, then settle. */
// @symbol _ZN16daObjDlPyramid_c8BehaviorEv
int daObjDlPyramid_c::Behavior() {
    u8 state = mState;
    switch (state) {
    case 0:
        if (mNumTagsTriggered == 4)
            mState++;
        break;
    case 1:
        Sound::PlaySecretSound(this, (u16 *)&mSoundTimer);
        if (mStateTimer == 0)
            Sound::PlayBank3(0x4b, mCamSpacePos);
        Descend();
        break;
    case 2:
        if (Sound::PlaySecretSound(this, (u16 *)&mSoundTimer)) {
            Sound::PlayBank3(0x4c, mCamSpacePos);
            FinishDescent();
        } else {
            Descend();
        }
        break;
    }
    mStateTimer++;
    if (state != mState)
        mStateTimer = 0;
    UpdateModelMatrix();
    UpdateClsnMatrix();
    return 1;
}

/* A touched tag counts once on the top it was bound to, then goes away. */
// @symbol _ZN21daObjDlPyramidDummy_c8BehaviorEv
s32 daObjDlPyramidDummy_c::Behavior() {
    if (mCylinder.otherOwner != 0) {
        if (mPyramidTopID == 0) {
            MarkForDestruction();
            return 1;
        }

        daObjDlPyramid_c *top = (daObjDlPyramid_c *)dActor_c::FindWithID(mPyramidTopID);
        if (top != 0)
            ++top->mNumTagsTriggered;

        MarkForDestruction();
        return 1;
    }

    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}

// @symbol _ZN16daObjDlPyramid_c13InitResourcesEv
int daObjDlPyramid_c::InitResources() {
    BMD_File *bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(gDlPyramidModelFile);
    mTopModel.SetFile(bmd, 1, -1);
    UpdateModelMatrix();
    UpdateClsnMatrix();
    void *kcl = dBgW_Kc::LoadFile(gDlPyramidCollisionFile);
    mMeshCollider.SetFile((KCL_File *)kcl, mClsnMat2, 0x199, mAngleY, data_ov024_021129f0);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosAndAngs);
    mMeshCollider.Enable(this);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mAngVelY = 0;
    mNumTagsTriggered = 0;
    mState = 0;
    mSpinParticleID = 0;
    mSoundTimer = 0;
    Event::ClearBit(0xe);
    return 1;
}

/* Binds to the one pyramid top in the level, or removes itself. */
// @symbol _ZN21daObjDlPyramidDummy_c13InitResourcesEv
s32 daObjDlPyramidDummy_c::InitResources() {
    dActor_c *top = dActor_c::FindWithActorID(0x55, 0);
    if (top == 0) {
        MarkForDestruction();
        return 1;
    }

    mPyramidTopID = top->uniqueID;
    mCylinder.Init(this, 0x7d000, 0x28000, 2, 0x400000);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* Factories. `return new` calls the inherited fBase_c::operator new -- the
   allocator the cartridge's own factories call. Spelled by hand they read as a four-step C
   transcription; this is the original shape. */
/* -------------------------------------------------------------------------- */
// @symbol daObjDlPyramidDummy_c_classInit
extern "C" daObjDlPyramidDummy_c *daObjDlPyramidDummy_c_classInit() {
    return new daObjDlPyramidDummy_c;
}

/* -------------------------------------------------------------------------- */
// @symbol daObjDlPyramid_c_classInit
extern "C" daObjDlPyramid_c *daObjDlPyramid_c_classInit() {
    return new daObjDlPyramid_c;
}

/* Retail construction order: model 1501, then collision 1502. mwcc emits
 * __sinit_d_a_obj_dl_pyramid.cpp from these two definitions. */
DlPyramidModelFilePtr gDlPyramidModelFile(ASSET_DL_PYRAMID_BMD);
DlPyramidCollisionFilePtr gDlPyramidCollisionFile(ASSET_DL_PYRAMID_KCL);
