//cpp
/* Floating score popup. It bounces above its starting height, follows an
 * optional owner, then disappears in a small particle burst.
 * The two resource pairs supply the ordinary and packed-digit displays. */

#include "daObjNumber_c.h"
#include "SharedFilePtr.h"
#include "PowerStar.h"

extern SharedFilePtr data_ov002_0210da08;
extern SharedFilePtr data_ov002_0210da28;
extern SharedFilePtr data_ov002_0210d9e8;
extern SharedFilePtr data_ov002_0210d9a8;

/* Scalar bridges agree with the implemented callees; shared member
 * reconstruction remains tracked in number-2744-source-forms.md. */
extern "C" {
extern int data_0209b454;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    TextureSequence *self, BTP_File *btp, s32 flags, s32 speed, u16 startFrame);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned int id, int x, int y, int z);
dActor_c *func_ov002_020f0918(dActor_c *number, dActor_c *owner);
}

// @symbol daObjNumber_c_classInit
/* classInit is a reconstructed factory name; the class identity comes from RTTI. */
extern "C" daObjNumber_c *daObjNumber_c_classInit(void)
{
    return new daObjNumber_c();
}

// @symbol _ZN13daObjNumber_c13InitResourcesEv
s32 daObjNumber_c::InitResources()
{
    /* SharedFilePtr has no recovered loaded-file member yet; it lives at +4. */
    if (param1 & 0x10) {
        TextureSequence::LoadFile(data_ov002_0210da08);
        void *m = Model::LoadFile(data_ov002_0210da28);
        if (mModel.SetFile((BMD_File *)m, 1, 1) == 0)
            return 0;
        TextureSequence::Prepare(
            **(BMD_File **)((char *)&data_ov002_0210da28 + 4),
            **(BTP_File **)((char *)&data_ov002_0210da08 + 4));
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence,
            *(BTP_File **)((char *)&data_ov002_0210da08 + 4),
            0x40000000, 0, 0);
        mTextureSequence.currFrame =
            (int)((((unsigned int)(param1 & 0xf) % 10) << 16) >> 4);
    } else {
        TextureSequence::LoadFile(data_ov002_0210d9e8);
        void *m = Model::LoadFile(data_ov002_0210d9a8);
        if (mModel.SetFile((BMD_File *)m, 1, 1) == 0)
            return 0;
        TextureSequence::Prepare(
            **(BMD_File **)((char *)&data_ov002_0210d9a8 + 4),
            **(BTP_File **)((char *)&data_ov002_0210d9e8 + 4));
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence,
            *(BTP_File **)((char *)&data_ov002_0210d9e8 + 4),
            0x40000000, 0, 0);
        mTextureSequence.currFrame =
            (int)((((unsigned int)(param1 & 0xf) % 10) << 16) >> 4);
    }

    mState = 0;
    mStartPos.x = mPosX;
    mStartPos.y = mPosY;
    mStartPos.z = mPosZ;
    mVertSpeed = 0x14000;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    mDelay = 0;
    mOwnerUniqueID = 0;
    mFollowOffsetY = 0;
    return 1;
}

// @symbol _ZN13daObjNumber_c8BehaviorEv
s32 daObjNumber_c::Behavior()
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
            mVertSpeed = 0xf000;
            ++mState;
        }
        break;
    case 1:
        if (mPosY < mStartPos.y) {
            /* The base header still splits position into scalar members. */
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0xd2, mPosX, reinterpret_cast<const Vector3 *>(&mPosX)->y, mPosZ);
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
            /* Retain the vector view until the shared position layout is recovered. */
            const Vector3 *op = reinterpret_cast<const Vector3 *>(&other->mPosX);
            s32 oy;
            pos.x = op->x;
            pos.y = oy = op->y;
            pos.z = op->z;
            pos.y = oy + (mFollowOffsetY + (mPosY - mStartPos.y));
        }
    }
    Matrix4x3_FromTranslation(
        &mModel.mat4x3,
        pos.x >> 3, pos.y >> 3, pos.z >> 3);
    return 1;
}

// @symbol _ZN13daObjNumber_c6RenderEv
s32 daObjNumber_c::Render()
{
    if (data_0209b454 & 0x4000000) {
        dActor_c *a = 0;
        u32 id = 0xb2;
        for (;;) {
            if ((a = FindWithActorID(id, a)) == 0)
                break;
            /* The star's halfword at 0x496 is still inside unrecovered padding. */
            if (static_cast<PowerStar *>(a)->unk_43c == 6
                && *(u16 *)((char *)a + 0x496) == 0x64) {
                MarkForDestruction();
                return 1;
            }
        }
    }

    if (mDelay != 0) {
        mDelay--;
        return 1;
    }

    mTextureSequence.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjNumber_c16CleanupResourcesEv
s32 daObjNumber_c::CleanupResources()
{
    if (param1 & 0x10) {
        data_ov002_0210da28.Release();
        data_ov002_0210da08.Release();
    } else {
        data_ov002_0210d9a8.Release();
        data_ov002_0210d9e8.Release();
    }
    return 1;
}

// @symbol func_ov002_020f0918
/* Attach the popup without changing the pointer returned by SpawnNumber. */
extern "C" dActor_c *func_ov002_020f0918(dActor_c *number, dActor_c *owner)
{
    daObjNumber_c *self = static_cast<daObjNumber_c *>(number);
    if (!owner)
        return number;
    self->mOwnerUniqueID = owner->uniqueID;
    self->mFollowOffsetY = self->mPosY - owner->mPosY;
    return number;
}
