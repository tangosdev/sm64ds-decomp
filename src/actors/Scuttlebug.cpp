//cpp
/* Scuttlebug, the spider enemy. ROM RTTI is daSpd_c; vtable 0x02122c2c.
 * daSpd_c_classInit is a reconstructed name (alias Scuttlebug_Spawn).
 * The destructor stays in its own file: it is the key function, and defining
 * it here would emit _ZTI10Scuttlebug, which the cartridge does not have.
 *
 * Source is REVERSE of ROM order (highest address first). Do not reorder.
 *
 * Leftover: the func_ov071 helpers keep linker names. State dispatch stays
 * an incomplete-class pointer-to-member. Vec3 and Mtx43 stay plain words so
 * ~Vector3 is not emitted.
 */

/* common.h first: its flat Matrix4x3 must win the guard; InitResources'
 * whole-matrix store at +0x350 only reproduces with that spelling. */
#include "common.h"
#include "Scuttlebug.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Animation.h"
#include "SurfaceInfo.h"

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

/* One row of the state table at ov071:0x02122fa8. */
typedef struct { int a, b, c, d; } Item16;

/* The dispatch object as the two state-machine trampolines see it: a
 * pointer-to-member-function table at +0x380.  PMF is deliberately formed
 * while ScuttlebugState is INCOMPLETE -- that is what selects the general
 * (offset + index) pointer-to-member representation the ROM uses. */
struct ScuttlebugState;
typedef void (ScuttlebugState::*PMF)();
struct ScuttlebugState { char pad[0x380]; PMF *pp; };

/* Slot 29 (vtable+0x74) reached through a raw cast, in the two functions that
 * ask the actor for its own height rather than going through the header. */
struct VSlot29 {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  m29();
};

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
void  _Z14ApproachLinearRsss(short &dst, short target, short step);
int   _ZN4cstd4fdivEii(int a, int b);

void  Matrix4x3_FromRotationY(void *m, int angle);
void  Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);

void  func_0201267c(int id, void *pos);
void  func_02012694(int id, void *pos);
int   func_02037e38(unsigned int *p);

void  dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
int   _ZNK10dBgCh_Actr12TouchesWaterEv(void *self);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);

void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a,
                                                  int fix, unsigned int j);

void  _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *pos,
                                                     unsigned int count,
                                                     int value, short s);
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
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
void *_ZN11ShadowModelC1Ev(void *p);
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
extern Item16    data_ov071_02122fa8[];   /* state table           */
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern Mtx43     data_020a0e68;           /* scratch matrix        */
extern s16       data_02082214[];         /* sin/cos table         */

/* Intra-TU forward declarations.  mwccarm lays .text down in reverse source
 * order, so this file is written ROM-descending and nearly every intra-TU call
 * is a forward reference. */
void Scuttlebug_SetState(char *self, int idx);
void func_ov071_021202b4(ScuttlebugState *c);
void func_ov071_02120278(ScuttlebugState *c);
void func_ov071_0211f0b4(char *c);
void func_ov071_0211f148(char *a, char *w);
void func_ov071_0211f29c(void *thiz);
void func_ov071_0211f498(int *t);
void func_ov071_0211f524(char *c);

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
        _ZN11ShadowModelC1Ev(&((Scuttlebug *)p)->mShadowModel);
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
    char *a = (char *)this;
    void *p = &player;
    int *bp;
    int t;
    if (((Player *)p)->IsCollectingCap())
        GivePlayerCoins(*(Player *)p, ((Scuttlebug *)a)->mCoinCount, 0);
    else
        ((Player *)p)->RegisterEggCoinCount(((Scuttlebug *)a)->mCoinCount, 0, 0);
    if (((Scuttlebug *)a)->param1 != 0) {
        ((Scuttlebug *)a)->mCoinCount = 0;
        bp = (int *)&((Scuttlebug *)a)->mFlags;
        t = *bp;
        t &= ~0x40000;
        *bp = t;
        Scuttlebug_SetState(a, 0);
    } else {
        MarkForDestruction();
    }
}

