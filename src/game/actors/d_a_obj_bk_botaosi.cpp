//cpp
/**
 * Whomp's Fortress knock-down plank (registry profile BK_BOTAOSI).
 *
 * A plank standing on end. The first hit starts it wobbling
 * (mState 1); a second hit from above, or a ground pound onto it once it
 * wobbles, tips it over (mState 2) toward the side it was struck from,
 * and it lands flat with an earthquake and a thud (mState 3). The cuboid
 * shadow is re-aimed every frame by UpdateShadow.
 *
 * The class's .text, its .init/.ctor pair and the two resource handles'
 * .bss are owned here. ov015 delinks no .data for this class, so the
 * _ZTV / _ZTI / _ZTS group it names is compiler-only output, compared
 * against the cartridge's own copies at ov015 0x02114420 / 0x021143dc /
 * 0x021143e8.
 *
 * SOURCE ORDER IS ROM ORDER. `#pragma defer_codegen off` makes mwccarm emit
 * each function as it is parsed, so this file is written ROM-ascending: the
 * out-of-line destructor first (D1 0x02111314, then D0 0x02111360; the D2
 * it also produces has no cartridge home and is deadstripped), the factory
 * last. Same arrangement as src/game/actors/d_a_btt_bk.cpp.
 *
 * What stays, and why:
 * - OnKicked and OnAttacked2 keep #pragma long_calls: the ROM
 *   veneers are the pooled `ldr ip,[pc]; bx ip` absolute tail call (0xc
 *   each); a near `b` to HandleHit in this same TU is 4. The
 *   pragma is positional in 2004/b56.
 * - data_ov015_02114974 / data_ov015_0211497c are this class's
 *   KCL and BMD handles, defined at the file end so mwcc emits the .init.
 *   symbols.txt used to coin PoleBillboard_ClsnFile / PoleBillboard_ModelFile
 *   on the same two addresses -- a stale spelling of a discarded class name --
 *   so this TU keeps the address-true spelling, the way d_a_obj_bk_lift.cpp
 *   does for its pair.
 * - data_ov015_02113574 is the CLPS block in overlay .data that
 *   this TU does not own; FX_SinCosTable_ is arm9's sin/cos table,
 *   gScratchMtx arm9's scratch matrix, and gCurrentStarId / gCurrentSublevel
 *   arm9 scene state. AngleDiff, IsStarCollectedInCurLevel and Vec3_Add
 *   have no shared header, so they keep a local extern "C" block.
 * - g_profile_BK_BOTAOSI lives outside this TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjBk_Botaosi_c_AssetId { ASSET_BK_BOTAOSI_BMD = 0x0589, ASSET_BK_BOTAOSI_KCL = 0x058a };

#pragma defer_codegen off

#include "daObjBk_Botaosi_c.h"
#include "daPly_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

/* The constructors and destructors are the ROM resource-family veneers,
 * aliased in the manifest. */
struct BkBotaosiModelFilePtr : SharedFilePtr {
    BkBotaosiModelFilePtr(u32 fileID);
    ~BkBotaosiModelFilePtr();
};

struct BkBotaosiCollisionFilePtr : SharedFilePtr {
    BkBotaosiCollisionFilePtr(u32 fileID);
    ~BkBotaosiCollisionFilePtr();
};

