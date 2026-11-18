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
#include "daCoin_c.h"
#include "daTrsTrap_c.h"
#include "daFPknBall_c.h"

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

/* ABI seams whose header member forms do not reproduce the ROM (see notes) */

extern signed char gCurrentSublevel;
extern unsigned char gCurrentEntranceId;
/* Per-variant tables indexed by mVariant: death mode, hurt damage. */
extern const int data_ov063_0211e22c[16] = {
    1, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 1, 0, 1,
};
extern const int data_ov063_0211e1ec[16] = {
    2, 2, 2, 0, 0, 3, 2, 0, 2, 2, 2, 0, 3, 3, 3, 3,
};
extern Matrix4x3 data_020a0e68;
}
void LoadKeyModels(int n);
void UnloadKeyModels(int n);
unsigned char NumStars(void);
int IsStarCollectedInCurLevel(int n);
u8 IsAreaShowing(int idx);

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
int func_020092c4(void *cam, void *out, void *target);
u16 func_0201277c(unsigned int a);
extern dCamera_c *gCamera;
extern void func_02011cfc(void);
extern const s16 data_ov063_0211e1c8[4] = {223, 224, 225, 226};
/* The call at 0x02118004 relocates to overlays(27,55). r0 still holds the
   state timer there, so the callee takes no object: ov055's free function at
   0x02111264, not ov027's daObjSlIceBlock_c::CleanupResources. */
extern int func_ov002_020c51d0(daPly_c *player, Vector3 *pos);
extern int data_ov008_02111b6c;
extern const s16 data_ov063_0211e1dc[4] = {219, 220, 221, 222};
extern const s16 data_ov063_0211e1e4[4] = {235, 220, 221, 222};
extern const s16 data_ov063_0211e1c0[4] = {0x380, 0x240, 0x180, 0x0};
extern const int data_ov063_0211e1d0[3] = {0x1000, 0xfa0, 0xee0};
extern int gMusicVolume;
extern u8 data_0209d660;
extern short data_ov063_0211e7e0[];
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
}
void func_02012694(unsigned int a, const Vector3 *p);
extern void func_020167a4(dExtAnmModel_c *model);
#include "daLuigi_c.h"
extern u16 func_02011d14(void);

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
    Matrix4x3_FromTranslation(&data_020a0e68, this->mPos.x, this->mPos.y, this->mPos.z);
    MTX_Concat43(this->mAnmModel.data.transforms, &data_020a0e68, &data_020a0e68);
    this->mClsnOff.x = data_020a0e68.t.x;
    this->mClsnOff.y = data_020a0e68.t.y;
    this->mClsnOff.z = data_020a0e68.t.z;
    VEC_Subtract(&this->mClsnOff, &this->mPos, &this->mClsnOff);
    VEC_Subtract(&this->mClsnOff, &this->mClsnBase, &this->mClsnOff);
    Vec3_MulScalarInPlace(&this->mClsnOff, 0x6800);
    p = &this->mClsnOff.z;
    *p += this->mClsnZBias;
}

