//cpp
/* Wing cap feather (FEATHER 345) -- ov002/daFeather_c.
 *
 * The collectible feather that gives the player wings: Behavior hands any
 * touching player to Player::InitWingFeathers and destroys itself. Until
 * then it sways down on mDriftSpeed/mSwayAngle float physics shared with the
 * sibling water bubble (daObjAbuku_c), blinking through its last 45 ground
 * frames before it pops.
 *
 * common.h FIRST: func_ov002_020b2c44's twelve-word matrix copy wants the
 * flat s32 m[12] spelling, and math/Matrix.h's nested {Matrix3x3 r; Vector3
 * t;} scalarizes it (Vector3 is non-POD). Whichever Matrix4x3 is seen first
 * stands; the func shard already won this way.
 *
 * deslop leftovers:
 * - dCcAc_c::Init / dBgCh_Actr::Init / dActor_c::DropShadowRadHeight /
 *   Particle::System::New / Particle::System::NewSimple stay mangled: they
 *   carry Fix12<int> BY VALUE (6az). dBgCh_Actr::Init's header is Fix12i,
 *   which mangles as i; ROM is Fix12<int>.
 * - dBgCh_Actr_UpdateContinuous_Veneer: arm9's own 0x020383fc tail; no method.
 * - Vec3_Asr / Matrix4x3_* / ApproachLinear / ApproachLinear2 /
 *   DecIfAbove0_Byte: no header declares them; TU-local extern "C".
 * - (Vector3 *)&mPosX in func_ov002_020b2c44: dActor_c has no Pos() on this
 *   branch (#2513); the pun stays at the call, not as a leaf accessor.
 * - The *(short*)&mSwayAngle increment: the cartridge sign-extends it
 *   (ldrsh) while the index below zero-extends (ldrh). The plain u16
 *   re-read holds the reload (volatile disproved, FEATHER-2713-01).
 * - Camera+0x17c (data_0209f318): the halfword InitResources and Behavior
 *   add 0x4000 to; Camera.h names nothing there.
 * - data_0209f2d8 game-mode flag, data_02082214 sin/cos table,
 *   data_020a0e68 scratch matrix, data_ov002_0210da58 file handle: ROM labels.
 * - func_ov002_020b2c44 keeps its ROM label (unowned helper, typed this).
 * - g_profile_FEATHER stays carved (unowned) between the stripped _ZTI/_ZTS
 *   and the stripped vtable (all three deadstrip-data, BBH-leaf shape);
 *   the profile is not reconstructed in this TU.
 */

#include "common.h"
#include "daFeather_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region. Everything here is reached from a
 * C++-named member, which cannot declare it in its own body.
 * ------------------------------------------------------------------------ */
extern "C" {

/* -- this TU's own members, forward-declared because mwcc lays .text down in
      reverse source order and every one of these calls is a forward reference. */
void func_ov002_020b2c44(daFeather_c *c);

/* -- other modules -- */
/* dCcAc_c::Init / dBgCh_Actr::Init stay scalar: they carry Fix12<int> BY
   VALUE (6az). dBgCh_Actr::Init's header is Fix12i, which mangles as i;
   ROM is Fix12<int>. */
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *a, Fix12i b, Fix12i c, unsigned int d, unsigned int e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(dBgCh_Actr *self, dActor_c *a, Fix12i b, Fix12i c, Vector3_16 *d, Vector3_16 *e);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 kind, u32 sub, int x, int y, int z, const Vector3_16 *vec, void* cb);
void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
int _Z15ApproachLinear2Rsss(short* ref, short target, short rate);
void _Z14ApproachLinearRiii(int* ref, int target, int rate);
u8 DecIfAbove0_Byte(u8* p);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 kind, int x, int y, int z);
void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
void Matrix4x3_FromTranslation(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 ang);
void Matrix4x3_FromRotationY(void *m, int angle);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *self, ShadowModel *sm, Matrix4x3 *mtx, Fix12i a, Fix12i b, u8 u);

}

