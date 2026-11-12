//cpp
/* Production translation unit for ov063/daTrs_c.
 * 97 functions, .text 0x02115ee0..0x0211c600: the Boo (TERESA, actor 0xd1) and
 * Big Boo (BOSS_TERESA, 0xd2), both daTrs_c; the Boo cage (T_BASKET, 0xd4,
 * daTBasket_c) that a variant-5 Boo carries; and ICON_TERESA (0xd3,
 * daTrsIcon_c), the marker a Big Boo spawns, which tracks a star with
 * dActor_c::TrackStar and spawns it when the Big Boo is beaten.
 *
 * One daTrs_c class serves every Boo: mVariant (param1's low nibble, +0xc
 * for BOSS_TERESA) picks the Behavior handler, and each handler keeps its
 * own mState numbering. Each handler has a one-line note above it.
 *
 * One object: its .data holds _ZTS7daTrs_c (0x0211e720) through
 * _ZTV11daTBasket_c (0x0211e930), all three classes' RTTI and vtables with the
 * four registry profiles between them, and __sinit_d_a_trs.cpp (0x0211e29c)
 * constructs the file handles this file loads and releases.
 *
 * The three out-of-line destructors are the key functions, so this TU emits
 * the vtables. `#pragma defer_codegen off` lays .text down in source order
 * (this file is ROM-ascending) and makes each destructor come out D1, D0, then
 * a D2 the cartridge has no home for.
 *
 * The func_ov063_* helpers are methods now (daTrs_c, one on daTBasket_c):
 * r0 is the object, so each one is a member of that class.
 * func_ov063_02118f24 stays free (C linkage): its receiver is the
 * ICON_TERESA marker, not the Boo. func_ov063_0211a0a8 is pure math.
 *
 * deslop leftovers:
 * - DropShadowRadHeight, dCcAcPos_c::Init, dCcAc_c::Init, dBgCh_Actr::Init,
 *   UpdateCapPos, dExtAnmModel_c::SetAnim, SpawnFireball, daPly_c::
 *   Bounce/Hurt, KillByInvincibleChar and the Sound/Particle/dCamera_c calls
 *   retain scalar or opaque boundaries. UpdateCapPos has a shared reference
 *   declaration, but the direct call currently changes two argument-address
 *   setup instructions in 021166ac; that source-shape reconstruction remains open.
 * - `*(u16 *)&member` puns stay on mStateTimer and the angle fields: the
 *   header declares s16 but the retail code loads those sites unsigned
 *   (ldrh), and the signed member spelling emits ldrsh.
 * - The MERRYGOROUND/KAIDAN (daTrsTrap_c), released coin (daCoin_c) and
 *   SpawnFireball result (daFPknBall_c) are reached through file-local views:
 *   including any of their headers renumbers this file's @NNN labels.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daTrs_c_AssetId {
    ASSET_BOSS_TERESA_BMD = 0x02cc,
    ASSET_BOSS_TERESA_FIRE_BCA = 0x02cd,
    ASSET_BOSS_TERESA_FIRE_ST_BCA = 0x02ce,
    ASSET_BOSS_TERESA_WAIT_BCA = 0x02cf,
    ASSET_TERESA_BMD = 0x0427,
    ASSET_TERESA_WAIT_BCA = 0x0428,
    ASSET_T_BASKET_BMD = 0x06c9
};

#pragma defer_codegen off

#include "common.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "types.h"
#include "dActor_c.h"
#include "daTrs_c.h"
#include "daTBasket_c.h"
#include "daTrsIcon_c.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "Sound.h"
#include "dCamera_c.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "LevelRng.h"
#include "SaveBlock.h"
#include "nitro/fx/fx_trig.h"
#include "math/MtxStack.h"

/* Views of three classes this Boo reaches through other actors' pointers.
   Their headers are not included because each one renumbers this file's
   pinned @NNN labels; offsets and member names are the headers'. */
struct daCoinView { /* daCoin_c.h: the released coin */
    u8 pad[0x3a8];
    u16 mDisappearTimer; /* 0x3a8 */
    u8 mNoClsnTimer;     /* 0x3aa */
};
struct daTrsTrapView { /* daTrsTrap_c.h: KAIDAN, MERRYGOROUND */
    u8 pad[0x150];
    u8 mState;        /* 0x150 */
    u8 mStepIndex;    /* 0x151 */
    u8 mInitLatch;    /* 0x152 */
    u8 mOccupiedFlag; /* 0x153 */
    u8 pad_154;       /* 0x154 */
    u8 mTriggered;    /* 0x155 */
};
struct daFPknBallView { /* daFPknBall_c.h: SpawnFireball's result */
    u8 pad[0x364];
    s32 mMaxDistance; /* 0x364 */
    u8 pad_368[0x6];
    u8 mMirrored; /* 0x36e */
};
/* One 0x34-byte runtime bone of mAnmModel.data.bones (ModelComponents types
   the array as BMD_Bone). Same rotX/rotY/rotZ at 0x1a as d_a_moray.cpp's
   MorayBone and d_a_cnn.cpp's CnnBone; the two render paths pose bone 0
   with the Boo's angles. */
struct TrsBone {
    u8 pad_00[0x1a];
    s16 rotX; /* 0x1a */
    s16 rotY; /* 0x1c */
    s16 rotZ; /* 0x1e */
    u8 pad_20[0x14];
};
#ifndef SM64DS_PLATFORM_PC
typedef char TrsBone_size_must_be_0x34[sizeof(TrsBone) == 0x34 ? 1 : -1];
#endif


extern "C" {
/* shared engine entry points without a header declaration */
void LoadKeyModels(int n);
void UnloadKeyModels(int n);
unsigned char NumStars(void);
int IsStarCollectedInCurLevel(int n);
u8 IsAreaShowing(int idx);

/* ABI seams whose header member forms do not reproduce the ROM (see notes) */
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, dActor_c *actor, Fix12i radius, Fix12i height, Vector3_16 *a,
                         Vector3_16 *b);
unsigned int d_a_trsNewHold(unsigned int uniqueID, unsigned int effectID, int x, int y, int z,
                            const void *dir, void *callback);

extern signed char gCurrentSublevel;
extern unsigned char gCurrentEntranceId;
/* Per-variant tables indexed by mVariant: death mode, hurt damage. */
extern int data_ov063_0211e22c[];
extern int data_ov063_0211e1ec[];
extern Matrix4x3 data_020a0e68;
}

/* The seven file handles __sinit_ov063_0211e29c constructs. SharedFilePtr
 * declares no constructor of its own, so the model handles get the
 * model-file pair func_02017acc/func_02017ab4 and the animation handles get
 * SharedFilePtr::Construct plus the anim destructor through thin
 * declaration-only wrappers; the manifest aliases those mangled spellings to
 * the arm9 destinations the retail initializer calls. The destructor chain records the file ID; actor cleanup releases handles
 * separately. These local wrappers retain the existing lifecycle aliases. */
struct TrsModelFileResHandle : SharedFilePtr {
    TrsModelFileResHandle(unsigned int fileID);
    ~TrsModelFileResHandle();
};
struct TrsAnimationResFileHandle : SharedFilePtr {
    TrsAnimationResFileHandle(unsigned int fileID);
    ~TrsAnimationResFileHandle();
};

extern TrsModelFileResHandle data_ov063_0211edc4;
extern TrsAnimationResFileHandle data_ov063_0211edcc;
extern TrsAnimationResFileHandle data_ov063_0211edd4;
extern TrsAnimationResFileHandle data_ov063_0211eddc;
extern TrsAnimationResFileHandle data_ov063_0211ede4;
extern TrsModelFileResHandle data_ov063_0211edec;
extern TrsModelFileResHandle data_ov063_0211edf4;
extern SharedFilePtr data_ov002_0210d9c8;
extern SharedFilePtr data_ov002_0210d9f8;
extern SharedFilePtr data_ov002_0210d9b8;

/* File-local helpers and engine entry points, with the C-linkage spellings
 * these bodies were matched against. */
extern "C" {
int d_a_trsPlaySubHold(unsigned int a, unsigned int b, unsigned int c, s32 d, int e);
int func_020092c4(void *cam, void *out, void *target);
void func_02012694(unsigned int a, const Vector3 *p);
u16 func_0201277c(unsigned int a);
extern dCamera_c *gCamera;
void func_ov063_02118f24(daTrsIcon_c *icon, const Vector3 &pos);
int func_ov063_0211a0a8(int a0, int a1, int a2, int a3, int a4);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern void func_020167a4(dExtAnmModel_c *model);
extern void d_a_trsChangeMusicVolumeHold(unsigned a, int fx);
extern void func_02011cfc(void);
extern s16 data_ov063_0211e1c8[];
/* The call at 0x02118004 relocates to overlays(27,55). r0 still holds the
   state timer there, so the callee takes no object: ov055's free function at
   0x02111264, not ov027's daObjSlIceBlock_c::CleanupResources. */
extern void func_ov055_02111264(void);
extern int func_ov002_020c51d0(void *c, int *st);
extern u16 func_02011d14(void);
extern int data_ov008_02111b6c;
extern s16 data_ov063_0211e1dc[];
extern s16 data_ov063_0211e1e4[];
extern s16 data_ov063_0211e1c0[];
extern int data_ov063_0211e1d0[];
extern int gMusicVolume;
extern u8 data_0209d660;
extern short data_ov063_0211e7e0[];
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
}

/* Actor ids this file spawns, finds or compares (the names are the ones in
   symbols/actor_debug_names.tsv). */
enum TrsActorID {
    TRS_ID_KAIDAN        = 0x41,
    TRS_ID_MERRYGOROUND  = 0x9f,
    TRS_ID_PLAYER        = 0xbf,
    TRS_ID_TERESA        = 0xd1,
    TRS_ID_BOSS_TERESA   = 0xd2,
    TRS_ID_ICON_TERESA   = 0xd3,
    TRS_ID_T_BASKET      = 0xd4,
    TRS_ID_OBJ_KEY       = 0x11a,
    TRS_ID_COIN          = 0x120,
    TRS_ID_RED_COIN      = 0x121,
    TRS_ID_BLUE_COIN     = 0x122,
    TRS_ID_END_OF        = 0x187
};

/* mBooFlags' bits as masks, for the sites that matched as a read-modify-write
   of the whole halfword (see BooFlags16 in daTrs_c.h for what each bit does). */
enum TrsFlag {
    TRS_FLAG_LIMIT_Z               = 0x1,
    TRS_FLAG_CARRYING              = 0x2,
    TRS_FLAG_FOLLOW_HEADING        = 0x4,
    TRS_FLAG_VISIBLE               = 0x8,
    TRS_FLAG_CAN_APPEAR            = 0x10,
    TRS_FLAG_GROUND_SEEN           = 0x20,
    TRS_FLAG_HIT_BY_PARAM3_PLAYER  = 0x40,
    TRS_FLAG_DEFEATED              = 0x80,
    TRS_FLAG_MIRRORED              = 0x100,
    TRS_FLAG_SOUND_STARTED         = 0x200,
    TRS_FLAG_CAM_HELD              = 0x400
};

inline int inline_fn(int arg0) { return data_020a0e68.m[arg0]; }
struct Frame {
    VecFx32 v;
    int pad[10];
};
/* mBooFlags seen as 8:1:7, for the one `mirrored` toggle in 02117364. Spelling
   that toggle as mBooFlags.mirrored ^= 1 keeps the bytes but lowers every
   @NNN by one for the dropped local and one more for this dropped
   anonymous typedef (measured). */
typedef struct { u16 lo8 : 8; u16 mirrored : 1; u16 hi7 : 7; } FlagW;

daTrs_c::~daTrs_c()
{
}

daTBasket_c::~daTBasket_c()
{
}

daTrsIcon_c::~daTrsIcon_c()
{
}

int daTrs_c::OnAimedAtWithEgg() {
    return mdCcAcPos_c.height / 2;
}

void daTrs_c::func_ov063_021160d4() {

    int *p;
    this->mClsnOff.x = 0;
    this->mClsnOff.y = 0;
    this->mClsnOff.z = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, this->mPosX, this->mPosY, this->mPosZ);
    MTX_Concat43(this->mAnmModel.data.transforms, &data_020a0e68, &data_020a0e68);
    this->mClsnOff.x = data_020a0e68.m[9];
    this->mClsnOff.y = inline_fn(10);
    this->mClsnOff.z = inline_fn(11);
    VEC_Subtract(&this->mClsnOff, &this->mPos, &this->mClsnOff);
    VEC_Subtract(&this->mClsnOff, &this->mClsnBase, &this->mClsnOff);
    Vec3_MulScalarInPlace((VecFx32 *)&this->mClsnOff.x, 0x6800);
    p = (int *)&this->mClsnOff.z;
    *p += this->mClsnZBias;
}

int daTrs_c::func_ov063_02116190() {

    Vector3 vec;
    daPly_c *p;

    if (this->mBooFlags.limitZ) {
        p = ClosestPlayer();
        if (p != 0 && p->mPosZ >= 0x3e8000)
            return 1;
    } else if (this->mPosY <= (int)0xff768000) {
        p = ClosestPlayer();
        vec.x = -0xbe000;
        vec.y = (int)0xff66e000;
        vec.z = 0xbe000;
        if (Vec3_HorzDist(&vec, &p->mPos) >= 0x73a000)
            return 1;
    }
    return 0;
}


void daTrs_c::func_ov063_02116244() {

    if (this->mBooDefeatCount < 5) return;
    if (this->mSpawnedActorID != 0) return;
    this->mFoundActor = dActor_c::Spawn(TRS_ID_ICON_TERESA, this->param1, this->mPos,
                                        (const Vector3_16 *)(0), this->mAreaIdx, -1);
    if (this->mFoundActor != 0) this->mSpawnedActorID = this->mFoundActor->uniqueID;
    this->mFoundActor = 0;
}


void daTrs_c::func_ov063_021162c8() {

    if (this->mBooFlags.hitByParam3Player) {
        this->mState = 4;
    } else {
        this->mState = 3;
        Vector3 v;
        v.x = this->mCapPosX;
        v.y = this->mCapPosY;
        v.z = this->mCapPosZ;
        ((dCapEnemy_c*)this)->ReleaseCap(*(Vector3*)&v);
        if (this->mBooFlags.carrying) {
            unsigned int flags = 2;
            if (this->mCarriedID == TRS_ID_RED_COIN) flags |= 0x10;
            dActor_c *a = dActor_c::Spawn(this->mCarriedID, flags,
                                    *(Vector3*)&this->mCarriedPosX, 0,
                                    this->mAreaIdx, -1);
            if (a != 0) {
                ((daCoinView *)a)->mNoClsnTimer = 0xa;
                if (this->mCarriedID == TRS_ID_RED_COIN)
                    ((daCoinView *)a)->mDisappearTimer = 0;
            }
            this->mBooFlags.carrying = 0;
        }
    }
    func_0201267c(0x14a, &this->mCamSpacePos);
}