// @symbol _ZN10Scuttlebug13InitResourcesEv
int Scuttlebug::InitResources()
{
    char *s = (char *)((dActor_c *)this);
    void *mf = Model::LoadFile(*(SharedFilePtr *)data_ov071_02122f80);
    ((ModelBase *)(&((Scuttlebug *)s)->mModelAnim))->SetFile((BMD_File *)mf, 1, -1);
    Animation::LoadFile(*(SharedFilePtr *)data_ov071_02122f88);
    if (((ShadowModel *)(&((Scuttlebug *)s)->mShadowModel))->InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &((Scuttlebug *)s)->mdCcAc_c, ((dActor_c *)this), 0x46000, 0x64000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &((Scuttlebug *)s)->mWithMeshClsn, ((dActor_c *)this), 0x50000, 0x50000, (Vector3_16 *)0, (Vector3_16 *)0);
    ((dBgCh_Actr *)(&((Scuttlebug *)s)->mWithMeshClsn))->StartDetectingWater();
    ((Scuttlebug *)s)->mHomeX = ((Scuttlebug *)s)->mPosX;
    ((Scuttlebug *)s)->mHomeY = ((Scuttlebug *)s)->mPosY;
    ((Scuttlebug *)s)->mHomeZ = ((Scuttlebug *)s)->mPosZ;
    ((Scuttlebug *)s)->mHomeAngleY = ((Scuttlebug *)s)->mAngleY;
    ((Scuttlebug *)s)->mAnchorX = ((Scuttlebug *)s)->mPosX;
    ((Scuttlebug *)s)->mAnchorY = ((Scuttlebug *)s)->mPosY;
    ((Scuttlebug *)s)->mAnchorZ = ((Scuttlebug *)s)->mPosZ;
    if (((Scuttlebug *)s)->param1 != 0)
        Scuttlebug_SetState(s, 0);
    else
        Scuttlebug_SetState(s, 2);
    ((Scuttlebug *)s)->mCoinCount = 3;
    ((Scuttlebug *)s)->mVertAccel = -0x2000;
    ((Scuttlebug *)s)->mTerminalVelocity = -0x3c000;
    ((Scuttlebug *)s)->mScaleX = 0x1000;
    ((Scuttlebug *)s)->mScaleY = 0x1000;
    ((Scuttlebug *)s)->mScaleZ = 0x1000;
    ((Scuttlebug *)s)->mParent = 0;
    ((Scuttlebug *)s)->mTimer = 0x3c;
    *(Matrix4x3 *)((Scuttlebug *)s)->mShadowMtx = IDENTITY_MATRIX4X3;
    func_ov071_0211f524(s);
    return 1;
}

// @symbol _ZN10Scuttlebug8BehaviorEv
int Scuttlebug::Behavior()
{
    DecIfAbove0_Short((char *)&mTimer);
    func_ov071_02120278((ScuttlebugState *)((char *)this));
    MakeVanishLuigiWork(mdCcAc_c);
    if (mWithMeshClsn.GetResultFlag1() &&
        _ZNK10dBgCh_Actr12TouchesWaterEv((char *)&mWithMeshClsn)) {
        func_ov071_0211f498((int *)((char *)this));
    }
    func_ov071_0211f524(((char *)this));
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

// @symbol Scuttlebug_SetState
extern "C" void Scuttlebug_SetState(char *self, int idx)
{
    ((Scuttlebug *)self)->mStateRow = &data_ov071_02122fa8[idx];
    func_ov071_021202b4((ScuttlebugState *)self);
}

// @symbol func_ov071_021202b4
extern "C" void func_ov071_021202b4(ScuttlebugState *c)
{
    PMF *p = c->pp;
    (c->**p)();
}

/* Call the state's per-frame handler, one slot further into the row. */

// @symbol func_ov071_02120278
extern "C" void func_ov071_02120278(ScuttlebugState *c)
{
    PMF *p = c->pp + 1;
    (c->**p)();
}

// @symbol func_ov071_02120200
extern "C" int func_ov071_02120200(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    int *p = (int *)&self->mFlags;
    int z;
    short ang;

    *p = *p & ~0x10000001;
    self->mPosX = self->mHomeX;
    z = 0;
    self->mPosY = self->mHomeY;
    self->mPosZ = self->mHomeZ;
    self->mPrevAngleY = self->mHomeAngleY;
    ang = self->mHomeAngleY;
    self->mAngleX = z;
    self->mAngleY = ang;
    self->mAngleZ = z;
    self->mHorzSpeed = z;
    self->mTimer = 0x1e;
    self->mdCcAc_c.Clear();
    self->mState = 0;
    return 1;
}

// @symbol func_ov071_021201b4
extern "C" int func_ov071_021201b4(void *c)
{
    if (((Scuttlebug *)c)->mTimer) return 1;
    if (((dActor_c *)c)->DistToCPlayer() < 0x5dc000) Scuttlebug_SetState((char *)c, 1);
    return 1;
}

// @symbol func_ov071_02120130
extern "C" int func_ov071_02120130(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    self->mVertAccel = -0x4000;
    self->mTerminalVelocity = -0x3e000;
    self->mHorzSpeed = 0x16000;
    self->mVertSpeed = 0x4d000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mWithMeshClsn.SetLimMovFlag();
    func_0201267c(0xf1, &self->mCamSpacePosX);
    self->mState = 1;
    return 1;
}

// @symbol func_ov071_02120028
extern "C" int func_ov071_02120028(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    self->mModelAnim.Advance();
    ((dActor_c *)c)->UpdatePos(&self->mdCcAc_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&self->mWithMeshClsn);
    if (self->mWithMeshClsn.JustHitGround()) {
        Vec3 v;
        int x, y, z;
        x = *(volatile int *)&self->mPosX;
        *(volatile int *)&v.x = x;
        y = *(volatile int *)&self->mPosY;
        *(volatile int *)&v.y = y;
        z = *(volatile int *)&self->mPosZ;
        y += 0x28000;
        *(volatile int *)&v.z = z;
        *(volatile int *)&v.y = y;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, x, y, z);
        self->mVertSpeed = self->mVertSpeed * -0x28 / 100;
    } else if (self->mWithMeshClsn.IsOnGround()) {
        self->mVertSpeed = 0;
        self->mWithMeshClsn.ClearLimMovFlag();
        self->mAnchorX = self->mPosX;
        self->mAnchorY = self->mPosY;
        self->mAnchorZ = self->mPosZ;
        self->mFlags |= 0x10000001;
        Scuttlebug_SetState(c, 2);
    }
    func_ov071_0211f29c(c);
    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    return 1;
}

