//cpp
/* daPkn_c (PAKUN / PIRANHA_PLANT 250), ov084 0x0212eaf0..0x02130174.
 *
 * RTTI names daPkn_c. Typeinfo at 0x02130bf8 points to the string at
 * 0x02130bec; the vtable header at 0x02130c24 points to that typeinfo.
 * The base is dEnemyBase_c. This vtable's 31 slots end at 0x02130ca4; seven
 * zero words follow. The section boundary alone does not establish its extent.
 *
 * Factory is `return new daPkn_c()`, through the inherited
 * fBase_c::operator new. Definitions run in reverse address order for
 * mwccarm 2004/b56. The inline destructor in the header emits the required
 * D1 and D0 pair; `new` odr-uses the class so that pair is emitted here.
 *
 * Known limits:
 * - ModelAnim::SetAnim, dCcAc_c::Init, dCcAcPos_c::Init, dBgCh_Actr::Init,
 *   Particle::System::New and NewSimple, Sound::PlaySub, Player::Hurt and
 *   Player::Bounce are called by their mangled names. Each symbol carries a
 *   Fix12<int> by value. The measured SetAnim and Init alternatives are
 *   noted at their declarations.
 * - Six helpers keep C linkage under their address names, because
 *   include/decl_common.h declares them that way.
 * - func_ov084_0212ec60 walks PknMtx43 and PknVec3 views: a Vector3 or
 *   Matrix4x3 copy is emitted differently.
 * - The shared files and the state table at 0x02130e80 keep their address
 *   names (text-only TU; the static initializer owns the .bss).
 *
 * Boundary, layout and compiler experiments are recorded in
 * notes/data/class-facts/daPkn_c.json and
 * notes/agents/handoffs/pr-2450-source-review-fixes.md.
 */

#include "common.h"
#include "daPkn_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"

/* The six shared files this actor claims.  include/SharedFilePtr.h declares no
   fields on purpose -- the files that shadow it disagree about the layout -- so
   this TU takes its own two-word view and casts to SharedFilePtr wherever it
   calls a method.  Every member below that reads a loaded handle reads it out
   of `.file`, which is the second word, so one spelling serves all of them. */
struct PknSharedFile { int id; void *file; };
extern PknSharedFile data_ov084_02130dfc;   /* the plant's own model */
extern PknSharedFile data_ov002_0210da38;   /* BUBBLE_MODEL_PTR -- sleep bubble, shared out of ov002 */
extern PknSharedFile data_ov084_02130df4;   /* idle animation */
extern PknSharedFile data_ov084_02130e0c;   /* recoil animation */
extern PknSharedFile data_ov084_02130e14;   /* lunge animation */
extern PknSharedFile data_ov084_02130e24;   /* death animation */

/* Six animation handles, held indirectly: the table is pointers, not objects. */
extern SharedFilePtr *data_ov084_021302f4[];

/* func_ov084_0212ec60's own view of the shared scratch matrix and of the
   Vector3 fields it walks.  The tags are uniquified because the file scope this
   body now shares with the real headers already has a Vector3 and a Matrix4x3,
   and the byte match is in this spelling. */
struct PknVec3 { int x, y, z; };
struct PknMtx43 { int w[12]; };

/* Behavior calls the current state through this actor's member-pointer table
   in .bss at 0x02130e80, which __sinit_ov084_02130654 fills from the .data
   records at 0x02130ba4. */
typedef void (daPkn_c::*PknStatePMF)();

int ApproachLinear(short &value, short target, short step);

/* C-linkage declarations for the remaining raw entry points and shared data. */
extern "C" {

/* Local helper declarations. */
void  func_ov084_0212ebb4(daPkn_c *c);
void  func_ov084_0212ec60(daPkn_c *c);
int   func_ov084_0212ef00(daPkn_c *self);
int   func_ov084_0212f1d0(daPkn_c *c);
void  func_ov084_0212f204(daPkn_c *c);

/* -- the state table __sinit_ov084_02130654 fills in -- */
extern PknStatePMF data_ov084_02130e80[];

/* -- other modules -- */
void  LoadBlueCoinModel(void *c);
void  UnloadBlueCoinModel(void *c);
int   func_0201267c(int a, void *b);
void  func_02012694(u32 id, void *pos);
void  func_020105cc(void *thiz, u32 flags);

void  _ZN6Player6BounceE5Fix12IiE(void *player, int fix);
int   _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);

