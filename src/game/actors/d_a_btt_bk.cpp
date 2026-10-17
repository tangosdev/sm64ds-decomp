//cpp
/* BATTA_BLOCK crate. Drops onto the ground, falls and bounces, then rides a
 * carrier. A Mega Mario hit, or the flag test in state 1, breaks it into
 * dust and five coins.
 *
 * _ZTS9daBttBk_c ov080:0x0212815c, _ZTI 0x02128168 (base _ZTI8dActor_c
 * 0x0208e390), _ZTV 0x02128198. Key function ~daBttBk_c, written first under
 * defer_codegen off: D1 0x02124a20, D0 0x02124a68, D2 deadstripped. Was
 * CrazedCrate. .text 0x02124a20..0x02125404. The six state bodies are the
 * pointer-to-member constants at 0x0212812c..0x02128158.
 */
// leftovers:
// - SpawnCoins, Particle::System::NewSimple, DropShadowScaleXYZ, dCcAc_c::Init
//   and dBgCh_Actr::Init stay mangled extern "C". SpawnCoins as a member, with
//   Fix12<int> by value, grew func_ov080_02124acc (0x170 to 0x17c) and
//   func_ov080_02124edc (0x110 to 0x11c). NewSimple has no header method.
// - mShadowMtx stays u8[0x30] and is cast to Matrix4x3. The header names the
//   array, not a Matrix4x3.
// - func_ov080_02124e60 stores 0 through *(int *)pad_0d0, the pad in front of
//   mModel.
// - func_02010304, Matrix4x3_ApplyInPlaceToTranslation, Vec3_LslInPlace,
//   Matrix4x3_FromRotationY and dBgCh_Actr_UpdateDiscreteNoLava_veneer stay
//   free calls defined outside this TU.


/* Runtime asset handles resolved through the overlay 0 file table. */
enum daBttBk_c_AssetId {
    ASSET_BATTA_BLOCK_BMD = 0x02ac
};

#include "decl_common.h"
#include "daBttBk_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "Player.h"
#include "nitro/fx/fx.h"

struct Mtx43 { int a[12]; };
typedef char Mtx43_size_must_be_0x30[sizeof(Mtx43) == 0x30 ? 1 : -1];

/* State-table rows are 8-byte mwcc pointer-to-member pairs. */
typedef void (daBttBk_c::*PMF)();

extern "C" {
void *func_02010304(void *a, void *b);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const Vector3 &v, unsigned int n, int vel, short unk);
void Matrix4x3_ApplyInPlaceToTranslation(Mtx43 *m, int x, int y, int z);
void Vec3_LslInPlace(Vector3 *v, int sh);
void Matrix4x3_FromRotationY(void *m, int angle);
void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int r, int t5, int t6, unsigned int u);
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *w);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
}

extern Mtx43 data_020a0e68;
extern s16 data_02082214[];

/* Model-file handle. The derived constructor and destructor are the retail
 * SharedFilePtr pair; this TU does not define them. */
struct BttBkModelFilePtr : SharedFilePtr {
    u32 words[2];

    BttBkModelFilePtr(u32 fileID);
    ~BttBkModelFilePtr();
};

typedef int (daBttBk_c::*daBttBk_StateFn)();
struct daBttBk_StateRow {
    daBttBk_StateFn enter;
    daBttBk_StateFn update;
};

extern "C" BttBkModelFilePtr gBattaBlockModelFile;
extern "C" daBttBk_StateRow data_ov080_0212847c[3];

#pragma defer_codegen off

// @symbol _ZN9daBttBk_cD1Ev
// @symbol _ZN9daBttBk_cD0Ev
/* Empty on purpose. mwccarm destroys the members, then ~dActor_c, and
 * emits retail D1 followed by D0. */
daBttBk_c::~daBttBk_c()
{
}

// @symbol _ZN9daBttBk_c13OnYoshiTryEatEv
s32 daBttBk_c::OnYoshiTryEat()
{
    return 1;
}

/* Collision reaction, run from state 0's update: a carrier found by
 * func_02010304 is stored in mCarrier and enters state 1; otherwise, against
 * actor id 0xbf, mFlags bit 17 enters state 2 and a hitFlags bit 4 contact
 * breaks the crate (Player::IncMegaKillCount, dust, five coins). */
// @symbol _ZN9daBttBk_c19func_ov080_02124accEv
void daBttBk_c::func_ov080_02124acc()
{
    if (mdCcAc_c.otherOwner == 0) return;
    void *p = func_02010304(this, &mdCcAc_c);
    if (p != 0) { mCarrier = (dActor_c *)p; func_ov080_0212513c(1); return; }
    dActor_c *other = dActor_c::FindWithID(mdCcAc_c.otherOwner);
    if (other == 0) return;
    int isId0xbf = (int)(other->actorID == 0xbf);
    if (isId0xbf == 0) return;
    int inYoshiMouthA = (int)((mFlags & 0x20000) != 0);
    if (inYoshiMouthA) { func_ov080_0212513c(2); return; }
    if ((mdCcAc_c.hitFlags & 0x10) == 0) return;
    ((Player *)other)->IncMegaKillCount();
    Vector3 v; Vector3 v2; Vector3 v3;
    int y0 = mPosY;
    int z = mPosZ;
    int x = mPosX;
    int y = y0 + 0x32000;
    v.x = x; v.y = y; v.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, v.x, v.y, v.z);
    v2.x = v.x;
    v2.y = v.y;
    v2.z = v.z;
    PoofDustAt(v2);
    int t = mPosY + 0x64000;
    v3.x = v.x;
    v.y = t;
    v3.y = t;
    v3.z = v.z;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, v3, 5, 0xf000, 0);
    Sound::PlayBank3(0x41, mCamSpacePos);
    MarkForDestruction();
}

