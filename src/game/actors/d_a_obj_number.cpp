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
 * - gSilverNumberModelFile with gSNumberTextureSequenceFile (param1 bit
 *   4, the silver numbers), data_ov002_0210d9a8 with
 *   gNumberTextureSequenceFile (the red ones): this TU consumes the four
 *   handles; ov002_handles.cpp owns them. data_ov002_0210d9a8 is named
 *   together with the player's handles, which share it.
 * - Particle::System::NewSimple(u32, const Vector3 &) is defined in this
 *   file, not in its header: Behavior's landing puff is its only user,
 *   and a body in Particle__System.h renumbers every includer's @NNN.
 *   The puff reads mPos through that reference, so it reloads mPos.y
 *   after the landing test; three scalar arguments (mPos.x, mPos.y, mPos.z)
 *   reuse the test's load and come out 4 bytes short.
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

inline void *Particle::System::NewSimple(u32 id, const Vector3 &pos) {
    return NewSimple(id, pos.x, pos.y, pos.z);
}

extern SharedFilePtr gSNumberTextureSequenceFile;
extern SharedFilePtr gSilverNumberModelFile;
extern SharedFilePtr gNumberTextureSequenceFile;
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
daObjNumber_c *daObjNumber_c_classInit(void) {
    return new daObjNumber_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjNumber_c::InitResources() {
    if (param1 & 0x10) {
        dExtAnmTexPat_c::LoadFile(gSNumberTextureSequenceFile);
        BMD_File *m = dExtSimpleModel_c::LoadFile(gSilverNumberModelFile);
        if (mModel.SetFile(m, 1, 1) == 0)
            return 0;
        dExtAnmTexPat_c::Prepare(*(BMD_File *)gSilverNumberModelFile.filePtr,
                                 *(BTP_File *)gSNumberTextureSequenceFile.filePtr);
        mTextureSequence.SetFile(*(BTP_File *)gSNumberTextureSequenceFile.filePtr, 0x40000000, 0,
                                 0);
        mTextureSequence.currFrame = (int)((((param1 & 15) % 10) << 16) >> 4);
    } else {
        dExtAnmTexPat_c::LoadFile(gNumberTextureSequenceFile);
        BMD_File *m = dExtSimpleModel_c::LoadFile(data_ov002_0210d9a8);
        if (mModel.SetFile(m, 1, 1) == 0)
            return 0;
        dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210d9a8.filePtr,
                                 *(BTP_File *)gNumberTextureSequenceFile.filePtr);
        mTextureSequence.SetFile(*(BTP_File *)gNumberTextureSequenceFile.filePtr, 0x40000000, 0, 0);
        mTextureSequence.currFrame = (int)((((param1 & 15) % 10) << 16) >> 4);
    }

    mState = 0;
    mStartPos.x = mPos.x;
    mStartPos.y = mPos.y;
    mStartPos.z = mPos.z;
    mSpeed.y = 0x14000;
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

    mSpeed.y += mVertAccel;
    if (mSpeed.y < mTerminalVelocity)
        mSpeed.y = mTerminalVelocity;
    mPos.y += mSpeed.y;

    switch (mState) {
    case 0:
        if (mPos.y < mStartPos.y) {
            mPos.y = mStartPos.y;
            mSpeed.y = 0xf000;
            mState++;
        }
        break;
    case 1:
        if (mPos.y < mStartPos.y) {
            Particle::System::NewSimple(kPtclItemVanish, mPos);
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
            pos.y += mFollowOffsetY + (mPos.y - mStartPos.y);
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
        for (;;) {
            if ((a = (daStar_c *)FindWithActorID(0xb2, a)) == 0)
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
        gSilverNumberModelFile.Release();
        gSNumberTextureSequenceFile.Release();
    } else {
        data_ov002_0210d9a8.Release();
        gNumberTextureSequenceFile.Release();
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
    mFollowOffsetY = mPos.y - owner->mPos.y;
    return this;
}
