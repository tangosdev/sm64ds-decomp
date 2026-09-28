//cpp
/**
 * daPgRcer_c -- Cool, Cool Mountain racing penguin (PENGUIN_RACER 259), ov019.
 *
 * He waits, talks the player into a race, then runs the path in param1's low
 * byte. func_ov019_02111254 rubber-bands his speed from how many checkpoints
 * the player is ahead. A long fall (mFallAccum past 0x7d0000) or the cheat
 * volume sets mPlayerCheated; the finish volume is data_ov019_021134e8.
 * NumStars() == 0x96 (every star) makes him the big penguin and changes the
 * invite message.
 *
 * daPgRcer_c_classInit is reconstructed (RTTI daPgRcer_c, allocation 0x398,
 * PENGUIN_RACER). Retail does not store that spelling. `return new` matches.
 *
 * Load-bearing, measured on this TU:
 *   #pragma opt_common_subs off stays file-global (last one wins).
 *   dCcAc_c::Init's ints do not match Init(dActor_c *, Fix12<int>, Fix12<int>,
 *   unsigned, unsigned): Fix12<int> has no converting constructor, so the
 *   method call does not compile. SetAnim, SetFile and DropShadowRadHeight
 *   take Fix12<int> by value the same way, so they stay scalar externs.
 *   dBgCh_Actr::Init is declared with Fix12i. The member call compiles and the
 *   bytes match, but it mangles _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_,
 *   and symbols.txt names the body 5Fix12IiE. SetRanges is not a method.
 *   dBgCh_Actr.h has no GetFloorResult / GetWallResult. The floor result is a
 *   dBgPi; its surface (dBgPc) sits at +4.
 *   func_02038414 is the 0xc veneer to UpdateDiscreteNoLava_2, not that body.
 *   func_0201267c is Sound::Play(3, id, pos). func_02012790 is Sound::Play2D(2, id).
 *   This TU calls the wrappers.
 *   dActor_c has no Pos(). GetNode and Render take (Vector3 *)&mPosX / &mScaleX.
 *   mTargetAngY is a halfword past ldrh's immediate. The path helpers address
 *   it as ((int)self + 0x300) + 0x8c.
 *   A member increment of mActionStep folds to ldrb [self, #0x38f]. The talk,
 *   countdown, walk and result sites add a pooled 0x38f. The race site's ++
 *   already does. The player's race position is (int)player + 0x5c, then three
 *   word loads, not mPosX.
 *   The helpers stay func_ov019_*. They are the state functions and the
 *   symbols.txt names. The state table is PMF records the sinit builds.
 *   data_ov019_* are that sinit's file handles, volumes and state table.
 *   g_profile_PENGUIN_RACER is not this TU's data.
 */

#pragma opt_common_subs off

#include "daPgRcer_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "dBgPi.h"

/* Word 1 of a SharedFilePtr is the loaded file. SharedFilePtr itself has no
 * fields; the sinit constructs these and this TU's LoadFile / SetAnim / SetFile
 * read that word. */
#define LOADED(handle) (((void **)&(handle))[1])

/* Halfword at 0x38c. ldrh's immediate only reaches 255, and a plain
 * mTargetAngY in these helpers picks a different base than the ROM. */
#define TARGET_ANG_Y(self) (*(short *)((char *)((int)(self) + 0x300) + 0x8c))

/* mActionStep is 0x38f. A member ++ folds to ldrb [self, #0x38f]; these
 * sites add a pooled 0x38f to the actor and increment through that pointer. */
#define BUMP_ACTION_STEP(self)                                                         \
    do {                                                                               \
        char *_raw = (char *)(self);                                                  \
        *(unsigned char *)((int)_raw + 0x38f) = *(unsigned char *)((int)_raw + 0x38f) + 1; \
    } while (0)

