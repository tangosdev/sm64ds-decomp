//cpp
/* daObjCasket_c -- CASKET, a dBgActor_c in ov071, .text 0x02121fe4..0x021226a0
 * (14 functions). It lies flat until a player comes near, swings up on end for
 * a moment, and swings back down. Behavior runs one of two states out of a
 * table of { init, exec, name } entries at data_ov071_02122ecc, which
 * __sinit_ov071_02122a64 fills:
 *
 *   0 "WAIT"     init St_Wait_Init     exec St_Wait_Main
 *   1 "STANDUP"  init St_StandUp_Init  exec St_StandUp_Main
 *
 * NAME: daObjCasket_c is the cartridge's RTTI spelling. _ZTS at ov071
 * 0x02122ea0 is the string "13daObjCasket_c", and the _ZTI at 0x02122e94
 * names the vtable at 0x02122efc as this class's. The tree called the class
 * Coffin until then.
 *
 * THE DESTRUCTOR IS INLINE AND EMPTY in the class header, so InitResources is
 * the key function and this TU emits _ZTV13daObjCasket_c, _ZTI13daObjCasket_c
 * and _ZTS13daObjCasket_c with the inherited bases' RTTI records. D1 and D0 are
 * emitted from the header, so there is no destructor text here to mark.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Leftover: nothing here casts this to char *, and no named field is
 *   read through a raw numeric offset. Each cleaner spelling below was
 *   measured with mwccarm 2004/b56 and reverted.
 * Leftover: assigning mMeshCollider.beforeClsnCallback instead of calling
 *   func_020393d4 sizes InitResources 0x110 against the ROM's 0x114.
 *   func_020393d4 is that store (p[6] = v), into dBgW::beforeClsnCallback
 *   at +0x18. The call stays.
 * Leftover: dBgW_KcMbg::SetFile as a member, with Fix12<int> scale.val
 *   set to 0x199, sizes InitResources 0x120 against 0x114. The scalar
 *   extern stays.
 * Leftover: dBgActor_c does not declare IsClsnInRange. Declaring
 *   IsClsnInRange(Fix12<int>, Fix12<int>) and calling it sizes Behavior
 *   0xac against 0x90. The scalar extern stays.
 * Leftover: dActor_c does not declare Earthquake. Declaring
 *   Earthquake(const Vector3 &, Fix12<int>) and calling it sizes
 *   St_Wait_Main 0x200 against 0x1f4. The scalar extern stays.
 * Leftover: copying closest->mPosX..mPosZ by name sizes St_Wait_Main
 *   0x1f0 against 0x1f4. The Vector3 pointer through mPosX is the match.
 * Leftover: a Vector3 built from mPosX..mPosZ sizes InitResources 0x12c
 *   against 0x114 at Vec3_Add, and St_Wait_Main 0x224 against 0x1f4 at
 *   the two AddVec3 calls. (Vector3 *)&mPosX stays. dActor_c has no
 *   Vector3 position member.
 * Leftover: a Vector3 built from mCamSpacePosX..Z for Sound::PlayBank3
 *   sizes St_Wait_Main 0x20c against 0x1f4, St_StandUp_Init 0x4c against
 *   0x28, and St_StandUp_Main 0xf8 against 0xcc. The in-place cast stays.
 * Leftover: mModel.mat4x3.t does not compile. common.h's flat Matrix4x3
 *   (s32 m[12]) is the one this TU sees, so the translation row is
 *   m[9], m[10] and m[11].
 * Leftover: data_ov071_02122ecc, data_ov071_021230d0 and
 *   data_ov071_021230d8 are defined at the end of this file so their
 *   dynamic init stays out of .text.
 */

#include "daObjCasket_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "Player.h"

typedef void (daObjCasket_c::*StateFunc)();

struct CasketState {
    StateFunc init;
    StateFunc exec;
    const char *name;
};

bool ApproachLinear(short &value, short target, short step);

/* Resource handles this TU's static initializer constructs. File IDs and
 * widths are the ROM's; constructor/destructor addresses are aliased in
 * the manifest (Eyerok idiom). */
struct CasketModelFilePtr : SharedFilePtr {
    u32 words[2];

    CasketModelFilePtr(u32 fileID);
    ~CasketModelFilePtr();
};

