//cpp
/* daDsnBase_c -- shared base of the Thwomp (daDsn_c, DOSUN 161, ov091)
 * and the Grindel (daDkk_c, DONKAKU 162, ov025). `dsn` is dossun.
 *
 * This TU owns CleanupResources (slot 3, the key function, so it emits
 * _ZTV11daDsnBase_c), Render (slot 9), Init, and the seven helpers both
 * leaves' Behaviors call: rise, hover, slam, rest, recover, the drop
 * shadow, and the Yoshi-egg check that wakes the mesh collider. No
 * factory: InitResources and Behavior are pure. Grindel's states 5..7
 * and the Thwomp's trigger live on the leaves.
 *
 * The file compiles under `#pragma defer_codegen off`, so mwccarm lays .text
 * in source order, which is ROM order. Do not reorder. The destructor stays
 * inline in daDsnBase_c.h and the two forcing functions near the top pull
 * its D0 and D1 out here, in the cartridge's order.
 *
 * Notes (measured spellings that do not match):
 * - UpdateDropShadow keeps the braced mClipRadius clamp. An if-clamp in
 *   that spot is a 64-word DIFF. (#821 r82: the early `s32 *clipRadius`
 *   pointer is gone; `mClipRadius >>= 3` is byte-identical.)
 * - StateSlam keeps `int isDosun` and `if (isDosun != 0)`.
 *   Comparing actorID in the if size-DIFFs 0xf0 -> 0xe4. Combining
 *   dustPos.y into `mPos.y + 0x3c000` size-DIFFs 0xf0 -> 0xec.
 * - Init keeps the two-step probePos.y (combined form size-DIFFs
 *   0x1a8 -> 0x1a4). Storing beforeClsnCallback directly size-DIFFs
 *   0x1a8 -> 0x1a4; the body calls dBgW::SetBeforeClsnCallback.
 * - CleanupResources reloads mFileTable around each Release. One pointer
 *   across the three calls is a 20-word DIFF.
 * - The cycle states are StateRise (0), StateHover (1), StateSlam (2),
 *   StateRest (3) and StateRecover (4); both leaves' Behaviors dispatch on
 *   mState, a member of this class (include/daDsnBase_c.h).
 */

#pragma defer_codegen off

#include "daDsnBase_c.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "LevelRng.h"
#include "math/CalcLib.h"

/* Actor IDs this TU compares. No header vocabulary exists (cleaned callers
 * pass raw hex), so they live here, cited to the ROM's own debug table. */
enum {
    kYoshiEggActorID = 9, /* YOSHI_EGG: WakeNearYoshiEgg's target */
    kDosunActorID = 0xa1, /* DOSUN (161): the Thwomp leaf; Grindel takes
                               the particle path in StateSlam */
};

/* arm9 C helpers with no shared header.
 * DecIfAbove0_Byte is spelled from its definition at src/DecIfAbove0_Byte.c:
 * unsigned char in, unsigned char out. RandomIntInternal's sites pass the RNG
 * state. */
extern "C" {
extern u8 DecIfAbove0_Byte(u8 *);
extern int RandomIntInternal(int *);
}

/* ROM ordinals 0 and 1 -- ov091 0x02132d04 (D0, 0x68) and 0x02132d6c (D1,
 * 0x54). No source here: both destructor variants come from the ONE
 * inline body in include/daDsnBase_c.h, which the class's descendants need
 * visible to inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. A delete-expression for D0 because
 * dBgActor_c declares Kill, a key function reachable from this class. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daDsnBase_c_EmitDeletingDestructor(daDsnBase_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daDsnBase_c_EmitDestructor(daDsnBase_c *p) {
    p->~daDsnBase_c();
}

/* The egg proximity check both Behaviors run in the collision tail. When the
 * nearest Yoshi egg closes to (mClipRadius << 3) and the mesh collider is
 * still asleep, wakes it and reports 1 so the caller refreshes the collider
 * position. The aimPos block is dead by value -- the distance check reads
 * the actor origin, not it -- but it keeps the OnAimedAtWithEgg
 * call and the three stores the ROM emits. */
