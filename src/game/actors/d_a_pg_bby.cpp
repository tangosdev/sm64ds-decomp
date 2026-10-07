//cpp
/* ov072/daPgBby_c -- PENGUIN_BABY (256), the lost baby penguin.
 *
 * ov072 is mixed (SNOWMAN_BODY / SNOWMAN_HEAD / BIG_SNOWMAN / PENGUIN_BABY);
 * this TU is the last .text run of the overlay, 0x02120c58..0x02121ffc.
 * RTTI names the class daPgBby_c (the tree's earlier BabyPenguin was coined).
 * daPgBby_c_classInit is a reconstructed spelling; retail stores only the
 * PENGUIN_BABY profile's pointer to it. Historical alias: BabyPenguin_Spawn.
 *
 * The penguin runs a six-state machine (see include/daPgBby_c.h). The state
 * steps and helpers are daPgBby_c methods; they keep their configured
 * func_ov072_* names, the cartridge having no symbols for them.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Known limits:
 * - SetAnim, DropShadowRadHeight, IsTooFarAwayFromPlayer, dCcAc_c::Init,
 *   dBgCh_Actr::Init and dClipper::Func_02015560 stay mangled: each takes
 *   Fix12<int> by value (notes/mwccarm-codegen.md 6az).
 * - The state table (data_ov072_02122d6c), its member-pointer literals, the
 *   carry offsets (data_ov072_02122d3c) and the file handles are built by
 *   the overlay static initializer __sinit_ov072_02122414, which this TU
 *   does not own; the handles are read as raw words because SharedFilePtr.h
 *   declares no fields.
 * - func_ov072_021210c4 reads the carrier Player's word at +0xc8, which
 *   dActor_c.h still carries as padding.
 */

#include "daPgBby_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

bool ApproachLinear(short &value, short target, short step);

/* A plain twelve-word copy of the carry matrix. */
struct PgBbyMtx {
    s32 m[12];
};

extern "C" {
/* Fix12<int> by-value walls (notes/mwccarm-codegen.md 6az). */
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int dist);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, void *actor, int r, int h, unsigned int a, unsigned int b);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int r, int h, void *v, int b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int flags, int speed, unsigned int start);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    void *self, void *shadow, void *mtx, int radius, int height, unsigned int flags);
int _ZN8dClipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void *m, void *v, Vector3 *a, int b, Vector3 *e);

int Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromRotationY(void *m, int angle);
void func_0201267c(unsigned int soundID, const Vector3 *pos);

/* The dBgCh_Actr::UpdateContinuous call goes through a veneer, and the two
   result getters are not declared by dBgCh_Actr.h. */
void dBgCh_Actr_UpdateContinuous_Veneer(void *c);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *c);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *c);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *surface, Vector3 &out);
int _ZN4cstd4fdivEii(int a, int b);

extern short data_02082214[];   /* sine/cosine table */
extern int data_0209f43c[];
extern Matrix4x3 data_0209b3ec;

/* Overlay file handles: the BMD, and the five BCA handles through the
   pointer table at 0x02122004 (word 1 of each handle is the loaded file). */
extern SharedFilePtr data_ov072_02122cb4;
extern SharedFilePtr *data_ov072_02122004[];
extern int data_ov072_02122c94[];
extern int data_ov072_02122c9c[];
extern int data_ov072_02122ca4[];
extern int data_ov072_02122cac[];
extern int data_ov072_02122cbc[];
extern Vector3 data_ov072_02122d3c[];   /* the four carry offsets */
extern daPgBby_c::State data_ov072_02122d6c[];/* the six states */
}

// @symbol daPgBby_c_classInit
extern "C" daPgBby_c *daPgBby_c_classInit()
{
    return new daPgBby_c();
}

// @symbol _ZN9daPgBby_c13OnTurnIntoEggER6Player
void daPgBby_c::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}

// @symbol _ZN9daPgBby_c13InitResourcesEv
s32 daPgBby_c::InitResources()
{
    BMD_File *f = (BMD_File *)Model::LoadFile(data_ov072_02122cb4);
    mModelAnim.SetFile(f, 1, -1);
    int i;
    for (i = 0; i < 5; i++) {
        dExtFrameCtrl_c::LoadFile(*data_ov072_02122004[i]);
    }
    if (mShadowModel.InitCylinder() == 0) return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mCylClsn, this, 0x28000, 0x50000, 0x800004, 0x9000);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x400;
    mScaleY = 0x400;
    mScaleZ = 0x400;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mEatingPlayer = 0;
    mCarrier = 0;
    func_ov072_02121d50(0);
    func_ov072_021210c4();
    return 1;
}

