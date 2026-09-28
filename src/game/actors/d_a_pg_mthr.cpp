//cpp
/**
 * daPgMthr_c -- Cool, Cool Mountain mother penguin (PENGUIN_MOTHER 257), ov018.
 *
 * She stands at her home (state 0 holds mHorzSpeed at 0) and talks when a
 * player touches her. The actor in his hand picks the message: 0xac with
 * nothing, 0xad with her own BABY_PENGUIN (actor 0x100, param1 0), 0xae
 * with the other baby (param1 1). Yoshi is player param1 3 and carries the
 * baby in mObjInMouth; the other characters use mHeldObj. The idle touch
 * path only looks at the hand. When the 0xad talk ends she spawns
 * POWER_STAR (actor 0xb2, param (param1 & 0xf) | 0x40) and sets mGaveStar.
 * After that, a player within 1500 of her home who holds her own baby --
 * mouth included -- sends her to state 2. She walks toward him and returns
 * to state 0 when he lets go or leaves that range.
 *
 * State pairs live in data_ov018_02113c4c, filled by __sinit_ov018_02112c80
 * from the PMF literals at 0211394c. Index << 4 selects {enter, update}:
 *   0  021122ec / 02112234   stand
 *   1  021121dc / 02111fac   talk
 *   2  02111f1c / 02111e28   follow
 * common.h stays first: Matrix4x3 is the flat 12-word copy this file assigns.
 *
 * Measured on this file, 2004/b56, and left as the call that matches:
 *   SetAnim(Fix12<int> speed = {0x1000}) is 021121dc 0x58->0x60 and
 *   021122ec 0x70->0x78. SetFile the same way is 02111f1c 0x90->0x9c.
 *   dCcAc_c::Init(Fix12<int> radius/height = {...}) is InitResources
 *   0x1ac->0x1bc; an int literal does not convert, so that call does not
 *   compile. Each aggregate also emitted an unlicensed local .data word.
 *   DropShadowRadHeight the same way is 02111d28 0x100->0x110.
 *   dBgCh_Actr::Init(this, 0x32000, 0x32000, 0, 0) keeps the 0x1ac body, but
 *   the reloc is _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_, which is
 *   not the ROM row (5Fix12IiE). The header parameter is Fix12i, a typedef
 *   of s32. 02111b3c without the (int) actorID compare is 0xa8, not 0xb4.
 *   Writing 02111d28's translation through mat4x3.m[] and passing that
 *   member into DropShadow, then storing the bone back through one pointer,
 *   is 0xfc, not 0x100. Hoisting the follow state's Player* is 0xf8, not
 *   0xf4; reloading mPlayer matches.
 *   func_0201267c is Sound::Play(3, id, pos) at 0x0201267c. PlayBank3 is the
 *   other wrapper, at 0x02012664. GetFloorResult and GetWallResult are not
 *   on dBgCh_Actr.h. The state helpers stay func_ov018_*: the PMF literals
 *   relocate to those names. The file symbols stay data_ov018_*; the sinit
 *   TU constructs them. g_profile_PENGUIN_MOTHER is not in this text TU.
 */

#include "common.h"
#include "daPgMthr_c.h"
#include "types.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"
#include "Animation.h"
#include "SurfaceInfo.h"
#include "dBgPi.h"
#include "Player.h"

bool ApproachLinear(short &value, short target, short step);

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

/* SharedFilePtr's header has no fields. Model::LoadFile and
   SharedFilePtr::Load both store the loaded buffer at +0x4. */
struct PgLoadedFile {
    u16 fileID;
    u8 numRefs;
    u8 pad;
    void *filePtr;
};

