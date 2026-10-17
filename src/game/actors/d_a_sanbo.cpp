//cpp
/* daSanbo_c -- Pokey (ov096).
 *
 * Head (actor id 0xf0) and body segments (0xf1) are the same class.
 * mPrevSegment / mNextSegment link the pieces. Both factories are
 * return new daSanbo_c(). There is no out-of-line constructor.
 *
 * Source order is ROM-descending. mwccarm emits .text in reverse
 * source order, and the inline destructor in include/daSanbo_c.h
 * emits D1 then D0 with no D2. Do not reorder, and do not write
 * the destructor out of line.
 *
 * common.h stays above daSanbo_c.h. Matrix4x3 has to be the flat
 * s32[12] here: the structured spelling scalarises InitResources'
 * copy of IDENTITY_MATRIX4X3 (0x23c against the cartridge's 0x220).
 *
 * All helpers are daSanbo_c members; the func_ov096_<addr> names are
 * ours, not recovered. The state pairs they serve live in
 * data_ov096_02137b48, filled by __sinit_ov096_0213770c, which this
 * file does not own. func_ov096_02135878's ROM signature carried a
 * dead first argument (r0) that was never read -- as a member its
 * real argument lands in r1 either way.
 *
 * deslop leftovers:
 *   - dCcAc_c::Init, dBgCh_Actr::Init, DropShadowRadHeight and
 *   IsTooFarAwayFromPlayer take Fix12 by value, so the calls stay
 *   computed names (InitResources, Behavior, func_ov096_02135efc).
 *   - func_02038414 is the veneer onto
 *   dBgCh_Actr::UpdateDiscreteNoLava_2 (func_ov096_02136434,
 *   func_ov096_021360c4, func_ov096_02135e2c).
 *   - Particle::System::New, NewUnkCallback818, NewSimple,
 *   RunningSlidingDustAt and Player::Hurt take Fix12 by value
 *   (func_ov096_02136434, func_ov096_02135948, func_ov096_02136134,
 *   func_ov096_021365d4).
 *   - func_ov096_02135e2c copies the floor and wall collision normals;
 *   the wall normal is unused.
 *   - gSanboHeadModelFile and gSanboBodyModelFile are the head
 *   and segment file handles (InitResources LoadFile, CleanupResources
 *   Release). g_profile_SANBO stays in its own file.
 *   - unk_3a8 is the head byte func_ov096_0213670c reads to
 *   choose a 0 or 0x5a regrowth delay. func_ov096_021365d4 clears it
 *   once three segments exist. It stays unk_ until that role is
 *   measured past those two stores.
 *   - unk_0a4 and unk_0ac keep dActor_c's names.
 *   func_ov096_02135e2c multiplies them by the floor normal.
 *   - func_ov096_02135948 keeps five int[3] positions. One
 *   Vector3 in their place changes that function.
 *   - data_02082214 is arm9's sine table. data_020a0e68 is the
 *   scratch matrix func_ov096_02135efc fills before copying it onto
 *   the model. Actor id 0x135 is only the id that function shoves
 *   the segment away from.
 */
/* common.h first: Matrix4x3 must stay the flat s32[12] spelling. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daSanbo_c_AssetId {
    ASSET_SANBO_BODY_BMD = 0x041a,
    ASSET_SANBO_HEAD_BMD = 0x041b
};

#include "common.h"
#include "daSanbo_c.h"
#include "dBgPi.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "decl_common.h"
#include "Player.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"

enum { SANBO_HEAD = 0xf0, SANBO_BODY = 0xf1 };

namespace cstd { int fdiv(int, int); }


extern "C" {
extern void Vec3_Asr(struct Vector3* d, struct Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j( void* self, void* sm, void* mtx, Fix12i fx, int t, u32 u);
extern struct Matrix4x3 data_020a0e68;
extern short Vec3_HorzAngle(const void* a, const void* b);
extern int RandomIntInternal(void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int data_0209e650[];
extern s16 data_02082214[];
extern int Vec3_HorzDist(const void* a, const void* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_02038414(void *p);
void UnloadBlueCoinModel(void *);
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *c, int d);
void LoadBlueCoinModel(void* actor);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int r, int h, unsigned int d, unsigned int e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int b, int c, void* v, int e);
extern Matrix4x3 IDENTITY_MATRIX4X3;
}

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * pair (func_02017acc / func_02017ab4), spelled through a declared-only
 * subclass so the static initializer names the real entry points. */