extern SharedFilePtr data_ov002_0210da58;
extern unsigned char data_0209f2d8;
extern void *data_0209f318;
extern s16 data_02082214[];
extern Matrix4x3 data_020a0e68;

/* ==========================================================================
 *
 * The registry factory. It is the TOP of the cartridge's contiguous run, so
 * it is written FIRST here: mwccarm lays .text down in reverse source order.
 *
 * C LINKAGE IS LOAD-BEARING -- the ROM symbol is the bare name.
 * `return new daFeather_c()` MATCHES; the synthesized ctor stores
 * `_ZTV11daFeather_c + 2` because this TU defines the vtable. Leaf operator
 * new forwards `_ZN7fBase_cnwEj` until #2570.
 *
 * Reconstructed source-style name: SM64DS proves this class through
 * allocation size, vtable identity, and the FEATHER registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: daFeather_c_Spawn.
 * ======================================================================== */

extern "C" {

// @symbol daFeather_c_classInit
daFeather_c *daFeather_c_classInit(void)
{
    return new daFeather_c();
}

}

/* ==========================================================================
 * Vtable slot 0.
 * ======================================================================== */

// @symbol _ZN11daFeather_c13InitResourcesEv
int daFeather_c::InitResources()
{
    BMD_File *bmd;
    int b;

    bmd = (BMD_File*)Model::LoadFile(data_ov002_0210da58);
    if (!mModel.SetFile(bmd, 1, 1))
        return 0;
    if (!mShadowModel.InitCylinder())
        return 0;

    mVertAccel = -0x199;
    mTerminalVelocity = -0x28000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x6e000, 0x6e000, 0x100002, 0);
    func_ov002_020b2c44(this);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x28000, 0xa000, 0, 0);

    mAngleX = 0x4000;
    mAngleZ = -0x4000;
    b = (data_0209f2d8 == 1);
    if (!b) {
        /* Camera+0x17c, the same halfword Behavior re-reads every frame. */
        mAngleY = *(s16*)((char*)data_0209f318 + 0x17c) + 0x4000;
    }
    mLifeTimer = 0xb4;
    mVertSpeed = 0x3000;
    return 1;
}

/* ==========================================================================
 * Vtable slot 6.
 * ======================================================================== */

