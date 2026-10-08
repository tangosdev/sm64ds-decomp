//cpp
/* Scuttlebug, the spider enemy. ROM RTTI is daSpd_c; vtable 0x02122c2c.
 * daSpd_c_classInit is a reconstructed name (alias Scuttlebug_Spawn).
 * The destructor stays in its own file: it is the key function, and defining
 * it here would emit _ZTI10Scuttlebug, which the cartridge does not have.
 *
 * Source is REVERSE of ROM order (highest address first). Do not reorder.
 *
 * deslop leftovers:
 *  - The 25 state handlers/helpers keep func_ov071_* address names as member
 *    names; no original names are recovered. SetState was Scuttlebug_SetState.
 *  - Vec3 and Mtx43 stay plain words: spelling them as Vector3/Matrix4x3
 *    drags ~Vector3's vague-linkage D1 into functions that never had it.
 *  - data_ov071_02122f80/_02122f88 stay int[] views: the code indexes the
 *    SharedFilePtr pairs by word, not by object.
 *  - The extern "C" preamble is hand-spelt: Fix12<int>/Vector3 parameters
 *    are written as plain words where the true types break the register
 *    convention, and the spelled mangled calls stay where the member
 *    spelling would change codegen.
 *  - volatile int force_stack pads OnTurnIntoEgg's frame to the ROM shape.
 */

/* common.h first: its flat Matrix4x3 must win the guard; InitResources'
 * whole-matrix store at +0x350 only reproduces with that spelling. */
#include "common.h"
#include "Scuttlebug.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dExtFrameCtrl_c.h"
#include "SurfaceInfo.h"

bool ApproachLinear(short &value, short target, short step);

/* ------------------------------------------------------------------------
 * Local value shapes carried from the legacy one-function files.
 * ------------------------------------------------------------------------ */

/* Three plain fixed-point words.  Vector3 has a user-declared destructor, so
 * spelling these as Vector3 would drag its vague-linkage D1 into functions
 * that never had it. */
typedef struct { int x, y, z; } Vec3;

/* Flat 0x30-byte matrix block, as func_ov071_0211f524 and the scratch matrix
 * at arm9:0x020a0e68 are used: whole-block assignment, never by field. */
typedef struct Mtx43 { int w[12]; } Mtx43;

typedef void (Scuttlebug::*PMF)();

/* One row of the state table at ov071:0x02122fa8: enter handler, then the
 * per-frame run handler. Built by __sinit_ov071_021226ac from .data words. */
typedef struct { PMF enter, run; } ScuttlebugStateRow;

/* ------------------------------------------------------------------------
 * ABI imports.  One declaration per symbol, hand-reconciled: the generated
 * preamble carried these names with contradictory spellings from thirty-odd
 * source files.  The by-value Fix12<int> and Vector3 parameters are spelt as
 * plain words / pointers on purpose -- the true types are passed differently
 * and break the register convention.
 * ------------------------------------------------------------------------ */
extern "C" {

short AngleDiff(short a, short b);
void  DecIfAbove0_Short(void *p);
int   Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int   _ZN4cstd4fdivEii(int a, int b);

void  Matrix4x3_FromRotationY(void *m, int angle);
void  Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);

void  func_0201267c(int id, void *pos);
void  func_02012694(int id, void *pos);
int   func_02037e38(unsigned int *p);

void  dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);

void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a,
                                                  int fix, unsigned int j);

void  _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *pos,
                                                     unsigned int count,
                                                     int value, short s);
void  _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
          void *self, void *shadow, void *mtx, int radius, int height, u32 flags);

void  _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const void *pos);
void  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x,
                                                     int y, int z);

void  _ZN6Player6BounceE5Fix12IiE(void *player, int speed);
int   _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, void *pos,
                                              unsigned int a, int damage,
                                              unsigned int b, unsigned int c,
                                              unsigned int d);

void *_ZN8dActor_cC2Ev(void *actor);
void *_ZN9ModelAnimC1Ev(void *p);
void *_ZN17dExtShadowModel_cC1Ev(void *p);
void *_ZN7dCcAc_cC1Ev(void *p);
void *_ZN10dBgCh_ActrC1Ev(void *p);