struct SanboModelFilePtr : SharedFilePtr {
    u32 words[2];
    SanboModelFilePtr(u32 fileID);
    ~SanboModelFilePtr();
};
extern SanboModelFilePtr gSanboHeadModelFile;
extern SanboModelFilePtr gSanboBodyModelFile;
extern daSanbo_c::StateFunc data_ov096_02137b48[];

/* Both profiles construct this class. The synthesized ctor stores
 * `_ZTV9daSanbo_c + 2`; do not restate a file-local `extern _ZTV`. */

// @symbol daSanbo_c_classInit_SANBO
extern "C" daSanbo_c *daSanbo_c_classInit_SANBO(void)
{
    return new daSanbo_c();
}

// @symbol daSanbo_c_classInit_SANBO_BODY
/* Same class, SANBO_BODY profile (actor id 0xf1). */
extern "C" daSanbo_c *daSanbo_c_classInit_SANBO_BODY(void)
{
    return new daSanbo_c();
}

// @symbol _ZN9daSanbo_c13OnTurnIntoEggER6Player
/* Only the head pays out a coin. Every piece then destroys itself. */
void daSanbo_c::OnTurnIntoEgg(Player &player)
{
    int flag = (actorID == SANBO_HEAD);
    if (flag)
        GivePlayerCoins(player, 1, 2);
    MarkForDestruction();
}

// @symbol _ZN9daSanbo_c13InitResourcesEv
int daSanbo_c::InitResources()
{
    int t;

    t = (actorID == SANBO_HEAD);
    if (t != false) {
        void* m = Model::LoadFile(gSanboHeadModelFile);
        mModel.SetFile((BMD_File *)m, 1, 1);
        Model::LoadFile(gSanboBodyModelFile);
        LoadBlueCoinModel(this);
        unk_3a8 = 1;
    } else {
        t = (actorID == SANBO_BODY);
        if (t != false) {
            void* m = Model::LoadFile(gSanboBodyModelFile);
            if (mModel.SetFile((BMD_File *)m, 1, 1) == 0)
                return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x3c000, 0x78000, 0x200004, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x3c000, 0x3c000, 0, 0);

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x3000;

    t = (actorID == SANBO_HEAD);
    if (t != false) {
        mRootPosX = mPosX;
        mRootPosY = mPosY;
        mRootPosZ = mPosZ;
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        mPrevSegment = 0;
        mNextSegment = 0;
    } else {
        t = (actorID == SANBO_BODY);
        if (t != false) {
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mPrevSegment = (daSanbo_c *)dActor_c::FindWithID(param1);
            mNextSegment = 0;
            {
                int *p = &mPrevSegment->mRootPosX;
                mRootPosX = p[0];
                mRootPosY = p[1];
                mRootPosZ = p[2];
            }
        }
    }

    func_ov096_02136928(1);
    mMatrix = IDENTITY_MATRIX4X3;
    func_ov096_02135efc();
    return 1;
}

// @symbol _ZN9daSanbo_c8BehaviorEv
int daSanbo_c::Behavior()
{
    int s = mState;
    if (s != 2 && s != 5) {
        if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x5dc000)) return 1;
    }
    func_ov096_021368b4();
    MakeVanishLuigiWork(mdCcAc_c);
    func_ov096_02135efc();
    return 1;
}

// @symbol _ZN9daSanbo_c6RenderEv
int daSanbo_c::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if(b) return 1;
    Model *model = &mModel;
    model->Render(&mScale);
    return 1;
}

// @symbol _ZN9daSanbo_c16OnPendingDestroyEv
void daSanbo_c::OnPendingDestroy()
{
    int isBody = *(unsigned short *)((char *)&actorID);
    isBody = (isBody == SANBO_BODY);
    if (isBody) return;
    daSanbo_c *p = mNextSegment;
    if (!p) return;
    do {
        p->func_ov096_0213585c();
        p = p->mNextSegment;
    } while (p);
}