extern "C" {

extern SharedFilePtr data_ov019_02113498;          /* model, file 0x3fb */
extern SharedFilePtr *data_ov019_02112788[7];      /* the seven BCA handles */
extern SharedFilePtr *data_ov019_0211277c[3];      /* the three BTP handles */
extern daPgRcerState data_ov019_0211356c[];
extern Vector3 data_ov019_021134e8;                /* finish volume */
extern Vector3 data_ov019_0211353c;                /* cheat volume */
extern SharedFilePtr data_ov019_02113460;          /* BCA land, file 0x401 */
extern SharedFilePtr data_ov019_02113468;          /* BTP neutral, file 0x407 */
extern SharedFilePtr data_ov019_02113470;          /* BCA idle, file 0x406 */
extern SharedFilePtr data_ov019_02113478;          /* BCA jump, file 0x402 */
extern SharedFilePtr data_ov019_02113480;          /* BCA race start, file 0x400 */
extern SharedFilePtr data_ov019_02113488;          /* BCA race, file 0x403 */
extern SharedFilePtr data_ov019_02113490;          /* BCA near finish, file 0x405 */
extern SharedFilePtr data_ov019_021134a0;          /* BCA walk, file 0x408 */
extern SharedFilePtr data_ov019_021134a8;          /* BTP lost, file 0x3fd */
extern SharedFilePtr data_ov019_021134b0;          /* BTP race, file 0x404 */
extern unsigned char data_0209d684;                /* talk-script choice */

extern unsigned char NumStars(void);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_0201267c(int id, void *pos);
extern void func_02012790(int id);
extern void func_02038414(void *clsn);
extern int _ZN4cstd4fdivEii(int a, int b);
extern Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern void _Z11UpdateAngleRssis(short *ang, short target, int step, short max);
extern int _Z14ApproachLinearRsss(short *cur, short target, short step);
extern void _Z14ApproachLinearRiii(int *cur, int target, int step);
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, short angle);

extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *clsn, void *actor, int radius, int height, unsigned int flags, unsigned int vuln);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *clsn, void *actor, int radius, int height, void *a, int b);
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int offsetY, int radius, int clip, int far);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *shadow, void *mtx, int rad, int height, unsigned int opacity);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *model, void *bca, int flags, int speed, unsigned int start);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *seq, void *btp, int flags, int speed, unsigned int start);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *clsn);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *clsn);

extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 id, void *pos, u32 b);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 handle, u32 effect, int x, int y, int z, void *dir, void *cb);

int func_ov019_02111254(void *unused, int playerLead);
int func_ov019_0211140c(daPgRcer_c *self, dBgCh_Actr *clsn);
void func_ov019_021114ec(daPgRcer_c *self);

}

extern int _ZTV10daPgRcer_c[];

// @symbol daPgRcer_c_classInit
extern "C" daPgRcer_c *daPgRcer_c_classInit()
{
    return new daPgRcer_c();
}

// @symbol _ZN10daPgRcer_c13InitResourcesEv
int daPgRcer_c::InitResources()
{
    int i;

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov019_02113498), 1, 1);

    for (i = 0; i < 7; i++)
        Animation::LoadFile(*data_ov019_02112788[i]);

    for (int j = 0; j < 3; j++) {
        SharedFilePtr *t = data_ov019_0211277c[j];
        TextureSequence::LoadFile(*t);
        TextureSequence::Prepare(
            *(BMD_File *)LOADED(data_ov019_02113498),
            *(BTP_File *)LOADED(*t));
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (NumStars() == 0x96) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0xd0000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1999;
        mScaleY = 0x1000;
        mScaleZ = 0x1999;
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0xd0000, 0xd0000, 0, 0);
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0xf0000, 0xf0000, 0x1c20000, 0x1c20000);
    } else {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x82000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x82000, 0x82000, 0, 0);
    }

    mStarSlot = (u8)TrackStar((u8)((param1 >> 8) & 0xf), 2);
    func_ov019_021122dc(0);
    mPath.FromID(param1 & 0xff);
    mPathNodeIndex = 0;
    mPath.GetNode(*(Vector3 *)&mPosX, mPathNodeIndex);
    func_ov019_021113b0();
    func_ov019_021114ec(this);
    return 1;
}

// @symbol _ZN10daPgRcer_c8BehaviorEv
int daPgRcer_c::Behavior()
{
    func_ov019_02112268();
    mModelAnim.Animation::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    func_ov019_021114ec(this);
    return 1;
}