/* Model and shadow matrices: follows the carrier's matrix while carried,
 * then rebuilds the model matrix (mModel.mat4x3) and the drop shadow
 * matrix (mShadowMtx). */
// @symbol _ZN9daBttBk_c19func_ov080_02124c3cEv
void daBttBk_c::func_ov080_02124c3c()
{
    Vector3 t;
    Vector3 pos;
    int flags = mFlags;
    int inYoshiMouthB = (flags & 0x40000) != 0;
    if (inYoshiMouthB != false) return;
    dActor_c *carrier = mCarrier;
    if (carrier != 0) {
        int hasBit14 = (flags & 0x4000) != 0;
        if (hasBit14 != false) {
            if (carrier->mCarryMatrix != 0) {

                int tx = 0xc000, ty = 0x2000, tz = 0;
                t.x = tx; t.y = ty; t.z = tz;
                carrier = mCarrier;
                data_020a0e68 = *(Mtx43 *)carrier->mCarryMatrix;
                Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, t.x, t.y, t.z);

                mPosX = data_020a0e68.a[9];
                mPosY = data_020a0e68.a[10];
                mPosZ = data_020a0e68.a[11];
                Vec3_LslInPlace(&mPos, 3);
            }
        }
    }
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y = pos.y + 0x14000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(pos, 0);
    int groundY = pos.y;
    if (rg.DetectClsn()) {
        groundY = rg.clsnY;
    }
    Matrix4x3_FromRotationY(mShadowMtx, mAngleY);
    ((Matrix4x3 *)mShadowMtx)->m[9] = mPosX >> 3;
    ((Matrix4x3 *)mShadowMtx)->m[10] = groundY >> 3;
    ((Matrix4x3 *)mShadowMtx)->m[11] = mPosZ >> 3;
    {
        s16 a = mAngleX;
        int sv = data_02082214[((unsigned short)(short)(a << 1) >> 4) * 2];
        if (sv < 0) sv = -sv;
        int result = FX_MUL(sv, 0x28000);
        _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
            this, &mShadowModel, mShadowMtx, 0x96000, 0x32000, result + 0x96000, 0xf);
    }
}

/* State 2 update: back to state 0 once neither flag bit 17 nor 18 is set.
 * The 0 stored through pad_0d0 is the state passed to func_ov080_0212513c. */
// @symbol _ZN9daBttBk_c19func_ov080_02124e60Ev
int daBttBk_c::func_ov080_02124e60()
{
    int flags = mFlags;
    int inYoshiMouthA = (flags & 0x20000) ? 1 : 0;
    if (inYoshiMouthA != 0) goto done;
    int inYoshiMouthB = (flags & 0x40000) ? 1 : 0;
    if (inYoshiMouthB != 0) goto done;
    *(int *)pad_0d0 = 0;
    func_ov080_0212513c(0);
done:
    return 1;
}

/* State 2 enter. */
// @symbol _ZN9daBttBk_c19func_ov080_02124eb0Ev
int daBttBk_c::func_ov080_02124eb0()
{
    mHorzSpeed = 0;
    mdCcAc_c.Clear();
    mState = 2;
    return 1;
}

/* State 1 update: copies the carrier's angles; breaks the crate unless
 * mFlags bit 8 is set and bit 13 clear. */
// @symbol _ZN9daBttBk_c19func_ov080_02124edcEv
int daBttBk_c::func_ov080_02124edc()
{
    mAngleX = mCarrier->mAngleX;
    mAngleY = mCarrier->mAngleY;
    mPrevAngleX = mAngleX;
    mPrevAngleY = mAngleY;
    {
        int flags = mFlags;
        int hasBit8 = (int)((flags & 0x100) != 0);
        if (hasBit8 != 0) {
            int hasBit13 = (int)((flags & 0x2000) != 0);
            if (hasBit13 == 0) goto clear;
        }
        {
            Vector3 vec;
            Vector3 vec2;
            int x = mPosX;
            int z = mPosZ;
            int y = mPosY + 0xb4000;
            vec.x = x;
            vec.y = y;
            vec.z = z;
            vec2.x = vec.x;
            vec2.y = vec.y;
            vec2.z = vec.z;
            PoofDustAt(vec2);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);

            {
                int ybase = mPosY;
                int xx = vec.x;
                int y2 = ybase + 0x64000;
                int zz = vec.z;
                Vector3 v3;
                v3.x = xx;
                v3.z = zz;
                vec.y = y2;
                v3.y = y2;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, v3, 5, 0xf000, 0);
            }

            Sound::PlayBank3(0x41, mCamSpacePos);
            MarkForDestruction();
        }
    }
