//cpp
/* Production translation unit for ov072/daBgSnmHed_c.
 * 19 function(s), .text 0x0211ff34..0x021207d4. The snowman's head
 * (BIG_SNOWMAN_HEAD).
 *
 * NAME: _ZTS12daBgSnmHed_c is "12daBgSnmHed_c" at ov072 0x02122888; _ZTI at
 * 0x0212287c reads [__si_class_type_info, that string, _ZTI8dActor_c]. The
 * factory's historical alias was SnowmanHead_Spawn (coined).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x0211ff34), D0
 * (0x0211ff7c), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * The factory daBgSnmHed_c_classInit (0x021207d4) sits just past this run's
 * right edge and stays a one-function source.
 *
 * Leftover: TextureSequence::SetFile, dActor_c::Earthquake,
 *   Particle::System::NewSimple, Sound::PlaySub, dCcAc_c::Init and
 *   dBgCh_Actr::Init take Fix12<int> by value, so they stay mangled.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daBgSnmHed_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"

struct BMD_File;
struct BTP_File;

extern "C" daBgSnmHed_c::StateFunc data_ov072_02122c00[];
extern int data_ov072_02122bcc[];
extern int data_ov072_02122bd4[];

extern "C" {
void Matrix4x3_FromRotationY(void *matrix, int angle);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void *self, void *file, int flags, int speed, unsigned int startFrame);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
    void *self, const void *pos, int magnitude);
int func_0201267c(int id, void *pos);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned int id, int x, int y, int z);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    unsigned int soundID, unsigned int volume, unsigned int pan,
    int distance, int loop);
unsigned char DecIfAbove0_Byte(unsigned char *value);
int Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *t, void *a, int b, int c, unsigned int d, unsigned int e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *t, void *a, int b, int c, void *d, void *e);
}

int ApproachLinear(short &value, short target, short step);

// @symbol _ZN12daBgSnmHed_cD1Ev
// @symbol _ZN12daBgSnmHed_cD0Ev
daBgSnmHed_c::~daBgSnmHed_c()
{
}

// @symbol _ZN12daBgSnmHed_c11UpdateModelEv
void daBgSnmHed_c::UpdateModel()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = (mPosY + 0x96000) >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN12daBgSnmHed_c6State3Ev
int daBgSnmHed_c::State3()
{
    struct Position {
        s32 x;
        s32 y;
        s32 z;
    } pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x1c2000;

    switch (mSubstate) {
    case 0:
        mTalkPlayer = ClosestPlayer();
        if (Vec3_HorzDist(&mPosX, &mTalkPlayer->mPosX) < 0x118000) {
            if (mTalkPlayer->StartTalk(*this, true))
                mSubstate++;
        }
        break;
    case 1:
        if (mTalkPlayer->ShowMessage(
                *this, 0xb1, (Vector3 *)&pos, 0, 2))
            mSubstate++;
        break;
    case 2:
        if (mTalkPlayer->GetTalkState() == -1) {
            unsigned int param = (*(int *)&param1) & 0xf;
            param = param & 0xff;
            param |= 0x40;
            Spawn(0xb2, param, *(Vector3 *)&mPosX,
                  0, mAreaId, -1);
            mSubstate++;
        }
        break;
    }
    mTextureSequence.Advance();
    ApproachLinear(mAngleY, -0x4000, 0x514);
    mPosY = (int)0xffc427c0;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState3Ev
int daBgSnmHed_c::InitState3()
{
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence, (void *)data_ov072_02122bcc[1], 0, 0x1000, 0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mSubstate = 0;
    mStateValue = 3;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c6State2Ev
int daBgSnmHed_c::State2()
{
    struct Position {
        s32 x;
        s32 y;
        s32 z;
    } pos;
    switch (mSubstate) {
    case 0:
        UpdatePos(&mCylinder);
        if (mVertSpeed < 0 && mPosY < (int)0xffc427c0) {
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                this, &pos, 0x5dc000);
            func_0201267c(0x115, &mCamSpacePosX);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x10f, mPosX, mPosY, mPosZ);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x110, mPosX, mPosY, mPosZ);
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x14, 0x7f, 0x15666, 0);
            mSubstate++;
        }
        break;
    case 1:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x7f, 0, 0x15666, 0);
            SetState(3);
        }
        break;
    }
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState2Ev
int daBgSnmHed_c::InitState2()
{
    mFlags &= ~1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mVertSpeed = 0x2d000;
    mStateTimer = 0x78;
    mSubstate = 0;
    mStateValue = 2;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c6State1Ev
int daBgSnmHed_c::State1()
{
    switch (mSubstate) {
    case 0:
    {
        short angle = Vec3_HorzAngle(&mPosX, &mTalkPlayer->mPosX);
        if (ApproachLinear(mAngleY, angle, 0x514)) {
            struct Position {
                s32 x;
                s32 y;
                s32 z;
            } pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y += 0xfa000;
            if (mTalkPlayer->ShowMessage(
                    *this, 0xaf, (Vector3 *)&pos, 0, 0))
                mSubstate++;
        }
        break;
    }
    case 1:
        if (mTalkPlayer->GetTalkState() == -1)
            SetState(0);
        break;
    }
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState1Ev
int daBgSnmHed_c::InitState1()
{
    mSubstate = 0;
    mStateTimer = 0x96;
    mStateValue = 1;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c6State0Ev
int daBgSnmHed_c::State0()
{
    ApproachLinear(mAngleY, -0x4000, 0x514);
    mTextureSequence.Advance();
    if (unk_336) {
        SetState(2);
        return 1;
    }
    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        Player *player = ClosestPlayer();
        if (Vec3_HorzDist(&mPosX, &player->mPosX) < 0x118000
            && player->StartTalk(*this, true) != 0) {
            mTalkPlayer = player;
            SetState(1);
        }
    }
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState0Ev
int daBgSnmHed_c::InitState0()
{
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence, (void *)data_ov072_02122bd4[1], 0, 0x1000, 0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mStateValue = 0;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c17CallStateBehaviorEv
void daBgSnmHed_c::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}

// @symbol _ZN12daBgSnmHed_c13CallStateInitEv
void daBgSnmHed_c::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}

// @symbol _ZN12daBgSnmHed_c8SetStateEi
void daBgSnmHed_c::SetState(int state)
{
    mStateFuncs = data_ov072_02122c00 + state * 2;
    CallStateInit();
}

// @symbol _ZN12daBgSnmHed_c16CleanupResourcesEv
int daBgSnmHed_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov072_02122bc4)->Release();
    int r5 = 0;
    do {
        ((SharedFilePtr **)&data_ov072_02121ffc)[r5]->Release();
        r5++;
    } while (r5 < 2);
    return 1;
}

// @symbol _ZN12daBgSnmHed_c16OnPendingDestroyEv
void daBgSnmHed_c::OnPendingDestroy()
{
}

// @symbol _ZN12daBgSnmHed_c6RenderEv
int daBgSnmHed_c::Render()
{
    mTextureSequence.Update(mModel.data);
    mModel.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN12daBgSnmHed_c8BehaviorEv
int daBgSnmHed_c::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    UpdateModel();
    return 1;
}

// @symbol _ZN12daBgSnmHed_c13InitResourcesEv
int daBgSnmHed_c::InitResources()
{
    struct Vector3 pos;
    int i;

    mModel.SetFile((BMD_File *)Model::LoadFile(
                       *(SharedFilePtr *)&data_ov072_02122bc4),
                   1, -1);

    for (i = 0; i < 2; i++) {
        void *tex = ((void **)&data_ov072_02121ffc)[i];
        TextureSequence::LoadFile(*(SharedFilePtr *)tex);
        TextureSequence::Prepare(*(BMD_File *)((void **)&data_ov072_02122bc4)[1],
                                 *(BTP_File *)((void **)tex)[1]);
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x96000, 0x12c000, 0x800004, 0);
    mScaleX = 0x1800;
    mScaleY = 0x1800;
    mScaleZ = 0x1800;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x96000, 0x96000, 0, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn() != 0)
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;

    SetState(0);
    UpdateModel();
    return 1;
}
