//cpp
/**
 * Whomp's Fortress knock-down plank (registry profile BK_BOTAOSI).
 *
 * A plank standing on end. The first hit starts it wobbling
 * (mState 1); a second hit from above, or a ground pound onto it once it
 * wobbles, tips it over (mState 2) toward the side it was struck from,
 * and it lands flat with an earthquake and a thud (mState 3). The cuboid
 * shadow is re-aimed every frame by func_ov015_021114f0.
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
 * last. Immediate codegen is also what lets the two per-member settings
 * below -- opt_foldconstants off on func_ov015_021114f0, opt_propagation
 * off on InitResources -- bind to their own member through push/pop. Under
 * the default deferred codegen both are file-global, last one wins, and no
 * single setting matches both members (measured; see the TU manifest).
 * Same arrangement as src/game/actors/d_a_btt_bk.cpp.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange,
 *   dActor_c::Earthquake and dActor_c::DropShadowScaleXYZ stay spelled as
 *   mangled extern-C free functions. Each takes Fix12<int> by value, and a
 *   real method call homes the argument and size-DIFFs the caller
 *   (notes/mwccarm-codegen.md 6az) -- the wall d_a_obj_bk_lift.cpp keeps.
 * Leftover: InitResources calls dBgCh_Gnd's C1/D1 by hand on word storage;
 *   see the comment there.
 * Leftover: OnKicked and OnAttacked2 keep #pragma long_calls: the ROM
 *   veneers are the pooled `ldr ip,[pc]; bx ip` absolute tail call (0xc
 *   each); a near `b` to func_ov015_02111414 in this same TU is 4. The
 *   pragma is positional in 2004/b56.
 * Leftover: func_ov015_02111414 / func_ov015_021114f0 / func_ov015_0211166c
 *   keep their C-ABI cartridge names. They are this TU's own helpers, not
 *   vtable slots.
 * Leftover: data_ov015_02114974 / data_ov015_0211497c are this class's
 *   KCL and BMD handles, defined at the file end so mwcc emits the .init.
 *   symbols.txt used to coin PoleBillboard_ClsnFile / PoleBillboard_ModelFile
 *   on the same two addresses -- a stale spelling of a discarded class name --
 *   so this TU keeps the address-true spelling, the way d_a_obj_bk_lift.cpp
 *   does for its pair.
 * Leftover: data_ov015_02113574 is the CLPS block in overlay .data that
 *   this TU does not own; data_02082214 is arm9's sin/cos table,
 *   data_020a0e68 arm9's scratch matrix, and data_0209f220 / data_0209f2f8
 *   arm9 scene state.
 * Leftover: g_profile_BK_BOTAOSI lives outside this TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjBk_Botaosi_c_AssetId {
    ASSET_BK_BOTAOSI_BMD = 0x0589,
    ASSET_BK_BOTAOSI_KCL = 0x058a
};

#pragma defer_codegen off

#include "daObjBk_Botaosi_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"

/* SharedFilePtr has no fields; the two words are the handle's 8 bytes. The
 * constructors and destructors are the ROM resource-family veneers, aliased
 * in the manifest. */
struct BkBotaosiModelFilePtr : SharedFilePtr {
    u32 words[2];

    BkBotaosiModelFilePtr(u32 fileID);
    ~BkBotaosiModelFilePtr();
};

struct BkBotaosiCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    BkBotaosiCollisionFilePtr(u32 fileID);
    ~BkBotaosiCollisionFilePtr();
};

extern "C" {
extern BkBotaosiCollisionFilePtr data_ov015_02114974;   /* collision KCL */
extern BkBotaosiModelFilePtr data_ov015_0211497c;       /* plank BMD */
extern CLPS_Block    data_ov015_02113574;
extern Matrix4x3     data_020a0e68;         /* arm9 scratch matrix */
extern s16 data_02082214[];                 /* arm9 sin/cos table */
extern s8  data_0209f2f8;
extern u8  data_0209f220;

s16  Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int  AngleDiff(int a, int b);
int  IsStarCollectedInCurLevel(int starID);
void Matrix4x3_FromRotationY(void *m, int ang);
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);

void func_ov015_02111414(daObjBk_Botaosi_c *plank, dActor_c *other);
void func_ov015_021114f0(daObjBk_Botaosi_c *plank);
void func_ov015_0211166c(daObjBk_Botaosi_c *plank);

void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
    dActor_c *self, const Vector3 *pos, Fix12i strength);
int  _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *file, void *mat, int scale, s16 angle, void *clps);
void *_ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);
void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mat, int a, int b, int c, u8 flags);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_cD1Ev
// @symbol _ZN17daObjBk_Botaosi_cD0Ev
/* Empty on purpose. mwccarm destroys the dExtShadowModel_c member, then runs
   ~dBgActor_c inline (its Model and dBgW_KcMbg, then ~dActor_c), and emits
   retail D1 followed by D0. D0's deallocation is an inline operator delete,
   which is why nothing here mentions a heap. */
