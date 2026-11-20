//cpp
/* Wing cap feather (FEATHER 345) -- ov002/daFeather_c.
 *
 * The collectible feather that gives the player wings: Behavior hands any
 * touching player to daPly_c::InitWingFeathers and destroys itself. Until
 * then it sways down on mDriftSpeed/mSwayAngle float physics shared with the
 * sibling water bubble (daObjAbuku_c), blinking through its last 45 ground
 * frames before it pops.
 *
 * What stays, and why:
 * - dBgCh_Actr_UpdateContinuous_Veneer: arm9's own 0x020383fc tail; no method.
 * - Vec3_Asr: its definition takes a plain triple and no header declares it,
 *   so it keeps a local extern "C".
 * - mSwayAngle is an s16 angle: the increment sign-extends it (ldrsh) and
 *   FX_SinIdx((u16)mSwayAngle >> 4) re-reads it zero-extended (ldrh), the
 *   SM64 angle idiom.
 * - dCamera_c::mAngleY (gCamera): the halfword InitResources and Behavior
 *   add 0x4000 to.
 * - gGameMode game-mode flag,
 *   gScratchMtx scratch matrix, gFeatherModelFile file handle: ROM labels.
 * - g_profile_FEATHER stays carved (unowned) between the stripped _ZTI/_ZTS
 *   and the stripped vtable (all three deadstrip-data, BBH-leaf shape);
 *   the profile is not reconstructed in this TU.
 */

#include "common.h"
#include "daFeather_c.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Particle__System.h"
#include "ParticleIds.h"

extern SharedFilePtr gFeatherModelFile;
extern unsigned char gGameMode;
extern dCamera_c *gCamera;

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

daFeather_c *daFeather_c_classInit(void) {
    return new daFeather_c();
}

/* ==========================================================================
 * Vtable slot 0.
 * ======================================================================== */

int daFeather_c::InitResources() {
    BMD_File *bmd;
    int b;

    bmd = dExtSimpleModel_c::LoadFile(gFeatherModelFile);
    if (!mModel.SetFile(bmd, 1, 1))
        return 0;
    if (!mShadowModel.InitCylinder())
        return 0;

    mVertAccel = -0x199;
    mTerminalVelocity = -0x28000;
    mdCcAc_c.Init(this, 0x6e000, 0x6e000, 0x100002, 0);
    UpdateModelAndShadow();
    mWithMeshClsn.Init(this, 0x28000, 0xa000, 0, 0);

    mAngle.x = 0x4000;
    mAngle.z = -0x4000;
    b = (gGameMode == 1);
    if (!b) {
        /* dCamera_c::mAngleY, the same halfword Behavior re-reads every frame. */
        mAngle.y = gCamera->mAngleY + 0x4000;
    }
    mLifeTimer = 180;
    mSpeed.y = 0x3000;
    return 1;
}

/* ==========================================================================
 * Vtable slot 6.
 * ======================================================================== */

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsVsMode(void) {
    return gGameMode == 1;
}

int daFeather_c::Behavior() {
    {
        if (!IsVsMode()) {
            daPly_c *p = ClosestPlayer();
            if (p->param1 != 0 || p->IsCollectingCap() != 0) {
                SmallPoofDust();
                MarkForDestruction();
                return 1;
            }
        }
    }

    mParticle = Particle::System::New(mParticle, kPtclFeather, mPos.x, mPos.y, mPos.z, 0, 0);

    UpdatePos(0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    if (mWithMeshClsn.IsOnGround() != 0) {
        ApproachLinear2(mSwayAngle, 0, 0x50);
        ApproachLinear(mHorzSpeed, 0, 0x555);
        if (DecIfAbove0_Byte(&mLifeTimer) == 0) {
            Particle::System::NewSimple(kPtclItemVanish, mPos.x, mPos.y, mPos.z);
            MarkForDestruction();
        }
    } else {
        mSwayAngle += 0x400;
        mHorzSpeed = FX_MUL(mDriftSpeed, FX_CosIdx((u16)mSwayAngle >> 4));
    }

    ApproachLinear(mDriftSpeed, 0x10000, 0x332);

    mAngle.x = FX_SinIdx((u16)mSwayAngle >> 4) + 0x4000;
    mAngle.z = FX_CosIdx((u16)mSwayAngle >> 4) * 2 - 0x6000;

    {
        if (!IsVsMode()) {
            mAngle.y = gCamera->mAngleY + 0x4000;
        }
    }

    u32 id = mdCcAc_c.otherOwner;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            if (IsPlayerActor(a)) {
                ((daPly_c *)a)->InitWingFeathers(true);
                Particle::System::NewSimple(kPtclItemVanish, mPos.x, mPos.y, mPos.z);
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

int daFeather_c::CleanupResources() {
    gFeatherModelFile.Release();
    return 1;
}

/* ==========================================================================
 * Rebuilds mModel.mat4x3 from the position and angles, then drops the
 * shadow: a fixed-size one in game mode 1, a ground-raycast one otherwise.
 * ======================================================================== */

void daFeather_c::UpdateModelAndShadow() {
    Vector3 pos;
    Vector3 v;
    int b;

    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&gScratchMtx, mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationZ(&gScratchMtx, mAngle.z);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, mAngle.x);
    mModel.mat4x3 = gScratchMtx;

    b = (gGameMode == 1);
    if (b) {
        Matrix4x3_FromRotationY(&mShadowMtx, mAngle.y);
        mShadowMtx.t.x = mPos.x >> 3;
        mShadowMtx.t.y = mPos.y >> 3;
        mShadowMtx.t.z = mPos.z >> 3;
        DropShadowRadHeight(mShadowModel, mShadowMtx, 0x96000, 0x320000, 0xf);
        return;
    }

    {
        int r5;
        int r4;

        dBgCh_Gnd rg;
        pos.x = mPos.x;
        pos.y = mPos.y;
        pos.z = mPos.z;
        pos.y = pos.y + 0x14000;
        rg.SetObjAndPos(pos, this);
        rg.DetectClsn();

        r5 = mPos.y - rg.clsnY;
        if (r5 <= 0x1000)
            r5 = 0x1000;
        r4 = 0x96000 - FX_MUL(r5, 0x180);
        if (r4 < 0xa000)
            r4 = 0xa000;

        Matrix4x3_FromRotationY(&mShadowMtx, mAngle.y);
        mShadowMtx.t.x = mPos.x >> 3;
        mShadowMtx.t.y = mPos.y >> 3;
        mShadowMtx.t.z = mPos.z >> 3;
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