int daDsnBase_c::WakeNearYoshiEgg() {
    dActor_c *egg = ClosestWithActorID(kYoshiEggActorID);
    if (egg != 0) {
        Vector3 aimPos;
        aimPos.x = mPos.x;
        aimPos.y = mPos.y;
        aimPos.z = mPos.z;
        aimPos.y = aimPos.y + OnAimedAtWithEgg();
        if (Vec3_Dist(&mPos, &egg->mPos) < (mClipRadius << 3)) {
            if (!mMeshCollider.IsEnabled()) {
                mMeshCollider.Enable(this);
                return 1;
            }
        }
    }
    return 0;
}

/* State 4, the recover. Spends the timer state 3 set, then closes the cycle
 * back to state 0 with a 0x28-frame hover preload. */
void daDsnBase_c::StateRecover() {
    int timeLeft = DecIfAbove0_Byte(&mTimer);
    if (timeLeft == 0) {
        mState = 0;
        mTimer = 40;
    }
}

/* State 3, the rest. Spends the landing timer, then moves to state 4 with
 * a fresh 0x14..0x1d-frame recovery time. */
void daDsnBase_c::StateRest() {
    if (DecIfAbove0_Byte(&mTimer) != 0)
        return;
    mState = 4;
    unsigned int roll = RandomIntInternal(&data_0209e650.value);
    unsigned int rollHigh = roll >> 16;
    mTimer = (rollHigh % 10 + 20);
}

/* State 2, the slam. Integrates the fall at 0x4000 a frame; on reaching the
 * stored ground (0x394) snaps to it, stops, moves to state 3 and lands:
 * the Thwomp (DOSUN) raises its landing dust, Grindel spawns particle 0x2e,
 * and both shake the camera and play 0xc7. */
void daDsnBase_c::StateSlam() {
    Vector3 dustPos;
    Vector3 quakePos;
    mSpeed.y = mSpeed.y - 0x4000;
    mPos.y = mPos.y + mSpeed.y;
    if (mPos.y > mGroundY)
        return;
    mPos.y = mGroundY;
    mSpeed.y = 0;
    mState = 3;
    mTimer = 10;
    int isDosun = (actorID == kDosunActorID);
    if (isDosun != 0) {
        HugeLandingDust(true);
    } else {
        dustPos.x = mPos.x;
        dustPos.y = mPos.y;
        dustPos.z = mPos.z;
        dustPos.y = dustPos.y + 0x3c000;
        Particle::System::NewSimple(kPtclGrindelLanding, dustPos.x, dustPos.y, dustPos.z);
    }
    quakePos.x = mPos.x;
    quakePos.y = mPos.y;
    quakePos.z = mPos.z;
    Earthquake(quakePos, 0x7d0000);
    func_0201267c(0xc7, &mCamSpacePos);
}

/* State 1, the hover. Spends the timer state 0 rolled, then drops to
 * state 2. */
void daDsnBase_c::StateHover() {
    int timeLeft = DecIfAbove0_Byte(&mTimer);
    if (timeLeft == 0)
        mState = 2;
}

/* State 0, the rise. Climbs 0xa000 a frame toward the stored top height
 * (0x390); on arrival snaps to it, moves to state 1 and rolls the hover
 * time (0xa..0x27 frames). */