// @symbol func_ov071_0211ff84
extern "C" int func_ov071_0211ff84(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    if (Vec3_Dist((Vector3 *)&self->mPosX,
                  (Vector3 *)&self->mAnchorX) > 0x5dc000) {
        Scuttlebug_SetState(c, 5);
        return 1;
    }
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    self->mHorzSpeed = 0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(void **)((char *)data_ov071_02122f88 + 4), 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mState = 2;
    return 1;
}

// @symbol func_ov071_0211fee4
extern "C" int func_ov071_0211fee4(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    self->mAngleY = (short)(self->mAngleY + 0x2bc);
    self->mPrevAngleY = self->mAngleY;
    self->mModelAnim.Advance();
    unsigned short f = (unsigned short)(self->mModelAnim.currFrame >> 12);
    if (f == 0 || f == 8 || f == 0x17 || f == 0x1f) {
        func_0201267c(0xf0, (void *)(&self->mCamSpacePosX));
    }
    func_ov071_0211f0b4(c);
    ((dActor_c *)c)->UpdatePos((dCc_c *)(&self->mdCcAc_c));
    func_ov071_0211f148(c, (char *)(&self->mWithMeshClsn));
    func_ov071_0211f29c(c);
    ((dCc_c *)(&self->mdCcAc_c))->Clear();
    ((dCc_c *)(&self->mdCcAc_c))->Update();
    return 1;
}

// @symbol func_ov071_0211fe38
extern "C" int func_ov071_0211fe38(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    int *p3a0 = &self->mLeapDist;
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    self->mHorzSpeed = 0xf000;
    self->mVertSpeed = 0x12000;
    self->mAngleY = self->mLeapAngle;
    self->mPrevAngleY = self->mAngleY;
    *p3a0 += 0x12c000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    self->mModelAnim.speed = 0x2c00;
    self->mModelAnim.currFrame = 0;
    func_0201267c(0xf1, &self->mCamSpacePosX);
    self->mState = 3;
    return 1;
}

