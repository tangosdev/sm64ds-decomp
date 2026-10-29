//cpp
/* Wing cap feather (FEATHER 345) -- ov002/daFeather_c.
 *
 * The collectible feather that gives the player wings: Behavior hands any
 * touching player to Player::InitWingFeathers and destroys itself. Until
 * then it sways down on mDriftSpeed/mSwayAngle float physics shared with the
 * sibling water bubble (daObjAbuku_c), blinking through its last 45 ground
 * frames before it pops.
 *
 * common.h FIRST: UpdateModelAndShadow's twelve-word matrix copy wants the
 * flat s32 m[12] spelling, and math/Matrix.h's nested {Matrix3x3 r; Vector3
 * t;} scalarizes it (Vector3 is non-POD). Whichever Matrix4x3 is seen first
 * stands; the func shard already won this way.
 *
 * What stays, and why:
 * - dBgCh_Actr_UpdateContinuous_Veneer: arm9's own 0x020383fc tail; no method.
 * - Vec3_Asr: its definition takes a plain triple and no header declares it,
 *   so it keeps a local extern "C".
 * - The *(short*)&mSwayAngle increment: the cartridge sign-extends it
 *   (ldrsh) while the index below zero-extends (ldrh). The plain u16
 *   re-read holds the reload (volatile disproved, FEATHER-2713-01).
 * - dCamera_c+0x17c (data_0209f318): the halfword InitResources and Behavior
 *   add 0x4000 to; dCamera_c.h names nothing there.
 * - data_0209f2d8 game-mode flag, data_02082214 sin/cos table,
 *   data_020a0e68 scratch matrix, data_ov002_0210da58 file handle: ROM labels.
 * - g_profile_FEATHER stays carved (unowned) between the stripped _ZTI/_ZTS
 *   and the stripped vtable (all three deadstrip-data, BBH-leaf shape);
 *   the profile is not reconstructed in this TU.
 */

#include "common.h"
#include "daFeather_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Particle__System.h"

/* local extern: Vec3_Asr's definition takes a plain triple; no shared header
   declares it yet. */
extern "C" void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);

extern SharedFilePtr data_ov002_0210da58;
extern unsigned char data_0209f2d8;
extern void *data_0209f318;
extern s16 data_02082214[];

/* ==========================================================================
 *
 * The registry factory. It is the TOP of the cartridge's contiguous run, so
 * it is written FIRST here: mwccarm lays .text down in reverse source order.
 *
 * C LINKAGE IS LOAD-BEARING -- the ROM symbol is the bare name.
 * `return new daFeather_c()` MATCHES; the synthesized ctor stores
 * `_ZTV11daFeather_c + 2` because this TU defines the vtable. `new` calls
 * the inherited fBase_c::operator new (`_ZN7fBase_cnwEm`).
 *
 * Reconstructed source-style name: SM64DS proves this class through
 * allocation size, vtable identity, and the FEATHER registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: daFeather_c_Spawn.
 * ======================================================================== */

extern "C" {

// @symbol daFeather_c_classInit
daFeather_c *daFeather_c_classInit(void) {
    return new daFeather_c();
}
}

/* ==========================================================================
 * Vtable slot 0.
 * ======================================================================== */

// @symbol _ZN11daFeather_c13InitResourcesEv
int daFeather_c::InitResources() {
    BMD_File *bmd;
    int b;

    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210da58);
    if (!mModel.SetFile(bmd, 1, 1))
        return 0;
    if (!mShadowModel.InitCylinder())
        return 0;

    mVertAccel = -0x199;
    mTerminalVelocity = -0x28000;
    mdCcAc_c.Init(this, 0x6e000, 0x6e000, 0x100002, 0);
    UpdateModelAndShadow();
    mWithMeshClsn.Init(this, 0x28000, 0xa000, 0, 0);

    mAngleX = 0x4000;
    mAngleZ = -0x4000;
    b = (data_0209f2d8 == 1);
    if (!b) {
        /* dCamera_c+0x17c, the same halfword Behavior re-reads every frame. */
        mAngleY = *(s16 *)((char *)data_0209f318 + 0x17c) + 0x4000;
    }
    mLifeTimer = 0xb4;
    mVertSpeed = 0x3000;
    return 1;
}

/* ==========================================================================
 * Vtable slot 6.
 * ======================================================================== */

