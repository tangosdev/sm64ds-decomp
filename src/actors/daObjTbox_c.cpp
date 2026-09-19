//cpp
/* Production translation unit for ov064/daObjTbox_c.
 * 16 function(s), .text 0x0211a200..0x0211a930.
 *
 * The treasure chests of Jolly Roger Bay and the sunken ship (profile
 * TREASURE_BOX, actor 0x0d). Each chest carries a 1-based order number; a
 * player who touches them out of order is shocked and every other chest in
 * the room is reset. Opening the last one spawns this room's star.
 *
 * WHAT THE CARTRIDGE PROVES:
 *   _ZTS  ov064 0x0211c4d8  "11daObjTbox_c"
 *   _ZTI  ov064 0x0211c4cc  __si_class_type_info; one direct base, dActor_c
 *                           at arm9:0x0208e390, at offset zero
 *   _ZTV  ov064 0x0211c50c  the address point; 31 slots, the same extent as
 *                           dActor_c, with 0, 3, 6, 9, 16 and 17 overridden
 *   profile ov064 0x0211c4e8  g_profile_TREASURE_BOX, whose factory word is
 *                           daObjTbox_c_classInit at 0x0211a8f0
 *   size  0x178             the literal the factory hands operator new
 * The RTTI name is the ROM's own and is final; the coined `TreasureChest`
 * spelling this class used to be glossed with is gone.
 *
 * WHY THIS IS ONE TRANSLATION UNIT: build/tu_map.json isolates the fifteen
 * daObjTbox_c methods as a contiguous linker run, 0x0211a200..0x0211a8f0,
 * between daWater_Ring_c's factory and daObjShell_c's destructor, with no
 * foreign function inside it and no unlabelled neighbour at either edge. The
 * factory run that follows, 0x0211a8f0..0x0211a930, carries no mangled name
 * for tu_map to attribute, but it allocates this class's own 0x178, installs
 * this class's vptr, constructs this class's two members and is the address
 * the TREASURE_BOX profile stores -- and it ends exactly where the next
 * class's D1 begins. The join is registry/factory-backed rather than
 * tu_map-proven, the same evidence ov064/daObjFl_Gura_c was joined on.
 * config/arm9/overlays/ov064/delinks.txt enrolled all sixteen as adjacent
 * `complete` entries with no hole, so the whole run could be claimed at once.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder. The
 * inline destructor in include/daObjTbox_c.h emits the retail D1/D0 pair
 * first and emits no D2 body.
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim and dCcAc_c::Init keep their C ABI spellings in
 *   InitResources: their real declarations pass Fix12<int> by value, which
 *   mwccarm lowers differently at a C++ call site.
 * - func_02012790 / func_02012694 are still linker names; they are the sound
 *   and positional-sound entry points State0 uses.
 * - data_ov064_0211c98c, the three PMF pairs, and the four SharedFilePtr
 *   handles are overlay data this TU consumes and does not own.
 *
 * Consolidated from sixteen one-function sources, one per symbol below.
 * Their retired paths are recorded per function in this TU's manifest
 * entry; naming them here would leave the comment pointing at files the
 * fold deletes. ROM address order:
 *   [ 0] 0x0211a200  _ZN11daObjTbox_cD1Ev
 *   [ 1] 0x0211a238  _ZN11daObjTbox_cD0Ev
 *   [ 2] 0x0211a284  _ZN11daObjTbox_c20UpdateModelTransformEv
 *   [ 3] 0x0211a2c4  _ZN11daObjTbox_c6State2Ev
 *   [ 4] 0x0211a380  _ZN11daObjTbox_c10InitState2Ev
 *   [ 5] 0x0211a39c  _ZN11daObjTbox_c6State1Ev
 *   [ 6] 0x0211a49c  _ZN11daObjTbox_c10InitState1Ev
 *   [ 7] 0x0211a4c4  _ZN11daObjTbox_c6State0Ev
 *   [ 8] 0x0211a6e0  _ZN11daObjTbox_c10InitState0Ev
 *   [ 9] 0x0211a6ec  _ZN11daObjTbox_c8SetStateEi
 *   [10] 0x0211a734  _ZN11daObjTbox_c17CallStateBehaviorEv
 *   [11] 0x0211a77c  _ZN11daObjTbox_c16CleanupResourcesEv
 *   [12] 0x0211a7c4  _ZN11daObjTbox_c6RenderEv
 *   [13] 0x0211a7ec  _ZN11daObjTbox_c8BehaviorEv
 *   [14] 0x0211a814  _ZN11daObjTbox_c13InitResourcesEv
 *   [15] 0x0211a8f0  daObjTbox_c_classInit
 */

#include "daObjTbox_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *, s32);
/* Decrements a u16 in place and yields its new value. The two legacy shards
 * disagreed on the return type -- State0 used it as a statement and declared
 * it void, State2 tests the result. The value-returning form is the more
 * complete observation of the same arm9 routine, and both call sites compile
 * against it unchanged. */
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_02012790(u32 code);
s16 Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
s32 AngleDiff(s32 a, s32 b);
void func_02012694(u32 code, const Vector3 *pos);
/* The overlay's three {init, behavior} pointer-to-member pairs, filled by the
 * ov064 static initializer at 0x0211b65c. */
