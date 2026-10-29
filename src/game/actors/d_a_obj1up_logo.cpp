//cpp
/* daObj1UpLogo_c -- 1-Up logo popup (OBJ_1UPLOGO 331).
 *
 * _ZTS14daObj1UpLogo_c at ov002 0x0210b174, _ZTI at 0x0210b168 (base
 * _ZTI8dActor_c). The tree used to call this class OneUpLogo. The
 * out-of-line destructor is the key function: D1 at 0x020f0dd0, D0 at
 * 0x020f0e08, then a D2 the cartridge does not keep. defer_codegen off
 * lays this file down in ROM order.
 *
 * Notes:
 * - Behavior keeps a volatile read of mPosY and of pos.z. A plain
 *   read is folded into the add and the stores above it.
 * - The follow target's position is read through &mPos. Naming that
 *   actor's mPosX/Y/Z DIFFs.
 * - SetFollowTarget is Player::IncMegaKillCount's attach helper; that
 *   caller passes a null owner.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObj1UpLogo_c_AssetId {
    ASSET_BIG_1UP_NUMBER_BMD = 0x0436,
    ASSET_BIG_1UP_NUMBER_BTP = 0x0437
};

#include "common.h"
#include "decl_common.h"
#include "daObj1UpLogo_c.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "math/MtxStack.h"

/* 20.12 launch. Same numbers as daObjNumber_c's score popup. */
enum {
    kRiseSpeed = 0x14000,      /* 20.0 */
    kGravity = -0x2000,        /* -0.5 */
    kTerminalSpeed = -0x32000, /* -50.0 */
    kBounceSpeed = 0xf000,     /* 15.0 */
    kPopParticle = 0xd2,
    kBtpFlags = 0x40000000,
    kFrameCountCap = 8,
    kLastFrame = 7
};

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x436, texture-sequence file 0x437). mwcc emits
 * __sinit_d_a_obj1up_logo.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. */
struct OneUpLogoModelFile : SharedFilePtr {
    OneUpLogoModelFile(u32 fileID);
    ~OneUpLogoModelFile();
};

struct OneUpLogoTexSeqFilePtr : SharedFilePtr {
    OneUpLogoTexSeqFilePtr(u32 fileID);
    ~OneUpLogoTexSeqFilePtr();
};

extern OneUpLogoModelFile data_ov002_02110aa4;     /* BMD */
extern OneUpLogoTexSeqFilePtr data_ov002_02110a9c; /* BTP */

/* Emission order is ROM order. Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN14daObj1UpLogo_cD1Ev
// @symbol _ZN14daObj1UpLogo_cD0Ev
daObj1UpLogo_c::~daObj1UpLogo_c() {}

// @symbol _ZN14daObj1UpLogo_c15SetFollowTargetEP8dActor_c
/* Player::IncMegaKillCount's attach helper. Not a vtable slot. The only
 * caller passes a null owner. */
void daObj1UpLogo_c::SetFollowTarget(dActor_c *owner) {
    if (owner == 0)
        return;
    mOwnerUniqueID = owner->uniqueID;
    mFollowOffsetY = mPosY - owner->mPosY;
}

// @symbol _ZN14daObj1UpLogo_c16CleanupResourcesEv
s32 daObj1UpLogo_c::CleanupResources() {
    data_ov002_02110aa4.Release();
    data_ov002_02110a9c.Release();
    return 1;
}

// @symbol _ZN14daObj1UpLogo_c6RenderEv
s32 daObj1UpLogo_c::Render() {
    if (mDelay != 0) {
        mDelay--;
        return 1;
    }
    mTextureSequence.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObj1UpLogo_c8BehaviorEv
s32 daObj1UpLogo_c::Behavior() {
    Vector3 pos;

    if (mDelay != 0)
        return 1;

    mVertSpeed += mVertAccel;
    if (mVertSpeed < mTerminalVelocity)
        mVertSpeed = mTerminalVelocity;
    mPosY += mVertSpeed;

    switch (mState) {
    case 0:
        if (mPosY < mStartPos.y) {
            mPosY = mStartPos.y;
            mVertSpeed = kBounceSpeed;
            mState++;
        }
        break;
    case 1:
        if (mPosY < mStartPos.y) {
            /* Volatile: a plain mPosY read is folded into the add above. */
            Particle::System::NewSimple(kPopParticle, mPosX, *(volatile s32 *)&mPosY, mPosZ);
            MarkForDestruction();
        }
        break;
    }

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    if (mOwnerUniqueID != 0) {
        dActor_c *other = FindWithID(mOwnerUniqueID);
        if (other != 0) {
            /* Naming other's three position scalars DIFFs. */
            Vector3 *otherPos = &other->mPos;
            s32 otherY;
            pos.x = otherPos->x;
            pos.y = otherY = otherPos->y;
            pos.z = otherPos->z;
            pos.y = otherY + (mFollowOffsetY + (mPosY - mStartPos.y));
        }
    }
    /* Volatile: a plain pos.z >> 3 is folded into the stores above. */
    Matrix4x3_FromTranslation(&mModel.mat4x3, pos.x >> 3, pos.y >> 3,
                              (*(volatile s32 *)&pos.z) >> 3);
    return 1;
}

// @symbol _ZN14daObj1UpLogo_c13InitResourcesEv
s32 daObj1UpLogo_c::InitResources() {
    unsigned int count = param1;
    unsigned short frame = (unsigned short)(count > kFrameCountCap ? kLastFrame : (count - 1));
    dExtAnmTexPat_c::LoadFile(data_ov002_02110a9c);
    if (mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_02110aa4), 1, -1) == 0)
        return 0;
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_02110aa4.filePtr,
                             *(BTP_File *)data_ov002_02110a9c.filePtr);
    mTextureSequence.SetFile(*(BTP_File *)data_ov002_02110a9c.filePtr, kBtpFlags, 0, frame);
    mState = 0;
    mStartPos.x = mPosX;
    mStartPos.y = mPosY;
    mStartPos.z = mPosZ;
    mVertSpeed = kRiseSpeed;
    mVertAccel = kGravity;
    mTerminalVelocity = kTerminalSpeed;
    mDelay = 0;
    mOwnerUniqueID = 0;
    mFollowOffsetY = 0;
    return 1;
}

// @symbol daObj1UpLogo_c_classInit
/* Historical alias OneUpLogo_Spawn. */
extern "C" daObj1UpLogo_c *daObj1UpLogo_c_classInit() {
    return new daObj1UpLogo_c();
}

/* Static-init globals (was the handwritten __sinit_ov002_02107298 shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x436), then the texture-sequence handle
 * (file 0x437). */
OneUpLogoModelFile data_ov002_02110aa4(ASSET_BIG_1UP_NUMBER_BMD);
OneUpLogoTexSeqFilePtr data_ov002_02110a9c(ASSET_BIG_1UP_NUMBER_BTP);