unsigned char daTrs_c::func_ov063_021163d0() {

    unsigned int id = this->mCachedActorID;
    if(id==0) return 0;
    char *a = (char*)(unsigned int)dActor_c::FindWithID(id);
    if(a==0) return 0;
    return ((daTrsTrapView *)a)->mOccupiedFlag;
}


void daTrs_c::func_ov063_0211640c() {

    struct Vector3 pos, t1, t2;
    s16 ang;

    if (!this->mBooFlags.visible)
        return;

    pos.x = this->mPosX;
    pos.y = this->mPosY;
    pos.z = this->mPosZ;
    ang = this->mAngleY;

    if (this->mBooFlags.mirrored) {
        s16 neg = -1;
        pos.x *= (int)neg;
        ang *= (int)neg;
    }

    if (this->mState == 3) {
        Vec3_Asr(&t1, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        this->mAnmModel.mat4x3 = data_020a0e68;
        this->mAnmModel.ApplyOpacity((u8)((int)this->mOpacity >> 3), 1);
        func_020167a4(&this->mAnmModel);
        {
            TrsBone *m = (TrsBone *)this->mAnmModel.data.bones;
            m->rotX = this->mAngleX;
            m->rotY = (s16)(ang - 0x4000);
            m->rotZ = this->mAngleZ;
        }
        this->mAnmModel.data.UpdateVertsUsingBones();
    } else {
        Vec3_Asr(&t2, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
        this->mAnmModel.mat4x3 = data_020a0e68;
        this->mAnmModel.ApplyOpacity((u8)((int)this->mOpacity >> 3), 1);
    }

    if (this->mOpacity >= 0x10) {
        Matrix4x3_FromTranslation(&data_020a0e68,
            pos.x >> 3, pos.y >> 3, pos.z >> 3);
        *(struct Matrix4x3 *)&this->mShadowMtx[0] = data_020a0e68;
        DropShadowRadHeight(this->mShadowModel1, *(Matrix4x3 *)&this->mShadowMtx[0], 0x12c000,
                            0xc8000, 0xf);

        if (this->mBooFlags.mirrored) {
            s16 neg = -1;
            pos.x *= (int)neg;
            Matrix4x3_FromTranslation(&data_020a0e68,
                pos.x >> 3, pos.y >> 3, pos.z >> 3);
            this->mShadowMtx2 = data_020a0e68;
            DropShadowRadHeight(this->mShadowModel2, this->mShadowMtx2, 0x12c000, 0xc8000, 0xf);
        }
    }

    func_ov063_021160d4();
}


void daTrs_c::func_ov063_021166ac() {

    daTrs_c *t = (daTrs_c *)this;
    Vector3_16 rot;
    Vector3 t1;
    Vector3 t2;
    Vector3 pos;

    if (t->mVariant == 0xf) {
        func_ov063_0211640c();
        return;
    }
    if (!t->mBooFlags.visible)
        return;
    if (t->mBooFlags.carrying) {
        if (this->mCarriedID != TRS_ID_T_BASKET) {
            s16 *ap = (s16 *)&this->mCarriedAngleY;
            *ap += 0xc00;
        }
        Matrix4x3_FromRotationY((Matrix4x3 *)&this->mCarriedModel.mat4x3, this->mCarriedAngleY);
        this->mCarriedModel.mat4x3.m[9] = this->mCarriedPosX >> 3;
        this->mCarriedModel.mat4x3.m[10] = this->mCarriedPosY >> 3;
        this->mCarriedModel.mat4x3.m[11] = this->mCarriedPosZ >> 3;
    }

    if (t->mState == 3 || t->mState == 3 ||
        t->mState == 3 || t->mState == 3) {
        Vec3_Asr(&t1, &this->mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        this->mAnmModel.mat4x3 = data_020a0e68;
        this->mAnmModel.ApplyOpacity((u8)(this->mOpacity >> 3), 1);
        func_020167a4(&this->mAnmModel);
        {
            TrsBone *p = (TrsBone *)this->mAnmModel.data.bones;
            p->rotX = this->mAngleX;
            p->rotY = this->mAngleY - 0x4000;
            p->rotZ = this->mAngleZ;
        }
        this->mAnmModel.data.UpdateVertsUsingBones();
    } else {
        Vec3_Asr(&t2, &this->mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, this->mAngleY);
        this->mAnmModel.mat4x3 = data_020a0e68;
        this->mAnmModel.ApplyOpacity((u8)(this->mOpacity >> 3), 1);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, this->mPosX >> 3, this->mPosY >> 3, this->mPosZ >> 3);
    *(Matrix4x3 *)&this->mShadowMtx[0] = data_020a0e68;
    {
        int big = (this->actorID == TRS_ID_BOSS_TERESA);
        if (big)
            DropShadowRadHeight(this->mShadowModel1, *(Matrix4x3 *)&this->mShadowMtx[0], 0x12c000,
                                0xc8000, 0xf);
        else
            DropShadowRadHeight(this->mShadowModel1, *(Matrix4x3 *)&this->mShadowMtx[0], 0x64000,
                                0xc8000, 0xf);
    }
    this->mCapPosY = this->mClsnOff.y + FX_MUL(this->mScaleX, 0x60000);
    pos.x = this->mCapPosX;
    pos.y = this->mCapPosY;
    pos.z = this->mCapPosZ;
    {
        /* The angle triple copies as two halfword loads, then two stores, then
           the third pair: two named u16 temps for x and y reproduce that; a
           Vector3_16 struct copy or three direct member copies alternate
           load/store (and the s16-typed copy sign-extends, ldrsh). */
        u16 ax = *(u16 *)&this->mAngleX;
        u16 ay = *(u16 *)&this->mAngleY;
        rot.y = ay;
        rot.x = ax;
        rot.z = *(u16 *)&this->mAngleZ;
    }
    /* equal-arm ternary: the rot address (r2) is set up before pos (r1), as in the ROM */
    UpdateCapPos(pos, this ? rot : rot);
    func_ov063_021160d4();
}


void daTBasket_c::func_ov063_021169c4() {

    Matrix4x3_FromTranslation(&this->mModel.mat4x3, this->mPosX>>3, this->mPosY>>3, this->mPosZ>>3);
    DropShadowRadHeight(this->mShadowModel, this->mModel.mat4x3, 0x64000, 0x64000, 0xf);
}


/* Behavior handler for variant 4 (needs 15 stars): the scripted-path Boo.
   Hidden until canAppear, then waits for the player within 0x3e8000, flies
   a fixed path and fades. */
void daTrs_c::func_ov063_02116a1c() {
    Vector3 v;
    int a;
    int scale;

    v.x = this->mHomePos.x;
    v.y = this->mHomePos.y;
    v.z = this->mHomePos.z;
    this->mClsnScale = 0x2000;

    a = this->mState;
    if (a == 0) {
        this->mBooFlags.visible = 0;
        if (NumStars() < 0xf) {
            this->MarkForDestruction();
            return;
        }
        if (this->mBooFlags.canAppear) {
            unsigned short *fp = (unsigned short *)&this->mBooFlags;
            unsigned char *st = (unsigned char *)&this->mState;
            *fp |= TRS_FLAG_VISIBLE;
            this->mOpacity = 0xb4;
            scale = this->mClsnScale;
            this->mScaleX = scale;
            this->mScaleY = scale;
            this->mScaleZ = scale;
            this->mdCcAcPos_c.radius = this->mClsnRadius * this->mClsnScale;
            this->mdCcAcPos_c.height = this->mClsnHeight * this->mClsnScale;
            *st += 1;
        }
    } else if (a == 1) {
        if (this->mDistToPlayer < 0x3e8000) {
            unsigned char *st = (unsigned char *)&this->mState;
            *st += 1;
            func_0201267c(0xf8, &this->mCamSpacePos);
        }
        this->mHorzSpeed = 0;
    } else {
        int t;
        ApproachLinear(*(int *)&this->mHorzSpeed, 0x30000, 0x1800);
        t = 0x3e8000;
        v.x = -t;
        v.z = (int)0xfdcd8000;
        if (this->mPosZ < (int)0xfec78000) {
            /* invert: laundered RMW as THEN, plain zero as ELSE
               -> movls/strbls + bls + unpredicated RMW */
            if (this->mOpacity > 0x14) {
                unsigned char *p = (unsigned char *)&this->mOpacity;
                *p = (unsigned char)(*p - 0x14);
            } else {
                this->mOpacity = 0;
            }
        }
    }

    this->mVertSpeed = 0;
    ApproachLinear(this->mPrevAngleY, Vec3_HorzAngle(&this->mPos, &v), 0x5a8);
    Oscillate(1);
}


void daTrs_c::func_ov063_02116bf0() {

}


/* Behavior handler for variant 3: spawns TERESA children (param 0xfff1)
   while fewer than two are alive, five at most. */
void daTrs_c::func_ov063_02116bf4() {

        unsigned short *ip = (unsigned short *)&this->mBooFlags;
        int *r3 = (int *)&this->mdCcAcPos_c.flags;
        unsigned char st;
    
        *ip = (unsigned short)(*ip & ~TRS_FLAG_VISIBLE);
        *r3 = *r3 | 1;
        func_ov063_02119c18(TRS_ID_MERRYGOROUND);
    
        st = this->mState;
        switch (st) {
        case 0:
            if (this->mDistToPlayer >= 0x3e8000)
                return;
            if (this->mChildDeaths < 5) {
                unsigned char cb = this->mSpawnCount;
                if (cb != 5 && (int)cb - this->mChildDeaths < 2) {
                    char *p = (char *)dActor_c::Spawn(TRS_ID_TERESA, 0xfff1, this->mPos,
                                                      &this->mPrevAngle, this->mAreaIdx, -1);
                    if (p != 0) {
                        ((daTrs_c *)p)->mSpawnerID = this->uniqueID;
                        ((daTrs_c *)p)->mCachedActorID = this->mCachedActorID;
                    }
                    {
                        unsigned char *q = (unsigned char *)&this->mSpawnCount;
                        *q = *q + 1;
                    }
                }
                {
                    unsigned char *q = (unsigned char *)&this->mState;
                    *q = *q + 1;
                }
            }
            if (this->mChildDeaths >= 5)
                this->mState = 2;
            break;
        case 1:
            if (*(u16 *)&this->mStateTimer > 0x3c)
                this->mState = 0;
            break;
        case 2:
            break;
        }
}


/* Behavior handler for variant 5: the Boo carrying the cage (T_BASKET). */
void daTrs_c::func_ov063_02116d38() {

    switch(this->mState){
        case 0: func_ov063_02116f48(); break;
        case 1: func_ov063_02116e14(); break;
        case 2: func_ov063_02116df0(); break;
        case 3: func_ov063_02116dbc(); break;
        case 4: func_ov063_02116d98(); break;
    }
    ApproachTargetOpacityAndUpdateScale();
}


void daTrs_c::func_ov063_02116d98() {
    if (UpdateDuringNonlethalHit(0x28)) {
        this->mState = 1;
    }
}


void daTrs_c::func_ov063_02116dbc() {

    if (UpdateDuringDeath() == 0)
        return;
    this->MarkForDestruction();
    func_0201267c(0xd5, &this->mCamSpacePos);
}


void daTrs_c::func_ov063_02116df0() {
    if (UpdateAfterBouncedOn(0x14)) {
        this->mState = 1;
    }
}


void daTrs_c::func_ov063_02116e14() {

    this->mBooFlags.hitByParam3Player = 0;
    if (this->mChaseDelay == 0) {
        ChaseMario(0x64, 0x200, 0x800);
    }
    {
        int r5 = GetAttackStatus();
        if (ShouldBeStopped() != 0) {
            this->mState = 0;
        }
        if (r5 == -1) {
            this->mState = 2;
            return;
        }
        if (r5 != 1) return;
    }
    if (this->mBooFlags.hitByParam3Player) {
        this->mState = 4;
        func_0201267c(0x14a, &this->mCamSpacePos);
        return;
    }
    this->mState = 3;
    {
        char* r = (char*)dActor_c::Spawn(this->mCarriedID, 0, *(const Vector3 *)((struct Vector3*)&this->mCarriedPosX), (const Vector3_16 *)(0), this->mAreaIdx, -1);
        if (r != 0) {
            ((dActor_c *)r)->mSpeedX = 0;
            ((dActor_c *)r)->mVertSpeed = 0x32000;
            ((dActor_c *)r)->mSpeedZ = 0;
        }
    }
    this->mBooFlags.carrying = 0;
}


int daTrs_c::func_ov063_02116f48() {

    this->mBigBooID = 0;
    this->mTargetOpacity = 0xff;
    this->mClsnScale = 0x2000;
    this->mScaleX = 0x2000;
    this->mScaleY = 0x2000;
    this->mScaleZ = 0x2000;
    this->mdCcAcPos_c.radius = this->mClsnRadius * this->mScaleX;
    this->mdCcAcPos_c.height = this->mClsnHeight * this->mScaleX;
    int r = ShouldBeActive();
    if (r) { this->mState = 1; r = 1; }
    return r;
}


/* L16(this, 0x5d4) is &mBooFlags and L8(this, 0x5cc) is &mState, spelled as
   integer arithmetic. Either member spelling keeps the bytes but lowers
   every @NNN in the TU by two (measured, each one alone). */
#define L16(c, off) ((u16*)((int)(c) + (off)))
#define L8(c, off) ((u8*)((int)(c) + (off)))
/* Behavior handler for variant 0xb: the scripted-path Boo. Hidden until
   canAppear, then waits for the player within 0x258000, flies a fixed path
   (hopping in state 4), then vanishes (opacity 0, hidden). */
void daTrs_c::func_ov063_02116fac() {

    struct Frame fr;
    s16 r4 = 0xc00;
    u16* flags;
    u8* state;

    fr.v.x = this->mHomePos.x;
    fr.v.y = this->mHomePos.y;
    fr.v.z = this->mHomePos.z;
    this->mClsnScale = 0x1000;

    switch (this->mState) {
    case 0:
        flags = L16(this, 0x5d4);
        *flags &= ~TRS_FLAG_VISIBLE;
        if (this->mBooFlags.canAppear) {
            state = L8(this, 0x5cc);
            (*state)++;
            this->mOpacity = 0xb4;
            *flags |= TRS_FLAG_VISIBLE;
        }
        break;
    case 1:
        if (this->mDistToPlayer < 0x258000) {
            state = L8(this, 0x5cc);
            (*state)++;
            func_0201267c(0xf8, &this->mCamSpacePos);
        }
        this->mHorzSpeed = 0;
        break;
    case 2:
        ApproachLinear(*(int *)&this->mHorzSpeed, 0x3a000, 0x1800);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = 0xfea84000;
        if (this->mPosZ < (int)0xfee08000) {
            state = L8(this, 0x5cc);
            (*state)++;
        }
        break;
    case 3:
        ApproachLinear(*(int *)&this->mHorzSpeed, 0, 0x9000);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = -0xfa0000;
        this->mTargetAngleY = Vec3_HorzAngle(&this->mPos, (Vector3 *)&fr.v);
        r4 = 0x1000;
        if (this->mAngleY == this->mTargetAngleY && this->mHorzSpeed == 0) {
            state = L8(this, 0x5cc);
            (*state)++;
        }
        break;
    case 4:
        if (*(u16 *)&this->mStateTimer == 6) {
            this->mVertSpeed = 0xf000;
            this->mVertAccel = -0x4000;
            this->mTerminalVelocity = -0xf000;
        }
        if (this->mPosY < this->mHomePos.y) {
            this->mPosY = this->mHomePos.y;
            state = L8(this, 0x5cc);
            (*state)++;
            this->mVertAccel = 0;
            this->mTerminalVelocity = 0;
        }
        break;
    case 5:
        ApproachLinear(*(int *)&this->mHorzSpeed, 0x3a000, 0x1800);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = 0xfea84000;
        r4 = 0x1000;
        if (this->mPosZ < (int)0xfec78000) {
            state = L8(this, 0x5cc);
            (*state)++;
        }
        break;
    case 6:
        this->mHorzSpeed = 0;
        this->mOpacity = 0;
        flags = L16(this, 0x5d4);
        *flags &= ~TRS_FLAG_VISIBLE;
        break;
    }

    if (this->mState != 4) {
        this->mVertSpeed = 0;
        this->mTargetAngleY = Vec3_HorzAngle(&this->mPos, (Vector3 *)&fr.v);
    }
    ApproachLinear(this->mPrevAngleY, this->mTargetAngleY, r4);
    Oscillate(1);
}

#undef L16
#undef L8

/* Behavior handler for variant 0xf: the Big Boo that leaves the OBJ_KEY. */
void daTrs_c::func_ov063_021172a8() {
    switch (this->mState) {
    case 0: func_ov063_02118914(); break;
    case 1: func_ov063_0211873c(); break;
    case 2: func_ov063_02118f50(); break;
    case 3: func_ov063_021177b0(); break;
    case 4: func_ov063_02118458(); break;
    case 5: func_ov063_0211776c(); break;
    case 6: func_ov063_02117364(); break;
    case 7: func_ov063_02117cdc(); break;
    case 8: func_ov063_02117b0c(); break;
    }
    {
        int state = this->mState;
        if (state == 7) return;
        if (state == 6) {
            if (this->mRelocateTimer > 0x5a) return;
        }
        ApproachTargetOpacityAndUpdateScale();
    }
}


void daTrs_c::func_ov063_02117364() {

    Vector3 pos = this->mPos;

    if (this->mRelocateTimer != 0)
        (this->mRelocateTimer)--;

    if (this->mTargetOpacity == 0) {
        if (this->mOpacity == 0) {
            if (this->mRelocateTimer != 0)
                return;
            {
                FlagW* fw = (FlagW*)&this->mBooFlags;
                fw->mirrored ^= 1;
            }
            func_ov063_02117650();
            this->mTargetOpacity = 0xff;
            this->mParticle1 = 0;
            this->mParticle0 = this->mParticle1;
            this->mRelocateTimer = 0x78;
            func_02012694(0x154, &this->mCamSpacePos);
            return;
        }
        pos.y += this->mClsnScale * 0xaf;
        if (this->mBooFlags.mirrored)
            pos.x *= -1;
        this->mParticle0 = Particle::System::New(this->mParticle0, 0x95, pos.x, pos.y, pos.z, 0, 0);
        this->mParticle1 = Particle::System::New(this->mParticle1, 0x96, pos.x, pos.y, pos.z, 0, 0);
        return;
    }

    if (this->mRelocateTimer > 0x1c) {
        pos.y += this->mClsnScale * 0xaf;
        if (this->mBooFlags.mirrored)
            pos.x *= -1;
        this->mParticle0 = Particle::System::New(this->mParticle0, 0x97, pos.x, pos.y, pos.z, 0, 0);
        this->mParticle1 = Particle::System::New(this->mParticle1, 0x98, pos.x, pos.y, pos.z, 0, 0);
    }
    if (this->mOpacity != 0xff)
        return;
    if (this->mRelocateTimer > 0x1c)
        this->mRelocateTimer = 0x1c;
    if (this->mRelocateTimer != 0)
        return;
    this->mState = 1;
    this->mdCcAcPos_c.flags &= ~1u;
    this->mAttackTimer = ((u32)RandomIntInternal(&data_0209e650.value) >> 16 & 0x3f) + 0x3c;
    this->mNextRelocateTimer = ((u32)RandomIntInternal(&data_0209e650.value) >> 16) % 150 + 0x12c;
    this->mParticle1 = 0;
    this->mParticle0 = this->mParticle1;
}

#undef New

void daTrs_c::func_ov063_02117650() {

    struct Vector3 ppos;
    struct Vector3 npos;
    char *p;
    int neg1 = (int)(-1LL);

    p = (char *)ClosestPlayer();
    if (p == 0) {
        return;
    }

    {
        int *pp = (int *)&((daPly_c *)p)->mPosX;
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
    }
    npos.y = this->mPosY;

    do {
        npos.x =
            ((int)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 0x578) * neg1 -
             0x190)
            << 0xc;
        npos.z =
            ((int)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 0xa28) - 0x514)
            << 0xc;
    } while (Vec3_HorzDist(&ppos, &npos) < 0x320000);

    this->mPosX = npos.x;
    this->mPosY = npos.y;
    this->mPosZ = npos.z;
    this->mAngleY = Vec3_HorzAngle(&this->mPos, &ppos);
    this->mPrevAngleY = this->mAngleY;
}


void daTrs_c::func_ov063_0211776c() {

    Stop();
    unsigned short v = *(u16 *)&this->mStateTimer;
    if (v > 0xf)
        return;
    if (v != 0xf)
        return;
    func_ov063_02118ddc();
    this->MarkForDestruction();
}


void daTrs_c::func_ov063_021177b0() {

    if (*(u16 *)&this->mStateTimer == 0) {
        unsigned char* p = (unsigned char*)&this->mHitPoints;
        *p = (unsigned char)(*p - 1);
    }

    if (this->mHitPoints == 0) {
        dCamera_c *cam = gCamera;
        unsigned short flags = *(unsigned short*)&this->mBooFlags;

        if (((unsigned)(flags << 21)) >> 31) {
            daPly_c *pl = ClosestPlayer();
            if (pl != 0) {
                Vector3 v;
                VecFx32 mid;
                VecFx32 look;
                int dist;
                int scaled;
                short ang;
                short sn;
                int t;

                {
                    int* src = &pl->mPosX;
                    v.x = src[0];
                    v.y = src[1];
                    v.z = src[2];
                }
                dist = Vec3_Dist(&this->mPos, &v);
                if (this->mBooFlags.mirrored) {
                    mid.x = (v.x - this->mPosX) / 2;
                } else {
                    mid.x = (this->mPosX + v.x) / 2;
                }
                mid.y = (this->mPosY + v.y) / 2;
                mid.z = (this->mPosZ + v.z) / 2;
                scaled = (int)((((long long)dist << 12) + 0x800) >> 12);
                look.x = mid.x;
                look.y = mid.y;
                look.z = mid.z;
                look.x = mid.x - scaled;

                if (look.x <= (int)0xFF894000) {
                    ang = Vec3_HorzAngle(&v, &this->mPos);
                    sn = FX_CosIdx((unsigned short)ang >> 4);
                    t = (int)0xFF894000 - look.x;
                    mid.z = mid.z + FX_MUL(t, sn);
                    look.x = (int)0xFF894000;
                }

                func_020092c4(cam, &cam->pos, &look);
                func_020092c4(cam, &cam->lookAt, &mid);
            }
        } else {
            this->mBooFlags.camHeld = 1;
            cam->SetFlag_3();
        }

        if (UpdateDuringDeath() == 0)
            return;

        {
            unsigned short* pf = (unsigned short*)&this->mBooFlags;
            *pf = (unsigned short)(*pf & ~TRS_FLAG_VISIBLE);
        }
        this->mState = 8;
        Stop();
        this->mPrevAngleX = 0;
        this->mPrevAngleY = 0;
        this->mPrevAngleZ = 0;
        this->mSubState = 0;

        {
            Vector3 pos = this->mPos;
            pos.y += 0xc8000;
            if (this->mBooFlags.mirrored)
                pos.x *= -1;
            Particle::System::NewSimple(kPtclBooDefeatA, pos.x, pos.y, pos.z);
            Particle::System::NewSimple(kPtclBooDefeatB, pos.x, pos.y, pos.z);
        }
        return;
    }

    if (*(u16 *)&this->mStateTimer == 0) {
        int* p584 = (int*)&this->mClsnScale;
        int dec = 0x255;
        *p584 = *p584 - dec;
    }
    if (UpdateDuringNonlethalHit(0x28) == 0)
        return;
    this->mState = 6;
    {
        int* p19c = (int*)&this->mdCcAcPos_c.flags;
        *p19c |= 1;
    }
    this->mTargetOpacity = 0;
    Stop();
    this->mRelocateTimer = 0x78;
    func_02012694(0x153, &this->mCamSpacePos);
}


void daTrs_c::func_ov063_02117b0c() {

    int v[3];

    switch (this->mSubState) {
    case 0:
        this->mTalkPlayer = (daPly_c *)ClosestPlayer();
        if (!this->mTalkPlayer->StartTalk(*(fBase_c *)(this), 1))
            return;
        {
            u8 *st = (u8 *)&this->mSubState;
            *st = *st + 1;
        }
        Sound::ChangeMusicVolume(0x14, 0x15666);
        break;
    case 1:
        {
            u8 *st = (u8 *)&this->mSubState;
            *st = *st + 1;
        }
        break;
    case 2:
        if (this->mTalkPlayer->GetTalkState() != 0)
            return;
        {
            int y = this->mPosY;
            int x = this->mPosX;
            int z = this->mPosZ;
            int ny = y + 0xc8000;
            int nx = -x;
            v[0] = nx;
            v[1] = ny;
            v[2] = z;
        }
        {
            void *pl = (void *)this->mTalkPlayer;
            unsigned m = (unsigned)(int)data_ov063_0211e1c8[((daPly_c *)pl)->param1];
            if (!this->mClosestPlayer->ShowMessage(*(fBase_c *)(this), m, (const Vector3 *)(v), 0, 2))
                return;
            func_0201277c(0x151);
            {
                u8 *st = (u8 *)&this->mSubState;
                *st = *st + 1;
            }
        }
        break;
    case 3:
        if (this->mTalkPlayer->GetTalkState() != -1)
            return;
        this->mState = 5;
        Sound::StopLoadedMusic_Layer3();
        Sound::ChangeMusicVolume(0x7f, 0x7222);
        func_02011cfc();
        {
            u16 *fl = (u16 *)&this->mBooFlags;
            *fl = (u16)(*fl & ~TRS_FLAG_CAM_HELD);
        }
        {
            gCamera->mFlags &= ~8;
        }
        break;
    }
}


void daTrs_c::func_ov063_02117cdc() {

    s16 v[3];
    int w[3];
    dCamera_c *cam;
    int neg;

    cam = gCamera;
    switch (this->mSubState) {
    case 0:
        this->mTalkPlayer = (daPly_c *)ClosestPlayer();
        if (this->mTalkPlayer->mPosX <= -0x2bc000) {
            return;
        }
        this->mSubState += 1;
        this->mStateTimer = 0;
        return;
    case 1: {
        u8 *st;
        int n;
        if (*(u16 *)&this->mStateTimer < 0x96) {
            return;
        }
        st = (u8 *)&this->mSubState;
        n = *st + 1;
        *st = n;
        return;
    }
    case 2: {
        VecFx32 *src;
        int *d54c;
        int *d554;
        if (this->mTalkPlayer == 0) {
            return;
        }
        src = (VecFx32 *)&this->mTalkPlayer->mPosX;
        w[0] = src->x;
        w[1] = src->y;
        w[2] = src->z;
        if (VEC_Mag((const Vector3 *)w) >= 0x12c000) {
            return;
        }
        if (this->mTalkPlayer->StartTalk(*(fBase_c *)(this), 1) == 0) {
            return;
        }
        cam->SetFlag_3();
        this->mTalkCamPosX = w[0];
        this->mTalkCamPosY = w[1];
        this->mTalkCamPosZ = w[2];
        d54c = (int *)&this->mTalkCamPosX;
        d554 = (int *)&this->mTalkCamPosZ;
        *d54c = *d54c - (0x3c000 - (w[0] / 8));
        this->mTalkCamPosY = 0x64000;
        *d554 = *d554 - (0x64000 - (w[0] / 6));
        this->mTalkCamLookX = w[0];
        this->mTalkCamLookY = w[1];
        this->mTalkCamLookZ = w[2];
        this->mTalkCamLookX = 0;
        this->mTalkCamLookY = 0x64000;
        {
            u8 *st = (u8 *)&this->mSubState;
            int n = *st + 1;
            *st = n;
        }
        return;
    }
    case 3: {
        s16 ang;
        int b;
        int *src;
        s16 *q;
        src = (int *)&this->mTalkPlayer->mPosX;
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = 0;
        ang = Vec3_HorzAngle(&this->mTalkPlayer->mPos, (Vector3 *)w);
        q = (s16 *)&this->mTalkPlayer->mAngleX;
        v[0] = q[0];
        v[1] = q[1];
        v[2] = q[2];
        b = func_020092c4(cam, &cam->pos, &this->mTalkCamPosX);
        b = b & func_020092c4(cam, &cam->lookAt, &this->mTalkCamLookX);
        if (ApproachLinear(v[1], ang, 0x200) != 0 && b != 0) {
            this->mSubState += 1;
            this->mStateTimer = 0;
        }
        {
            daPly_c *d = this->mTalkPlayer;
            d->mAngleX = v[0];
            d->mAngleY = v[1];
            d->mAngleZ = v[2];
        }
        {
            daPly_c *d = this->mTalkPlayer;
            d->mPrevAngleX = v[0];
            d->mPrevAngleY = v[1];
            d->mPrevAngleZ = v[2];
        }
        return;
    }
    case 4:
        if (*(u16 *)&this->mStateTimer < 0x1e) {
            return;
        }
        func_ov055_02111264();
        {
            u8 *st = (u8 *)&this->mSubState;
            int n = *st + 1;
            *st = n;
        }
        return;
    case 5: {
        int *src;
        int *d560;
        u8 *st;
        int v0;
        if (data_ov008_02111b6c == 0x1f000 || (data_0209caa0.file.flags1 & 0x10)) {
            v0 = 1;
        } else {
            v0 = 0;
        }
        if (v0 == 0) {
            if ((data_0209caa0.file.flags1 & 0x10) == 0) {
                return;
            }
        }
        src = (int *)&this->mTalkPlayer->mPosX;
        d560 = (int *)&this->mTalkCamLookZ;
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        *d560 = *d560 + (0x50000 - (w[0] / 3));
        this->mPosX = w[0] - 0xc8000;
        this->mPosZ = (this->mTalkCamLookZ + 0x12c000) - ((w[0] * 2) / 3);
        this->mTalkCamPosX = w[0] - 0x82000;
        this->mTalkCamPosZ = w[2] - 0x32000;
        this->mStateTimer = 0;
        {
            u8 *st2 = (u8 *)&this->mSubState;
            int n = *st2 + 1;
            *st2 = n;
        }
        return;
    }
    case 6:
        if (*(u16 *)&this->mStateTimer < 0x1e) {
            return;
        }
        {
            u8 *st = (u8 *)&this->mSubState;
            int n = *st + 1;
            *st = n;
        }
        return;
    case 7: {
        int *src;
        int t0;
        t0 = func_020092c4(cam, &cam->pos, &this->mTalkCamPosX);
        if ((t0 & func_020092c4(cam, &cam->lookAt, &this->mTalkCamLookX)) == 0) {
            return;
        }
        this->mOpacity = 0;
        this->mSubState += 1;
        neg = -1;
        src = (int *)&this->mTalkPlayer->mPosX;
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = w[0] * neg;
        this->mAngleY = Vec3_HorzAngle(&this->mPos, (Vector3 *)w);
        this->mPrevAngleY = this->mAngleY;
        func_0201277c(0x150);
        return;
    }
    case 8:
        /* invert so ELSE (=0xff) is predicated and THEN (RMW) is branched (codegen 6c) */
        if (this->mOpacity + 5 < 0xff) {
            u8 *p = (u8 *)&this->mOpacity;
            *p = (u8)(*p + 5);
        } else {
            this->mOpacity = 0xff;
        }
        w[0] = this->mPosX;
        w[1] = this->mPosY;
        w[2] = this->mPosZ;
        neg = -1;
        w[0] = w[0] * neg;
        w[1] = w[1] + 0xc8000;
        func_ov002_020c51d0(this->mTalkPlayer, w);
        if (this->mOpacity == 0xff) {
            this->mSubState += 1;
            this->mTargetOpacity = 0xff;
        }
        this->mAngleY = this->mPrevAngleY;
        return;
    case 9: {
        int tk = this->mTalkPlayer->GetTalkState();
        s16 msg;
        if (tk != 0) {
            return;
        }

        {
            int x = 0 - this->mPosX;
            int z = this->mPosZ;
            int y = this->mPosY + 0xc8000;
            int fl = data_0209caa0.file.flags1 & 0x10;
            w[0] = x;
            w[1] = y;
            w[2] = z;
            if (fl)
                msg = data_ov063_0211e1e4[this->mTalkPlayer->param1];
            else
                msg = data_ov063_0211e1dc[this->mTalkPlayer->param1];
        }
        if (this->mClosestPlayer->ShowMessage(*(fBase_c *)(this), (u32)msg, (const Vector3 *)(w), 0, 2) == 0) {
            return;
        }
        Sound::LoadAndSetMusic_Layer3(0x2c);
        {
            u8 *st = (u8 *)&this->mSubState;
            int n = *st + 1;
            *st = n;
        }
        return;
    }
    case 10: {
        int tk = this->mTalkPlayer->GetTalkState();
        u32 rr;
        if (tk != -1) {
            return;
        }
        this->mState = 1;
        this->mdCcAcPos_c.flags &= ~1;
        this->mAttackTimer = (((u32)RandomIntInternal(&data_0209e650.value) >> 0x10) & 0x3f) + 0xb4;
        cam->mFlags &= ~8;
        rr = RandomIntInternal(&data_0209e650.value);
        this->mNextRelocateTimer = ((rr >> 0x10) % 0x96) + 0x12c;
        Sound::LoadAndSetMusic_Layer3(0x2d);
        func_02011d14();
        return;
    }
    }
    return;
}


void daTrs_c::func_ov063_02118458() {
    int r4 = GetAttackStatus();

    ApproachLinear(this->mPrevAngleY, this->mAngleToPlayer,
        data_ov063_0211e1c0[this->mHitPoints - 1]);

    if (this->mAnmModel.Finished()) {
        if (VanishOrAppear()) {
            if (this->mAnmModel.file == (BCA_File *)data_ov063_0211edcc.filePtr) {
                func_02012694(0x158, &this->mCamSpacePos);
                this->mAnmModel.SetAnim((BCA_File *)data_ov063_0211edd4.filePtr, 0x40000000, 0x1000,
                                        0);
            } else if (this->mAttackRepeats != 0) {
                this->mAttackRepeats -= 1;
                this->mAnmModel.currFrame = 0;
                func_02012694(0x158, &this->mCamSpacePos);
            } else {
                int rnd;
                this->mState = 1;
                this->mdCcAcPos_c.flags &= ~1;
                rnd = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) & 0x3f;
                this->mAttackTimer = rnd + 0xb4;
                this->mAnmModel.SetAnim((BCA_File *)data_ov063_0211ede4.filePtr, 0, 0x1000, 0);
            }
        } else {
            int rnd;
            this->mState = 1;
            this->mdCcAcPos_c.flags &= ~1;
            rnd = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) & 0x3f;
            this->mAttackTimer = rnd + 0xb4;
            this->mAnmModel.SetAnim((BCA_File *)data_ov063_0211ede4.filePtr, 0, 0x1000, 0);
        }
    } else {
        if (this->mAnmModel.file == (BCA_File *)data_ov063_0211edd4.filePtr &&
            this->mAnmModel.WillHitFrame(6)) {
            VecFx32 v;
            s16 m = 0x78;
            v.x = this->mPosX;
            v.y = this->mPosY;
            v.z = this->mPosZ;
            v.x = FX_SinIdx(*(u16 *)&this->mAngleY >> 4) * m + v.x;
            v.z = FX_CosIdx(*(u16 *)&this->mAngleY >> 4) * m + v.z;
            dActor_c *fb;
            this->mAngleX = 0x1000;
            func_02012694(0x156, &this->mCamSpacePos);
            fb = SpawnFireball(*(const Vector3 *)&v, (const Vector3_16 *)&this->mAngleX, 0x14000,
                               0x6999, 4);
            this->mAngleX = 0;
            if (this->mBooFlags.mirrored)
                ((daFPknBallView *)fb)->mMirrored = 1;
            else
                ((daFPknBallView *)fb)->mMirrored = 0;
            ((daFPknBallView *)fb)->mMaxDistance = 0x3e8000;
            func_02012694(0x122, &this->mCamSpacePos);
        }
    }

    if (r4 == -1) {
        this->mState = 2;
        return;
    }
    if (r4 != 1)
        return;
    this->mState = 3;
    func_02012694(0x152, &this->mCamSpacePos);
}