/* Declared by final name rather than as members: both take Fix12<int> where
 * these call sites pass int literals, and dBgCh_Actr::Init's last two
 * parameters are Vector3_16* (the S5_ back-references the pointer type). */
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *actor,
                                                int radius, int height,
                                                unsigned int flags,
                                                unsigned int vulnFlags);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
          void *self, dActor_c *actor, int radius, int height,
          Vector3_16 *a, Vector3_16 *b);

extern int       data_ov071_02122f80[];   /* SharedFilePtr, model  */
extern int       data_ov071_02122f88[];   /* SharedFilePtr, anim   */
extern ScuttlebugStateRow data_ov071_02122fa8[]; /* state table           */
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern Mtx43     data_020a0e68;           /* scratch matrix        */
extern s16       data_02082214[];         /* sin/cos table         */

/* Intra-TU forward declarations.  mwccarm lays .text down in reverse source
 * order, so this file is written ROM-descending and nearly every intra-TU call
 * is a forward reference. */
}

/* Natural `new` selects the wrong allocator, so the measured actor
 * construction seam is retained verbatim. */

// @symbol daSpd_c_classInit
extern "C" int *daSpd_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(940);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV10Scuttlebug[2];
        _ZN9ModelAnimC1Ev(&((Scuttlebug *)p)->mModelAnim);
        _ZN17dExtShadowModel_cC1Ev(&((Scuttlebug *)p)->mShadowModel);
        _ZN7dCcAc_cC1Ev(&((Scuttlebug *)p)->mdCcAc_c);
        _ZN10dBgCh_ActrC1Ev(&((Scuttlebug *)p)->mWithMeshClsn);
    }
    return p;
}

/* Pays the player mCoinCount coins (a cap coin if Yoshi is wearing the cap,
 * an egg coin otherwise). A child (param1 != 0) resets; the original dies. */

// @symbol _ZN10Scuttlebug13OnTurnIntoEggER6Player
void Scuttlebug::OnTurnIntoEgg(Player &player)
{
    volatile int force_stack;
    void *p = &player;
    int *bp;
    int t;
    if (((Player *)p)->IsCollectingCap())
        GivePlayerCoins(*(Player *)p, mCoinCount, 0);
    else
        ((Player *)p)->RegisterEggCoinCount(mCoinCount, 0, 0);
    if (param1 != 0) {
        mCoinCount = 0;
        bp = (int *)&mFlags;
        t = *bp;
        t &= ~0x40000;
        *bp = t;
        SetState(0);
    } else {
        MarkForDestruction();
    }
}

// @symbol _ZN10Scuttlebug13InitResourcesEv
int Scuttlebug::InitResources()
{
    void *mf = Model::LoadFile(*(SharedFilePtr *)data_ov071_02122f80);
    ((ModelBase *)(&mModelAnim))->SetFile((BMD_File *)mf, 1, -1);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov071_02122f88);
    if (((dExtShadowModel_c *)(&mShadowModel))->InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, ((dActor_c *)this), 0x46000, 0x64000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, ((dActor_c *)this), 0x50000, 0x50000, (Vector3_16 *)0, (Vector3_16 *)0);
    mWithMeshClsn.StartDetectingWater();
    mHomeX = mPosX;
    mHomeY = mPosY;
    mHomeZ = mPosZ;
    mHomeAngleY = mAngleY;
    mAnchorX = mPosX;
    mAnchorY = mPosY;
    mAnchorZ = mPosZ;
    if (param1 != 0)
        SetState(0);
    else
        SetState(2);
    mCoinCount = 3;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mParent = 0;
    mTimer = 0x3c;
    *(Matrix4x3 *)mShadowMtx = IDENTITY_MATRIX4X3;
    func_ov071_0211f524();
    return 1;
}

// @symbol _ZN10Scuttlebug8BehaviorEv
int Scuttlebug::Behavior()
{
    DecIfAbove0_Short((char *)&mTimer);
    func_ov071_02120278();
    MakeVanishLuigiWork(mdCcAc_c);
    if (mWithMeshClsn.GetResultFlag1() &&
        mWithMeshClsn.TouchesWater()) {
        func_ov071_0211f498();
    }
    func_ov071_0211f524();
    return 1;
}

