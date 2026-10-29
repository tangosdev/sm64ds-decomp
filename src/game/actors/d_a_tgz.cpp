//cpp
namespace Particle {
struct Callback;
}
struct Vector3_16f;
/* daTgz_c -- the Spiny on the castle roof, ov077.
 *
 * Six PMF state records (func_ov077_02125e94 picks one). State 0 is the
 * spinning bounce that settles back into the walk (state 1); state 4 spins
 * the same way after its carrier lets go. State 5 is the knock-up: it springs
 * up at vertical speed 40.0, flips backward 0x1000 a frame, and on landing
 * (or 45 frames later) drops a coin, poofs and is destroyed. It caches the
 * water surface under it once, splashes on entry, and poofs away if it sinks
 * too far or drops below course 0x1c's floor. Outside states 4 and 5, while
 * it is more than 1500.0 from the player it stops updating and counts
 * mDespawnTimer down from 44 frames, then is destroyed with no poof.
 *
 * Cartridge names: _ZTS7daTgz_c at 0x02127948, _ZTI7daTgz_c at 0x02127954,
 * vtable address point _ZTV7daTgz_c at 0x02127984. Thirty-four functions,
 * 0x02124b64..0x0212624c, between daJgm_c and daPopoi_c.
 *
 * Load-bearing:
 *   Function order is ROM-ascending. `#pragma defer_codegen off` makes
 *   CodeWarrior emit each definition where it stands, and the destructor pair
 *   comes out D1 then D0. One ~daTgz_c() emits both; a hand-mangled D0 next
 *   to it is the mwccarm ICE (ELFgen.c:483).
 *   decl_common.h comes before any actor header, so Matrix4x3 stays the flat
 *   12-word struct. The nested spelling's Vector3 makes the copy in
 *   func_ov077_021251d0 call a destructor the cartridge does not.
 *
 * deslop leftovers:
 * - func_ov077_02124eb0 and func_ov077_021253a4: dActor_c::SpawnCoins is
 *   +12 (0x1f8->0x204, 0xdc->0xe8). Scalar extern kept.
 * - func_ov077_02125304: dActor_c::DropShadowRadHeight is +16 (0xa0->0xb0).
 * - func_ov077_02125480 and func_ov077_02125b1c: dExtAnmModel_c::SetAnim is +12
 *   (0xd0->0xdc, 0x98->0xa4). Scalar extern passes the u16 start frame.
 * - InitResources: SetAnim + dCcAc_c::Init + dBgCh_Actr::Init together are
 *   +36 (0x13c->0x160). dBgCh_Actr::Init's header mangles
 *   _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_, not the ROM Fix12
 *   symbol.
 * - func_ov077_021256b4 and func_ov077_02125830: carrier->mPosX/Y/Z is -4
 *   (0x17c->0x178, 0xac->0xa8). The ROM keeps add #0x5c then [0]/[4]/[8].
 * - decl_common.h already declares func_ov077_02124ce4, 02124eb0 and
 *   021250a8 as void* and 02125304 as char*. daTgz_c* is illegal
 *   overloading, so those four cast.
 */
#include "decl_common.h"
#include "nitro/fx/fx.h"
#include "daTgz_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgCh_Gnd.h"
#include "decl_Particle.h"
#include "dBgCh_Actr.h"
#include "cstd.h"
#include "math/Approach.h"
#include "SpawnInfo.h"

#pragma defer_codegen off

/* The BCA gTgzWalkAnimFile has loaded. */
#define TGZ_BCA (gTgzWalkAnimFile.filePtr)

/* Lakitu's model-file handles, owned and constructed by d_a_jgm.cpp's static
 * init. Same TU-local wrapper spelling as the owning TU. */
struct DaJgmModelFile : SharedFilePtr {
    DaJgmModelFile(u32 fileID);
    ~DaJgmModelFile();
};

/* Runtime asset handle, distinct from the NitroFS file ID. */
enum daTgz_c_AssetId { ASSET_TOGEZO_WALK_BCA = 0x042b };

/* The handle reaches its file through this wrapper's constructor and
 * destructor veneers; the manifest maps each to the retail destination.
 * Descriptive local type, not a recovered spelling. */
struct TgzAnimFile : SharedFilePtr {
    TgzAnimFile(u32 fileID);
    ~TgzAnimFile();
};

TgzAnimFile gTgzWalkAnimFile(ASSET_TOGEZO_WALK_BCA);