void daTrs_c::func_ov063_0211873c() {

    short a;
    int b;
    int r;
    u8 st = this->mHitPoints;
    if (st == 3) {
        a = 0x180;
        b = 0x999;
    } else if (st == 2) {
        a = 0x240;
        b = 0x1000;
    } else {
        a = 0x380;
        b = 0x1800;
    }
    if (this->mChaseDelay != 0) {
        a = 0;
        b = 0;
    }
    ChaseMario(-100, a, b);
    r = GetAttackStatus();
    if (r == -1) {
        this->mState = 2;
        return;
    }
    if (r == 1) {
        this->mState = 3;
        if (this->mHitPoints == 1)
            func_02012694(0x155, &this->mCamSpacePos);
        else
            func_02012694(0x152, &this->mCamSpacePos);
        return;
    }
    if (this->mNextRelocateTimer == 0) {
        int* p = (int*)&this->mdCcAcPos_c.flags;
        this->mState = 6;
        *p |= 1;
        this->mTargetOpacity = 0;
        this->mHorzSpeed = 0;
        Stop();
        this->mRelocateTimer = 0x78;
        func_02012694(0x153, &this->mCamSpacePos);
        return;
    }
    if (this->mAttackTimer != 0)
        return;
    this->mState = 4;
    this->mVertSpeed = 0;
    this->mVertAccel = 0;
    func_02012694(0x159, &this->mCamSpacePos);
    this->mAnmModel.SetAnim((BCA_File *)data_ov063_0211edcc.filePtr, 0x40000000, 0x1000, 0);
    {
        unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650.value);
        this->mAttackRepeats = (u8)((rnd >> 0x10) % 3);
    }
    this->mHorzSpeed = 0;
}