// @symbol _ZN10Scuttlebug6RenderEv
int Scuttlebug::Render()
{
    int flag = (mFlags & 0x40000) ? 1 : 0;
    if (flag) goto ret;
    if (!mState) goto ret;
    goto call;
ret:
    return 1;
call:
    /* ModelAnim slot 5. The call stays virtual. */
    mModelAnim.Render(0);
    return 1;
}

/* Empty: the override exists only to occupy vtable slot 12. */

// @symbol _ZN10Scuttlebug16OnPendingDestroyEv
void Scuttlebug::OnPendingDestroy()
{
}

/* Releases the two shared files. Never touches `this`. */

// @symbol _ZN10Scuttlebug16CleanupResourcesEv
int Scuttlebug::CleanupResources()
{
    ((SharedFilePtr *)data_ov071_02122f80)->Release();
    ((SharedFilePtr *)data_ov071_02122f88)->Release();
    return 1;
}

/* Points mStateRow at one row of the table and tail-calls its entry handler. */

// @symbol _ZN10Scuttlebug8SetStateEi
void Scuttlebug::SetState(int idx)
{
    mStateRow = &data_ov071_02122fa8[idx];
    func_ov071_021202b4();
}

// @symbol _ZN10Scuttlebug19func_ov071_021202b4Ev
void Scuttlebug::func_ov071_021202b4()
{
    PMF *p = (PMF *)mStateRow;
    (this->**p)();
}

/* Call the state's per-frame handler, one slot further into the row. */

// @symbol _ZN10Scuttlebug19func_ov071_02120278Ev
void Scuttlebug::func_ov071_02120278()
{
    PMF *p = (PMF *)mStateRow + 1;
    (this->**p)();
}

