//cpp
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
 * - func_ov077_02125480 and func_ov077_02125b1c: ModelAnim::SetAnim is +12
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
#include "daTgz_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgCh_Gnd.h"
#include "decl_dBgCh_Actr.h"
#include "decl_Particle.h"

bool ApproachLinear(short &value, short target, short step);

#pragma defer_codegen off

/* SharedFilePtr publishes no fields. The loaded BCA is the word at +4. */
#define TGZ_BCA (*(void **)((char *)&data_ov077_02127c14 + 4))

extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr data_ov077_02127c14;

extern "C" {
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
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
void func_02012694(unsigned int id, const Vector3 *pos);
void func_0201267c(unsigned int id, const Vector3 *pos);
void * _ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
int _ZN4cstd4fdivEii(int a, int b);
s32 func_02010844(void *unused, Vector3 *v, s16 angle);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const Vector3 *pos, unsigned int count, int spread, short angle);
short Vec3_HorzAngle(void *a, void *b);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const Vector3 *pos, unsigned int a, int fix, u8 b, u8 c, u8 d);
void Vec3_Asr(void *dst, void *src, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
extern Matrix4x3 data_020a0e68;
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *shadow, void *mtx, int rad, int depth, u8 opacity);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int flags, int speed, u16 startFrame);
void * _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern s16 data_02082214[];
void func_02035684(int *p, int v);
int RandomIntInternal(int *seed);
extern int data_0209e650;
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID, int x, int y, int z, const void *dir, void *callback);
extern int data_0209f32c;
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *actor, int radius, int height, Vector3_16 *a, Vector3_16 *b);
extern char IDENTITY_MATRIX4X3;
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int dist);
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
 * from unk_0a4/unk_0ac and pitches mAngleX/mAngleZ to the normal. */
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
                func_02012694(0xc4, (const Vector3 *)&self->mCamSpacePosX);
                self->MarkForDestruction();
                return;
            }
            {
                dBgPi *fr = (dBgPi *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn);
                fr->surface.CopyNormalTo(normal);
            }
            if (normal.y != 0) {
                self->mVertSpeed = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * self->unk_0a4 + 0x800) >> 12)
                  + (int)(((long long)normal.z * self->unk_0ac + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
            self->mAngleX = func_02010844(self, &normal, self->mAngleY);
            self->mAngleZ = func_02010844(self, &normal, (s16)(self->mAngleY - 0x4000));
        }
    }
    if (clsn->IsOnWall()) {
        dBgPi *wr = (dBgPi *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn);
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
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, (const Vector3 *)v, 1, 0x2000, 0);
        self->PoofDust();
        func_02012694(0xc4, (const Vector3 *)&self->mCamSpacePosX);
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
        Sound::PlayBank0(0xb5, *(const Vector3 *)&self->mCamSpacePosX);
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
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, (const Vector3 *)v, 2, 0xc000, 1, 0, 1);
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
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v, 2, 0xc000, 1, 0, 1);
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
    self->mModelAnim.mat4x3 = data_020a0e68;
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
        m = &self->mModelAnim.mat4x3;
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
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel, self->mShadowMatrix, 0x50000, 0x320000, 0xf);
}

// @symbol func_ov077_021253a4
/* State 5 update: tumble backward until the ground or the 45-frame timer. */
extern "C" int func_ov077_021253a4(daTgz_c *self)
{
    self->mAngleX = self->mAngleX - 0x1000;
    self->mModelAnim.Animation::Advance();
    self->UpdatePos(&self->mdCcAc_c);

    if (self->mHorzSpeed >= self->mWithMeshClsn.mRadius || self->mVertSpeed >= self->mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&self->mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&self->mWithMeshClsn);

    if (self->mWithMeshClsn.JustHitGround() || DecIfAbove0_Byte(&self->mActionTimer) == 0) {
        Vector3 v;
        v.x = self->mPosX;
        v.y = self->mPosY;
        v.z = self->mPosZ;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &v, 1, 0x2000, 0);
        self->PoofDust();
        func_02012694(0xc4, (const Vector3 *)&self->mCamSpacePosX);
        self->MarkForDestruction();
    }
    return 1;
}