void daTrs_c::func_ov063_02118914() {

    int zero = 0;
    u8 copied;
    s32 scale;

    this->mState = 7;
    this->mOpacity = zero;
    copied = this->mOpacity;
    this->mTargetOpacity = copied;
    this->mClsnScale = 0x1000;
    this->mHitPoints = 3;

    scale = this->mClsnScale;
    this->mScaleX = scale;
    this->mScaleY = scale;
    this->mScaleZ = scale;
    this->mdCcAcPos_c.radius = this->mClsnRadius * this->mClsnScale;
    this->mdCcAcPos_c.height = this->mClsnHeight * this->mClsnScale;
    this->mBooFlags.visible = 1;
    this->mdCcAcPos_c.flags |= 1;
    this->mBooFlags.mirrored = 1;
    this->mPosX = -this->mHomePos.x;
    this->mAngleY += 0x8000;
    this->mPrevAngleY += 0x8000;
    this->mSubState = zero;
    this->mFlags &= ~2;
}


/* Behavior handler for variants 0xc/0xd/0xe: the Big Boo. */
void daTrs_c::func_ov063_021189f4() {

    if (this->mSoundCount != 0) {
        if (this->mVariant == 0xd) {
            if (this->mSoundCount < 0x4b) {
                Sound::PlaySub(0x20, 0x14, 0x7f, 0x6b000, 1);
                this->mSoundCount += 1;
            } else {
                int r = Sound::PlaySub(0x20, this->mSavedMusicVolume >> 0xc, 0, 0x7f000, 1);
                if (r != 0)
                    this->mSoundCount = 0;
            }
        } else {
            int r = Sound::PlaySecretSound(this, (u16 *)((u16 *)&this->mSoundCount));
            if (r != 0)
                this->mSoundCount = 0;
        }
    }

    switch (this->mState) {
    case 0:
        func_ov063_02119074();
        break;
    case 1:
        func_ov063_02118f74();
        break;
    case 2:
        func_ov063_02118f50();
        break;
    case 3:
        func_ov063_02118cd8();
        break;
    case 4:
        func_ov063_02118b98();
        break;
    case 5:
        func_ov063_02118b2c();
        break;
    }

    ApproachTargetOpacityAndUpdateScale();
}


void daTrs_c::func_ov063_02118b2c() {
    mHorzSpeed = 0x5000;
    int r = GetAttackStatus();
    if (r == -1) {
        mState = 2;
        return;
    }
    if (r == 1) {
        mState = 3;
        func_0201267c(0xc7, &mCamSpacePos);
        return;
    }
    if ((u16)mStateTimer > 0x64)
        mState = 1;
}


void daTrs_c::func_ov063_02118b98() {

    u16 v;

    this->mBooFlags.hitByParam3Player = 0;
    Stop();

    v = *(u16 *)&this->mStateTimer;
    if (v > 0xf) goto bigblock;
    if (v != 0xf) return;

    {
        int v2 = this->mDeathMode;
        if (v2 == 0) {
            func_ov063_02118eac();
        } else if (v2 == 1) {
            func_ov063_02118e5c();
        } else {
            func_ov063_02118ea0();
        }
    }

    this->mPosX = this->mHomePos.x;
    this->mPosY = this->mHomePos.y;
    this->mPosZ = this->mHomePos.z;
    return;

bigblock:
    if (this->mDeathMode != 0) goto special;
    if (v <= 0x3c) return;
    if (this->mDistToPlayer >= 0x258000) return;
    {
        dActor_c *r1 = 0;
        for (;;) {
            r1 = dActor_c::FindWithActorID(TRS_ID_KAIDAN, r1);
            if (r1 == 0) goto after_strb;
            if (((unsigned int)r1->param1 >> 8 & 3) == 0) break;
        }
        ((daTrsTrapView *)r1)->mTriggered = 1;
    }
after_strb:
    this->MarkForDestruction();
    func_0201267c(0xd5, &this->mCamSpacePos);
    return;

special:
    this->MarkForDestruction();
    func_0201267c(0xd5, &this->mCamSpacePos);
}


void daTrs_c::func_ov063_02118cd8() {

    if (*(u16 *)&this->mStateTimer == 0) {
        unsigned int v = *(unsigned short *)&this->mBooFlags;
        if ((v << 25 >> 31) == 0) {
            unsigned char *p = (unsigned char *)&this->mHitPoints;
            (*p)--;
        }
    }
    if (this->mHitPoints == 0) {
        unsigned short *p;
        if (UpdateDuringDeath() == 0)
            return;
        this->mdCcAcPos_c.flags |= 1;
        p = (unsigned short *)&this->mBooFlags;
        *p &= ~TRS_FLAG_VISIBLE;
        this->mState = 4;
        this->mPrevAngleX = 0;
        this->mPrevAngleY = 0;
        this->mPrevAngleZ = 0;
        *p |= TRS_FLAG_DEFEATED;
        return;
    }
    if (*(u16 *)&this->mStateTimer == 0) {
        this->mClsnScale -= 0x255;
    }
    if (UpdateDuringNonlethalHit(0x28) != 0) {
        this->mState = 1;
    }
}