// @symbol _ZN11daFeather_c8BehaviorEv
int daFeather_c::Behavior() {
    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            Player *p = ClosestPlayer();
            if (p->param1 != 0 || p->IsCollectingCap() != 0) {
                SmallPoofDust();
                MarkForDestruction();
                return 1;
            }
        }
    }

    mParticle = Particle::System::New(mParticle, 0x4a, mPosX, mPosY, mPosZ, 0, 0);

    UpdatePos((dCc_c *)0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    if (mWithMeshClsn.IsOnGround() != 0) {
        ApproachLinear2(*(short *)&mSwayAngle, 0, 0x50);
        ApproachLinear(mHorzSpeed, 0, 0x555);
        if (DecIfAbove0_Byte(&mLifeTimer) == 0) {
            Particle::System::NewSimple(0xd2, mPosX, mPosY, mPosZ);
            MarkForDestruction();
        }
    } else {
        /* The short* increment is load-bearing: the cartridge sign-extends
           it (ldrsh) while the index below zero-extends (ldrh), so a plain
           `mSwayAngle += 0x400` on the u16 field misses. The plain re-read
           still holds the cartridge's reload (FEATHER-2713-01). */
        *(short *)&mSwayAngle += 0x400;
        u16 newv = mSwayAngle;
        int idx = ((newv >> 4) << 1) + 1;
        mHorzSpeed = FX_MUL(mDriftSpeed, data_02082214[idx]);
    }

    ApproachLinear(mDriftSpeed, 0x10000, 0x332);

    {
        int idx2 = (mSwayAngle >> 4) << 1;
        mAngleX = data_02082214[idx2] + 0x4000;
        int idx3 = ((mSwayAngle >> 4) << 1) + 1;
        mAngleZ = data_02082214[idx3] * 2 - 0x6000;
    }

    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            mAngleY = *(s16 *)((char *)data_0209f318 + 0x17c) + 0x4000;
        }
    }

    u32 id = mdCcAc_c.otherOwner;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            int eq = (a->actorID == 0xbf);
            if (eq) {
                ((Player *)a)->InitWingFeathers(true);
                Particle::System::NewSimple(0xd2, mPosX, mPosY, mPosZ);
                MarkForDestruction();
                return 1;
            }
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    UpdateModelAndShadow();
    return 1;
}

/* ==========================================================================
 * Vtable slot 9.
 * ======================================================================== */

// @symbol _ZN11daFeather_c6RenderEv
int daFeather_c::Render() {
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
int daFeather_c::CleanupResources() {
    data_ov002_0210da58.Release();
    return 1;
}

/* ==========================================================================
 * Rebuilds mModel.mat4x3 from the position and angles, then drops the
 * shadow: a fixed-size one in game mode 1, a ground-raycast one otherwise.
 * ======================================================================== */

// @symbol _ZN11daFeather_c20UpdateModelAndShadowEv
void daFeather_c::UpdateModelAndShadow() {
    Vector3 pos;
    Vector3 v;
    int b;

    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    mModel.mat4x3 = data_020a0e68;

    b = (data_0209f2d8 == 1);
    if (b) {
        Matrix4x3_FromRotationY(&mShadowMtx, mAngleY);
        mShadowMtx.m[9] = mPosX >> 3;
        mShadowMtx.m[10] = mPosY >> 3;
        mShadowMtx.m[11] = mPosZ >> 3;
        DropShadowRadHeight(mShadowModel, mShadowMtx, 0x96000, 0x320000, 0xf);
        return;
    }

    {
        int r5;
        int r4;

        dBgCh_Gnd rg;
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        pos.y = pos.y + 0x14000;
        rg.SetObjAndPos(pos, this);
        rg.DetectClsn();

        r5 = mPosY - rg.clsnY;
        if (r5 <= 0x1000)
            r5 = 0x1000;
        r4 = 0x96000 - FX_MUL(r5, 0x180);
        if (r4 < 0xa000)
            r4 = 0xa000;

        Matrix4x3_FromRotationY(&mShadowMtx, mAngleY);
        mShadowMtx.m[9] = mPosX >> 3;
        mShadowMtx.m[10] = mPosY >> 3;
        mShadowMtx.m[11] = mPosZ >> 3;
        DropShadowRadHeight(mShadowModel, mShadowMtx, r4, r5 + 0x28000, 0xf);
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