struct CasketCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    CasketCollisionFilePtr(u32 fileID);
    ~CasketCollisionFilePtr();
};

extern "C" {
/* Pointer-to-member descriptors the initializer copies into the state
 * table, and the state names (all ROM .data, owned by no TU). */
extern StateFunc data_ov071_02122e74;
extern StateFunc data_ov071_02122e7c;
extern StateFunc data_ov071_02122e84;
extern StateFunc data_ov071_02122e8c;
extern char data_ov071_02122e64[];
extern char data_ov071_02122e6c[];
/* The state table, indexed by mState. Defined at the end of this file. */
extern CasketState data_ov071_02122ecc[];
/* The casket's model and collision files. Defined at the end of this file. */
extern CasketModelFilePtr data_ov071_021230d0;
extern CasketCollisionFilePtr data_ov071_021230d8;
/* The CLPS block handed to dBgW_KcMbg::SetFile. */
extern CLPS_Block data_ov063_0211ebd8;
/* Scratch rotation matrix. */
extern Matrix4x3 data_020a0e68;

void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *out);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_020393d4(int *p, int v);

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 &mat, int scale,
    short angleY, CLPS_Block &clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, Vector3 *pos, int strength);
}

// @symbol daObjCasket_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjCasket_c through RTTI,
 * allocation size, vtable identity, and the CASKET registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: Coffin_Spawn. */
extern "C" daObjCasket_c *daObjCasket_c_classInit()
{
    return new daObjCasket_c();
}

// @symbol _ZN13daObjCasket_c13InitResourcesEv
int daObjCasket_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov071_021230d0), 1, -1);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    /* Moves the actor 200 units along its facing from where it was placed.
       St_Wait_Main measures from, and lands its dust at, the point 200 units
       back along the facing. */
    Vector3 localOffset;
    Vector3 worldOffset;
    localOffset.x = 0;
    localOffset.y = 0;
    localOffset.z = 0xc8000;
    worldOffset.x = 0;
    worldOffset.y = 0;
    worldOffset.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&localOffset, &data_020a0e68, &worldOffset);
    Vector3 newPos;
    Vec3_Add(&newPos, (Vector3 *)&mPosX, &worldOffset);
    mPosX = newPos.x;
    mPosY = newPos.y;
    mPosZ = newPos.z;

    UpdateModelTransform();
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov071_021230d8),
        mClsnMat, 0x199, mAngleY, data_ov063_0211ebd8);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    return 1;
}

// @symbol _ZN13daObjCasket_c8BehaviorEv
int daObjCasket_c::Behavior()
{
    /* Setting 1 is a casket that never moves. */
    if ((param1 & 0xff) == 1) {
        UpdateModelTransform();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
            UpdateClsnPosAndRot();
        return 1;
    }

    mBehaviorTimer++;
    RunState();
    UpdateModelTransform();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN13daObjCasket_c6RenderEv
int daObjCasket_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjCasket_c16CleanupResourcesEv
int daObjCasket_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov071_021230d0.Release();
    data_ov071_021230d8.Release();
    return 1;
}

// @symbol _ZN13daObjCasket_c8RunStateEv
/* Runs the current state's exec function. */
void daObjCasket_c::RunState()
{
    (this->*data_ov071_02122ecc[mState].exec)();
}

// @symbol _ZN13daObjCasket_c8SetStateEi
/* Enters a state and runs its init function. */
void daObjCasket_c::SetState(int state)
{
    mState = state;
    (this->*data_ov071_02122ecc[mState].init)();
}

// @symbol _ZN13daObjCasket_c12St_Wait_InitEv
/* State 0 init, WAIT. */
void daObjCasket_c::St_Wait_Init()
{
    mAngleStep = 0;
    mStateTimer = 60;
}

// @symbol _ZN13daObjCasket_c12St_Wait_MainEv
/* State 0 exec, WAIT. While mAngleX is not 0, swing back down, gathering
 * speed toward -0x7d0 a frame; on reaching 0, call dActor_c::Earthquake at the
 * actor's position, spawn dust at the point 200 units back along the facing and
 * play sound 0x5a. Once flat, go to STANDUP when a player is within 300 units
 * (horizontally) of that same point and the wait timer has run out. */