/* One {enter, update} record per state; func_ov077_02125e94 selects one and
 * func_ov077_02125e5c / 02125e20 call through it. State 1's entry returns
 * nothing, so its pointer is cast to the shared record type. */
typedef int (daTgz_c::*TgzState)();

struct TgzStateRecord {
    TgzState enter;
    TgzState update;
};

TgzStateRecord gTgzStates[6] = {
    {&daTgz_c::func_ov077_02125dd4, &daTgz_c::func_ov077_02125bb4},
    {(TgzState)&daTgz_c::func_ov077_02125b1c, &daTgz_c::func_ov077_02125a54},
    {&daTgz_c::func_ov077_02125a0c, &daTgz_c::func_ov077_02125908},
    {&daTgz_c::func_ov077_021258dc, &daTgz_c::func_ov077_02125830},
    {&daTgz_c::func_ov077_021256b4, &daTgz_c::func_ov077_02125550},
    {&daTgz_c::func_ov077_02125480, &daTgz_c::func_ov077_021253a4},
};

extern "C" {
extern DaJgmModelFile data_ov077_02127b48;
extern DaJgmModelFile data_ov077_02127b38;
int func_ov077_02124c28(daTgz_c *self);
void func_ov077_02124d08(daTgz_c *self, dBgCh_Actr *clsn);
void func_ov077_021251d0(daTgz_c *self);
void func_ov077_02125290(daTgz_c *self);
void func_ov077_02125e20(daTgz_c *self);
void func_ov077_02125e5c(daTgz_c *self);
void func_ov077_02125e94(daTgz_c *self, int state);
/* decl_common.h already declares these four as void* / char*. A second
 * extern "C" prototype with daTgz_c* is illegal overloading. */

int SurfaceInfo_TestFlag0x20(int *p);
void func_02012694(unsigned int id, const Vector3 *pos);
s32 func_02010844(void *unused, Vector3 *v, s16 angle);
short Vec3_HorzAngle(void *a, void *b);
void Vec3_Asr(void *dst, void *src, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
extern Matrix4x3 data_020a0e68;
unsigned char DecIfAbove0_Byte(unsigned char *p);
namespace Particle {
struct System {
    static void *NewSimple(unsigned int id, int x, int y, int z);
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
    static void NewBigSplash(Fix12i, Fix12i, Fix12i);
};
}
extern s16 data_02082214[];
void func_02035684(int *p, int v);
int RandomIntInternal(int *seed);
extern int data_0209e650;
unsigned int d_a_tgzNewHold(unsigned int uniqueID, unsigned int effectID, int x, int y, int z,
                            const void *dir, void *callback);
extern int data_0209f32c;
extern char IDENTITY_MATRIX4X3;
extern signed char data_0209f2f8;
}

// @symbol _ZN7daTgz_cD1Ev
/* Vtable slot 16. The compiler writes the whole body: one vtable store, the
 * members in reverse, then ~dActor_c. Slot 17 (D0) is the deleting variant of
 * this same definition. */
daTgz_c::~daTgz_c()
{
}

// @symbol _ZN7daTgz_c13OnYoshiTryEatEv
/* Vtable slot 18. */
int daTgz_c::OnYoshiTryEat()
{
    return 6;
}

// @symbol _ZN7daTgz_c16OnAimedAtWithEggEv
/* Vtable slot 29: the height an egg aims at (30.0). */
int daTgz_c::OnAimedAtWithEgg()
{
    return 122880;
}

// @symbol func_ov077_02124c28
/* Until one is found, probe for a water surface below the Spiny and cache
 * its height in mWaterY. Returns how far above it the Spiny is (0 while
 * nothing has been found). */
extern "C" int func_ov077_02124c28(daTgz_c *self)
{
    if (self->mWaterY == 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = self->mPosY;
            int z = self->mPosZ;
            int y2 = y + 0xc8000;
            int x = self->mPosX;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, self);
        rg.StartDetectingWater();
        if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0)
            self->mWaterY = rg.clsnY;
        else
            return 0;
        rg.StopDetectingWater();
    }
    return self->mPosY - self->mWaterY;
}

// @symbol func_ov077_02124ce4
/* Is the Spiny below the cached water surface? */
extern "C" int func_ov077_02124ce4(void *vc)
{
    daTgz_c *self = (daTgz_c *)vc;
    int waterY = self->mWaterY;
    if (waterY == 0)
        return 0;
    return waterY > self->mPosY;
}

