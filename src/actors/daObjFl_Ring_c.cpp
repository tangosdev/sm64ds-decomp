//cpp
/* Production translation unit for ov022/daObjFl_Ring_c, hand-curated.
 * 6 function(s), .text 0x021111a0..0x02111558.
 *
 * Lethal Lava Land's spinning fire ring of the FL_RING profile: a dBgActor_c
 * that turns about Y at a rate it approaches on its own Z channel, and while
 * that rate is up drops the actor 0xf3 flames that ride it. ov022 is Lethal
 * Lava Land.
 *
 * NAME: daObjFl_Ring_c is the cartridge's RTTI spelling -- _ZTS at ov022
 * 0x02113ce0 is the byte string "14daObjFl_Ring_c", and _ZTI at 0x02113cd4
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c].
 *
 * THE DESTRUCTOR IS THIS CLASS'S KEY FUNCTION, so this TU also emits
 * _ZTV14daObjFl_Ring_c, _ZTI14daObjFl_Ring_c and _ZTS14daObjFl_Ring_c as
 * vague linkage, alongside the inherited bases' RTTI records. Every one of
 * them has a configured ROM home, so all of them license as deadstrip-data
 * and the six-function object isolates cleanly. The destructor is declared
 * and defined inline and empty in the class header -- see that file for why
 * the body must not move here, and note that there is deliberately no
 * destructor text in this .cpp for an @symbol marker to sit above.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * compiler chooses where the D1/D0 pair lands on its own.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled in
 *   this TU -- both take Fix12<int> by value (wall 6az); a member call homes
 *   the argument and size-DIFFs InitResources / Behavior.
 * Leftover: func_020393d4 / func_020393c4 are 4-byte stores into dBgW's
 *   callback slots, and func_020393a4 / func_02039394 write its collision
 *   extents. This TU calls them; naming belongs with dBgW in arm9.
 * Leftover: the three pointers this class's file table holds -- the ov022
 *   .bss SharedFilePtrs at 0x02114500 and 0x02114508, and the ov064 CLPS
 *   block at 0x0211bbac -- are unnamed rows in their own modules' data, which
 *   this TU does not own.
 * Leftover: func_ov022_02111564, the collision callback InitResources
 *   installs, sits at 0x02111564 just past this run's right edge and is still
 *   a one-function C source; so is the factory daObjFl_Ring_c_classInit at
 *   0x02111578. tu_map.py cuts those three as their own candidate
 *   (ov022/@02111558-021115a8), so they are deliberately out of scope here.
 * Leftover: g_profile_FL_RING, the registry row that names that factory, is
 *   ov022 .data at 0x02113cf4 and lives outside this TU.
 */

#include "daObjFl_Ring_c.h"
#include "SharedFilePtr.h"

/* This class's own file table, ov022 .data at 0x02113cc8: the model and
 * collision SharedFilePtrs (ov022 .bss 0x02114500 / 0x02114508) and the CLPS
 * block SetFile is handed (ov064 0x0211bbac). All three targets are unnamed
 * rows in their modules' symbol tables, so the table keeps its
 * address-derived name. */
struct daObjFl_Ring_c_Files {
    SharedFilePtr *mModelFile;
    SharedFilePtr *mClsnFile;
    void          *mClps;
};

extern "C" {
extern daObjFl_Ring_c_Files data_ov022_02113cc8;

u16 DecIfAbove0_Short(u16 *p);
u8  DecIfAbove0_Byte(u8 *p);
int ApproachAngle(s16 *p, int target, int a, int b, int c);

void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, void *pos);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorId, u32 param, void *pos, void *rot, int areaId, int mapObjId);

void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, s16 angleY, void *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

void func_020393a4(void *p, int v);
void func_02039394(void *p, int v);
void func_020393d4(void *p, void *fn);
void func_020393c4(void *p, void *fn);
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];
void func_ov022_02111564(void *);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjFl_Ring_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so that one call deliberately keeps the measured register-level spelling. */
s32 daObjFl_Ring_c::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov022_02113cc8.mModelFile);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov022_02113cc8.mClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x199, mAngleY,
        data_ov022_02113cc8.mClps);

    func_020393d4(&mMeshCollider, _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(&mMeshCollider, (void *)func_ov022_02111564);

    /* The ring's own Z channel carries its spin rate, not an angle: it rests
       at -0x100 per frame unless the map row gave it one. */
    mPrevAngleZ = -0x100;
    if (mAngleZ != 0)
        mPrevAngleZ = mAngleZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjFl_Ring_c8BehaviorEv
/* IsClsnInRange takes two Fix12<int> by value; see InitResources for why that
 * one call keeps the measured register-level spelling. */
s32 daObjFl_Ring_c::Behavior()
{
    switch (mState) {
    case 0:
        if (DecIfAbove0_Short(&mCooldown) != 0)
            break;
        if (mTriggered == 0)
            break;
        if (ApproachAngle(&mPrevAngleZ, 0, 0xf, 0x30, 2) != 0)
            break;
        mCooldown = 0x96;
        mState = 1;
        _ZN5Sound9PlayBank3EjRK7Vector3(0xc, &mCamSpacePosX);
        break;

    case 1:
        if (DecIfAbove0_Short(&mCooldown) == 0) {
            if (ApproachAngle(&mPrevAngleZ, -0x100, 0xf, 0x30, 2) != 0)
                break;
            mCooldown = 0x96;
            mState = 0;
            break;
        }
        {
            int offscreen;
            offscreen = (int)((mFlags & 8) != 0);
            if (offscreen != 0)
                break;
        }
        if (DecIfAbove0_Byte(&mSpawnGuard) != 0)
            break;
        {
            int v[3];
            void *s;
            v[0] = mPosX;
            v[1] = mPosY;
            v[2] = mPosZ;
            v[1] = mPosY + 0x1f4000;
            s = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xf3, 0, v, 0, mAreaId, -1);
            *(void **)((u8 *)s + 0x10c) = this;
            *(int *)((u8 *)s + 0x118) = mPosY;
            {
                u16 *t = &mSpawnCount;
                *t = *t + 1;
            }
            mSpawnGuard = 2;
        }
        break;
    }

    {
        s16 *ip = &mPrevAngleY;
        *ip = (s16)(*ip + mPrevAngleZ);
    }
    mAngleY = mPrevAngleY;

    func_020393a4(&mMeshCollider, 0x780000);
    func_02039394(&mMeshCollider, 0x1000);
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x780000, 0x1000) != 0)
        UpdateClsnPosAndRot();

    mTriggered = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjFl_Ring_c6RenderEv
s32 daObjFl_Ring_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjFl_Ring_c16CleanupResourcesEv
s32 daObjFl_Ring_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov022_02113cc8.mModelFile->Release();
    data_ov022_02113cc8.mClsnFile->Release();
    return 1;
}
