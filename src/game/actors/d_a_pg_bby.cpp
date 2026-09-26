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
 * steps and helpers keep their configured func_ov072_* names as extern "C"
 * functions.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Known limits:
 * - SetAnim, DropShadowRadHeight, IsTooFarAwayFromPlayer, dCcAc_c::Init,
 *   dBgCh_Actr::Init and Clipper::Func_02015560 stay mangled: each takes
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

/* The state table's member pointers are called through a plain,
   non-polymorphic view of the actor: an {address, adjustment} pair with no
   virtual bit, matching the eight-byte literals in overlay .data. */
struct PgBbyStateHost;
typedef void (PgBbyStateHost::*PgBbyStateFunc)();
struct PgBbyState {
    PgBbyStateFunc enter;
    PgBbyStateFunc update;
};
struct PgBbyStateHost {
    u8 pad[0x35c];
    PgBbyState *mState;             /* daPgBby_c::mState */
};

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
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *shadow, void *mtx, int radius, int height, unsigned int flags);
int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void *m, void *v, Vector3 *a, int b, Vector3 *e);

int Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromRotationY(void *m, int angle);
void func_0201267c(unsigned int soundID, const Vector3 *pos);
void _Z14ApproachLinearRsss(short *cur, short tgt, short step);

/* The dBgCh_Actr::UpdateContinuous call goes through a veneer, and the two
   result getters are not declared by dBgCh_Actr.h. */
int dBgCh_Actr_UpdateContinuous_Veneer(void *c);
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
extern PgBbyState data_ov072_02122d6c[];/* the six states */

void func_ov072_02120d04(daPgBby_c *self);
void func_ov072_02120ddc(daPgBby_c *self);
void func_ov072_02120e20(daPgBby_c *self);
int func_ov072_02120e50(daPgBby_c *self);
int func_ov072_02120f14(daPgBby_c *self);
void func_ov072_02120fd4(daPgBby_c *self, dBgCh_Actr *clsn);
void func_ov072_021210c4(daPgBby_c *self);
void func_ov072_02121cdc(PgBbyStateHost *self);
void func_ov072_02121d18(PgBbyStateHost *self);
void func_ov072_02121d50(daPgBby_c *self, int state);
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
        Animation::LoadFile(*data_ov072_02122004[i]);
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
    func_ov072_02121d50(this, 0);
    func_ov072_021210c4(this);
    return 1;
}

// @symbol _ZN9daPgBby_c8BehaviorEv
s32 daPgBby_c::Behavior()
{
    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x7d0000) && mWithMeshClsn.IsOnGround()) {
        func_ov072_02120d04(this);
    } else {
        if (mMother == 0)
            mMother = FindWithActorID(0x101, 0);
        mRespawnTimer = 0x384;
        func_ov072_02121cdc((PgBbyStateHost *)this);
        func_ov072_021210c4(this);
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

// @symbol func_ov072_02121d50
/* Switch to state `state` and run its enter step. */
extern "C" void func_ov072_02121d50(daPgBby_c *self, int state)
{
    self->mState = &data_ov072_02122d6c[state];
    func_ov072_02121d18((PgBbyStateHost *)self);
}

// @symbol func_ov072_02121d18
/* Run the current state's enter step. */
extern "C" void func_ov072_02121d18(PgBbyStateHost *self)
{
    PgBbyStateFunc *step = &self->mState->enter;
    (self->**step)();
}

// @symbol func_ov072_02121cdc
/* Run the current state's per-frame update step. */
extern "C" void func_ov072_02121cdc(PgBbyStateHost *self)
{
    PgBbyStateFunc *step = &self->mState->update;
    (self->**step)();
}

// @symbol func_ov072_02121c94
/* State 0, enter. */
extern "C" int func_ov072_02121c94(daPgBby_c *self)
{
    self->mSubState = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122c9c[1], 0, 0x1000, 0);
    self->mStateId = 0;
    return 1;
}

// @symbol func_ov072_02121a84
/* State 0, update: keep a comfortable distance from the closest player,
   walking away when it comes near and back when it wanders off. */