// @symbol _ZN9daSanbo_c16CleanupResourcesEv
int daSanbo_c::CleanupResources()
{
  int id = actorID;
  int a = (id == SANBO_HEAD);
  if (a) {
    UnloadBlueCoinModel(this);
    gSanboHeadModelFile.Release();
    gSanboBodyModelFile.Release();
  } else {
    a = (id == SANBO_BODY);
    if (a) {
      gSanboBodyModelFile.Release();
    }
  }
  return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_02136928Ei
void daSanbo_c::func_ov096_02136928(int a) {
    mStateFunctions = data_ov096_02137b48 + (a << 1);
    func_ov096_021368f0();
}

// @symbol _ZN9daSanbo_c19func_ov096_021368f0Ev
void daSanbo_c::func_ov096_021368f0()
{
    daSanbo_c::StateFunc *p = mStateFunctions;
    (this->**p)();
}

// @symbol _ZN9daSanbo_c19func_ov096_021368b4Ev
void daSanbo_c::func_ov096_021368b4()
{
    daSanbo_c::StateFunc *p = mStateFunctions + 1;
    (this->**p)();
}

// @symbol _ZN9daSanbo_c19func_ov096_021368a4Ev
int daSanbo_c::func_ov096_021368a4()
{
    mState = 0;
    return 1;
}


namespace Sound { void PlayBank0(unsigned int id, const Vector3 &pos); }

void ApproachLinear(int &dst, int target, int step);
bool ApproachLinear(short &value, short target, short step);

extern "C" {
void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int x, int y, int z, const void *dir, void *cb);
unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned a, unsigned b, int x, int y, int z, const Vector3_16f *dir);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *player, void *at, unsigned a, int fix, unsigned b, unsigned d, unsigned e);
}