extern daObjTbox_c::StateFunc data_ov064_0211c98c[][2];
extern SharedFilePtr data_ov064_0211c96c;
extern SharedFilePtr data_ov064_0211c964;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov002_0210da38;
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed, u32 startFrame);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- daObjTbox_c_classInit, 0x0211a8f0, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjTbox_c_classInit
/* Reconstructed source-style name: the cartridge proves daObjTbox_c through
 * RTTI, allocation size and vtable identity, and the TREASURE_BOX profile at
 * 0x0211c4e8 takes this function's address as its factory slot; later EAD
 * lineage supplies the classInit spelling. The exact original spelling is not
 * preserved. Historical alias: TreasureChest_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`. The
 * literal 0x178 is the class's own size, handed to the leaf operator new the
 * header declares; then dActor_c's C2, this class's vptr store, and the
 * ModelAnim (0x0d4) and dCcAc_c (0x138) member constructors, in declaration
 * order. The null check is the one `new` itself emits, and the implicit
 * default constructor is what keeps that construction inlined -- declaring a
 * constructor of our own would emit a `bl` the factory does not have.
 *
 * The legacy one-function file this replaces had to spell the allocation as a
 * direct call to the mangled fBase_c::operator new, because without a leaf
 * `operator new` in scope `new daObjTbox_c` reached for the global one. The
 * header now declares that leaf, so the factory is a plain `new` and the ROM's
 * own allocator call is what the compiler emits.
 */
