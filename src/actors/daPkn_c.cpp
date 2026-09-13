//cpp
/* daPkn_c (PAKUN / PIRANHA_PLANT 250), ov084 0x0212eaf0..0x02130174.
 * deslop
 *
 * RTTI names daPkn_c. Typeinfo at 0x02130bf8 points to the string at
 * 0x02130bec; the vtable header at 0x02130c24 points to that typeinfo.
 * The base is dEnemyBase_c. This vtable's 31 slots end at 0x02130ca4; seven
 * zero words follow. The section boundary alone does not establish its extent.
 *
 * Factory is `return new daPkn_c()`. Leaf operator new(unsigned long) forwards
 * `_ZN7fBase_cnwEj` until #2570. Definitions run in reverse address order for
 * mwccarm 2004/b56. The inline destructor in the header emits the required
 * D1/D0 pair; `new` odr-uses the class so that pair is emitted here.
 *
 * Leftover:
 * - ModelAnim::SetAnim / dCcAc_c::Init / dCcAcPos_c::Init stay mangled
 *   (Fix12<int> by value, wall 6az -- InitResources and the state helpers)
 * - dBgCh_Actr::Init stays mangled: header Fix12i mangles as i; ROM is
 *   Fix12<int> (InitResources)
 * - Particle::System::New / NewSimple, Sound::PlaySub / PlayLong, Player::Hurt
 *   / Bounce stay mangled (Fix12<int> by value, 6az)
 * - Animation::Finished / WillHitFrame keep +0x160 (ModelAnim MI +0x50)
 * - (int)this + 0x440 in InitResources (named `mHomePos = mHeadClsnOffset` DIFFs)
 * - mStateTimer unsigned halfword increment (ldrh; ++mStateTimer is ldrsh)
 * - func_ov084_0212f204 / 0212f460 named fields size-DIFF (keep offset soup)
 * - PknMtx43 / PknVec3 POD copies (Vector3 / nested Matrix4x3 scalarize)
 * - data_ov084_* SharedFilePtr handles and the PMF table at 0x02130e80
 *   (text-only TU; sinit-owned BSS)
 *
 * Boundary, layout and compiler experiments are recorded in
 * notes/data/class-facts/daPkn_c.json and
 * notes/agents/handoffs/pr-2450-source-review-fixes.md.
 */

#include "common.h"
#include "daPkn_c.h"
#include "decl_Animation.h"
#include "SharedFilePtr.h"

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
   in .bss at 0x02130e80.  The free handler identities and the unowned PMF
   records stay unchanged until their coordinated method conversion. */
typedef void (daPkn_c::*PknStatePMF)();

#define AT(p, off) ((void *)(int)((char *)(p) + (off)))

/* C-linkage declarations for the remaining raw entry points and shared data. */
extern "C" {

/* Local helper declarations. */
void  func_ov084_0212ebb4(daPkn_c *c);
void  func_ov084_0212ec60(daPkn_c *c);
int   func_ov084_0212ef00(daPkn_c *self);
int   func_ov084_0212f1d0(daPkn_c *c);
void  func_ov084_0212f204(char *r4);
void  func_ov084_0212f33c(daPkn_c *self);
void  func_ov084_0212f460(void *self);

/* -- the state table __sinit_ov084_02130654 fills in -- */
extern PknStatePMF data_ov084_02130e80[];

/* -- other modules -- */
void  LoadBlueCoinModel(void *c);
void  UnloadBlueCoinModel(void *c);
int   func_0201267c(int a, void *b);
void  func_02012694(u32 id, void *pos);
void  func_020105cc(void *thiz, u32 flags);

void *_ZN8dActor_c10FindWithIDEj(u32 id);
char *_ZN8dActor_c13ClosestPlayerEv(void);
int   _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *clsn, void *player);
void  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3 *c, const void *d, int e, int f);

