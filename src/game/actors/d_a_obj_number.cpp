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
 * deslop leftovers:
 * - TextureSequence::SetFile 6az: InitResources passes Fix12<int> by
 *   value (notes/mwccarm-codegen.md 6az).
 * - Particle::System::NewSimple 6az: Behavior's particle 0xd2; the
 *   header has no NewSimple and the mangled name carries Fix12<int>.
 * - SharedFilePtr +4: InitResources' Prepare/SetFile read the BMD/BTP
 *   at data_ov002_0210da28 / 0210da08 / 0210d9a8 / 0210d9e8 +4;
 *   SharedFilePtr.h has no fields.
 * - data_ov002_0210d9a8 / 0210d9e8 / 0210da08 / 0210da28: this TU
 *   consumes the four handles; overlay .bss owns them. symbols.txt has
 *   no recovered name, so they are not coined.
 * - NumberPos: a local Vector3 would emit vague-linkage ~Vector3.
 * - Behavior rematerializes mVertSpeed/mPosY/mState through (char *)
 *   casts; the named `+=` / `++` form shrinks the body 0x174 -> 0x144.
 * - volatile mPosY / pos.z: a plain field read CSE's with the add/store
 *   above it (Particle::System::NewSimple and FromTranslation).
 * - owner+0x5c: FindWithID result overlaid as NumberPos; dActor_c has
 *   no Pos() in this tree.
 * - POWER_STAR (0xb2) +0x43c / +0x496: Render walks stars while
 *   data_0209b454 bit 0x4000000 is set. PowerStar.h has unk_43c; 0x496
 *   sits in that class's pad. This leaf does not name those fields.
 * - func_ov002_020f0918 keeps its func_ name: not a vtable slot, and
 *   the only caller is arm9 dActor_c::SpawnNumber.
 * - Matrix4x3_FromTranslation stays the C helper (no class method).
 * - g_profile_OBJ_NUMBER is overlay .data, not licensed to this TU.
 */

#include "daObjNumber_c.h"
#include "SharedFilePtr.h"

/* POD view used only to preserve the three-word position ABI without
 * emitting Vector3's vague-linkage destructor. */
struct NumberPos {
    s32 x;
    s32 y;
    s32 z;
};

typedef char NumberPos_size_must_be_0xc[
    sizeof(NumberPos) == 0xc ? 1 : -1];

extern SharedFilePtr data_ov002_0210da08;
extern SharedFilePtr data_ov002_0210da28;
extern SharedFilePtr data_ov002_0210d9e8;
extern SharedFilePtr data_ov002_0210d9a8;

extern "C" {
extern int data_0209b454;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void *self, void *btp, int a, int b, unsigned int c);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned int id, int x, int y, int z);
void func_ov002_020f0918(daObjNumber_c *self, dActor_c *owner);
}

extern int _ZTV13daObjNumber_c[];

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjNumber_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjNumber_c through RTTI,
 * allocation size, vtable identity, and the OBJ_NUMBER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Number_Spawn / daObjNumber_c_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x020f0d94 loads 336 = 0x150 -- the class's own size -- into
 * fBase_c::operator new; dActor_c's C2, this class's vptr, Model C1 at
 * +0xd4 and TextureSequence C1 at +0x124 are the implicit constructor. */
extern "C" daObjNumber_c *daObjNumber_c_classInit(void)
{
    return new daObjNumber_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjNumber_c13InitResourcesEv
s32 daObjNumber_c::InitResources()
{
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
            *(void **)((char *)&data_ov002_0210da08 + 4),
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
            *(void **)((char *)&data_ov002_0210d9e8 + 4),
            0x40000000, 0, 0);
        mTextureSequence.currFrame =
            (int)((((unsigned int)(param1 & 0xf) % 10) << 16) >> 4);
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
// @symbol _ZN13daObjNumber_c8BehaviorEv
s32 daObjNumber_c::Behavior()
{
    NumberPos pos;

    if (mDelay != 0)
        return 1;

    /* Address rematerialization: `mVertSpeed += mVertAccel` / `mPosY +=`
       / `mState++` shrink the body 0x174 -> 0x144. */
    *(int *)((int)((char *)&mVertSpeed)) += mVertAccel;
    if (mVertSpeed < mTerminalVelocity)
        mVertSpeed = mTerminalVelocity;
    *(int *)((int)((char *)&mPosY)) += mVertSpeed;

    switch (mState) {
    case 0:
        if (mPosY < mStartPosY) {
            mPosY = mStartPosY;
            mVertSpeed = 0xf000;
            (*(u8 *)((int)((char *)&mState)))++;
        }
        break;
    case 1:
        if (mPosY < mStartPosY) {
            /* volatile mPosY: a plain field read CSE's with the add above. */
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0xd2, mPosX, *(volatile int *)((char *)&mPosY), mPosZ);
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
            /* owner+0x5c: dActor_c has no Pos() in this tree. NumberPos
               overlay of mPosX/Y/Z; a Vector3 would emit ~Vector3. */
            NumberPos *op = (NumberPos *)((int)((char *)other + 0x5c));
            s32 oy;
            pos.x = op->x;
            pos.y = oy = op->y;
            pos.z = op->z;
            pos.y = oy + (mFollowOffsetY + (mPosY - mStartPosY));
        }
    }
    /* volatile pos.z: a plain `pos.z >> 3` CSE's with the stores above. */
    Matrix4x3_FromTranslation(
        (void *)((char *)&mModel.mat4x3),
        pos.x >> 3, pos.y >> 3, (*(volatile int *)&pos.z) >> 3);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjNumber_c6RenderEv
s32 daObjNumber_c::Render()
{
    if (data_0209b454 & 0x4000000) {
        dActor_c *a = 0;
        u32 id = 0xb2;
        for (;;) {
            if ((a = FindWithActorID(id, a)) == 0)
                break;
            if (*(int *)((char *)a + 0x43c) == 6
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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f0918
/* dActor_c::SpawnNumber's attach helper. Not a vtable slot; the only
 * inbound call is arm9 SpawnNumber, so the func_ name stays. Parameters
 * are this popup and the owner SpawnNumber was given -- uniqueID at
 * owner+4, follow offset is this mPosY minus the owner's. */
extern "C" void func_ov002_020f0918(daObjNumber_c *self, dActor_c *owner)
{
    if (!owner)
        return;
    self->mOwnerUniqueID = owner->uniqueID;
    self->mFollowOffsetY = self->mPosY - owner->mPosY;
}