void daTrs_c::func_ov063_02118ddc() {

  struct Vector3 v;
  v.x = this->mPosX;
  int y = this->mPosY;
  v.y = y;
  v.z = this->mPosZ;
  v.y = y + 0x64000;
  int s = this->mPosX;
  if(s < 0) s = -s;
  v.x = s;
  dActor_c::Spawn(TRS_ID_OBJ_KEY, 3, *(const Vector3 *)(&v), (const Vector3_16 *)(0), this->mAreaIdx, -1);
  func_02012694(0xbb, &this->mCamSpacePos);
}


void daTrs_c::func_ov063_02118e5c() {

    func_ov063_02118eac();
    unsigned int id = this->mCachedActorID;
    if (id == 0) return;
    void *actor = (void *)dActor_c::FindWithID(id);
    this->mFoundActor = (dActor_c *)actor;
    void *p = (void *)this->mFoundActor;
    if (p != 0) {
        ((daTrsTrapView *)p)->mState = 1;
    }
    this->mFoundActor = 0;
}


void daTrs_c::func_ov063_02118ea0() {

    func_ov063_02118eac();
}


void daTrs_c::func_ov063_02118eac() {

    Vector3 v;
    void *p;
    unsigned int id;

    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    this->mPosY =
        this->mPosY + 0x64000;
    if (this->mSpawnedActorID == 0) {
        return;
    }
    id = this->mSpawnedActorID;
    p = (void *)dActor_c::FindWithID(id);
    this->mFoundActor = (dActor_c *)p;
    if (this->mFoundActor != 0) {
        func_ov063_02118f24(static_cast<daTrsIcon_c *>(this->mFoundActor), v);
    }
    this->mFoundActor = 0;
}


extern "C" void func_ov063_02118f24(daTrsIcon_c *icon, const Vector3 &pos)
{
    unsigned int val = 4;
    icon->UntrackAndSpawnStar(icon->mTrackStarID, icon->mStarID, pos, val);
}

void daTrs_c::func_ov063_02118f50() {

    if (UpdateAfterBouncedOn(0x14))
        this->mState = 1;
}


void daTrs_c::func_ov063_02118f74() {

    short a2;
    int a3;
    int r5v;
    unsigned char mode;

    this->mBooFlags.hitByParam3Player = 0;
    mode = this->mHitPoints;
    if (mode == 3) {
        a2 = 0x180;
        a3 = 0x999;
    } else if (mode == 2) {
        a2 = 0x240;
        a3 = 0x1000;
    } else {
        a2 = 0x380;
        a3 = 0x1800;
    }
    if (this->mChaseDelay != 0) {
        a2 = 0;
        a3 = 0;
    }
    ChaseMario(-0x64, a2, a3);
    r5v = GetAttackStatus();
    if (this->mVariant == 0xd) {
        if (func_ov063_021163d0() == 0) {
            this->mState = 0;
        }
    } else {
        if (ShouldBeStopped() != 0) {
            this->mState = 0;
        }
    }
    if (r5v == -1) {
        this->mState = 2;
        return;
    }
    if (r5v != 1) {
        return;
    }
    this->mState = 3;
    func_0201267c(0xc7, &this->mCamSpacePos);
}


void daTrs_c::func_ov063_02119074() {

    volatile int dummy[2];
    (void)&dummy;
    /* ROM: beq body on ==0xc; bne after on !=0xf  =>  body when 0xc OR 0xf */
    if (this->mVariant == 0xc ||
        this->mVariant == 0xf) {
        func_ov063_02116bf0();
        this->mBooDefeatCount = 0xa;
    }

    if (this->mVariant == 0xd)
        func_ov063_02119c18(TRS_ID_MERRYGOROUND);

    this->mBigBooID = 0;
    this->mOpacity = 0x28;

    if (ShouldBeActive() != 0 && this->mBooDefeatCount >= 5) {
        char *r;
        s32 scale;

        this->mBooFlags.visible = 1;
        this->mHitPoints = 3;
        this->mClsnScale =
            data_ov063_0211e1d0[this->mVariant - 0xc];
        scale = this->mClsnScale;
        this->mScaleX = scale;
        this->mScaleY = scale;
        this->mScaleZ = scale;

        this->mAnmModel.SetPolygonID(0x16);

        if (this->mVariant == 0xd) {
            this->mSavedMusicVolume = gMusicVolume;

            if (!this->mBooFlags.soundStarted) {
                Sound::PlaySub(0x20, 0x14, 0x7f, 0x6b000, 1);
                this->mSoundCount = 1;
                this->mBooFlags.soundStarted = 1;
            }

            this->mHorzSpeed = 0x800;
            this->mState = 5;
            r = (char *)dActor_c::FindWithActorID(TRS_ID_MERRYGOROUND, 0);

            if (r != 0)
                this->mPrevAngleY = Vec3_HorzAngle(&this->mPos, &((dActor_c *)r)->mPos);
        } else {
            if (this->mVariant == 0xe)
                func_ov063_02116244();
            this->mState = 1;
        }

        this->mTargetOpacity = 0xff;
        this->mdCcAcPos_c.radius =
            this->mClsnRadius * this->mClsnScale;
        this->mdCcAcPos_c.height =
            this->mClsnHeight * this->mClsnScale;
        this->mdCcAcPos_c.flags &= ~1;
    } else {
        this->mBooFlags.visible = 0;
        this->mdCcAcPos_c.flags |= 1;
        Stop();
    }
}


/* Behavior handler for variant 8. */
void daTrs_c::func_ov063_02119274() {

    switch(this->mState){
        case 0: func_ov063_02119b84(); break;
        case 1: func_ov063_02119894(); break;
        case 2:
            UpdateBouncedOn();
            break;
        case 3:
            UpdateDying();
            break;
        case 4:
            UpdateNonlethalHit();
            break;
    }
    ApproachTargetOpacityAndUpdateScale();
}


/* Behavior handler for variants 0/1/2/6/9/10: the chasing Boo. */
void daTrs_c::UpdateBoo() {

    switch (this->mState) {
    case 0:
        UpdateInactive();
        break;
    case 1:
        UpdateChase();
        break;
    case 2:
        UpdateBouncedOn();
        break;
    case 3:
        UpdateDying();
        break;
    case 4:
        UpdateNonlethalHit();
        break;
    case 5:
        func_ov063_0211934c();
        break;
    case 6:
        UpdateEmerge();
        break;
    }
    ApproachTargetOpacityAndUpdateScale();
}

void daTrs_c::func_ov063_0211934c() {

    void *r4;
    u8 st;

    r4 = (void *)this->mTalkPlayer;
    if (r4 == 0 || this->mVariant != 2) {
        KillAndTrackInDeathTable();
        func_0201267c(0xd5, &this->mCamSpacePos);
        if (((this->mCapId) & 0xf) >= 6)
            return;
        
        {
            void *cap;
            this->mPosX = this->mHomePos.x;
            this->mPosY = this->mHomePos.y;
            this->mPosZ = this->mHomePos.z;
            this->mAreaId = this->mAreaIdx;
            this->mPrevAngleX = this->mHomeAngleX;
            this->mPrevAngleY = this->mHomeAngleY;
            this->mPrevAngleZ = this->mHomeAngleZ;
            s16 *src = (s16 *)&this->mPrevAngleX;
            this->mAngleX = src[0];
            this->mAngleY = src[1];
            this->mAngleZ = src[2];
            
            cap = RespawnIfHasCap();
            if (cap == 0)
                return;
            {
                u16 *p = (u16 *)&((daTrs_c *)cap)->mBooFlags;
                *p &= ~TRS_FLAG_CARRYING;
            }
        }
        return;
    }

    st = this->mSubState;
    switch (st) {
    case 0:
        if (((daPly_c *)r4)->StartTalk(*(fBase_c *)(this), 1) == 0)
            return;
        {
            u8 *q = (u8 *)&this->mSubState;
            *q = *q + 1;
        }
        return;

    case 1:
    {
        void *found;
        this->mFoundActor = dActor_c::FindWithID(this->mBigBooID);
        found = (void *)this->mFoundActor;
        if (found != 0) {
            s32 *cnt = &((daTrs_c *)found)->mBooDefeatCount;
            *cnt = *cnt + 1;
        }
        found = (void *)this->mFoundActor;
        if (found != 0 && ((daTrs_c *)found)->mBooDefeatCount == 5) {
            ((daPly_c *)r4)->ShowMessage(*(fBase_c *)(this), 0xb5, &this->mPos, 0, 2);
        } else {
            ((daPly_c *)r4)->ShowMessage(*(fBase_c *)(this), 0xb4, &this->mPos, 0, 2);
        }
        this->mFoundActor = 0;
        {
            u8 *q = (u8 *)&this->mSubState;
            *q = *q + 1;
        }
        func_0201267c(0xf8, &this->mCamSpacePos);
        return;
    }

    case 2:
        if (data_0209d660 != 0)
            return;
        func_0201267c(0xd5, &this->mCamSpacePos);
        if (this->mBigBooID != 0) {
            void *found;
            this->mFoundActor = dActor_c::FindWithID(this->mBigBooID);
            found = (void *)this->mFoundActor;
            if (found != 0 && ((daTrs_c *)found)->mBooDefeatCount == 5) {
                ((daTrs_c *)found)->func_ov063_02116244();
                {
                    u16 *p = (u16 *)&this->mSoundCount;
                    *p = *p + 1;
                }
            }
        }
        {
            u8 *q = (u8 *)&this->mSubState;
            *q = *q + 1;
        }
        return;

    case 3:
        if (this->mSoundCount != 0) {
            if (Sound::PlaySecretSound(this, (u16 *)((u16 *)&this->mSoundCount)) == 0)
                return;
            this->MarkForDestruction();
            if (((this->mCapId) & 0xf) >= 6)
                return;
            
        {
            void *cap2;
            this->mPosX = this->mHomePos.x;
            this->mPosY = this->mHomePos.y;
            this->mPosZ = this->mHomePos.z;
            this->mAreaId = this->mAreaIdx;
            this->mPrevAngleX = this->mHomeAngleX;
            this->mPrevAngleY = this->mHomeAngleY;
            this->mPrevAngleZ = this->mHomeAngleZ;
            s16 *src = (s16 *)&this->mPrevAngleX;
            this->mAngleX = src[0];
            this->mAngleY = src[1];
            this->mAngleZ = src[2];
            
            cap2 = RespawnIfHasCap();
            if (cap2 == 0)
                return;
            {
                u16 *p = (u16 *)&((daTrs_c *)cap2)->mBooFlags;
                *p &= ~TRS_FLAG_CARRYING;
            }
        }
            return;
        }
        this->MarkForDestruction();
        if (((this->mCapId) & 0xf) >= 6)
            return;
        
        {
            void *cap3;
            this->mPosX = this->mHomePos.x;
            this->mPosY = this->mHomePos.y;
            this->mPosZ = this->mHomePos.z;
            this->mAreaId = this->mAreaIdx;
            this->mPrevAngleX = this->mHomeAngleX;
            this->mPrevAngleY = this->mHomeAngleY;
            this->mPrevAngleZ = this->mHomeAngleZ;
            s16 *src = (s16 *)&this->mPrevAngleX;
            this->mAngleX = src[0];
            this->mAngleY = src[1];
            this->mAngleZ = src[2];
            
            cap3 = RespawnIfHasCap();
            if (cap3 != 0) {
                u16 *p = (u16 *)&((daTrs_c *)cap3)->mBooFlags;
                *p &= ~TRS_FLAG_CARRYING;
            }
        }
        return;
    }
}

void daTrs_c::UpdateDying() {

    if (!UpdateDuringDeath())
        return;
    if (this->mDeathMode != 0) {
        KillAndTrackInDeathTable();
        func_0201267c(0xd5, &this->mCamSpacePos);
        if ((this->mCapId & 0xf) >= 6) return;
        this->mPosX = this->mHomePos.x;
        this->mPosY = this->mHomePos.y;
        this->mPosZ = this->mHomePos.z;
        this->mAreaId = this->mAreaIdx;
        this->mPrevAngleX = this->mHomeAngleX;
        this->mPrevAngleY = this->mHomeAngleY;
        this->mPrevAngleZ = this->mHomeAngleZ;
        s16* src = (s16*)&this->mPrevAngleX;
        this->mAngleX = src[0];
        this->mAngleY = src[1];
        this->mAngleZ = src[2];
        char* r = (char*)RespawnIfHasCap();
        if (r == 0) return;
        {
            u16* p = (u16*)&((daTrs_c *)r)->mBooFlags;
            *p &= ~TRS_FLAG_CARRYING;
        }
    } else {
        this->mState = 5;
        this->mSubState = 0;
        {
            int* q = (int*)&this->mdCcAcPos_c.flags;
            *q |= 1;
        }
        {
            u16* p = (u16*)&this->mBooFlags;
            *p &= ~TRS_FLAG_VISIBLE;
        }
    }
}

void daTrs_c::UpdateBouncedOn() {

    if (UpdateAfterBouncedOn(0x14))
        this->mState = 1;
}

void daTrs_c::func_ov063_02119894() {

    this->mBooFlags.hitByParam3Player = 0;
    if (*(u16 *)&this->mStateTimer == 0) {
        unsigned r0 = (unsigned)RandomIntInternal(&data_0209e650.value);
        this->mSpeedJitter = (int)(((r0 >> 16) & 0xfff) * 5);
        r0 = (unsigned)RandomIntInternal(&data_0209e650.value);
        {
            int x = (int)((r0 >> 16) & 0xfff);
            this->mTurnJitter = (int)(((s64)x << 7) + 0x800 >> 12);
        }
    }
    if (this->mChaseDelay == 0) {
        func_ov063_02119cc0(-100,(short)(this->mTurnJitter + 0x180),0xc00);
    }
    HandleAttackStatus();
}


#define M(p) (p)
void daTrs_c::UpdateChase() {

    unsigned short *p = (unsigned short *)M(&this->mBooFlags);
    *p &= ~TRS_FLAG_HIT_BY_PARAM3_PLAYER;
    if (*(u16 *)&this->mStateTimer == 0) {
        int r0 = RandomIntInternal(&data_0209e650.value);
        this->mSpeedJitter = (((unsigned)r0 >> 16) & 0xfff) * 5;
        int ri = RandomIntInternal(&data_0209e650.value);
        int x = 0xfff & ((unsigned)ri >> 16);
        this->mTurnJitter = (int)((((long long)x << 7) + 0x800) >> 12);
    }
    if (this->mChaseDelay == 0) {
        ChaseMario(-100, (short)(this->mTurnJitter + 0x180), 0xfe0);
    }
    HandleAttackStatus();
}

#undef M

void daTrs_c::UpdateNonlethalHit() {

    if (UpdateDuringNonlethalHit(0x28))
        this->mState = 1;
}

