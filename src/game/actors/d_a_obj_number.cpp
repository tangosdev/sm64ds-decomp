//cpp
/* d_a_obj_number.cpp
 * Object - floating score popup (NUMBER / 330)
 *
 * ov002/daObjNumber_c, one genuine translation unit, 8 functions,
 * .text 0x020f0894..0x020f0dd0. The class name is the cartridge's own
 * (_ZTS13daObjNumber_c). Direct base dActor_c.
 *
 * mwccarm emits ordinary text in reverse source order, so the ROM-high
 * factory is defined first. The destructor is inline in the header;
 * InitResources is the key function and this TU owns the RTTI/vtable.
 *
 * What stays:
 * - data_ov002_0210d9a8 / 0210d9e8 / 0210da08 / 0210da28: this TU
 *   consumes the four handles; overlay .bss owns them. symbols.txt has
 *   no recovered name, so they are not coined.
 * - volatile mPosY in Behavior's landing puff: a plain field read
 *   CSE's with the add and store above it (likely the Fix12<int> class
 *   wall; mPosY, mPos.y and Vector3 copies are all 4 bytes off).
 * - Render's star walk keeps the `for (;;)` with a break: a `while`
 *   on the FindWithActorID result DIFFs.
 * - SetOwner is a non-virtual member returning this: the only caller,
 *   arm9 dActor_c::SpawnNumber, tail-calls it and returns the result.
 * - g_profile_OBJ_NUMBER is overlay .data, not licensed to this TU.
 */

#include "daObjNumber_c.h"
#include "daStar_c.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx.h"

/* Behavior's position is a VecFx32, preserving the three-word position ABI
 * without emitting Vector3's vague-linkage destructor. */

extern SharedFilePtr data_ov002_0210da08;
extern SharedFilePtr data_ov002_0210da28;
extern SharedFilePtr data_ov002_0210d9e8;
extern SharedFilePtr data_ov002_0210d9a8;

extern u32 gNextActorUpdateMask;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjNumber_c through RTTI,
 * allocation size, vtable identity, and the OBJ_NUMBER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Number_Spawn / daObjNumber_c_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x020f0d94 loads 336 = 0x150 -- the class's own size -- into
 * fBase_c::operator new; dActor_c's C2, this class's vptr, dExtSimpleModel_c C1 at
 * +0xd4 and dExtAnmTexPat_c C1 at +0x124 are the implicit constructor. */
extern "C" daObjNumber_c *daObjNumber_c_classInit(void) {
    return new daObjNumber_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjNumber_c::InitResources() {
    if (param1 & 0x10) {
        dExtAnmTexPat_c::LoadFile(data_ov002_0210da08);
        BMD_File *m = dExtSimpleModel_c::LoadFile(data_ov002_0210da28);
        if (mModel.SetFile(m, 1, 1) == 0)
            return 0;
        dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210da28.filePtr,
                                 *(BTP_File *)data_ov002_0210da08.filePtr);
        mTextureSequence.SetFile(*(BTP_File *)data_ov002_0210da08.filePtr, 0x40000000, 0, 0);
        mTextureSequence.currFrame = (int)((((unsigned int)(param1 & 0xf) % 10) << 16) >> 4);
    } else {
        dExtAnmTexPat_c::LoadFile(data_ov002_0210d9e8);
        BMD_File *m = dExtSimpleModel_c::LoadFile(data_ov002_0210d9a8);
        if (mModel.SetFile(m, 1, 1) == 0)
            return 0;
        dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210d9a8.filePtr,
                                 *(BTP_File *)data_ov002_0210d9e8.filePtr);
        mTextureSequence.SetFile(*(BTP_File *)data_ov002_0210d9e8.filePtr, 0x40000000, 0, 0);
        mTextureSequence.currFrame = (int)((((unsigned int)(param1 & 0xf) % 10) << 16) >> 4);
    }

    mState = 0;
    mStartPosX = mPosX;
    mStartPosY = mPosY;
    mStartPosZ = mPosZ;
    mVertSpeed = 0x14000;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    mDelay = 0;
    mOwnerUniqueID = 0;
    mFollowOffsetY = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjNumber_c::Behavior() {
    Vector3 pos;

    if (mDelay != 0)
        return 1;

    mVertSpeed += mVertAccel;
    if (mVertSpeed < mTerminalVelocity)
        mVertSpeed = mTerminalVelocity;
    mPosY += mVertSpeed;

    switch (mState) {
    case 0:
        if (mPosY < mStartPosY) {
            mPosY = mStartPosY;
            mVertSpeed = 0xf000;
            mState++;
        }
        break;
    case 1:
        if (mPosY < mStartPosY) {
            /* volatile mPosY: a plain field read CSE's with the add above. */
            Particle::System::NewSimple(kPtclItemVanish, mPosX, *(volatile int *)&mPosY, mPosZ);
            MarkForDestruction();
        }
        break;
    }

    pos = mPos;
    if (mOwnerUniqueID != 0) {
        dActor_c *other = FindWithID(mOwnerUniqueID);
        if (other != 0) {
            {
                const Vector3 &op = other->mPos;
                pos = op;
            }
            pos.y += mFollowOffsetY + (mPosY - mStartPosY);
        }
    }
    Matrix4x3_FromTranslation(&mModel.mat4x3, pos.x >> 3, pos.y >> 3, pos.z >> 3);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjNumber_c::Render() {
    /* While gNextActorUpdateMask bit 0x4000000 is set, a power star (0xb2) of
       kind 6 at camera step 0x64 removes the popup. */
    if (gNextActorUpdateMask & 0x4000000) {
        daStar_c *a = 0;
        u32 id = 0xb2;
        for (;;) {
            if ((a = (daStar_c *)FindWithActorID(id, a)) == 0)
                break;
            if (a->mKind == daStar_c::KIND_6 && a->mCamSeq == 0x64) {
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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjNumber_c::CleanupResources() {
    if (param1 & 0x10) {
        data_ov002_0210da28.Release();
        data_ov002_0210da08.Release();
    } else {
        data_ov002_0210d9a8.Release();
        data_ov002_0210d9e8.Release();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* dActor_c::SpawnNumber's attach step (coined name; formerly
 * func_ov002_020f0918). Follows the owner by uniqueID, at this popup's
 * height above it. Returns this, which SpawnNumber hands back. */
daObjNumber_c *daObjNumber_c::SetOwner(dActor_c *owner) {
    if (!owner)
        return this;
    mOwnerUniqueID = owner->uniqueID;
    mFollowOffsetY = mPosY - owner->mPosY;
    return this;
}