// @symbol _ZN11daFeather_c8BehaviorEv
int daFeather_c::Behavior()
{
    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            Player* p = ClosestPlayer();
            if (p->param1 != 0 || p->IsCollectingCap() != 0) {
                SmallPoofDust();
                MarkForDestruction();
                return 1;
            }
        }
    }

    mParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticle, 0x4a, mPosX, mPosY, mPosZ, 0, 0);

    UpdatePos((dCc_c *)0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    if (mWithMeshClsn.IsOnGround() != 0) {
        _Z15ApproachLinear2Rsss((short*)&mSwayAngle, 0, 0x50);
        _Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x555);
        if (DecIfAbove0_Byte(&mLifeTimer) == 0) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
            MarkForDestruction();
        }
    } else {
        /* The short* increment is load-bearing: the cartridge sign-extends
           it (ldrsh) while the index below zero-extends (ldrh), so a plain
           `mSwayAngle += 0x400` on the u16 field misses. The plain re-read
           still holds the cartridge's reload (FEATHER-2713-01). */
        *(short*)&mSwayAngle += 0x400;
        u16 newv = mSwayAngle;
        int idx = ((newv >> 4) << 1) + 1;
        mHorzSpeed = (int)(((s64)mDriftSpeed * data_02082214[idx] + 0x800) >> 12);
    }

    _Z14ApproachLinearRiii(&mDriftSpeed, 0x10000, 0x332);

    {
        int idx2 = (mSwayAngle >> 4) << 1;
        mAngleX = data_02082214[idx2] + 0x4000;
        int idx3 = ((mSwayAngle >> 4) << 1) + 1;
        mAngleZ = data_02082214[idx3] * 2 - 0x6000;
    }

    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            mAngleY = *(s16*)((char*)data_0209f318 + 0x17c) + 0x4000;
        }
    }

    u32 id = mdCcAc_c.otherOwner;
    if (id != 0) {
        dActor_c* a = dActor_c::FindWithID(id);
        if (a != 0) {
            int eq = (a->actorID == 0xbf);
            if (eq) {
                ((Player *)a)->InitWingFeathers(true);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
                MarkForDestruction();
                return 1;
            }
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    func_ov002_020b2c44(this);
    return 1;
}

/* ==========================================================================
 * Vtable slot 9.
 * ======================================================================== */

// @symbol _ZN11daFeather_c6RenderEv
int daFeather_c::Render()
{
    u8 b = mLifeTimer;
    if (b < 0x2d && (b & 1) != 0)
        return 1;
    mModel.Render(0);
    return 1;
}

/* ==========================================================================
 * Vtable slot 3. Releases the one shared file the class holds; it never
 * touches `this`, which is why the legacy C form could declare itself
 * nullary and still reproduce.
 * ======================================================================== */

// @symbol _ZN11daFeather_c16CleanupResourcesEv
int daFeather_c::CleanupResources()
{
    data_ov002_0210da58.Release();
    return 1;
}

/* ==========================================================================
 * Rebuilds mModel.mat4x3 from the position and angles, then drops the
 * shadow: a fixed-size one in game mode 1, a ground-raycast one otherwise.
 * ======================================================================== */

extern "C" {

// @symbol func_ov002_020b2c44
void func_ov002_020b2c44(daFeather_c *c)
{
    Vector3 pos;
    Vector3 v;
    int b;

    /* (Vector3 *)&mPosX: dActor_c has no Pos() on this branch (#2513), so
       the three words are punned at the call. */
    Vec3_Asr(&v, (Vector3 *)&c->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, c->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, c->mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, c->mAngleX);
    c->mModel.mat4x3 = data_020a0e68;

    b = (data_0209f2d8 == 1);
    if (b) {
        Matrix4x3_FromRotationY(&c->mShadowMtx, c->mAngleY);
        c->mShadowMtx.m[9] = c->mPosX >> 3;
        c->mShadowMtx.m[10] = c->mPosY >> 3;
        c->mShadowMtx.m[11] = c->mPosZ >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, &c->mShadowModel, &c->mShadowMtx, 0x96000, 0x320000, 0xf);
        return;
    }

    {
        int r5;
        int r4;

        dBgCh_Gnd rg;
        pos.x = c->mPosX;
        pos.y = c->mPosY;
        pos.z = c->mPosZ;
        pos.y = pos.y + 0x14000;
        rg.SetObjAndPos(pos, c);
        rg.DetectClsn();

        r5 = c->mPosY - rg.clsnY;
        if (r5 <= 0x1000)
            r5 = 0x1000;
        r4 = 0x96000 - (int)(((long long)r5 * 0x180 + 0x800) >> 12);
        if (r4 < 0xa000)
            r4 = 0xa000;

        Matrix4x3_FromRotationY(&c->mShadowMtx, c->mAngleY);
        c->mShadowMtx.m[9] = c->mPosX >> 3;
        c->mShadowMtx.m[10] = c->mPosY >> 3;
        c->mShadowMtx.m[11] = c->mPosZ >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, &c->mShadowModel, &c->mShadowMtx, r4, r5 + 0x28000, 0xf);
    }
}

}

/* ==========================================================================
 * ROM ordinals 1/0 -- _ZN11daFeather_cD0Ev/_ZN11daFeather_cD1Ev.
 *
 * NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daFeather_c.h and declared ahead of the out-of-line virtuals:
 * out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 * plus a homeless D2. D0's trailing deallocation is the inherited inline
 * operator delete, which is why nothing here names a heap.
 * ======================================================================== */