extern "C" daObjTbox_c *daObjTbox_c_classInit()
{
    return new daObjTbox_c();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN11daObjTbox_c13InitResourcesEv, 0x0211a814, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * ModelAnim::SetAnim and dCcAc_c::Init keep their C ABI spellings here. Their
 * real declarations pass Fix12<int> by value, which mwccarm lowers differently
 * at a C++ call site (the documented Fix12 caller wall); the scalar ABI forms
 * are the measured ROM calls. Everything else uses the recovered class API.
 */
int daObjTbox_c::InitResources()
{
    Model::LoadFile(data_ov002_0210d9a8);
    Model::LoadFile(data_ov002_0210da38);
    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(data_ov064_0211c96c), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        (BCA_File *)Animation::LoadFile(data_ov064_0211c964),
        0x40000000, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x96000, 0x96000, 0x200004, 0);
    UpdateModelTransform();
    mOrder = param1;
    mStarID = (u32)param1 >> 8;
    if (mStarID != 0xff) {
        mTrackedStarID = TrackStar(mStarID, 2);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN11daObjTbox_c8BehaviorEv, 0x0211a7ec, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c8BehaviorEv
int daObjTbox_c::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN11daObjTbox_c6RenderEv, 0x0211a7c4, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c6RenderEv
int daObjTbox_c::Render()
{
    Model *model = &mModelAnim;
    model->Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN11daObjTbox_c16CleanupResourcesEv, 0x0211a77c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Four releases, and the split is the finding: two handles live in this
 * overlay and two in ov002, borrowed from the always-resident module. The
 * ROM's order is own-first, borrowed-after, and is reproduced verbatim.
 *
 * `SharedFilePtr::Release` is now spelt as the member call it is, so the
 * compiler emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
int daObjTbox_c::CleanupResources()
{
    data_ov064_0211c96c.Release();
    data_ov064_0211c964.Release();
    data_ov002_0210d9a8.Release();
    data_ov002_0210da38.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN11daObjTbox_c17CallStateBehaviorEv, 0x0211a734, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c17CallStateBehaviorEv
void daObjTbox_c::CallStateBehavior()
{
    s32 index = mState;
    (this->*data_ov064_0211c98c[index][1])();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN11daObjTbox_c8SetStateEi, 0x0211a6ec, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c8SetStateEi
void daObjTbox_c::SetState(s32 state)
{
    mState = state;
    s32 index = mState;
    (this->*data_ov064_0211c98c[index][0])();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN11daObjTbox_c10InitState0Ev, 0x0211a6e0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c10InitState0Ev
void daObjTbox_c::InitState0()
{
    mModelAnim.currFrame = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN11daObjTbox_c6State0Ev, 0x0211a4c4, size 0x21c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c6State0Ev
void daObjTbox_c::State0()
{
    DecIfAbove0_Short(&mStateTimer);

    if (mStateTimer == 0x58)
        func_02012790(0xe);

    u32 id = mCylinder.otherOwner;
    if (id == 0)
        return;
    if (mStateTimer != 0)
        return;

    Player *player = (Player *)FindWithID(id);
    if (player == 0)
        return;

    {
        int t = player->actorID;
        t = t == 0xbf;
        if (t != false) {
            Vector3 *playerPosPtr = (Vector3 *)&player->mPosX;
            Vector3 playerPos;
            playerPos.x = playerPosPtr->x;
            playerPos.y = playerPosPtr->y;
            playerPos.z = playerPosPtr->z;
            if (AngleDiff(Vec3_HorzAngle((Vector3 *)&mPosX, &playerPos), mAngleY) < 0x4000) {
                int sbcount = 0;
                int count = 0;
                dActor_c *cur = FindWithActorID(0xd, 0);
                while (cur != 0) {
                    count++;
                    if (cur != this) {
                        s32 fv = ((daObjTbox_c *)cur)->mState;
                        int r1v = 1;
                        if (fv != 1) {
                            if (fv != 2)
                                r1v = 0;
                        }
                        if (r1v != 0)
                            sbcount++;
                    }
                    cur = FindWithActorID(0xd, cur);
                }

                if (sbcount + 1 == mOrder) {
                    if (count == mOrder) {
                        SpawnSoundObj(0);
                        mIsLastChest = 1;
                    } else {
                        func_02012790(0x26);
                    }
                    if (player->mIsUnderwater != 0)
                        func_02012694(0x22, (Vector3 *)&mCamSpacePosX);
                    else
                        func_02012694(0x20, (Vector3 *)&mCamSpacePosX);
                    SetState(1);
                } else {
                    mStateTimer = 0x5a;
                    if (player->mIsMetal != 0)
                        player->Shock(0);
                    else
                        player->Shock(1);

                    for (;;) {
                        cur = FindWithActorID(0xd, cur);
                        if (cur == 0)
                            break;
                        if (cur != this)
                            ((daObjTbox_c *)cur)->SetState(0);
                    }
                }
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN11daObjTbox_c10InitState1Ev, 0x0211a49c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c10InitState1Ev
void daObjTbox_c::InitState1()
{
    if (mIsLastChest != 0)
        mStateTimer = 0x2d;

    mFlags &= ~1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN11daObjTbox_c6State1Ev, 0x0211a39c, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c6State1Ev
void daObjTbox_c::State1()
{
    mModelAnim.Advance();
    if (((u32)(mModelAnim.currFrame << 4) >> 0x10) == 0x14) {
        Player *player = ClosestPlayer();

        Vector3 spawnPos;
        spawnPos.x = mPosX;
        spawnPos.y = mPosY;
        spawnPos.z = mPosZ;
        spawnPos.y += 0xc8000;

        if (mIsLastChest == 0) {
            Vector3 numberPos;
            numberPos.x = spawnPos.x;
            numberPos.y = spawnPos.y;
            numberPos.z = spawnPos.z;
            SpawnNumber(numberPos, mOrder, false, 0, 0);
        }

        if (player->mIsUnderwater != 0) {
            dActor_c *bubble = dActor_c::Spawn(
                0x123, 0, spawnPos, 0, mAreaId, -1);
            bubble->unk_0a4 = 0;
            bubble->mVertSpeed = 0x800;
            bubble->unk_0ac = 0;
        }
    }

    if (mModelAnim.Finished() != 0)
        SetState(2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN11daObjTbox_c10InitState2Ev, 0x0211a380, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c10InitState2Ev
void daObjTbox_c::InitState2()
{
    if (mIsLastChest == 0)
        mFlags |= 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN11daObjTbox_c6State2Ev, 0x0211a2c4, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c6State2Ev
void daObjTbox_c::State2()
{
    if (mIsLastChest == 0)
        return;
    if (mStateTimer == 0)
        return;
    if (DecIfAbove0_Short(&mStateTimer) != 0)
        return;

    if (mStarID != 0xff) {
        Vector3 spawnPos;
        spawnPos.x = mPosX;
        spawnPos.y = mPosY;
        spawnPos.z = mPosZ;
        spawnPos.y += 0xc8000;
        UntrackAndSpawnStar(mTrackedStarID, mStarID, spawnPos, 4);
    }

    mFlags |= 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN11daObjTbox_c20UpdateModelTransformEv, 0x0211a284, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_c20UpdateModelTransformEv
void daObjTbox_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.t.x = mPosX >> 3;
    mModelAnim.mat4x3.t.y = mPosY >> 3;
    mModelAnim.mat4x3.t.z = mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11daObjTbox_cD0Ev, 0x0211a238, size 0x4c */
/* ROM ordinal 0 -- _ZN11daObjTbox_cD1Ev, 0x0211a200, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjTbox_cD1Ev
// @symbol _ZN11daObjTbox_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjTbox_c() {}` in the header is
 * the whole source of both variants: from an inline body mwccarm emits D1 and
 * then D0 -- the cartridge's own order -- and no leaf D2. Written out of line
 * here instead, the two legacy shards' body reproduces the same bytes but the
 * variants come out D0-before-D1, and the isolation step rejects the object.
 *
 * Every instruction in both is compiler-generated teardown implied by
 * `struct daObjTbox_c : dActor_c`: this class's own vptr store, then dCcAc_c
 * at 0x138 and ModelAnim at 0x0d4 in reverse construction order, then the
 * dActor_c base. D0 additionally returns the object to the actor heap through
 * the inline operator delete.
 */