// @symbol func_ov071_0211fd58
extern "C" int func_ov071_0211fd58(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    self->mModelAnim.Advance();
    int *p = &self->mLeapDist;
    *p -= 0xf000;
    if (self->mLeapDist <= 0) {
        self->mTimer = 0x3c;
        Scuttlebug_SetState(c, 2);
    }
    ((dActor_c *)c)->UpdatePos(&self->mdCcAc_c);
    func_ov071_0211f148(c, (char *)&self->mWithMeshClsn);
    func_ov071_0211f29c(c);
    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    if (self->mWithMeshClsn.IsOnGround() != 0) {
        unsigned int t = (unsigned int)(self->mModelAnim.currFrame << 4) >> 0x10;
        if ((t <= 2) || (t >= 8 && t <= 0xa) || (t >= 0x18 && t <= 0x1a) || (t >= 0x20 && t <= 0x22)) {
            func_0201267c(0xf0, &self->mCamSpacePosX);
        }
    }
    return 1;
}

// @symbol func_ov071_0211fcd4
extern "C" int func_ov071_0211fcd4(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    self->mHorzSpeed = -0x4000;
    self->mVertSpeed = 0x12000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    self->mModelAnim.speed = 0x2c00;
    func_0201267c(0xf1, &self->mCamSpacePosX);
    self->mState = 4;
    return 1;
}

// @symbol func_ov071_0211fc60
extern "C" int func_ov071_0211fc60(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    self->mModelAnim.Advance();
    if (self->mWithMeshClsn.IsOnGround()) {
        self->mTimer = 0x3c;
        Scuttlebug_SetState(c, 2);
    }
    ((dActor_c *)c)->UpdatePos(&self->mdCcAc_c);
    func_ov071_0211f148(c, (char *)&self->mWithMeshClsn);
    func_ov071_0211f29c(c);
    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    return 1;
}

// @symbol func_ov071_0211fbf4
extern "C" int func_ov071_0211fbf4(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    self->mHorzSpeed = 0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mState = 5;
    return 1;
}

// @symbol func_ov071_0211fb24
extern "C" int func_ov071_0211fb24(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    short ang = Vec3_HorzAngle((Vector3 *)&self->mPosX,
                               (Vector3 *)&self->mAnchorX);
    _Z14ApproachLinearRsss(self->mAngleY, ang, 0x2bc);
    self->mPrevAngleY = self->mAngleY;
    self->mModelAnim.Advance();
    if (Vec3_Dist((Vector3 *)&self->mPosX,
                  (Vector3 *)&self->mAnchorX) < 0x12c000)
        Scuttlebug_SetState(c, 2);
    func_ov071_0211f0b4(c);
    ((dActor_c *)c)->UpdatePos(&self->mdCcAc_c);
    func_ov071_0211f148(c, (char *)&self->mWithMeshClsn);
    func_ov071_0211f29c(c);
    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    unsigned short v = (unsigned short)(self->mModelAnim.currFrame >> 0xc);
    if (v == 0 || v == 8 || v == 0x17 || v == 0x1f)
        func_0201267c(0xf0, &self->mCamSpacePosX);
    return 1;
}

// @symbol func_ov071_0211fb0c
extern "C" int func_ov071_0211fb0c(char *p)
{
    ((Scuttlebug *)p)->mHorzSpeed = 0;
    ((Scuttlebug *)p)->mState = 6;
    return 1;
}