void daObjCasket_c::St_Wait_Main()
{
    Vector3 landLocal, landWorld;
    Vector3 playerPos;
    Vector3 reachLocal, reachWorld;
    Vector3 quakePos;
    Vector3 dustPos;

    if (mAngleX != 0) {
        ApproachLinear(mAngleStep, -0x7d0, 0xc8);
        if (ApproachLinear(mAngleX, 0, -mAngleStep) == 0)
            return;
        quakePos.x = mPosX;
        quakePos.y = mPosY;
        quakePos.z = mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &quakePos, 0x5dc000);

        landLocal.x = 0;
        landLocal.y = 0;
        landLocal.z = -0xc8000;
        landWorld.x = 0;
        landWorld.y = 0;
        landWorld.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&landLocal, &data_020a0e68, &landWorld);
        AddVec3(&landWorld, (Vector3 *)&mPosX, &landWorld);
        dustPos.x = landWorld.x;
        dustPos.y = landWorld.y;
        dustPos.z = landWorld.z;
        LandingDustAt(dustPos, true);
        Sound::PlayBank3(0x5a, *(Vector3 *)&mCamSpacePosX);
    } else {
        Vector3 *playerPosPtr;
        Player *closest = ClosestPlayer();
        if (closest == 0)
            return;
        /* Through a pointer: copied straight off the player, it is one instruction short. */
        playerPosPtr = (Vector3 *)&closest->mPosX;
        playerPos.x = playerPosPtr->x;
        playerPos.y = playerPosPtr->y;
        playerPos.z = playerPosPtr->z;
        reachLocal.x = 0;
        reachLocal.y = 0x64000;
        reachLocal.z = -0xc8000;
        reachWorld.x = 0;
        reachWorld.y = 0;
        reachWorld.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&reachLocal, &data_020a0e68, &reachWorld);
        AddVec3(&reachWorld, (Vector3 *)&mPosX, &reachWorld);
        if (Vec3_HorzDist(&reachWorld, &playerPos) >= 0x12c000)
            return;
        if (DecIfAbove0_Short(&mStateTimer) != 0)
            return;
        SetState(1);
    }
}

// @symbol _ZN13daObjCasket_c15St_StandUp_InitEv
/* State 1 init, STANDUP. */
void daObjCasket_c::St_StandUp_Init()
{
    mAngleStep = 0;
    mStateTimer = 60;
    Sound::PlayBank3(0x58, *(Vector3 *)&mCamSpacePosX);
}

// @symbol _ZN13daObjCasket_c15St_StandUp_MainEv
/* State 1 exec, STANDUP. Swing mAngleX up to 0x4000, gathering speed toward
 * 0x3e8 a frame. Then the timer counts down from 60: while it is 30 or less,
 * mAngleZ alternates between -0xc8 and 0xc8 each frame and sound 0x59 plays
 * every fourth frame. At zero, go back to WAIT and level mAngleZ. */
void daObjCasket_c::St_StandUp_Main()
{
    if (mAngleX != 0x4000) {
        ApproachLinear(mAngleStep, 0x3e8, 0xc8);
        ApproachLinear(mAngleX, 0x4000, mAngleStep);
        return;
    }
    DecIfAbove0_Short(&mStateTimer);
    if (mStateTimer == 0) {
        SetState(0);
        mAngleZ = 0;
        return;
    }
    if (mStateTimer > 30)
        return;
    if (mBehaviorTimer % 4 == 0)
        Sound::PlayBank3(0x59, *(Vector3 *)&mCamSpacePosX);
    mAngleZ = (mBehaviorTimer & 1) * 0x190 - 0xc8;
}

// @symbol _ZN13daObjCasket_c20UpdateModelTransformEv
/* Puts the model where the actor is. */
void daObjCasket_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3,
                                 mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* Definition order is the retail initializer's construction order: the
 * model handle, then the collision handle, then the state table whose
 * pointer-to-member slots copy the ROM descriptors. */
CasketModelFilePtr data_ov071_021230d0(0x5b4);
CasketCollisionFilePtr data_ov071_021230d8(0x5b5);

CasketState data_ov071_02122ecc[] = {
    { data_ov071_02122e74, data_ov071_02122e8c, data_ov071_02122e64 },
    { data_ov071_02122e84, data_ov071_02122e7c, data_ov071_02122e6c },
};