extern "C" int func_ov072_02121a84(daPgBby_c *self)
{
    Player *player;
    int dist;
    short angle;
    int frame;

    player = self->ClosestPlayer();
    dist = Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&player->mPosX);
    switch (self->mSubState) {
    case 0:
        angle = (short)(Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&player->mPosX) + 0x8000);
        _Z14ApproachLinearRsss(&self->mAngleY, angle, 0x514);
        self->mPrevAngleY = self->mAngleY;
        self->mHorzSpeed = 0x5000;
        if (dist >= 0x1c2000) {
            self->mSubState = 2;
        }
        break;
    case 1:
        angle = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&player->mPosX);
        _Z14ApproachLinearRsss(&self->mAngleY, angle, 0x514);
        self->mPrevAngleY = self->mAngleY;
        self->mHorzSpeed = 0x5000;
        if (dist > 0x384000) {
            self->mSubState = 2;
        } else if (dist < 0x1c2000) {
            self->mSubState = 2;
        }
        break;
    case 2:
        self->mHorzSpeed = 0;
        if (dist < 0x15e000) {
            self->mSubState = 0;
        } else if (dist >= 0x226000) {
            if (dist < 0x384000) {
                self->mSubState = 1;
            }
        }
        break;
    }
    if (self->mHorzSpeed != 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122cbc[1], 0, 0x1000, 0);
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122c9c[1], 0, 0x1000, 0);
    }
    self->mModelAnim.speed = 0x1000;
    self->mModelAnim.Advance();
    func_ov072_02120e20(self);
    func_ov072_02120ddc(self);
    self->UpdatePos(&self->mCylClsn);
    func_ov072_02120fd4(self, &self->mWithMeshClsn);
    func_ov072_02120f14(self);
    func_ov072_02120e50(self);
    self->mCylClsn.Clear();
    self->mCylClsn.Update();
    if ((int)self->mModelAnim.file == data_ov072_02122cbc[1]) {
        frame = (int)(((u32)self->mModelAnim.currFrame << 4) >> 16);
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, (Vector3 *)&self->mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol func_ov072_02121a28
/* State 1, enter: a hop. */
extern "C" int func_ov072_02121a28(daPgBby_c *self)
{
    self->mHorzSpeed = 0x19000;
    self->mVertSpeed = 0xc000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122c94[1], 0x40000000, 0x1000, 0);
    self->mSubState = 0;
    self->mStateId = 1;
    return 1;
}

// @symbol func_ov072_021218dc
/* State 1, update. */
extern "C" int func_ov072_021218dc(daPgBby_c *self)
{
    switch (self->mSubState) {
    case 0:
        if (self->mModelAnim.Finished()) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122cac[1], 0, 0x1000, 0);
            self->mModelAnim.speed = 0x1000;
            self->mSubState++;
        }
        break;
    case 1:
        self->mHorzSpeed -= 0x3000;
        if (self->mHorzSpeed < 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122ca4[1], 0x40000000, 0x1000, 0);
            self->mHorzSpeed = 0;
            self->mSubState++;
        }
        break;
    case 2:
        if (self->mModelAnim.Finished()) {
            func_ov072_02121d50(self, 0);
        }
        break;
    }
    self->mModelAnim.Advance();
    self->UpdatePos(&self->mCylClsn);
    func_ov072_02120fd4(self, &self->mWithMeshClsn);
    func_ov072_02120f14(self);
    func_ov072_02120e50(self);
    self->mCylClsn.Clear();
    self->mCylClsn.Update();
    return 1;
}

// @symbol func_ov072_02121890
/* State 2, enter: picked up. */
extern "C" int func_ov072_02121890(daPgBby_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122c9c[1], 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mStateId = 2;
    return 1;
}

// @symbol func_ov072_021217ac
/* State 2, update: face where the carrier faces; once no longer held, drop
   to the ground under the carrier and return to state 0. */