int daTrs_c::func_ov063_02116190() {

    Vector3 vec;
    daPly_c *p;

    if (this->mBooFlags.limitZ) {
        p = ClosestPlayer();
        if (p != 0 && p->mPos.z >= 0x3e8000)
            return 1;
    } else if (this->mPos.y <= (int)0xff768000) {
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
    this->mFoundActor =
        dActor_c::Spawn(TRS_ID_ICON_TERESA, this->param1, this->mPos, (0), this->mAreaIdx, -1);
    if (this->mFoundActor != 0) this->mSpawnedActorID = this->mFoundActor->uniqueID;
    this->mFoundActor = 0;
}


void daTrs_c::func_ov063_021162c8() {

    if (this->mBooFlags.hitByParam3Player) {
        this->mState = 4;
    } else {
        this->mState = 3;
        Vector3 v;
        v = this->mCapPos;
        ReleaseCap(v);
        if (this->mBooFlags.carrying) {
            unsigned int flags = 2;
            if (this->mCarriedID == TRS_ID_RED_COIN) flags |= 0x10;
            dActor_c *a =
                dActor_c::Spawn(this->mCarriedID, flags, this->mCarriedPos, 0, this->mAreaIdx, -1);
            if (a != 0) {
                ((daCoin_c *)a)->mNoClsnTimer = 10;
                if (this->mCarriedID == TRS_ID_RED_COIN)
                    ((daCoin_c *)a)->mDisappearTimer = 0;
            }
            this->mBooFlags.carrying = 0;
        }
    }
    func_0201267c(0x14a, &this->mCamSpacePos);
}


unsigned char daTrs_c::func_ov063_021163d0() {

    unsigned int id = this->mCachedActorID;
    if(id==0) return 0;
    daTrsTrap_c *a = (daTrsTrap_c *)dActor_c::FindWithID(id);
    if(a==0) return 0;
    return a->mOccupiedFlag;
}


void daTrs_c::func_ov063_0211640c() {

    struct Vector3 pos, t1, t2;
    s16 ang;

    if (!this->mBooFlags.visible)
        return;

    pos = this->mPos;
    ang = this->mAngle.y;

    if (this->mBooFlags.mirrored) {
        pos.x *= -1;
        ang *= -1;
    }

    if (this->mState == 3) {
        Vec3_Asr(&t1, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        this->mAnmModel.mat4x3 = data_020a0e68;
        this->mAnmModel.ApplyOpacity((u8)((int)this->mOpacity >> 3), 1);
        func_020167a4(&this->mAnmModel);
        {
            TrsBone *m = (TrsBone *)this->mAnmModel.data.bones;
            m->rotX = this->mAngle.x;
            m->rotY = (s16)(ang - 0x4000);
            m->rotZ = this->mAngle.z;
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
        this->mShadowMtx = data_020a0e68;
        DropShadowRadHeight(this->mShadowModel1, this->mShadowMtx, 0x12c000, 0xc8000, 0xf);

        if (this->mBooFlags.mirrored) {
            pos.x *= -1;
            Matrix4x3_FromTranslation(&data_020a0e68,
                pos.x >> 3, pos.y >> 3, pos.z >> 3);
            this->mShadowMtx2 = data_020a0e68;
            DropShadowRadHeight(this->mShadowModel2, this->mShadowMtx2, 0x12c000, 0xc8000, 0xf);
        }
    }

    func_ov063_021160d4();
}


void daTrs_c::func_ov063_021166ac() {

    Vector3_16 rot;
    Vector3 t1;
    Vector3 t2;
    Vector3 pos;

    if (mVariant == 0xf) {
        func_ov063_0211640c();
        return;
    }
    if (!mBooFlags.visible)
        return;
    if (mBooFlags.carrying) {
        if (this->mCarriedID != TRS_ID_T_BASKET) {
            this->mCarriedAngleY += 0xc00;
        }
        Matrix4x3_FromRotationY(&this->mCarriedModel.mat4x3, this->mCarriedAngleY);
        this->mCarriedModel.mat4x3.t.x = this->mCarriedPos.x >> 3;
        this->mCarriedModel.mat4x3.t.y = this->mCarriedPos.y >> 3;
        this->mCarriedModel.mat4x3.t.z = this->mCarriedPos.z >> 3;
    }

    if (mState == 3 || mState == 3 || mState == 3 || mState == 3) {
        Vec3_Asr(&t1, &this->mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        this->mAnmModel.mat4x3 = data_020a0e68;
        this->mAnmModel.ApplyOpacity((u8)(this->mOpacity >> 3), 1);
        func_020167a4(&this->mAnmModel);
        {
            TrsBone *p = (TrsBone *)this->mAnmModel.data.bones;
            p->rotX = this->mAngle.x;
            p->rotY = this->mAngle.y - 0x4000;
            p->rotZ = this->mAngle.z;
        }
        this->mAnmModel.data.UpdateVertsUsingBones();
    } else {
        Vec3_Asr(&t2, &this->mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, this->mAngle.y);
        this->mAnmModel.mat4x3 = data_020a0e68;
        this->mAnmModel.ApplyOpacity((u8)(this->mOpacity >> 3), 1);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, this->mPos.x >> 3, this->mPos.y >> 3,
                              this->mPos.z >> 3);
    this->mShadowMtx = data_020a0e68;
    {
        int big = (this->actorID == TRS_ID_BOSS_TERESA);
        if (big)
            DropShadowRadHeight(this->mShadowModel1, this->mShadowMtx, 0x12c000, 0xc8000, 0xf);
        else
            DropShadowRadHeight(this->mShadowModel1, this->mShadowMtx, 0x64000, 0xc8000, 0xf);
    }
    this->mCapPos.y = this->mClsnOff.y + FX_MUL(this->mScale.x, 0x60000);
    pos = this->mCapPos;
    {
        /* The angle triple copies as two halfword loads, then two stores, then
           the third pair: two named u16 temps for x and y reproduce that; a
           Vector3_16 struct copy or three direct member copies alternate
           load/store (and the s16-typed copy sign-extends, ldrsh).
           Kept (#821 r76): `rot = this->mAngle;` is memberwise ldrsh/strh, 4 words off. */
        u16 ax = *(u16 *)&this->mAngle.x;
        u16 ay = *(u16 *)&this->mAngle.y;
        rot.y = ay;
        rot.x = ax;
        rot.z = *(u16 *)&this->mAngle.z;
    }
    /* equal-arm ternary: the rot address (r2) is set up before pos (r1), as in the ROM */
    UpdateCapPos(pos, this ? rot : rot);
    func_ov063_021160d4();
}


void daTBasket_c::func_ov063_021169c4() {

    Matrix4x3_FromTranslation(&this->mModel.mat4x3, this->mPos.x >> 3, this->mPos.y >> 3,
                              this->mPos.z >> 3);
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
            *fp |= TRS_FLAG_VISIBLE;
            this->mOpacity = 0xb4;
            scale = this->mClsnScale;
            this->mScale.x = scale;
            this->mScale.y = scale;
            this->mScale.z = scale;
            this->mdCcAcPos_c.radius = this->mClsnRadius * this->mClsnScale;
            this->mdCcAcPos_c.height = this->mClsnHeight * this->mClsnScale;
            this->mState++;
        }
    } else if (a == 1) {
        if (this->mDistToPlayer < 0x3e8000) {
            this->mState++;
            func_0201267c(0xf8, &this->mCamSpacePos);
        }
        this->mHorzSpeed = 0;
    } else {
        int t;
        ApproachLinear(this->mHorzSpeed, 0x30000, 0x1800);
        t = 0x3e8000;
        v.x = -t;
        v.z = (int)0xfdcd8000;
        if (this->mPos.z < (int)0xfec78000) {
            /* invert: laundered RMW as THEN, plain zero as ELSE
               -> movls/strbls + bls + unpredicated RMW */
            if (this->mOpacity > 0x14) {
                this->mOpacity -= 0x14;
            } else {
                this->mOpacity = 0;
            }
        }
    }

    this->mSpeed.y = 0;
    ApproachLinear(this->mPrevAngle.y, Vec3_HorzAngle(&this->mPos, &v), 0x5a8);
    Oscillate(1);
}


void daTrs_c::func_ov063_02116bf0() {

}


/* Behavior handler for variant 3: spawns TERESA children (param 0xfff1)
   while fewer than two are alive, five at most. */
void daTrs_c::func_ov063_02116bf4() {

    unsigned short *ip = (unsigned short *)&this->mBooFlags;
    unsigned char st;

    *ip = (unsigned short)(*ip & ~TRS_FLAG_VISIBLE);
    this->mdCcAcPos_c.flags = this->mdCcAcPos_c.flags | 1;
    func_ov063_02119c18(TRS_ID_MERRYGOROUND);

    st = this->mState;
    switch (st) {
    case 0:
        if (this->mDistToPlayer >= 0x3e8000)
            return;
        if (this->mChildDeaths < 5) {
            unsigned char cb = this->mSpawnCount;
            if (cb != 5 && (int)cb - this->mChildDeaths < 2) {
                daTrs_c *p = (daTrs_c *)dActor_c::Spawn(TRS_ID_TERESA, 0xfff1, this->mPos,
                                                        &this->mPrevAngle, this->mAreaIdx, -1);
                if (p != 0) {
                    p->mSpawnerID = this->uniqueID;
                    p->mCachedActorID = this->mCachedActorID;
                }
                this->mSpawnCount++;
            }
            this->mState++;
        }
        if (this->mChildDeaths >= 5)
            this->mState = 2;
        break;
    case 1:
        if (this->mStateTimer > 60)
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
        dActor_c *r =
            dActor_c::Spawn(this->mCarriedID, 0, this->mCarriedPos, (0), this->mAreaIdx, -1);
        if (r != 0) {
            r->mSpeed.x = 0;
            r->mSpeed.y = 0x32000;
            r->mSpeed.z = 0;
        }
    }
    this->mBooFlags.carrying = 0;
}


int daTrs_c::func_ov063_02116f48() {

    this->mBigBooID = 0;
    this->mTargetOpacity = 0xff;
    this->mClsnScale = 0x2000;
    this->mScale.x = 0x2000;
    this->mScale.y = 0x2000;
    this->mScale.z = 0x2000;
    this->mdCcAcPos_c.radius = this->mClsnRadius * this->mScale.x;
    this->mdCcAcPos_c.height = this->mClsnHeight * this->mScale.x;
    int r = ShouldBeActive();
    if (r) { this->mState = 1; r = 1; }
    return r;
}


/* Behavior handler for variant 0xb: the scripted-path Boo. Hidden until
   canAppear, then waits for the player within 0x258000, flies a fixed path
   (hopping in state 4), then vanishes (opacity 0, hidden). */
void daTrs_c::func_ov063_02116fac() {

    Vector3 target = this->mHomePos;
    s16 r4 = 0xc00;

    this->mClsnScale = 0x1000;

    switch (this->mState) {
    case 0:
        this->mBooFlags.visible = 0;
        if (this->mBooFlags.canAppear) {
            this->mState++;
            this->mOpacity = 0xb4;
            this->mBooFlags.visible = 1;
        }
        break;
    case 1:
        if (this->mDistToPlayer < 0x258000) {
            this->mState++;
            func_0201267c(0xf8, &this->mCamSpacePos);
        }
        this->mHorzSpeed = 0;
        break;
    case 2:
        ApproachLinear(this->mHorzSpeed, 0x3a000, 0x1800);
        target = Vector3(-0x3e8000, 0, 0xfea84000);
        if (this->mPos.z < (int)0xfee08000) {
            this->mState++;
        }
        break;
    case 3:
        ApproachLinear(this->mHorzSpeed, 0, 0x9000);
        target = Vector3(-0x3e8000, 0, -0xfa0000);
        this->mTargetAngleY = Vec3_HorzAngle(&this->mPos, &target);
        r4 = 0x1000;
        if (this->mAngle.y == this->mTargetAngleY && this->mHorzSpeed == 0) {
            this->mState++;
        }
        break;
    case 4:
        if (this->mStateTimer == 6) {
            this->mSpeed.y = 0xf000;
            this->mVertAccel = -0x4000;
            this->mTerminalVelocity = -0xf000;
        }
        if (this->mPos.y < this->mHomePos.y) {
            this->mPos.y = this->mHomePos.y;
            this->mState++;
            this->mVertAccel = 0;
            this->mTerminalVelocity = 0;
        }
        break;
    case 5:
        ApproachLinear(this->mHorzSpeed, 0x3a000, 0x1800);
        target = Vector3(-0x3e8000, 0, 0xfea84000);
        r4 = 0x1000;
        if (this->mPos.z < (int)0xfec78000) {
            this->mState++;
        }
        break;
    case 6:
        this->mHorzSpeed = 0;
        this->mOpacity = 0;
        this->mBooFlags.visible = 0;
        break;
    }

    if (this->mState != 4) {
        this->mSpeed.y = 0;
        this->mTargetAngleY = Vec3_HorzAngle(&this->mPos, &target);
    }
    ApproachLinear(this->mPrevAngle.y, this->mTargetAngleY, r4);
    Oscillate(1);
}

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
            if (this->mRelocateTimer > 90)
                return;
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
                this->mBooFlags.mirrored ^= 1;
            }
            func_ov063_02117650();
            this->mTargetOpacity = 0xff;
            this->mParticle1 = 0;
            this->mParticle0 = this->mParticle1;
            this->mRelocateTimer = 120;
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

    if (this->mRelocateTimer > 28) {
        pos.y += this->mClsnScale * 0xaf;
        if (this->mBooFlags.mirrored)
            pos.x *= -1;
        this->mParticle0 = Particle::System::New(this->mParticle0, 0x97, pos.x, pos.y, pos.z, 0, 0);
        this->mParticle1 = Particle::System::New(this->mParticle1, 0x98, pos.x, pos.y, pos.z, 0, 0);
    }
    if (this->mOpacity != 0xff)
        return;
    if (this->mRelocateTimer > 28)
        this->mRelocateTimer = 28;
    if (this->mRelocateTimer != 0)
        return;
    this->mState = 1;
    this->mdCcAcPos_c.flags &= ~1u;
    this->mAttackTimer = ((u32)RandomIntInternal(&data_0209e650.value) >> 16 & 0x3f) + 60;
    this->mNextRelocateTimer = ((u32)RandomIntInternal(&data_0209e650.value) >> 16) % 150 + 300;
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
        int *pp = &((daPly_c *)p)->mPos.x;
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
    }
    npos.y = this->mPos.y;

    do {
        npos.x =
            ((int)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 0x578) * neg1 -
             0x190)
            << 0xc;
        npos.z =
            ((int)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 0xa28) - 0x514)
            << 0xc;
    } while (Vec3_HorzDist(&ppos, &npos) < 0x320000);

    this->mPos.x = npos.x;
    this->mPos.y = npos.y;
    this->mPos.z = npos.z;
    this->mAngle.y = Vec3_HorzAngle(&this->mPos, &ppos);
    this->mPrevAngle.y = this->mAngle.y;
}