// @symbol _ZN10daPgRcer_c6RenderEv
int daPgRcer_c::Render()
{
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN10daPgRcer_c16OnPendingDestroyEv
void daPgRcer_c::OnPendingDestroy()
{
}

// @symbol _ZN10daPgRcer_c16CleanupResourcesEv
int daPgRcer_c::CleanupResources()
{
    data_ov019_02113498.Release();
    for (int i = 0; i < 7; i++)
        data_ov019_02112788[i]->Release();
    for (int i = 0; i < 3; i++)
        data_ov019_0211277c[i]->Release();
    return 1;
}

// @symbol _ZN10daPgRcer_c19func_ov019_021122dcEi
void daPgRcer_c::func_ov019_021122dc(int state)
{
    mStateDesc = &data_ov019_0211356c[state];
    func_ov019_021122a4();
}

// @symbol _ZN10daPgRcer_c19func_ov019_021122a4Ev
void daPgRcer_c::func_ov019_021122a4()
{
    daPgRcerStateMethod *p = &mStateDesc->init;
    (this->**p)();
}

// @symbol _ZN10daPgRcer_c19func_ov019_02112268Ev
void daPgRcer_c::func_ov019_02112268()
{
    daPgRcerStateMethod *p = &mStateDesc->behavior;
    (this->**p)();
}

extern "C" {

/* State 0: idle. */
// @symbol func_ov019_021121f8
int func_ov019_021121f8(daPgRcer_c *self)
{
    self->mVertAccel = 0;
    self->mTerminalVelocity = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113470), 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&self->mTextureSequence, LOADED(data_ov019_02113468), 0, 0x1000, 0);
    self->mAction = 0;
    return 1;
}

}

// @symbol _ZN10daPgRcer_c19func_ov019_02112168Ev
int daPgRcer_c::func_ov019_02112168()
{
    if (DecIfAbove0_Byte(&mTalkWaitTimer))
        return 1;
    Player *pl = ClosestPlayer();
    if (pl == 0)
        return 1;
    int radius = mdCcAc_c.radius;
    if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&pl->mPosX) < radius + 0x78000) {
        if (pl->StartTalk(*(fBase_c *)this, 1)) {
            mTalkPlayer = pl;
            func_ov019_021122dc(1);
        }
    }
    return 1;
}

extern "C" {

/* State 1: invite. */
// @symbol func_ov019_0211213c
int func_ov019_0211213c(daPgRcer_c *self)
{
    self->mActionStep = 0;
    func_0201267c(0xdf, &self->mCamSpacePosX);
    self->mAction = 1;
    return 1;
}

// @symbol func_ov019_02111fec
int func_ov019_02111fec(daPgRcer_c *self)
{
    switch (self->mActionStep) {
    case 0: {
        short ang = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mTalkPlayer->mPosX);
        if (_Z14ApproachLinearRsss(&self->mAngleY, ang, 0x514) != 0) {
            Vector3 pos;
            int msg;
            int eq;
            int y;
            int z;
            pos.x = self->mPosX;
            y = self->mPosY;
            pos.y = y;
            z = self->mPosZ;
            pos.z = z;
            pos.y = y + 0x190000;
            eq = (int)(NumStars() == 0x96);
            if (eq != 0)
                msg = 0xab;
            else
                msg = 0xa7;
            if (self->mTalkPlayer->ShowMessage(*(fBase_c *)self, (unsigned int)(short)msg, &pos, 1, 2) != 0) {
                BUMP_ACTION_STEP(self);
            }
        }
        break;
    }
    case 1:
        if (self->mTalkPlayer->GetTalkState() == 2) {
            unsigned char choice = data_0209d684;
            if (choice == 1) {
                func_02012790(0x98);
                self->func_ov019_021122dc(2);
            } else if (choice == 2) {
                func_02012790(0x63);
                self->mTalkWaitTimer = 0x5a;
                self->mTalkPlayer->HasFinishedTalking();
                self->func_ov019_021122dc(0);
            }
        }
        break;
    }
    return 1;
}

/* State 2: jump into the countdown. */
// @symbol func_ov019_02111f54
int func_ov019_02111f54(daPgRcer_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113478), 0x40000000, 0x1000, 0);
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    self->mVertSpeed = 0x16000;
    self->mActionStep = 0;
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x14, 0x7f, 0x15666, 0);
    func_0201267c(0x138, &self->mCamSpacePosX);
    self->mAction = 2;
    return 1;
}

// @symbol func_ov019_02111dec
int func_ov019_02111dec(daPgRcer_c *self)
{
    switch (self->mActionStep) {
    case 0:
        if (self->mTalkPlayer->Unk_020c4f40(0x5a) != 0) {
            BUMP_ACTION_STEP(self);
        }
        break;
    case 1:
        _Z11UpdateAngleRssis(&self->mPrevAngleY, TARGET_ANG_Y(self), 2, 0x800);
        self->mAngleY = self->mPrevAngleY;
        self->UpdatePos(&self->mdCcAc_c);
        func_ov019_0211140c(self, &self->mWithMeshClsn);
        if (self->mWithMeshClsn.JustHitGround() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113460), 0x40000000, 0x1000, 0);
            BUMP_ACTION_STEP(self);
        }
        break;
    case 2:
        if (self->mModelAnim.Animation::Finished() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113470), 0, 0x1000, 0);
        }
        if (self->mTalkPlayer->GetTalkState() == -1) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x7f, 0, 0x15666, 0);
            func_0201267c(0x4d, &self->mCamSpacePosX);
            self->func_ov019_021122dc(3);
        }
        break;
    }
    return 1;
}