/* __sinit_ov018_02112c80 constructs these, and the relocs at 02112c04 /
   02112c0c point the two tables at them:
     02113c00  file 0x3fb  BMD   model          (dtor 02017ab4)
     02113bf0  file 0x406  BCA   stand/talk
     02113c08  file 0x408  BCA   walk
     02113bf8  file 0x3fd  BTP   walk
     02113be8  file 0x407  BTP   stand/talk
     02112c04 = { &02113bf8, &02113be8 }
     02112c0c = { &02113bf0, &02113c08 }   (decl_common spells this int[]) */
extern "C" {
extern Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern int AngleDiff(int a, int b);
extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
/* 0x0201267c plays Sound::Play(3, id, pos). Sound::PlayBank3 is the
   previous function (0x02012664); this TU's bl is not that one. */
extern void func_0201267c(int a, void *b);
extern int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, ShadowModel &sm, Matrix4x3 &mf, int c, int d, unsigned int e);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *mf, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *mf, short angX);
extern Matrix4x3 data_020a0e68;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *f, int a, int b, unsigned int c);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *, BTP_File &f, int a, int b, unsigned int c);
extern SharedFilePtr data_ov018_02113c08;
extern SharedFilePtr data_ov018_02113bf8;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern dActor_c *func_ov018_021118fc(char *c);
extern SharedFilePtr data_ov018_02113be8;
extern SharedFilePtr data_ov018_02113bf0;
extern char data_ov018_02113c4c[];
typedef void (daPgMthr_c::*PMF)();
extern void func_ov018_02112398(daPgMthr_c *self);
extern void func_ov018_0211235c(daPgMthr_c *self);
extern SharedFilePtr data_ov018_02113c00;
extern SharedFilePtr *data_ov018_02112c04[2];
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *act, int a, int b, unsigned int c2, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *c2, void *d);
}

// @symbol daPgMthr_c_classInit
extern "C" daPgMthr_c *daPgMthr_c_classInit()
{
    return new daPgMthr_c();
}

// @symbol _ZN10daPgMthr_c13InitResourcesEv
int daPgMthr_c::InitResources()
{
    void *m = Model::LoadFile(data_ov018_02113c00);
    mModelAnim.SetFile((BMD_File *)m, 1, 1);
    for (int i = 0; i < 2; i++)
        Animation::LoadFile(*(SharedFilePtr *)data_ov018_02112c0c[i]);
    for (int i = 0; i < 2; i++) {
        SharedFilePtr *t = data_ov018_02112c04[i];
        TextureSequence::LoadFile(*t);
        TextureSequence::Prepare(
            *(BMD_File *)((PgLoadedFile *)&data_ov018_02113c00)->filePtr,
            *(BTP_File *)((PgLoadedFile *)t)->filePtr);
    }
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    /* Cylinder 260 x 300. Flags 0x4800004 / vuln 0x900000. */
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x104000, 0x12c000, 0x4800004, 0x900000);
    func_ov018_021123d0(0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    /* Mesh radius and height 50.0. */
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
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
    mModelAnim.Animation::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    mModelAnim.UpdateVerts();
    func_ov018_02111d28();
    return 1;
}