/* Measured remaining call seams in InitResources (mwccarm 2004/b56): real
   SetAnim and dCcAc/dCcAcPos Init calls with local Fix12<int> arguments change
   the instructions.  dBgCh_Actr's current scalar Init declaration instead names
   an unresolved symbol.  These results do not establish the original Fix12
   parameter types or a different caller ABI.  Exact alternatives and results:
   notes/agents/handoffs/pr-2450-source-review-fixes.md, compiler experiments. */
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *anim, int flags, int speed, u32 startFrame);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *a, int r, int h, u32 e, u32 g);
void  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, const Vector3 *v, int r, int h, u32 e, u32 g);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *a, int b, int c, void *d, void *e);

/* -- shared tables -- */
extern s16 data_02082214[];              /* the sin/cos table, two shorts a step */
extern int data_ov084_0213030c[];        /* per-frame particle X/Z offset (0212f460, 0xfb) */
extern int data_ov084_02130334[];        /* per-frame particle Y offset */
extern u8  data_ov084_021302ec[];        /* five indices {0,3,4,5,6}; 0212ec60 sums s16 at [i]*0x34+0x1c into ang[1] (dead -- later y-adjust reads ang[0]==0, so the offset is constant 0x32000). 0x34 is not BMD_Bone (0x40). */
extern PknMtx43 data_020a0e68;           /* the shared scratch matrix */

int   Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
void  Vec3_Asr(PknVec3 *d, PknVec3 *s, int sh);
void  Vec3_LslInPlace(PknVec3 *v, int sh);
void  SubVec3(PknVec3 *a, PknVec3 *b, PknVec3 *c);
void  Matrix4x3_FromTranslation(PknMtx43 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationY(PknMtx43 *m, s16 ang);
void  MulMat4x3Mat4x3(void *a, void *b, void *c);

void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 handle, u32 id, int x, int y, int z, const void *f, void *cb);
void  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
int   _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int t, int flag);

}

/* ==========================================================================
 *
 * Allocates sizeof(daPkn_c) (0x47c) through fBase_c::operator new and
 * inlines the implicit constructor: dEnemyBase_c C2, vptr store, then the
 * six member C1s in declaration order.
 * ======================================================================== */