extern BkBotaosiCollisionFilePtr data_ov015_02114974; /* collision KCL */
extern BkBotaosiModelFilePtr data_ov015_0211497c;     /* plank BMD */
struct CLPS_Block {
    u32 tag;
    u32 hdr;
    u32 w0;
    u32 w1;
    u32 w2;
    u32 w3;
    u32 tail0;
    u32 tail1;
};
CLPS_Block data_ov015_02113574 = {0x53504c43, 0x00010008, 0x01000fc5, 0x000000ff,
                                  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
extern s8 gCurrentSublevel;
extern u8 gCurrentStarId;

/* local extern: arm9 C helpers with no shared header yet. */
extern "C" {
int AngleDiff(int a, int b);
}
int IsStarCollectedInCurLevel(int starID);

/* -------------------------------------------------------------------------- */
/* Empty on purpose. mwccarm destroys the dExtShadowModel_c member, then runs
   ~dBgActor_c inline (its dExtSimpleModel_c and dBgW_KcMbg, then ~dActor_c), and emits
   retail D1 followed by D0. D0's deallocation is an inline operator delete,
   which is why nothing here mentions a heap. */
daObjBk_Botaosi_c::~daObjBk_Botaosi_c() {}

/* -------------------------------------------------------------------------- */
void daObjBk_Botaosi_c::OnHitByMegaChar(daPly_c &player) {
    if (mState >= 2)
        return;
    player.IncMegaKillCount();
    mState = 2;
    mWobbleTimer = 1600;
}

/* -------------------------------------------------------------------------- */
#pragma long_calls on
void daObjBk_Botaosi_c::OnKicked(dActor_c &other) {
    HandleHit(&other);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* The helper is void; the override falls off its end so the call stays a
   tail call, the shape func_ov015_021128e8 has in d_a_obj_bk_lift.cpp. */
#pragma long_calls on
int daObjBk_Botaosi_c::OnAttacked2(dActor_c &other) {
    HandleHit(&other);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* The hit handler OnKicked and OnAttacked2 share. The first hit starts the
   wobble; a hit within 0x2000 of mAngle.y knocks toward -1, one more than
   0x6000 off it toward +1, and one from the flank in between changes
   nothing else. A hit from well above while the wobble is still strong
   tips the plank over. The second `diff < 0x2000` test is dead, but the
   cartridge compiles it: written as a plain else-if chain the branch
   layout changes and the body DIFFs (measured). */
void daObjBk_Botaosi_c::HandleHit(dActor_c *other) {
    int diff;
    s16 angle;

    if (mState >= 2)
        return;
    if (mState == 0)
        mState++;

    angle = Vec3_HorzAngle(&mPos, &other->mPos);
    diff = AngleDiff(angle, mAngle.y);
    if (diff < 0x2000) {
        mKnockDir = -1;
    } else {
        if (diff < 0x2000) {
        } else if (diff < 0x6000) {
            return;
        }
        mKnockDir = 1;
    }

    if (mState == 1) {
        if (mWobbleTimer >= 800) {
            if (other->mPos.y > mPos.y + 0x64000)
                mState++;
        }
    }
    mWobbleTimer = 1600;
}

/* -------------------------------------------------------------------------- */
/* Re-aims the cuboid shadow. The plank's tilt (the sine of mAngle.x, scaled
   by 0x7d0) sets how far the shadow stretches and toward which end: a
   non-positive tilt turns it a further half turn. It is centred half a lean
   out from mFront*, and once the plank goes over it drops to mFrontFloorY.
   The two scoped row blocks are load-bearing: written flat, with the table
   lookup inlined, the body DIFFs (measured). */
void daObjBk_Botaosi_c::UpdateShadow() {
    u16 yaw;
    int lean = 0;
    int sine = FX_SinIdx((u16)mAngle.x >> 4);
    int tilt = FX_MUL(sine, 0x7d0000);
    yaw = 0x8000;
    if (tilt <= 0) {
        yaw += 0x8000;
        tilt = -tilt;
    }
    lean = lean + tilt;
    Matrix4x3_FromRotationY(&mShadowMat, (s16)(mAngle.y + yaw));
    {
        int front = mFrontPos.x;
        int cosine = FX_CosIdx((u16)(s16)(mAngle.y + yaw) >> 4);
        mShadowMat.t.x = (front + FX_MUL(cosine, (lean >> 1))) >> 3;
    }
    mShadowMat.t.y = mFrontPos.y >> 3;
    {
        int front = mFrontPos.z;
        int sine2 = FX_SinIdx((u16)(s16)(mAngle.y + yaw) >> 4);
        mShadowMat.t.z = (front + FX_MUL(sine2, (lean >> 1))) >> 3;
    }
    if (mState >= 2)
        mShadowMat.t.y = mFrontFloorY >> 3;
    DropShadowScaleXYZ(mShadowModel, mShadowMat, 0xc8000, 0x12c000, lean, 0xf);
}

/* -------------------------------------------------------------------------- */
/* mModel's matrix from all three angles, the position in its translation
   row. */
void daObjBk_Botaosi_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngle.x, mAngle.y, mAngle.z);
    mModel.mat4x3.t.x = mPos.x >> 3;
    mModel.mat4x3.t.y = mPos.y >> 3;
    mModel.mat4x3.t.z = mPos.z >> 3;
}

/* -------------------------------------------------------------------------- */
int daObjBk_Botaosi_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_0211497c.Release();
    data_ov015_02114974.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
int daObjBk_Botaosi_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
int daObjBk_Botaosi_c::Behavior() {
    switch (mState) {
    case 0:
        break;
    case 1: {
        unsigned short ang = mWobbleAng;
        int s = FX_SinIdx(ang >> 4);
        int amp = mKnockDir;
        int prod = amp * s;
        int d = FX_MUL(prod, mWobbleTimer);
        mAngle.x = d;
        if (mWobbleTimer <= 0) {
            mAngle.x = 0;
            mWobbleAng = 0;
            mState = 0;
        } else {
            mWobbleTimer -= 8;
        }
        mWobbleAng += 0x400;
        break;
    }
    case 2: {
        int amt = mKnockDir << 0x17;
        mFallAngVel += amt >> 16;
        mAngle.x += mFallAngVel;
        if (mKnockDir == -1) {
            if (mAngle.x < -0x4000) {
                mAngle.x = -0x4000;
                mFallAngVel = 0;
                mState++;

                Vector3 pos;
                pos.x = mPos.x;
                pos.y = mPos.y;
                pos.z = mPos.z;
                Earthquake(pos, 0x2000000);
                Sound::PlayBank3(0x44, mCamSpacePos);
            }
        } else {
            mJumpSpeed += 0x2000;
            mPos.y += mJumpSpeed;
            int limit = mOriginalPosY + 0x46000;
            if (mPos.y > limit) {
                mPos.y = limit;
            }
            if (mAngle.x > 0x4000) {
                mAngle.x = 0x4000;
                mFallAngVel = 0;
                mState++;

                Vector3 pos;
                pos.x = mPos.x;
                pos.y = mPos.y;
                pos.z = mPos.z;
                Earthquake(pos, 0x2000000);
                Sound::PlayBank3(0x44, mCamSpacePos);
            }
        }
        break;
    }
    case 3:
        break;
    default:
        break;
    }

    UpdateModelMatrix();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    UpdateShadow();
    return 1;
}

/* -------------------------------------------------------------------------- */
int daObjBk_Botaosi_c::InitResources() {
    Vector3 a, b, c, d;
    int one;

    mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov015_0211497c), 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelMatrix();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(data_ov015_02114974), mClsnMat, 0x1000, mAngle.y,
                          data_ov015_02113574);

    mAngle.y += 0x8000;
    a.x = 0;
    a.y = 0;
    a.z = -0xfa000;
    b.x = 0;
    b.y = 0;
    b.z = 0;

    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    MTX_MultVec43(&a, &gScratchMtx, &b);
    Vec3_Add(&c, &b, &mPos);
    c.y += 0x14000;
    /* The floor probe under the swing's far end. The ROM destroys it on
       each return, which is the scope-exit destructor of this local. */
    dBgCh_Gnd gnd;
    gnd.SetObjAndPos(c, 0);
    mFrontFloorY = c.y;
    if (gnd.DetectClsn() != 0)
        mFrontFloorY = gnd.clsnY;

    a.x = 0;
    a.y = 0;
    a.z = 0x32000;
    b.x = 0;
    b.y = 0;
    b.z = 0;

    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    MTX_MultVec43(&a, &gScratchMtx, &b);
    Vec3_Add(&d, &b, &mPos);

    one = 1;
    mFrontPos.x = d.x;
    mFrontPos.y = d.y;
    mFrontPos.z = d.z;
    mOriginalPosY = mPos.y;
    mKnockDir = one;

    if (gCurrentSublevel == 7 && (gCurrentStarId == 1 || IsStarCollectedInCurLevel(one) == 0) &&
        mPos.y >= 0xdac000) {
        return 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* BK_BOTAOSI's registry factory. 0x39c is this class's size, and the calls
   the cartridge inlines here -- fBase_c::operator new, dBgActor_c's base
   constructor, the vptr store and dExtShadowModel_c's constructor on the member at
   0x320 -- are exactly what the implicit default constructor of
   `struct daObjBk_Botaosi_c : dBgActor_c` with a dExtShadowModel_c member
   generates. Same shape as daObjBk_Lift_c_classInit in this overlay. */
daObjBk_Botaosi_c *daObjBk_Botaosi_c_classInit() {
    return new daObjBk_Botaosi_c();
}

/* The ROM's initializer constructs the model handle with file ID 1417 and
 * the collision handle with file ID 1418, registering a destructor node for
 * each. mwcc emits __sinit_d_a_obj_bk_botaosi.cpp from these definitions. */
BkBotaosiModelFilePtr data_ov015_0211497c(ASSET_BK_BOTAOSI_BMD);
BkBotaosiCollisionFilePtr data_ov015_02114974(ASSET_BK_BOTAOSI_KCL);