// @symbol _ZN9daSanbo_c19func_ov096_02136754Ev
/* A segment tracks the one in front of it while that piece is still small. */
int daSanbo_c::func_ov096_02136754()
{
    daSanbo_c *next = mNextSegment;

    if (!next) {
        func_ov096_02136928(1);
    } else {
        int offset = 0xe000;
        int vy;
        int clamped;
        s16 *tbl;
        s16 cs;
        s16 sn;
        u16 ang;

        mPosX = next->mPosX;
        next = mNextSegment;
        mPosZ = next->mPosZ;

        vy = mVertSpeed - 0x2000;
        clamped = mTerminalVelocity;
        if (vy >= clamped)
            clamped = vy;
        mVertSpeed = clamped;
        mPosY += mVertSpeed;

        next = mNextSegment;
        if (mPosY < next->mPosY + 0x6e000) {
            mPosY = next->mPosY + 0x6e000;
            mVertSpeed = 0;
        }

        next = mNextSegment;
        tbl = data_02082214;
        mAngleY = next->mAngleY;
        next = mNextSegment;
        s16 segAng = (s16)(next->mSegAngY + 0x13000);
        mSegAngY = segAng;
        ang = (u16)mSegAngY;
        cs = tbl[(ang >> 4) * 2];
        mOffsetX = FX_MUL(cs, offset);
        mOffsetY = 0;
        ang = (u16)mSegAngY;
        sn = tbl[(ang >> 4) * 2 + 1];
        mOffsetZ = FX_MUL(sn, offset);
    }
    func_ov096_02135948();
    mdCcAc_c.Clear();
    mdCcAc_c.dCc_c::Update();
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_0213670cEv
/* unk_3a8 on the head picks a zero delay or a 90-frame one. */
void daSanbo_c::func_ov096_0213670c()
{
    daSanbo_c *head = func_ov096_021357b4();
    u8 timer = head->unk_3a8;

    if (timer != 0)
        timer = 0;
    else
        timer = 0x5a;
    mTimer = timer;
    timer = mTimer;
    if (timer == 0) {
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
    }
    mState = 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_021365d4Ev
/* Grow this segment, then sprout the next one when the timer expires. */
int daSanbo_c::func_ov096_021365d4()
{
    int scale = mScaleX;
    int copied;

    ApproachLinear(scale, 0x1000, 0x12c);
    copied = scale;
    mScaleX = copied;
    mScaleY = copied;
    mScaleZ = copied;
    mSegAngY = (s16)(mSegAngY + 0x500);
    mOffsetX = 0;
    mOffsetY = 0;
    mOffsetZ = 0;
    func_ov096_021358c8();
    if (func_ov096_02135838() < 4) {
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            dActor_c *spawned = dActor_c::Spawn(SANBO_BODY, uniqueID, mPos, &mAngle, mAreaId, -1);
            if (spawned != 0) {
                mNextSegment = (daSanbo_c *)spawned;
                func_ov096_02136928(4);
                if (func_ov096_02135838() >= 3)
                    func_ov096_021357b4()->unk_3a8 = 0;
            }
        }
    }
    UpdatePos(&mdCcAc_c);
    func_ov096_02135e2c(&mWithMeshClsn);
    func_ov096_02135948();
    mdCcAc_c.Clear();
    mdCcAc_c.dCc_c::Update();
    if (mWithMeshClsn.IsOnGround() != 0) {
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
            mPosX, mPosY, mPosZ);
    }
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_02136534Ev
int daSanbo_c::func_ov096_02136534()
{
    int head = (actorID == SANBO_HEAD);

    if (head != 0) {
        dActor_c::Spawn(0x122, 2, mPos, (Vector3_16 *)0, mAreaId, -1);
    }
    func_ov096_02135800();
    mPrevAngleY = Vec3_HorzAngle(&mHitActor->mPosX, &mPosX);
    mVertSpeed = 0x14000;
    if (mHitActor->param1 == 2)
        mHorzSpeed = 0x28000;
    else
        mHorzSpeed = 0x14000;
    mState = 2;
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_02136434Ev
int daSanbo_c::func_ov096_02136434()
{
    struct Vector3 pos;
    Particle::System *p0;
    Particle::System *p1;
    int x;
    int y;
    int z;

    UpdatePos(&mdCcAc_c);
    if (mParticle0 != 0 && mParticle1 != 0) {
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x3c000;
        ((int *)&pos)[0] = x;
        ((int *)&pos)[1] = y;
        ((int *)&pos)[2] = z;
        mParticle0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticle0, 0x13a, pos.x, pos.y, pos.z, 0, 0);
        mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            mParticle1, 0x13b, pos.x, pos.y, pos.z, 0);
        p0 = Particle::System::FromUniqueID(mParticle0);
        p1 = Particle::System::FromUniqueID(mParticle1);
        if (p0)
            p0->callbackScale = 0x7fff;
        if (p1)
            p1->callbackScale = 0x7fff;
    }
    func_02038414(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() != 0)
        func_ov096_0213585c();
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_0213640cEv
int daSanbo_c::func_ov096_0213640c()
{

    func_ov096_02135800();
    mHorzSpeed = 0;
    mState = 3;
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_021363c4Ev
int daSanbo_c::func_ov096_021363c4()
{
    int flags = mFlags;
    int held = (flags & 0x20000) ? 1 : 0;

    if (held == 0) {
        held = (flags & 0x40000) ? 1 : 0;
        if (held == 0)
            func_ov096_0213585c();
    }
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_021363b4Ev
int daSanbo_c::func_ov096_021363b4()
{
    mState = 4;
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_02136264Ev
/* Follow the segment ahead. Once its scale reaches full size, settle. */
int daSanbo_c::func_ov096_02136264()
{
    daSanbo_c *next = mNextSegment;

    if (!next) {
        func_ov096_02136928(1);
    } else {
        int fullHeight = 0x6e000;
        int scale = next->mScaleX;
        int raised = FX_MUL(scale, fullHeight);
        int offset = 0xe000;
        s16 *tbl;
        s16 cs;
        s16 sn;
        u16 ang;

        mPosX = next->mPosX;
        next = mNextSegment;
        mPosY = raised + next->mPosY;
        next = mNextSegment;
        tbl = data_02082214;
        mPosZ = next->mPosZ;
        next = mNextSegment;
        mAngleY = next->mAngleY;
        next = mNextSegment;
        s16 segAng = (s16)(next->mSegAngY + 0x13000);
        mSegAngY = segAng;
        ang = (u16)mSegAngY;
        cs = tbl[(ang >> 4) * 2];
        mOffsetX = FX_MUL(cs, offset);
        mOffsetY = 0;
        ang = (u16)mSegAngY;
        sn = tbl[(ang >> 4) * 2 + 1];
        mOffsetZ = FX_MUL(sn, offset);
        if (raised == fullHeight)
            func_ov096_02136928(0);
    }
    func_ov096_02135948();
    mdCcAc_c.Clear();
    mdCcAc_c.dCc_c::Update();
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_02136134Ev
int daSanbo_c::func_ov096_02136134()
{
    int head = (actorID == SANBO_HEAD);
    int rnd;

    if (head) {
        dActor_c::Spawn(0x122, 2, mPos, 0, mAreaId, -1);
    }
    Sound::PlayBank0(9, mCamSpacePos);
    func_ov096_02135800();
    mPrevAngleY = Vec3_HorzAngle(&mHitActor->mPosX, &mPosX);
    rnd = RandomIntInternal(data_0209e650);
    mPrevAngleY = (s16)(mPrevAngleY + ((rnd & 0x7fff) - 0x4000));
    mAngleY = (s16)(mPrevAngleY + 0x8000);
    mVertSpeed = 0x28000;
    mHorzSpeed = 0xa000;
    mTimer = 0x2d;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, mPosX,
        mPosY + OnAimedAtWithEgg(), mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, mPosX,
        mPosY + OnAimedAtWithEgg(), mPosZ);
    mState = 5;
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_021360c4Ev
int daSanbo_c::func_ov096_021360c4()
{
    int head = (actorID == SANBO_HEAD) ? 1 : 0;

    if (head != 0)
        mAngleX = (s16)(mAngleX - 0x1000);
    UpdatePos(&mdCcAc_c);
    func_02038414(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() || !DecIfAbove0_Byte(&mTimer))
        func_ov096_0213585c();
    return 1;
}

// @symbol _ZN9daSanbo_c19func_ov096_02135efcEv
void daSanbo_c::func_ov096_02135efc()
{
    struct Vector3 cam;
    int head;

    if (mState == 5) {
        head = (actorID == SANBO_HEAD);
        if (head) {
            int y;

            Vec3_Asr(&cam, &mPos, 3);
            Matrix4x3_FromTranslation(&data_020a0e68, cam.x, cam.y, cam.z);

            y = OnAimedAtWithEgg() >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y, 0);
            Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
                mAngleX, mAngleY, mAngleZ);

            y = (-OnAimedAtWithEgg()) >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y, 0);
            mModel.mat4x3 = data_020a0e68;

            mMatrix.m[9] = mPosX >> 3;
            mMatrix.m[10] = mPosY >> 3;
            mMatrix.m[11] = mPosZ >> 3;

            _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
                this, &mShadowModel, &mMatrix, 0xa0000, 0x2bc000, 0xf);
            return;
        }
    }

    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9] = (mPosX + mOffsetX) >> 3;
    mModel.mat4x3.m[10] = (mPosY + 0x3c000) >> 3;
    mModel.mat4x3.m[11] = (mPosZ + mOffsetZ) >> 3;

    if (mNextSegment == 0 || mState == 2 || mState == 5) {
        _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
            this, &mShadowModel, &mModel.mat4x3, 0x82000, 0x2bc000, 0xf);
    }
}