void  _ZN6Player16IncMegaKillCountEv(void *player);
void  _ZN6Player6BounceE5Fix12IiE(void *player, int fix);
int   _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);


int   _ZN9Animation8FinishedEv(void *a);

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

void  _Z14ApproachLinearRsss(s16 *val, s16 target, s16 step);
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
int   _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 b, u32 c, void *pos, u32 d);

}

/* ==========================================================================
 *
 * Allocates sizeof(daPkn_c) (0x47c) through the leaf operator new and
 * inlines the implicit constructor: dEnemyBase_c C2, vptr store, then the
 * six member C1s in declaration order.
 * ======================================================================== */

// @symbol daPkn_c_classInit
extern "C" daPkn_c *daPkn_c_classInit()
{
    return new daPkn_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c13InitResourcesEv
/*
 * This was still `extern "C" int _ZN7daPkn_c13InitResourcesEv(char* c)`
 * working raw offsets -- a file renamed .cpp without ever being migrated. It is
 * a real method now, with the remaining copy seam measured below.
 *
 * Unlike FirePiranhaPlantBig's, this one DOES check its loads: a failed SetFile
 * on either model returns 0 rather than carrying on.
 *
 * The tail seeds the sleep-bubble position: 0xe0 along the facing angle out
 * of the shared sin/cos table at data_02082214, and 0x37800 above mPosY.
 * 0212ec60 rewrites mBubblePos every frame. PIRANHA_PLANT (250) does not
 * breathe fire -- that is daFPkn_c.
 *
 * The remaining SetAnim/Init bridges are described at their declarations.
 * Their measured alternatives are recorded in the continuation handoff;
 * the general callee-homing note does not prove a caller ABI restriction.
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
    unk_464 = 0x7fffffff;
    unk_460 = 0;
    mClsnEnabled = 0;
    unk_45d = 1;
    mStateTimer = 0;
    unk_108 = 3;
    unk_46c = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
    mHeadClsnOffset.x = mPosX;
    mHeadClsnOffset.y = mPosY;
    mHeadClsnOffset.z = mPosZ;
    {
        s16 *tbl = data_02082214;
        /* The add sits INSIDE the integer cast, which is load-bearing here:
           not interchangeable with `&mHeadClsnOffset`. */
        Vector3* home = (Vector3*)(((int)this + 0x440));
        mHomePos = *home;
        /* The shift must be LOGICAL so the angle wraps -- on the signed s16 it
           would index the wrong table entry for negative angles. */
        unsigned short angh = *(unsigned short*)&mAngleY;
        int ang = angh >> 4;
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
    unk_474 = 0;
    mParticleHandle = unk_474;
    unk_478 = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
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
    func_ov084_0212f204((char *)this);
    old = mState;
    (this->*data_ov084_02130e80[old])();
    {
        /* This counter loads the shared signed field as an unsigned halfword.
           ++mStateTimer instead emits ldrsh where the ROM uses ldrh. */
        unsigned short* p100 = (unsigned short*)((char*)&mStateTimer);
        *p100 = (unsigned short)(*p100 + 1);
    }
    cur = mState;
    if (old != cur) {
        if (cur == 5) {
            mFlags &= ~0x10000000;
        }
        mStateTimer = 0;
        unk_478 = 0;
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c6RenderEv
/*
 * The six-slot `struct Obj` this file used to cast both models to was their own
 * vtable, and the slot it called is Render -- ModelAnim's for the plant, Model's
 * for the sleep bubble (BUBBLE_MODEL_PTR).
 *
 * `mModelAnim.file` is a BCA_File * where the raw field was an int, so the
 * comparison against the second word of data_ov084_02130df4 needs the cast the
 * offset spelling did not.
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16OnPendingDestroyEv
/*
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daPkn_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16CleanupResourcesEv
int daPkn_c::CleanupResources()
{
  int i;
  ((SharedFilePtr *)&data_ov084_02130dfc)->Release();
  for(i=0;i<6;i++){
    data_ov084_021302f4[i]->Release();
  }
  UnloadBlueCoinModel(((void*)this));
  ((SharedFilePtr *)&data_ov002_0210da38)->Release();
  return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212fc10
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212fc10(daPkn_c *c)
{
    c->mScaleX = 0x1000;
    c->mScaleY = 0x1000;
    c->mScaleZ = 0x1000;
    c->mClsnEnabled = 1;
    c->mFlags |= 0x10000000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    if (c->unk_464 < 0x4b0000)
        c->mState = 1;
    func_ov084_0212ef00(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212fa7c
/* daPkn state 1: the PMF record at ov084:0x02130ba4 targets this body.
   The former FirePiranhaPlantBig_Kill / daFPkn_c::Kill comments were a
   misattribution: this is not a vtable slot.  Its original name is unknown. */
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212fa7c(daPkn_c *c) {
    c->mClsnEnabled = 1;
    if (_ZN9Animation8FinishedEv((char *)c + 0x160) || _ZNK9Animation12WillHitFrameEi((char *)c + 0x160, 0)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    }
    {
        int thr = (c->unk_46c != 0) ? 0x12c000 : 0x190000;
        if (c->unk_464 < thr && func_ov084_0212f1d0(c)) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e0c.file, 0x40000000, 0x1000, 0);
            c->mState = 3;
            func_02012694(0x175, &c->mCamSpacePosX);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfe, c->mBubblePos.x, c->mBubblePos.y, c->mBubblePos.z);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xff, c->mBubblePos.x, c->mBubblePos.y, c->mBubblePos.z);
            return;
        }
    }
    c->unk_478 = _ZN5Sound8PlayLongEjjjRK7Vector3s(c->unk_478, 3, 0x174, &c->mCamSpacePosX, 0);
    if (c->unk_464 < 0x352000) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0, 0x7f, 0x1451, 0);
        c->unk_45d = 0;
    } else if (c->unk_45d == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0x1451, 0))
            c->unk_45d = 1;
    }
    func_ov084_0212ef00(c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f6d8
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212f6d8(daPkn_c *self)
{
    char *c = (char *)self;
    void *actor;
    u16 type;
    int isPlayer;
    int isNine;
    u32 flags;
    struct Vector3 pos1;
    struct Vector3 pos2;

    *(u8 *)(c + 0x45c) = 1;

    if (_ZNK9Animation12WillHitFrameEi(c + 0x160, 0x10)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x20)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x34)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x4b)) {
        func_0201267c(0xc0, c + 0x74);
    }

    _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x468), 0x800);
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);

    if (_ZN9Animation8FinishedEv(c + 0x160) != 0) {
        int thr;
        if (*(int *)(c + 0x46c) != 0)
            thr = 0x12c000;
        else
            thr = 0x190000;
        thr = thr + 0x64000;
        if (*(int *)(c + 0x464) > thr)
            *(int *)(c + 0x458) = 4;
        else
            *(int *)(c + 0x168) = 0;
    }

    /* cylinder 0 (id @ +0x3a4, flags @ +0x3a0) */
    if (*(u32 *)(c + 0x3a4) != 0) {
        actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x3a4));
        if (actor != 0) {
            type = *(u16 *)((char *)actor + 0xc);
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                flags = *(u32 *)(c + 0x3a0);
                if (flags & 0x10) {
                    _ZN6Player16IncMegaKillCountEv(actor);
                    func_02012694(0x1d, c + 0x74);
                    func_ov084_0212ebb4(self);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6f9) != 0) {
                    func_ov084_0212ebb4(self);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6fb) == 0) {
                    pos1.x = *(int *)(c + 0x5c);
                    pos1.y = *(int *)(c + 0x60);
                    pos1.z = *(int *)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos1, 3, 0xc000, 1, 0, 1);
                    return;
                }
                if ((flags & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(self);
                return;
            }
            if ((*(u32 *)(c + 0x3a0) & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(self);
                    return;
                }
            }
        }
    }

    /* cylinder 1 (id @ +0x3d8, flags @ +0x3d4; mega bit read from +0x3a0 per ROM) */
    if (*(u32 *)(c + 0x3d8) != 0) {
        actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x3d8));
        if (actor != 0) {
            type = *(u16 *)((char *)actor + 0xc);
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                if ((*(u32 *)(c + 0x3a0) & 0x10) != 0) {
                    _ZN6Player16IncMegaKillCountEv(actor);
                    func_02012694(0x1d, c + 0x74);
                    func_ov084_0212ebb4(self);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6f9) != 0) {
                    func_ov084_0212ebb4(self);
                    return;
                }
                if ((*(u32 *)(c + 0x3d4) & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(self);
                return;
            }
            if ((*(u32 *)(c + 0x3d4) & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(self);
                    return;
                }
            }
        }
    }

    /* cylinder 2 (id @ +0x40c, flags @ +0x408) — player only */
    if (*(u32 *)(c + 0x40c) == 0)
        return;
    actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x40c));
    if (actor == 0)
        return;
    type = *(u16 *)((char *)actor + 0xc);
    isPlayer = (int)(type == 0xbf);
    if (isPlayer == 0)
        return;
    if ((*(u32 *)(c + 0x408) & 0x10) != 0) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, c + 0x74);
        func_ov084_0212ebb4(self);
        return;
    }
    if (*(u8 *)((char *)actor + 0x6f9) != 0)
        return;
    pos2.x = *(int *)(c + 0x5c);
    pos2.y = *(int *)(c + 0x60);
    pos2.z = *(int *)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos2, 3, 0xc000, 1, 0, 1);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f630