void daTrs_c::UpdateEmerge() {

    this->mBooFlags.hitByParam3Player = 0;
    if (*(u16 *)&this->mStateTimer >= 0x1e) {
        this->mState = 1;
    } else {
        this->mVertSpeed = 0;
        this->mHorzSpeed = 0x7ccc;
        Oscillate(0);
    }
    HandleAttackStatus();
}

void daTrs_c::UpdateCarriedItemPos() {

    if (!this->mBooFlags.carrying)
        return;

    this->mCarriedPosX = this->mPosX;
    this->mCarriedPosY = this->mPosY;
    this->mCarriedPosZ = this->mPosZ;

    if (this->mCarriedID == TRS_ID_T_BASKET) {
        this->mCarriedPosY += 0x3c000;
        return;
    }
    this->mCarriedPosY += 0xa000;
}

void daTrs_c::HandleAttackStatus() {

    int r4 = GetAttackStatus();
    if (ShouldBeStopped() != 0)
        this->mState = 0;
    if (r4 == -1) {
        this->mState = 2;
        return;
    }
    if (r4 != 1) return;
    func_ov063_021162c8();
}

void daTrs_c::func_ov063_02119b84() {

    Stop();
    this->mState = 1;
    this->mClsnScale = 0x1000;
    this->mTargetOpacity = 0xff;
}

void daTrs_c::UpdateInactive() {

    if (this->mDeathMode == 2) this->unk_5a8 = 0xa;
    Stop();
    FindParentBigBoo();
    if (ShouldBeActive() != 0) {
        if (this->mDeathMode != 2) this->mState = 6;
        else this->mState = 1;
    }
    this->mClsnScale = 0x1000;
    this->mTargetOpacity = 0xff;
}

void daTrs_c::func_ov063_02119c18(unsigned int id) {

    void* r = (void*)this->mCachedActorID;
    if (r != 0) return;
    void *a = (void *)dActor_c::FindWithActorID(id, 0);
    this->mFoundActor = (dActor_c *)a;
    a = (void *)this->mFoundActor;
    if (a != 0) {
        this->mCachedActorID = ((dActor_c *)a)->uniqueID;
    }
}

void daTrs_c::FindParentBigBoo() {

    unsigned int id;
    dActor_c *a;
    if (this->mBigBooID != 0) return;
    id = TRS_ID_BOSS_TERESA;
    a = 0;
    for (;;) {
        a = dActor_c::FindWithActorID(id, a);
        if (a == 0) return;
        unsigned char t = ((daTrs_c *)a)->mVariant;
        if (t == 0xe) break;
        if (t == 0xd) break;
    }
    this->mBigBooID = a->uniqueID;
}

void daTrs_c::func_ov063_02119cc0(int unused, s16 a2, int a3) {

    if (VanishOrAppear() == 0)
        goto reset;

    if (Vec3_HorzDist(&this->mPos, &this->mHomePos) >= this->mLeashDist) {
        this->mTargetAngleY = Vec3_HorzAngle(&this->mPos, &this->mHomePos);
        this->mStateTimer = 0;
    } else if (Vec3_HorzDist(&this->mHomePos, &this->mClosestPlayer->mPos) < this->mLeashDist) {
        this->mTargetAngleY = this->mAngleToPlayer;
        this->mStateTimer = 0;
    } else {
        unsigned rnd = (unsigned)RandomIntInternal(&data_0209e650.value) >> 16 & 0x3f;
        if (*(u16 *)&this->mStateTimer >= rnd + 0x5a) {
            int rnd2 =
                (int)((unsigned)RandomIntInternal(&data_0209e650.value) >> 16 & 0x3fff) - 0x2000;
            this->mTargetAngleY = this->mPrevAngleY + rnd2;
            this->mStateTimer = 0;
        }
    }

    ApproachLinear(this->mPrevAngleY, this->mTargetAngleY, a2);
    this->mVertSpeed = 0;
    SetVelFromPlayerVel(0xa000 - this->mSpeedJitter, a3);

    if (this->mHorzSpeed != 0) {
        Oscillate(0);
    } else {
        func_ov063_0211a960();
    }
    return;

reset:
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    func_ov063_0211a960();
}

void daTrs_c::ChaseMario(int a1, short a2, int a3) {

    int thresh;
    short angle;

    if (VanishOrAppear() != 0) {
        if (this->mBooFlags.mirrored) {
        } else {
            if (this->mAttackTimer != 0) {
                unsigned short *q = (unsigned short *)&this->mAttackTimer;
                *q = *q - 1;
            }
        }
        if (this->mNextRelocateTimer != 0) {
            unsigned short *q = (unsigned short *)&this->mNextRelocateTimer;
            *q = *q - 1;
        }

        {
            unsigned char st = this->mVariant;
            if (st == 0xf || st == 9)
                thresh = 0x7fffffff;
            else
                thresh = 0x5dc000;
        }

        if (gCurrentSublevel == 3 &&
            Vec3_HorzDist(&this->mHomePos, &this->mClosestPlayer->mPos) > thresh) {
            angle = Vec3_HorzAngle(&this->mPos, &this->mHomePos);
        } else {
            if (Vec3_HorzDist(&this->mPos, &this->mClosestPlayer->mPos) <= thresh)
                angle = this->mAngleToPlayer;
            else
                angle = Vec3_HorzAngle(&this->mPos, &this->mHomePos);
        }

        ApproachLinear(this->mPrevAngleY, angle, a2);
        this->mVertSpeed = 0;

        {
            daPly_c *p = this->mClosestPlayer;
            if (p->mIsAirborne == 0) {
                int myY = this->mPosY;
                int otherY = p->mPosY;
                int dy = myY - otherY;
                if ((a1 << 0xc) < dy && dy < 0x1f4000 && (this->mHomePos.y - myY) < 0xfa000) {
                    this->mVertSpeed = func_ov063_0211a0a8((int)this, myY, otherY, 0xa000, 0x2000);
                }
            }
        }

        SetVelFromPlayerVel(0xa000 - this->mSpeedJitter, a3);
        if (this->mHorzSpeed != 0) {
            Oscillate(0);
            return;
        }
        func_ov063_0211a960();
        return;
    }

    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    func_ov063_0211a960();
}

void daTrs_c::SetVelFromPlayerVel(int a, int b) {

    int h = b >> 1;
    int pv = this->mClosestPlayer->mHorzSpeed;
    int m = FX_MUL(a, h);
    if (pv < m) {
        this->mHorzSpeed = m;
        return;
    }
    this->mHorzSpeed = FX_MUL(pv, h);
}

extern "C" int func_ov063_0211a0a8(int a0, int a1, int a2, int a3, int a4) {
    int diff = a1 - a2;
    if (diff > 0) {
        if (diff < a3) return 0;
        return -a4;
    } else {
        a3 = -a3;
        if (diff > a3) return 0;
        return a4;
    }
}

int daTrs_c::GetAttackStatus() {

    dActor_c *actor;
    daPly_c *player;
    u32 id;
    VecFx32 v1, v2;

    id = this->mdCcAcPos_c.otherOwner;
    if (id == 0)
        goto ret0;

    if (this->mdCcAcPos_c.hitFlags & 0x207e0) {
        void* found;

        this->mdCcAcPos_c.flags |= 1;
        found = (void *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);
        if (found) {
            this->mTalkPlayer = (daPly_c *)found;

            {
                int isBf = (int)(((dActor_c *)found)->actorID == TRS_ID_PLAYER);
                if (isBf) {
                    if (((dActor_c *)found)->param1 == 3) {
                        u16* p = (u16*)&this->mBooFlags;
                        *p |= TRS_FLAG_HIT_BY_PARAM3_PLAYER;
                    }
                }
            }
        }
        return 1;
    }

    actor = dActor_c::FindWithID(id);
    if (!actor)
        goto ret0;
    if (!(this->mdCcAcPos_c.hitFlags & 0x400000))
        goto ret0;
    player = static_cast<daPly_c *>(actor);
    if (player->mIsVanish != 0)
        return 0;

    if (player->mIsMetal != 0) {
        this->mdCcAcPos_c.flags |= 1;
        this->mTalkPlayer = (daPly_c *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);
        return 1;
    }

    if (this->JumpedOnByPlayer(*(dCc_c *)(&this->mdCcAcPos_c), *player) != 0) {
        player->Bounce(0x28000);
        func_0201267c(0x149, &this->mCamSpacePos);
        return -1;
    }

    {
    int isBigBoo = (int)(this->actorID == TRS_ID_BOSS_TERESA);
    if (isBigBoo) {
        if (player->mIsAirborne != 0) {
            player->Bounce(0x28000);
            func_0201267c(0x149, &this->mCamSpacePos);
            return -1;
        }

        v1.x = this->mPosX;
        v1.y = this->mPosY;
        v1.z = this->mPosZ;
        player->Hurt(*(Vector3 *)&v1, this->mHurtDamage, 0xc000, 1, 0, 1);
        func_0201267c(0x149, &this->mCamSpacePos);
        return -1;
    }
    }

    if (this->mdCcAcPos_c.hitFlags & 0x10) {
        u8 kind = this->mVariant;

        if (kind < 2)
            goto found_kind;
        if (kind >= 6)
            goto found_kind;
        if (kind > 9)
            goto no_kind;

found_kind:
        {
            void* found2;
            s16 v[3];

            this->mdCcAcPos_c.flags |= 1;
            found2 = (void *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);
            if (!found2)
                goto ret0;
            this->mTalkPlayer = (daPly_c *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);

            v[0] = -0x2000;
            v[1] = 0;
            v[2] = 0;
            KillByInvincibleChar(*(Vector3_16 *)v, *(daPly_c *)found2, 0);

            {
                s16* ap = (s16*)&this->mAngleY;
                *ap = (s16)(*ap + 0x8000);
            }
            return 1;
        }
    }

no_kind:
    v2.x = this->mPosX;
    v2.y = this->mPosY;
    v2.z = this->mPosZ;
    player->Hurt(*(Vector3 *)&v2, this->mHurtDamage, 0xc000, 1, 0, 1);
    func_0201267c(0x149, &this->mCamSpacePos);
    return -1;

ret0:
    return 0;
}

int daTrs_c::UpdateDuringDeath() {

    VecFx32 v, w;

    if (*(u16 *)&this->mStateTimer == 0) {
        this->mHorzSpeed = 0x28000;
        this->mPrevAngleY = this->mClosestPlayer->mAngleY;
        this->mChildDeaths = 1;
        this->mBooFlags.followHeading = 0;
        goto fall;
    }

    if (*(u16 *)&this->mStateTimer == 5)
        this->mTargetOpacity = 8;

    if (*(u16 *)&this->mStateTimer <= 0x1e) {
        if (this->mWithMeshClsn.IsOnWall() == 0)
            goto fall;
    }

    if (this->mVariant == 0xf) {
        int x = this->mPosX;
        v.x = x;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        if (this->mBooFlags.mirrored)
            v.x = x * (u32)-1;
        ((int*)&w)[0] = ((int*)&v)[0];
        ((int*)&w)[1] = ((int*)&v)[1];
        ((int*)&w)[2] = ((int*)&v)[2];
        this->PoofDustAt(*(const Vector3 *)(&w));
    } else {
        PoofDust();
    }

    this->mChildDeaths = 2;

    if (this->mVariant != 0 && this->mBigBooID != 0) {
        this->mFoundActor = dActor_c::FindWithID(this->mBigBooID);
        if (this->mFoundActor != 0) {
            if (this->mVariant != 2) {
                ((daTrs_c *)this->mFoundActor)->mBooDefeatCount += 1;
            }
            ((daTrs_c *)this->mFoundActor)->func_ov063_02116244();
        }
        this->mFoundActor = 0;
    }
    return 1;

fall:
    this->mVertSpeed = 0x5000;
    this->mAngleZ += 0x800;
    this->mAngleY += 0x800;
    return 0;
}

int daTrs_c::UpdateDuringNonlethalHit(int arg1) {
    Stop();
    if (*(u16 *)&mStateTimer == 0)
        SetMoveYawForDuringHit(1);
    {
        u16 t = *(u16 *)&mStateTimer;
        if (t < 0x20) {
            MoveDuringHit(1, ((arg1 * data_ov063_0211e7e0[t]) << 12) / 5000);
            goto ret0;
        }
        if (t < 0x30) {
            ShakeAfterHit();
            goto ret0;
        }
    }
    mdCcAcPos_c.flags &= ~1;
    ResetAfterHit();
    mState = 1;
    return 1;
ret0:
    return 0;
}

int daTrs_c::UpdateAfterBouncedOn(int arg) {
    unsigned int t;
    int rv;

    Stop();
    if (*(u16 *)&mStateTimer == 0)
        SetMoveYawForDuringHit(0);
    t = *(u16 *)&mStateTimer;
    if (t < 0x20) {
        MoveDuringHit(0, ((arg * data_ov063_0211e7e0[t]) << 12) / 5000);
        rv = 0;
        goto done;
    }
    mdCcAcPos_c.flags &= ~1;
    ResetAfterHit();
    mState = 1;
    return 1;
done:
    return rv;
}

void daTrs_c::ResetAfterHit() {

    this->mPrevAngleY = this->mPreKnockbackAngleY;
    this->mBooFlags.followHeading = 1;
}

void daTrs_c::ShakeAfterHit() {
    mAngle.y += (FX_CosIdx((u16)(s16)((*(u16 *)&mStateTimer - 0x1f) << 13) >> 4) << 10) / 4096;
}

void daTrs_c::MoveDuringHit(int cond, int val) {
    s16 ang = (*(u16 *)&mStateTimer + 1) << 11;

    mHorzSpeed = val;
    mVertSpeed = FX_CosIdx((u16)ang >> 4);
    mPrevAngleY = mKnockbackAngleY;
    if (cond == 0)
        return;

    mAngle.y += data_ov063_0211e7e0[*(u16 *)&mStateTimer];
    mAngle.z += data_ov063_0211e7e0[*(u16 *)&mStateTimer];
}

void daTrs_c::SetMoveYawForDuringHit(int cond) {
    mdCcAcPos_c.flags |= 1;
    mBooFlags.followHeading = 0;
    mPreKnockbackAngleY = mPrevAngleY;
    if (cond != 0) {
        mKnockbackAngleY = mClosestPlayer->mAngleY;
        return;
    }
    s16 base = mPrevAngleY;
    if (FX_CosIdx((u16)(s16)(base - mAngleToPlayer) >> 4) < 0)
        mKnockbackAngleY = base;
    else
        mKnockbackAngleY = base + 0x8000;
}

