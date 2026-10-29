//cpp
/* leftovers:
 * - dBgCh_Actr::Init's member call mangles with int. The ROM row is
 *   Fix12<int>. GetFloorResult and GetWallResult are not on the header.
 * - func_0201267c is Sound::Play(3, id, pos). The state helpers stay
 *   func_ov018_* because the PMF literals relocate to those names.
 * - data_ov018_* handles are constructed by the sinit. g_profile is
 *   not this TU. common.h stays first so the matrix copy stays flat.
 * - 02111b3c keeps the (int) actorID compare. 02111d28 keeps the
 *   bone store. The follow state reloads mPlayer.
 *
 * daPgMthr_c, the Cool Cool Mountain mother penguin (PENGUIN_MOTHER).
 * She talks from the actor in the player's hand and, after giving
 * the star, walks toward whoever still holds her own baby.
 * State pairs are data_ov018_02113c4c: stand, talk, follow.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_pg_mthr_AssetId {
    ASSET_PENGUIN_BMD = 0x03fb,
    ASSET_PENGUIN_ANGRY_BTP = 0x03fd,
    ASSET_PENGUIN_WAIT1_BCA = 0x0406,
    ASSET_PENGUIN_WAIT1_BTP = 0x0407,
    ASSET_PENGUIN_WALK_BCA = 0x0408
};

#include "common.h"
#include "daPgMthr_c.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dExtAnmTexPat_c.h"
#include "dBgCh_Gnd.h"
#include "dExtFrameCtrl_c.h"
#include "SurfaceInfo.h"
#include "dBgPi.h"
#include "Player.h"
#include "dBgCh_Actr.h"
#include "Sound.h"
#include "cstd.h"
#include "math/Approach.h"

enum {
    ACTOR_PLAYER = 0xbf,
    ACTOR_BABY_PENGUIN = 0x100,
    ACTOR_POWER_STAR = 0xb2,
    CHAR_YOSHI = 3,
    MSG_NO_BABY = 0xac,
    MSG_OWN_BABY = 0xad,
    MSG_OTHER_BABY = 0xae,
    BABY_OWN = 0,
    BABY_OTHER = 1,
    /* 1500.0 and 450.0 in 20.12. The look test also wants the player in
       front (angle within 0x1400 of mAngleY). */
    HOME_RANGE = 0x5dc000,
    LOOK_RANGE = 0x1c2000,
    /* transforms[5]: 5 * sizeof(Matrix4x3) == 0xf0, the bone 02111d28 rotates
       and 02111a48 aims from. */
    LOOK_BONE = 5,
    /* dCc hit bit both the touch test and the new-player scan use. */
    HIT_TOUCH = 0x8000000
};

/* __sinit_d_a_pg_mthr.cpp constructs these, and the relocs at 02112c04 /
   02112c0c point the two tables at them:
     02113c00  file 0x3fb  BMD   model          (dtor 02017ab4)
     02113bf0  file 0x406  BCA   stand/talk
     02113c08  file 0x408  BCA   walk
     02113bf8  file 0x3fd  BTP   walk
     02113be8  file 0x407  BTP   stand/talk
     02112c04 = { &02113bf8, &02113be8 }
     02112c0c = { &02113bf0, &02113c08 }   (decl_common spells this int[])
   SharedFilePtr declares no constructor of its own, so the
   model handle gets the func_02017acc/func_02017ab4 pair, the animation
   handles get SharedFilePtr::Construct/Destruct_Anim and the texture
   sequences get SharedFilePtr_Construct_TexSeq/Destruct_TexSeq through
   thin declaration-only wrappers; the manifest aliases those mangled
   spellings to the arm9 destinations the retail initializer calls. */
struct PgMthrModelFileHandle : SharedFilePtr {
    PgMthrModelFileHandle(unsigned int fileID);
    ~PgMthrModelFileHandle();
};
struct PgMthrAnimFileResHandle : SharedFilePtr {
    PgMthrAnimFileResHandle(unsigned int fileID);
    ~PgMthrAnimFileResHandle();
};
struct PgMthrTexSeqFileHandle : SharedFilePtr {
    PgMthrTexSeqFileHandle(unsigned int fileID);
    ~PgMthrTexSeqFileHandle();
};

/* The three {enter, update} state pairs __sinit_d_a_pg_mthr.cpp copies
   from the PMF literals at 0211394c (see the header comment). */