// @symbol func_ov077_02125480
/* State 5 entry: launch upward at 40.0 and arm the 45-frame tumble. */
extern "C" int func_ov077_02125480(daTgz_c *self)
{
    Sound::PlayBank0(9, *(Vector3 *)&self->mCamSpacePosX);
    self->mHorzSpeed = 0xa000;
    self->mVertSpeed = 0x28000;
    self->mAngleX = 0;
    self->mAngleZ = 0;
    self->mActionTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, TGZ_BCA, 0, 0x1000, 0);
    self->mModelAnim.speed = 0x4000;
    int aimHeight = self->OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, self->mPosX, self->mPosY + aimHeight, self->mPosZ);
    int aimHeightAgain = self->OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, self->mPosX, self->mPosY + aimHeightAgain, self->mPosZ);
    self->mState = 5;
    return 1;
}

// @symbol func_ov077_02125550
/* State 4 update: the same spin as state 0, after the carrier lets go. */
extern "C" int func_ov077_02125550(daTgz_c *self)
{
    Vector3 vec;
    int x, y, z;
    int d;

    self->mAngleX = self->mAngleX + 0x4e20;

    if (self->mHorzSpeed >= self->mWithMeshClsn.mRadius || self->mVertSpeed >= self->mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&self->mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&self->mWithMeshClsn);

    if (self->mWithMeshClsn.JustHitGround()) {
        self->mVertSpeed = self->mVertSpeed * -0x3c / 100;
        if (self->mVertSpeed > 0x8000) {
            x = self->mPosX;
            y = self->mPosY + 0x28000;
            z = self->mPosZ;
            ((int *)&vec)[0] = x;
            ((int *)&vec)[1] = y;
            ((int *)&vec)[2] = z;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, (const Vector3 *)&self->mCamSpacePosX);
        } else {
            self->mVertSpeed = 0;
            self->mAngleX = 0;
            self->mWithMeshClsn.ClearLimMovFlag();
            func_ov077_02125e94(self, 1);
        }
    }

    d = self->mWaterY ? self->mPosY - self->mWaterY : 0;
    if (d < -0xc8000) {
        self->PoofDust();
        func_02012694(0x166, (const Vector3 *)&self->mCamSpacePosX);
        self->MarkForDestruction();
    }

    self->UpdatePos(&self->mdCcAc_c);
    func_ov077_02124eb0(self);
    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    return 1;
}

// @symbol func_ov077_021256b4
/* State 4 entry: leave the carrier, pop up in front of it, and start spinning. */
extern "C" int func_ov077_021256b4(daTgz_c *self)
{
    dActor_c *carrier;
    int idx;
    s16 sinv;
    s16 cosv;
    s16 carrierYaw;
    Vector3 v;
    int one;

    self->mFlags &= ~0x80000;
    self->mHorzSpeed = 0xa000;
    self->mVertSpeed = 0;

    carrier = self->mCarrier;
    carrierYaw = carrier->mAngleY;
    self->mAngleX = 0;
    self->mAngleY = carrierYaw;
    self->mAngleZ = 0;

    self->mPrevAngleY = self->mAngleY;

    carrier = self->mCarrier;
    one = 1;
    int *src = (int *)(int)&carrier->mPosX;
    self->mPosX = src[0];
    self->mPosY = src[1];
    self->mPosZ = src[2];

    idx = ((int)(u16)self->mAngleY) >> 4;
    sinv = data_02082214[idx * 2];
    self->mPosX = self->mPosX + (int)(((s64)sinv * 0x3c000 + 0x800) >> 12);
    self->mPosY = self->mPosY + 0x85000;
    idx = ((int)(u16)self->mAngleY) >> 4;
    cosv = data_02082214[idx * 2 + 1];
    self->mPosZ = self->mPosZ + (int)(((s64)cosv * 0x3c000 + 0x800) >> 12);

    carrier = self->mCarrier;
    {
        int ty = carrier->mPosY;
        int tz = carrier->mPosZ;
        int ty2 = ty + 0x50000;
        int tx = carrier->mPosX;
        v.x = tx;
        v.y = ty2;
        v.z = tz;
    }

    self->DetectRaycastClsn(v, *(Vector3 *)&self->mPosX, one);

    self->mCarrier = 0;
    self->mWithMeshClsn.SetLimMovFlag();

    self->mState = 4;
    return 1;
}