extern "C" void func_ov084_0212f630(daPkn_c *c)
{
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    if (func_ov084_0212ef00(c) != 0) return;
    /* mStateTimer is signed; the ROM compares it as unsigned ldrh. */
    if ((unsigned short)c->mStateTimer <= 0xb) return;
    if (!_ZN9Animation8FinishedEv((char *)c + 0x160)) return;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    c->mState = 2;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f588
extern "C" void func_ov084_0212f588(daPkn_c *c)
{
    c->mClsnEnabled = 1;
    if (_ZN9Animation8FinishedEv((char *)c + 0x160)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e0c.file, 0x40000000, 0x1000, 0);
        c->mState = 1;
    } else {
        int thresh = (c->unk_46c != 0) ? 0x12c000 : 0x190000;
        if (c->unk_464 < thresh) {
            if (func_ov084_0212f1d0(c) != 0) {
                *(int *)((char *)c + 0x168) = 0;
                c->mState = 2;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f460
/* Named mStateTimer / mPos* / mAngleY / mParticleHandle size-DIFF this body. */
void func_ov084_0212f460(void *self)
{
    char *c = (char *)self;
    int space[3];
    int zero;

    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    /* `fp` is assigned BEFORE `zero`, and the test reads through it. Under 2004/b56
       the callee-saved registers are handed out in assignment order, so this is what
       puts fp in r7 and zero in r6 as the ROM has them; with `zero = 0` first they come
       out swapped and nothing else moves. The ROM emits `mov r6,#0; strb` before
       `add r7,r4,#0x100` regardless -- the store is independent, so it schedules ahead
       of the address computation that feeds the compare. */
    u16 *fp = (u16 *)(c + 0x100);
    zero = 0;
    *(unsigned char *)(c + 0x45c) = zero;
    if (*fp < 0xa) {
        int x = *(int *)(c + 0x5c);
        s16 *sintbl = data_02082214;
        *(volatile int *)&space[0] = x;
        int y = *(int *)(c + 0x60);
        int *xtbl = data_ov084_0213030c;
        *(volatile int *)&space[1] = y;
        int z = *(int *)(c + 0x64);
        int *ytbl = data_ov084_02130334;
        *(volatile int *)&space[2] = z;

        x = xtbl[*fp] * sintbl[(*(u16 *)(c + 0x8e) >> 4) * 2] + x;
        *(volatile int *)&space[0] = x;
        z = xtbl[*fp] * sintbl[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1] + z;
        *(volatile int *)&space[2] = z;
        y = y + (ytbl[*fp] << 12);
        *(volatile int *)&space[1] = y;

        u32 slot = *(u32 *)(c + 0x470);
        int px = space[0];
        int py = space[1];
        *(void **)(c + 0x470) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            slot, 0xfb, px, py, z, 0, 0);
    }
    if (_ZN9Animation8FinishedEv(c + 0x160) == 0)
        return;
    *(int *)(c + 0x458) = 6;
    *(int *)(c + 0x474) = 0;
    *(void **)(c + 0x470) = *(void **)(c + 0x474);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f33c
void func_ov084_0212f33c(daPkn_c *c)
{
    int r4;
    struct Vector3 v;

    c->mClsnEnabled = 0;
    r4 = c->mScaleX;
    if ((unsigned short)c->mStateTimer == 0) {
        func_02012694(0x11f, &c->mCamSpacePosX);
        r4 = 0x1000;
    }

    c->mParticleHandle = (s32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        (unsigned int)c->mParticleHandle, 0xfc, c->mPosX,
        c->mPosY + 0x1e000, c->mPosZ, 0, 0);
    c->unk_474 = (s32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        (unsigned int)c->unk_474, 0xfd, c->mPosX,
        c->mPosY + 0x1e000, c->mPosZ, 0, 0);

    if (r4 > 0) {
        r4 = r4 - 0xa3;
    } else {
        r4 = 0;
        if (c->unk_108 != 0) {
            int y = c->mPosY;
            int z = c->mPosZ;
            v = (struct Vector3){c->mPosX, y + 0x78000, z};
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x122, r4, &v, (void*)r4, c->mAreaId, -1);
            c->unk_108 = r4;
        }
        c->mState = 7;
    }
    c->mScaleX = r4;
    c->mScaleY = r4;
    c->mScaleZ = r4;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f2dc
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212f2dc(daPkn_c *c){
    c->mClsnEnabled = 0;
    if (c->unk_464 <= 0x4b0000) return;
    c->mState = 8;
    c->unk_474 = 0;
    c->mParticleHandle = c->unk_474;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e0c.file, 0, 0x1000, 0);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f298
/* State 8 is reached through the void PMF table and has no result consumer.
   The previous int declaration fell through when this body became C++. */
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212f298(daPkn_c *c)
{
    int v;
    c->mClsnEnabled = 0;
    v = c->mScaleX;
    if ((u16)c->mStateTimer == 0)
        v = 0x4cc;
    if (v < 0x1000)
    {
        v += 0x52;
    }
    else
    {
        c->mState = 0;
        v = 0x1000;
    }
    c->mScaleX = v;
    c->mScaleY = v;
    c->mScaleZ = v;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f204
extern "C" {  /* Retained C-linkage helper. */
/* Named unk_460 / mPosX / mTargetAngleY / mAngleY size-DIFF this body. */
void func_ov084_0212f204(char* r4){
  struct Vector3 v;
  *(char**)(r4 + 0x460) = _ZN8dActor_c13ClosestPlayerEv();
  {
    char* p = *(char**)(r4 + 0x460);
    if (p != 0) {
      struct Vector3* pp = (struct Vector3*)(((int)p + 0x5c));
      v.x = pp->x;
      v.y = pp->y;
      v.z = pp->z;
      *(int*)(r4 + 0x464) = Vec3_Dist((struct Vector3*)(r4 + 0x5c), &v);
      *(short*)(r4 + 0x468) = Vec3_HorzAngle((struct Vector3*)(r4 + 0x5c), &v);
      *(int*)(r4 + 0x46c) = *(unsigned char*)(*(char**)(r4 + 0x460) + 0x6de);
    } else {
      *(int*)(r4 + 0x464) = 0x7fffffff;
      *(short*)(r4 + 0x468) = *(short*)(r4 + 0x8e);
    }
  }
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f1d0
extern "C" {  /* Retained C-linkage helper. */
int func_ov084_0212f1d0(daPkn_c *c) {
    char *p = (char *)c->unk_460;
    if (p == 0) return 0;
    /* Player mVertSpeed at +0xa8 / mHorzSpeed at +0x98. */
    if (*(int*)(p + 0xa8) > 0xa000) return 1;
    return *(int*)(p + 0x98) > 0xa000;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ef00
extern "C" {  /* Retained C-linkage helper. */
int func_ov084_0212ef00(daPkn_c *pkn)
{
    char *self = (char *)pkn;
    void *actor;
    u32 flags;
    u32 raw;
    u16 type;
    u32 id;

    id = *(u32*)(self + 0x3a4);
    if (id == 0) goto second;

    actor = _ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto second;

    type = *(u16*)((char*)actor + 0xc);
    {
        int t = (int)(type == 0xbf);
        if (t == 0) goto checkA2;
    }

    raw = *(u32*)(self + 0x3a0);
    flags = raw & 0x26ff0;
    if (flags == 0) goto jumpA;

    if (flags & 0x10) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, self + 0x74);
    }
    func_020105cc(self, flags);
    func_ov084_0212ebb4(pkn);
    goto successA;

jumpA:
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(self, self + 0x380, actor) != 0) {
        _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
        func_ov084_0212ebb4(pkn);
        goto successA;
    }

    if (*(u32*)(self + 0x3a0) & 0x40000) {
        func_ov084_0212ebb4(pkn);
        goto successA;
    }

    {
        int v[3];
        v[0] = *(int*)(self + 0x5c);
        v[1] = *(int*)(self + 0x60);
        v[2] = *(int*)(self + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, v, 3, 0xc000, 1, 0, 1);
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x110, data_ov084_02130e0c.file, 0, 0x1000, 0);
    _ZN9Animation8SetFlagsEi(self + 0x160, 0x40000000);
    *(int*)(self + 0x458) = 3;
    func_02012694(0x175, self + 0x74);
    return 0;

successA:
    return 1;

checkA2:
    if (!(*(u32*)(self + 0x3a0) & 0x2000)) goto second;
    {
        int u = (int)(type == 9);
        if (u == 0) goto second;
    }
    func_ov084_0212ebb4(pkn);
    return 1;

second:
    id = *(u32*)(self + 0x3d8);
    if (id == 0) goto fail;

    actor = _ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto fail;

    type = *(u16*)((char*)actor + 0xc);
    {
        int t = (int)(type == 0xbf);
        if (t == 0) goto checkB2;
    }

    raw = *(u32*)(self + 0x3d4);
    flags = raw & 0x26ff0;
    if (flags == 0) goto jumpB;

    if (flags & 0x10) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, self + 0x74);
    }
    func_020105cc(self, flags);
    func_ov084_0212ebb4(pkn);
    return 1;

jumpB:
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(self, self + 0x380, actor) != 0) {
        _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
        func_ov084_0212ebb4(pkn);
        return 1;
    }

    if (*(u32*)(self + 0x3d4) & 0x40000) {
        func_ov084_0212ebb4(pkn);
        return 1;
    }

    goto fail;

checkB2:
    if (!(*(u32*)(self + 0x3d4) & 0x2000)) goto fail;
    {
        int u = (int)(type == 9);
        if (u == 0) goto fail;
    }
    func_ov084_0212ebb4(pkn);
    return 1;

fail:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ec60
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212ec60(daPkn_c *self)
{
    char *c = (char *)self;
    volatile s16 ang[3];
    struct { PknMtx43 saved; PknVec3 tv; PknVec3 v; } L;
    int lr;
    int r3;

    Vec3_Asr(&L.v, (PknVec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, L.v.x, L.v.y, L.v.z);
    L.saved = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
    *(PknMtx43*)(c + 0x12c) = data_020a0e68;

    ang[0] = 0;
    ang[1] = 0;
    ang[2] = 0;
    for (lr = 0; lr < 5; lr++) {
        ang[1] = (s16)(ang[1] + *(s16*)(*(char**)(c + 0x120) + data_ov084_021302ec[lr] * 0x34 + 0x1c));
    }

    *(s32*)(c + 0x440) = 0;
    *(s32*)(c + 0x444) = 0;
    *(s32*)(c + 0x448) = 0;
    data_020a0e68 = L.saved;
    MulMat4x3Mat4x3((char*)*(void**)(c + 0x124) + 0x120, &data_020a0e68, &data_020a0e68);
    *(s32*)(c + 0x440) = data_020a0e68.w[9];
    *(s32*)(c + 0x444) = data_020a0e68.w[10];
    *(s32*)(c + 0x448) = data_020a0e68.w[11];
    Vec3_LslInPlace((PknVec3*)(c + 0x440), 3);
    SubVec3((PknVec3*)(c + 0x440), (PknVec3*)(c + 0x5c), (PknVec3*)(c + 0x440));

    *(s32*)AT(c, 0x444) -= data_02082214[((u16)ang[0] >> 4) << 1] * (s16)0x14 + 0x32000;
    *(s32*)AT(c, 0x448) += 0x32000;

    L.tv.x = *(s32*)(c + 0x5c);
    L.tv.y = *(s32*)(c + 0x60);
    L.tv.z = *(s32*)(c + 0x64);

    {
        int rr = (int)((u32)(*(s32*)(c + 0x168) << 4) >> 0x10);
        int d = rr - 0x28;
        if (d < 0)
            d = -d;
        r3 = ((d << 12) / 10) + 0x400;
        if (r3 < 0x800)
            r3 = 0x800;
        *(s32*)(c + 0x428) = r3;
        *(s32*)(c + 0x42c) = r3;
        *(s32*)(c + 0x430) = r3;
    }

    L.tv.x = data_02082214[(*(u16*)(c + 0x8e) >> 4) << 1] * (s16)0xe0 + L.tv.x;
    r3 = r3 - 0xc00;
    L.tv.z = data_02082214[((*(u16*)(c + 0x8e) >> 4) << 1) + 1] * (s16)0xe0 + L.tv.z;
    L.tv.y = L.tv.y + (r3 * 0x18 + 0x38000);

    *(s32*)(c + 0x434) = L.tv.x;
    *(s32*)(c + 0x438) = L.tv.y;
    *(s32*)(c + 0x43c) = L.tv.z;

    Matrix4x3_FromTranslation((PknMtx43*)(c + 0x190), L.tv.x >> 3, L.tv.y >> 3, L.tv.z >> 3);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16OnAimedAtWithEggEv
/* daPkn_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daPkn_c::OnAimedAtWithEgg() {
    return 0x46000;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ec04
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212ec04(daPkn_c *c, int arg) {
    c->mState = 2;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    /* currFrame is ModelAnim's Animation base at +0x50, then +0x8. Named
       `mModelAnim.currFrame` DIFFs the store (MI this-adjust). */
    *(unsigned int *)((char *)c + 0x168) = ((unsigned int)(arg << 0x10)) >> 4;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ebb4
extern "C" {  /* Retained C-linkage helper. */
void func_ov084_0212ebb4(daPkn_c *c)
{
    func_0201267c(0xc1, &c->mCamSpacePosX);
    c->mState = 5;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, data_ov084_02130e24.file, 0x40000000, 0x1000, 0);
}
}