// @symbol _ZN10Scuttlebug19func_ov071_02120200Ev
int Scuttlebug::func_ov071_02120200()
{
    int *p = (int *)&mFlags;
    int z;
    short ang;

    *p = *p & ~0x10000001;
    mPosX = mHomeX;
    z = 0;
    mPosY = mHomeY;
    mPosZ = mHomeZ;
    mPrevAngleY = mHomeAngleY;
    ang = mHomeAngleY;
    mAngleX = z;
    mAngleY = ang;
    mAngleZ = z;
    mHorzSpeed = z;
    mTimer = 0x1e;
    mdCcAc_c.Clear();
    mState = 0;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_021201b4Ev
int Scuttlebug::func_ov071_021201b4()
{
    if (mTimer) return 1;
    if (DistToCPlayer() < 0x5dc000) SetState(1);
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_02120130Ev
int Scuttlebug::func_ov071_02120130()
{
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x3e000;
    mHorzSpeed = 0x16000;
    mVertSpeed = 0x4d000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mWithMeshClsn.SetLimMovFlag();
    func_0201267c(0xf1, &mCamSpacePosX);
    mState = 1;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_02120028Ev
int Scuttlebug::func_ov071_02120028()
{
    mModelAnim.Advance();
    UpdatePos(&mdCcAc_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround()) {
        Vec3 v;
        int x, y, z;
        x = *(volatile int *)&mPosX;
        *(volatile int *)&v.x = x;
        y = *(volatile int *)&mPosY;
        *(volatile int *)&v.y = y;
        z = *(volatile int *)&mPosZ;
        y += 0x28000;
        *(volatile int *)&v.z = z;
        *(volatile int *)&v.y = y;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, x, y, z);
        mVertSpeed = mVertSpeed * -0x28 / 100;
    } else if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        mAnchorX = mPosX;
        mAnchorY = mPosY;
        mAnchorZ = mPosZ;
        mFlags |= 0x10000001;
        SetState(2);
    }
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211ff84Ev
int Scuttlebug::func_ov071_0211ff84()
{
    if (Vec3_Dist((Vector3 *)&mPosX,
                  (Vector3 *)&mAnchorX) > 0x5dc000) {
        SetState(5);
        return 1;
    }
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)((char *)data_ov071_02122f88 + 4), 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mState = 2;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fee4Ev
int Scuttlebug::func_ov071_0211fee4()
{
    mAngleY = (short)(mAngleY + 0x2bc);
    mPrevAngleY = mAngleY;
    mModelAnim.Advance();
    unsigned short f = (unsigned short)(mModelAnim.currFrame >> 12);
    if (f == 0 || f == 8 || f == 0x17 || f == 0x1f) {
        func_0201267c(0xf0, (void *)(&mCamSpacePosX));
    }
    func_ov071_0211f0b4();
    UpdatePos((dCc_c *)(&mdCcAc_c));
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    ((dCc_c *)(&mdCcAc_c))->Clear();
    ((dCc_c *)(&mdCcAc_c))->Update();
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fe38Ev
int Scuttlebug::func_ov071_0211fe38()
{
    int *p3a0 = &mLeapDist;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0xf000;
    mVertSpeed = 0x12000;
    mAngleY = mLeapAngle;
    mPrevAngleY = mAngleY;
    *p3a0 += 0x12c000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x2c00;
    mModelAnim.currFrame = 0;
    func_0201267c(0xf1, &mCamSpacePosX);
    mState = 3;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fd58Ev
int Scuttlebug::func_ov071_0211fd58()
{
    mModelAnim.Advance();
    int *p = &mLeapDist;
    *p -= 0xf000;
    if (mLeapDist <= 0) {
        mTimer = 0x3c;
        SetState(2);
    }
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    if (mWithMeshClsn.IsOnGround() != 0) {
        unsigned int t = (unsigned int)(mModelAnim.currFrame << 4) >> 0x10;
        if ((t <= 2) || (t >= 8 && t <= 0xa) || (t >= 0x18 && t <= 0x1a) || (t >= 0x20 && t <= 0x22)) {
            func_0201267c(0xf0, &mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fcd4Ev
int Scuttlebug::func_ov071_0211fcd4()
{
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = -0x4000;
    mVertSpeed = 0x12000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x2c00;
    func_0201267c(0xf1, &mCamSpacePosX);
    mState = 4;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fc60Ev
int Scuttlebug::func_ov071_0211fc60()
{
    mModelAnim.Advance();
    if (mWithMeshClsn.IsOnGround()) {
        mTimer = 0x3c;
        SetState(2);
    }
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fbf4Ev
int Scuttlebug::func_ov071_0211fbf4()
{
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mState = 5;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fb24Ev
int Scuttlebug::func_ov071_0211fb24()
{
    short ang = Vec3_HorzAngle((Vector3 *)&mPosX,
                               (Vector3 *)&mAnchorX);
    ApproachLinear(mAngleY, ang, 0x2bc);
    mPrevAngleY = mAngleY;
    mModelAnim.Advance();
    if (Vec3_Dist((Vector3 *)&mPosX,
                  (Vector3 *)&mAnchorX) < 0x12c000)
        SetState(2);
    func_ov071_0211f0b4();
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    unsigned short v = (unsigned short)(mModelAnim.currFrame >> 0xc);
    if (v == 0 || v == 8 || v == 0x17 || v == 0x1f)
        func_0201267c(0xf0, &mCamSpacePosX);
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fb0cEv
int Scuttlebug::func_ov071_0211fb0c()
{
    mHorzSpeed = 0;
    mState = 6;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211fa54Ev
int Scuttlebug::func_ov071_0211fa54()
{
    int b0 = (int)((mFlags & 0x40000) != 0);
    if (b0 != 0) {
        int *src = &((dActor_c *)mParent)->mPosX;
        mPosX = src[0];
        mPosY = src[1];
        mPosZ = src[2];
    }
    {
        int v = mFlags;
        int b1 = (int)((v & 0x80000) != 0);
        if (b1 != 0) {
            SetState(7);
            goto done;
        }
        {
            int b2 = (int)((v & 0x20000) != 0);
            if (b2 != 0) goto done;
        }
        {
            int b3 = (int)((v & 0x40000) != 0);
            if (b3 != 0) goto done;
        }
        mParent = 0;
        SetState(2);
    }
done:
    mdCcAc_c.Clear();
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f8d0Ev
int Scuttlebug::func_ov071_0211f8d0()
{
    Vector3 *pos;
    Vector3 v;
    int zero;
    char *parent;
    int *pb0;
    int *py;
    int *pz;
    int s0, s1;
    u16 hang;
    int saved_x;
    int mul;
    int rnd;
    int one;
    Vector3 *srcv;
    int adj;
    int y, z, x, y2;

    zero = 0;
    pb0 = (int *)&mFlags;
    *pb0 = (*pb0) & 0xfff7fffe;

    parent = (char *)mParent;
    pos = (Vector3 *)&mPosX;
    mul = 0x5a000;
    mHorzSpeed = ((dActor_c *)parent)->mHorzSpeed + 0x7000;
    mVertSpeed = zero;

    parent = (char *)mParent;
    rnd = 0x800;
    mAngleY = ((dActor_c *)parent)->mAngleY;
    py = &mPosY;
    pz = &mPosZ;
    mPrevAngleY = mAngleY;

    parent = (char *)mParent;
    one = 1;
    srcv = (Vector3 *)&((dActor_c *)parent)->mPosX;
    mPosX = srcv->x;
    mPosY = srcv->y;
    mPosZ = srcv->z;

    saved_x = pos->x;
    /* These two reads are ldrh. mAngleY is signed everywhere else. */
    hang = *(u16 *)&mAngleY;
    s0 = data_02082214[(hang >> 4) * 2];
    adj = (int)(((s64)s0 * mul + rnd) >> 12);
    pos->x = saved_x + adj;

    *py = *py + 0x50000;

    hang = *(u16 *)&mAngleY;
    s1 = data_02082214[(hang >> 4) * 2 + 1];
    adj = (int)(((s64)s1 * mul + rnd) >> 12);
    *pz = *pz + adj;

    parent = (char *)mParent;
    y = ((dActor_c *)parent)->mPosY;
    z = ((dActor_c *)parent)->mPosZ;
    y2 = y + 0x50000;
    x = ((dActor_c *)parent)->mPosX;
    v.x = x;
    v.y = y2;
    v.z = z;

    DetectRaycastClsn(v, *pos, one);

    mParent = (dActor_c *)zero;
    mWithMeshClsn.SetLimMovFlag();

    mState = 7;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f7d4Ev
int Scuttlebug::func_ov071_0211f7d4()
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    mAngleX = mAngleX + 0x1000;
    if (mWithMeshClsn.JustHitGround()) {
        if (func_02037e38((unsigned int *)((char *)mWithMeshClsn.GetFloorResult() + 4)) == 4) {
            func_ov071_0211f498();
        } else {
            mVertSpeed = (mVertSpeed * -0x3c) / 0x64;
        }
    } else if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        mFlags |= 1;
        short z = 0;
        short ang = mPrevAngleY;
        mAngleX = z;
        mAngleY = ang;
        mAngleZ = z;
        SetState(2);
    }
    UpdatePos((dCc_c *)(&mdCcAc_c));
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f6f8Ev
int Scuttlebug::func_ov071_0211f6f8()
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (const void *)(&mCamSpacePosX));
    mFlags &= ~1;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)((char *)data_ov071_02122f88 + 4), 0, 0x1000, 0);
    mModelAnim.speed = 0x4000;
    int yOffset1 = OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, mPosX, mPosY + yOffset1, mPosZ);
    int yOffset2 = OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, mPosX, mPosY + yOffset2, mPosZ);
    mState = 8;
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f694Ev
int Scuttlebug::func_ov071_0211f694()
{
    mAngleX = mAngleX - 0x1000;
    mModelAnim.Advance();
    UpdatePos(&mdCcAc_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() != 0 || mTimer == 0)
        func_ov071_0211f498();
    return 1;
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f524Ev
void Scuttlebug::func_ov071_0211f524()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b) {
        if (mState == 0)
            return;
    }

    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;

    if (mAngleX != 0) {
        data_020a0e68 = *(Mtx43 *)&mModelAnim.mat4x3;
        int y1 = OnAimedAtWithEgg() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        int y2 = (-OnAimedAtWithEgg()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        *(Mtx43 *)&mModelAnim.mat4x3 = data_020a0e68;
    }

    mShadowMtx[9] = mPosX >> 3;
    mShadowMtx[10] = mPosY >> 3;
    mShadowMtx[11] = mPosZ >> 3;

    int dh = (mState == 8) ? 0x190000 : 0xc8000;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, mShadowMtx, 0xa0000, dh, 0xf);
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f498Ev
void Scuttlebug::func_ov071_0211f498()
{
    Vec3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, &v, mCoinCount, 0xf000, 0);
    PoofDust();
    func_02012694(0xc4, &mCamSpacePosX);
    if (param1) {
        mCoinCount = 0;
        SetState(0);
        return;
    }
    ((fBase_c *)this)->MarkForDestruction();
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f29cEv
void Scuttlebug::func_ov071_0211f29c()
{
    Player *hitPlayer;
    int b;

    if (FindEgg(mdCcAc_c) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, &mCamSpacePosX);
        func_ov071_0211f498();
        return;
    }

    {
        unsigned int id = mdCcAc_c.otherOwner;
        if (id == 0)
            return;
        hitPlayer = (Player *)dActor_c::FindWithID(id);
    }
    if (hitPlayer == 0)
        return;

    b = (int)(hitPlayer->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((mFlags & 0x20000) != 0);
    if (b != 0) {
        SetState(6);
        return;
    }

    if ((mdCcAc_c.hitFlags & 0x66fe0)
        || hitPlayer->IsOnShell() != 0
        || hitPlayer->mIsMetal != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, &mCamSpacePosX);
        func_ov071_0211f498();
        return;
    }

    if (mdCcAc_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle(
            (Vector3 *)&hitPlayer->mPosX,
            (Vector3 *)&mPosX);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        hitPlayer->IncMegaKillCount();
        SetState(8);
        return;
    }

    if (JumpedOnByPlayer(mdCcAc_c, *hitPlayer) != 0) {
        _ZN6Player6BounceE5Fix12IiE(hitPlayer, 0x28000);
        func_ov071_0211f498();
        return;
    }

    if (mState == 7)
        return;

    {
        int v[3];
        v[0] = mPosX;
        v[1] = mPosY;
        v[2] = mPosZ;
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(hitPlayer, v, 1, 0xc000, 1, 0, 1) != 0)
            SetState(4);
    }
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f148EP10dBgCh_Actr
void Scuttlebug::func_ov071_0211f148(dBgCh_Actr *w)
{
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (w->IsOnGround()) {
        dBgCh_Gnd rc;
        {
            int p60 = mPosY;
            int pz = mPosZ;
            int py = p60 + 0x1e000;
            pos.x = mPosX;
            pos.y = py;
            pos.z = pz;
        }
        rc.SetObjAndPos(pos, (dActor_c *)this);
        if (!rc.DetectClsn() || rc.clsnY < mPosY - 0x32000) {
            mHorzSpeed = 0;
            mPosX = mPrevPosX;
            mPosY = mPrevPosY;
            mPosZ = mPrevPosZ;
        } else {
            void *fr = w->GetFloorResult();
            ((SurfaceInfo *)((char *)fr + 4))->CopyNormalTo(normal);
            if (normal.y != 0) {
                mVertSpeed = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * unk_0a4 + 0x800) >> 12)
                  + (int)(((long long)normal.z * unk_0ac + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
    }
    if (w->IsOnWall()) {
        void *wr = w->GetWallResult();
        ((SurfaceInfo *)((char *)wr + 4))->CopyNormalTo(wallnormal);
    }
}

// @symbol _ZN10Scuttlebug19func_ov071_0211f0b4Ev
void Scuttlebug::func_ov071_0211f0b4()
{
    dActor_c *p;
    Fix12i d;
    short ang;
    if (mTimer != 0) return;
    p = (dActor_c *)ClosestNonVanishPlayer();
    if (p == 0) return;
    d = Vec3_Dist((const Vector3 *)&mPosX, (const Vector3 *)&p->mPosX);
    if (d > 0x5dc000) return;
    ang = Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)&p->mPosX);
    if (AngleDiff(ang, mAngleY) > 0x12c) return;
    mLeapDist = d;
    mLeapAngle = ang;
    SetState(3);
}

/* Vtable slot 29. Ignores `this` and returns a constant. */

// @symbol _ZN10Scuttlebug16OnAimedAtWithEggEv
int Scuttlebug::OnAimedAtWithEgg()
{
    return 204800;
}

/* Vtable slot 18, and the first function of this run. Not the key function. */

// @symbol _ZN10Scuttlebug13OnYoshiTryEatEv
int Scuttlebug::OnYoshiTryEat()
{
    return 6;
}