daObjBk_Botaosi_c::~daObjBk_Botaosi_c()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_c15OnHitByMegaCharER6Player
void daObjBk_Botaosi_c::OnHitByMegaChar(Player &player)
{
    if (mState >= 2) return;
    player.IncMegaKillCount();
    mState = 2;
    mWobbleTimer = 0x640;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_c8OnKickedER8dActor_c
#pragma long_calls on
void daObjBk_Botaosi_c::OnKicked(dActor_c &other)
{
    func_ov015_02111414(this, &other);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_c11OnAttacked2ER8dActor_c
/* The helper is void; the override falls off its end so the call stays a
   tail call, the shape func_ov015_021128e8 has in d_a_obj_bk_lift.cpp. */
#pragma long_calls on
int daObjBk_Botaosi_c::OnAttacked2(dActor_c &other)
{
    func_ov015_02111414(this, &other);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111414
/* The hit handler OnKicked and OnAttacked2 share. The first hit starts the
   wobble; a hit within 0x2000 of mAngleY knocks toward -1, one more than
   0x6000 off it toward +1, and one from the flank in between changes
   nothing else. A hit from well above while the wobble is still strong
   tips the plank over. The second `diff < 0x2000` test is dead, but the
   cartridge compiles it: written as a plain else-if chain the branch
   layout changes and the body DIFFs (measured). */
extern "C" void func_ov015_02111414(daObjBk_Botaosi_c *plank, dActor_c *other)
{
    int diff;
    s16 angle;

    if (plank->mState >= 2)
        return;
    if (plank->mState == 0)
        plank->mState++;

    angle = Vec3_HorzAngle(&plank->mPos, &other->mPos);
    diff = AngleDiff(angle, plank->mAngleY);
    if (diff < 0x2000) {
        plank->mKnockDir = -1;
    } else {
        if (diff < 0x2000)
            goto knockForward;
        if (diff < 0x6000)
            return;
    knockForward:
        plank->mKnockDir = 1;
    }

    if (plank->mState == 1) {
        if (plank->mWobbleTimer >= 0x320) {
            if (other->mPosY > plank->mPosY + 0x64000)
                plank->mState++;
        }
    }
    plank->mWobbleTimer = 0x640;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov015_021114f0
/* Re-aims the cuboid shadow. The plank's tilt (the sine of mAngleX, scaled
   by 0x7d0) sets how far the shadow stretches and toward which end: a
   non-positive tilt turns it a further half turn. It is centred half a lean
   out from mFront*, and once the plank goes over it drops to mFrontFloorY.
   The two scoped row blocks are load-bearing: written flat, with the table
   lookup inlined, the body DIFFs (measured). */
#pragma push
#pragma opt_foldconstants off
extern "C" void func_ov015_021114f0(daObjBk_Botaosi_c *plank)
{
    u16 yaw;
    int lean = 0;
    int idx = (u16)plank->mAngleX >> 4;
    int sine = data_02082214[idx * 2];
    int tilt = FX_MUL(sine, 0x7d0000);
    yaw = 0x8000;
    if (tilt <= 0) { yaw = yaw + 0x8000; tilt = -tilt; }
    lean = lean + tilt;
    Matrix4x3_FromRotationY(&plank->mShadowMat, (s16)(plank->mAngleY + yaw));
    {
        int front = plank->mFrontPosX;
        int cosine = data_02082214[((u16)(s16)(plank->mAngleY + yaw) >> 4) * 2 + 1];
        plank->mShadowMat.t.x = (front + FX_MUL(cosine, (lean >> 1))) >> 3;
    }
    plank->mShadowMat.t.y = plank->mFrontPosY >> 3;
    {
        int front = plank->mFrontPosZ;
        int sine2 = data_02082214[((u16)(s16)(plank->mAngleY + yaw) >> 4) * 2];
        plank->mShadowMat.t.z = (front + FX_MUL(sine2, (lean >> 1))) >> 3;
    }
    if (plank->mState >= 2)
        plank->mShadowMat.t.y = plank->mFrontFloorY >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        plank, &plank->mShadowModel, &plank->mShadowMat, 0xc8000, 0x12c000, lean, 0xf);
}
#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol func_ov015_0211166c
/* mModel's matrix from all three angles, the position in its translation
   row. */
extern "C" void func_ov015_0211166c(daObjBk_Botaosi_c *plank)
{
    Matrix4x3_FromRotationXYZExt(&plank->mModel.mat4x3,
                                 plank->mAngleX, plank->mAngleY, plank->mAngleZ);
    plank->mModel.mat4x3.t.x = plank->mPosX >> 3;
    plank->mModel.mat4x3.t.y = plank->mPosY >> 3;
    plank->mModel.mat4x3.t.z = plank->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_c16CleanupResourcesEv
int daObjBk_Botaosi_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_0211497c.Release();
    data_ov015_02114974.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_c6RenderEv
int daObjBk_Botaosi_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_c8BehaviorEv
int daObjBk_Botaosi_c::Behavior()
{
    switch (mState) {
    case 0:
        break;
    case 1: {
        unsigned short ang = mWobbleAng;
        int idx = ang >> 4;
        int s = data_02082214[idx * 2];
        int amp = mKnockDir;
        int prod = amp * s;
        int d = FX_MUL(prod, mWobbleTimer);
        mAngleX = d;
        if (mWobbleTimer <= 0) {
            mAngleX = 0;
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
        mAngleX += mFallAngVel;
        if (mKnockDir == -1) {
            if (mAngleX < -0x4000) {
                mAngleX = -0x4000;
                mFallAngVel = 0;
                mState++;

                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                    this, &pos, 0x2000000);
                Sound::PlayBank3(0x44, mCamSpacePos);
            }
        } else {
            mJumpSpeed += 0x2000;
            mPosY += mJumpSpeed;
            int limit = mOriginalPosY + 0x46000;
            if (mPosY > limit) {
                mPosY = limit;
            }
            if (mAngleX > 0x4000) {
                mAngleX = 0x4000;
                mFallAngVel = 0;
                mState++;

                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                    this, &pos, 0x2000000);
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

    func_ov015_0211166c(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    func_ov015_021114f0(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjBk_Botaosi_c13InitResourcesEv
#pragma push
#pragma opt_propagation off
int daObjBk_Botaosi_c::InitResources()
{
    /* Word storage, not a typed local: a dBgCh_Gnd local synthesizes its
       constructor at the declaration and its destructor at scope exit, and
       under opt_propagation off that reorders the whole body. The ROM
       constructs the probe after the model and collider are set up and
       destroys it on each return, so the calls are written by hand. */
    u32 rg[sizeof(dBgCh_Gnd) / sizeof(u32)];
    Vector3 a, b, c, d;
    int zero, one;

    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov015_0211497c), 1, -1);
    mShadowModel.InitCuboid();
    func_ov015_0211166c(this);
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        dBgW_Kc::LoadFile(data_ov015_02114974),
        &mClsnMat,
        0x1000,
        mAngleY,
        &data_ov015_02113574);

    mAngleY += 0x8000;
    zero = 0;
    a.z = -0xfa000; a.x = zero; a.y = zero; b.x = zero; b.y = zero; b.z = zero;

    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&a, &data_020a0e68, &b);
    Vec3_Add(&c, &b, &mPos);
    c.y += 0x14000;
    _ZN9dBgCh_GndC1Ev((dBgCh_Gnd *)rg);
    ((dBgCh_Gnd *)rg)->SetObjAndPos(c, 0);
    mFrontFloorY = c.y;
    if (((dBgCh_Gnd *)rg)->DetectClsn() != 0)
        mFrontFloorY = ((dBgCh_Gnd *)rg)->clsnY;

    zero = 0;
    a.z = 0x32000; a.x = zero; a.y = zero; b.x = zero; b.y = zero; b.z = zero;

    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&a, &data_020a0e68, &b);
    Vec3_Add(&d, &b, &mPos);

    one = 1;
    mFrontPosX = d.x;
    mFrontPosY = d.y;
    mFrontPosZ = d.z;
    mOriginalPosY = mPosY;
    mKnockDir = (s8)one;

    if (data_0209f2f8 == 7 && (data_0209f220 == 1 || IsStarCollectedInCurLevel(one) == 0)
        && mPosY >= 0xdac000)
    {
        _ZN9dBgCh_GndD1Ev((dBgCh_Gnd *)rg);
        return 0;
    }
    _ZN9dBgCh_GndD1Ev((dBgCh_Gnd *)rg);
    return 1;
}
#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol daObjBk_Botaosi_c_classInit
/* BK_BOTAOSI's registry factory. 0x39c is this class's size, and the calls
   the cartridge inlines here -- fBase_c::operator new, dBgActor_c's base
   constructor, the vptr store and dExtShadowModel_c's constructor on the member at
   0x320 -- are exactly what the implicit default constructor of
   `struct daObjBk_Botaosi_c : dBgActor_c` with a dExtShadowModel_c member
   generates. Same shape as daObjBk_Lift_c_classInit in this overlay. */
extern "C" daObjBk_Botaosi_c *daObjBk_Botaosi_c_classInit()
{
    return new daObjBk_Botaosi_c();
}

/* The ROM's initializer constructs the model handle with file ID 1417 and
 * the collision handle with file ID 1418, registering a destructor node for
 * each. mwcc emits __sinit_d_a_obj_bk_botaosi.cpp from these definitions. */
BkBotaosiModelFilePtr data_ov015_0211497c(ASSET_BK_BOTAOSI_BMD);
BkBotaosiCollisionFilePtr data_ov015_02114974(ASSET_BK_BOTAOSI_KCL);