// @symbol func_ov077_02124d08
/* Toxic floor destroys the Spiny. A sloped floor rewrites vertical speed
 * from mSpeedX/mSpeedZ and pitches mAngleX/mAngleZ to the normal. */
extern "C" void func_ov077_02124d08(daTgz_c *self, dBgCh_Actr *clsn)
{
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(clsn);
    if (clsn->IsOnGround()) {
        dBgCh_Gnd rc;
        {
            int actorY = self->mPosY;
            int pz = self->mPosZ;
            int py = actorY + 0xc8000;
            pos.x = self->mPosX;
            pos.y = py;
            pos.z = pz;
        }
        rc.StartDetectingToxic();
        rc.SetObjAndPos(pos, self);
        if (rc.DetectClsn()) {
            if (func_02037e20((int *)&rc.surface) != 0 && self->mPosY < rc.clsnY) {
                self->PoofDust();
                func_02012694(0xc4, &self->mCamSpacePos);
                self->MarkForDestruction();
                return;
            }
            {
                dBgPi *fr = clsn->GetFloorResult();
                fr->surface.CopyNormalTo(normal);
            }
            if (normal.y != 0) {
                self->mVertSpeed =
                    -(cstd::fdiv(FX_MUL(normal.x, self->mSpeedX) + FX_MUL(normal.z, self->mSpeedZ),
                                 normal.y) +
                      0x8000);
            }
            self->mAngleX = func_02010844(self, &normal, self->mAngleY);
            self->mAngleZ = func_02010844(self, &normal, (s16)(self->mAngleY - 0x4000));
        }
    }
    if (clsn->IsOnWall()) {
        dBgPi *wr = clsn->GetWallResult();
        wr->surface.CopyNormalTo(wallnormal);
    }
}

// @symbol func_ov077_02124eb0
/* Hit response while walking or spinning. Egg and the knock-up destroy or
 * launch; a shell / metal / attack hit in state 1 starts the chase. */