struct PgMthrState {
    int (daPgMthr_c::*enter)();
    int (daPgMthr_c::*update)();
};
extern "C" {
extern Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern int AngleDiff(int a, int b);
extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
/* 0x0201267c plays Sound::Play(3, id, pos). Sound::PlayBank3 is the
   previous function (0x02012664); this TU's bl is not that one. */
extern int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *mf, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *mf, short angX);
extern Matrix4x3 data_020a0e68;
extern PgMthrAnimFileResHandle gPenguinWalkAnimationFile;
extern PgMthrTexSeqFileHandle gPenguinAngryTextureSequenceFile;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern Player *func_ov018_021118fc(daPgMthr_c *self);
extern PgMthrTexSeqFileHandle gPenguinWait1TextureSequenceFile;
extern PgMthrAnimFileResHandle gPenguinWait1AnimationFile;
extern PgMthrState data_ov018_02113c4c[3];
typedef void (daPgMthr_c::*PMF)();
extern void func_ov018_02112398(daPgMthr_c *self);
extern void func_ov018_0211235c(daPgMthr_c *self);
extern PgMthrModelFileHandle data_ov018_02113c00;
extern SharedFilePtr *data_ov018_02112c04[2];
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *act, int a, int b, void *c2, void *d);
}

// @symbol daPgMthr_c_classInit
extern "C" daPgMthr_c *daPgMthr_c_classInit()
{
    return new daPgMthr_c();
}

// @symbol _ZN10daPgMthr_c13InitResourcesEv
int daPgMthr_c::InitResources()
{
    void *m = dExtSimpleModel_c::LoadFile(data_ov018_02113c00);
    mAnmModel.SetFile((BMD_File *)m, 1, 1);
    for (int i = 0; i < 2; i++)
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov018_02112c0c[i]);
    for (int i = 0; i < 2; i++) {
        SharedFilePtr *t = data_ov018_02112c04[i];
        dExtAnmTexPat_c::LoadFile(*t);
        dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov018_02113c00.filePtr, *(BTP_File *)t->filePtr);
    }
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    /* Cylinder 260 x 300. Flags 0x4800004 / vuln 0x900000. */
    mdCcAc_c.Init(this, 0x104000, 0x12c000, 0x4800004, 0x900000);
    func_ov018_021123d0(0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    /* Mesh radius and height 50.0. */
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    Vector3 pos;
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
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mPlayer = 0;
    func_ov018_02111d28();
    return 1;
}

// @symbol _ZN10daPgMthr_c8BehaviorEv
int daPgMthr_c::Behavior()
{
    func_ov018_0211235c(this);
    mAnmModel.dExtFrameCtrl_c::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    mAnmModel.UpdateVerts();
    func_ov018_02111d28();
    return 1;
}

