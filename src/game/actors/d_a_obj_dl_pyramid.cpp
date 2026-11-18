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
 * 16-byte collision CLPS block (the 0xFF tail is not part of it).
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
#include "ParticleIds.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

struct BMD_File;
struct KCL_File;
struct CLPS_Block;

extern char data_ov024_021129f0[16];

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
daObjDlPyramid_c::~daObjDlPyramid_c() {}

/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
daObjDlPyramidDummy_c::~daObjDlPyramidDummy_c() {}

/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* The top has finished sinking: four puffs of dust at the old summit, then
 * the top removes itself and raises the event the level script waits on. */
void daObjDlPyramid_c::FinishDescent() {
    Vector3 v;
    v.x = mPos.x;
    v.y = mPos.y;
    v.z = mPos.z;
    v.y += 0xfa000;
    Particle::System::NewSimple(kPtclPyramidTopSunkA, v.x, v.y, v.z);
    Particle::System::NewSimple(kPtclPyramidTopSunkB, v.x, v.y, v.z);
    Particle::System::NewSimple(kPtclPyramidTopSunkC, v.x, v.y, v.z);
    Particle::System::NewSimple(kPtclPyramidTopSunkD, v.x, v.y, v.z);
    MarkForDestruction();
    Event::SetBit(0xe);
    dMap_c::UpdateLevelSpecific();
}

/* One frame of the descent: wobble along X off the home position, bob up
 * for the first two seconds, then spin up and sink. Keeps the dust
 * particle alive and moves to state 2 at frame 150. */
void daObjDlPyramid_c::Descend() {
    mPos.x = FX_SinIdx((u16)(s16)(mStateTimer << 14) >> 4) * 0x28 + mHomePos.x;
    if (mStateTimer < 60) {
        int bob = FX_SinIdx((u16)(s16)(mStateTimer << 13) >> 4) * 0xa;
        if (bob < 0)
            bob = -bob;
        mPos.y = mHomePos.y + bob;
    } else {
        mAngVelY += 0x100;
        if (mAngVelY > 0x1800) {
            mAngVelY = 0x1800;
            mSpeed.y = 0x5000;
        }
        mAngle.y += mAngVelY;
        mPos.y += mSpeed.y;
    }
    mSpinParticleID = Particle::System::New(mSpinParticleID, kPtclPyramidTopSinking, mPos.x, mPos.y,
                                            mPos.z, 0, 0);
    if (mStateTimer == 150)
        mState = 2;
}

/* The collider follows the top: yaw matrix, actor position in the
 * translation row, then hand both to the mesh collider. */
void daObjDlPyramid_c::UpdateClsnMatrix() {
    Matrix4x3_FromRotationY(&mClsnMat2, mAngle.y);
    mClsnMat2.m[9] = mPos.x;
    mClsnMat2.m[10] = mPos.y;
    mClsnMat2.m[11] = mPos.z;
    mMeshCollider.Transform(mClsnMat2, mAngle.y);
}

/* The model follows the top: yaw matrix, position at 1/8 scale. */
void daObjDlPyramid_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mTopModel.mat4x3, mAngle.y);
    mTopModel.mat4x3.m[9] = mPos.x >> 3;
    mTopModel.mat4x3.m[10] = mPos.y >> 3;
    mTopModel.mat4x3.m[11] = mPos.z >> 3;
}

int daObjDlPyramid_c::CleanupResources() {
    mMeshCollider.Disable();
    gDlPyramidModelFile.Release();
    gDlPyramidCollisionFile.Release();
    return 1;
}

int daObjDlPyramid_c::Render() {
    mTopModel.Render(0);
    return 1;
}

/* 0: wait for all four tags. 1: jingle, then descend. 2: descend until the
 * jingle has finished, then settle. */
int daObjDlPyramid_c::Behavior() {
    u8 state = mState;
    switch (state) {
    case 0:
        if (mNumTagsTriggered == 4)
            mState++;
        break;
    case 1:
        Sound::PlaySecretSound(this, &mSoundTimer);
        if (mStateTimer == 0)
            Sound::PlayBank3(0x4b, mCamSpacePos);
        Descend();
        break;
    case 2:
        if (Sound::PlaySecretSound(this, &mSoundTimer)) {
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

int daObjDlPyramid_c::InitResources() {
    BMD_File *bmd = dExtSimpleModel_c::LoadFile(gDlPyramidModelFile);
    mTopModel.SetFile(bmd, 1, -1);
    UpdateModelMatrix();
    UpdateClsnMatrix();
    KCL_File *kcl = dBgW_Kc::LoadFile(gDlPyramidCollisionFile);
    /* Kept (#821 r76): genuine; this TU defines the CLPS block as its bytes and CLPS_Block is
       opaque (CLPS.h). */
    mMeshCollider.SetFile(kcl, mClsnMat2, 0x199, mAngle.y, *(CLPS_Block *)&data_ov024_021129f0);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    mMeshCollider.Enable(this);
    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;
    mAngVelY = 0;
    mNumTagsTriggered = 0;
    mState = 0;
    mSpinParticleID = 0;
    mSoundTimer = 0;
    Event::ClearBit(0xe);
    return 1;
}

/* Binds to the one pyramid top in the level, or removes itself. */
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
daObjDlPyramidDummy_c *daObjDlPyramidDummy_c_classInit() {
    return new daObjDlPyramidDummy_c;
}

/* -------------------------------------------------------------------------- */
daObjDlPyramid_c *daObjDlPyramid_c_classInit() {
    return new daObjDlPyramid_c;
}

/* Retail construction order: model 1501, then collision 1502. mwcc emits
 * __sinit_d_a_obj_dl_pyramid.cpp from these two definitions. */
DlPyramidModelFilePtr gDlPyramidModelFile(ASSET_DL_PYRAMID_BMD);
DlPyramidCollisionFilePtr gDlPyramidCollisionFile(ASSET_DL_PYRAMID_KCL);
char data_ov024_021129f0[16] = {
    0x43, 0x4c, 0x50, 0x53, 0x08, 0x00, 0x01, 0x00, 0xc4, 0x3f, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00,
};
