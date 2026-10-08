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
 * reads in ROM order. The four helpers keep their unmangled ROM-only
 * names and take the top by pointer.
 *
 * deslop
 * Leftover: the four helpers keep their unmangled ROM-only names
 *   (func_ov024_021112c0, 02111350, 02111480, 021114c4). They are this TU's
 *   own statics/helpers, not vtable slots, and no header names them.
 * Leftover: dBgW_KcMbg::SetFile, Model::LoadFile and dCcAc_c::Init keep
 *   their mangled extern-C spellings. SetFile takes Fix12<int> by value
 *   (Fix12 wall, notes/mwccarm-codegen.md 6az).
 * The two resource handles below (model data_ov024_02113968, collision
 * data_ov024_02113960) are defined by this TU; mwcc builds
 * __sinit_daObjDlPyramid_c.cpp from them. data_ov024_021129f0 is the
 * collision CLPS block, overlay .data this TU does not own.
 */

#include "daObjDlPyramid_c.h"
#include "daObjDlPyramidDummy_c.h"
#include "common.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dMap_c.h"

struct BMD_File;
struct KCL_File;
struct CLPS_Block;

namespace Event { void SetBit(unsigned int bit); int ClearBit(unsigned int bit); }

/* Fix12<int> is passed by value at these call boundaries. Spelled as the
 * class type, mwccarm homes the register arguments and the callers grow, so
 * the measured scalar views stay. */
extern "C" {
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 id, int x, int y, int z, void *dir, void *callback);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *cylinder, dActor_c *owner, Fix12i radius, Fix12i height, u32 flags, u32 hitFlags);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *collider, void *kcl, void *mat, int scale, short angY, void *clps);
void Matrix4x3_FromRotationY(void *m, int angleY);
void func_020393d4(int *collider, int callback);

extern s16 data_02082214[];         /* sine/cosine table, interleaved */
extern int data_ov024_021129f0[];   /* the collision CLPS block */

void func_ov024_021112c0(daObjDlPyramid_c *self);
void func_ov024_02111350(daObjDlPyramid_c *self);
void func_ov024_02111480(daObjDlPyramid_c *self);
void func_ov024_021114c4(daObjDlPyramid_c *self);
}

/* The retail static initializer constructs these two 8-byte resource handles
 * in source order (model 1501, then collision 1502) and lets the C++ runtime
 * register their destructors. InitResources loads the model through the first
 * (Model::LoadFile) and the collision through the second (dBgW_Kc::LoadFile);
 * CleanupResources releases both (S19/S31: typed from this TU's callees).
 * The family spellings are reconstructed; the constructor/destructor
 * addresses, file IDs, object widths, BSS order, and registration topology
 * are direct ROM evidence. The intact-TU manifest maps their
 * compiler-generated undefined member imports onto the existing
 * evidence-bounded ROM symbols. */
struct DlPyramidModelFilePtr : SharedFilePtr {
    u32 words[2];

    DlPyramidModelFilePtr(u32 fileID);
    ~DlPyramidModelFilePtr();
};

struct DlPyramidCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    DlPyramidCollisionFilePtr(u32 fileID);
    ~DlPyramidCollisionFilePtr();
};

extern "C" DlPyramidModelFilePtr data_ov024_02113968;
extern "C" DlPyramidCollisionFilePtr data_ov024_02113960;

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* 0x021111a0 _ZN16daObjDlPyramid_cD1Ev, 0x021111ec _ZN16daObjDlPyramid_cD0Ev   */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjDlPyramid_cD1Ev
daObjDlPyramid_c::~daObjDlPyramid_c()
{
}

// @symbol _ZN16daObjDlPyramid_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* -------------------------------------------------------------------------- */
/* 0x0211124c _ZN21daObjDlPyramidDummy_cD1Ev,                                  */
/* 0x0211127c _ZN21daObjDlPyramidDummy_cD0Ev                                   */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjDlPyramidDummy_cD1Ev
daObjDlPyramidDummy_c::~daObjDlPyramidDummy_c()
{
}

// @symbol _ZN21daObjDlPyramidDummy_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* The top has finished sinking: four puffs of dust at the old summit, then
 * the top removes itself and raises the event the level script waits on. */
// @symbol func_ov024_021112c0
extern "C" void func_ov024_021112c0(daObjDlPyramid_c *self)
{
    Vector3 v;
    v.x = self->mPosX;
    v.y = self->mPosY;
    v.z = self->mPosZ;
    v.y += 0xfa000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1f, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x20, *(volatile int *)&v.x, *(volatile int *)&v.y, *(volatile int *)&v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x21, *(volatile int *)&v.x, *(volatile int *)&v.y, *(volatile int *)&v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x22, *(volatile int *)&v.x, *(volatile int *)&v.y, *(volatile int *)&v.z);
    self->MarkForDestruction();
    Event::SetBit(0xe);
    dMap_c::UpdateLevelSpecific();
}

/* One frame of the descent: wobble along X off the home position, bob up
 * for the first two seconds, then spin up and sink. Keeps the dust
 * particle alive and moves to state 2 at frame 150. */