// @symbol _ZN9daSanbo_c19func_ov096_02135e2cEP10dBgCh_Actr
void daSanbo_c::func_ov096_02135e2c(dBgCh_Actr *clsn)
{
    int n0[3];
    int n1[3];

    func_02038414(clsn);
    if (clsn->IsOnGround()) {
        static_cast<dBgPi *>(clsn->GetFloorResult())->surface.CopyNormalTo(*(Vector3 *)n0);
        if (n0[1] != 0) {
            int x = FX_MUL(n0[0], unk_0a4);
            int z = FX_MUL(n0[2], unk_0ac);
            int dot = x + z;
            int slope = cstd::fdiv(dot, n0[1]);
            mVertSpeed = -(slope + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        static_cast<dBgPi *>(clsn->GetWallResult())->surface.CopyNormalTo(*(Vector3 *)n1);
    }
}

#include "decl_Actor.h"
#include "decl_Player.h"

// @symbol _ZN9daSanbo_c19func_ov096_02135948Ev
void daSanbo_c::func_ov096_02135948()
{
    unsigned int hitId;
    dActor_c *other;
    int radius;
    int angle;
    int index;
    s16 cs;
    int dist;
    s16 sn;
    int dist2;
    unsigned int flags;
    int state;
    daSanbo_c *prev;
    struct Vector3 pos;
    /* Five arrays on purpose. One shared int[3] changes this function. */
    int vv1[3];
    int vv2[3];
    int vv3[3];
    int vv4[3];
    int vv5[3];
    int x;
    int y;
    int z;

    if (FindEgg(mdCcAc_c) != 0 ||
        FindExplosionActor(mdCcAc_c) != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        func_ov096_02135800();
        {
            int head = (actorID == SANBO_HEAD);
            if (head)
                dActor_c::Spawn(0x122, 2, mPos, 0, mAreaId, -1);
        }
        func_ov096_0213585c();
        return;
    }

    hitId = mdCcAc_c.otherOwner;
    if (hitId == 0)
        return;

    other = dActor_c::FindWithID(hitId);
    if (other == 0)
        return;

    if (mState == 1) {
        int shove = (other->actorID == 0x135);
        if (shove) {
        radius = mdCcAc_c.radius;
        if (Vec3_HorzDist(&mPosX, &other->mPosX) < radius + 0x1a9000) {
            angle = Vec3_HorzAngle(&other->mPosX, &mPosX);
            index = ((unsigned short)angle >> 4) * 2;
            cs = data_02082214[index];
            dist = mdCcAc_c.radius + 0x1a9000;
            mPosX = other->mPosX + FX_MUL(dist, cs);
            sn = data_02082214[index + 1];
            dist2 = mdCcAc_c.radius + 0x1a9000;
            mPosZ = other->mPosZ + FX_MUL(dist2, sn);
        }
        }
    }

    {
        int playerId = (other->actorID == 0xbf);
        if (!playerId)
            return;
    }

    flags = mdCcAc_c.hitFlags;
    if (!(flags & 0x8000)) {
        Player *player = (Player *)other;

        if (flags & 0x40000) {
            x = mPosX;
            z = mPosZ;
            y = mPosY + 0x3c000;
            ((int *)&pos)[0] = x;
            ((int *)&pos)[1] = y;
            ((int *)&pos)[2] = z;
            mParticle0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticle0, 0x13a, pos.x, pos.y, pos.z, 0, 0);
            mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticle1, 0x13b, pos.x, pos.y, pos.z, 0);
            Sound::PlayBank0(9, mCamSpacePos);
            mHitActor = player;
            func_ov096_02136928(2);
        } else if ((flags & 0x26fe0) || player->IsOnShell() != 0 ||
                   player->mIsMetal != 0) {
            Sound::PlayBank0(9, mCamSpacePos);
            mHitActor = player;
            func_ov096_02136928(2);
        } else if (mdCcAc_c.hitFlags & 0x10) {
            mHitActor = player;
            player->IncMegaKillCount();
            func_ov096_02136928(5);
        } else {
            state = mState;
            if (state == 0) {
                prev = mPrevSegment;
                if (prev == 0) {
                    if (mVertSpeed == 0) {
                        vv1[0] = mPosX;
                        vv1[1] = mPosY;
                        vv1[2] = mPosZ;
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                            player, vv1, 2, 0xc000, 1, 0, 1);
                    }
                } else if (mVertSpeed == 0 && prev->mVertSpeed == 0) {
                    vv2[0] = mPosX;
                    vv2[1] = mPosY;
                    vv2[2] = mPosZ;
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                        player, vv2, 2, 0xc000, 1, 0, 1);
                }
            } else if (state == 1) {
                if (mPrevSegment == 0) {
                    if (mWithMeshClsn.IsOnGround() != 0) {
                        vv3[0] = mPosX;
                        vv3[1] = mPosY;
                        vv3[2] = mPosZ;
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                            player, vv3, 2, 0xc000, 1, 0, 1);
                    }
                } else if (mWithMeshClsn.IsOnGround() != 0 &&
                           mPrevSegment->mVertSpeed == 0) {
                    vv4[0] = mPosX;
                    vv4[1] = mPosY;
                    vv4[2] = mPosZ;
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                        player, vv4, 2, 0xc000, 1, 0, 1);
                }
            } else {
                vv5[0] = mPosX;
                vv5[1] = mPosY;
                vv5[2] = mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                    player, vv5, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    {
        int inMouth = (mFlags & 0x20000) != 0;
        if (inMouth)
            func_ov096_02136928(3);
    }
}