void daTrs_c::func_ov063_0211776c() {

    Stop();
    unsigned short v = this->mStateTimer;
    if (v > 0xf)
        return;
    if (v != 0xf)
        return;
    func_ov063_02118ddc();
    this->MarkForDestruction();
}


void daTrs_c::func_ov063_021177b0() {

    if (this->mStateTimer == 0)
        this->mHitPoints--;

    if (this->mHitPoints == 0) {
        dCamera_c *cam = gCamera;

        if (this->mBooFlags.camHeld) {
            daPly_c *pl = ClosestPlayer();
            if (pl != 0) {
                Vector3 v;
                Vector3 mid;
                Vector3 look;
                int dist;
                int scaled;
                short ang;
                short sn;
                int t;

                const Vector3 &playerPos = pl->mPos;
                v = playerPos;
                dist = Vec3_Dist(&this->mPos, &v);
                if (this->mBooFlags.mirrored) {
                    mid.x = (v.x - this->mPos.x) / 2;
                } else {
                    mid.x = (this->mPos.x + v.x) / 2;
                }
                mid.y = (this->mPos.y + v.y) / 2;
                mid.z = (this->mPos.z + v.z) / 2;
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

        this->mBooFlags.visible = 0;
        this->mState = 8;
        Stop();
        this->mPrevAngle.x = 0;
        this->mPrevAngle.y = 0;
        this->mPrevAngle.z = 0;
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

    if (this->mStateTimer == 0)
        this->mClsnScale -= 0x255;
    if (UpdateDuringNonlethalHit(0x28) == 0)
        return;
    this->mState = 6;
    this->mdCcAcPos_c.flags |= 1;
    this->mTargetOpacity = 0;
    Stop();
    this->mRelocateTimer = 120;
    func_02012694(0x153, &this->mCamSpacePos);
}


void daTrs_c::func_ov063_02117b0c() {

    Vector3 v;

    switch (this->mSubState) {
    case 0:
        this->mTalkPlayer = ClosestPlayer();
        if (!this->mTalkPlayer->StartTalk(*this, 1))
            return;
        this->mSubState++;
        Sound::ChangeMusicVolume(0x14, 0x15666);
        break;
    case 1:
        this->mSubState++;
        break;
    case 2:
        if (this->mTalkPlayer->GetTalkState() != 0)
            return;
        {
            int y = this->mPos.y;
            int x = this->mPos.x;
            int z = this->mPos.z;
            int ny = y + 0xc8000;
            int nx = -x;
            v.x = nx;
            v.y = ny;
            v.z = z;
        }
        {
            void *pl = this->mTalkPlayer;
            unsigned m = (unsigned)(int)data_ov063_0211e1c8[((daPly_c *)pl)->param1];
            if (!this->mClosestPlayer->ShowMessage(*this, m, &v, 0, 2))
                return;
            func_0201277c(0x151);
            this->mSubState++;
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
    Vector3 w;
    dCamera_c *cam;
    int neg;

    cam = gCamera;
    switch (this->mSubState) {
    case 0:
        this->mTalkPlayer = ClosestPlayer();
        if (this->mTalkPlayer->mPos.x <= -0x2bc000) {
            return;
        }
        this->mSubState += 1;
        this->mStateTimer = 0;
        return;
    case 1: {
        if (this->mStateTimer < 150) {
            return;
        }
        this->mSubState++;
        return;
    }
    case 2: {
        if (this->mTalkPlayer == 0) {
            return;
        }
        w = this->mTalkPlayer->mPos;
        if (VEC_Mag(&w) >= 0x12c000) {
            return;
        }
        if (this->mTalkPlayer->StartTalk(*this, 1) == 0) {
            return;
        }
        cam->SetFlag_3();
        this->mTalkCamPos.x = w.x;
        this->mTalkCamPos.y = w.y;
        this->mTalkCamPos.z = w.z;
        this->mTalkCamPos.x -= 0x3c000 - (w.x / 8);
        this->mTalkCamPos.y = 0x64000;
        this->mTalkCamPos.z -= 0x64000 - (w.x / 6);
        this->mTalkCamLook.x = w.x;
        this->mTalkCamLook.y = w.y;
        this->mTalkCamLook.z = w.z;
        this->mTalkCamLook.x = 0;
        this->mTalkCamLook.y = 0x64000;
        this->mSubState++;
        return;
    }
    case 3: {
        s16 ang;
        int b;
        s16 *q;
        w = this->mTalkPlayer->mPos;
        w.x = 0;
        ang = Vec3_HorzAngle(&this->mTalkPlayer->mPos, &w);
        q = &this->mTalkPlayer->mAngle.x;
        v[0] = q[0];
        v[1] = q[1];
        v[2] = q[2];
        b = func_020092c4(cam, &cam->pos, &this->mTalkCamPos);
        b = b & func_020092c4(cam, &cam->lookAt, &this->mTalkCamLook);
        if (ApproachLinear(v[1], ang, 0x200) != 0 && b != 0) {
            this->mSubState += 1;
            this->mStateTimer = 0;
        }
        {
            daPly_c *d = this->mTalkPlayer;
            d->mAngle.x = v[0];
            d->mAngle.y = v[1];
            d->mAngle.z = v[2];
        }
        {
            daPly_c *d = this->mTalkPlayer;
            d->mPrevAngle.x = v[0];
            d->mPrevAngle.y = v[1];
            d->mPrevAngle.z = v[2];
        }
        return;
    }
    case 4:
        if (this->mStateTimer < 30) {
            return;
        }
        daLuigi_c::func_ov055_02111264();
        this->mSubState++;
        return;
    case 5: {
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
        w = this->mTalkPlayer->mPos;
        this->mTalkCamLook.z += 0x50000 - (w.x / 3);
        this->mPos.x = w.x - 0xc8000;
        this->mPos.z = (this->mTalkCamLook.z + 0x12c000) - ((w.x * 2) / 3);
        this->mTalkCamPos.x = w.x - 0x82000;
        this->mTalkCamPos.z = w.z - 0x32000;
        this->mStateTimer = 0;
        this->mSubState++;
        return;
    }
    case 6:
        if (this->mStateTimer < 30) {
            return;
        }
        this->mSubState++;
        return;
    case 7: {
        int t0;
        t0 = func_020092c4(cam, &cam->pos, &this->mTalkCamPos);
        if ((t0 & func_020092c4(cam, &cam->lookAt, &this->mTalkCamLook)) == 0) {
            return;
        }
        this->mOpacity = 0;
        this->mSubState += 1;
        neg = -1;
        w = this->mTalkPlayer->mPos;
        w.x = w.x * neg;
        this->mAngle.y = Vec3_HorzAngle(&this->mPos, &w);
        this->mPrevAngle.y = this->mAngle.y;
        func_0201277c(0x150);
        return;
    }
    case 8:
        /* invert so ELSE (=0xff) is predicated and THEN (RMW) is branched (codegen 6c) */
        if (this->mOpacity + 5 < 0xff) {
            this->mOpacity += 5;
        } else {
            this->mOpacity = 0xff;
        }
        w.x = this->mPos.x;
        w.y = this->mPos.y;
        w.z = this->mPos.z;
        neg = -1;
        w.x = w.x * neg;
        w.y = w.y + 0xc8000;
        func_ov002_020c51d0(this->mTalkPlayer, &w);
        if (this->mOpacity == 0xff) {
            this->mSubState += 1;
            this->mTargetOpacity = 0xff;
        }
        this->mAngle.y = this->mPrevAngle.y;
        return;
    case 9: {
        int tk = this->mTalkPlayer->GetTalkState();
        s16 msg;
        if (tk != 0) {
            return;
        }

        {
            int x = 0 - this->mPos.x;
            int z = this->mPos.z;
            int y = this->mPos.y + 0xc8000;
            int fl = data_0209caa0.file.flags1 & 0x10;
            w.x = x;
            w.y = y;
            w.z = z;
            if (fl)
                msg = data_ov063_0211e1e4[this->mTalkPlayer->param1];
            else
                msg = data_ov063_0211e1dc[this->mTalkPlayer->param1];
        }
        if (this->mClosestPlayer->ShowMessage(*this, (u32)msg, &w, 0, 2) == 0) {
            return;
        }
        Sound::LoadAndSetMusic_Layer3(0x2c);
        this->mSubState++;
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
        this->mAttackTimer = (((u32)RandomIntInternal(&data_0209e650.value) >> 0x10) & 0x3f) + 180;
        cam->mFlags &= ~8;
        rr = RandomIntInternal(&data_0209e650.value);
        this->mNextRelocateTimer = ((rr >> 0x10) % 150) + 300;
        Sound::LoadAndSetMusic_Layer3(0x2d);
        func_02011d14();
        return;
    }
    }
    return;
}


void daTrs_c::func_ov063_02118458() {
    int status = GetAttackStatus();

    ApproachLinear(this->mPrevAngle.y, this->mAngleToPlayer,
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
                this->mAttackTimer = rnd + 180;
                this->mAnmModel.SetAnim((BCA_File *)data_ov063_0211ede4.filePtr, 0, 0x1000, 0);
            }
        } else {
            int rnd;
            this->mState = 1;
            this->mdCcAcPos_c.flags &= ~1;
            rnd = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) & 0x3f;
            this->mAttackTimer = rnd + 180;
            this->mAnmModel.SetAnim((BCA_File *)data_ov063_0211ede4.filePtr, 0, 0x1000, 0);
        }
    } else {
        if (this->mAnmModel.file == (BCA_File *)data_ov063_0211edd4.filePtr &&
            this->mAnmModel.WillHitFrame(6)) {
            Vector3 v = this->mPos;
            v.x += FX_SinIdx((u16)this->mAngle.y >> 4) * 0x78;
            v.z += FX_CosIdx((u16)this->mAngle.y >> 4) * 0x78;
            dActor_c *fb;
            this->mAngle.x = 0x1000;
            func_02012694(0x156, &this->mCamSpacePos);
            fb = SpawnFireball(v, &this->mAngle, 0x14000, 0x6999, 4);
            this->mAngle.x = 0;
            if (this->mBooFlags.mirrored)
                ((daFPknBall_c *)fb)->mMirrored = 1;
            else
                ((daFPknBall_c *)fb)->mMirrored = 0;
            ((daFPknBall_c *)fb)->mMaxDistance = 0x3e8000;
            func_02012694(0x122, &this->mCamSpacePos);
        }
    }

    if (status == -1) {
        this->mState = 2;
        return;
    }
    if (status != 1)
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
        this->mState = 6;
        this->mdCcAcPos_c.flags |= 1;
        this->mTargetOpacity = 0;
        this->mHorzSpeed = 0;
        Stop();
        this->mRelocateTimer = 120;
        func_02012694(0x153, &this->mCamSpacePos);
        return;
    }
    if (this->mAttackTimer != 0)
        return;
    this->mState = 4;
    this->mSpeed.y = 0;
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

    s32 scale;

    this->mState = 7;
    this->mOpacity = 0;
    this->mTargetOpacity = this->mOpacity;
    this->mClsnScale = 0x1000;
    this->mHitPoints = 3;

    scale = this->mClsnScale;
    this->mScale.x = scale;
    this->mScale.y = scale;
    this->mScale.z = scale;
    this->mdCcAcPos_c.radius = this->mClsnRadius * this->mClsnScale;
    this->mdCcAcPos_c.height = this->mClsnHeight * this->mClsnScale;
    this->mBooFlags.visible = 1;
    this->mdCcAcPos_c.flags |= 1;
    this->mBooFlags.mirrored = 1;
    this->mPos.x = -this->mHomePos.x;
    this->mAngle.y += 0x8000;
    this->mPrevAngle.y += 0x8000;
    this->mSubState = 0;
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
            int r = Sound::PlaySecretSound(this, (&this->mSoundCount));
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
    if (mStateTimer > 100)
        mState = 1;
}


void daTrs_c::func_ov063_02118b98() {

    u16 v;

    this->mBooFlags.hitByParam3Player = 0;
    Stop();

    v = this->mStateTimer;
    if (v > 15)
        goto bigblock;
    if (v != 15)
        return;

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

    this->mPos.x = this->mHomePos.x;
    this->mPos.y = this->mHomePos.y;
    this->mPos.z = this->mHomePos.z;
    return;

bigblock:
    if (this->mDeathMode != 0) goto special;
    if (v <= 60)
        return;
    if (this->mDistToPlayer >= 0x258000) return;
    {
        dActor_c *r1 = 0;
        for (;;) {
            r1 = dActor_c::FindWithActorID(TRS_ID_KAIDAN, r1);
            if (r1 == 0) goto after_strb;
            if ((r1->param1 >> 8 & 3) == 0)
                break;
        }
        ((daTrsTrap_c *)r1)->mTriggered = 1;
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

    if (this->mStateTimer == 0) {
        unsigned int v = *(unsigned short *)&this->mBooFlags;
        if ((v << 25 >> 31) == 0) {
            this->mHitPoints--;
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
        this->mPrevAngle.x = 0;
        this->mPrevAngle.y = 0;
        this->mPrevAngle.z = 0;
        *p |= TRS_FLAG_DEFEATED;
        return;
    }
    if (this->mStateTimer == 0) {
        this->mClsnScale -= 0x255;
    }
    if (UpdateDuringNonlethalHit(0x28) != 0) {
        this->mState = 1;
    }
}


void daTrs_c::func_ov063_02118ddc() {

  struct Vector3 v;
  v.x = this->mPos.x;
  int y = this->mPos.y;
  v.y = y;
  v.z = this->mPos.z;
  v.y = y + 0x64000;
  int s = this->mPos.x;
  if(s < 0) s = -s;
  v.x = s;
  dActor_c::Spawn(TRS_ID_OBJ_KEY, 3, v, (0), this->mAreaIdx, -1);
  func_02012694(0xbb, &this->mCamSpacePos);
}


void daTrs_c::func_ov063_02118e5c() {

    func_ov063_02118eac();
    unsigned int id = this->mCachedActorID;
    if (id == 0) return;
    void *actor = dActor_c::FindWithID(id);
    this->mFoundActor = (dActor_c *)actor;
    void *p = this->mFoundActor;
    if (p != 0) {
        ((daTrsTrap_c *)p)->mState = 1;
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

    v.x = this->mPos.x;
    v.y = this->mPos.y;
    v.z = this->mPos.z;
    this->mPos.y = this->mPos.y + 0x64000;
    if (this->mSpawnedActorID == 0) {
        return;
    }
    id = this->mSpawnedActorID;
    p = dActor_c::FindWithID(id);
    this->mFoundActor = (dActor_c *)p;
    if (this->mFoundActor != 0) {
        (static_cast<daTrsIcon_c *>(this->mFoundActor))->func_ov063_02118f24(v);
    }
    this->mFoundActor = 0;
}

void daTrsIcon_c::func_ov063_02118f24(const Vector3 &pos) {
    unsigned int val = 4;
    this->UntrackAndSpawnStar(this->mTrackStarID, this->mStarID, pos, val);
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
        /* the Vector3 temporary's stack home is the frame's 12 spare bytes */
        this->mScale = Vector3(scale, scale, scale);

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
                this->mPrevAngle.y = Vec3_HorzAngle(&this->mPos, &((dActor_c *)r)->mPos);
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

    r4 = this->mTalkPlayer;
    if (r4 == 0 || this->mVariant != 2) {
        KillAndTrackInDeathTable();
        func_0201267c(0xd5, &this->mCamSpacePos);
        if (((this->mCapId) & 0xf) >= 6)
            return;
        
        {
            void *cap;
            this->mPos.x = this->mHomePos.x;
            this->mPos.y = this->mHomePos.y;
            this->mPos.z = this->mHomePos.z;
            this->mAreaId = this->mAreaIdx;
            this->mPrevAngle.x = this->mHomeAngleX;
            this->mPrevAngle.y = this->mHomeAngleY;
            this->mPrevAngle.z = this->mHomeAngleZ;
            /* Kept (#821 r83): mAngle = mPrevAngle and the three member copies
               each come out a word short of the ROM. */
            s16 *src = &this->mPrevAngle.x;
            this->mAngle.x = src[0];
            this->mAngle.y = src[1];
            this->mAngle.z = src[2];

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
        if (((daPly_c *)r4)->StartTalk(*this, 1) == 0)
            return;
        this->mSubState++;
        return;

    case 1:
    {
        void *found;
        this->mFoundActor = dActor_c::FindWithID(this->mBigBooID);
        found = this->mFoundActor;
        if (found != 0) {
            s32 *cnt = &((daTrs_c *)found)->mBooDefeatCount;
            *cnt = *cnt + 1;
        }
        found = this->mFoundActor;
        if (found != 0 && ((daTrs_c *)found)->mBooDefeatCount == 5) {
            ((daPly_c *)r4)->ShowMessage(*this, 0xb5, &this->mPos, 0, 2);
        } else {
            ((daPly_c *)r4)->ShowMessage(*this, 0xb4, &this->mPos, 0, 2);
        }
        this->mFoundActor = 0;
        this->mSubState++;
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
            found = this->mFoundActor;
            if (found != 0 && ((daTrs_c *)found)->mBooDefeatCount == 5) {
                ((daTrs_c *)found)->func_ov063_02116244();
                this->mSoundCount++;
            }
        }
        this->mSubState++;
        return;

    case 3:
        if (this->mSoundCount != 0) {
            if (Sound::PlaySecretSound(this, (&this->mSoundCount)) == 0)
                return;
            this->MarkForDestruction();
            if (((this->mCapId) & 0xf) >= 6)
                return;
            
        {
            void *cap2;
            this->mPos.x = this->mHomePos.x;
            this->mPos.y = this->mHomePos.y;
            this->mPos.z = this->mHomePos.z;
            this->mAreaId = this->mAreaIdx;
            this->mPrevAngle.x = this->mHomeAngleX;
            this->mPrevAngle.y = this->mHomeAngleY;
            this->mPrevAngle.z = this->mHomeAngleZ;
            s16 *src = &this->mPrevAngle.x;
            this->mAngle.x = src[0];
            this->mAngle.y = src[1];
            this->mAngle.z = src[2];

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
            this->mPos.x = this->mHomePos.x;
            this->mPos.y = this->mHomePos.y;
            this->mPos.z = this->mHomePos.z;
            this->mAreaId = this->mAreaIdx;
            this->mPrevAngle.x = this->mHomeAngleX;
            this->mPrevAngle.y = this->mHomeAngleY;
            this->mPrevAngle.z = this->mHomeAngleZ;
            s16 *src = &this->mPrevAngle.x;
            this->mAngle.x = src[0];
            this->mAngle.y = src[1];
            this->mAngle.z = src[2];

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
        this->mPos.x = this->mHomePos.x;
        this->mPos.y = this->mHomePos.y;
        this->mPos.z = this->mHomePos.z;
        this->mAreaId = this->mAreaIdx;
        this->mPrevAngle.x = this->mHomeAngleX;
        this->mPrevAngle.y = this->mHomeAngleY;
        this->mPrevAngle.z = this->mHomeAngleZ;
        /* Kept (#821 r83): mAngle = mPrevAngle and the three member copies
           each come out a word short of the ROM. */
        s16 *src = &this->mPrevAngle.x;
        this->mAngle.x = src[0];
        this->mAngle.y = src[1];
        this->mAngle.z = src[2];
        daTrs_c *r = (daTrs_c *)RespawnIfHasCap();
        if (r == 0) return;
        {
            u16 *p = (u16 *)&r->mBooFlags;
            *p &= ~TRS_FLAG_CARRYING;
        }
    } else {
        this->mState = 5;
        this->mSubState = 0;
        {
            this->mdCcAcPos_c.flags |= 1;
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
    if (this->mStateTimer == 0) {
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
    if (this->mStateTimer == 0) {
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
    if (this->mStateTimer >= 30) {
        this->mState = 1;
    } else {
        this->mSpeed.y = 0;
        this->mHorzSpeed = 0x7ccc;
        Oscillate(0);
    }
    HandleAttackStatus();
}

void daTrs_c::UpdateCarriedItemPos() {

    if (!this->mBooFlags.carrying)
        return;

    this->mCarriedPos = this->mPos;

    if (this->mCarriedID == TRS_ID_T_BASKET) {
        this->mCarriedPos.y += 0x3c000;
        return;
    }
    this->mCarriedPos.y += 0xa000;
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
    void *a = dActor_c::FindWithActorID(id, 0);
    this->mFoundActor = (dActor_c *)a;
    a = this->mFoundActor;
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
        if (this->mStateTimer >= rnd + 0x5a) {
            int rnd2 =
                (int)((unsigned)RandomIntInternal(&data_0209e650.value) >> 16 & 0x3fff) - 0x2000;
            this->mTargetAngleY = this->mPrevAngle.y + rnd2;
            this->mStateTimer = 0;
        }
    }

    ApproachLinear(this->mPrevAngle.y, this->mTargetAngleY, a2);
    this->mSpeed.y = 0;
    SetVelFromPlayerVel(0xa000 - this->mSpeedJitter, a3);

    if (this->mHorzSpeed != 0) {
        Oscillate(0);
    } else {
        func_ov063_0211a960();
    }
    return;

reset:
    this->mHorzSpeed = 0;
    this->mSpeed.y = 0;
    func_ov063_0211a960();
}

void daTrs_c::ChaseMario(int a1, short a2, int a3) {

    int thresh;
    short angle;

    if (VanishOrAppear() != 0) {
        if (this->mBooFlags.mirrored) {
        } else {
            if (this->mAttackTimer != 0) {
                this->mAttackTimer--;
            }
        }
        if (this->mNextRelocateTimer != 0) {
            this->mNextRelocateTimer--;
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

        ApproachLinear(this->mPrevAngle.y, angle, a2);
        this->mSpeed.y = 0;

        {
            daPly_c *p = this->mClosestPlayer;
            if (p->mIsAirborne == 0) {
                int myY = this->mPos.y;
                int otherY = p->mPos.y;
                int dy = myY - otherY;
                if ((a1 << 0xc) < dy && dy < 0x1f4000 && (this->mHomePos.y - myY) < 0xfa000) {
                    this->mSpeed.y = this->func_ov063_0211a0a8(myY, otherY, 0xa000, 0x2000);
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
    this->mSpeed.y = 0;
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

int daTrs_c::func_ov063_0211a0a8(int a1, int a2, int a3, int a4) {
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
    Vector3 v1, v2;

    id = this->mdCcAcPos_c.otherOwner;
    if (id != 0) {
        if (this->mdCcAcPos_c.hitFlags & 0x207e0) {
            void *found;

            this->mdCcAcPos_c.flags |= 1;
            found = (void *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);
            if (found) {
                this->mTalkPlayer = (daPly_c *)found;

                {
                    int isBf = (int)(((dActor_c *)found)->actorID == TRS_ID_PLAYER);
                    if (isBf) {
                        if (((dActor_c *)found)->param1 == 3) {
                            u16 *p = (u16 *)&this->mBooFlags;
                            *p |= TRS_FLAG_HIT_BY_PARAM3_PLAYER;
                        }
                    }
                }
            }
            return 1;
        }

        actor = dActor_c::FindWithID(id);
        if (actor != 0) {
            if (this->mdCcAcPos_c.hitFlags & 0x400000) {
                player = static_cast<daPly_c *>(actor);
                if (player->mIsVanish != 0)
                    return 0;

                if (player->mIsMetal != 0) {
                    this->mdCcAcPos_c.flags |= 1;
                    this->mTalkPlayer =
                        (daPly_c *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);
                    return 1;
                }

                if (this->JumpedOnByPlayer(this->mdCcAcPos_c, *player) != 0) {
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

                        v1 = this->mPos;
                        player->Hurt(v1, this->mHurtDamage, 0xc000, 1, 0, 1);
                        func_0201267c(0x149, &this->mCamSpacePos);
                        return -1;
                    }
                }

                do {
                    if ((this->mdCcAcPos_c.hitFlags & 0x10) != 0) {
                        u8 kind = this->mVariant;
                        if (kind < 2 || kind >= 6 || kind <= 9) {
                            daPly_c *found2;
                            Vector3_16 spin;

                            this->mdCcAcPos_c.flags |= 1;
                            found2 = (daPly_c *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);
                            if (found2) {
                                this->mTalkPlayer =
                                    (daPly_c *)dActor_c::FindWithID(this->mdCcAcPos_c.otherOwner);

                                spin.x = -0x2000;
                                spin.y = 0;
                                spin.z = 0;
                                KillByInvincibleChar(spin, *found2, 0);

                                this->mAngle.y += 0x8000;
                                return 1;
                            }
                            break;
                        }
                    }
                    v2 = this->mPos;
                    player->Hurt(v2, this->mHurtDamage, 0xc000, 1, 0, 1);
                    func_0201267c(0x149, &this->mCamSpacePos);
                    return -1;
                } while (0);
            }
        }
    }
    return 0;
}

int daTrs_c::UpdateDuringDeath() {

    Vector3 v, w;

    if (this->mStateTimer == 0) {
        this->mHorzSpeed = 0x28000;
        this->mPrevAngle.y = this->mClosestPlayer->mAngle.y;
        this->mChildDeaths = 1;
        this->mBooFlags.followHeading = 0;
        goto fall;
    }

    if (this->mStateTimer == 5)
        this->mTargetOpacity = 8;

    if (this->mStateTimer <= 30) {
        if (this->mWithMeshClsn.IsOnWall() == 0)
            goto fall;
    }

    if (this->mVariant == 0xf) {
        int x = this->mPos.x;
        v.x = x;
        v.y = this->mPos.y;
        v.z = this->mPos.z;
        if (this->mBooFlags.mirrored)
            v.x = x * (u32)-1;
        w = v;
        this->PoofDustAt(w);
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
    this->mSpeed.y = 0x5000;
    this->mAngle.z += 0x800;
    this->mAngle.y += 0x800;
    return 0;
}

int daTrs_c::UpdateDuringNonlethalHit(int arg1) {
    Stop();
    if (mStateTimer == 0)
        SetMoveYawForDuringHit(1);
    {
        u16 t = mStateTimer;
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
    if (mStateTimer == 0)
        SetMoveYawForDuringHit(0);
    t = mStateTimer;
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

    this->mPrevAngle.y = this->mPreKnockbackAngleY;
    this->mBooFlags.followHeading = 1;
}

void daTrs_c::ShakeAfterHit() {
    mAngle.y += (FX_CosIdx((u16)(s16)((mStateTimer - 31) << 13) >> 4) << 10) / 0x1000;
}

void daTrs_c::MoveDuringHit(int cond, int val) {
    s16 ang = (mStateTimer + 1) << 11;

    mHorzSpeed = val;
    mSpeed.y = FX_CosIdx((u16)ang >> 4);
    mPrevAngle.y = mKnockbackAngleY;
    if (cond == 0)
        return;

    mAngle.y += data_ov063_0211e7e0[mStateTimer];
    mAngle.z += data_ov063_0211e7e0[mStateTimer];
}

void daTrs_c::SetMoveYawForDuringHit(int cond) {
    mdCcAcPos_c.flags |= 1;
    mBooFlags.followHeading = 0;
    mPreKnockbackAngleY = mPrevAngle.y;
    if (cond != 0) {
        mKnockbackAngleY = mClosestPlayer->mAngle.y;
        return;
    }
    s16 base = mPrevAngle.y;
    if (FX_CosIdx((u16)(s16)(base - mAngleToPlayer) >> 4) < 0)
        mKnockbackAngleY = base;
    else
        mKnockbackAngleY = base + 0x8000;
}

int daTrs_c::VanishOrAppear() {

    int r6 = GetSubtraction(this->mAngleToPlayer, this->mPrevAngle.y);
    int r0 = GetSubtraction(this->mPrevAngle.y, this->mClosestPlayer->mAngle.y);
    int ret = 0;
    this->mSpeed.y = 0;
    if (r6 > 0x1568 || r0 < 0x6b58) {
        if (this->mOpacity == 0x28) {
            this->mTargetOpacity = 0xff;
            if (this->mVariant != 0xf)
                func_0201267c(0xf8, &this->mCamSpacePos);
            this->mChaseDelay = 30;
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
    mScale.x = mClsnScale + (FX_SinIdx(mWobblePhase >> 4) << 3) / 100;
    mScale.y = mClsnScale + -(FX_SinIdx(mWobblePhase >> 4) << 3) / 100;
    mScale.z = mScale.x;
    func_ov063_0211a960();
    mdCcAcPos_c.radius = mClsnRadius * mScale.x;
    mdCcAcPos_c.height = mClsnHeight * mScale.y;
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
                this->mOpacity += 0x14;
            }
        } else {
            if (cur - 0x14 > tgt) {
                this->mOpacity -= 0x14;
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
    this->mScale.x = v;
    this->mScale.y = v;
    this->mScale.z = v;
    this->mdCcAcPos_c.radius = this->mClsnRadius * v;
    this->mdCcAcPos_c.height = this->mClsnHeight * v;
}

/* Behavior handler for variant 7: spawns three TERESA (param 0xfff6, random
   yaw) at fixed offsets and removes itself; below 15 stars it only removes
   itself. */
void daTrs_c::SpawnCourtyardBooTriplet() {

    Vector3 pos;
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
        pos = tmp;
        rot.x = this->mPrevAngle.x;
        rot.y = this->mPrevAngle.y;
        rot.z = this->mPrevAngle.z;
        rot.y = (u32)RandomIntInternal(&data_0209e650.value) >> 16;
        dActor_c::Spawn(TRS_ID_TERESA, 0xfff6, pos, &rot, this->mAreaIdx, -1);
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
    this->mSpeed.y = 0;
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
            cnt = &((daTrs_c *)mFoundActor)->mChildDeaths;
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
            mCarriedModel.Render(&mCarriedScale);
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
    Vector3_16 *prev;
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
            ((dActor_c *)q)->mPos.x = mPos.x;
            ((dActor_c *)q)->mPos.y = mPos.y;
            ((dActor_c *)q)->mPos.z = mPos.z;
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
                    mPos.x = mHomePos.x;
                    mPos.y = mHomePos.y;
                    mPos.z = mHomePos.z;
                    mAreaId = mAreaIdx;
                    mPrevAngle.x = mHomeAngleX;
                    mPrevAngle.y = mHomeAngleY;
                    mPrevAngle.z = mHomeAngleZ;
                    {
                        s16(*ap)[3] = (s16(*)[3]) & this->mPrevAngle.x;
                        mAngle.x = (*ap)[0];
                        mAngle.y = (*ap)[1];
                        mAngle.z = (*ap)[2];
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
                this->mdCcAcPos_c.flags = this->mdCcAcPos_c.flags | 1;
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
                prev = &mPrevAngle;
                mPos.x = mHomePos.x;
                fp = (u16 *)&this->mBooFlags;
                mPos.y = mHomePos.y;
                mPos.z = mHomePos.z;
                mPrevAngle.x = mHomeAngleX;
                mPrevAngle.y = mHomeAngleY;
                mPrevAngle.z = mHomeAngleZ;
                mAngle.x = prev->x;
                mAngle.y = prev->y;
                mAngle.z = prev->z;
                *fp = (*fp) | TRS_FLAG_CAN_APPEAR;
                mState = 0;
                mHorzSpeed = 0;
                mVertAccel = 0;
                mSpeed.y = 0;
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
            ve = mCapPos;
            if (GetCapEatenOffIt(ve) != 0)
                return 1;
        }
        mVertAccel = -0x2000;
        UpdateCarriedItemPos();
        if (mEatenByYoshi != 0) {
            if (mSpitOutTimer == 0) {
                mEatenByYoshi = 0;
                mSpitOutTimer = 0;
                mVertAccel = 0;
                mSpeed.y = 0;
                goto block_39;
            }
        }
        v = (((mFlags) & 0x40000) ? 1 : 0);
        if (v != 0) {
            u8 st = mTalkStep;
            daPly_c *pl = mEatingPlayer;
            switch (st) {
            case 0:
                if (pl->ShowMessage(*this, 0x15a, 0, 0, 2) != 0) {
                    bp = &this->mTalkStep;
                    *bp = (*bp) + 1;
                    func_0201267c(0xf8, &mCamSpacePos);
                }
                break;
            case 1:
                if (pl->GetTalkState() == -1) {
                    pl->DropActor();
                    bp = &this->mTalkStep;
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
            s32 *pp = &plr->mPos.x;
            pv.x = pp[0];
            pv.y = pp[1];
            pv.z = pp[2];
            mAngleToPlayer = Vec3_HorzAngle(&mPos, &pv);
            mDistToPlayer = Vec3_HorzDist(&mPos, &pv);
        } else {
            this->mAngleToPlayer = mAngle.y;
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
        mAngle.y = mPrevAngle.y;
    mStateTimer += 1;
    DecIfAbove0_Short(&mChaseDelay);
    if (mPrevState != mState)
        mStateTimer = 0;
    if (mVariant != 3) {
        UpdatePos(&mdCcAcPos_c);
        UpdateCarriedItemPos();
        if ((mBooFlags.limitZ != 0) && (mPos.z < -0x12c000))
            mPos.z = -0x12c000;
        dBgCh_Gnd rc1;
        y = mPos.y;
        z = mPos.z;
        w = y + 0x32000;
        x = mPos.x;
        v1.x = x;
        v1.y = w;
        v1.z = z;
        rc1.SetObjAndPos(v1, this);
        if (rc1.DetectClsn() != 0) {
            s32 ground = rc1.clsnY + 0x2000;
            if (mPos.y < ground)
                mPos.y = ground;
        }
        if ((mVariant != 4) && (mVariant != 0xb)) {
            dBgCh_Gnd rc2;
            y = mPos.y;
            z = mPos.z;
            w = y + 0x32000;
            x = mPos.x;
            v2.x = x;
            v2.y = w;
            v2.z = z;
            rc2.SetObjAndPos(v2, this);
            d1 = (int)(actorID == TRS_ID_TERESA);
            if (d1 != 0) {
                if (mVariant < 8) {
                    if (mBooFlags.groundSeen != 0) {
                        if ((rc2.DetectClsn() == 0) || ((mPos.y - rc2.clsnY) > 0x12c000)) {
                            mPos = mLastGroundPos;
                        } else {
                            mLastGroundPos = mPos;
                        }
                    } else {
                        goto ray_e;
                    }
                } else {
                    goto ray_e;
                }
            } else {
            ray_e:
                if ((rc2.DetectClsn() != 0) && ((mPos.y - rc2.clsnY) < 0x12c000)) {
                    fp = (u16 *)&this->mBooFlags;
                    *fp = (*fp) | TRS_FLAG_GROUND_SEEN;
                    mLastGroundPos = mPos;
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
        secretDone = Sound::PlaySecretSound(this, &mSoundTimer);

    if (mWithMeshClsn.JustHitGround()) {
        int vertSpeed = mSpeed.y;
        mSpeed.y = (-vertSpeed) >> 1;
        LandingDust(false);
    } else if (mWithMeshClsn.IsOnGround()) {
        onGround = 1;
        if (secretDone != 0 || (u16)mSoundTimer > 60) {
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
        int z = mPos.z;
        int y = mPos.y;
        int x = mPos.x;
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

/* InitResources drops param1's cap nibble with a read through this
   accessor. A plain `param1 &= 0xfff` lets mwccarm CSE the field address
   (addlt r2,r4,#8 + [r2], 0x9a4 of 0x9a0); the cartridge stores to
   [r4,#8] directly. */
inline u32 daTrs_c::GetParam1() const {
    return param1;
}

int daTrs_c::InitResources()
{
    int cond;
    int tmp598;
    dActor_c *spawned;
    daPly_c *pp;

    /* Kept (#821 r76): BooFlags16 cleared as its halfword; no block-copy or type lever (r72-r75)
       applies to this scalar pin. Kept (#821 r80): getter tried -> no read to route through
       one; `= BooFlags16()`, a `{0}` or `{}` local and a static const zero copy are all
       0x9ac of 0x9a0. */
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
        if (gCurrentSublevel == 0xc && mPos.x == 0xbb8000 && mAreaId == 2) {
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
    mCapPos.x = 0;
    mCapPos.y = 0x60000;
    mCapPos.z = 0;
    cond = 0;
    if (actorID == TRS_ID_TERESA) cond = 1;
    if (cond && mVariant != 8) {
        unsigned char capIdx;
        mHadBank1Cap = (param1 >> 0xc) & 0xf;
        capIdx = (param1 >> 8) & 0xf;
        AddCap(capIdx);
        if ((mCapId & 7) < 6) {
            param1 = GetParam1() & 0xfff;
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

    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;
    mHomeAngleX = mAngle.x;
    mHomeAngleY = mAngle.y;
    mHomeAngleZ = mAngle.z;
    mStateTimer = 0;
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    mOpacity = 0xff;
    mWithMeshClsn.StartIgnoringVanishSurfaces();
    mState = 0;
    unk_5b2 = mPrevAngle.y;
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
    mCarriedScale.x = 0xc00;
    mCarriedScale.y = 0xc00;
    mCarriedScale.z = 0xc00;

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
            if (pp != 0 && pp->mPos.z > (int)0xffaec000) {
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
            if (pp != 0 && pp->mPos.z > (int)0xffaec000) {
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
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    MTX_Concat43(this->mAnmModel.data.transforms, &data_020a0e68, &data_020a0e68);
    mClsnBase.x = data_020a0e68.t.x;
    mClsnBase.y = data_020a0e68.t.y;
    mClsnBase.z = data_020a0e68.t.z;
    VEC_Subtract(&mClsnBase, &mPos, &mClsnBase);
    mTalkStep = 0;
    mTargetAngleY = mPrevAngle.y;
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

daTrsIcon_c *daTrsIcon_c_classInit() {
    return new daTrsIcon_c();
}

daTBasket_c *daTBasket_c_classInit() {
    return new daTBasket_c();
}

daTrs_c *daTrs_c_classInit_BOSS_TERESA() {
    return new daTrs_c();
}

daTrs_c *daTrs_c_classInit_TERESA() {
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