extern "C" int func_ov072_021217ac(daPgBby_c *self)
{
    self->mAngleY = self->mCarrier->mAngleY;
    self->mPrevAngleY = self->mAngleY;
    u32 flags = self->mFlags;
    int held = (flags & 0x100) != 0;
    if (held) {
        int thrown = (flags & 0x2000) != 0;
        if (!thrown)
            goto animate;
    }
    {
        Player *carrier = self->mCarrier;
        Vector3 from;
        int y = carrier->mPosY;
        int z = carrier->mPosZ;
        int x = carrier->mPosX;
        int aboveY = y + 0x32000;
        from.x = x;
        from.y = aboveY;
        from.z = z;
        self->DetectRaycastClsn(from, *(Vector3 *)&self->mPosX, true);
        self->mCarrier = 0;
        func_ov072_02121d50(self, 0);
    }
animate:
    self->mModelAnim.Advance();
    self->mCylClsn.Clear();
    if (self->param1 == 0) {
        u32 frame = (u32)(self->mModelAnim.currFrame << 4) >> 16;
        if (frame == 0x10 || frame == 0x25) {
            func_0201267c(0xf2, (Vector3 *)&self->mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol func_ov072_02121758
/* State 3, enter. */
extern "C" int func_ov072_02121758(daPgBby_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov072_02122cbc[1], 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mHorzSpeed = 0x5000;
    self->mStateId = 3;
    return 1;
}

// @symbol func_ov072_02121670
/* State 3, update: walk to the mother, keeping between 0xc8 and 0xdc
   units from her. */
extern "C" int func_ov072_02121670(daPgBby_c *self)
{
    int dist = Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&self->mMother->mPosX);
    short tgt = self->mAngleY;
    if (dist > 0xdc000) {
        tgt = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mMother->mPosX);
    } else if (dist < 0xc8000) {
        tgt = (short)(Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mMother->mPosX) + 0x8000);
    }
    _Z14ApproachLinearRsss(&self->mAngleY, tgt, 0x2bc);
    self->mPrevAngleY = self->mAngleY;
    self->mModelAnim.Advance();
    self->UpdatePos(&self->mCylClsn);
    func_ov072_02120fd4(self, &self->mWithMeshClsn);
    func_ov072_02120f14(self);
    func_ov072_02120e50(self);
    self->mCylClsn.Clear();
    self->mCylClsn.Update();
    {
        int frame = (int)((unsigned)(self->mModelAnim.currFrame << 4) >> 16);
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, (Vector3 *)&self->mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol func_ov072_02121640
/* State 4, enter. */
extern "C" int func_ov072_02121640(daPgBby_c *self)
{
    self->mSubState = 0;
    self->mHorzSpeed = 0;
    self->mCylClsn.Clear();
    self->mStateId = 4;
    return 1;
}

// @symbol func_ov072_021214dc
/* State 4, update: the carrier's message 0xb2, then wait for the talk to
   close and be put down. */
extern "C" int func_ov072_021214dc(daPgBby_c *self)
{
    switch (self->mSubState) {
    case 0:
    {
        u32 flags = self->mFlags;
        int inMouth = (flags & 0x40000) != 0;
        if (inMouth) {
            self->mSubState++;
        } else {
            int talking = (flags & 0x20000) != 0;
            if (talking) break;
            if (inMouth) break;
            self->mEatingPlayer = 0;
            func_ov072_02121d50(self, 0);
        }
        break;
    }
    case 1:
    {
        if (self->mCarrier->ShowMessage(*self, 0xb2, 0, 0, 0)) {
            func_0201267c(0xf2, (Vector3 *)&self->mCamSpacePosX);
            self->mSubState++;
        }
        int spat = (self->mFlags & 0x80000) != 0;
        if (spat) {
            func_ov072_02121d50(self, 5);
        }
        break;
    }
    case 2:
        if (self->mCarrier->GetTalkState() == -1) {
            self->mCarrier->DropActor();
            self->mSubState++;
        }
        break;
    case 3:
    {
        int spat = (self->mFlags & 0x80000) != 0;
        if (spat) {
            func_ov072_02121d50(self, 5);
        }
        break;
    }
    }
    return 1;
}

// @symbol func_ov072_02121368
/* State 5, enter: spat out by Yoshi -- appear 0x50 units in front of the
   eating player and raycast down from above it. The statement order below
   is the retail schedule's. */
extern "C" int func_ov072_02121368(daPgBby_c *self)
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

    self->mFlags &= ~0x80000;
    self->mHorzSpeed = 0xa000;
    self->mVertSpeed = 0;

    eater = self->mEatingPlayer;
    posX = &self->mPosX;
    angle = eater->mAngleY;
    dist = 0x50000;
    round = 0x800;
    self->mAngleY = angle;
    angle = self->mAngleY;
    posY = &self->mPosY;
    posZ = &self->mPosZ;
    self->mPrevAngleY = angle;

    eater = self->mEatingPlayer;
    {
        s32 *src = &eater->mPosX;
        s32 srcX = src[0];
        fromp = &from;
        self->mPosX = srcX;
        s32 srcY = src[1];
        store = 1;
        self->mPosY = srcY;
        s32 srcZ = src[2];
        self->mPosZ = srcZ;
    }

    index = self->mAngleY;
    angle = data_02082214[(index >> 4) * 2];
    *posX = *posX + (int)(((s64)angle * dist + round) >> 12);
    *posY = *posY + dist;

    index = self->mAngleY;
    angle = data_02082214[(index >> 4) * 2 + 1];
    *posZ = *posZ + (int)(((s64)angle * dist + round) >> 12);

    eater = self->mEatingPlayer;
    y = eater->mPosY;
    z = eater->mPosZ;
    aboveY = y + dist;
    x = eater->mPosX;
    from.x = x;
    from.y = aboveY;
    from.z = z;
    self->DetectRaycastClsn(*fromp, *(Vector3 *)posX, store);

    self->mEatingPlayer = 0;
    self->mWithMeshClsn.SetLimMovFlag();
    self->mStateId = 5;
    return 1;
}

// @symbol func_ov072_021212c0
/* State 5, update: fall, bounce once at 40% and settle back into state 0. */
extern "C" int func_ov072_021212c0(daPgBby_c *self)
{
    dBgCh_Actr_UpdateContinuous_Veneer(&self->mWithMeshClsn);
    if (self->mWithMeshClsn.JustHitGround()) {
        self->mVertSpeed = (self->mVertSpeed * -0x28) / 100;
    } else if (self->mWithMeshClsn.IsOnGround()) {
        self->mVertSpeed = 0;
        self->mWithMeshClsn.ClearLimMovFlag();
        func_ov072_02121d50(self, 0);
    }
    self->UpdatePos(&self->mCylClsn);
    func_ov072_02120e50(self);
    self->mCylClsn.Clear();
    self->mCylClsn.Update();
    return 1;
}

// @symbol func_ov072_021210c4
/* Place the model: in the carrier's hands (one of four carry offsets) or at
   the actor, then size the drop shadow by the height above the ground. */
extern "C" void func_ov072_021210c4(daPgBby_c *self)
{
    int idx;
    Matrix4x3 *carry;
    Vector3 v;
    int shadowHeight;
    int shadowRadius;
    long long tmp;

    if (self->mCarrier && ((self->mFlags & 0x4000) ? 1 : 0) && *(int *)((char *)self->mCarrier + 0xc8)) {
        idx = 0;
        if (self->mCarrier->IsFrontSliding() || self->mCarrier->LostGrabbedObject()) {
            idx = 1;
        }
        if (self->mCarrier->param1 == 2) {
            idx = (idx + 2) & 0xff;
        }
        carry = self->UpdateCarry(*self->mCarrier, data_ov072_02122d3c[idx]);
        *(PgBbyMtx *)&self->mModelAnim.mat4x3 = *(PgBbyMtx *)carry;
    } else {
        Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
        self->mModelAnim.mat4x3.t.x = self->mPosX >> 3;
        self->mModelAnim.mat4x3.t.y = self->mPosY >> 3;
        self->mModelAnim.mat4x3.t.z = self->mPosZ >> 3;
    }

    if ((self->mFlags & 0x40000) ? 1 : 0) {
        return;
    }

    shadowHeight = 0xb4000;
    shadowRadius = 0x5a000;
    if (self->mCarrier && ((self->mFlags & 0x4000) ? 1 : 0)) {
        v.x = self->mPosX;
        v.y = self->mPosY;
        v.z = self->mPosZ;
        v.y -= 0xa000;
        dBgCh_Gnd ground;
        ground.SetObjAndPos(v, 0);
        if (ground.DetectClsn()) {
            shadowHeight = self->mPosY - ground.clsnY;
            if (shadowHeight < 0x1000) shadowHeight = 0x1000;
            tmp = (long long)shadowHeight * 0x180 + 0x800;
            shadowRadius = shadowRadius - (int)(tmp >> 12);
            if (shadowRadius < 0x1000) shadowRadius = 0x1000;
            if (shadowRadius > 0x5a000) shadowRadius = 0x5a000;
            shadowHeight = shadowHeight + 0x28000;
        }
    }

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel, &self->mModelAnim.mat4x3, shadowRadius, shadowHeight, 0xf);
}

// @symbol func_ov072_02120fd4
/* Wall/floor pass: on a slope, turn horizontal motion into vertical speed. */
extern "C" void func_ov072_02120fd4(daPgBby_c *self, dBgCh_Actr *clsn)
{
    Vector3 floorNormal;
    Vector3 wallNormal;
    int b = (int)((self->mFlags & 0x4000) != 0);
    if (b != 0) return;
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4, floorNormal);
        if (floorNormal.y != 0) {
            long long a = (long long)floorNormal.x * (long long)self->unk_0a4;
            long long bb = (long long)floorNormal.z * (long long)self->unk_0ac;
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((bb + 0x800) >> 12);
            self->mVertSpeed = -(_ZN4cstd4fdivEii(x + y, floorNormal.y) + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn) + 4, wallNormal);
    }
}

