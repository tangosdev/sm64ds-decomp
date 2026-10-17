//cpp
/* daObj1UpLogo_c -- 1-Up logo popup (OBJ_1UPLOGO 331).
 *
 * _ZTS14daObj1UpLogo_c at ov002 0x0210b174, _ZTI at 0x0210b168 (base
 * _ZTI8dActor_c). The tree used to call this class OneUpLogo. The
 * out-of-line destructor is the key function: D1 at 0x020f0dd0, D0 at
 * 0x020f0e08, then a D2 the cartridge does not keep. defer_codegen off
 * lays this file down in ROM order.
 *
 * leftovers:
 * - TextureSequence::SetFile and Particle::System::NewSimple stay
 *   mangled. Both take Fix12<int> by value.
 * - SharedFilePtr has no fields, so InitResources reads the loaded
 *   BMD and BTP at +4 of data_ov002_02110aa4 and 02110a9c.
 * - Behavior keeps a volatile read of mPosY and of pos.z. A plain
 *   read is folded into the add and the stores above it.
 * - The follow target's position is read through &mPos. Naming that
 *   actor's mPosX/Y/Z DIFFs.
 * - func_ov002_020f0e54 keeps the char * parameters decl_common.h
 *   declares. It is Player::IncMegaKillCount's attach helper.
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

/* 20.12 launch. Same numbers as daObjNumber_c's score popup. */
enum {
    kRiseSpeed = 0x14000,        /* 20.0 */
    kGravity = -0x2000,          /* -0.5 */
    kTerminalSpeed = -0x32000,   /* -50.0 */
    kBounceSpeed = 0xf000,       /* 15.0 */
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
    u32 words[2];

    OneUpLogoModelFile(u32 fileID);
    ~OneUpLogoModelFile();
};

struct OneUpLogoTexSeqFilePtr : SharedFilePtr {
    u32 words[2];

    OneUpLogoTexSeqFilePtr(u32 fileID);
    ~OneUpLogoTexSeqFilePtr();
};

extern OneUpLogoModelFile data_ov002_02110aa4; /* BMD */
extern OneUpLogoTexSeqFilePtr data_ov002_02110a9c; /* BTP */

extern "C" {
void Matrix4x3_FromTranslation(Matrix4x3 *matrix, s32 x, s32 y, s32 z);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void *self, void *btp, int flags, int speed, unsigned int startFrame);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, s32 x, s32 y, s32 z);
}

/* Emission order is ROM order. Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN14daObj1UpLogo_cD1Ev
// @symbol _ZN14daObj1UpLogo_cD0Ev
daObj1UpLogo_c::~daObj1UpLogo_c()
{
}

// @symbol func_ov002_020f0e54
/* Player::IncMegaKillCount's attach helper. Not a vtable slot. The only
 * caller passes a null owner. Parameters stay char* -- that is the
 * declaration in decl_common.h. */
extern "C" void func_ov002_020f0e54(char *a, char *b)
{
    if (b == 0)
        return;
    daObj1UpLogo_c *self = (daObj1UpLogo_c *)a;
    dActor_c *owner = (dActor_c *)b;
    self->mOwnerUniqueID = owner->uniqueID;
    self->mFollowOffsetY = self->mPosY - owner->mPosY;
}

// @symbol _ZN14daObj1UpLogo_c16CleanupResourcesEv
s32 daObj1UpLogo_c::CleanupResources()
{
    data_ov002_02110aa4.Release();
    data_ov002_02110a9c.Release();
    return 1;
}

// @symbol _ZN14daObj1UpLogo_c6RenderEv
s32 daObj1UpLogo_c::Render()
{
    if (mDelay != 0) {
        mDelay--;
        return 1;
    }
    mTextureSequence.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObj1UpLogo_c8BehaviorEv
s32 daObj1UpLogo_c::Behavior()
{
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
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                kPopParticle, mPosX, *(volatile s32 *)&mPosY, mPosZ);
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
s32 daObj1UpLogo_c::InitResources()
{
    unsigned int count = param1;
    unsigned short frame = (unsigned short)(count > kFrameCountCap ? kLastFrame : (count - 1));
    TextureSequence::LoadFile(data_ov002_02110a9c);
    if (mModel.SetFile((BMD_File *)Model::LoadFile(data_ov002_02110aa4), 1, -1) == 0)
        return 0;
    TextureSequence::Prepare(
        **(BMD_File **)((char *)&data_ov002_02110aa4 + 4),
        **(BTP_File **)((char *)&data_ov002_02110a9c + 4));
    /* InitResources: SetFile's Fix12<int> parameter. A braced method call
       does not compile, so this stays the scalar extern. */
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence,
        *(void **)((char *)&data_ov002_02110a9c + 4),
        kBtpFlags, 0, frame);
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
extern "C" daObj1UpLogo_c *daObj1UpLogo_c_classInit()
{
    return new daObj1UpLogo_c();
}

/* Static-init globals (was the handwritten __sinit_ov002_02107298 shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x436), then the texture-sequence handle
 * (file 0x437). */
OneUpLogoModelFile data_ov002_02110aa4(ASSET_BIG_1UP_NUMBER_BMD);
OneUpLogoTexSeqFilePtr data_ov002_02110a9c(ASSET_BIG_1UP_NUMBER_BTP);