// @symbol func_ov077_02125830
/* Held update: stick to the carrier, or on release enter state 4 / the walk. */
extern "C" int func_ov077_02125830(daTgz_c *self)
{
    if (((self->mFlags & 0x40000) ? 1 : 0) != 0) {
        dActor_c *carrier = self->mCarrier;
        /* Address of the triple, then [0]/[4]/[8]. Three field loads fold the
           0x5c into the ldr and drop the add the ROM kept. */
        int *src = (int *)(int)&carrier->mPosX;
        self->mPosX = src[0];
        self->mPosY = src[1];
        self->mPosZ = src[2];
    }
    {
        int flags = self->mFlags;
        if (((flags & 0x80000) ? 1 : 0) != 0)
            func_ov077_02125e94(self, 4);
        else if (((flags & 0x20000) ? 1 : 0) == 0 && ((flags & 0x40000) ? 1 : 0) == 0) {
            self->mCarrier = 0;
            func_ov077_02125e94(self, 1);
        }
    }
    return 1;
}

// @symbol func_ov077_021258dc
/* State 3 entry. */
extern "C" int func_ov077_021258dc(daTgz_c *self)
{
    self->mHorzSpeed = 0;
    self->mdCcAc_c.Clear();
    self->mState = 3;
    return 1;
}

// @symbol func_ov077_02125908
/* State 3 update: bounce, then stand up into the walk. */
extern "C" int func_ov077_02125908(daTgz_c *self)
{
    int v;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&self->mWithMeshClsn);
    if (self->mWithMeshClsn.JustHitGround() != 0)
        self->mVertSpeed = self->mVertSpeed * -50 / 100;
    else if (self->mWithMeshClsn.IsOnGround() != 0) {
        self->mVertSpeed = 0;
        self->mWithMeshClsn.ClearLimMovFlag();
        self->mChasePlayer = 0;
        self->mPrevAngleY = self->mAngleY;
        func_ov077_02125e94(self, 1);
    }

    if (self->mWaterY != 0)
        v = self->mPosY - self->mWaterY;
    else
        v = 0;

    if (v < -0xc8000) {
        self->PoofDust();
        func_02012694(0x166, (const Vector3 *)&self->mCamSpacePosX);
        self->MarkForDestruction();
    }

    self->mModelAnim.Animation::Advance();
    self->UpdatePos(&self->mdCcAc_c);
    func_ov077_02124eb0(self);
    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    return 1;
}

// @symbol func_ov077_02125a0c
/* State 2 entry: hop toward the player that just hit us. */
extern "C" int func_ov077_02125a0c(daTgz_c *self)
{
    self->mHorzSpeed = 0x5000;
    self->mVertSpeed = 0xd000;
    short ang = (short)Vec3_HorzAngle(&self->mChasePlayer->mPosX, &self->mPosX);
    self->mPrevAngleY = ang;
    self->mWithMeshClsn.SetLimMovFlag();
    self->mState = 2;
    return 1;
}

// @symbol func_ov077_02125a54
/* State 1 update: walk, turning toward mTurnTarget, until the timer expires. */
extern "C" int func_ov077_02125a54(daTgz_c *self)
{
    int d;
    ApproachLinear(self->mAngleY, self->mTurnTarget, 0x64);
    self->mPrevAngleY = self->mAngleY;
    self->mModelAnim.Animation::Advance();
    func_ov077_02124eb0(self);
    if (self->mWaterY)
        d = self->mPosY - self->mWaterY;
    else
        d = 0;
    if (d < -0xc8000) {
        self->PoofDust();
        func_02012694(0x166, (const Vector3 *)&self->mCamSpacePosX);
        self->MarkForDestruction();
    }
    self->UpdatePos(&self->mdCcAc_c);
    func_ov077_02124d08(self, &self->mWithMeshClsn);
    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    if (DecIfAbove0_Byte(&self->mActionTimer) == 0)
        func_ov077_02125e94(self, 1);
    return 1;
}

// @symbol func_ov077_02125b1c
/* State 1 entry. func_02035684 stores its second argument at int offset 7,
 * which is dBgCh_Actr::mHeight. */
extern "C" void func_ov077_02125b1c(daTgz_c *self)
{
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, TGZ_BCA, 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    func_02035684((int *)&self->mWithMeshClsn, 0x28000);
    self->mHorzSpeed = 0x1b33;
    self->mTurnTarget = RandomIntInternal(&data_0209e650);
    self->mActionTimer = RandomIntInternal(&data_0209e650);
    self->mState = 1;
}