int daTrs_c::VanishOrAppear() {

    dActor_c *a = (dActor_c*)this;
    int r6 = a->GetSubtraction(this->mAngleToPlayer, this->mPrevAngleY);
    int r0 = a->GetSubtraction(this->mPrevAngleY,
                               this->mClosestPlayer->mAngleY);
    int ret = 0;
    this->mVertSpeed = 0;
    if (r6 > 0x1568 || r0 < 0x6b58) {
        if (this->mOpacity == 0x28) {
            this->mTargetOpacity = 0xff;
            if (this->mVariant != 0xf)
                func_0201267c(0xf8, &this->mCamSpacePos);
            this->mChaseDelay = 0x1e;
        }
        if (this->mOpacity > 0xb4)
            ret = 1;
    } else {
        if (this->mOpacity == 0xff)
            this->mTargetOpacity = 0x28;
    }
    return ret;
}

void daTrs_c::func_ov063_0211a960() {

}

void daTrs_c::Oscillate(int arg1) {

    if (mOpacity != 0xff && arg1 == 0)
        return;
    mScaleX = mClsnScale + (FX_SinIdx(mWobblePhase >> 4) << 3) / 100;
    mScaleY = mClsnScale + -(FX_SinIdx(mWobblePhase >> 4) << 3) / 100;
    mScaleZ = mScaleX;
    func_ov063_0211a960();
    mdCcAcPos_c.radius = mClsnRadius * mScaleX;
    mdCcAcPos_c.height = mClsnHeight * mScaleY;
}

void daTrs_c::ApproachTargetOpacityAndUpdateScale() {

    u8 cur = this->mOpacity;
    u8 tgt = this->mTargetOpacity;
    int v;
    if (tgt != cur) {
        if (tgt > cur) {
            if (cur + 0x14 >= tgt) {
                this->mOpacity = tgt;
            } else {
                u8 *p = (u8*)&this->mOpacity;
                *p += 0x14;
            }
        } else {
            if (cur - 0x14 > tgt) {
                u8 *p = (u8*)&this->mOpacity;
                *p -= 0x14;
            } else {
                this->mOpacity = tgt;
            }
        }
    }
    if (this->mOpacity == 0xff) {
        this->mAnmModel.SetPolygonID(1);
    } else if (this->mBooFlags.mirrored) {
        this->mAnmModel.SetPolygonID(2);
    } else {
        this->mAnmModel.SetPolygonID(0x16);
    }
    v = this->mClsnScale * 8 / 10
        + this->mClsnScale * (this->mOpacity * 2) / 2550;
    this->mScaleX = v;
    this->mScaleY = v;
    this->mScaleZ = v;
    this->mdCcAcPos_c.radius = this->mClsnRadius * v;
    this->mdCcAcPos_c.height = this->mClsnHeight * v;
}

/* Behavior handler for variant 7: spawns three TERESA (param 0xfff6, random
   yaw) at fixed offsets and removes itself; below 15 stars it only removes
   itself. */
void daTrs_c::SpawnCourtyardBooTriplet() {

    VecFx32 pos;
    Vector3 tmp;
    Vector3_16 rot;
    int i;

    static Vector3 sSpawnOff[3] = {
        Vector3(0, 0x32000, 0),
        Vector3(0xd2000, 0x6e000, 0xd2000),
        Vector3(-0xd2000, 0x46000, -0xd2000),
    };

    if (NumStars() < 15) {
        this->MarkForDestruction();
        return;
    }

    for (i = 0; i < 3; i++) {
        Vec3_Add(&tmp, &this->mPos, &sSpawnOff[i]);
        pos.x = tmp.x;
        pos.y = tmp.y;
        pos.z = tmp.z;
        rot.x = this->mPrevAngleX;
        rot.y = this->mPrevAngleY;
        rot.z = this->mPrevAngleZ;
        rot.y = (u32)RandomIntInternal(&data_0209e650.value) >> 16;
        dActor_c::Spawn(TRS_ID_TERESA, 0xfff6, *(const Vector3 *)(&pos), (const Vector3_16 *)(&rot), this->mAreaIdx, -1);
    }
    this->MarkForDestruction();
}

int daTrs_c::ShouldBeActive() {

    int r4;
    int v;
    int b = (int)(this->actorID == TRS_ID_BOSS_TERESA);
    if (b != 0 && this->mVariant != 0xf) {
        r4 = 0x2e6000;
    } else {
        v = this->mVariant;
        if (v == 9) r4 = 0x7fffffff;
        else r4 = 0x5dc000;
    }
    v = this->mVariant;
    if (v == 0xd || v == 1) {
        if (func_ov063_021163d0() != 0) return 1;
        return 0;
    }
    if (ShouldBeStopped() == 0) {
        if (this->mDistToPlayer < r4) return 1;
    }
    return 0;
}

int daTrs_c::ShouldBeStopped() {

    unsigned char v = this->mVariant;
    if (v == 0xd || v == 1) {
        return func_ov063_021163d0() == 0 ? 1 : 0;
    }
    return 0;
}

void daTrs_c::Stop() {

    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    this->mVertAccel = 0;
    this->mChaseDelay = 30;
}

int daTBasket_c::CleanupResources()
{
    data_ov063_0211edec.Release();
    return 1;
}

int daTrs_c::CleanupResources()
{
    int b;
    int *cnt;

    if (mSpawnedActorID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnedActorID);
        if (mFoundActor != 0)
            mFoundActor->MarkForDestruction();
        mFoundActor = 0;
    }
    if (mSpawnerID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnerID);
        if (mFoundActor != 0) {
            cnt = (int *)&((daTrs_c *)mFoundActor)->mChildDeaths;
            (*cnt)++;
        }
        mFoundActor = 0;
    }
    if (mCarriedID == TRS_ID_BLUE_COIN)
        UnloadBlueCoinModel();
    else if (mCarriedID == TRS_ID_T_BASKET)
        data_ov063_0211edec.Release();

    b = (actorID == TRS_ID_TERESA);
    if (b != 0) {
        data_ov063_0211edc4.Release();
        data_ov063_0211eddc.Release();
    } else {
        data_ov063_0211edf4.Release();
        data_ov063_0211ede4.Release();
        if (mVariant == 0xf) {
            UnloadKeyModels(3);
            data_ov063_0211edd4.Release();
            data_ov063_0211edcc.Release();
        }
    }
    UnloadCapModel();
    return 1;
}

void daTrs_c::OnPendingDestroy()
{
}

int daTrs_c::Render()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0)
        return 1;

    {
        if (!mBooFlags.visible)
            return 1;
        if (mBooFlags.carrying) {
            mCarriedModel.Render((const Vector3 *)&mCarriedScaleX);
        }
        RenderCapModel(0);
    }

    if (mOpacity < 8)
        return 1;

    {
        unsigned char st = mVariant;
        if (st >= 0xc && st != 0xf)
            mAnmModel.HideMaterial(0, 2);
    }

    if (mDeathState != 8 &&
        (mState == 3 ||
         mState == 3 ||
         mState == 3 ||
         mState == 3)) {
        mAnmModel.dExtSimpleModel_c::Render(&mScale);
    } else {
        mAnmModel.Render(&mScale);
    }

    return 1;
}

int daTBasket_c::Render()
{
    mModel.Render(0);
    return 1;
}

int daTrs_c::Behavior()
{
    Vector3 pv;
    Vector3 v1;
    Vector3 v2;
    Vector3 ve;
    int t;
    void *p;
    char *q;
    char *r2;
    s32 *p19c;
    u16 *fp;
    u8 *bp;
    s32 y;
    s32 w;
    s32 z;
    s32 x;
    int d1;
    int v;

    func_0200f760(&mdCcAcPos_c);
    if (mSpawnedActorID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnedActorID);
        q = (char *)mFoundActor;
        if (q != 0) {
            ((dActor_c *)q)->mPosX = mPosX;
            ((dActor_c *)q)->mPosY = mPosY;
            ((dActor_c *)q)->mPosZ = mPosZ;
        }
        mFoundActor = 0;
    }
    if (GetCapState() == 0)
        return 1;

    t = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 0);
    if (t != 0) {
        if (t == 2) {
            PoofDust();
            if (mDeathMode != 0) {
                KillAndTrackInDeathTable();
                func_0201267c(0xd5, &mCamSpacePos);
                if (((mCapId) & 0xf) < 6) {
                    mPosX = mHomePos.x;
                    mPosY = mHomePos.y;
                    mPosZ = mHomePos.z;
                    mAreaId = mAreaIdx;
                    mPrevAngleX = mHomeAngleX;
                    mPrevAngleY = mHomeAngleY;
                    mPrevAngleZ = mHomeAngleZ;
                    {
                        s16 (*ap)[3] = (s16 (*)[3])&this->mPrevAngleX;
                        mAngleX = (*ap)[0];
                        mAngleY = (*ap)[1];
                        mAngleZ = (*ap)[2];
                    }
                    p = RespawnIfHasCap();
                    if (p != 0) {
                        fp = (u16 *)&((daTrs_c *)p)->mBooFlags;
                        *fp = (*fp) & (~TRS_FLAG_CARRYING);
                    }
                }
            } else {
                mState = 5;
                mSubState = 0;
                p19c = (s32 *)&this->mdCcAcPos_c.flags;
                *p19c = (*p19c) | 1;
                fp = (u16 *)&this->mBooFlags;
                *fp = (*fp) & (~TRS_FLAG_VISIBLE);
            }
        }
        return 1;
    }

    mAreaId = -1;
    if (mSoundCount == 0) {
        /* Area-reset: when the home area is not showing or the leave test
           (02116190) passes, an undefeated Boo goes back to its home. */
        if ((IsAreaShowing(mAreaIdx) == 0) || (func_ov063_02116190() != 0)) {
            if (mBooFlags.defeated == 0) {
                r2 = (char *)&mPrevAngleX;
                mPosX = mHomePos.x;
                fp = (u16 *)&this->mBooFlags;
                mPosY = mHomePos.y;
                mPosZ = mHomePos.z;
                mPrevAngleX = mHomeAngleX;
                mPrevAngleY = mHomeAngleY;
                mPrevAngleZ = mHomeAngleZ;
                mAngleX = *(s16 *)r2;
                mAngleY = *(s16 *)(r2 + 2);
                mAngleZ = *(s16 *)(r2 + 4);
                *fp = (*fp) | TRS_FLAG_CAN_APPEAR;
                mState = 0;
                mHorzSpeed = 0;
                mVertAccel = 0;
                mVertSpeed = 0;
                mSubState = 0;
                mAreaId = mAreaIdx;
            }
            return 1;
        }
    }
    Unk_02005d94();
    t = UpdateYoshiEat(mWithMeshClsn);
    if (t != 0) {
        if (t == 1) {
            ve.x = mCapPosX;
            ve.y = mCapPosY;
            ve.z = mCapPosZ;
            if (GetCapEatenOffIt(ve) != 0)
                return 1;
        }
        mVertAccel = -0x2000;
        UpdateCarriedItemPos();
        if (mEatenByYoshi != 0) {
            u16 *p100 = (u16 *)&mStateTimer;
            if (p100[2] == 0) {
                mEatenByYoshi = 0;
                ((u16 *)&mStateTimer)[2] = 0;
                mVertAccel = 0;
                mVertSpeed = 0;
                goto block_39;
            }
        }
        v = (((mFlags) & 0x40000) ? 1 : 0);
        if (v != 0) {
            u8 st = mTalkStep;
            daPly_c *pl = (daPly_c *)mEatingPlayer;
            switch (st) {
            case 0:
                if (pl->ShowMessage(*this, 0x15a, 0, 0, 2) != 0) {
                    bp = (u8 *)&this->mTalkStep;
                    *bp = (*bp) + 1;
                    func_0201267c(0xf8, &mCamSpacePos);
                }
                break;
            case 1:
                if (pl->GetTalkState() == -1) {
                    pl->DropActor();
                    bp = (u8 *)&this->mTalkStep;
                    *bp = (*bp) + 1;
                }
                break;
            }
        }
        func_ov063_021166ac();
        mdCcAcPos_c.Clear();
        return 1;
    }
block_39:
    mTalkStep = 0;

    {
        mClosestPlayer = ClosestPlayer();
        daPly_c *plr = mClosestPlayer;
        if (plr != 0) {
            s32 *pp = &plr->mPosX;
            pv.x = pp[0];
            pv.y = pp[1];
            pv.z = pp[2];
            mAngleToPlayer = Vec3_HorzAngle(&mPos, &pv);
            mDistToPlayer = Vec3_HorzDist(&mPos, &pv);
        } else {
            this->mAngleToPlayer = mAngleY;
            mDistToPlayer = 0x2710000;
        }
    }
    mPrevState = mState;
    switch (mVariant) {
    case 0:
    case 1:
    case 2:
        UpdateBoo();
        break;
    case 3:
        func_ov063_02116bf4();
        break;
    case 4:
        func_ov063_02116a1c();
        break;
    case 5:
        func_ov063_02116d38();
        break;
    case 6:
    case 10:
        UpdateBoo();
        break;
    case 7:
        SpawnCourtyardBooTriplet();
        break;
    case 12:
    case 13:
    case 14:
        func_ov063_021189f4();
        break;
    case 15:
        func_ov063_021172a8();
        break;
    case 8:
        func_ov063_02119274();
        break;
    case 9:
        UpdateBoo();
        break;
    case 11:
        func_ov063_02116fac();
    }

    if (mBooFlags.followHeading != 0)
        mAngleY = mPrevAngleY;
    *(u16 *)&mStateTimer += 1;
    DecIfAbove0_Short(&mChaseDelay);
    if (mPrevState != mState)
        mStateTimer = 0;
    if (mVariant != 3) {
        UpdatePos(&mdCcAcPos_c);
        UpdateCarriedItemPos();
        if ((mBooFlags.limitZ != 0) && (mPosZ < -0x12c000))
            mPosZ = -0x12c000;
        dBgCh_Gnd rc1;
        y = mPosY;
        z = mPosZ;
        w = y + 0x32000;
        x = mPosX;
        v1.x = x;
        v1.y = w;
        v1.z = z;
        rc1.SetObjAndPos(v1, this);
        if (rc1.DetectClsn() != 0) {
            s32 ground = rc1.clsnY + 0x2000;
            if (mPosY < ground)
                mPosY = ground;
        }
        if ((mVariant != 4) && (mVariant != 0xb)) {
            dBgCh_Gnd rc2;
            y = mPosY;
            z = mPosZ;
            w = y + 0x32000;
            x = mPosX;
            v2.x = x;
            v2.y = w;
            v2.z = z;
            rc2.SetObjAndPos(v2, this);
            d1 = (int)(actorID == TRS_ID_TERESA);
            if (d1 != 0) {
                if (mVariant < 8) {
                    if (mBooFlags.groundSeen != 0) {
                        if ((rc2.DetectClsn() == 0) ||
                            ((mPosY - rc2.clsnY) > 0x12c000)) {
                            mPosX = mLastGroundPosX;
                            mPosY = mLastGroundPosY;
                            mPosZ = mLastGroundPosZ;
                        } else {
                            mLastGroundPosX = mPosX;
                            mLastGroundPosY = mPosY;
                            mLastGroundPosZ = mPosZ;
                        }
                    } else {
                        goto ray_e;
                    }
                } else {
                    goto ray_e;
                }
            } else {
            ray_e:
                if ((rc2.DetectClsn() != 0) &&
                    ((mPosY - rc2.clsnY) < 0x12c000)) {
                    fp = (u16 *)&this->mBooFlags;
                    *fp = (*fp) | TRS_FLAG_GROUND_SEEN;
                    mLastGroundPosX = mPosX;
                    mLastGroundPosY = mPosY;
                    mLastGroundPosZ = mPosZ;
                }
            }
            UpdateWMClsn(mWithMeshClsn, 0);
        }
    }
    if ((((mState != 3) && (mState != 3)) && (mState != 3)) && (mState != 3))
        mAnmModel.Advance();
    func_ov063_021166ac();
    mdCcAcPos_c.Clear();
    if (mOpacity == 0xff) {
        mdCcAcPos_c.SetPosRelativeToActor(mClsnOff);
        mdCcAcPos_c.Update();
    }
    return 1;
}