// @symbol _ZN10daPgMthr_c6RenderEv
int daPgMthr_c::Render()
{
    mTextureSequence.Update(mAnmModel.data);
    /* dExtAnmModel_c overrides Render. Qualifying it is the direct bl. */
    mAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

// @symbol _ZN10daPgMthr_c16OnPendingDestroyEv
void daPgMthr_c::OnPendingDestroy()
{
}

// @symbol _ZN10daPgMthr_c16CleanupResourcesEv
int daPgMthr_c::CleanupResources()
{
    data_ov018_02113c00.Release();
    for (int i = 0; i < 2; i++)
        ((SharedFilePtr *)data_ov018_02112c0c[i])->Release();
    for (int i = 0; i < 2; i++)
        data_ov018_02112c04[i]->Release();
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_021123d0Ei
void daPgMthr_c::func_ov018_021123d0(int i)
{
    mState = &data_ov018_02113c4c[i];
    func_ov018_02112398(this);
}

extern "C" void func_ov018_02112398(daPgMthr_c *self)
{
    PMF *enter = (PMF *)self->mState;
    (self->* *enter)();
}

extern "C" void func_ov018_0211235c(daPgMthr_c *self)
{
    PMF *update = (PMF *)self->mState + 1;
    (self->* *update)();
}

// @symbol _ZN10daPgMthr_c19func_ov018_021122ecEv
/* State 0 enter. Idle anim, stopped, not talking. */
int daPgMthr_c::func_ov018_021122ec()
{
    mAnmModel.SetAnim((BCA_File *)gPenguinWait1AnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTextureSequence.SetFile(*(BTP_File *)gPenguinWait1TextureSequenceFile.filePtr, 0, 0x1000, 0);
    mHorzSpeed = 0;
    mPlayer = 0;
    unk_37c = 0;
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02112234Ev
int daPgMthr_c::func_ov018_02112234()
{
    Player *toucher;
    dActor_c *held;
    Player *near;

    if (mGaveStar != 0)
        func_ov018_02111b3c();
    toucher = func_ov018_021118fc(this);
    held = 0;
    if (toucher != 0) {
        /* Yoshi's baby is in his mouth; this path does not count it. */
        if (toucher->param1 != CHAR_YOSHI)
            held = (dActor_c *)toucher->mHeldObj;
        func_ov018_02111968(toucher, (char *)held);
    }
    near = ClosestPlayer();
    func_ov018_02111a48((char *)near);
    UpdatePos(&mdCcAc_c);
    func_ov018_02111bf0((char *)this, &mWithMeshClsn);
    if (mGaveStar == 0 && mHoldingBaby == 0) {
        unsigned int frame = ((unsigned int)mAnmModel.currFrame << 4) >> 16;
        if (frame == 0x10 || frame == 0x25)
            func_0201267c(0xdf, &mCamSpacePos);
    }
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_021121dcEv
int daPgMthr_c::func_ov018_021121dc()
{
    mAnmModel.SetAnim((BCA_File *)gPenguinWait1AnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTalkStep = 0;
    mTalkTimer = 0x3c;
    unk_37c = 1;
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111facEv
int daPgMthr_c::func_ov018_02111fac()
{
    switch (mTalkStep) {
    case 0:
        if (mdCcAc_c.hitFlags & HIT_TOUCH) {
            if (mPlayer->StartTalk(*(fBase_c *)this, 1)) {
                u8 *p = &mTalkStep;
                *p = *p + 1;
            }
        } else {
            func_ov018_021123d0(0);
        }
        break;
    case 1:
        if (ApproachLinear(mAngleY, Vec3_HorzAngle(&mPos, &mPlayer->mPos), 0x514)) {
            Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
            Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x300000, -0x480000);
            {
                int mouth[3];
                mouth[0] = data_020a0e68.m[9];
                mouth[1] = data_020a0e68.m[10];
                mouth[2] = data_020a0e68.m[11];
                if (mPlayer->ShowMessage(*(fBase_c *)this, mMessageId, (Vector3 *)mouth, 0, 0)) {
                    func_0201267c(0xdf, &mCamSpacePos);
                    {
                        u8 *p = &mTalkStep;
                        *p = *p + 1;
                    }
                }
            }
        }
        break;
    case 2:
        if (mPlayer->GetTalkState() == -1) {
            if (mMessageId == MSG_OWN_BABY) {
                unsigned starParam;
                unsigned char slot;
                mGaveStar = 1;
                slot = (unsigned char)(param1 & 0xf);
                starParam = (unsigned)slot | 0x40;
                dActor_c::Spawn(ACTOR_POWER_STAR, starParam, mPos, 0, mAreaId, -1);
            }
            mPlayer->DropActor();
            {
                u8 *p = &mTalkStep;
                *p = *p + 1;
            }
        }
        break;
    case 3:
        if (!DecIfAbove0_Byte(&mTalkTimer))
            func_ov018_021123d0(0);
        break;
    }
    func_ov018_02111a48((char *)mPlayer);
    if (mGaveStar == 0 && mHoldingBaby == 0) {
        unsigned frame = ((unsigned)mAnmModel.currFrame << 4) >> 0x10;
        if (frame == 0x10 || frame == 0x25)
            func_0201267c(0xdf, &mCamSpacePos);
    }
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111f1cEv
int daPgMthr_c::func_ov018_02111f1c()
{
    if (mPlayer == 0 && mGaveStar == 0)
        func_ov018_021123d0(0);
    mHorzSpeed = 0x5000;
    mAnmModel.SetAnim((BCA_File *)gPenguinWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTextureSequence.SetFile(*(BTP_File *)gPenguinAngryTextureSequenceFile.filePtr, 0, 0x1000, 0);
    unk_37c = 2;
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111e28Ev
int daPgMthr_c::func_ov018_02111e28()
{
    ApproachLinear(mAngleY, Vec3_HorzAngle(&mPos, &mPlayer->mPos), 0x514);
    mPrevAngleY = mAngleY;
    UpdatePos(&mdCcAc_c);
    func_ov018_02111bf0((char *)this, &mWithMeshClsn);
    func_ov018_02111a48((char *)mPlayer);
    unsigned int frame = ((unsigned int)mAnmModel.currFrame << 4) >> 0x10;
    if (frame == 9 || frame == 0x15)
        func_0201267c(0xde, &mCamSpacePos);
    int holding;
    if (mPlayer->param1 == CHAR_YOSHI)
        holding = (mPlayer->mObjInMouth != 0);
    else
        holding = (mPlayer->mHeldObj != 0);
    if (holding == 0 || Vec3_Dist((Vector3 *)&mHomePosX, &mPlayer->mPos) > HOME_RANGE) {
        mPlayer = 0;
        func_ov018_021123d0(0);
    }
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111d28Ev
void daPgMthr_c::func_ov018_02111d28()
{
    /* Unused, but deleting this local DIFFs. */
    char *s = (char *)this;
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    /* m[9..11] are mat4x3's translation (pos >> 3). */
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x140000, 0x50000, 0xf);
    if (mLookAngX != 0 || mLookAngY != 0) {
        Matrix4x3 *bone = &mAnmModel.data.transforms[LOOK_BONE];
        data_020a0e68 = *bone;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mLookAngY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mLookAngX);
        mAnmModel.data.transforms[LOOK_BONE] = data_020a0e68;
    }
}

extern "C" {
/* dBgPi is 0x28. The copy below is not dBgPi::CopyTo: `b ? a : a` is the
   load that matches, and a real dBgPi local would run the constructor.
   A view of dBgPi past its vptr, flattened: nesting the real SurfaceInfo
   (whose Vector3 has a constructor) renumbers every @NNN in this file. */
struct dBgPiRawBody {
    int clpsW0, clpsW1;            /* surface.clps.w0/w1 */
    int normalX, normalY, normalZ; /* surface.normal */
    unsigned short triangleID, colliderIdx;
    int clsnID, owner, collider;
};
struct dBgPiRaw {
    void *vt;
    struct dBgPiRawBody info;
};
extern int data_02099368[];

void func_ov018_02111bf0(void *cv, void *wv)
{
    daPgMthr_c *self = (daPgMthr_c *)cv;
    dBgCh_Actr *w = (dBgCh_Actr *)wv;
    /* ROM calls the veneer at 0x02038420, not UpdateDiscreteNoLava. */
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (w->IsOnGround() != 0) {
        Vector3 n;
        dBgPi *floor = w->GetFloorResult();
        floor->surface.CopyNormalTo(n);
        if (n.y != 0) {
            int s = FX_MUL(n.x, self->mSpeedX) + FX_MUL(n.z, self->mSpeedZ);
            self->mVertSpeed = -(cstd::fdiv(s, n.y) + 0x8000);
        }
    }
    if (w->IsOnWall() != 0) {
        struct dBgPiRaw *src = (struct dBgPiRaw *)w->GetWallResult();
        struct dBgPiRaw cr;
        Vector3 wn;
        struct dBgPiRawBody *dst = &cr.info;
        int a = src->info.clpsW0;
        int b = src->info.clpsW1;
        /* `= a` DIFFs. The tautology is the load. */
        dst->clpsW0 = b ? a : a;
        dst->clpsW1 = b;
        int t = src->info.normalX;
        void *vt = (void *)data_02099368;
        dst->normalX = t;
        t = src->info.normalY;
        dst->normalY = t;
        t = src->info.normalZ;
        dst->normalZ = t;
        cr.vt = vt;
        cr.info.triangleID = src->info.triangleID;
        cr.info.colliderIdx = src->info.colliderIdx;
        cr.info.clsnID = src->info.clsnID;
        cr.info.owner = src->info.owner;
        cr.info.collider = src->info.collider;
        ((SurfaceInfo *)dst)->CopyNormalTo(wn);
        ((dBgPi *)&cr)->dBgPi::~dBgPi();
    }
}
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111b3cEv
void daPgMthr_c::func_ov018_02111b3c()
{
    Player *p = ClosestPlayer();
    dActor_c *held;

    if (p == 0)
        return;
    if (Vec3_Dist((Vector3 *)&mHomePosX, &p->mPos) > HOME_RANGE)
        return;
    if (p->param1 == CHAR_YOSHI)
        held = (dActor_c *)p->mObjInMouth;
    else
        held = (dActor_c *)p->mHeldObj;
    if (held == 0)
        return;
    /* (int) of the compare is the longer body. A plain != is 0xa8, ROM is 0xb4. */
    {
        int baby = (int)(held->actorID == ACTOR_BABY_PENGUIN);
        if (baby == 0)
            return;
    }
    if (held->param1 != BABY_OWN)
        return;
    mPlayer = p;
    func_ov018_021123d0(2);
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111a48EPc
void daPgMthr_c::func_ov018_02111a48(char *b)
{
    dActor_c *who;
    Fix12i dist;
    s16 horzAngle;
    s16 delta, vert;
    Vector3 lookTarget;
    Vector3 nodePos;
    Matrix4x3 *bone;
    Fix12i tx, ty, tz;

    if (b == 0)
        return;
    who = (dActor_c *)b;

    dist = Vec3_HorzDist(&mPos, &who->mPos);
    horzAngle = Vec3_HorzAngle(&mPos, &who->mPos);

    if (dist < LOOK_RANGE && AngleDiff(horzAngle, mAngleY) < 0x1400) {
        tz = who->mPosZ;
        ty = who->mPosY + 0x640000;
        tx = who->mPosX;
        lookTarget.x = tx;
        lookTarget.z = tz;
        lookTarget.y = ty;
        bone = &mAnmModel.data.transforms[LOOK_BONE];
        nodePos.x = bone->m[9];
        nodePos.y = bone->m[10];
        nodePos.z = bone->m[11];
        vert = Vec3_VertAngle(&nodePos, &lookTarget);
        delta = horzAngle - mAngleY;
    } else {
        vert = 0;
        delta = 0;
    }

    ApproachLinear(mLookAngY, delta, 0x250);
    ApproachLinear(mLookAngX, vert, 0x250);
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111968EPvPc
void daPgMthr_c::func_ov018_02111968(void *found, char *heldRaw)
{
    dActor_c *held = (dActor_c *)heldRaw;
    Player *who = (Player *)found;

    if (held) {
        int baby = held->actorID == ACTOR_BABY_PENGUIN;
        if (baby != 0) {
            if (held->param1 == BABY_OTHER) {
                mPlayer = who;
                mMessageId = MSG_OTHER_BABY;
                mHoldingBaby = 1;
                func_ov018_021123d0(1);
                return;
            }
        }
    }
    if (mGaveStar)
        return;
    if (held) {
        int baby = held->actorID == ACTOR_BABY_PENGUIN;
        if (baby != 0) {
            if (held->param1 == BABY_OWN) {
                mMessageId = MSG_OWN_BABY;
                goto talk;
            }
        }
    }
    if (mHoldingBaby)
        return;
    mMessageId = MSG_NO_BABY;
talk:
    mPlayer = who;
    func_ov018_021123d0(1);
}

extern "C" {
Player *func_ov018_021118fc(daPgMthr_c *self) {
    Player *newToucher = 0;
    if (self->mdCcAc_c.hitFlags & HIT_TOUCH) {
        struct dActor_c *a = dActor_c::FindWithID(self->mdCcAc_c.otherOwner);
        if (a) {
            int ok = (a->actorID == ACTOR_PLAYER) ? 1 : (int)newToucher;
            if (ok) {
                if (a != self->mLastPlayer)
                    newToucher = static_cast<Player *>(a);
                self->mLastPlayer = a;
            }
        }
    } else {
        self->mLastPlayer = newToucher;
    }
    return newToucher;
}
}

/* File-scope objects __sinit_d_a_pg_mthr.cpp initializes: the five resource
   handles in construction order, then the three {enter, update} state pairs
   the dispatch helpers index by state. */
PgMthrModelFileHandle data_ov018_02113c00(ASSET_PENGUIN_BMD);   /* model */
PgMthrAnimFileResHandle gPenguinWait1AnimationFile(ASSET_PENGUIN_WAIT1_BCA); /* stand/talk anim */
PgMthrAnimFileResHandle gPenguinWalkAnimationFile(ASSET_PENGUIN_WALK_BCA); /* walk anim */
PgMthrTexSeqFileHandle gPenguinAngryTextureSequenceFile(ASSET_PENGUIN_ANGRY_BTP);  /* walk texseq */
PgMthrTexSeqFileHandle gPenguinWait1TextureSequenceFile(ASSET_PENGUIN_WAIT1_BTP);  /* stand/talk texseq */

PgMthrState data_ov018_02113c4c[3] = {
    { &daPgMthr_c::func_ov018_021122ec, &daPgMthr_c::func_ov018_02112234 }, /* stand */
    { &daPgMthr_c::func_ov018_021121dc, &daPgMthr_c::func_ov018_02111fac }, /* talk */
    { &daPgMthr_c::func_ov018_02111f1c, &daPgMthr_c::func_ov018_02111e28 }, /* follow */
};