// @symbol func_ov024_02111350
extern "C" void func_ov024_02111350(daObjDlPyramid_c *self)
{
    s16 *tbl = data_02082214;
    int a1 = ((int)((unsigned int)self->mStateTimer << 0x1e)) >> 0x10;
    self->mPosX = tbl[((u16)a1 >> 4) * 2] * (s16)0x28 + self->mHomePosX;
    if (self->mStateTimer < 0x3c) {
        int a2 = ((int)((unsigned int)self->mStateTimer << 0x1d)) >> 0x10;
        int bob = tbl[((u16)a2 >> 4) * 2] * (s16)0xa;
        if (bob < 0) bob = -bob;
        self->mPosY = self->mHomePosY + bob;
    } else {
        self->mAngVelY += 0x100;
        if (self->mAngVelY > 0x1800) {
            self->mAngVelY = 0x1800;
            self->mVertSpeed = 0x5000;
        }
        self->mAngleY += self->mAngVelY;
        self->mPosY += self->mVertSpeed;
    }
    self->mSpinParticleID = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        self->mSpinParticleID, 0x23, self->mPosX, self->mPosY, self->mPosZ, 0, 0);
    if (self->mStateTimer == 150)
        self->mState = 2;
}

/* The collider follows the top: yaw matrix, actor position in the
 * translation row, then hand both to the mesh collider. */
// @symbol func_ov024_02111480
extern "C" void func_ov024_02111480(daObjDlPyramid_c *self)
{
    Matrix4x3_FromRotationY(&self->mClsnMat2, self->mAngleY);
    self->mClsnMat2.t.x = self->mPosX;
    self->mClsnMat2.t.y = self->mPosY;
    self->mClsnMat2.t.z = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat2, self->mAngleY);
}

/* The model follows the top: yaw matrix, position at 1/8 scale. */
// @symbol func_ov024_021114c4
extern "C" void func_ov024_021114c4(daObjDlPyramid_c *self)
{
    Matrix4x3_FromRotationY(&self->mTopModel.mat4x3, self->mAngleY);
    self->mTopModel.mat4x3.t.x = self->mPosX >> 3;
    self->mTopModel.mat4x3.t.y = self->mPosY >> 3;
    self->mTopModel.mat4x3.t.z = self->mPosZ >> 3;
}

// @symbol _ZN16daObjDlPyramid_c16CleanupResourcesEv
int daObjDlPyramid_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov024_02113968.Release();
    data_ov024_02113960.Release();
    return 1;
}

// @symbol _ZN16daObjDlPyramid_c6RenderEv
int daObjDlPyramid_c::Render()
{
    mTopModel.Render(0);
    return 1;
}

/* 0: wait for all four tags. 1: jingle, then descend. 2: descend until the
 * jingle has finished, then settle. */
// @symbol _ZN16daObjDlPyramid_c8BehaviorEv
int daObjDlPyramid_c::Behavior()
{
    u8 state = mState;
    switch (state) {
    case 0:
        if (mNumTagsTriggered == 4)
            mState++;
        break;
    case 1:
        Sound::PlaySecretSound(this, (u16 *)&mSoundTimer);
        if (mStateTimer == 0)
            Sound::PlayBank3(0x4b, *(Vector3 *)&mCamSpacePosX);
        func_ov024_02111350(this);
        break;
    case 2:
        if (Sound::PlaySecretSound(this, (u16 *)&mSoundTimer)) {
            Sound::PlayBank3(0x4c, *(Vector3 *)&mCamSpacePosX);
            func_ov024_021112c0(this);
        } else {
            func_ov024_02111350(this);
        }
        break;
    }
    mStateTimer++;
    if (state != mState)
        mStateTimer = 0;
    func_ov024_021114c4(this);
    func_ov024_02111480(this);
    return 1;
}

/* A touched tag counts once on the top it was bound to, then goes away. */
// @symbol _ZN21daObjDlPyramidDummy_c8BehaviorEv
s32 daObjDlPyramidDummy_c::Behavior()
{
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
int daObjDlPyramid_c::InitResources()
{
    BMD_File *bmd = (BMD_File *)Model::LoadFile(data_ov024_02113968);
    mTopModel.SetFile(bmd, 1, -1);
    func_ov024_021114c4(this);
    func_ov024_02111480(this);
    void *kcl = dBgW_Kc::LoadFile(data_ov024_02113960);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat2, 0x199, mAngleY, data_ov024_021129f0);
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
s32 daObjDlPyramidDummy_c::InitResources()
{
    dActor_c *top = dActor_c::FindWithActorID(0x55, 0);
    if (top == 0) {
        MarkForDestruction();
        return 1;
    }

    mPyramidTopID = top->uniqueID;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x7d000, 0x28000, 2, 0x400000);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* Factories. `return new` routes through each class's operator new adapter,
   which forwards to fBase_c::operator new -- the allocator the cartridge's
   own factories call. Spelled by hand they read as a four-step C
   transcription; this is the original shape. */
/* -------------------------------------------------------------------------- */
// @symbol daObjDlPyramidDummy_c_classInit
extern "C" daObjDlPyramidDummy_c *daObjDlPyramidDummy_c_classInit()
{
    return new daObjDlPyramidDummy_c;
}

/* -------------------------------------------------------------------------- */
// @symbol daObjDlPyramid_c_classInit
extern "C" daObjDlPyramid_c *daObjDlPyramid_c_classInit()
{
    return new daObjDlPyramid_c;
}

/* Retail construction order: model 1501, then collision 1502. mwcc emits
 * __sinit_daObjDlPyramid_c.cpp from these two definitions. */
DlPyramidModelFilePtr data_ov024_02113968(1501);
DlPyramidCollisionFilePtr data_ov024_02113960(1502);