// @symbol _ZN10daPgMthr_c6RenderEv
int daPgMthr_c::Render()
{
    mTextureSequence.Update(mModelAnim.data);
    /* ModelAnim overrides Render. Qualifying it is the direct bl. */
    mModelAnim.Model::Render(0);
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
    mState = data_ov018_02113c4c + (i << 4);
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
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)((PgLoadedFile *)&data_ov018_02113bf0)->filePtr, 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, *(BTP_File *)((PgLoadedFile *)&data_ov018_02113be8)->filePtr, 0, 0x1000, 0);
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
    toucher = (Player *)func_ov018_021118fc((char *)this);
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
        unsigned int frame = ((unsigned int)mModelAnim.currFrame << 4) >> 16;
        if (frame == 0x10 || frame == 0x25)
            func_0201267c(0xdf, &mCamSpacePosX);
    }
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_021121dcEv
int daPgMthr_c::func_ov018_021121dc()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)((PgLoadedFile *)&data_ov018_02113bf0)->filePtr, 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
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
        if (ApproachLinear(mAngleY,
                Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&mPlayer->mPosX), 0x514)) {
            Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
            Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x300000, -0x480000);
            {
                int mouth[3];
                mouth[0] = data_020a0e68.m[9];
                mouth[1] = data_020a0e68.m[10];
                mouth[2] = data_020a0e68.m[11];
                if (mPlayer->ShowMessage(*(fBase_c *)this, mMessageId, (Vector3 *)mouth, 0, 0)) {
                    func_0201267c(0xdf, &mCamSpacePosX);
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
                dActor_c::Spawn(ACTOR_POWER_STAR, starParam, *(Vector3 *)&mPosX, 0, mAreaId, -1);
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
        unsigned frame = ((unsigned)mModelAnim.currFrame << 4) >> 0x10;
        if (frame == 0x10 || frame == 0x25)
            func_0201267c(0xdf, &mCamSpacePosX);
    }
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111f1cEv
int daPgMthr_c::func_ov018_02111f1c()
{
    if (mPlayer == 0 && mGaveStar == 0)
        func_ov018_021123d0(0);
    mHorzSpeed = 0x5000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)((PgLoadedFile *)&data_ov018_02113c08)->filePtr, 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, *(BTP_File *)((PgLoadedFile *)&data_ov018_02113bf8)->filePtr, 0, 0x1000, 0);
    unk_37c = 2;
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111e28Ev
int daPgMthr_c::func_ov018_02111e28()
{
    ApproachLinear(mAngleY,
        Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&mPlayer->mPosX), 0x514);
    mPrevAngleY = mAngleY;
    UpdatePos(&mdCcAc_c);
    func_ov018_02111bf0((char *)this, &mWithMeshClsn);
    func_ov018_02111a48((char *)mPlayer);
    unsigned int frame = ((unsigned int)mModelAnim.currFrame << 4) >> 0x10;
    if (frame == 9 || frame == 0x15)
        func_0201267c(0xde, &mCamSpacePosX);
    int holding;
    if (mPlayer->param1 == CHAR_YOSHI)
        holding = (mPlayer->mObjInMouth != 0);
    else
        holding = (mPlayer->mHeldObj != 0);
    if (holding == 0 || Vec3_Dist((Vector3 *)&mHomePosX, (Vector3 *)&mPlayer->mPosX) > HOME_RANGE) {
        mPlayer = 0;
        func_ov018_021123d0(0);
    }
    return 1;
}

// @symbol _ZN10daPgMthr_c19func_ov018_02111d28Ev
void daPgMthr_c::func_ov018_02111d28()
{
    char *s = (char *)this;
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    /* 0x114/0x118/0x11c are mat4x3's translation (pos >> 3). Writing those
       through m[] and passing the member into DropShadow, then storing the
       bone back through the same pointer, was 0xfc. ROM is 0x100. */
    *(int *)(s + 0x114) = mPosX >> 3;
    *(int *)(s + 0x118) = mPosY >> 3;
    *(int *)(s + 0x11c) = mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, mShadowModel, *(Matrix4x3 *)(s + 0xf0), 0x140000, 0x50000, 0xf);
    if (mLookAngX != 0 || mLookAngY != 0) {
        Matrix4x3 *bone = &mModelAnim.data.transforms[LOOK_BONE];
        data_020a0e68 = *bone;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mLookAngY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mLookAngX);
        *(Matrix4x3 *)((*(char **)(s + 0xe8)) + 0xf0) = data_020a0e68;
    }
}

extern "C" {
/* dBgPi is 0x28. The copy below is not dBgPi::CopyTo: `b ? a : a` is the
   load that matches, and a real dBgPi local would run the constructor. */
struct dBgPiRawBody {
    int a, b, c, d, e;
    unsigned short f, g;
    int h, i, j;
};
struct dBgPiRaw {
    void *vt;
    struct dBgPiRawBody info;
};
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *w);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(dBgCh_Actr *w);
int _ZN4cstd4fdivEii(int a, int b);
struct dBgPiRaw *_ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr *w);
void _ZN5dBgPiD1Ev(struct dBgPiRaw *r);
extern int data_02099368[];