extern "C" void func_ov077_02124eb0(void *vc)
{
    daTgz_c *self = (daTgz_c *)vc;
    Player *player;
    int b;

    if (self->FindEgg(self->mdCcAc_c) != 0) {
        int v[3];
        v[0] = self->mPosX;
        v[1] = self->mPosY;
        v[2] = self->mPosZ;
        self->SpawnCoins(*(const Vector3 *)v, 1, 0x2000, 0);
        self->PoofDust();
        func_02012694(0xc4, &self->mCamSpacePos);
        self->MarkForDestruction();
        return;
    }

    {
        unsigned int id = self->mdCcAc_c.otherOwner;
        if (id == 0)
            return;
        player = (Player *)dActor_c::FindWithID(id);
    }
    if (player == 0)
        return;

    b = (int)(player->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((self->mFlags & 0x20000) != 0);
    if (b != 0) {
        func_ov077_02125e94(self, 3);
        return;
    }

    /* 0x403c0 = punch | kick | breakdance | slide kick | fire. */
    if ((self->mdCcAc_c.hitFlags & 0x403c0)
        || player->IsOnShell() != 0
        || player->mIsMetal != 0) {
        if (self->mState != 1)
            return;
        Sound::PlayBank0(0xb5, self->mCamSpacePos);
        self->mChasePlayer = player;
        func_ov077_02125e94(self, 2);
        return;
    }

    if (self->mdCcAc_c.hitFlags & 0x10) {
        self->mPrevAngleY = Vec3_HorzAngle(&player->mPosX, &self->mPosX);
        player->IncMegaKillCount();
        func_ov077_02125e94(self, 5);
        return;
    }

    if (self->mState == 4)
        return;

    {
        int v[3];
        v[0] = self->mPosX;
        v[1] = self->mPosY;
        v[2] = self->mPosZ;
        player->Hurt(*(const Vector3 *)v, 2, 0xc000, 1, 0, 1);
    }
}

// @symbol func_ov077_021250a8
/* Hit response during the thrown spin. Mega turns it to face away and
 * knocks it up; fire is ignored; anything else hurts the player. */
extern "C" void func_ov077_021250a8(void *vc)
{
    daTgz_c *self = (daTgz_c *)vc;
    unsigned int id = self->mdCcAc_c.otherOwner;
    if (id == 0)
        return;
    Player *player = (Player *)dActor_c::FindWithID(id);
    if (player == 0)
        return;
    int b1 = (int)(player->actorID == 0xbf);
    if (b1 == 0)
        return;
    int b2 = (int)((self->mFlags & 0x20000) != 0);
    if (b2 != 0) {
        func_ov077_02125e94(self, 3);
        return;
    }
    int flags = self->mdCcAc_c.hitFlags;
    if ((flags & 0x10) != 0) {
        self->mPrevAngleY = Vec3_HorzAngle(&player->mPosX, &self->mPosX);
        self->mAngleY = self->mPrevAngleY + 0x8000;
        player->IncMegaKillCount();
        func_ov077_02125e94(self, 5);
        return;
    }
    if ((flags & 0x40000) != 0)
        return;
    Vector3 v;
    v.x = self->mPosX;
    v.y = self->mPosY;
    v.z = self->mPosZ;
    player->Hurt(v, 2, 0xc000, 1, 0, 1);
}

// @symbol func_ov077_021251d0
/* Tumble matrix: translate to pos>>3, up by the egg-aim height, rotate, then
 * back down by that height, and keep the result on the anim model's matrix. */
extern "C" void func_ov077_021251d0(daTgz_c *self)
{
    int v[3];
    Vec3_Asr(v, &self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    int aimHeight = self->OnAimedAtWithEgg();
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, aimHeight >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    int aimHeightAgain = self->OnAimedAtWithEgg();
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, (-aimHeightAgain) >> 3, 0);
    self->mAnmModel.mat4x3 = data_020a0e68;
}

// @symbol func_ov077_02125290
/* Upright matrix on the still model in states 0 and 4, otherwise on the anim. */
extern "C" void func_ov077_02125290(daTgz_c *self)
{
    int v = self->mState;
    int b = 1;
    Matrix4x3 *m;
    if (v != 0)
        b = (v == 4);
    if (b)
        m = &self->mModel.mat4x3;
    else
        m = &self->mAnmModel.mat4x3;
    Matrix4x3_FromRotationZXYExt(m, self->mAngleX, self->mAngleY, self->mAngleZ);
    m->m[9] = self->mPosX >> 3;
    m->m[10] = self->mPosY >> 3;
    m->m[11] = self->mPosZ >> 3;
}

// @symbol func_ov077_02125304
extern "C" void func_ov077_02125304(char *vc)
{
    daTgz_c *self = (daTgz_c *)vc;
    int b = (int)((self->mFlags & 0x40000) != 0);
    if (b != 0)
        return;
    if (self->mState == 5)
        func_ov077_021251d0(self);
    else
        func_ov077_02125290(self);
    self->mShadowMatrix[9] = self->mPosX >> 3;
    self->mShadowMatrix[10] = self->mPosY >> 3;
    self->mShadowMatrix[11] = self->mPosZ >> 3;
    self->DropShadowRadHeight(self->mShadowModel, *(Matrix4x3 *)self->mShadowMatrix, 0x50000,
                              0x320000, 0xf);
}

// @symbol _ZN7daTgz_c19func_ov077_021253a4Ev
/* State 5 update: tumble backward until the ground or the 45-frame timer. */
int daTgz_c::func_ov077_021253a4() {
    mAngleX = mAngleX - 0x1000;
    mAnmModel.dExtFrameCtrl_c::Advance();
    UpdatePos(&mdCcAc_c);

    if (mHorzSpeed >= mWithMeshClsn.mRadius || mVertSpeed >= mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);

    if (mWithMeshClsn.JustHitGround() || DecIfAbove0_Byte(&mActionTimer) == 0) {
        Vector3 v;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        SpawnCoins(v, 1, 0x2000, 0);
        PoofDust();
        func_02012694(0xc4, &mCamSpacePos);
        MarkForDestruction();
    }
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125480Ev
/* State 5 entry: launch upward at 40.0 and arm the 45-frame tumble. */
int daTgz_c::func_ov077_02125480() {
    Sound::PlayBank0(9, mCamSpacePos);
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mAngleX = 0;
    mAngleZ = 0;
    mActionTimer = 0x2d;
    mAnmModel.SetAnim((BCA_File *)TGZ_BCA, 0, 0x1000, 0);
    mAnmModel.speed = 0x4000;
    int aimHeight = OnAimedAtWithEgg();
    Particle::System::NewSimple(0x43, mPosX, mPosY + aimHeight, mPosZ);
    int aimHeightAgain = OnAimedAtWithEgg();
    Particle::System::NewSimple(0x44, mPosX, mPosY + aimHeightAgain, mPosZ);
    mState = 5;
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125550Ev
/* State 4 update: the same spin as state 0, after the carrier lets go. */
int daTgz_c::func_ov077_02125550() {
    Vector3 vec;
    int x, y, z;
    int d;

    mAngleX = mAngleX + 0x4e20;

    if (mHorzSpeed >= mWithMeshClsn.mRadius || mVertSpeed >= mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);

    if (mWithMeshClsn.JustHitGround()) {
        mVertSpeed = mVertSpeed * -0x3c / 100;
        if (mVertSpeed > 0x8000) {
            x = mPosX;
            y = mPosY + 0x28000;
            z = mPosZ;
            ((int *)&vec)[0] = x;
            ((int *)&vec)[1] = y;
            ((int *)&vec)[2] = z;
            Particle::System::NewSimple(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, &mCamSpacePos);
        } else {
            mVertSpeed = 0;
            mAngleX = 0;
            mWithMeshClsn.ClearLimMovFlag();
            func_ov077_02125e94(this, 1);
        }
    }

    d = mWaterY ? mPosY - mWaterY : 0;
    if (d < -0xc8000) {
        PoofDust();
        func_02012694(0x166, &mCamSpacePos);
        MarkForDestruction();
    }

    UpdatePos(&mdCcAc_c);
    func_ov077_02124eb0(this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_021256b4Ev
/* State 4 entry: leave the carrier, pop up in front of it, and start spinning. */
int daTgz_c::func_ov077_021256b4() {
    dActor_c *carrier;
    int idx;
    s16 sinv;
    s16 cosv;
    s16 carrierYaw;
    Vector3 v;
    int one;

    mFlags &= ~0x80000;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0;

    carrier = mCarrier;
    carrierYaw = carrier->mAngleY;
    mAngleX = 0;
    mAngleY = carrierYaw;
    mAngleZ = 0;

    mPrevAngleY = mAngleY;

    carrier = mCarrier;
    one = 1;
    int *src = (int *)(int)&carrier->mPosX;
    mPosX = src[0];
    mPosY = src[1];
    mPosZ = src[2];

    idx = ((int)(u16)mAngleY) >> 4;
    sinv = data_02082214[idx * 2];
    mPosX = mPosX + FX_MUL(sinv, 0x3c000);
    mPosY = mPosY + 0x85000;
    idx = ((int)(u16)mAngleY) >> 4;
    cosv = data_02082214[idx * 2 + 1];
    mPosZ = mPosZ + FX_MUL(cosv, 0x3c000);

    carrier = mCarrier;
    {
        int ty = carrier->mPosY;
        int tz = carrier->mPosZ;
        int ty2 = ty + 0x50000;
        int tx = carrier->mPosX;
        v.x = tx;
        v.y = ty2;
        v.z = tz;
    }

    DetectRaycastClsn(v, mPos, one);

    mCarrier = 0;
    mWithMeshClsn.SetLimMovFlag();

    mState = 4;
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125830Ev
/* Held update: stick to the carrier, or on release enter state 4 / the walk. */
int daTgz_c::func_ov077_02125830() {
    if (((mFlags & 0x40000) ? 1 : 0) != 0) {
        dActor_c *carrier = mCarrier;
        /* Address of the triple, then [0]/[4]/[8]. Three field loads fold the
           0x5c into the ldr and drop the add the ROM kept. */
        int *src = (int *)(int)&carrier->mPosX;
        mPosX = src[0];
        mPosY = src[1];
        mPosZ = src[2];
    }
    {
        int flags = mFlags;
        if (((flags & 0x80000) ? 1 : 0) != 0)
            func_ov077_02125e94(this, 4);
        else if (((flags & 0x20000) ? 1 : 0) == 0 && ((flags & 0x40000) ? 1 : 0) == 0) {
            mCarrier = 0;
            func_ov077_02125e94(this, 1);
        }
    }
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_021258dcEv
/* State 3 (held) entry. */
int daTgz_c::func_ov077_021258dc() {
    mHorzSpeed = 0;
    mdCcAc_c.Clear();
    mState = 3;
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125908Ev
/* State 2 update: bounce, then stand up into the walk. */
int daTgz_c::func_ov077_02125908() {
    int v;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() != 0)
        mVertSpeed = mVertSpeed * -50 / 100;
    else if (mWithMeshClsn.IsOnGround() != 0) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        mChasePlayer = 0;
        mPrevAngleY = mAngleY;
        func_ov077_02125e94(this, 1);
    }

    if (mWaterY != 0)
        v = mPosY - mWaterY;
    else
        v = 0;

    if (v < -0xc8000) {
        PoofDust();
        func_02012694(0x166, &mCamSpacePos);
        MarkForDestruction();
    }

    mAnmModel.dExtFrameCtrl_c::Advance();
    UpdatePos(&mdCcAc_c);
    func_ov077_02124eb0(this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125a0cEv
/* State 2 entry: hop toward the player that just hit us. */
int daTgz_c::func_ov077_02125a0c() {
    mHorzSpeed = 0x5000;
    mVertSpeed = 0xd000;
    short ang = (short)Vec3_HorzAngle(&mChasePlayer->mPosX, &mPosX);
    mPrevAngleY = ang;
    mWithMeshClsn.SetLimMovFlag();
    mState = 2;
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125a54Ev
/* State 1 update: walk, turning toward mTurnTarget, until the timer expires. */
int daTgz_c::func_ov077_02125a54() {
    int d;
    ApproachLinear(mAngleY, mTurnTarget, 0x64);
    mPrevAngleY = mAngleY;
    mAnmModel.dExtFrameCtrl_c::Advance();
    func_ov077_02124eb0(this);
    if (mWaterY)
        d = mPosY - mWaterY;
    else
        d = 0;
    if (d < -0xc8000) {
        PoofDust();
        func_02012694(0x166, &mCamSpacePos);
        MarkForDestruction();
    }
    UpdatePos(&mdCcAc_c);
    func_ov077_02124d08(this, &mWithMeshClsn);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    if (DecIfAbove0_Byte(&mActionTimer) == 0)
        func_ov077_02125e94(this, 1);
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125b1cEv
/* State 1 entry. func_02035684 stores its second argument at int offset 7,
 * which is dBgCh_Actr::mHeight. */
void daTgz_c::func_ov077_02125b1c() {
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mAnmModel.SetAnim((BCA_File *)TGZ_BCA, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    func_02035684((int *)&mWithMeshClsn, 0x28000);
    mHorzSpeed = 0x1b33;
    mTurnTarget = RandomIntInternal(&data_0209e650);
    mActionTimer = RandomIntInternal(&data_0209e650);
    mState = 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125bb4Ev
/* State 0 update: spin, splash, and settle into the walk. */
int daTgz_c::func_ov077_02125bb4() {
    int underwater;
    int d;
    int x, y, z;
    Vector3 vec;

    mAngleX = mAngleX + 0x4e20;
    UpdatePos(&mdCcAc_c);
    func_ov077_021250a8(this);
    mWithMeshClsn.SetLimMovFlag();
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    underwater = func_ov077_02124ce4(this);
    if (underwater) {
        if (mInWater == 0) {
            func_02012694(0xe2, &mCamSpacePos);
            Particle::System::NewBigSplash(mPosX, data_0209f32c, mPosZ);
            mRippleId = Particle::System::New(mRippleId, 0x109, mPosX, data_0209f32c, mPosZ, 0, 0);
        }
        mVertAccel = -0x400;
        mTerminalVelocity = -0x5000;
        mHorzSpeed = 0x2000;
    } else {
        mVertAccel = -0x2000;
        mTerminalVelocity = -0x3c000;
        mHorzSpeed = 0x4000;
    }
    mInWater = (unsigned char)underwater;

    if (mWithMeshClsn.JustHitGround()) {
        mVertSpeed = (mVertSpeed * -0x3c) / 100;
        d = mWaterY ? mPosY - mWaterY : 0;
        if (d < -0xc8000) {
            PoofDust();
            func_02012694(0x166, &mCamSpacePos);
            MarkForDestruction();
        } else if (mVertSpeed < 0xa000) {
            mVertSpeed = 0;
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            mWithMeshClsn.ClearLimMovFlag();
            mFlags |= 0x10000000u;
            func_ov077_02125e94(this, 1);
        } else {
            x = mPosX;
            y = mPosY + 0x28000;
            z = mPosZ;
            ((int *)&vec)[0] = x;
            ((int *)&vec)[1] = y;
            ((int *)&vec)[2] = z;
            Particle::System::NewSimple(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, &mCamSpacePos);
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN7daTgz_c19func_ov077_02125dd4Ev
/* State 0 entry. */
int daTgz_c::func_ov077_02125dd4() {
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x4000;
    mVertSpeed = 0x19000;
    mWithMeshClsn.SetLimMovFlag();
    mState = 0;
    return 1;
}

// @symbol func_ov077_02125e20
extern "C" void func_ov077_02125e20(daTgz_c *self)
{
    TgzState *p = (TgzState *)self->mStateDesc + 1;
    (self->* *p)();
}

// @symbol func_ov077_02125e5c
extern "C" void func_ov077_02125e5c(daTgz_c *self)
{
    TgzState *p = (TgzState *)self->mStateDesc;
    (self->* *p)();
}

// @symbol func_ov077_02125e94
extern "C" void func_ov077_02125e94(daTgz_c *self, int i)
{
    self->mStateDesc = &gTgzStates[i];
    func_ov077_02125e5c(self);
}

// @symbol _ZN7daTgz_c16CleanupResourcesEv
int daTgz_c::CleanupResources()
{
    data_ov077_02127b48.Release();
    data_ov077_02127b38.Release();
    gTgzWalkAnimFile.Release();
    return 1;
}

// @symbol _ZN7daTgz_c16OnPendingDestroyEv
void daTgz_c::OnPendingDestroy()
{
}

// @symbol _ZN7daTgz_c6RenderEv
int daTgz_c::Render()
{
    if ((mFlags & 0x40000) ? 1 : 0)
        return 1;
    int s = mState;
    if (s == 0 || s == 4)
        mModel.Render(0);
    else
        mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN7daTgz_c8BehaviorEv
int daTgz_c::Behavior()
{
    int s = mState;
    if (s != 1 || mWithMeshClsn.IsOnGround()) {
        s = mState;
        if (s != 4 && s != 5 && IsTooFarAwayFromPlayer(0x5dc000)) {
            if (DecIfAbove0_Byte(&mDespawnTimer) == 0) {
                MarkForDestruction();
                return 1;
            }
            goto done;
        }
    }
    func_ov077_02124c28(this);
    func_ov077_02125e20(this);
    MakeVanishLuigiWork(mdCcAc_c);
    func_ov077_02125304((char *)this);
    if (data_0209f2f8 == 0x1c && mPosY <= -0x1600000) {
        PoofDust();
        func_02012694(0xc4, &mCamSpacePos);
        MarkForDestruction();
    }
done:
    return 1;
}

// @symbol _ZN7daTgz_c13InitResourcesEv
int daTgz_c::InitResources()
{
    BMD_File *bmd;
    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov077_02127b48);
    mModel.SetFile(bmd, 1, -1);
    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov077_02127b38);
    mAnmModel.SetFile(bmd, 1, -1);
    dExtFrameCtrl_c::LoadFile(gTgzWalkAnimFile);
    mAnmModel.SetAnim((BCA_File *)TGZ_BCA, 0, 0x1000, 0);
    if (!mShadowModel.InitCylinder())
        return 0;
    mdCcAc_c.Init(this, 0x2d000, 0x3c000, 0x200000, 0x4a3d0);
    mWithMeshClsn.Init(this, 0x2d000, 0, &mPrevAngle, &mAngle);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mDespawnTimer = 0x2c;
    func_ov077_02125e94(this, 0);
    *(Matrix4x3 *)mShadowMatrix = *(Matrix4x3 *)&IDENTITY_MATRIX4X3;
    func_ov077_02125304((char *)this);
    return 1;
}

// @symbol _ZN7daTgz_c13OnTurnIntoEggER6Player
/* Vtable slot 19. Yoshi's egg is worth one coin -- paid directly while the
 * player is collecting a cap, otherwise counted as an egg coin. */
void daTgz_c::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 1, 0);
    else
        player.RegisterEggCoinCount(1, 0, 0);
    MarkForDestruction();
}

// @symbol daTgz_c_classInit
extern "C" daTgz_c *daTgz_c_classInit() {
    return new daTgz_c;
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daTgz_c> g_profile_TOGEZO = {
    daTgz_c_classInit, /* classInit */
    0x0104,            /* behaviorPriority */
    0x0076,            /* renderPriority */
    0x10000002,        /* actorFlags */
    0,                 /* clipOffsetY */
    0x3c000,           /* clipRadius */
    0x01000000,        /* clipDistance */
    0x007d0000         /* farDistance */
};