// @symbol func_ov071_0211fa54
extern "C" int func_ov071_0211fa54(void *thiz)
{
    char *c = (char *)thiz;
    int b0 = (int)((((Scuttlebug *)c)->mFlags & 0x40000) != 0);
    if (b0 != 0) {
        int *src = &((dActor_c *)((Scuttlebug *)c)->mParent)->mPosX;
        ((Scuttlebug *)c)->mPosX = src[0];
        ((Scuttlebug *)c)->mPosY = src[1];
        ((Scuttlebug *)c)->mPosZ = src[2];
    }
    {
        int v = ((Scuttlebug *)c)->mFlags;
        int b1 = (int)((v & 0x80000) != 0);
        if (b1 != 0) {
            Scuttlebug_SetState(c, 7);
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
        ((Scuttlebug *)c)->mParent = 0;
        Scuttlebug_SetState(c, 2);
    }
done:
    ((Scuttlebug *)c)->mdCcAc_c.Clear();
    return 1;
}

// @symbol func_ov071_0211f8d0
extern "C" int func_ov071_0211f8d0(char *self)
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
    pb0 = (int *)&((Scuttlebug *)self)->mFlags;
    *pb0 = (*pb0) & 0xfff7fffe;

    parent = (char *)((Scuttlebug *)self)->mParent;
    pos = (Vector3 *)&((Scuttlebug *)self)->mPosX;
    mul = 0x5a000;
    ((Scuttlebug *)self)->mHorzSpeed = ((dActor_c *)parent)->mHorzSpeed + 0x7000;
    ((Scuttlebug *)self)->mVertSpeed = zero;

    parent = (char *)((Scuttlebug *)self)->mParent;
    rnd = 0x800;
    ((Scuttlebug *)self)->mAngleY = ((dActor_c *)parent)->mAngleY;
    py = &((Scuttlebug *)self)->mPosY;
    pz = &((Scuttlebug *)self)->mPosZ;
    ((Scuttlebug *)self)->mPrevAngleY = ((Scuttlebug *)self)->mAngleY;

    parent = (char *)((Scuttlebug *)self)->mParent;
    one = 1;
    srcv = (Vector3 *)&((dActor_c *)parent)->mPosX;
    ((Scuttlebug *)self)->mPosX = srcv->x;
    ((Scuttlebug *)self)->mPosY = srcv->y;
    ((Scuttlebug *)self)->mPosZ = srcv->z;

    saved_x = pos->x;
    /* These two reads are ldrh. mAngleY is signed everywhere else. */
    hang = *(u16 *)&((Scuttlebug *)self)->mAngleY;
    s0 = data_02082214[(hang >> 4) * 2];
    adj = (int)(((s64)s0 * mul + rnd) >> 12);
    pos->x = saved_x + adj;

    *py = *py + 0x50000;

    hang = *(u16 *)&((Scuttlebug *)self)->mAngleY;
    s1 = data_02082214[(hang >> 4) * 2 + 1];
    adj = (int)(((s64)s1 * mul + rnd) >> 12);
    *pz = *pz + adj;

    parent = (char *)((Scuttlebug *)self)->mParent;
    y = ((dActor_c *)parent)->mPosY;
    z = ((dActor_c *)parent)->mPosZ;
    y2 = y + 0x50000;
    x = ((dActor_c *)parent)->mPosX;
    v.x = x;
    v.y = y2;
    v.z = z;

    ((dActor_c *)self)->DetectRaycastClsn(v, *pos, one);

    ((Scuttlebug *)self)->mParent = (dActor_c *)zero;
    ((Scuttlebug *)self)->mWithMeshClsn.SetLimMovFlag();

    ((Scuttlebug *)self)->mState = 7;
    return 1;
}

// @symbol func_ov071_0211f7d4
extern "C" int func_ov071_0211f7d4(dActor_c *self)
{
    char *s = (char *)self;
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&((Scuttlebug *)s)->mWithMeshClsn);
    ((Scuttlebug *)s)->mAngleX = ((Scuttlebug *)s)->mAngleX + 0x1000;
    if (((dBgCh_Actr *)(&((Scuttlebug *)s)->mWithMeshClsn))->JustHitGround()) {
        if (func_02037e38((unsigned int *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv((dBgCh_Actr *)(&((Scuttlebug *)s)->mWithMeshClsn)) + 4)) == 4) {
            func_ov071_0211f498((int *)s);
        } else {
            ((Scuttlebug *)s)->mVertSpeed = (((Scuttlebug *)s)->mVertSpeed * -0x3c) / 0x64;
        }
    } else if (((dBgCh_Actr *)(&((Scuttlebug *)s)->mWithMeshClsn))->IsOnGround()) {
        dBgCh_Actr *wm = (dBgCh_Actr *)(&((Scuttlebug *)s)->mWithMeshClsn);
        ((Scuttlebug *)s)->mVertSpeed = 0;
        wm->ClearLimMovFlag();
        ((Scuttlebug *)s)->mFlags |= 1;
        short z = 0;
        short ang = ((Scuttlebug *)s)->mPrevAngleY;
        ((Scuttlebug *)s)->mAngleX = z;
        ((Scuttlebug *)s)->mAngleY = ang;
        ((Scuttlebug *)s)->mAngleZ = z;
        Scuttlebug_SetState(s, 2);
    }
    self->UpdatePos((dCc_c *)(&((Scuttlebug *)s)->mdCcAc_c));
    func_ov071_0211f29c(s);
    ((Scuttlebug *)s)->mdCcAc_c.Clear();
    ((Scuttlebug *)s)->mdCcAc_c.Update();
    return 1;
}