// @symbol _ZN9daSanbo_c19func_ov096_021358c8Ev
void daSanbo_c::func_ov096_021358c8()
{
    Player *player = ClosestNonVanishPlayer();
    void *target;
    int dist;
    short ang;
    int turn;

    if (player) {
        dist = Vec3_HorzDist(&mRootPosX, &player->mPosX);
        if (dist < 0x3e8000)
            target = &player->mPosX;
        else
            target = &mRootPosX;
    } else {
        target = &mRootPosX;
    }
    dist = Vec3_HorzDist(&mPosX, target);
    ang = Vec3_HorzAngle(&mPosX, target);
    ApproachLinear(mAngleY, ang, (short)0x320);
    turn = func_ov096_02135878(dist);
    mPrevAngleY = (short)(mAngleY + turn);
}

// @symbol _ZN9daSanbo_c19func_ov096_02135878Ei
int daSanbo_c::func_ov096_02135878(int x)
{
    if (x > 0x190000)
        return 0;
    int rate = cstd::fdiv(0x4000, 0xc8000);
    int drop = FX_MUL(rate, x);
    int r = 0x8000 - drop;
    return (short)r;
}

// @symbol _ZN9daSanbo_c19func_ov096_0213585cEv
void daSanbo_c::func_ov096_0213585c()
{

    PoofDust();
    MarkForDestruction();
}