// @symbol func_ov072_02120f14
/* Refuse a step onto ground more than 0x1.9 away from the current height:
   put the actor back where it was last frame. */
extern "C" int func_ov072_02120f14(daPgBby_c *self)
{
    Vector3 v;

    if (self->mWithMeshClsn.IsOnGround()) {
        dBgCh_Gnd ground;
        {
            int z = self->mPosZ;
            int y = self->mPosY + 0x1e000;
            int x = self->mPosX;
            v.x = x;
            v.y = y;
            v.z = z;
        }
        ground.SetObjAndPos(v, self);
        if (ground.DetectClsn() != 0) {
            int d = ground.clsnY - self->mPosY;
            if (d < 0) d = -d;
            if (d <= 0x1900) goto bail;
        }
        self->mPosX = self->mPrevPosX;
        self->mPosY = self->mPrevPosY;
        self->mPosZ = self->mPrevPosZ;
        return 1;
    bail:
        ;
    }
    return 0;
}

// @symbol func_ov072_02120e50
/* Touched by the player (actor 0xbf): enter state 4 while talking, or
   state 2 when the player grabs it. */
extern "C" int func_ov072_02120e50(daPgBby_c *self)
{
    dActor_c *a;
    int isType;
    int flag;
    if (self->mCylClsn.otherOwner == 0) return 0;
    a = dActor_c::FindWithID(self->mCylClsn.otherOwner);
    if (a == 0 || (isType = (a->actorID == 0xbf)) == 0) return 0;
    flag = ((self->mFlags & 0x20000) != 0);
    if (flag) {
        self->mCarrier = (Player *)a;
        func_ov072_02121d50(self, 4);
    } else if ((self->mCylClsn.hitFlags & 0x1000) && ((Player *)a)->TryGrab(*self)) {
        self->mCarrier = (Player *)a;
        func_ov072_02121d50(self, 2);
    }
    return 1;
}