// @symbol func_ov071_0211f6f8
extern "C" int func_ov071_0211f6f8(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (const void *)(&self->mCamSpacePosX));
    self->mFlags &= ~1;
    self->mHorzSpeed = 0xa000;
    self->mVertSpeed = 0x28000;
    self->mTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(void **)((char *)data_ov071_02122f88 + 4), 0, 0x1000, 0);
    self->mModelAnim.speed = 0x4000;
    VSlot29 *b = (VSlot29 *)c;
    int yOffset1 = b->m29();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, self->mPosX, self->mPosY + yOffset1, self->mPosZ);
    int yOffset2 = b->m29();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, self->mPosX, self->mPosY + yOffset2, self->mPosZ);
    self->mState = 8;
    return 1;
}

// @symbol func_ov071_0211f694
extern "C" int func_ov071_0211f694(char *t)
{
    ((Scuttlebug *)t)->mAngleX = ((Scuttlebug *)t)->mAngleX - 0x1000;
    ((Scuttlebug *)t)->mModelAnim.Advance();
    ((dActor_c *)t)->UpdatePos(&((Scuttlebug *)t)->mdCcAc_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&((Scuttlebug *)t)->mWithMeshClsn);
    if (((Scuttlebug *)t)->mWithMeshClsn.JustHitGround() != 0 || ((Scuttlebug *)t)->mTimer == 0)
        func_ov071_0211f498((int *)t);
    return 1;
}

// @symbol func_ov071_0211f524
extern "C" void func_ov071_0211f524(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    int b = (int)((self->mFlags & 0x40000) != 0);
    if (b) {
        if (self->mState == 0)
            return;
    }

    Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    self->mModelAnim.mat4x3.m[9] = self->mPosX >> 3;
    self->mModelAnim.mat4x3.m[10] = self->mPosY >> 3;
    self->mModelAnim.mat4x3.m[11] = self->mPosZ >> 3;

    if (self->mAngleX != 0) {
        data_020a0e68 = *(Mtx43 *)&self->mModelAnim.mat4x3;
        int y1 = ((VSlot29 *)c)->m29() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mAngleX);
        int y2 = (-((VSlot29 *)c)->m29()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        *(Mtx43 *)&self->mModelAnim.mat4x3 = data_020a0e68;
    }

    self->mShadowMtx[9] = self->mPosX >> 3;
    self->mShadowMtx[10] = self->mPosY >> 3;
    self->mShadowMtx[11] = self->mPosZ >> 3;

    int dh = (self->mState == 8) ? 0x190000 : 0xc8000;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &self->mShadowModel, self->mShadowMtx, 0xa0000, dh, 0xf);
}

// @symbol func_ov071_0211f498
extern "C" void func_ov071_0211f498(int *t)
{
    Vec3 v;
    v.x = ((Scuttlebug *)t)->mPosX;
    v.y = ((Scuttlebug *)t)->mPosY;
    v.z = ((Scuttlebug *)t)->mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(t, &v, ((Scuttlebug *)t)->mCoinCount, 0xf000, 0);
    ((dActor_c *)t)->PoofDust();
    func_02012694(0xc4, &((Scuttlebug *)t)->mCamSpacePosX);
    if (((Scuttlebug *)t)->param1) {
        ((Scuttlebug *)t)->mCoinCount = 0;
        Scuttlebug_SetState((char *)t, 0);
        return;
    }
    ((fBase_c *)t)->MarkForDestruction();
}

