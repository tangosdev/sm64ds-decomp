//cpp
/* daPgDfdr_c -- Penguin Defender (PENGUIN_DEFENDER 258), ov027.
 *
 * A solid penguin (it carries a KCL mesh) placed at a fixed spot that ignores
 * its spawn position. InitResources drops it onto the ground at
 * (0x6c4000, 0xcb2000, 0x182bb8) facing 0xdd30. It then patrols nine steps
 * from the stride-0xc table at data_ov027_02113a1c (distance, speed, start
 * heading, end heading). State 1 walks the current step; when the distance
 * is spent it advances the step and enters state 0, which waits 20 frames
 * and turns onto that step's end heading. Equal headings play the straight
 * walk clip (rate follows the step speed); a heading change plays the turn
 * clip. Footsteps are sound 0xf3.
 *
 * ov027 is mixed (sliding ice / chill bully / Bubba / snowman breath).
 * RTTI names this class daPgDfdr_c; the debug table names PENGUIN_DEFENDER.
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim as a method homes Fix12<int> (str, add, ldm; +12
 *   bytes per call). func_ov027_02111ca8 0x54 -> 0x60. func_ov027_02111b2c
 *   0x11c -> 0x134 (+24, two calls). The scalar extern "C" stays.
 * - TextureSequence::SetFile and dBgW_KcMbg::SetFile home one Fix12<int>
 *   each (+12), and dCcAc_c::Init homes two (+24). Together InitResources
 *   0x1c8 -> 0x1f8.
 * - dBgActor_c::IsClsnInRangeOnScreen(Fix12, Fix12) homes both zeros.
 *   Behavior 0x80 -> 0x9c. The scalar extern "C" stays.
 * - Sound::Play(3, 0xf3, pos) is +8 bytes in func_ov027_02111a28
 *   (0x104 -> 0x10c). The ROM calls func_0201267c.
 * - Assigning mMeshCollider.beforeClsnCallback is -4 bytes in
 *   InitResources (0x1c8 -> 0x1c4). The ROM calls func_020393d4.
 * - mStepIndex = mStepIndex + 1 is ldrb [this, #0x3d9] and drops the pooled
 *   add. func_ov027_02111a28 0x104 -> 0xf8.
 * - Naming mModelAnim.currFrame and .file in func_ov027_02111a28 reorders
 *   6 words (the straight-walk file loads after the frame). c+0x378 /
 *   c+0x380 and the ternary stay.
 */

#include "common.h"
#include "daPgDfdr_c.h"
#include "Player.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"

struct BMD_File;
struct BTP_File;

/* SharedFilePtr has no fields. This TU reads the file word at +4
   (sinit constructs these as SharedFilePtr; LoadFile/Release take the
   object, SetAnim/SetFile/Prepare read +4). */
struct FileRef {
    s32 refs;
    void *file;
};

/* Two 8-byte daPgDfdr_c member pointers (enter, then the per-frame tick). */
struct StatePair {
    int w[4];
};

enum {
    kStateTurn = 0,
    kStateWalk = 1,
    kStepCount = 9,
    /* dActor_c::mFlags 0x2: with the clip test, skip Render. */
    kSkipRender = 2
};

extern "C" {
void Matrix4x3_FromRotationY(void *mat, short angY);
int DecIfAbove0_Byte(void *p);
int _Z14ApproachLinearRsss(short *value, short target, short step);
/* Sound::Play(3, id, pos). The ROM calls this veneer, not Play itself. */
void func_0201267c(int id, char *pos);
/* Stores dBgW::beforeClsnCallback (p[6]). dBgW.h has no setter. */
void func_020393d4(int *collider, int callback);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int flags, int speed, unsigned startFrame);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *file, int flags, int speed, unsigned startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *kcl, void *mat, int scale, short angY, void *clps);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int radius, int height, unsigned flags, unsigned vuln);
void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int x, int z);

void func_ov027_02111994(daPgDfdr_c *self);
void func_ov027_02111b2c(daPgDfdr_c *self);
int func_ov027_02111c48(daPgDfdr_c *self);
int func_ov027_02111ca8(daPgDfdr_c *self);