// @symbol _ZN9daSanbo_c19func_ov096_02135838Ev
int daSanbo_c::func_ov096_02135838()
{
    daSanbo_c *seg = mPrevSegment;
    int count = 0;

    if (seg == 0)
        return count;
    do {
        seg = seg->mPrevSegment;
        count = count + 1;
    } while (seg != 0);
    return count;
}

// @symbol _ZN9daSanbo_c19func_ov096_02135800Ev
void daSanbo_c::func_ov096_02135800()
{
    daSanbo_c *next;
    daSanbo_c *prev;
    int head = (actorID == SANBO_HEAD);

    if (head)
        return;
    next = mNextSegment;
    prev = mPrevSegment;
    prev->mNextSegment = next;
    next = mNextSegment;
    if (next)
        next->mPrevSegment = mPrevSegment;
}

// @symbol _ZN9daSanbo_c19func_ov096_021357b4Ev
daSanbo_c *daSanbo_c::func_ov096_021357b4()
{
    daSanbo_c *prev = mPrevSegment;

    if (prev == 0)
        return this;
    while (prev) {
        unsigned int notHead = (prev->actorID != SANBO_HEAD) ? 1u : 0u;
        if (!notHead)
            return prev;
        prev = prev->mPrevSegment;
    }
    return 0;
}

// @symbol _ZN9daSanbo_c16OnAimedAtWithEggEv
/* Constant height used by the segment particles and the head's shadow. */
int daSanbo_c::OnAimedAtWithEgg()
{
    return 245760;
}

// @symbol _ZN9daSanbo_c13OnYoshiTryEatEv
/* Returns a constant. The body ignores `this`. */
int daSanbo_c::OnYoshiTryEat()
{
    return 4;
}

/* D1 and D0 come from the inline destructor in include/daSanbo_c.h.
 * An out-of-line body here would emit D2 and put D0 below D1. */

// @symbol __sinit_d_a_sanbo.cpp
SanboModelFilePtr gSanboHeadModelFile(ASSET_SANBO_HEAD_BMD);   /* head model */
SanboModelFilePtr gSanboBodyModelFile(ASSET_SANBO_BODY_BMD);   /* segment model */

/* The twelve state PMF records live in .data (0x02137920..0x02137980); the
 * static initializer copies them into the .bss table below, an init/update
 * pair per state. */
extern "C" daSanbo_c::StateFunc data_ov096_02137948;
extern "C" daSanbo_c::StateFunc data_ov096_02137940;
extern "C" daSanbo_c::StateFunc data_ov096_02137950;
extern "C" daSanbo_c::StateFunc data_ov096_02137928;
extern "C" daSanbo_c::StateFunc data_ov096_02137930;
extern "C" daSanbo_c::StateFunc data_ov096_02137920;
extern "C" daSanbo_c::StateFunc data_ov096_02137978;
extern "C" daSanbo_c::StateFunc data_ov096_02137970;
extern "C" daSanbo_c::StateFunc data_ov096_02137938;
extern "C" daSanbo_c::StateFunc data_ov096_02137968;
extern "C" daSanbo_c::StateFunc data_ov096_02137960;
extern "C" daSanbo_c::StateFunc data_ov096_02137958;

daSanbo_c::StateFunc data_ov096_02137b48[12] = {
    data_ov096_02137948, data_ov096_02137940,   /* state 0 */
    data_ov096_02137950, data_ov096_02137928,   /* state 1 */
    data_ov096_02137930, data_ov096_02137920,   /* state 2 */
    data_ov096_02137978, data_ov096_02137970,   /* state 3 */
    data_ov096_02137938, data_ov096_02137968,   /* state 4 */
    data_ov096_02137960, data_ov096_02137958,   /* state 5 */
};