// @symbol func_ov071_0211f29c
extern "C" void func_ov071_0211f29c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *hitPlayer;
    int b;

    if (((dActor_c *)c)->FindEgg(((Scuttlebug *)c)->mdCcAc_c) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, &((Scuttlebug *)c)->mCamSpacePosX);
        func_ov071_0211f498((int *)c);
        return;
    }

    {
        unsigned int id = ((Scuttlebug *)c)->mdCcAc_c.otherOwner;
        if (id == 0)
            return;
        hitPlayer = (unsigned char *)dActor_c::FindWithID(id);
    }
    if (hitPlayer == 0)
        return;

    b = (int)(((Player *)hitPlayer)->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((((Scuttlebug *)c)->mFlags & 0x20000) != 0);
    if (b != 0) {
        Scuttlebug_SetState((char *)c, 6);
        return;
    }

    if ((((Scuttlebug *)c)->mdCcAc_c.hitFlags & 0x66fe0)
        || ((Player *)hitPlayer)->IsOnShell() != 0
        || ((Player *)hitPlayer)->mIsMetal != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, &((Scuttlebug *)c)->mCamSpacePosX);
        func_ov071_0211f498((int *)c);
        return;
    }

    if (((Scuttlebug *)c)->mdCcAc_c.hitFlags & 0x10) {
        ((Scuttlebug *)c)->mPrevAngleY = Vec3_HorzAngle(
            (Vector3 *)&((Player *)hitPlayer)->mPosX,
            (Vector3 *)&((Scuttlebug *)c)->mPosX);
        ((Scuttlebug *)c)->mAngleY = (short)(((Scuttlebug *)c)->mPrevAngleY + 0x8000);
        ((Player *)hitPlayer)->IncMegaKillCount();
        Scuttlebug_SetState((char *)c, 8);
        return;
    }

    if (((dActor_c *)c)->JumpedOnByPlayer(((Scuttlebug *)c)->mdCcAc_c, *(Player *)hitPlayer) != 0) {
        _ZN6Player6BounceE5Fix12IiE(hitPlayer, 0x28000);
        func_ov071_0211f498((int *)c);
        return;
    }

    if (((Scuttlebug *)c)->mState == 7)
        return;

    {
        int v[3];
        v[0] = ((Scuttlebug *)c)->mPosX;
        v[1] = ((Scuttlebug *)c)->mPosY;
        v[2] = ((Scuttlebug *)c)->mPosZ;
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(hitPlayer, v, 1, 0xc000, 1, 0, 1) != 0)
            Scuttlebug_SetState((char *)c, 4);
    }
}

// @symbol func_ov071_0211f148
extern "C" void func_ov071_0211f148(char *a, char *w)
{
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (((dBgCh_Actr *)w)->IsOnGround()) {
        dBgCh_Gnd rc;
        {
            int p60 = ((Scuttlebug *)a)->mPosY;
            int pz = ((Scuttlebug *)a)->mPosZ;
            int py = p60 + 0x1e000;
            pos.x = ((Scuttlebug *)a)->mPosX;
            pos.y = py;
            pos.z = pz;
        }
        rc.SetObjAndPos(pos, (dActor_c *)a);
        if (!rc.DetectClsn() || rc.clsnY < ((Scuttlebug *)a)->mPosY - 0x32000) {
            ((Scuttlebug *)a)->mHorzSpeed = 0;
            ((Scuttlebug *)a)->mPosX = ((Scuttlebug *)a)->mPrevPosX;
            ((Scuttlebug *)a)->mPosY = ((Scuttlebug *)a)->mPrevPosY;
            ((Scuttlebug *)a)->mPosZ = ((Scuttlebug *)a)->mPrevPosZ;
        } else {
            void *fr = _ZNK10dBgCh_Actr14GetFloorResultEv(w);
            ((SurfaceInfo *)((char *)fr + 4))->CopyNormalTo(normal);
            if (normal.y != 0) {
                ((Scuttlebug *)a)->mVertSpeed = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * ((Scuttlebug *)a)->unk_0a4 + 0x800) >> 12)
                  + (int)(((long long)normal.z * ((Scuttlebug *)a)->unk_0ac + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
    }
    if (((dBgCh_Actr *)w)->IsOnWall()) {
        void *wr = _ZNK10dBgCh_Actr13GetWallResultEv(w);
        ((SurfaceInfo *)((char *)wr + 4))->CopyNormalTo(wallnormal);
    }
}

// @symbol func_ov071_0211f0b4
extern "C" void func_ov071_0211f0b4(char *c)
{
    Scuttlebug *self = (Scuttlebug *)c;
    dActor_c *p;
    Fix12i d;
    short ang;
    if (self->mTimer != 0) return;
    p = (dActor_c *)((dActor_c *)c)->ClosestNonVanishPlayer();
    if (p == 0) return;
    d = Vec3_Dist((const Vector3 *)&self->mPosX, (const Vector3 *)&p->mPosX);
    if (d > 0x5dc000) return;
    ang = Vec3_HorzAngle((const Vector3 *)&self->mPosX, (const Vector3 *)&p->mPosX);
    if (AngleDiff(ang, self->mAngleY) > 0x12c) return;
    self->mLeapDist = d;
    self->mLeapAngle = ang;
    Scuttlebug_SetState(c, 3);
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