extern FileRef data_ov027_02113c6c;
extern FileRef data_ov027_02113c7c;
extern FileRef data_ov027_02113c94;
extern FileRef data_ov027_02113c84;
extern FileRef data_ov027_02113c74;
extern FileRef data_ov027_02113c8c;
struct PatrolStep {
    s32 distance;
    s32 speed;
    s16 startHeading;
    s16 endHeading;
};
extern PatrolStep data_ov027_02113a1c[];
extern StatePair data_ov027_02113ce4[];
}

// @symbol daPgDfdr_c_classInit
extern "C" daPgDfdr_c *daPgDfdr_c_classInit()
{
    return new daPgDfdr_c();
}

// @symbol _ZN10daPgDfdr_c13InitResourcesEv
s32 daPgDfdr_c::InitResources()
{
    int i;
    void *file;
    Vector3 pos;

    file = Model::LoadFile(*(SharedFilePtr *)&data_ov027_02113c7c);
    mModelAnim.SetFile((BMD_File *)file, 1, -1);

    for (i = 0; i < 3; i++)
        Animation::LoadFile(*(SharedFilePtr *)data_ov027_02112ca4[i]);

    TextureSequence::LoadFile(*(SharedFilePtr *)&data_ov027_02113c94);
    TextureSequence::Prepare(*(BMD_File *)data_ov027_02113c7c.file,
                             *(BTP_File *)data_ov027_02113c94.file);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence, data_ov027_02113c94.file, 0, 0x1000, 0);

    mAngleY = (short)0xdd30;
    mPrevAngleY = mAngleY;
    mPosX = 0x6c4000;
    mPosY = 0xcb2000;
    mPosZ = 0x182bb8;
    func_ov027_02111994(this);

    file = dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov027_02113c6c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, file, &mClsnMat, 0x199, mAngleY, &data_ov027_021130e8);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosAndAngs);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x82000, 0xc8000, 0x800004, 0);
    func_ov027_02111d70(kStateWalk);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}

// @symbol _ZN10daPgDfdr_c8BehaviorEv
s32 daPgDfdr_c::Behavior()
{
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    func_ov027_02111cfc();
    if (ClosestPlayer()->IsInsideOfCannon()) {
        mFlags &= ~kSkipRender;
    } else {
        mFlags |= kSkipRender;
    }
    mModelAnim.Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    func_ov027_02111994(this);
    return 1;
}

// @symbol _ZN10daPgDfdr_c6RenderEv
s32 daPgDfdr_c::Render()
{
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN10daPgDfdr_c16OnPendingDestroyEv
void daPgDfdr_c::OnPendingDestroy()
{
}

// @symbol _ZN10daPgDfdr_c16CleanupResourcesEv
s32 daPgDfdr_c::CleanupResources()
{
    int i;
    (*(SharedFilePtr *)&data_ov027_02113c7c).Release();
    for (i = 0; i < 3; i++) {
        ((SharedFilePtr *)data_ov027_02112ca4[i])->Release();
    }
    (*(SharedFilePtr *)&data_ov027_02113c94).Release();
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    (*(SharedFilePtr *)&data_ov027_02113c6c).Release();
    return 1;
}

// @symbol _ZN10daPgDfdr_c19func_ov027_02111d70Ei
/* Select state idx and run its enter function. */
void daPgDfdr_c::func_ov027_02111d70(int idx)
{
    mStateTable = &data_ov027_02113ce4[idx];
    func_ov027_02111d38();
}

// @symbol _ZN10daPgDfdr_c19func_ov027_02111d38Ev
void daPgDfdr_c::func_ov027_02111d38()
{
    typedef void (daPgDfdr_c::*Fn)();
    Fn *fn = (Fn *)mStateTable;
    (this->**fn)();
}

// @symbol _ZN10daPgDfdr_c19func_ov027_02111cfcEv
void daPgDfdr_c::func_ov027_02111cfc()
{
    typedef void (daPgDfdr_c::*Fn)();
    Fn *fn = (Fn *)mStateTable + 1;
    (this->**fn)();
}

// @symbol func_ov027_02111ca8
/* State 0 enter: pause-and-turn clip, 20 frames, unk_3d0 = 0. */
extern "C" int func_ov027_02111ca8(daPgDfdr_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov027_02113c84.file, 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mTimer = 20;
    self->unk_3d0 = 0;
    return 1;
}

// @symbol func_ov027_02111c48
/* State 0 tick: after the pause, turn mAngleY toward this step's end heading. */
extern "C" int func_ov027_02111c48(daPgDfdr_c *self)
{
    if (DecIfAbove0_Byte(&self->mTimer) == 0) {
        unsigned char step = self->mStepIndex;
        short endHeading = data_ov027_02113a1c[step].endHeading;
        if (_Z14ApproachLinearRsss(&self->mAngleY, endHeading, 0x514) != 0) {
            self->func_ov027_02111d70(kStateWalk);
        }
    }
    return 1;
}

// @symbol func_ov027_02111b2c
/* State 1 enter: distance, speed and start heading of this step, then the
   straight-walk clip when the headings match and the turn clip when they do not. */
extern "C" void func_ov027_02111b2c(daPgDfdr_c *self)
{
    self->mDistanceLeft = data_ov027_02113a1c[self->mStepIndex].distance;
    self->mHorzSpeed = data_ov027_02113a1c[self->mStepIndex].speed;
    self->mPrevAngleY = data_ov027_02113a1c[self->mStepIndex].startHeading;
    if (data_ov027_02113a1c[self->mStepIndex].startHeading != data_ov027_02113a1c[self->mStepIndex].endHeading) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov027_02113c8c.file, 0, 0x1000, 0);
        self->mModelAnim.speed = 0x1000;
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov027_02113c74.file, 0, 0x1000, 0);
        self->mModelAnim.speed = (int)(((s64)data_ov027_02113a1c[self->mStepIndex].speed * 0x5000 + 0x800) >> 12);
    }
    self->unk_3d0 = 1;
}