void func_ov018_02111bf0(void *cv, void *wv)
{
    daPgMthr_c *self = (daPgMthr_c *)cv;
    dBgCh_Actr *w = (dBgCh_Actr *)wv;
    /* ROM calls the veneer at 0x02038420, not UpdateDiscreteNoLava. */
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (w->IsOnGround() != 0) {
        Vector3 n;
        dBgPi *floor = (dBgPi *)_ZNK10dBgCh_Actr14GetFloorResultEv(w);
        floor->surface.CopyNormalTo(n);
        if (n.y != 0) {
            int s = (int)(((long long)n.x * self->unk_0a4 + 0x800) >> 0xc)
                  + (int)(((long long)n.z * self->unk_0ac + 0x800) >> 0xc);
            self->mVertSpeed = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
        }
    }
    if (w->IsOnWall() != 0) {
        struct dBgPiRaw *src = _ZNK10dBgCh_Actr13GetWallResultEv(w);
        struct dBgPiRaw cr;
        Vector3 wn;
        struct dBgPiRawBody *dst = &cr.info;
        int a = *(int *)((char *)src + 4);
        int b = *(int *)((char *)src + 8);
        /* `= a` DIFFs. The tautology is the load. */
        *(int *)((char *)dst + 0) = b ? a : a;
        *(int *)((char *)dst + 4) = b;
        int t = *(int *)((char *)src + 0xc);
        void *vt = (void *)data_02099368;
        *(int *)((char *)dst + 8) = t;
        t = *(int *)((char *)src + 0x10);
        *(int *)((char *)dst + 0xc) = t;
        t = *(int *)((char *)src + 0x14);
        *(int *)((char *)dst + 0x10) = t;
        cr.vt = vt;
        cr.info.f = *(unsigned short *)((char *)src + 0x18);
        cr.info.g = *(unsigned short *)((char *)src + 0x1a);
        cr.info.h = *(int *)((char *)src + 0x1c);
        cr.info.i = *(int *)((char *)src + 0x20);
        cr.info.j = *(int *)((char *)src + 0x24);
        ((SurfaceInfo *)dst)->CopyNormalTo(wn);
        _ZN5dBgPiD1Ev(&cr);
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
    if (Vec3_Dist((Vector3 *)&mHomePosX, (Vector3 *)&p->mPosX) > HOME_RANGE)
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

    dist = Vec3_HorzDist((Vector3 *)&mPosX, (Vector3 *)&who->mPosX);
    horzAngle = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&who->mPosX);

    if (dist < LOOK_RANGE && AngleDiff(horzAngle, mAngleY) < 0x1400) {
        tz = who->mPosZ;
        ty = who->mPosY + 0x640000;
        tx = who->mPosX;
        lookTarget.x = tx;
        lookTarget.z = tz;
        lookTarget.y = ty;
        bone = &mModelAnim.data.transforms[LOOK_BONE];
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
struct dActor_c *func_ov018_021118fc(char *c)
{
    daPgMthr_c *self = (daPgMthr_c *)c;
    struct dActor_c *newToucher = 0;
    if (self->mdCcAc_c.hitFlags & HIT_TOUCH) {
        struct dActor_c *a = dActor_c::FindWithID(self->mdCcAc_c.otherOwner);
        if (a) {
            int ok = (a->actorID == ACTOR_PLAYER) ? 1 : (int)newToucher;
            if (ok) {
                if (a != self->mLastPlayer)
                    newToucher = a;
                self->mLastPlayer = a;
            }
        }
    } else {
        self->mLastPlayer = newToucher;
    }
    return newToucher;
}
}