/* State 3: the race. */
// @symbol func_ov019_02111d58
int func_ov019_02111d58(daPgRcer_c *self)
{
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113480), 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&self->mTextureSequence, LOADED(data_ov019_021134b0), 0, 0x1000, 0);
    self->mHorzSpeed = 0x30000;
    self->mActionStep = 0;
    self->mPlayerFinished = 0;
    self->mAction = 3;
    return 1;
}

// @symbol func_ov019_0211197c
int func_ov019_0211197c(daPgRcer_c *self)
{
    switch (self->mActionStep) {
    case 0:
        if (self->func_ov019_0211131c()) {
            self->func_ov019_021113b0();
        }
        if (self->mModelAnim.Animation::Finished()) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113488), 0, 0x1000, 0);
            self->mModelAnim.speed = 0x1000;
            self->mActionStep++;
        }
        self->UpdatePos(&self->mdCcAc_c);
        func_ov019_0211140c(self, &self->mWithMeshClsn);
        if (self->mPlayerCheated == 0 && self->mPlayerFinished == 0) {
            {
                Vector3 v;
                v.x = self->mPosX;
                v.y = self->mPosY;
                v.z = self->mPosZ;
                if (self->func_ov019_0211127c(&v, self->mPenguinCheckpoints)) {
                    self->mPenguinCheckpoints++;
                }
            }
            {
                Player *o = self->mTalkPlayer;
                char *p = (char *)((int)o + 0x5c);
                Vector3 v;
                v.x = *(int *)p;
                v.y = *(int *)(p + 4);
                v.z = *(int *)(p + 8);
                if (self->func_ov019_0211127c(&v, self->mPlayerCheckpoints)) {
                    self->mPlayerCheckpoints++;
                }
            }
        }
        break;

    case 1:
        if (self->func_ov019_0211131c()) {
            if (self->mPathNodeIndex >= (int)self->mPath.NumNodes() - 2) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113490), 0x40000000, 0x1000, 0);
                self->mActionStep++;
            } else {
                self->func_ov019_021113b0();
            }
        }
        self->UpdatePos(&self->mdCcAc_c);
        func_ov019_0211140c(self, &self->mWithMeshClsn);
        if (self->mPlayerCheated == 0 && self->mPlayerFinished == 0 &&
            self->mPathNodeIndex < (int)self->mPath.NumNodes() - 2) {
            {
                Vector3 v;
                v.x = self->mPosX;
                v.y = self->mPosY;
                v.z = self->mPosZ;
                if (self->func_ov019_0211127c(&v, self->mPenguinCheckpoints)) {
                    self->mPenguinCheckpoints++;
                }
            }
            {
                Player *o = self->mTalkPlayer;
                char *p = (char *)((int)o + 0x5c);
                Vector3 v;
                v.x = *(int *)p;
                v.y = *(int *)(p + 4);
                v.z = *(int *)(p + 8);
                if (self->func_ov019_0211127c(&v, self->mPlayerCheckpoints)) {
                    self->mPlayerCheckpoints++;
                }
            }
        }
        if (self->mPlayerCheckpoints > 1) {
            int val = func_ov019_02111254(self, self->mPlayerCheckpoints - self->mPenguinCheckpoints);
            if (self->mPlayerCheated != 0 || self->mPlayerFinished != 0) {
                val = 0x65000;
            }
            _Z14ApproachLinearRiii(&self->mHorzSpeed, val, 0x800);
        }
        if (self->mPenguinFinished == 0 && Vec3_Dist(&data_ov019_021134e8, (Vector3 *)&self->mPosX) < 0x190000) {
            if (self->mPlayerFinished != 0) {
                self->mPenguinWon = 0;
            } else {
                self->mPenguinWon = 1;
            }
            self->mPenguinFinished = 1;
        }
        self->mSlideSound = _ZN5Sound8PlayLongEjjjRK7Vector3s(self->mSlideSound, 3, 0x185, &self->mCamSpacePosX, 0);
        self->mDustParticle = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->mDustParticle, 0x101, self->mPosX, self->mPosY + 0x32000, self->mPosZ, 0, 0);
        break;

    case 2:
        if (self->mModelAnim.Animation::Finished()) {
            self->func_ov019_021122dc(4);
        }
        break;
    }

    self->func_ov019_021112b8();
    if (Vec3_Dist(&data_ov019_0211353c, (Vector3 *)&self->mTalkPlayer->mPosX) < 0x280000) {
        self->mPlayerCheated = 1;
    }
    if (Vec3_Dist(&data_ov019_021134e8, (Vector3 *)&self->mTalkPlayer->mPosX) < 0x190000) {
        self->mPlayerFinished = 1;
    }
    return 1;
}