// @symbol func_ov077_02125bb4
/* State 0 update: spin, splash, and settle into the walk. */
extern "C" int func_ov077_02125bb4(daTgz_c *self)
{
    int underwater;
    int d;
    int x, y, z;
    Vector3 vec;

    self->mAngleX = self->mAngleX + 0x4e20;
    self->UpdatePos(&self->mdCcAc_c);
    func_ov077_021250a8(self);
    self->mWithMeshClsn.SetLimMovFlag();
    dBgCh_Actr_UpdateContinuous_Veneer(&self->mWithMeshClsn);

    underwater = func_ov077_02124ce4(self);
    if (underwater) {
        if (self->mInWater == 0) {
            func_02012694(0xe2, (const Vector3 *)&self->mCamSpacePosX);
            _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(
                self->mPosX, data_0209f32c, self->mPosZ);
            self->mRippleId =
                _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    self->mRippleId, 0x109,
                    self->mPosX, data_0209f32c, self->mPosZ,
                    0, 0);
        }
        self->mVertAccel = -0x400;
        self->mTerminalVelocity = -0x5000;
        self->mHorzSpeed = 0x2000;
    } else {
        self->mVertAccel = -0x2000;
        self->mTerminalVelocity = -0x3c000;
        self->mHorzSpeed = 0x4000;
    }
    self->mInWater = (unsigned char)underwater;

    if (self->mWithMeshClsn.JustHitGround()) {
        self->mVertSpeed = (self->mVertSpeed * -0x3c) / 100;
        d = self->mWaterY ? self->mPosY - self->mWaterY : 0;
        if (d < -0xc8000) {
            self->PoofDust();
            func_02012694(0x166, (const Vector3 *)&self->mCamSpacePosX);
            self->MarkForDestruction();
        } else if (self->mVertSpeed < 0xa000) {
            self->mVertSpeed = 0;
            self->mAngleX = self->mPrevAngleX;
            self->mAngleY = self->mPrevAngleY;
            self->mAngleZ = self->mPrevAngleZ;
            self->mWithMeshClsn.ClearLimMovFlag();
            self->mFlags |= 0x10000000u;
            func_ov077_02125e94(self, 1);
        } else {
            x = self->mPosX;
            y = self->mPosY + 0x28000;
            z = self->mPosZ;
            ((int *)&vec)[0] = x;
            ((int *)&vec)[1] = y;
            ((int *)&vec)[2] = z;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, (const Vector3 *)&self->mCamSpacePosX);
        }
    }

    self->mdCcAc_c.Clear();
    self->mdCcAc_c.Update();
    return 1;
}

// @symbol func_ov077_02125dd4
/* State 0 entry. */
extern "C" int func_ov077_02125dd4(daTgz_c *self)
{
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    self->mHorzSpeed = 0x4000;
    self->mVertSpeed = 0x19000;
    self->mWithMeshClsn.SetLimMovFlag();
    self->mState = 0;
    return 1;
}

typedef void (daTgz_c::*TgzState)();

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
    extern char data_ov077_02127c28[];
    self->mStateDesc = data_ov077_02127c28 + (i << 4);
    func_ov077_02125e5c(self);
}

// @symbol _ZN7daTgz_c16CleanupResourcesEv
int daTgz_c::CleanupResources()
{
    data_ov077_02127b48.Release();
    data_ov077_02127b38.Release();
    data_ov077_02127c14.Release();
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
        mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN7daTgz_c8BehaviorEv
int daTgz_c::Behavior()
{
    int s = mState;
    if (s != 1 || mWithMeshClsn.IsOnGround()) {
        s = mState;
        if (s != 4 && s != 5 && _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x5dc000)) {
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
        func_02012694(0xc4, (const Vector3 *)&mCamSpacePosX);
        MarkForDestruction();
    }
done:
    return 1;
}

// @symbol _ZN7daTgz_c13InitResourcesEv
int daTgz_c::InitResources()
{
    BMD_File *bmd;
    bmd = (BMD_File *)Model::LoadFile(data_ov077_02127b48);
    mModel.SetFile(bmd, 1, -1);
    bmd = (BMD_File *)Model::LoadFile(data_ov077_02127b38);
    mModelAnim.SetFile(bmd, 1, -1);
    Animation::LoadFile(data_ov077_02127c14);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, TGZ_BCA, 0, 0x1000, 0);
    if (!mShadowModel.InitCylinder())
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x2d000, 0x3c000, 0x200000, 0x4a3d0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x2d000, 0, (Vector3_16 *)&mPrevAngleX, (Vector3_16 *)&mAngleX);
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
extern "C" int *daTgz_c_classInit(void)
{
    return (int *)new daTgz_c;
}