void daDsnBase_c::StateRise() {
    mPos.y += 0xa000;
    if (mPos.y < mRiseY)
        return;
    mPos.y = mRiseY;
    mState = 1;
    mTimer = (((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 30 + 10);
}

/* The per-frame shadow refresh both Behaviors run after the state step.
 * Scales the drop shadow with the height above groundY, grows mClipRadius
 * with that height, and copies the model matrix into shadowMtx with the
 * translation shifted. mClipRadius is also WakeNearYoshiEgg's trigger
 * radius. The braced clamp is load-bearing; see the file comment. */
void daDsnBase_c::UpdateDropShadow() {
    int shadowDrop = 0x20000;
    int heightAboveGround = mPos.y - mGroundY;
    if (heightAboveGround <= 0x14000) {
        heightAboveGround = 0x14000;
        shadowDrop = 0;
    }
    int radius = FX_MUL(heightAboveGround, 0x60);
    DsnBaseFileTable *files = mFileTable;
    int scaleX = files->shadowExtentX - radius;
    if (scaleX < 0xa000)
        scaleX = 0xa000;
    int scaleZ = files->shadowExtentZ - radius;
    if (scaleZ < 0xa000)
        scaleZ = 0xa000;
    mClipRadius = heightAboveGround + 0x8c000;
    {
        int clamped = mClipRadius;
        if (clamped < 0x200000)
            clamped = 0x200000;
        mClipRadius = clamped;
    }
    mClipRadius >>= 3;
    mShadowMtx = mModel.mat4x3;
    mShadowMtx.m[9] = mPos.x >> 3;
    mShadowMtx.m[10] = (mPos.y - shadowDrop) >> 3;
    mShadowMtx.m[11] = mPos.z >> 3;
    DropShadowScaleXYZ(mShadowModel, mShadowMtx, scaleX, heightAboveGround + 0x28000, scaleZ, 0xf);
}

/* Vtable slot 3, inherited by both leaves -- and the key function, so this
 * TU emits the vtable. Releases the model, the collision mesh and, when the
 * leaf bound one, the texture animation; the CLPS block at [2] is not a
 * file and is not released. */
int daDsnBase_c::CleanupResources() {
    DsnBaseFileTable *files;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    files = mFileTable;
    files->model->Release();
    files = mFileTable;
    files->collision->Release();
    files = mFileTable;
    if (files->texAnim != 0)
        files->texAnim->Release();
    return 1;
}

/* Vtable slot 9, inherited by both leaves. The texture animation runs only
 * when the leaf bound one: Grindel's table carries null at [3], the Thwomp's
 * carries its BTP handle. */
int daDsnBase_c::Render() {
    DsnBaseFileTable *files = mFileTable;
    if (files->texAnim != 0)
        mTextureSequence.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* Init is the run's highest-address definition, so it is written last.
 * Both leaves' InitResources call it with their own file table already
 * stored. `Init` is a coined name: class ownership, the two inbound calls,
 * the body and the layout are proven; the original English is not. */
s32 daDsnBase_c::Init() {
    Vector3 probePos;
    BMD_File *bmd;
    KCL_File *kcl;
    DsnBaseFileTable *files;
    CLPS_Block *clps;
    SharedFilePtr *texAnim;

    files = mFileTable;
    bmd = dExtSimpleModel_c::LoadFile(*files->model);
    mModel.SetFile(bmd, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    files = mFileTable;
    kcl = dBgW_Kc::LoadFile(*files->collision);
    clps = files->clps;
    mMeshCollider.SetFile(kcl, mClsnMat, 0x199, mAngle.y, *clps);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    mMeshCollider.Enable(this);

    texAnim = mFileTable->texAnim;
    if (texAnim != 0) {
        dExtAnmTexPat_c::LoadFile(*texAnim);
        files = mFileTable;
        dExtAnmTexPat_c::Prepare(*(BMD_File *)files->model->filePtr,
                                 *(BTP_File *)files->texAnim->filePtr);
        files = mFileTable;
        texAnim = files->texAnim;
        mTextureSequence.SetFile(*(BTP_File *)texAnim->filePtr, 0x40000000, 0x1000, 0);
    }

    if (!mShadowModel.InitCuboid())
        return 0;

    probePos.x = mPos.x;
    probePos.y = mPos.y;
    probePos.z = mPos.z;
    probePos.y = probePos.y + 0x32000;
    {
        dBgCh_Gnd ground;
        ground.SetObjAndPos(probePos, 0);
        mGroundY = probePos.y;
        if (ground.DetectClsn())
            mGroundY = ground.clsnY;

        mRiseY = mPos.y + 0x190000;
        mPos.y = mGroundY;
        mTimer = 40;
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x3c000;
        mHorzSpeed = 0xc000;
        mLandCount = 0;
    }
    return 1;
}