/* State 4: walk up to the player. */
// @symbol func_ov019_02111904
int func_ov019_02111904(daPgRcer_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_021134a0), 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&self->mTextureSequence, LOADED(data_ov019_02113468), 0, 0x1000, 0);
    self->mActionStep = 0;
    self->mHorzSpeed = 0x5000;
    self->mAction = 4;
    return 1;
}

// @symbol func_ov019_021117a8
int func_ov019_021117a8(daPgRcer_c *self)
{
    int node[3];
    switch (self->mActionStep) {
    case 0: {
        int n = self->mPath.NumNodes();
        self->mPath.GetNode(*(Vector3 *)node, n - 1);
        self->mPathDist = Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)node);
        TARGET_ANG_Y(self) = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)node);
        _Z14ApproachLinearRsss(&self->mAngleY, TARGET_ANG_Y(self), 0x200);
        self->mPrevAngleY = self->mAngleY;
        if (self->mPathDist < self->mHorzSpeed) {
            self->mHorzSpeed = self->mPathDist;
            BUMP_ACTION_STEP(self);
        }
        self->UpdatePos(&self->mdCcAc_c);
        func_ov019_0211140c(self, &self->mWithMeshClsn);
        {
            unsigned int frame = ((unsigned int)self->mModelAnim.currFrame << 4) >> 0x10;
            if (frame == 9 || frame == 0x15) {
                func_0201267c(0xde, &self->mCamSpacePosX);
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mTalkPlayer->mPosX);
        if (_Z14ApproachLinearRsss(&self->mAngleY, ang, 0x514) != 0) {
            self->func_ov019_021122dc(5);
        }
        break;
    }
    }
    {
        unsigned int frame = ((unsigned int)self->mModelAnim.currFrame << 4) >> 0x10;
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, &self->mCamSpacePosX);
        }
    }
    return 1;
}

/* State 5: result, and the star if the penguin did not win. */
// @symbol func_ov019_02111754
int func_ov019_02111754(daPgRcer_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED(data_ov019_02113470), 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mActionStep = 0;
    self->mAction = 5;
    return 1;
}

// @symbol func_ov019_02111558
int func_ov019_02111558(daPgRcer_c *self)
{
    switch (self->mActionStep) {
    case 0: {
        int radius = self->mdCcAc_c.radius;
        int d = Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&self->mTalkPlayer->mPosX);
        if (d < radius + 0x78000) {
            if (self->mTalkPlayer->StartTalk(*(fBase_c *)self, 1) != 0) {
                BUMP_ACTION_STEP(self);
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mTalkPlayer->mPosX);
        if (_Z14ApproachLinearRsss(&self->mAngleY, ang, 0x514) != 0) {
            unsigned int id;
            struct { int x, y, z; } v;
            int z = self->mPosZ;
            int y = self->mPosY + 0x190000;
            int x = self->mPosX;
            v.x = x;
            v.y = y;
            v.z = z;
            if (self->mPlayerCheated != 0) {
                id = 0xa9;
                _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&self->mTextureSequence, LOADED(data_ov019_021134a8), 0, 0x1000, 0);
                self->mPenguinWon = 1;
            } else {
                if (self->mPenguinWon != 0)
                    id = 0xaa;
                else
                    id = 0xa8;
            }
            if (self->mTalkPlayer->ShowMessage(*(fBase_c *)self, id, (Vector3 *)&v, 1, 2) != 0) {
                func_0201267c(0xdf, &self->mCamSpacePosX);
                BUMP_ACTION_STEP(self);
            }
        }
        break;
    }
    case 2:
        if (self->mTalkPlayer->GetTalkState() == 2) {
            if (self->mPenguinWon == 0) {
                self->UntrackAndSpawnStar(
                    *(s8 *)&self->mStarSlot,
                    (unsigned int)(unsigned char)((self->param1 >> 8) & 0xf),
                    *(Vector3 *)&self->mPosX, 4);
            }
            self->mTalkPlayer->HasFinishedTalking();
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&self->mTextureSequence, LOADED(data_ov019_02113468), 0, 0x1000, 0);
            BUMP_ACTION_STEP(self);
        }
        break;
    }
    return 1;
}