int daTBasket_c::Behavior()
{
    int onGround = 0;
    int secretDone = 1;

    if (mMuteSecretSound == 0)
        secretDone = Sound::PlaySecretSound(this, (u16 *)&mSoundTimer);

    if (mWithMeshClsn.JustHitGround()) {
        int vertSpeed = mVertSpeed;
        mVertSpeed = (-vertSpeed) >> 1;
        LandingDust(false);
    } else if (mWithMeshClsn.IsOnGround()) {
        onGround = 1;
        if (secretDone != 0 || (u16)mSoundTimer > 0x3c) {
            unsigned int id = mdCcAc_c.otherOwner;
            if (id != 0) {
                dActor_c *touched = dActor_c::FindWithID(id);
                if (touched != 0) {
                    if ((mdCcAc_c.hitFlags & 0x400000) != 0)
                        ((daPly_c *)touched)->JumpIntoBooCage(mPos);
                }
            }
        }
    }

    if (onGround == 0) {
        int z = mPosZ;
        int y = mPosY;
        int x = mPosX;
        unsigned int pid = (unsigned int)mParticleID;
        mParticleID = Particle::System::New(pid, kPtclBooCageFall, x, y + 0x64000, z, 0, 0);
    }

    UpdatePos(0);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov063_021169c4();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}


int daTrs_c::InitResources()
{
    int cond;
    int tmp598;
    dActor_c *spawned;
    daPly_c *pp;

    *(u16 *)&mBooFlags = 0;
    mSpawnedActorID = 0;
    cond = 0;
    if (actorID == TRS_ID_BOSS_TERESA) cond = 1;
    if (cond) {
        mVariant = (param1 & 0xf) + 0xc;
        if (mVariant == 0xf) {
            LoadKeyModels(3);
            dExtFrameCtrl_c::LoadFile(data_ov063_0211edd4);
            dExtFrameCtrl_c::LoadFile(data_ov063_0211edcc);
        } else if (mVariant == 0xc) {
            mFoundActor = dActor_c::Spawn(TRS_ID_ICON_TERESA, param1, mPos, 0, mAreaIdx, -1);
            if (mFoundActor != 0) {
                mSpawnedActorID = mFoundActor->uniqueID;
            }
            mFoundActor = 0;
        }
        dExtFrameCtrl_c::LoadFile(data_ov063_0211ede4);
        mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov063_0211edf4), 1, 1);
        mAnmModel.SetAnim((BCA_File *)data_ov063_0211ede4.filePtr, 0, 0x1000, 0);
        mHitPoints = 3;
        mClsnRadius = 0xc8;
        mClsnHeight = 0x104;
        mClsnZBias = -0x14000;
        tmp598 = mClsnZBias;
        mClsnOff.x = 0;
        mClsnOff.y = 0;
        mClsnOff.z = tmp598;
        mdCcAcPos_c.Init(this, mClsnOff, mClsnRadius << 0xc, mClsnHeight << 0xc, 0x200000, 0x207e0);
        if (mVariant != 0xf) {
            mWithMeshClsn.Init(this, 0xdc000, 0xdc000, 0, 0);
        } else {
            mWithMeshClsn.Init(this, 0xc8000, 0xb4000, 0, 0);
        }
    } else {
        mVariant = param1 & 0xf;
        dExtFrameCtrl_c::LoadFile(data_ov063_0211eddc);
        mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov063_0211edc4), 1, 0x16);
        mAnmModel.SetAnim((BCA_File *)data_ov063_0211eddc.filePtr, 0, 0x1000, 0);
        mHitPoints = 1;
        if (mVariant == 5) {
            mClsnZBias = -0x24000;
            tmp598 = mClsnZBias;
            mClsnOff.x = 0;
            mClsnOff.y = 0;
            mClsnOff.z = tmp598;
            mClsnRadius = 0x43;
            mClsnHeight = 0x5a;
        } else {
            mClsnZBias = -0x14000;
            tmp598 = mClsnZBias;
            mClsnOff.x = 0;
            mClsnOff.y = 0;
            mClsnOff.z = tmp598;
            mClsnRadius = 0x4a;
            mClsnHeight = 0x64;
        }
        mdCcAcPos_c.Init(this, mClsnOff, mClsnRadius << 0xc, mClsnHeight << 0xc, 0x200000, 0x207e0);
        if (gCurrentSublevel == 0xc && mPosX == 0xbb8000 && mAreaId == 2) {
            mBooFlags.limitZ = 1;
        }
        mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    }

    mCarriedID = TRS_ID_END_OF;
    if (mVariant == 5) {
        mBooFlags.carrying = 1;
        mCarriedID = TRS_ID_T_BASKET;
        dExtSimpleModel_c::LoadFile(data_ov063_0211edec);
    } else if (mVariant == 0 || mVariant == 1 || mVariant == 2 || mVariant == 6 || (unsigned)(unsigned char)(mVariant + 0xf8) <= 3) {
        mdCcAcPos_c.vulnFlags |= 0x8000;
        mBooFlags.carrying = 1;
        if (mVariant == 6) {
            mCarriedID = TRS_ID_COIN;
        } else if ((unsigned)(unsigned char)(mVariant + 0xf6) <= 1) {
            mCarriedID = TRS_ID_RED_COIN;
        } else {
            mCarriedID = TRS_ID_BLUE_COIN;
            LoadBlueCoinModel();
        }
    }

    mCapId = 6;
    mCapPosX = 0;
    mCapPosY = 0x60000;
    mCapPosZ = 0;
    cond = 0;
    if (actorID == TRS_ID_TERESA) cond = 1;
    if (cond && mVariant != 8) {
        unsigned char capIdx;
        mHadBank1Cap = (param1 >> 0xc) & 0xf;
        capIdx = (param1 >> 8) & 0xf;
        AddCap(capIdx);
        if ((mCapId & 7) < 6) {
            /* int on the store side only: spelling both sides identically
               lets mwccarm CSE the field address (addlt r2,r4,#8 + [r2]),
               one instruction the ROM does not have -- it wants [r4,#8] direct.
               Same lever as daKrb_c::OnTurnIntoEgg in src/game/actors/d_a_krb.cpp. */
            *(int *)&this->param1 = param1 & 0xfff;
        }
        if (DestroyIfCapNotNeeded() == 0) {
            return 0;
        }
    }

    if ((unsigned)(unsigned char)(mVariant + 0xf6) <= 1) {
        if ((unsigned)NumStars() < 3) {
            MarkForDestruction();
            return 0;
        }
        if (mVariant == 0xb && (unsigned)NumStars() >= 0xf) {
            MarkForDestruction();
            return 0;
        }
        if (IsStarCollectedInCurLevel(1) != 0) {
            mCarriedID = TRS_ID_COIN;
        }
    }

    if (mShadowModel1.InitCylinder() == 0) return 0;
    if (mShadowModel2.InitCylinder() == 0) return 0;

    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    mStateTimer = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mOpacity = 0xff;
    mWithMeshClsn.StartIgnoringVanishSurfaces();
    mState = 0;
    unk_5b2 = mPrevAngleY;
    mWobblePhase = 0;
    mBooFlags.followHeading = 1;
    mBigBooID = 0;
    mSpawnerID = 0;
    mCachedActorID = 0;
    mTalkPlayer = 0;
    mChildDeaths = 0;
    mWobblePhase = 0;
    mDeathMode = data_ov063_0211e22c[mVariant];
    mHurtDamage = data_ov063_0211e1ec[mVariant];
    unk_5a8 = 0;
    mBooFlags.visible = 1;
    mBooDefeatCount = 0;
    mTerminalVelocity = -0x3c000;
    mAreaIdx = mAreaId;
    mSoundCount = 0;
    mRelocateTimer = 0;
    mNextRelocateTimer = 0;
    mLeashDist = ((param1 >> 8) & 0xff) * 0x64000;
    unk_5d3 = 0;
    mCarriedScaleX = 0xc00;
    mCarriedScaleY = 0xc00;
    mCarriedScaleZ = 0xc00;

    if (mVariant == 5) {
        if ((unsigned)NumStars() < 0xf) {
            MarkForDestruction();
            return 1;
        }
        /* gCurrentEntranceId is LAST_ENTRANCE_ID. When it is 0 the cage (mCarriedID
           is T_BASKET here) is spawned at once, muted; if that Spawn succeeds
           this Boo is removed. */
        if (gCurrentEntranceId == 0) {
            spawned = dActor_c::Spawn(mCarriedID, 0, mPos, 0, mAreaIdx, -1);
            if (spawned != 0) {
                ((daTBasket_c *)spawned)->mMuteSecretSound = 1;
                MarkForDestruction();
                return 1;
            }
        }
        mCarriedModel.SetFile((BMD_File *)data_ov063_0211edec.filePtr, 1, -1);
    } else if (mVariant == 0 || mVariant == 1 || mVariant == 2 || mVariant == 6 || (unsigned)(unsigned char)(mVariant + 0xf8) <= 3) {
        unsigned short t = mCarriedID;
        if (t == TRS_ID_BLUE_COIN) {
            mCarriedModel.SetFile((BMD_File *)data_ov002_0210d9c8.filePtr, 1, -1);
        } else if (t == TRS_ID_RED_COIN) {
            mCarriedModel.SetFile((BMD_File *)data_ov002_0210d9f8.filePtr, 1, -1);
        } else {
            mCarriedModel.SetFile((BMD_File *)data_ov002_0210d9b8.filePtr, 1, -1);
        }
        if (mVariant == 0xb) {
            mAreaId = -1;
            pp = ClosestPlayer();
            if (pp != 0 && pp->mPosZ > (int)0xffaec000) {
                mBooFlags.canAppear = 1;
            }
        }
    } else {
        if ((unsigned)mVariant < 0xc) {
            mdCcAcPos_c.flags |= 1;
        }
        if (mVariant == 4) {
            mAreaId = -1;
            pp = ClosestPlayer();
            if (pp != 0 && pp->mPosZ > (int)0xffaec000) {
                mBooFlags.canAppear = 1;
            }
        } else if (mVariant == 7) {
            mBooFlags.carrying = 1;
            mCarriedID = TRS_ID_COIN;
        } else if (mVariant == 0xe) {
            mAreaId = -1;
        }
    }

    mClsnBase.x = 0;
    mClsnBase.y = 0;
    mClsnBase.z = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    MTX_Concat43(this->mAnmModel.data.transforms, &data_020a0e68, &data_020a0e68);
    mClsnBase.x = data_020a0e68.m[9];
    mClsnBase.y = data_020a0e68.m[10];
    mClsnBase.z = data_020a0e68.m[11];
    VEC_Subtract(&mClsnBase, &mPos, &mClsnBase);
    mTalkStep = 0;
    mTargetAngleY = mPrevAngleY;
    mParticle1 = 0;
    mParticle0 = mParticle1;
    return 1;
}


int daTBasket_c::InitResources()
{
    BMD_File *bmd = dExtSimpleModel_c::LoadFile(data_ov063_0211edec);
    if (mModel.SetFile(bmd, 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x46000;
    mdCcAc_c.Init(this, 0x64000, 0x64000, 0x200004, 0);
    mWithMeshClsn.Init(this, 0x64000, 0x64000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    mParticleID = 0;
    mSoundTimer = 0;
    mMuteSecretSound = 0;
    return 1;
}

int daTrsIcon_c::InitResources()
{
    mStarID = (param1 >> 8) & 0xf;
    mTrackStarID = -1;
    mTrackStarID = TrackStar(mStarID, 2);
    return 1;
}

int daTrs_c::OnYoshiTryEat() {
    unsigned short v = actorID;
    int r;
    if (v == TRS_ID_TERESA) r = 1; else r = 0;
    if (r != 0) r = 7; else r = 0;
    return r;
}

extern "C" daTrsIcon_c *daTrsIcon_c_classInit()
{
    return new daTrsIcon_c();
}

extern "C" daTBasket_c *daTBasket_c_classInit()
{
    return new daTBasket_c();
}

extern "C" daTrs_c *daTrs_c_classInit_BOSS_TERESA()
{
    return new daTrs_c();
}

extern "C" daTrs_c *daTrs_c_classInit_TERESA()
{
    return new daTrs_c();
}

/* File-scope objects in the order __sinit_ov063_0211e29c builds them: the
 * three model-file handles first, then the four animation-file handles. Each
 * definition emits one construct-and-register pair into .init plus a
 * registration cell in .bss. */
TrsModelFileResHandle data_ov063_0211edc4(ASSET_TERESA_BMD);
TrsModelFileResHandle data_ov063_0211edec(ASSET_T_BASKET_BMD);
TrsModelFileResHandle data_ov063_0211edf4(ASSET_BOSS_TERESA_BMD);
TrsAnimationResFileHandle data_ov063_0211eddc(ASSET_TERESA_WAIT_BCA);
TrsAnimationResFileHandle data_ov063_0211ede4(ASSET_BOSS_TERESA_WAIT_BCA);
TrsAnimationResFileHandle data_ov063_0211edd4(ASSET_BOSS_TERESA_FIRE_BCA);
TrsAnimationResFileHandle data_ov063_0211edcc(ASSET_BOSS_TERESA_FIRE_ST_BCA);