// @symbol _ZN9daPgBby_c8BehaviorEv
s32 daPgBby_c::Behavior()
{
    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x7d0000) && mWithMeshClsn.IsOnGround()) {
        func_ov072_02120d04();
    } else {
        if (mMother == 0)
            mMother = FindWithActorID(0x101, 0);
        mRespawnTimer = 0x384;
        func_ov072_02121cdc();
        func_ov072_021210c4();
    }
    return 1;
}

// @symbol _ZN9daPgBby_c6RenderEv
s32 daPgBby_c::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN9daPgBby_c16OnPendingDestroyEv
void daPgBby_c::OnPendingDestroy()
{
}

// @symbol _ZN9daPgBby_c16CleanupResourcesEv
s32 daPgBby_c::CleanupResources()
{
    data_ov072_02122cb4.Release();

    int i = 0;
    do {
        data_ov072_02122004[i]->Release();
        i++;
    } while (i < 5);

    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121d50Ei
/* Switch to state `state` and run its enter step. */
void daPgBby_c::func_ov072_02121d50(int state)
{
    mState = &data_ov072_02122d6c[state];
    func_ov072_02121d18();
}

// @symbol _ZN9daPgBby_c19func_ov072_02121d18Ev
/* Run the current state's enter step. */
void daPgBby_c::func_ov072_02121d18()
{
    StateFunc *step = &mState->enter;
    (this->**step)();
}

// @symbol _ZN9daPgBby_c19func_ov072_02121cdcEv
/* Run the current state's per-frame update step. */
void daPgBby_c::func_ov072_02121cdc()
{
    StateFunc *step = &mState->update;
    (this->**step)();
}

// @symbol _ZN9daPgBby_c19func_ov072_02121c94Ev
/* State 0, enter. */
int daPgBby_c::func_ov072_02121c94()
{
    mSubState = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122c9c[1], 0, 0x1000, 0);
    mStateId = 0;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121a84Ev
/* State 0, update: keep a comfortable distance from the closest player,
   walking away when it comes near and back when it wanders off. */
int daPgBby_c::func_ov072_02121a84()
{
    Player *player;
    int dist;
    short angle;
    int frame;

    player = ClosestPlayer();
    dist = Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&player->mPosX);
    switch (mSubState) {
    case 0:
        angle = (short)(Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&player->mPosX) + 0x8000);
        ApproachLinear(mAngleY, angle, 0x514);
        mPrevAngleY = mAngleY;
        mHorzSpeed = 0x5000;
        if (dist >= 0x1c2000) {
            mSubState = 2;
        }
        break;
    case 1:
        angle = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&player->mPosX);
        ApproachLinear(mAngleY, angle, 0x514);
        mPrevAngleY = mAngleY;
        mHorzSpeed = 0x5000;
        if (dist > 0x384000) {
            mSubState = 2;
        } else if (dist < 0x1c2000) {
            mSubState = 2;
        }
        break;
    case 2:
        mHorzSpeed = 0;
        if (dist < 0x15e000) {
            mSubState = 0;
        } else if (dist >= 0x226000) {
            if (dist < 0x384000) {
                mSubState = 1;
            }
        }
        break;
    }
    if (mHorzSpeed != 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122cbc[1], 0, 0x1000, 0);
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122c9c[1], 0, 0x1000, 0);
    }
    mModelAnim.speed = 0x1000;
    mModelAnim.Advance();
    func_ov072_02120e20();
    func_ov072_02120ddc();
    UpdatePos(&mCylClsn);
    func_ov072_02120fd4(&mWithMeshClsn);
    func_ov072_02120f14();
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    if ((int)mModelAnim.file == data_ov072_02122cbc[1]) {
        frame = (int)(((u32)mModelAnim.currFrame << 4) >> 16);
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, (Vector3 *)&mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121a28Ev
/* State 1, enter: a hop. */
int daPgBby_c::func_ov072_02121a28()
{
    mHorzSpeed = 0x19000;
    mVertSpeed = 0xc000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122c94[1], 0x40000000, 0x1000, 0);
    mSubState = 0;
    mStateId = 1;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021218dcEv
/* State 1, update. */
int daPgBby_c::func_ov072_021218dc()
{
    switch (mSubState) {
    case 0:
        if (mModelAnim.Finished()) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122cac[1], 0, 0x1000, 0);
            mModelAnim.speed = 0x1000;
            mSubState++;
        }
        break;
    case 1:
        mHorzSpeed -= 0x3000;
        if (mHorzSpeed < 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122ca4[1], 0x40000000, 0x1000, 0);
            mHorzSpeed = 0;
            mSubState++;
        }
        break;
    case 2:
        if (mModelAnim.Finished()) {
            func_ov072_02121d50(0);
        }
        break;
    }
    mModelAnim.Advance();
    UpdatePos(&mCylClsn);
    func_ov072_02120fd4(&mWithMeshClsn);
    func_ov072_02120f14();
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121890Ev
/* State 2, enter: picked up. */
int daPgBby_c::func_ov072_02121890()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122c9c[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mStateId = 2;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021217acEv
/* State 2, update: face where the carrier faces; once no longer held, drop
   to the ground under the carrier and return to state 0. */
int daPgBby_c::func_ov072_021217ac()
{
    mAngleY = mCarrier->mAngleY;
    mPrevAngleY = mAngleY;
    u32 flags = mFlags;
    int held = (flags & 0x100) != 0;
    if (held) {
        int thrown = (flags & 0x2000) != 0;
        if (!thrown)
            goto animate;
    }
    {
        Player *carrier = mCarrier;
        Vector3 from;
        int y = carrier->mPosY;
        int z = carrier->mPosZ;
        int x = carrier->mPosX;
        int aboveY = y + 0x32000;
        from.x = x;
        from.y = aboveY;
        from.z = z;
        DetectRaycastClsn(from, *(Vector3 *)&mPosX, true);
        mCarrier = 0;
        func_ov072_02121d50(0);
    }
animate:
    mModelAnim.Advance();
    mCylClsn.Clear();
    if (param1 == 0) {
        u32 frame = (u32)(mModelAnim.currFrame << 4) >> 16;
        if (frame == 0x10 || frame == 0x25) {
            func_0201267c(0xf2, (Vector3 *)&mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121758Ev
/* State 3, enter. */
int daPgBby_c::func_ov072_02121758()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov072_02122cbc[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mHorzSpeed = 0x5000;
    mStateId = 3;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121670Ev
/* State 3, update: walk to the mother, keeping between 0xc8 and 0xdc
   units from her. */
int daPgBby_c::func_ov072_02121670()
{
    int dist = Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&mMother->mPosX);
    short tgt = mAngleY;
    if (dist > 0xdc000) {
        tgt = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&mMother->mPosX);
    } else if (dist < 0xc8000) {
        tgt = (short)(Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&mMother->mPosX) + 0x8000);
    }
    ApproachLinear(mAngleY, tgt, 0x2bc);
    mPrevAngleY = mAngleY;
    mModelAnim.Advance();
    UpdatePos(&mCylClsn);
    func_ov072_02120fd4(&mWithMeshClsn);
    func_ov072_02120f14();
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    {
        int frame = (int)((unsigned)(mModelAnim.currFrame << 4) >> 16);
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, (Vector3 *)&mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121640Ev
/* State 4, enter. */
int daPgBby_c::func_ov072_02121640()
{
    mSubState = 0;
    mHorzSpeed = 0;
    mCylClsn.Clear();
    mStateId = 4;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021214dcEv
/* State 4, update: the carrier's message 0xb2, then wait for the talk to
   close and be put down. */
int daPgBby_c::func_ov072_021214dc()
{
    switch (mSubState) {
    case 0:
    {
        u32 flags = mFlags;
        int inMouth = (flags & 0x40000) != 0;
        if (inMouth) {
            mSubState++;
        } else {
            int talking = (flags & 0x20000) != 0;
            if (talking) break;
            if (inMouth) break;
            mEatingPlayer = 0;
            func_ov072_02121d50(0);
        }
        break;
    }
    case 1:
    {
        if (mCarrier->ShowMessage(*this, 0xb2, 0, 0, 0)) {
            func_0201267c(0xf2, (Vector3 *)&mCamSpacePosX);
            mSubState++;
        }
        int spat = (mFlags & 0x80000) != 0;
        if (spat) {
            func_ov072_02121d50(5);
        }
        break;
    }
    case 2:
        if (mCarrier->GetTalkState() == -1) {
            mCarrier->DropActor();
            mSubState++;
        }
        break;
    case 3:
    {
        int spat = (mFlags & 0x80000) != 0;
        if (spat) {
            func_ov072_02121d50(5);
        }
        break;
    }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121368Ev
/* State 5, enter: spat out by Yoshi -- appear 0x50 units in front of the
   eating player and raycast down from above it. The statement order below
   is the retail schedule's. */
int daPgBby_c::func_ov072_02121368()
{
    Vector3 from;
    Vector3 *fromp;
    Player *eater;
    s32 *posX;
    s32 *posY;
    s32 *posZ;
    s16 angle;
    u16 index;
    int dist;
    int round;
    int store;
    int x;
    int y;
    int z;
    int aboveY;

    mFlags &= ~0x80000;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0;

    eater = mEatingPlayer;
    posX = &mPosX;
    angle = eater->mAngleY;
    dist = 0x50000;
    round = 0x800;
    mAngleY = angle;
    angle = mAngleY;
    posY = &mPosY;
    posZ = &mPosZ;
    mPrevAngleY = angle;

    eater = mEatingPlayer;
    {
        s32 *src = &eater->mPosX;
        s32 srcX = src[0];
        fromp = &from;
        mPosX = srcX;
        s32 srcY = src[1];
        store = 1;
        mPosY = srcY;
        s32 srcZ = src[2];
        mPosZ = srcZ;
    }

    index = mAngleY;
    angle = data_02082214[(index >> 4) * 2];
    *posX = *posX + (int)(((s64)angle * dist + round) >> 12);
    *posY = *posY + dist;

    index = mAngleY;
    angle = data_02082214[(index >> 4) * 2 + 1];
    *posZ = *posZ + (int)(((s64)angle * dist + round) >> 12);

    eater = mEatingPlayer;
    y = eater->mPosY;
    z = eater->mPosZ;
    aboveY = y + dist;
    x = eater->mPosX;
    from.x = x;
    from.y = aboveY;
    from.z = z;
    DetectRaycastClsn(*fromp, *(Vector3 *)posX, store);

    mEatingPlayer = 0;
    mWithMeshClsn.SetLimMovFlag();
    mStateId = 5;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021212c0Ev
/* State 5, update: fall, bounce once at 40% and settle back into state 0. */
int daPgBby_c::func_ov072_021212c0()
{
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround()) {
        mVertSpeed = (mVertSpeed * -0x28) / 100;
    } else if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        func_ov072_02121d50(0);
    }
    UpdatePos(&mCylClsn);
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021210c4Ev
/* Place the model: in the carrier's hands (one of four carry offsets) or at
   the actor, then size the drop shadow by the height above the ground. */
void daPgBby_c::func_ov072_021210c4()
{
    int idx;
    Matrix4x3 *carry;
    Vector3 v;
    int shadowHeight;
    int shadowRadius;
    long long tmp;

    if (mCarrier && ((mFlags & 0x4000) ? 1 : 0) && *(int *)((char *)mCarrier + 0xc8)) {
        idx = 0;
        if (mCarrier->IsFrontSliding() || mCarrier->LostGrabbedObject()) {
            idx = 1;
        }
        if (mCarrier->param1 == 2) {
            idx = (idx + 2) & 0xff;
        }
        carry = UpdateCarry(*mCarrier, data_ov072_02122d3c[idx]);
        *(PgBbyMtx *)&mModelAnim.mat4x3 = *(PgBbyMtx *)carry;
    } else {
        Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
        mModelAnim.mat4x3.t.x = mPosX >> 3;
        mModelAnim.mat4x3.t.y = mPosY >> 3;
        mModelAnim.mat4x3.t.z = mPosZ >> 3;
    }

    if ((mFlags & 0x40000) ? 1 : 0) {
        return;
    }

    shadowHeight = 0xb4000;
    shadowRadius = 0x5a000;
    if (mCarrier && ((mFlags & 0x4000) ? 1 : 0)) {
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y -= 0xa000;
        dBgCh_Gnd ground;
        ground.SetObjAndPos(v, 0);
        if (ground.DetectClsn()) {
            shadowHeight = mPosY - ground.clsnY;
            if (shadowHeight < 0x1000) shadowHeight = 0x1000;
            tmp = (long long)shadowHeight * 0x180 + 0x800;
            shadowRadius = shadowRadius - (int)(tmp >> 12);
            if (shadowRadius < 0x1000) shadowRadius = 0x1000;
            if (shadowRadius > 0x5a000) shadowRadius = 0x5a000;
            shadowHeight = shadowHeight + 0x28000;
        }
    }

    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mModelAnim.mat4x3, shadowRadius, shadowHeight, 0xf);
}

// @symbol _ZN9daPgBby_c19func_ov072_02120fd4EP10dBgCh_Actr
/* Wall/floor pass: on a slope, turn horizontal motion into vertical speed. */
void daPgBby_c::func_ov072_02120fd4(dBgCh_Actr *clsn)
{
    Vector3 floorNormal;
    Vector3 wallNormal;
    int b = (int)((mFlags & 0x4000) != 0);
    if (b != 0) return;
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4, floorNormal);
        if (floorNormal.y != 0) {
            long long a = (long long)floorNormal.x * (long long)unk_0a4;
            long long bb = (long long)floorNormal.z * (long long)unk_0ac;
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((bb + 0x800) >> 12);
            mVertSpeed = -(_ZN4cstd4fdivEii(x + y, floorNormal.y) + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn) + 4, wallNormal);
    }
}

// @symbol _ZN9daPgBby_c19func_ov072_02120f14Ev
/* Refuse a step onto ground more than 0x1.9 away from the current height:
   put the actor back where it was last frame. */
int daPgBby_c::func_ov072_02120f14()
{
    Vector3 v;

    if (mWithMeshClsn.IsOnGround()) {
        dBgCh_Gnd ground;
        {
            int z = mPosZ;
            int y = mPosY + 0x1e000;
            int x = mPosX;
            v.x = x;
            v.y = y;
            v.z = z;
        }
        ground.SetObjAndPos(v, this);
        if (ground.DetectClsn() != 0) {
            int d = ground.clsnY - mPosY;
            if (d < 0) d = -d;
            if (d <= 0x1900) goto bail;
        }
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
        return 1;
    bail:
        ;
    }
    return 0;
}

// @symbol _ZN9daPgBby_c19func_ov072_02120e50Ev
/* Touched by the player (actor 0xbf): enter state 4 while talking, or
   state 2 when the player grabs it. */
int daPgBby_c::func_ov072_02120e50()
{
    dActor_c *a;
    int isType;
    int flag;
    if (mCylClsn.otherOwner == 0) return 0;
    a = dActor_c::FindWithID(mCylClsn.otherOwner);
    if (a == 0 || (isType = (a->actorID == 0xbf)) == 0) return 0;
    flag = ((mFlags & 0x20000) != 0);
    if (flag) {
        mCarrier = (Player *)a;
        func_ov072_02121d50(4);
    } else if ((mCylClsn.hitFlags & 0x1000) && ((Player *)a)->TryGrab(*this)) {
        mCarrier = (Player *)a;
        func_ov072_02121d50(2);
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02120e20Ev
/* A diving player nearby startles it into state 1. */
void daPgBby_c::func_ov072_02120e20()
{
    Player *player = ClosestPlayer();
    if (player->IsDiving()) func_ov072_02121d50(1);
}

// @symbol _ZN9daPgBby_c19func_ov072_02120ddcEv
/* Within 0x190 of the mother: state 3. */
void daPgBby_c::func_ov072_02120ddc()
{
    dActor_c *mother = mMother;
    if (mother == 0)
        return;
    if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&mother->mPosX) >= 0x190000)
        return;
    func_ov072_02121d50(3);
}

// @symbol _ZN9daPgBby_c19func_ov072_02120d04Ev
/* Respawn: fallen far below the level, or far from home with the timer
   run out, and off screen -- go back to the spawn position. */
void daPgBby_c::func_ov072_02120d04()
{
    Vector3 out;
    Vector3 tmp;
    if (mStateId == 3) return;
    if ((unsigned)mPosY > (unsigned)(-0x1800000)) {
        if (Vec3_Dist((Vector3 *)&mSpawnPosX, (Vector3 *)&mPosX) < 0x190000) return;
        if (DecIfAbove0_Short(&mRespawnTimer) != 0) return;
    }
    Vec3_Asr(&tmp, (Vector3 *)&mSpawnPosX, 3);
    if (_ZN8dClipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_((void *)data_0209f43c, &data_0209b3ec, &tmp, 0x1f400, &out) <= 0xc350000) return;
    mPosX = mSpawnPosX;
    mPosY = mSpawnPosY;
    mPosZ = mSpawnPosZ;
}

// @symbol _ZN9daPgBby_c13OnYoshiTryEatEv
s32 daPgBby_c::OnYoshiTryEat()
{
    return 7;
}

/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
// @symbol _ZN9daPgBby_cD1Ev
// @symbol _ZN9daPgBby_cD0Ev