// @symbol func_ov019_021114ec
void func_ov019_021114ec(daPgRcer_c *self)
{
    Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    self->mModelAnim.mat4x3.t.x = self->mPosX >> 3;
    self->mModelAnim.mat4x3.t.y = self->mPosY >> 3;
    self->mModelAnim.mat4x3.t.z = self->mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel, &self->mModelAnim.mat4x3, 0x140000, 0x50000, 0xf);
}

// @symbol func_ov019_0211140c
int func_ov019_0211140c(daPgRcer_c *self, dBgCh_Actr *clsn)
{
    int floorNormal[3];
    int wallNormal[3];
    func_02038414(clsn);
    if (clsn->IsOnGround()) {
        ((dBgPi *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn))->surface.CopyNormalTo(*(Vector3 *)floorNormal);
        if (floorNormal[1] != 0) {
            long long a = (long long)floorNormal[0] * (long long)self->unk_0a4;
            long long b = (long long)floorNormal[2] * (long long)self->unk_0ac;
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self->mVertSpeed = -(_ZN4cstd4fdivEii(x + y, floorNormal[1]) + 0x8000);
            if (self->mVertSpeed > 0)
                self->mVertSpeed = 0;
        }
    }
    if (clsn->IsOnWall()) {
        ((dBgPi *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn))->surface.CopyNormalTo(*(Vector3 *)wallNormal);
    }
}

}

// @symbol _ZN10daPgRcer_c19func_ov019_021113b0Ev
void daPgRcer_c::func_ov019_021113b0()
{
    Vector3 node;
    mPathNodeIndex = mPathNodeIndex + 1;
    mPath.GetNode(node, mPathNodeIndex);
    mPathDist = Vec3_Dist((Vector3 *)&mPosX, &node);
    TARGET_ANG_Y(this) = Vec3_HorzAngle((Vector3 *)&mPosX, &node);
}

// @symbol _ZN10daPgRcer_c19func_ov019_0211131cEv
int daPgRcer_c::func_ov019_0211131c()
{
    _Z11UpdateAngleRssis(&mPrevAngleY, TARGET_ANG_Y(this), 2, 0x600);
    int n = mPathNodeIndex;
    if (n >= (int)mPath.NumNodes() - 2) {
        mAngleY = mPrevAngleY;
    } else {
        Vector3 node;
        mPath.GetNode(node, n + 1);
        mAngleY = Vec3_HorzAngle((Vector3 *)&mPosX, &node);
    }
    mPathDist = mPathDist - mHorzSpeed;
    return mPathDist < 0 ? 1 : 0;
}

// @symbol _ZN10daPgRcer_c19func_ov019_021112b8Ev
void daPgRcer_c::func_ov019_021112b8()
{
    if (mPlayerCheated != 0)
        return;
    Player *t = mTalkPlayer;
    if (t->mIsAirborne != 0) {
        int vertSpeed = t->mVertSpeed;
        int terminalVel = t->mTerminalVelocity;
        if (vertSpeed > terminalVel)
            return;
        {
            int d = vertSpeed;
            if (d < 0)
                d = -d;
            mFallAccum = mFallAccum + d;
            if (mFallAccum > 0x7d0000)
                mPlayerCheated = 1;
        }
        return;
    }
    mFallAccum = 0;
}

// @symbol _ZN10daPgRcer_c19func_ov019_0211127cEP7Vector3j
int daPgRcer_c::func_ov019_0211127c(Vector3 *pos, unsigned int nodeIndex)
{
    Vector3 node;
    mPath.GetNode(node, nodeIndex);
    Fix12i dist = Vec3_HorzDist(&node, pos);
    return dist <= 0x320000 ? 1 : 0;
}

extern "C" {

// @symbol func_ov019_02111254
/* The further ahead the player is, in path nodes, the faster the penguin
 * runs. Clamped to 0x30000..0x65000. */
int func_ov019_02111254(void *unused, int playerLead)
{
    int speed = playerLead * 0x54cc + 0x4a800;

    if (speed > 0x65000) {
        speed = 0x65000;
    }
    if (speed < 0x30000) {
        speed = 0x30000;
    }

    return speed;
}

}