// @symbol func_ov072_02120e20
/* A diving player nearby startles it into state 1. */
extern "C" void func_ov072_02120e20(daPgBby_c *self)
{
    Player *player = self->ClosestPlayer();
    if (player->IsDiving()) func_ov072_02121d50(self, 1);
}

// @symbol func_ov072_02120ddc
/* Within 0x190 of the mother: state 3. */
extern "C" void func_ov072_02120ddc(daPgBby_c *self)
{
    dActor_c *mother = self->mMother;
    if (mother == 0)
        return;
    if (Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&mother->mPosX) >= 0x190000)
        return;
    func_ov072_02121d50(self, 3);
}

// @symbol func_ov072_02120d04
/* Respawn: fallen far below the level, or far from home with the timer
   run out, and off screen -- go back to the spawn position. */
extern "C" void func_ov072_02120d04(daPgBby_c *self)
{
    Vector3 out;
    Vector3 tmp;
    if (self->mStateId == 3) return;
    if ((unsigned)self->mPosY > (unsigned)(-0x1800000)) {
        if (Vec3_Dist((Vector3 *)&self->mSpawnPosX, (Vector3 *)&self->mPosX) < 0x190000) return;
        if (DecIfAbove0_Short(&self->mRespawnTimer) != 0) return;
    }
    Vec3_Asr(&tmp, (Vector3 *)&self->mSpawnPosX, 3);
    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_((void *)data_0209f43c, &data_0209b3ec, &tmp, 0x1f400, &out) <= 0xc350000) return;
    self->mPosX = self->mSpawnPosX;
    self->mPosY = self->mSpawnPosY;
    self->mPosZ = self->mSpawnPosZ;
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