clear:
    mdCcAc_c.Clear();
    return 1;
}

/* State 1 enter. */
// @symbol _ZN9daBttBk_c19func_ov080_02124fecEv
int daBttBk_c::func_ov080_02124fec()
{
    u32 *flagsPtr = &mFlags;
    int one = 1;
    mState = one;
    *flagsPtr &= ~3;
    return one;
}

/* State 0 update: fall, bounce at 60% on landing, collide. */
// @symbol _ZN9daBttBk_c19func_ov080_0212500cEv
int daBttBk_c::func_ov080_0212500c()
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround()) {
        int v = mVertSpeed * -0x3c;
        mVertSpeed = v / 100;
    } else if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0xc000;
    }
    UpdatePos(&mdCcAc_c);
    func_ov080_02124acc();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* State 0 enter. */
// @symbol _ZN9daBttBk_c19func_ov080_0212509cEv
int daBttBk_c::func_ov080_0212509c()
{
    mVertSpeed = 49152;
    mWithMeshClsn.SetLimMovFlag();
    mState = 0;
    return 1;
}

/* Runs the current state row's update function (second PMF). */
// @symbol _ZN9daBttBk_c19func_ov080_021250c8Ev
void daBttBk_c::func_ov080_021250c8()
{
    PMF *p = (PMF *)mStateRow + 1;
    (this->**p)();
}

/* Runs the current state row's enter function (first PMF). */
// @symbol _ZN9daBttBk_c19func_ov080_02125104Ev
void daBttBk_c::func_ov080_02125104()
{
    PMF *p = (PMF *)mStateRow;
    (this->**p)();
}

/* Changes state: points mStateRow at row i of the state table, then enters it. */
// @symbol _ZN9daBttBk_c19func_ov080_0212513cEi
void daBttBk_c::func_ov080_0212513c(int i)
{
    mStateRow = (char *)data_ov080_0212847c + (i << 4);
    func_ov080_02125104();
}

/* Slot 3: one shared model file handle to give back. */
// @symbol _ZN9daBttBk_c16CleanupResourcesEv
s32 daBttBk_c::CleanupResources()
{
    ((SharedFilePtr *)&gBattaBlockModelFile)->Release();
    return 1;
}

/* Slot 12: empty override. */
// @symbol _ZN9daBttBk_c16OnPendingDestroyEv
void daBttBk_c::OnPendingDestroy()
{
}

// @symbol _ZN9daBttBk_c6RenderEv
int daBttBk_c::Render()
{
    int flags = mFlags;
    flags = flags & 0x40000;
    flags = flags ? 1 : 0;
    if (flags) return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daBttBk_c8BehaviorEv
int daBttBk_c::Behavior()
{
    func_ov080_021250c8();
    func_ov080_02124c3c();
    return 1;
}

// @symbol _ZN9daBttBk_c13InitResourcesEv
int daBttBk_c::InitResources()
{
    Vector3 pos;
    void *file = Model::LoadFile(*(SharedFilePtr *)&gBattaBlockModelFile);
    mModel.SetFile((BMD_File *)file, 1, 1);
    mShadowModel.InitCuboid();
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000, 0x78000, 0x800004, 0x9010);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    {
        int spawnY;
        pos.x = mPosX;
        spawnY = mPosY;
        pos.y = spawnY;
        pos.z = mPosZ;
        pos.y = spawnY + 0xc8000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mCarrier = 0;
    func_ov080_0212513c(0);
    func_ov080_02124c3c();
    return 1;
}

// @symbol _ZN9daBttBk_c13OnTurnIntoEggER6Player
void daBttBk_c::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap()) {
        GivePlayerCoins(player, 5, 0);
    }
    Vector3 vec;
    Vector3 vec2;
    int x = mPosX;
    int z = mPosZ;
    int y = mPosY + 0xb4000;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec2.x = vec.x;
    vec2.y = vec.y;
    vec2.z = vec.z;
    PoofDustAt(vec2);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);
    Sound::PlayBank3(0x41, mCamSpacePos);
    MarkForDestruction();
}

// @symbol daBttBk_c_classInit
extern "C" daBttBk_c *daBttBk_c_classInit()
{
    return new daBttBk_c();
}

/* __sinit_d_a_btt_bk.cpp constructs the model handle, then copies the six
 * anonymous pointer-to-member descriptors into this table. */
BttBkModelFilePtr gBattaBlockModelFile(ASSET_BATTA_BLOCK_BMD);
daBttBk_StateRow data_ov080_0212847c[3] = {
    { &daBttBk_c::func_ov080_0212509c, &daBttBk_c::func_ov080_0212500c },
    { &daBttBk_c::func_ov080_02124fec, &daBttBk_c::func_ov080_02124edc },
    { &daBttBk_c::func_ov080_02124eb0, &daBttBk_c::func_ov080_02124e60 },
};