// @symbol daPkn_c_classInit
extern "C" daPkn_c *daPkn_c_classInit()
{
    return new daPkn_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c13InitResourcesEv
/*
 * Unlike daFPkn_c's, this one checks its loads: a failed SetFile on either
 * model returns 0.
 *
 * The tail seeds the sleep-bubble position: 0xe0 along the facing angle out
 * of the shared sin and cos table at data_02082214, and 0x37800 above mPosY.
 * func_ov084_0212ec60 rewrites mBubblePos every frame.
 */
int daPkn_c::InitResources()
{
    int i;
    Vector3 v;
    for (i = 0; i < 6; i++)
        Animation::LoadFile(*data_ov084_021302f4[i]);
    LoadBlueCoinModel(this);
    Model::LoadFile(*(SharedFilePtr *)&data_ov084_02130dfc);
    Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210da38);
    if (mModelAnim.SetFile((BMD_File *)data_ov084_02130dfc.file, 1, -1) == 0)
        return 0;
    if (mModel.SetFile((BMD_File *)data_ov002_0210da38.file, 1, -1) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c1, this, 0x32000, 0x64000, 0x200004, 0x423e0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c2, this, 0x82000, 0x64000, 2, 0x423e0);
    v.x = 0; v.y = 0; v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x46000, 0x64000, 0x200002, 0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mState = 0;
    mTargetAngleY = mPrevAngleY;
    mPlayerDist = 0x7fffffff;
    mClosestPlayer = 0;
    mClsnEnabled = 0;
    mSubSoundFadedOut = 1;
    mStateTimer = 0;
    unk_108 = 3;
    mPlayerAirborne = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
    mHeadClsnOffset.x = mPosX;
    mHeadClsnOffset.y = mPosY;
    mHeadClsnOffset.z = mPosZ;
    {
        s16 *tbl = data_02082214;
        /* Copied through a pointer: `mHomePos = mHeadClsnOffset` emits a
           different copy. */
        Vector3 *home = &mHeadClsnOffset;
        mHomePos = *home;
        /* The shift must be LOGICAL so the angle wraps -- on the signed s16 it
           would index the wrong table entry for negative angles. */
        int ang = (u16)mAngleY >> 4;
        /* Temporaries in the ROM's load order; folding them reorders it. */
        int y0 = mPosY;
        int z0 = mPosZ;
        int cosv = tbl[(ang << 1) + 1];
        int sinv = tbl[ang << 1];
        int z = cosv * 0xe0 + z0;
        int y = y0 + 0x37800;
        int x = sinv * 0xe0 + mPosX;
        mBubblePos.x = x;
        mBubblePos.y = y;
        mBubblePos.z = z;
    }
    mParticleHandle2 = 0;
    mParticleHandle = mParticleHandle2;
    mLoopSoundHandle = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c8BehaviorEv
int daPkn_c::Behavior()
{
    int r;
    int old;
    int cur;
    r = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 1);
    if (r != 0) {
        if (r == 2) {
            unk_108 = 0;
            mState = 7;
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mPosX = mHomePos.x;
            mPosY = mHomePos.y;
            mPosZ = mHomePos.z;
        }
        return 1;
    }
    MakeVanishLuigiWork(mdCcAc_c1);
    mModelAnim.Advance();
    func_ov084_0212f204(this);
    old = mState;
    (this->*data_ov084_02130e80[old])();
    {
        /* mStateTimer is s16; the ROM counts it as a u16 (ldrh, not ldrsh). */
        u16 *timer = (u16 *)&mStateTimer;
        *timer = *timer + 1;
    }
    cur = mState;
    if (old != cur) {
        if (cur == 5) {
            mFlags &= ~0x10000000;
        }
        mStateTimer = 0;
        mLoopSoundHandle = 0;
    }
    func_ov084_0212ec60(this);
    mdCcAc_c1.Clear();
    mdCcAc_c2.Clear();
    mdCcAcPos_c.Clear();
    if (mClsnEnabled != 0) {
        mdCcAc_c1.Update();
        mdCcAc_c2.Update();
        if (mState == 2) {
            mdCcAcPos_c.SetPosRelativeToActor(mHeadClsnOffset);
            mdCcAcPos_c.Update();
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c6RenderEv
/*
 * The bubble is drawn only while the idle animation (data_ov084_02130df4)
 * is playing.
 */
int daPkn_c::Render()
{
    if (mScaleX == 0)
        return 1;
    mModelAnim.Render((Vector3 *)&mScaleX);
    if ((int)mModelAnim.file == (int)data_ov084_02130df4.file)
        mModel.Render(&mBubbleScale);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16OnPendingDestroyEv
/*
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daPkn_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16CleanupResourcesEv
int daPkn_c::CleanupResources()
{
    int i;
    ((SharedFilePtr *)&data_ov084_02130dfc)->Release();
    for (i = 0; i < 6; i++)
        data_ov084_021302f4[i]->Release();
    UnloadBlueCoinModel(this);
    ((SharedFilePtr *)&data_ov002_0210da38)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c9StateWaitEv
void daPkn_c::StateWait()
{
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mClsnEnabled = 1;
    mFlags |= 0x10000000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    if (mPlayerDist < 0x4b0000)
        mState = 1;
    func_ov084_0212ef00(this);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c10StateSleepEv
void daPkn_c::StateSleep()
{
    mClsnEnabled = 1;
    if (mModelAnim.Finished() || mModelAnim.WillHitFrame(0))
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    int thr = (mPlayerAirborne != 0) ? 0x12c000 : 0x190000;
    if (mPlayerDist < thr && func_ov084_0212f1d0(this)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e0c.file, 0x40000000, 0x1000, 0);
        mState = 3;
        func_02012694(0x175, &mCamSpacePosX);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfe, mBubblePos.x, mBubblePos.y, mBubblePos.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xff, mBubblePos.x, mBubblePos.y, mBubblePos.z);
        return;
    }
    mLoopSoundHandle = Sound::PlayLong(mLoopSoundHandle, 3, 0x174, *(Vector3 *)&mCamSpacePosX, 0);
    if (mPlayerDist < 0x352000) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0, 0x7f, 0x1451, 0);
        mSubSoundFadedOut = 0;
    } else if (mSubSoundFadedOut == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0x1451, 0))
            mSubSoundFadedOut = 1;
    }
    func_ov084_0212ef00(this);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c9StateBiteEv
void daPkn_c::StateBite()
{
    dActor_c *actor;
    u16 type;
    int isPlayer;
    int isNine;
    u32 flags;
    Vector3 pos1;
    Vector3 pos2;

    /* isPlayer and isNine hold the compares as ints: testing
       `type == 0xbf` directly changes the code. */
    mClsnEnabled = 1;

    if (mModelAnim.WillHitFrame(0x10)
        || mModelAnim.WillHitFrame(0x20)
        || mModelAnim.WillHitFrame(0x34)
        || mModelAnim.WillHitFrame(0x4b)) {
        func_0201267c(0xc0, &mCamSpacePosX);
    }

    ApproachLinear(mPrevAngleY, mTargetAngleY, 0x800);
    mAngleY = mPrevAngleY;

    if (mModelAnim.Finished() != 0) {
        int thr;
        if (mPlayerAirborne != 0)
            thr = 0x12c000;
        else
            thr = 0x190000;
        thr = thr + 0x64000;
        if (mPlayerDist > thr)
            mState = 4;
        else
            mModelAnim.currFrame = 0;
    }

    /* Body cylinder. */
    if (mdCcAc_c1.otherOwner != 0) {
        actor = dActor_c::FindWithID(mdCcAc_c1.otherOwner);
        if (actor != 0) {
            type = actor->actorID;
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                flags = mdCcAc_c1.hitFlags;
                if (flags & 0x10) {
                    ((Player *)actor)->IncMegaKillCount();
                    func_02012694(0x1d, &mCamSpacePosX);
                    func_ov084_0212ebb4(this);
                    return;
                }
                if (((Player *)actor)->mIsMetal != 0) {
                    func_ov084_0212ebb4(this);
                    return;
                }
                if (((Player *)actor)->mIsVanish == 0) {
                    pos1.x = mPosX;
                    pos1.y = mPosY;
                    pos1.z = mPosZ;
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos1, 3, 0xc000, 1, 0, 1);
                    return;
                }
                if ((flags & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(this);
                return;
            }
            if ((mdCcAc_c1.hitFlags & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(this);
                    return;
                }
            }
        }
    }

    /* Second body cylinder. Its mega test reads mdCcAc_c1's hit flags. */
    if (mdCcAc_c2.otherOwner != 0) {
        actor = dActor_c::FindWithID(mdCcAc_c2.otherOwner);
        if (actor != 0) {
            type = actor->actorID;
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                if ((mdCcAc_c1.hitFlags & 0x10) != 0) {
                    ((Player *)actor)->IncMegaKillCount();
                    func_02012694(0x1d, &mCamSpacePosX);
                    func_ov084_0212ebb4(this);
                    return;
                }
                if (((Player *)actor)->mIsMetal != 0) {
                    func_ov084_0212ebb4(this);
                    return;
                }
                if ((mdCcAc_c2.hitFlags & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(this);
                return;
            }
            if ((mdCcAc_c2.hitFlags & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(this);
                    return;
                }
            }
        }
    }

    /* Head cylinder, players only. */
    if (mdCcAcPos_c.otherOwner == 0)
        return;
    actor = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (actor == 0)
        return;
    type = actor->actorID;
    isPlayer = (int)(type == 0xbf);
    if (isPlayer == 0)
        return;
    if ((mdCcAcPos_c.hitFlags & 0x10) != 0) {
        ((Player *)actor)->IncMegaKillCount();
        func_02012694(0x1d, &mCamSpacePosX);
        func_ov084_0212ebb4(this);
        return;
    }
    if (((Player *)actor)->mIsMetal != 0)
        return;
    pos2.x = mPosX;
    pos2.y = mPosY;
    pos2.z = mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos2, 3, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c9StateWakeEv
void daPkn_c::StateWake()
{
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    if (func_ov084_0212ef00(this) != 0) return;
    /* mStateTimer is s16; the ROM compares it as a u16 (ldrh). */
    if ((u16)mStateTimer <= 0xb) return;
    if (!mModelAnim.Finished()) return;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    mState = 2;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c9StateDozeEv
void daPkn_c::StateDoze()
{
    mClsnEnabled = 1;
    if (mModelAnim.Finished()) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e0c.file, 0x40000000, 0x1000, 0);
        mState = 1;
    } else {
        int thresh = (mPlayerAirborne != 0) ? 0x12c000 : 0x190000;
        if (mPlayerDist < thresh) {
            if (func_ov084_0212f1d0(this) != 0) {
                mModelAnim.currFrame = 0;
                mState = 2;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c8StateDieEv
void daPkn_c::StateDie()
{
    Vector3 pos;

    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    mClsnEnabled = 0;
    if ((u16)mStateTimer < 0xa) {
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        pos.x += data_ov084_0213030c[(u16)mStateTimer] * data_02082214[((u16)mAngleY >> 4) * 2];
        pos.z += data_ov084_0213030c[(u16)mStateTimer] * data_02082214[((u16)mAngleY >> 4) * 2 + 1];
        pos.y += data_ov084_02130334[(u16)mStateTimer] << 12;
        mParticleHandle = (s32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleHandle, 0xfb, pos.x, pos.y, pos.z, 0, 0);
    }
    if (mModelAnim.Finished() == 0)
        return;
    mState = 6;
    mParticleHandle2 = 0;
    mParticleHandle = mParticleHandle2;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c11StateShrinkEv
void daPkn_c::StateShrink()
{
    int scale;
    Vector3 v;

    mClsnEnabled = 0;
    scale = mScaleX;
    if ((u16)mStateTimer == 0) {
        func_02012694(0x11f, &mCamSpacePosX);
        scale = 0x1000;
    }

    mParticleHandle = (s32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle, 0xfc, mPosX,
        mPosY + 0x1e000, mPosZ, 0, 0);
    mParticleHandle2 = (s32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle2, 0xfd, mPosX,
        mPosY + 0x1e000, mPosZ, 0, 0);

    if (scale > 0) {
        scale = scale - 0xa3;
    } else {
        scale = 0;
        if (unk_108 != 0) {
            /* Loading y and z ahead of the aggregate keeps the ROM's order. */
            int y = mPosY;
            int z = mPosZ;
            v = (Vector3){mPosX, y + 0x78000, z};
            dActor_c::Spawn(0x122, 0, v, 0, mAreaId, -1);
            unk_108 = 0;
        }
        mState = 7;
    }
    mScaleX = scale;
    mScaleY = scale;
    mScaleZ = scale;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c9StateGoneEv
void daPkn_c::StateGone()
{
    mClsnEnabled = 0;
    if (mPlayerDist <= 0x4b0000) return;
    mState = 8;
    mParticleHandle2 = 0;
    mParticleHandle = mParticleHandle2;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e0c.file, 0, 0x1000, 0);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c11StateRegrowEv
void daPkn_c::StateRegrow()
{
    int v;
    mClsnEnabled = 0;
    v = mScaleX;
    if ((u16)mStateTimer == 0)
        v = 0x4cc;
    if (v < 0x1000) {
        v += 0x52;
    } else {
        mState = 0;
        v = 0x1000;
    }
    mScaleX = v;
    mScaleY = v;
    mScaleZ = v;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f204
extern "C" {  /* include/decl_common.h declares it with C linkage */
/* Tracks the closest player: its distance, the yaw toward it and whether it
   is airborne. */
void func_ov084_0212f204(daPkn_c *c)
{
    Vector3 v;
    c->mClosestPlayer = c->ClosestPlayer();
    Player *p = c->mClosestPlayer;
    if (p != 0) {
        /* Member by member: `v = *pos` emits a different copy. */
        Vector3 *pos = (Vector3 *)&p->mPosX;
        v.x = pos->x;
        v.y = pos->y;
        v.z = pos->z;
        c->mPlayerDist = Vec3_Dist((Vector3 *)&c->mPosX, &v);
        c->mTargetAngleY = Vec3_HorzAngle((Vector3 *)&c->mPosX, &v);
        c->mPlayerAirborne = c->mClosestPlayer->mIsAirborne;
    } else {
        c->mPlayerDist = 0x7fffffff;
        c->mTargetAngleY = c->mAngleY;
    }
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f1d0
extern "C" {  /* include/decl_common.h declares it with C linkage */
/* Is the closest player moving fast enough to wake the plant? */
int func_ov084_0212f1d0(daPkn_c *c)
{
    Player *p = c->mClosestPlayer;
    if (p == 0) return 0;
    if (p->mVertSpeed > 0xa000) return 1;
    return p->mHorzSpeed > 0xa000;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ef00
extern "C" {  /* include/decl_common.h declares it with C linkage */
/* The collision check the idle states share: returns 1 when a hit killed
   the plant. t and u hold the actor-type compares as ints; testing
   `type == 0xbf` directly changes the code. */
int func_ov084_0212ef00(daPkn_c *pkn)
{
    dActor_c *actor;
    u32 flags;
    u16 type;
    u32 id;

    id = pkn->mdCcAc_c1.otherOwner;
    if (id != 0) {
        actor = dActor_c::FindWithID(id);
        if (actor != 0) {
            type = actor->actorID;
            int t = (int)(type == 0xbf);
            if (t != 0) {
                flags = pkn->mdCcAc_c1.hitFlags & 0x26ff0;
                if (flags != 0) {
                    if (flags & 0x10) {
                        ((Player *)actor)->IncMegaKillCount();
                        func_02012694(0x1d, &pkn->mCamSpacePosX);
                    }
                    func_020105cc(pkn, flags);
                    func_ov084_0212ebb4(pkn);
                } else if (pkn->JumpedOnByPlayer(pkn->mdCcAc_c1, *(Player *)actor) != 0) {
                    _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
                    func_ov084_0212ebb4(pkn);
                } else if (pkn->mdCcAc_c1.hitFlags & 0x40000) {
                    func_ov084_0212ebb4(pkn);
                } else {
                    Vector3 v;
                    v.x = pkn->mPosX;
                    v.y = pkn->mPosY;
                    v.z = pkn->mPosZ;
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &v, 3, 0xc000, 1, 0, 1);
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&pkn->mModelAnim, data_ov084_02130e0c.file, 0, 0x1000, 0);
                    pkn->mModelAnim.SetFlags(0x40000000);
                    pkn->mState = 3;
                    func_02012694(0x175, &pkn->mCamSpacePosX);
                    return 0;
                }
                return 1;
            } else if (pkn->mdCcAc_c1.hitFlags & 0x2000) {
                int u = (int)(type == 9);
                if (u != 0) {
                    func_ov084_0212ebb4(pkn);
                    return 1;
                }
            }
        }
    }

    id = pkn->mdCcAc_c2.otherOwner;
    if (id != 0) {
        actor = dActor_c::FindWithID(id);
        if (actor != 0) {
            type = actor->actorID;
            int t = (int)(type == 0xbf);
            if (t != 0) {
                flags = pkn->mdCcAc_c2.hitFlags & 0x26ff0;
                if (flags != 0) {
                    if (flags & 0x10) {
                        ((Player *)actor)->IncMegaKillCount();
                        func_02012694(0x1d, &pkn->mCamSpacePosX);
                    }
                    func_020105cc(pkn, flags);
                    func_ov084_0212ebb4(pkn);
                    return 1;
                }
                if (pkn->JumpedOnByPlayer(pkn->mdCcAc_c1, *(Player *)actor) != 0) {
                    _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
                    func_ov084_0212ebb4(pkn);
                    return 1;
                }
                if (pkn->mdCcAc_c2.hitFlags & 0x40000) {
                    func_ov084_0212ebb4(pkn);
                    return 1;
                }
            } else if (pkn->mdCcAc_c2.hitFlags & 0x2000) {
                int u = (int)(type == 9);
                if (u != 0) {
                    func_ov084_0212ebb4(pkn);
                    return 1;
                }
            }
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ec60
extern "C" {  /* include/decl_common.h declares it with C linkage */
/* Poses both models and places the head cylinder and the sleep bubble. The
   bone sum into ang[1] is never read. */
void func_ov084_0212ec60(daPkn_c *self)
{
    volatile s16 ang[3];
    struct { PknMtx43 saved; PknVec3 tv; PknVec3 v; } L;
    int lr;
    int scale;

    Vec3_Asr(&L.v, (PknVec3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, L.v.x, L.v.y, L.v.z);
    L.saved = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->mAngleY);
    *(PknMtx43 *)&self->mModelAnim.mat4x3 = data_020a0e68;

    ang[0] = 0;
    ang[1] = 0;
    ang[2] = 0;
    for (lr = 0; lr < 5; lr++) {
        ang[1] = (s16)(ang[1] + *(s16*)((char *)self->mModelAnim.data.bones + data_ov084_021302ec[lr] * 0x34 + 0x1c));
    }

    self->mHeadClsnOffset.x = 0;
    self->mHeadClsnOffset.y = 0;
    self->mHeadClsnOffset.z = 0;
    data_020a0e68 = L.saved;
    MulMat4x3Mat4x3(&self->mModelAnim.data.transforms[6], &data_020a0e68, &data_020a0e68);
    self->mHeadClsnOffset.x = data_020a0e68.w[9];
    self->mHeadClsnOffset.y = data_020a0e68.w[10];
    self->mHeadClsnOffset.z = data_020a0e68.w[11];
    Vec3_LslInPlace((PknVec3 *)&self->mHeadClsnOffset, 3);
    SubVec3((PknVec3 *)&self->mHeadClsnOffset, (PknVec3 *)&self->mPosX, (PknVec3 *)&self->mHeadClsnOffset);

    self->mHeadClsnOffset.y -= data_02082214[((u16)ang[0] >> 4) << 1] * (s16)0x14 + 0x32000;
    self->mHeadClsnOffset.z += 0x32000;

    L.tv.x = self->mPosX;
    L.tv.y = self->mPosY;
    L.tv.z = self->mPosZ;

    {
        int rr = (int)((u32)(self->mModelAnim.currFrame << 4) >> 0x10);
        int d = rr - 0x28;
        if (d < 0)
            d = -d;
        scale = ((d << 12) / 10) + 0x400;
        if (scale < 0x800)
            scale = 0x800;
        self->mBubbleScale.x = scale;
        self->mBubbleScale.y = scale;
        self->mBubbleScale.z = scale;
    }

    L.tv.x = data_02082214[((u16)self->mAngleY >> 4) << 1] * (s16)0xe0 + L.tv.x;
    scale = scale - 0xc00;
    L.tv.z = data_02082214[(((u16)self->mAngleY >> 4) << 1) + 1] * (s16)0xe0 + L.tv.z;
    L.tv.y = L.tv.y + (scale * 0x18 + 0x38000);

    self->mBubblePos.x = L.tv.x;
    self->mBubblePos.y = L.tv.y;
    self->mBubblePos.z = L.tv.z;

    Matrix4x3_FromTranslation((PknMtx43 *)&self->mModel.mat4x3, L.tv.x >> 3, L.tv.y >> 3, L.tv.z >> 3);
}
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16OnAimedAtWithEggEv
/* Vtable slot 29. */
s32 daPkn_c::OnAimedAtWithEgg()
{
    return 0x46000;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ec04
extern "C" {  /* include/decl_common.h declares it with C linkage */
/* Enters state 2 with the lunge animation at `frame`. */
void func_ov084_0212ec04(daPkn_c *c, int frame)
{
    c->mState = 2;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    c->mModelAnim.currFrame = (u16)frame << 12;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ebb4
extern "C" {  /* include/decl_common.h declares it with C linkage */
/* Kills the plant: death animation and state 5. */
void func_ov084_0212ebb4(daPkn_c *c)
{
    func_0201267c(0xc1, &c->mCamSpacePosX);
    c->mState = 5;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e24.file, 0x40000000, 0x1000, 0);
}
}