// @symbol _ZN10daPgDfdr_c19func_ov027_02111a28Ev
/* State 1 tick: spend mDistanceLeft. At zero, wrap the step and enter the turn.
   Otherwise update position and play 0xf3 on the walk clip's footstep frames. */
int daPgDfdr_c::func_ov027_02111a28()
{
    int distLeft = mDistanceLeft;
    if (distLeft == 0) {
        /* 0x3d9 is mStepIndex. Named ++ is ldrb [this, #0x3d9] and drops the
           pooled add (this function 0x104 -> 0xf8). */
        unsigned char *step = (unsigned char *)((int)this + 0x3d9);
        *step = *step + 1;
        if (mStepIndex >= kStepCount) mStepIndex = 0;
        func_ov027_02111d70(kStateTurn);
        return 1;
    }
    {
        int speed = mHorzSpeed;
        if (distLeft < speed) {
            mHorzSpeed = distLeft;
            mDistanceLeft = 0;
        } else {
            mDistanceLeft -= speed;
        }
    }
    UpdatePos(&mdCcAc_c);
    {
        /* currFrame is +0x378 and the BCA is +0x380. Naming them reorders
           6 words; the ternary keeps the straight-walk file in the compare. */
        char *c = (char *)this;
        int frameRaw = *(int *)(c + 0x378);
        int walkAnim = (int)data_ov027_02113c74.file;
        int curAnim = walkAnim ? *(int *)(c + 0x380) : *(int *)(c + 0x380);
        unsigned int frame = (unsigned int)(frameRaw << 4) >> 0x10;
        if (curAnim == walkAnim) {
            if (frame == 0xa || frame == 0x16) {
                func_0201267c(0xf3, (char *)&mCamSpacePosX);
            }
        } else if (curAnim == (int)data_ov027_02113c8c.file) {
            if (frame == 9 || frame == 0x16) {
                func_0201267c(0xf3, (char *)&mCamSpacePosX);
            }
        }
    }
    return 1;
}

// @symbol func_ov027_02111994
/* Model matrix from mAngleY plus position >> 3; collider copy keeps the
   full position. Flat Matrix4x3 (common.h before Model.h) is the 12-word copy. */
extern "C" void func_ov027_02111994(daPgDfdr_c *self)
{
    Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    self->mModelAnim.mat4x3.m[9] = self->mPosX >> 3;
    self->mModelAnim.mat4x3.m[10] = self->mPosY >> 3;
    self->mModelAnim.mat4x3.m[11] = self->mPosZ >> 3;
    self->mClsnMat = self->mModelAnim.mat4x3;
    self->mClsnMat.m[9] = self->mPosX;
    self->mClsnMat.m[10] = self->mPosY;
    self->mClsnMat.m[11] = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat, self->mAngleY);
}
