//cpp
/* dCamera_c, the main camera, with its states and script ops.
 * arm9 .text 0x020072c4..0x0200e494.
 *
 * _ZTS9dCamera_c is at 0x02086e6c, _ZTI9dCamera_c at 0x02086ee4,
 * _ZTV9dCamera_c at 0x02086f84. ChangeState's mangling names the
 * nested State. The D1/D0 pair is its own TU at 0x0200705c
 * (src/engine/view/d_camera_dtor.cpp), outside this run.
 *
 * Below 0x0200bec4 sit the follow modes, states and script ops
 * (CutsceneGrandStarFlyMarioOffscreen..Mode8DirectionsCamera). They share this file's .data block
 * (0x02086bc0..0x0208747c: the script-op member-function constants, the
 * mode records and the offset triples) and the controller records at
 * 0x0209f498 with func_0200bec4 and up.
 *
 * leftovers:
 * - The func_0200* handlers whose names are not recovered keep their
 *   addresses. The ones the header declares are dCamera_c members; the rest
 *   stay free functions because they are reached as 8-byte member-function
 *   pointers: the script ops through the ROM .data constants
 *   data_02086bc0..data_02086e38, the state entries and ticks through the
 *   nineteen State records at 0x0209b008. Those records name their symbol,
 *   so making one a method is a config/arm9/symbols.txt + relocs.txt re-key
 *   rather than a body edit; it is left for a separate pass.
 * - The PMF class is the CamPmfHost shadow below: it exists only to give
 *   `int (Host::*)()` a class for those .data words.
 * - Scratch vectors are VecFx32 or s32[3]; collision-query results use
 *   Vector3 and its empty destructor.
 * - The 0x0209b008 state table is nineteen dCamera_c::State records defined
 *   at the bottom of this file; __sinit_d_camera.cpp copies their PMFs.
 * - CleanupResources keeps both null checks. The second is the inlined
 *   deleting destructor.
 */
#include "KuppaScript.h"
#include "dCamera_c.h"
#include "CutsceneSpline.h"
#include "daPly_c.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "Memory.h"
#include "decl_common.h"
#include "decl_Particle.h"
#include "dClipper.h"
#include "cstd.h"
#include "math/Approach.h"
#include "PlayerInput.h"
#include "private/controller_input.h"
#include "PathPtr.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Sound.h"
#include "OAM.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/g3_util.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/fx/fx_cp.h"

/* Defined at the bottom of this file; every use here only takes the address. */
extern dCamera_c::State data_0209b008;
extern dCamera_c::State data_0209b018;
extern dCamera_c::State data_0209b028;
extern dCamera_c::State data_0209b038;
extern dCamera_c::State data_0209b048;
extern dCamera_c::State data_0209b058;
extern dCamera_c::State data_0209b068;
extern dCamera_c::State data_0209b078;
extern dCamera_c::State data_0209b088;
extern dCamera_c::State data_0209b098;
extern dCamera_c::State data_0209b0a8;
extern dCamera_c::State data_0209b0b8;
extern dCamera_c::State data_0209b0c8;
extern dCamera_c::State data_0209b0d8;
extern dCamera_c::State data_0209b0e8;
extern dCamera_c::State data_0209b0f8;
extern dCamera_c::State data_0209b108;
extern dCamera_c::State data_0209b118;
extern dCamera_c::State data_0209b128;
extern Matrix4x3 data_0209b41c;
extern unsigned int gNextActorUpdateMask;

extern void *data_02086efc;
extern void *data_02086f08;
extern s32 data_02086f14[3];
extern Vector3 data_02086f20;
extern Vector3 data_02086f2c;
struct CamMode;
extern struct CamMode data_02086fcc;
extern struct CamMode data_02086ff4;
extern struct CamMode data_02087094;
extern struct CamMode data_0208706c;
extern struct CamMode data_0208715c;
extern int data_02087184;
extern int data_020871fc;
extern int data_02087224;
extern int data_02087274;
extern int data_02087314;
extern struct CamMode data_0208747c;
extern struct CamMode data_020871ac;
extern struct CamMode data_020871d4;
extern struct CamMode data_02087404;
extern u8 data_0208738c;
extern s8 gNextSublevel;
extern s32 data_0209ee90[];
extern u8 gNumViewObjs;
extern u8 data_0209f20c;
extern u8 gLocalPlayerId;
extern u8 gVsExitScreenActive;
extern u8 gGamePaused;
extern s8 gCurrentSublevel;
extern s32 data_0209f32c;
extern daPly_c *gPlayers[];
extern dClipper data_0209f43c;
extern s32 gRunningKuppaScript;
extern Matrix4x3 data_020a0e68;
extern struct OamAttr data_ov002_0210c3b0;
extern void *data_ov002_0210c3e0[2];

extern "C" {
/* Siblings below 0x0200bec4 (defined at the end of the file). */
extern int func_020092c4(void *cam, Vector3 *out, const Vector3 *target);
extern s32 Math_Function_0203b14c(s32 *p, s32 tgt, s32 rate, s32 lim, s32 step);
}

extern int StartWithFarCamera();
extern "C" {
/* Actor-table factory; the registry row names the bare symbol. */
extern dCamera_c *dCamera_c_classInit();
}

extern struct ViewObj *GetViewObj(int idx);

/* local extern: CalcLib.h clashes with this TU's C Vec3_HorzLen (int) and Vec3_MulScalarInPlace (void *) */
extern "C" s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
/* local extern: CalcLib.h clashes with this TU's C Vec3_HorzLen (int) and Vec3_MulScalarInPlace (void *) */
extern "C" int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
/* local extern: CalcLib.h clashes with this TU's C Vec3_HorzLen (int) and Vec3_MulScalarInPlace (void *) */
extern "C" int Vec3_HorzLen(const Vector3 *v);
extern "C" {
extern int VEC_Mag(void *v);
extern void Vec3_Add(void *out, const void *a, const void *b);
extern void Vec3_Sub(void *out, const void *a, const void *b);
extern void VEC_Add(void *a, void *b, void *c);
extern void VEC_Subtract(void *a, void *b, void *c);
extern void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern int *Vec3_LslInPlace(int *v, int sh);
extern void Vec3_RotateYAndTranslate(void *out, const void *in, short angle, const void *src);
extern void MTX_MultVec43(void *dst, Matrix4x3 *src, void *out);
}

/* local extern: CalcLib.h clashes with this TU's C Vec3_HorzLen (int) and Vec3_MulScalarInPlace (void *) */
extern "C" s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern "C" {
/* local extern: including math/MtxStack.h renumbers this TU's function-local
   statics (sScriptOps, sPrevTurnInput), whose numbered names symbols.txt
   records, so its matrix routines stay declared here. */
extern void Matrix4x3_FromRotationZ(void *m, int angle);
extern void MTX_Concat43(const Matrix4x3 *a, const Matrix4x3 *b, Matrix4x3 *out);
extern void MTX_Identity43_(Matrix4x3 *matrix);
}

/* Camera-side records used by the follow handlers. Ground and line queries
   use the real dBgCh_Gnd and dBgCh_Lin classes above. */
typedef struct Entry3 {
    int a, b, c;
} Entry3; /* data_02086f58: max distance, max distance with mFlags 0xc00, fraction */
extern Entry3 data_02086f58[];
/* The 0x28-byte mode record dCamera_c::mMode and mAreaMode point at, entry N
   of the table at 0x02086fcc (SetMode). func_02009e70 calls it CamMode.
   Only the fields this file reads are named. */
typedef struct CamMode {
    /* func_02009e70: below nearDist (ScaleDistByFov-scaled) of horizontal
       travel the camera turns toward the player's facing; within farDist of
       lookAt it swings faster. */
    s32 nearDist; /* 0x00 */
    s32 farDist;  /* 0x04 */
    /* The least and greatest rise of pos over lookAt per unit of distance
       (func_02009e70, adjusted by func_0200c9e0). */
    s32 minRiseSlope; /* 0x08 */
    s32 maxRiseSlope; /* 0x0c */
    /* Scaled by ScaleHeightByFov and added to the target's y to place lookAt;
       func_0200bec4 triples it while mFlags 0x20000 is set. */
    s32 lookAtHeight; /* 0x10 */
    /* Scaled by ScaleDistByFov; the z of the offset that, turned by the
       player's yaw, places lookAt from the target. */
    s32 lookAtDist; /* 0x14 */
    /* func_0200bec4 moves lookAt.y linearly toward its goal while the rise
       is below this (ScaleHeightByFov-scaled), and lets it lag otherwise. */
    s32 followRise; /* 0x18 */
    /* The orbit distance func_02009e70 keeps pos beyond. */
    s32 minDist; /* 0x1c */
    /* The z of the offset that places pos from lookAt (func_0200d1e4,
       func_0200cce4). */
    s32 posDist; /* 0x20 */
    /* UpdateState eases mFovAngle toward it. */
    s16 fovAngle; /* 0x24 */
    /* Mode flags. 0x4: func_0200c394 returns early and func_0200c66c keeps
       the record it chose; 0x10: func_0200c66c falls back to mAreaMode and
       mViewObj; 0x20: func_0200c394 skips its framing; 0x40: func_0200bec4
       uses the fixed rate 0x10000. func_0200d0ac refuses the far start while
       mAreaMode's bit 0 is set. */
    u16 flags; /* 0x26 */
} CamMode;
/* GetViewObj record. type 2 gives the camera a yaw (angleY); p1 is a
   per-type parameter (a path id, a row of data_02086f58, or zero to jump
   straight to the view). */
typedef struct CamViewObj {
    u8 type;     /* 0x0 */
    u8 p1;       /* 0x1 */
    s16 x, y, z; /* 0x2 */
    u8 pad_8[2];
    s16 angleY; /* 0xa */
} CamViewObj;
typedef struct CamEntryE {
    s16 f0, f2, f4, f6, f8, fa;
} CamEntryE; /* func_0200cf40's spawn view */
/* The state objects carry member-function pointers back into the camera at
   +0 and +8; EnterState/UpdateState dispatch them through this shadow,
   which exists only to give the pointer-to-member a class. */
struct CamPmfHost;
typedef int (CamPmfHost::*CamPmf)();
struct CamState {
    CamPmf enter;
    CamPmf exec;
};
struct CamPmfHost {
    char pad138[0x138];
    CamPmf *pp;                     /* camera +0x138 == mState */
    char pad13c[0x154 - 0x13c];
    unsigned int flags;             /* camera +0x154 == mFlags */
};

/* A camera state: two CamPmf (enter at +0, execute at +8), plain-copied
   from the .data member-function constants (data_0209b008 and on, below). */
struct dCamera_c::State {
    CamPmf enter;
    CamPmf exec;
};

enum Bool { FALSE, TRUE };

/* Allocation factory. new dCamera_c is the fBase constructor, the dBase and
 * dView vtables, the view-matrix clear, and the dCamera vtable. */
dCamera_c *dCamera_c_classInit() {
    return new dCamera_c;
}

int dCamera_c::InitResources()
{
    u32 i;
    for (i = 0; i < gNumViewObjs; i = (u8)(i + 1)) {
        CamViewObj *v = (CamViewObj *)GetViewObj(i);
        if (v->type == 7) {
            s32 *p;
            s16 x, y, z;
            this->mFixedViewPos = ::operator new(0xc);
            x = v->x; z = v->z; y = v->y;
            p = (s32 *)this->mFixedViewPos;
            p[0] = x << 0xc;
            p[1] = y << 0xc;
            p[2] = z << 0xc;
        }
    }
    this->mFovBlend = 0;
    this->mTargetPlayer = gPlayers[gLocalPlayerId];
    ChangeArea(this->mTargetPlayer->mAreaId);
    this->mFramedActor = 0;
    this->mFocusActor = 0;
    ChangeState(&data_0209b008);
    func_0200cf40();
    if (StartWithFarCamera() != 0)
        func_0200d0ac(gLocalPlayerId);
    return this->Render();
}

static inline int IsKuppaScriptRunning() {
    return gRunningKuppaScript != 0;
}

/* Actor 0xbf is PLAYER. */
static inline int IsPlayer(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsSnowWeatherLevel() {
    return gCurrentSublevel == 0xa || gCurrentSublevel == 0x13;
}

static inline int IsRainWeatherLevel() {
    return gCurrentSublevel == 0x8 || gCurrentSublevel == 0x12 || gCurrentSublevel == 0x15 ||
           gCurrentSublevel == 0x1e;
}

/* Per-frame update: remember last frame's eye and target, run the camera state
 * and decay the shakes (unless a Kuppa script is driving the camera), keep the
 * level's weather particles around the eye, and track whether the eye has gone
 * below the water line. */
int dCamera_c::Behavior()
{
    s32 weatherPos[3];
    s32 flags;
    s32 weather;

    mPrevLookAt.x = lookAt.x;
    mPrevLookAt.y = lookAt.y;
    mPrevLookAt.z = lookAt.z;
    mPrevPos.x = pos.x;
    mPrevPos.y = pos.y;
    mPrevPos.z = pos.z;
    mFlags &= ~0x1000u;
    if ((u32)mAreaMode != (u32)&data_0208733c) {
        mFlags &= ~0x60u;
    }
    flags = mFlags;
    if ((flags & 8) || IsKuppaScriptRunning()) {
        if (flags & 0xc000) {
            mFlags &= ~0xc000u;
            ChangeState(&data_0209b008);
        }
        mAngleY = Vec3_HorzAngle(&lookAt, &pos);
        mAngleX = Vec3_VertAngle(&lookAt, &pos);
        UpdateTargetPos();
    }

    if ((u8)(gVsExitScreenActive | (gGamePaused | data_0209f20c)) == 0 &&
        !(gNextActorUpdateMask & ACTOR_UPDATE_MASK_STAGE_ENDING)) {
        if (!IsKuppaScriptRunning()) {
            if (mFlags & 0x2000)
                Math_Function_0203b0fc(&mFovBlend, 0x1000, 0x88, 0x7fffffff);
            else
                Math_Function_0203b0fc(&mFovBlend, 0, 0x88, 0x7fffffff);
            UpdateState();
            if (mShakeY != 0) {
                ApproachLinear(mShakeY, 0, 0x1000);
                mShakeY = -mShakeY;
            }
            if (mRollShake != 0) {
                if (ApproachLinear2(mRollShake, 0, 0x10) != 0) {
                    mRollShakePhase = 0;
                } else {
                    mRollShakePhase += 0x3000;
                }
            }
        }
        if (mFovShake != 0) {
            mFovShakePhase += mFovShakePhaseStep;
            ApproachLinear2(mFovShake, 0, mFovShakeDecay);
        }
        if (IsSnowWeatherLevel())
            weather = 0x112;
        else if (IsRainWeatherLevel())
            weather = 0xbc;
        else if (gCurrentSublevel == 0x31)
            weather = 0x86;
        else
            weather = -1;
        if (weather >= 0) {
            MTX_MultVec43(&CAM_SPACE_CAM_POS_ASR_3, &data_0209b41c, weatherPos);
            Vec3_LslInPlace(weatherPos, 3);
            int flag;
            if (weather != 0xbc)
                flag = 1;
            else
                flag = IsUnderwater();
            mWeather = Particle::System::NewWeather(mWeather, weather, weatherPos[0], weatherPos[1],
                                                    weatherPos[2], 0, flag);
            if (weather == 0x86) {
                mWeather2 = Particle::System::NewWeather(mWeather2, 0x87, weatherPos[0],
                                                         weatherPos[1], weatherPos[2], 0, 1);
            }
        }
    }

    {
        if (IsPlayer(mTargetPlayer)) {
            daPly_c *player = (daPly_c *)mTargetPlayer;
            s32 waterLine = data_0209f32c - FX_SinIdx((u16)mAngleX >> 4) * 0x10;
            if (!(mFlags & 1)) {
                if (player->mIsUnderwater != 0 || player->mIsInShallowWater != 0) {
                    if (pos.y <= waterLine) {
                        mFlags |= 1u;
                    }
                }
            } else if (pos.y > waterLine) {
                mFlags &= ~1u;
            }
        }
    }
    mFlags &= 0xfff8fdfbu;
    if (mFocusActor != 0) {
        mFocusActor = 0;
        mFlags |= 4u;
    }
    mFramedActor = 0;
    func_0203dafc((short)(mAngleY + mInputYawLag));
    ApproachLinear2(mInputYawLag, 0, 0x400);
    return 1;
}

static inline void SetVec(Vector3 &v, s32 x, s32 y, s32 z) {
    v.x = x;
    v.y = y;
    v.z = z;
}

/* fx32 world position to the 1/8 scale the view matrix is built at, rounded.
 * The arguments are evaluated right to left, so z is computed first. */
static inline void ToCameraUnits(Vector3 &v) {
    SetVec(v, (v.x + 4) >> 3, (v.y + 4) >> 3, (v.z + 4) >> 3);
}

/* Builds this frame's view: last frame's eye and target when the camera barely
 * moved (no sub-unit jitter), the FOV blend and fixed-view override, the
 * vertical, roll and FOV shakes, then the projection and the look-at matrix. */
int dCamera_c::Render()
{
    Vector3 target;
    Vector3 eye;
    s16 fov;
    s16 roll;

    target = lookAt;
    eye = pos;
    fov = mFovAngle;
    roll = mAngleZ;

    if (Vec3_Dist(&target, &mPrevLookAt) < 0x1000 && Vec3_Dist(&eye, &mPrevPos) < 0x1000) {
        target = mPrevLookAt;
        eye = mPrevPos;
    }

    if (!IsKuppaScriptRunning()) {
        s32 flags = mFlags;
        if (!(flags & 8)) {
            if ((u8 *)mMode == &data_0208738c) {
                OAM::Render(0, &data_ov002_0210c3b0, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                G2_SetBlendAlpha(0x10, 0x2f, 0xc, 6);
            } else {
                if (flags & 0x20) {
                    OAM::Render(0, (OamAttr *)data_ov002_0210c3e0[0], 0x14, 0x60, -1, -1, 0x1000,
                                0x1000, 0, -1);
                }
                if (mFlags & 0x40) {
                    OAM::Render(0, (OamAttr *)data_ov002_0210c3e0[1], 0xec, 0x60, -1, -1, 0x1000,
                                0x1000, 0, -1);
                }
            }
            s32 blend = mFovBlend;
            if (blend != 0x1000) {
                fov += (s16)FX_MUL(0xaaa - fov, blend);
            }
            if (data_0209f20c == 0 && gVsExitScreenActive == 0 &&
                (u8)(gVsExitScreenActive | (gGamePaused | data_0209f20c)) != 0) {
                func_0200af20(&target, &eye, &fov);
            }
        }
    }

    target.y += mShakeY;
    eye.y += mShakeY;

    s16 rollShake = mRollShake;
    if (rollShake != 0) {
        s16 sin = FX_SinIdx((u16)mRollShakePhase >> 4);
        roll += (s16)FX_MUL(rollShake, sin);
    }

    s16 fovShake = mFovShake;
    if (fovShake != 0) {
        s16 cos = FX_CosIdx((u16)mFovShakePhase >> 4);
        s32 scaled = FX_MUL(cos, 0x360);
        fov += (fovShake * scaled) << 4 >> 16;
    }

    SetFovAngle(fov);

    G3_ViewPort(mViewport[0], mViewport[1], mViewport[2], mViewport[3]);

    s32 scaleW;
    if (gCurrentSublevel == 0x28) {
        scaleW = 0x10;
    } else {
        scaleW = data_0209ee90[0x44 / 4];
    }

    G3i_PerspectiveW_(FX_SinIdx((u16)fov >> 4), FX_CosIdx((u16)fov >> 4), mAspect, mNear, mFar,
                      scaleW, 1, 0);

    ToCameraUnits(target);
    ToCameraUnits(eye);

    void *up = &data_02086efc;
    if (Vec3_HorzDist(&target, &eye) == 0) {
        up = &data_02086f08;
    }
    G3i_LookAt_(&eye, (Vector3 *)up, &target, 1, &viewMat);

    if (roll != 0) {
        Matrix4x3_FromRotationZ(&data_020a0e68, roll);
        MTX_Concat43(&viewMat, &data_020a0e68, &viewMat);
    }

    dView_c::Render();
    return 1;
}

/* No-op override; the camera does nothing extra when flagged for destruction. */
void dCamera_c::OnPendingDestroy()
{
    this;
}

/* Frees the owned fixed-view-position record at 0x148 and reports success.
 * The doubled null check is the ROM's own guard followed by the inlined
 * deleting-destructor's, giving two consecutive `cmp r0,#0; beq`. */
int dCamera_c::CleanupResources()
{
    void *owned = mFixedViewPos;
    if (owned) {
        if (owned)
            ::operator delete(owned);
    }
    return 1;
}

void dCamera_c::SetFovAngle(short arg) {
    int idx = (u16)arg >> 4;
    mInvSinFov = FX_Inv(FX_SinIdx(idx));
    mInvCosFov = FX_Inv(FX_CosIdx(idx));
    data_0209f43c.Func_020156DC(mAspect, arg, mNear, mFar);
}

/* Shake from a point (landings, bonks, ground pounds, boss steps): if the
 * point is within strength of pos, raise mShakeY to strength/distance scaled
 * by 0x9000, clamped to 0xc000.
 * Super Mario 64's set_pitch_shake_from_point is the likely counterpart: a
 * shake from a point that only reaches the camera within a maximum distance,
 * and only ever raises the current shake. The bodies differ, so the address
 * name stays. Here strength is that distance and the amount grows as the
 * point comes closer; there is no magnitude, decay or phase step, and the
 * shake is mShakeY's vertical offset rather than a pitch magnitude. */
void dCamera_c::func_0200d8c8(const Vector3 *point, int strength) {
    fx32 dist;
    fx32 offset;

    dist = Vec3_Dist(&pos, point);
    if (strength <= dist)
        return;

    offset = FX_MUL(FX_Div(strength, dist), 0x9000);

    if (offset <= mShakeY)
        return;

    if (offset > 0xc000)
        offset = 0xc000;

    mShakeY = offset;
}

/* Super Mario 64's set_fov_shake: a stronger shake replaces the current one
 * together with its decay and phase step; a weaker one is ignored. */
void dCamera_c::SetFovShake(int amplitude, int decay, int shakeSpeed) {
    short current;

    current = mFovShake;
    if (amplitude > current) {
        mFovShake = amplitude;
        mFovShakeDecay = decay;
        mFovShakePhaseStep = shakeSpeed;
    }
}

/* Starts the roll shake (mRollShake 0x180). Super Mario 64's
 * set_camera_roll_shake is the candidate, but that takes a magnitude, decay
 * and phase step and only raises the shake; this one sets a fixed magnitude
 * unconditionally, so the address name stays. */
void dCamera_c::func_0200d89c() {
    mRollShake = 384;
}

/* Returns the under-water bit of mFlags MASKED, not normalised to 0/1 -- the
 * ROM's `and r0,r0,#1` is the whole body. */
int dCamera_c::IsUnderwater() const
{
    return mFlags & 1;
}

/* Moves lookAt and pos by the same offset. Its one caller is the endless
 * stairs warp, where Super Mario 64 calls warp_camera; that also moves
 * Lakitu's goal and transition points and Mario's water level, so the
 * address name stays. */
void dCamera_c::func_0200d858(const Vector3 *offset) {
    VEC_Add(&lookAt, (void *)offset, &lookAt);
    VEC_Add(&pos, (void *)offset, &pos);
}

void dCamera_c::func_0200d81c(int playerID) {
    if (playerID != gLocalPlayerId)
        return;
    ChangeState(&data_0209b008);
}

void dCamera_c::SetCameraZoomState(int playerID) {
    if (playerID != gLocalPlayerId)
        return;
    ChangeState(&data_0209b098);
}

void dCamera_c::func_0200d7a4(u8 playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b108);
}

void dCamera_c::func_0200d768(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b018);
}

void dCamera_c::func_0200d72c(unsigned char playerID) {
    if (playerID == *&gLocalPlayerId)
        ChangeState(&data_0209b028);
}

void dCamera_c::func_0200d6f0(unsigned char playerID) {
    if (playerID == *&gLocalPlayerId) {
        ChangeState(&data_0209b038);
    }
}

void dCamera_c::func_0200d6b4(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b048);
}

void dCamera_c::SetSpinState(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b058);
}

void dCamera_c::func_0200d63c(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b068);
}

void dCamera_c::func_0200d5fc(int playerID) {
    if (playerID == gLocalPlayerId) {
        ChangeState(&data_0209b078);
        SetMode(5);
    }
}

void dCamera_c::func_0200d5c0(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b078);
}

void dCamera_c::SetHeadstandState(int playerID) {
    if (playerID != gLocalPlayerId)
        return;
    ChangeState(&data_0209b078);
    SetMode(8);
}

void dCamera_c::func_0200d544(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b088);
}

void dCamera_c::func_0200d508(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b0a8);
}

void dCamera_c::StartDanceCutscene(unsigned int playerID, int arg2) {
    unsigned char localID = gLocalPlayerId;
    if (playerID == localID) {
        volatile int dummy;
        struct dActor_c *player;
        mOrbitTargetYaw = mAngleY;
        player = mTargetPlayer;
        player->mFlags |= 0x20000000;
        KuppaScript::StartDanceCutscene(arg2);
    }
}

void dCamera_c::func_0200d474(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b0b8);
}

void dCamera_c::func_0200d438(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b0c8);
}

void dCamera_c::func_0200d3f8(unsigned char playerID, void *ptr) {
    unsigned char localID = *&gLocalPlayerId;
    if (playerID != localID)
        return;
    mLookTarget = (s32 *)ptr;
    ChangeState(&data_0209b0d8);
}

void dCamera_c::GoBehindPlayer(unsigned int j)
{
    int slot4, slot8, slotc;

    if (j != gLocalPlayerId)
        return;
    if (gNextSublevel >= 0)
        return;

    mFlags &= 0xfffffaf7u;
    SetMode(0xa);
    mFlags |= 4u;

    slot4 = (int)mMode;
    slotc = 0;
    func_0200c66c(&mTargetPlayer->mPos, &slot8, &slot4, &slotc);
    if (slot4 == (int)&data_020873dc)
        return;
    if (slot4 == (int)&data_0208742c)
        return;
    ChangeState(&data_0209b0e8);
}

void dCamera_c::func_0200d1e4() {
    struct dActor_c *player = mTargetPlayer;
    Vector3 *playerPos = &player->mPos;
    int lookAtDistMode;
    unsigned int lookAtDist;
    s32 lookAtOffset[3];
    short angle;
    int lookAtHeightMode;
    unsigned int lookAtHeight;
    int posDistMode;
    unsigned int posDist;
    s32 posOffset[3];

    mTargetPos.x = playerPos->x;
    mTargetPos.y = playerPos->y;
    mTargetPos.z = playerPos->z;

    lookAtDistMode = mMode->lookAtDist;
    lookAtDist = ScaleDistByFov(lookAtDistMode);

    lookAtOffset[0] = data_02086f2c.x;
    lookAtOffset[1] = data_02086f2c.y;
    lookAtOffset[2] = lookAtDist;
    data_02086f2c.z = lookAtDist;

    angle = mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&lookAt, &mTargetPos, angle, lookAtOffset);

    lookAtHeightMode = mMode->lookAtHeight;
    lookAtHeight = ScaleHeightByFov(lookAtHeightMode);
    lookAt.y += lookAtHeight;

    posDistMode = mMode->posDist;
    posDist = ScaleDistByFov(posDistMode);

    data_02086f14[2] = posDist;
    posOffset[0] = data_02086f14[0];
    posOffset[1] = data_02086f14[1];
    posOffset[2] = posDist;

    angle = mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&pos, &lookAt, angle, posOffset);

    pos.y += (lookAtHeight - 0x8b000);

    mLookAtOffset.x = 0;
    mLookAtOffset.y = 0xc3f9d;
    mLookAtOffset.z = 0;

    ChangeArea(mTargetPlayer->mAreaId);
}

void dCamera_c::LookAtExit(dActor_c &actor)
{
    int *src;

    mLookTarget = &actor.mPosX;
    src = mLookTarget;
    mLookTargetPos.x = src[0];
    mLookTargetPos.y = src[1];
    mLookTargetPos.z = src[2];
    mTargetAngleY = -actor.mAngleY;
    ChangeState(&data_0209b0f8);
    mFlags |= 0x10;
}

void dCamera_c::func_0200d148(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b118);
}

void dCamera_c::SetGrabBowserTailState(unsigned char playerID) {
    if (playerID != *&gLocalPlayerId)
        return;
    ChangeState(&data_0209b128);
}

int dCamera_c::func_0200d0ac(unsigned int playerID) {
    if (playerID == gLocalPlayerId) {
        unsigned short areaFlags = mAreaMode->flags;
        if (areaFlags & 1) {
            return 0;
        }
        mFlags |= 2u;
        func_02012790(7);
    }
    return 1;
}

void dCamera_c::func_0200d064(unsigned char playerID) {
    unsigned char localID = gLocalPlayerId;
    if (playerID == localID) {
        volatile int dummy;
        unsigned int *addr = &mFlags;
        unsigned int val = *addr;
        val &= ~2u;
        *addr = val;
        func_02012790(6);
    }
}

void dCamera_c::SetFlag_3()
{
    u32 *p = &mFlags;
    u32 v = *p | 8;
    *p = v;
    ((void (*)(u32))FUN_02029a68)(v);
}

void dCamera_c::func_0200cf40() {
    mViewport[0] = 0;
    mViewport[1] = 0;
    mViewport[2] = 0xff;
    mViewport[3] = 0xbf;
    mAspect = 0x1555;
    mNear = 0x1000;
    mFar = 0x1388000;
    mAreaMode = &data_0208715c;
    mViewObj = 0;
    if (param1 != 0xf) {
        CamEntryE *v;
        SetMode(0xa);
        mFovAngle = mMode->fovAngle;
        SetFovAngle(mFovAngle);
        func_0200cce4();
        v = (CamEntryE *)GetViewObj(param1 & 0xff);
        {
            int x = v->f2 << 12;
            int z = v->f6 << 12;
            int y = v->f4 << 12;
            pos.x = x;
            pos.y = y;
            pos.z = z;
        }
        mBaseAngleY = v->fa + 0x8000;
        return;
    }
    SetMode(9);
    mFovAngle = mMode->fovAngle;
    SetFovAngle(mFovAngle);
    func_0200cce4();
}

void dCamera_c::func_0200cce4() {
    Vector3 vec;
    VecFx32 tmp1;
    VecFx32 tmp2;
    u32 d;

    if (mTargetPlayer != 0) {
        Vector3 *p = &mTargetPlayer->mPos;
        mTargetPos.x = p->x;
        mTargetPos.y = p->y;
        mTargetPos.z = p->z;

        {
            u32 d1 = ScaleDistByFov(mMode->lookAtDist);
            tmp1.z = d1;
            tmp1.x = data_02086f2c.x;
            tmp1.y = data_02086f2c.y;
            data_02086f2c.z = d1;
        }
        Vec3_RotateYAndTranslate(&lookAt, &mTargetPos, mTargetPlayer->mAngleY, &tmp1);

        d = ScaleHeightByFov(mMode->lookAtHeight);
        lookAt.y += d;

        {
            u32 d2 = ScaleDistByFov(-mMode->posDist);
            tmp2.z = d2;
            data_02086f20.z = d2;
            tmp2.x = data_02086f20.x;
            tmp2.y = data_02086f20.y;
        }
        Vec3_RotateYAndTranslate(&pos, &lookAt, mTargetPlayer->mAngleY, &tmp2);

        dBgCh_Lin line;
        PrepareBgCh(line);
        {
            int tx;
            int ty;
            int tz;
            tx = pos.x;
            tz = pos.z;
            ty = pos.y + 0x100000;
            vec.x = tx;
            vec.y = ty;
            vec.z = tz;
        }
        line.SetObjAndLine(pos, vec, 0);
        if (line.DetectClsn() != 0) {
            Vector3 cp = line.GetClsnPos();
            vec.x = cp.x;
            vec.y = cp.y;
            vec.z = cp.z;
        }
        dBgCh_Gnd ground;
        PrepareBgCh(ground);
        ground.SetObjAndPos(vec, 0);
        if (ground.DetectClsn() != 0) {
            pos.y = d + ground.clsnY;
        }
        pos.y += ScaleHeightByFov(0x3c286);
        {
            int diff;
            int base;
            int a;
            base = lookAt.y;
            diff = pos.y - base;
            a = (diff < 0) ? -diff : diff;
            if (a > 0x3e8000) {
                if (diff < -0x3e8000) diff = -0x3e8000;
                else if (diff > 0x3e8000) diff = 0x3e8000;
                pos.y = base + diff;
            }
        }
        {
            u32 t = ScaleHeightByFov(mMode->lookAtHeight);
            mLookAtOffset.x = 0;
            mLookAtOffset.y = t;
            mLookAtOffset.z = 0;
        }
        UpdateAngles();
        ChangeArea(mTargetPlayer->mAreaId);
    }
    mFlags |= 4;
}

void dCamera_c::SetLookAt(const Vector3 & lookAt_)
{
    lookAt.x = lookAt_.x;
    lookAt.y = lookAt_.y;
    lookAt.z = lookAt_.z;
}

void dCamera_c::SetPos(const Vector3 & pos_)
{
    pos.x = pos_.x;
    pos.y = pos_.y;
    pos.z = pos_.z;
}

void dCamera_c::SaveCameraStateBeforeTalk()
{
    if (mFlags & 0x4000U)
        return;
    savedLookAt.x = lookAt.x;
    savedLookAt.y = lookAt.y;
    savedLookAt.z = lookAt.z;
    savedPos.x = pos.x;
    savedPos.y = pos.y;
    savedPos.z = pos.z;
    mFlags |= 0x4000U;
}

/* The other half of SaveCameraStateBeforeTalk: once EnterState has turned
   the save flag 0x4000 into 0x8000, UpdateState calls this every frame to ease
   lookAt and pos back to the saved pair, clearing 0x8000 when both arrive. */
void dCamera_c::RestoreCameraStateAfterTalk() {
    int lookDone = func_020092c4(this, &lookAt, &savedLookAt);
    int posDone = func_020092c4(this, &pos, &savedPos);
    short yaw;
    short pitch;

    if (posDone != 0) {
        if (lookDone != 0) {
            mFlags &= ~0x8000;
        }
    }

    yaw = Vec3_HorzAngle(&lookAt, &pos);
    mAngleY = yaw;

    pitch = Vec3_VertAngle(&lookAt, &pos);
    mAngleX = pitch;
}

/* Swaps the state object; the PNS_5StateE mangling is the evidence for the
 * nested State. Flag 0x10 vetoes; a genuine change tears down the outgoing
 * state only when it is the one at data_0209b0c8; the tail dispatch runs
 * regardless. */
int dCamera_c::ChangeState(State * state)
{
    if ((mFlags & 0x10) != 0)
        return 0;
    if (state != mState) {
        if (mState == &data_0209b0c8) {
            FUN_02029a68();
            UpdateTargetPos();
        }
        mState = state;
        mTurningToTarget = 0;
    }
    return EnterState();
}

void dCamera_c::SetMode(int index) {
    mMode = (CamMode *)(index * 0x28 + (int)&data_02086fcc);
}

/* State dispatch: honors the deferred-change flags, then calls the state's
 * +0 member-function pointer on the camera. */
int dCamera_c::EnterState() {
    CamPmfHost *host = (CamPmfHost *)this;
    if (host->flags & 0x4000) {
        unsigned int &flags = host->flags;
        flags &= ~0x4000u;
        flags |= 0x8000u;
    }
    CamPmf *enterPmf = host->pp;
    if (*enterPmf == 0)
        return 1;
    return (host->**enterPmf)();
}

/* Per-frame state tick: mFovAngle eases toward the mode record's +0x24
 * angle, then the state's +8 member-function pointer runs. */
int dCamera_c::UpdateState() {
    CamPmfHost *host = (CamPmfHost *)this;
    int flags;
    int result;
    {
        CamMode *mode = mMode;
        ApproachLinear2(mFovAngle, mode->fovAngle, 0x80);
    }
    result = 1;
    flags = mFlags;
    if (flags & 8) {
        ;
    } else if (flags & 0x8000) {
        RestoreCameraStateAfterTalk();
    } else {
        State *state = mState;
        if (state->exec != 0)
            result = (host->*state->exec)();
    }
    return result;
}

void dCamera_c::func_0200ca14(unsigned char playerID, int clpsBehavior) {
    if (playerID != gLocalPlayerId)
        return;

    if (clpsBehavior == 0)
        mFlags |= 0x20000;
    else
        mFlags |= 0x10000;
}

void dCamera_c::func_0200c9e0(s32 *minRise, s32 *maxRise) {
    unsigned int v = mFlags;
    if (v & 0x10000u) {
        *minRise = 0xfffffd39;
        *maxRise = 0xfffffd39;
        return;
    }
    if (v & 0x20000u) {
        *minRise = 0xd1b;
        *maxRise = 0xd1b;
    }
}

/* Chooses the camera mode and view object from the floor under pos: the
 * target player's own floor record when it is the player (except in level
 * 0x1c), otherwise a ground check from up to 0x100 units above pos. Records
 * the choice in mAreaMode and mViewObj. Returns the player's ground height,
 * or the floor height when a type-4 view object takes over. */
int dCamera_c::func_0200c66c(void *posArg, int *viewArg, int *modeArg, int *outFramed) {
    Vector3 *pos = (Vector3 *)posArg;
    void **outView = (void **)viewArg;
    void **outMode = (void **)modeArg;
    Vector3 above;
    int floorY;
    int areaType;
    int viewId;
    daPly_c *target;
    daPly_c *player;
    int groundY;

    target = (daPly_c *)mTargetPlayer;
    player = 0;
    if (IsPlayer(target)) {
        groundY = target->mGroundY;
        if (groundY == 0x80000000)
            groundY = target->mPosY;
        player = target;
    }

    if (player != 0 && gCurrentSublevel != 0x1c) {
        areaType = player->mFloorCameraBehavior & 0xff;
        viewId = player->mFloorViewID & 0xff;
        floorY = groundY;
    } else {
        dBgCh_Lin line;
        PrepareBgCh(line);
        {
            int x = pos->x;
            int z = pos->z;
            int y = pos->y + 0x100000;
            above.x = x;
            above.y = y;
            above.z = z;
        }
        line.SetObjAndLine(*pos, above, 0);
        if (line.DetectClsn() != 0) {
            Vector3 hit = line.GetClsnPos();
            above.x = hit.x;
            above.y = hit.y;
            above.z = hit.z;
        }
        dBgCh_Gnd ground;
        PrepareBgCh(ground);
        ground.SetObjAndPos(above, 0);
        if (ground.DetectClsn() == 0) {
            floorY = pos->y;
            areaType = 0;
            viewId = 0x3f;
        } else {
            floorY = ground.clsnY;
            areaType = ground.surface.clps.GetCameraBehavior() & 0xff;
            viewId = ground.surface.clps.GetViewID() & 0xff;
        }
    }

    if (areaType == 0 || (mFlags & 0x400) != 0) {
        if (mMode->flags & 0x10) {
            if (mAreaMode)
                *outMode = mAreaMode;
            if (mViewObj)
                *outView = mViewObj;
        }
    } else {
        if (viewId != 0x3f)
            *outView = GetViewObj(viewId);
        if (func_0200c394(areaType, *outView, pos, outMode, outFramed) == 0) {
            if ((u32)mMode <= (u32)&data_02086ff4 || *outMode == &data_02087274) {
                if (*outView != 0 && ((CamViewObj *)*outView)->type == 6)
                    *outMode = &data_0208742c;
            }
        } else if (viewId != 0x3f) {
            int type = ((CamViewObj *)*outView)->type;
            if (type == 4) {
                *outMode = &data_020873dc;
                if (((CamViewObj *)*outView)->p1 == 2) {
                    if (mAreaMode != *outMode)
                        mFlags |= 4;
                }
                groundY = floorY;
            } else if (type == 6) {
                *outMode = &data_0208742c;
            } else if ((mMode->flags & 4) == 0) {
                if (type == 0) {
                    if (*outMode == &data_02087274 && gCurrentSublevel == 0x2d) {
                        mLookTargetPos = mTargetPos;
                    }
                    *outMode = &data_020871ac;
                } else if (type == 1) {
                    *outMode = &data_020871d4;
                } else if (type == 5) {
                    *outMode = &data_02087404;
                }
            }
            mFlags &= ~0x100;
        }
        if (*outMode != &data_02087094)
            mAreaMode = (CamMode *)*outMode;
        if (*outMode != &data_0208733c) {
            if (*outView != 0 && ((CamViewObj *)*outView)->type != 2)
                mViewObj = *outView;
        }
    }

    return groundY;
}

/* Picks *outMode for the camera-area type under the target (the ground's CLPS
 * camera behaviour) and its view object. Type 7, or an actor to frame, aims
 * mLookTargetPos midway between that point and pos at half their distance.
 * Otherwise the default follow mode maps the other area types to their modes.
 * Returns 0 when the current mode (flag 0x20) or mFlags 0x100 holds the mode. */
int dCamera_c::func_0200c394(int areaType, void *viewArg, void *pos, void **outMode,
                             int *outFramed) {
    CamViewObj *view = (CamViewObj *)viewArg;
    CamMode *mode = mMode;
    VecFx32 toFocus;
    Vector3 focus;
    int modeFlags = mode->flags;
    int locked;

    if ((modeFlags & 0x20) || (locked = mFlags & 0x100) != 0)
        return 0;

    if (areaType == 7 && (mFramedActor != 0 || (view != 0 && view->type == 3))) {
        *outFramed = 1;
        if (mFramedActor != 0) {
            mLookTargetPos = mFramedActor->mPos;
        } else {
            int x = view->x << 12;
            int z = view->z << 12;
            int y = view->y << 12;
            mLookTargetPos.x = x;
            mLookTargetPos.y = y;
            mLookTargetPos.z = z;
        }
        Vec3_Sub(&toFocus, &mLookTargetPos, (const Vector3 *)pos);
        mFocusDist = VEC_Mag(&toFocus) >> 1;
        signed char level = gCurrentSublevel;
        int inLevel2d = level == 0x2d;
        if (inLevel2d) {
            if (mFocusDist < 0x100000)
                mFocusDist = 0x100000;
            else if (mFocusDist > 0x180000)
                mFocusDist = 0x180000;
        } else if (mFocusDist < 0x180000) {
            mFocusDist = 0x180000;
        }
        if (mFlags & 2) {
            mFocusDist = FX_MUL(mFocusDist, 0x1800);
        }
        if (level == 0x2f && mFocusDist > 0xc0000) {
            Vec3_MulScalarInPlace(&toFocus, FX_Div(0xc0000, mFocusDist << 1));
            Vec3_Add(&focus, (const Vector3 *)pos, &toFocus);
            mLookTargetPos = focus;
            mFocusDist = 0xc0000;
        } else {
            VEC_Add(&mLookTargetPos, pos, &mLookTargetPos);
            Vec3_AsrInPlace(&mLookTargetPos, 1);
        }
        *outMode = &data_02087274;
        return inLevel2d;
    }

    if (modeFlags & 4)
        return mode != &data_0208706c;
    if (mode == &data_0208715c) {
        if (locked)
            *outMode = &data_0208747c;
        else if (areaType == 8)
            *outMode = &data_0208715c;
        else if (areaType == 6)
            *outMode = &data_02087184;
        else if (areaType == 0xa)
            *outMode = &data_020871fc;
        else if (areaType == 0xb)
            *outMode = &data_02087224;
        else if (areaType == 9)
            *outMode = &data_0208733c;
        else if (areaType == 2)
            *outMode = &data_02087314;
    }
    return 1;
}

/* Places lookAt for the follow modes: mode's look-at point ahead of the
 * target player, pulled in by walls and ceilings and lifted onto the ground,
 * eases mLookAtOffset toward it at speed, then moves lookAt.y within the
 * mode's followRise. Returns the ground height under that point. */
int dCamera_c::func_0200bec4(int *target, int baseY, char *modeArg, int speed) {
    Vector3 goal;
    Vector3 to;
    Vector3 from;
    Vector3 above;
    Vector3 delta;
    VecFx32 offset;
    int groundY;
    int height;
    int horzDist;
    CamMode *mode = (CamMode *)modeArg;

    offset.z = ScaleDistByFov(mode->lookAtDist);
    data_02086f2c.z = offset.z;
    offset.x = data_02086f2c.x;
    offset.y = data_02086f2c.y;
    Vec3_RotateYAndTranslate(&goal, target, mTargetPlayer->mAngleY, &offset);
    dBgCh_Lin line;
    to.x = goal.x;
    to.y = goal.y + 0x32000;
    to.z = goal.z;
    SetVec(from, target[0], target[1] + 0x32000, target[2]);
    PrepareBgCh(line);
    line.SetObjAndLine(from, to, 0);
    if (line.DetectClsn() != 0) {
        Vector3 hit = line.GetClsnPos();
        goal.x = hit.x;
        goal.y = hit.y;
        goal.z = hit.z;
    }
    above.x = goal.x;
    above.y = goal.y + 0x100000;
    above.z = goal.z;
    PrepareBgCh(line);
    line.SetObjAndLine(goal, above, 0);
    if (line.DetectClsn() != 0) {
        Vector3 hit = line.GetClsnPos();
        above = hit;
    }
    dBgCh_Gnd ground;
    PrepareBgCh(ground);
    ground.SetObjAndPos(above, 0);
    groundY = goal.y;
    if (ground.DetectClsn() != 0) {
        int rise;
        groundY = ground.clsnY;
        rise = groundY - goal.y;
        if (rise > 0) {
            if (rise > 0x30000)
                rise = 0x30000;
            goal.y += rise;
        }
    }
    VEC_Subtract(&goal, target, &goal);
    height = mode->lookAtHeight;
    if (mFlags & 0x20000) {
        height = FX_MUL(height, 0x3000);
    }
    goal.y += ScaleHeightByFov(height);
    horzDist = Vec3_HorzLen((const Vector3 *)&goal);
    VecFx32 focus;
    Vec3_MulScalar((Vector3 *)&focus, &mLookTargetPos, mFocusBlend);
    VEC_Add(&goal, &focus, &goal);
    if (mFlags & 4) {
        mLookAtOffset.x = goal.x;
        mLookAtOffset.y = goal.y;
        mLookAtOffset.z = goal.z;
    } else {
        int shrink = Vec3_HorzLen(&mLookAtOffset) - horzDist;
        if (mode->flags & 0x40) {
            speed = 0x10000;
        } else if (shrink > 0) {
            speed += FX_MUL(shrink, 0x400);
        }
        Vec3_Sub(&delta, &goal, &mLookAtOffset);
        horzDist = Vec3_HorzLen(&delta);
        {
            int left = horzDist;
            int frac;
            if (horzDist != 0) {
                Math_Function_0203b14c(&left, 0, 0x100, FX_MUL(speed, 0x600), 0x1000);
                frac = FX_Div(left, horzDist);
                mLookAtOffset.x = goal.x - FX_MUL(delta.x, frac);
                mLookAtOffset.z = goal.z - FX_MUL(delta.z, frac);
            }
        }
        mLookAtOffset.y = goal.y - FX_MUL(goal.y - mLookAtOffset.y, 0xf80);
        if (mFlags & 0x400) {
            return groundY;
        }
    }
    lookAt.x = target[0] + mLookAtOffset.x;
    lookAt.z = target[2] + mLookAtOffset.z;
    {
        int goalY = target[1] + mLookAtOffset.y;
        int climb = goalY - lookAt.y;
        if (climb < (int)ScaleHeightByFov(mode->followRise)) {
            ApproachLinear(*&lookAt.y, goalY, 0x64000);
        } else {
            int drop = baseY - target[1];
            if (drop >= 0) {
                drop = 0;
            } else {
                int lim = ScaleHeightByFov(-0xf0a00);
                if (drop < lim)
                    drop = lim;
            }
            if (drop + climb > 0) {
                lookAt.y = goalY - (climb - FX_MUL(drop + climb, 0x600));
            }
        }
    }
    return groundY;
}

/* ---------------------------------------------------------------------------
 * The camera's states and script ops, 0x020073a0..0x0200bec4.
 *
 * The states run through the State records in the 0x0209b008 table, and the
 * script ops through the member-function table RunCameraCommand builds from
 * 0x02086bc0. Their names are not recovered: both dispatches reach them as
 * the code words of the .data PMF constants, whose symbol is the address
 * label. The follow and placement helpers among them are dCamera_c members;
 * the rest stay free and take the camera as their first argument.
 * ------------------------------------------------------------------------- */

/* KuppaScript.cpp's readers of the script's unaligned operands and its
   lookups of the script's actors. */
dActor_c *KuppaScript::FindScriptPlayer(u32 character);
dActor_c *KuppaScript::FindDemoActor(u32 param1);

/* local extern: math/CalcLib.h spells the Vec3 helpers with const Vector3 *,
   which this file's VecFx32 callers cannot pass; only these two are taken
   from it. */
/* int target: a short parameter sign-extends and ApproachLookAtAndAngles /
   func_02009e70 no longer reproduce. C linkage matches the definition. */
extern "C" s32 ApproachAngle(s16 *cur, s32 target, s32 div, s32 band, s32 step);
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
/* local extern: math/MtxStack.h declares these; see the matrix helpers above. */
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, short angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angle);

extern int NormalizeVec3IfNonZero(Vector3 *v);
extern fx32 VEC_DotProduct(const Vector3 *a, const Vector3 *b);
/* The 64-bit squared distance between two points. */
extern long long func_0203cf94(int *a, int *b);
}

extern void func_ov002_020effb8(Vector3 *out, s32 t, Vector3 *p0, Vector3 *p1, Vector3 *p2,
                                Vector3 *p3);

extern void FUN_02029ab0(void);
extern int Sound_PlayIfNotActive(int handle, int bank, int id, short arg);
extern void func_020070e8(dCamera_c *cam, int distTarget, int distStep, short vertTarget,
                          short vertDiv, short horzTarget, short horzDiv);
extern void StarDanceBoundYaw(dCamera_c *cam, short ang, int threshold);

extern dActor_c *STAR_MARKERS[12];
extern Matrix4x3 data_020a0e68;
extern u8 data_0209f224;
/* The per-player controller records, indexed by gActivePlayerSlot: the
   camera reads the turn buttons (held, pressed), the camera-input scale (mag)
   and axes (nx, ny), and the control mode. */
extern ControllerInput data_0209f498[];
extern u8 gGameMode;
extern int data_02086cc8[2];
extern int data_02086d50[2];
extern Vector3 data_02086e84;
extern int data_02086e90[3];
extern int data_02086e9c[3];
extern int data_02086ea8[3];
extern int data_02086eb4[3];
extern int data_02086ec0[3];
extern int data_02086ed8[3];
extern int data_02086f38[4];
extern int data_02086f48[4];
extern struct CamMode data_020870e4;
extern struct CamMode data_0208710c;
extern struct CamMode data_0208724c;
extern struct CamMode data_0208729c;
extern struct CamMode data_020874cc;
extern "C" {
/* Siblings defined below. */
int func_02009414(dCamera_c *cam);
int func_020094a8(dCamera_c *cam);
int func_02008b08(dCamera_c *cam);
short func_02008abc(dCamera_c *cam);
short func_02008a70(dCamera_c *cam);
}

extern "C" void Vec3fSetDistAndAngle(Vector3 *res, const Vector3 *trans, int mag, short ang,
                                     short angY);
extern "C" void Vec3fGetDistAndAngle(const Vector3 *v0, const Vector3 *v1, int *outDist,
                                     short *outVertAng, short *outHorzAng);
void ApproachVec3fAsymptotic(Vector3 *self, const Vector3 *src, int stepX, int stepY, int stepZ);
extern "C" {
/* State entries and helpers defined below. The declarations carry the C
   linkage their ROM symbols need. */
int func_02009e30(dCamera_c *cam);
int func_02009e10(dCamera_c *cam);
int func_02009df0(dCamera_c *cam);
int func_02009dd0(dCamera_c *cam);
int func_02009db0(dCamera_c *cam);
int func_02009d90(dCamera_c *cam);
int func_02009d70(dCamera_c *cam);
int func_02009aa8(dCamera_c *cam);
int func_020098b8(dCamera_c *cam);
void func_0200987c(dCamera_c *cam);
int func_0200985c(dCamera_c *cam);
int func_020097ec(dCamera_c *cam);
int func_020095e4(dCamera_c *cam);
int func_020095c4(dCamera_c *cam);
int func_02009540(dCamera_c *cam);
int func_020094e4(dCamera_c *cam);
void func_020094c0(dCamera_c *cam);
}

/* The turn-by-buttons follow (mode data_0208733c), Super Mario 64's
   mode_8_directions_camera: the yaw is mBaseAngleY plus mBaseAngleOffset
   (s8DirModeBaseYaw plus s8DirModeYawOffset there), the 0x100/0x200 buttons
   step mBaseAngleOffset by 0x2000 (45 degrees) and button 0x4000 swings the
   camera behind the player. A type-2 view object supplies the base yaw at
   +0xa; its byte 1 zero means jump to it at once (snap mEasedAngleY and set
   mFlags bit 4, which places pos directly). */
void dCamera_c::Mode8DirectionsCamera(CamViewObj *view) {
    s32 flag4000;
    s32 sp_speed;
    struct Vector3 vsrc;
    struct Vector3 vin;
    struct Vector3 vout;
    s32 *tbl;
    s32 comp;

    mFlags |= 0x1000;

    flag4000 = data_0209f498[gActivePlayerSlot].held & 0x4000;

    if (flag4000 != 0 || mRecentering != 0) {
        if (data_0209f498[gActivePlayerSlot].pressed & 0x4000) {
            func_02012790(0x1a);
        }
        if (flag4000 != 0) {
            mBehindAngleY = (mTargetPlayer->mAngleY + 0x8000);
        }
        mRecentering = (ApproachAngle(&mEasedAngleY, mBehindAngleY, 4, 0x400, 0x100) != 0);
        mBaseAngleOffset = (mEasedAngleY - mBaseAngleY);
        mBaseAngleOffset = ((mBaseAngleOffset + 0x1000) & -0x2000);
        mFlags |= 0x60;
    } else {
        if (view != 0) {
            if (view->type == 2 && view != mViewObj) {
                mBaseAngleY = (view->angleY + 0x8000);
                if (view->p1 == 0) {
                    mEasedAngleY = mBaseAngleY;
                    mFlags |= 4;
                }
                mBaseAngleOffset = 0;
                mViewObj = view;
            }
        } else {
            mViewObj = 0;
        }
        if (!(mFlags & 0x60)) {
            s32 f1;
            /* Here mStateTimer is the delay before a held turn button
               repeats. */
            DecIfAbove0_Short(&mStateTimer);
            f1 = data_0209f498[gActivePlayerSlot].held & 0x300;
            if (f1 != 0) {
                s32 f2 = data_0209f498[gActivePlayerSlot].pressed & 0x300;
                if (f2 != 0) {
                    mTurnButtons = f2;
                } else if (!(mTurnButtons & f1)) {
                    f2 = f1;
                    mTurnButtons = f1;
                }
                if (f2 != 0 || mStateTimer == 0) {
                    u32 dir = mTurnButtons;
                    if (dir & 0x200) {
                        mBaseAngleOffset -= 0x2000;
                        mFlags |= 0x20;
                    } else if (dir & 0x100) {
                        mBaseAngleOffset += 0x2000;
                        mFlags |= 0x40;
                    }
                    mStateTimer = 0x14;
                }
            }
        }
        ApproachLinear(mEasedAngleY, (mBaseAngleY + mBaseAngleOffset), 0x800);
    }

    tbl = (mFlags & 2) ? data_02086d50 : data_02086cc8;
    sp_speed = tbl[1];
    func_0200c9e0(&sp_speed, &sp_speed);
    comp = FX_MUL(tbl[0], sp_speed);
    vsrc.x = 0;
    vsrc.z = 0;
    vsrc.y = comp;
    {
        s32 radius = tbl[0];
        vsrc.z = cstd::sqrt((s64)radius * radius - (s64)comp * comp);
    }
    {
        s32 tz = mTargetPos.z;
        s32 ty = lookAt.y;
        s32 tx = mTargetPos.x;
        vin.x = tx;
        vin.y = ty;
        vin.z = tz;
    }
    Vec3_RotateYAndTranslate(&vout.x, &vin.x, mEasedAngleY, &vsrc.x);
    if (mFlags & 4) {
        pos.x = vout.x;
        pos.y = vout.y;
        pos.z = vout.z;
    }
    Math_Function_0203b14c(&pos.y, vout.y, 0x300, 0x18000, 0x100);
    vout.y = pos.y;
    if (ApproachLinear(pos, vout, flag4000 != 0 ? 0xc0000 : 0x40000) != 0 && flag4000 == 0) {
        mFlags &= ~0x60;
    }
    mAngleY = mEasedAngleY;
}

/* The orbit for modes data_020871ac and data_020871d4, the radial camera of
   Super Mario 64's update_radial_camera and radial_camera_move, which turn
   around the area's centre point: ease mAngleY, at a rate scaled by how far
   the player moved this frame, toward the yaw from the view object (the
   centre) to lookAt, pushed a step to the side the target player faces away
   from. With byte 0 and byte 1 both clear, a wall between lookAt and the
   object can take over the yaw (except in three levels). */
void dCamera_c::UpdateRadialCamera(CamViewObj *obj, int arg2) {
    Vector3 v;
    s16 angle;
    u8 b0, b1;
    int step;
    int rate;

    {
        int sz = obj->z << 12;
        int sy = obj->y << 12;
        int sx = obj->x << 12;
        v.x = sx;
        v.y = sy;
        v.z = sz;
    }
    angle = Vec3_HorzAngle(&v, &lookAt);

    rate = 8;
    b0 = obj->type;
    if (b0 == 0) {
        b1 = obj->p1;
        if (b1 == 0xff) {
            signed char zone = gCurrentSublevel;
            if (zone != 0x16 && zone != 0xa && zone != 0x10) {
                dBgCh_Lin rc;
                Vector3 lineB;
                PrepareBgCh(rc);
                lineB.y = lookAt.y;
                lineB.x = v.x;
                lineB.z = v.z;
                rc.SetObjAndLine(lookAt, lineB, 0);
                if (rc.DetectClsn() != 0) {
                    Vector3 normal;
                    s16 a2;
                    rc.surface.CopyNormalTo(normal);
                    a2 = cstd::atan2(normal.x, normal.z);
                    if (AngleDiff(a2, angle) < 0x1800)
                        angle = a2;
                }
            }
            step = 0x2000;
        } else {
            step = 0;
            rate = (gCurrentSublevel == 0x2d) ? 0x12 : 0xf;
        }
    } else {
        step = 0x6800;
    }

    {
        dActor_c *ptr = mTargetPlayer;
        s16 target = ptr->mAngleY;
        s16 diff = (angle - target);
        if (diff < 0) {
            angle -= step;
        } else {
            angle += step;
        }
    }

    {
        int fixmul = FX_MUL(arg2, rate);
        ApproachLinear(mAngleY, angle, fixmul);
    }
}

/* The fixed-point follow (mode data_020873dc), Super Mario 64's
   update_fixed_camera: place pos on the line from the view object's ground
   point (sFixedModeBasePosition there) to the target, at the fraction and
   distance limits of row p1 of data_02086f58, raised by 0x220000 less the
   distance when closer than that (the N64's 300 - distCamToFocus). */
void dCamera_c::UpdateFixedCamera(CamViewObj *obj, int arg2) {
    Vector3 viewPos;
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    Vector3 v4;
    int idx;
    Entry3 *e;
    int len;
    int limit;
    int scale;
    int c;
    int sum;

    idx = obj->p1;
    if (idx == 0xff)
        idx = 0;

    e = &data_02086f58[idx];
    {
        int tz = obj->z << 12;
        int ty = obj->y << 12;
        int tx = obj->x << 12;
        viewPos.x = tx;
        viewPos.y = ty;
        viewPos.z = tz;
    }

    dBgCh_Gnd ray;
    ray.SetObjAndPos(viewPos, 0);
    if (ray.DetectClsn() != 0) {
        viewPos.y -= ray.clsnY;
    }

    sum = arg2 + viewPos.y;
    {
        int sz = mTargetPos.z;
        int sx = mTargetPos.x;
        int dz = sz - viewPos.z;
        int dx = sx - viewPos.x;
        int dy = sum - viewPos.y;
        v1.x = dx;
        v1.y = dy;
        v1.z = dz;
    }
    c = e->c;
    v2.y = sum;
    v2.x = viewPos.x + FX_MUL(v1.x, c);
    v2.z = viewPos.z + FX_MUL(v1.z, c);

    len = VEC_Mag(&v1);
    if (len < 0x220000) {
        v2.y += 0x220000 - len;
    }

    Vec3_Sub(&v3, &v2, &lookAt);
    v1.x = v3.x;
    v1.y = v3.y;
    v1.z = v3.z;
    len = VEC_Mag(&v1);

    limit = e->a;
    if (mFlags & 0xc00)
        limit = e->b;

    if (len > limit) {
        scale = FX_Div(limit, len);
        Vec3_MulScalarInPlace(&v1, scale);
        Vec3_Add(&v4, &lookAt, &v1);
        v2.x = v4.x;
        v2.y = v4.y;
        v2.z = v4.z;
    }

    if (mFlags & 4) {
        pos.x = v2.x;
        pos.y = v2.y;
        pos.z = v2.z;
    } else {
        func_020092c4(this, &pos, &v2);
    }

    mAngleY = Vec3_HorzAngle(&lookAt, &pos);
}

/* The view-object orbit (mode data_02087404), Super Mario 64's
   update_spiral_stairs_camera: turn data_02086e84 by the yaw from the object
   (the staircase centre) to the target, held within 0x4000 (90 degrees) of
   the yaw from the object to pos, probe the floor 0.7 (0xb33) of the way out
   from lookAt, and lift the result 125 (0x7d000) above the floor below it. */
void dCamera_c::UpdateSpiralStairsCamera(CamViewObj *obj, int arg2) {
    Vector3 v0;
    Vector3 rotated;
    short angle1;
    short angle2;
    int diff;
    short newAngle;
    Vector3 pos;
    Vector3 base;
    int dx, dz;
    int hitY;
    int dist;
    int rem;
    int selfZ;
    int selfX;

    {
        int tz = obj->z << 12;
        int ty = obj->y << 12;
        int tx = obj->x << 12;
        v0.x = tx;
        v0.y = ty;
        v0.z = tz;
    }

    angle1 = Vec3_HorzAngle(&mTargetPos, &v0);
    angle2 = Vec3_HorzAngle(&this->pos, &v0);
    diff = (short)(angle2 - angle1);
    if (diff < -0x4000)
        diff = -0x4000;
    else if (diff > 0x4000)
        diff = 0x4000;
    newAngle = (angle1 + diff);

    base.x = data_02086e84.x;
    base.y = data_02086e84.y;
    base.z = data_02086e84.z;
    Vec3_RotateYAndTranslate(&rotated, &v0, newAngle, &base);
    rotated.y = arg2;

    selfZ = lookAt.z;
    dz = rotated.z - selfZ;
    selfX = lookAt.x;
    dx = rotated.x - selfX;
    pos.y = arg2 + 0x100000;

    pos.x = selfX + FX_MUL(dx, 0xb33);
    pos.z = selfZ + FX_MUL(dz, 0xb33);

    dBgCh_Gnd rg;
    PrepareBgCh(rg);
    rg.SetObjAndPos(pos, 0);
    if (rg.DetectClsn()) {
        hitY = rg.clsnY;
        rotated.y = hitY;
        if (hitY < arg2)
            rotated.y = arg2;
    }
    rotated.y = rotated.y + 0x7d000;
    dist = Vec3_HorzDist(&this->pos, &mTargetPos);
    rem = 0x100000 - dist;
    if (rem > 0)
        rotated.y += rem;
    if (mFlags & 4) {
        this->pos.x = rotated.x;
        this->pos.y = rotated.y;
        this->pos.z = rotated.z;
    } else {
        func_020092c4(this, &this->pos, &rotated);
    }

    mAngleY = Vec3_HorzAngle(&lookAt, &this->pos);
}

/* The path follow (mode data_0208742c), Super Mario 64's
   update_parallel_tracking_camera: keep pos on the path the view object's
   byte 1 names, abreast of lookAt, stepping mPathNode forward or back as
   lookAt passes a node, with the N64's 400 and 700 limits as 0x190000 and
   0x2bc000. */
void dCamera_c::UpdateParallelTrackingCamera(CamViewObj *obj, int floorY) {
    PathPtr path;
    struct Vector3 node0;
    struct Vector3 node1;
    struct Vector3 dir;
    struct Vector3 toPos;
    struct Vector3 toTarget;
    struct Vector3 cross1;
    struct Vector3 side;
    int idx;
    int projSide;
    int projDir;
    int diff;
    int maxDelta;
    int negMax;
    int clamped;
    int segLen;
    struct Vector3 node2;
    struct Vector3 nextStep;
    struct Vector3 diffBack;
    int lenFwd;
    int lenBack;
    struct Vector3 diffFwd2;
    struct Vector3 prevNode;
    struct Vector3 diffPrev;
    int lenA;
    int lenB;
    struct Vector3 targetPos;
    struct Vector3 farPoint;
    struct Vector3 diffFar;
    struct Vector3 back;
    struct Vector3 farBack;
    struct Vector3 vecFwd2;
    struct Vector3 farFwd2;
    struct Vector3 farPrev;
    struct Vector3 diffFarPrev;
    struct Vector3 offset;
    int pullFactor;
    int sideScale;

    path.FromID(obj->p1);

    if (mFlags & 4) {
        mPathNode = 0;
    }
    idx = mPathNode + 1;

    path.GetNode(node0, mPathNode);
    path.GetNode(node1, idx);
    Vec3_Sub(&dir, &node1, &node0);
    segLen = VEC_Mag(&dir);
    NormalizeVec3IfNonZero(&dir);

    Vec3_Sub(&toPos, &lookAt, &node0);
    Vec3_Sub(&toTarget, &pos, &node0);
    func_0203cf00(&cross1, &toPos, &dir);
    func_0203cf00(&side, &dir, &cross1);
    NormalizeVec3IfNonZero(&side);

    projSide = VEC_DotProduct(&toPos, &side);
    projDir = VEC_DotProduct(&toPos, &dir);
    diff = projDir - VEC_DotProduct(&toTarget, &dir);

    if (path.GetUnk004() == 0xff) {
        maxDelta = 0;
    } else {
        maxDelta = path.GetUnk004() << 12;
    }
    negMax = -maxDelta;
    if (diff < negMax) {
        clamped = negMax;
    } else {
        if (diff <= maxDelta)
            maxDelta = diff;
        clamped = maxDelta;
    }
    projDir -= clamped;

    if (idx + 1 < (int)path.NumNodes()) {
        path.GetNode(node2, idx + 1);
        Vec3_Sub(&nextStep, &node2, &node1);
        Vec3_MulScalarInPlace(&nextStep, FX_Div(0x190000, VEC_Mag(&nextStep)));
        Vec3_Add(&farPoint, &node1, &nextStep);
        Vec3_Sub(&diffFar, &farPoint, &lookAt);
        nextStep.x = diffFar.x;
        nextStep.y = diffFar.y;
        nextStep.z = diffFar.z;

        Vec3_MulScalar(&back, &dir, -0x190000);
        Vec3_Add(&farBack, &node1, &back);
        Vec3_Sub(&diffBack, &farBack, &lookAt);

        lenBack = VEC_Mag(&diffBack);
        lenFwd = VEC_Mag(&nextStep);
        if (lenFwd < lenBack) {
            mPathNode++;
        }
    } else if (path.Loops() != 0) {
        if (projDir > segLen)
            projDir = segLen;
    }

    if (mPathNode != 0 && mPathNode != idx) {
        Vec3_MulScalar(&vecFwd2, &dir, 0x2bc000);
        Vec3_Add(&farFwd2, &node0, &vecFwd2);
        Vec3_Sub(&diffFwd2, &farFwd2, &lookAt);

        path.GetNode(prevNode, mPathNode - 1);
        Vec3_Sub(&diffPrev, &prevNode, &node0);
        Vec3_MulScalarInPlace(&diffPrev, FX_Div(0x190000, VEC_Mag(&diffPrev)));
        Vec3_Add(&farPrev, &node0, &diffPrev);
        Vec3_Sub(&diffFarPrev, &farPrev, &lookAt);
        diffPrev.x = diffFarPrev.x;
        diffPrev.y = diffFarPrev.y;
        diffPrev.z = diffFarPrev.z;

        lenA = VEC_Mag(&diffFwd2);
        lenB = VEC_Mag(&diffPrev);
        if (lenB < lenA) {
            mPathNode--;
        }
    } else if (path.Loops() != 0) {
        if (projDir < 0)
            projDir = 0;
    }

    Vec3_MulScalar(&offset, &dir, projDir);
    Vec3_Add(&targetPos, &node0, &offset);

    if (path.GetPullFactor() == 0xff) {
        pullFactor = 0;
    } else {
        pullFactor = path.GetPullFactor() << 4;
    }

    if (gCurrentSublevel == 0x2f) {
        projSide = projSide - FX_MUL(mFocusDist, 0x5000);
        if (projSide < 0) {
            targetPos.y -= FX_MUL(projSide, 0x400);
            projSide = 0;
        }
    } else {
        projSide = FX_MUL(projSide, pullFactor);
    }

    Vec3_MulScalarInPlace(&side, projSide);
    VEC_Add(&targetPos, &side, &targetPos);

    if (mFlags & 4) {
        pos.x = targetPos.x;
        pos.y = targetPos.y;
        pos.z = targetPos.z;
    } else {
        func_020092c4(this, &pos, &targetPos);
    }

    mAngleY = Vec3_HorzAngle(&lookAt, &pos);
}

/* Render's view from mFixedViewPos: v1 halfway up from the fixed point to
   the target, v2 far out and above the target on the bearing from v1, boxed
   by data_02086f38 in level 6 and data_02086f48 in level 0x15. */
void dCamera_c::func_0200af20(Vector3 *v1, Vector3 *v2, short *out) {
    Vector3 *f = (Vector3 *)mFixedViewPos;
    int *b;
    unsigned short ang;
    int y;
    int z;

    if (f == 0)
        return;

    b = 0;
    {
        signed char g = gCurrentSublevel;
        if (g == 6) {
            b = data_02086f38;
        } else if (g == 0x15) {
            if (mTargetPos.y < 0)
                return;
            b = data_02086f48;
        }
    }

    z = f->z;
    y = f->y;
    y = y + mTargetPos.y;
    v1->x = f->x;
    v1->y = y >> 1;
    v1->z = z;
    ang = Vec3_HorzAngle(v1, &mTargetPos);

    v2->y = mTargetPos.y + 0x8f7690;
    v2->x = mTargetPos.x + FX_MUL(FX_SinIdx(ang >> 4), 0x15a6f80);
    v2->z = mTargetPos.z + FX_MUL(FX_CosIdx(ang >> 4), 0x15a6f80);

    if (b != 0) {
        {
            int lo = b[1];
            int hi = b[0];
            int x = v2->x;
            if (x >= lo) {
                if (x <= hi)
                    hi = x;
                lo = hi;
            }
            v2->x = lo;
        }
        {
            int lo = b[3];
            int hi = b[2];
            int x = v2->z;
            if (x >= lo) {
                if (x <= hi)
                    hi = x;
                lo = hi;
            }
            v2->z = lo;
        }
    }

    *out = 0xe38;
    data_0209f43c.Func_020156DC(mAspect, *out, mNear, mFar);
    mFlags &= ~1;
}

int dCamera_c::func_0200af0c() {
    mMode = &data_0208715c;
    return 1;
}

/* func_02009e70's helper. */
static inline int CheckMode(void) {
    return gGameMode == 1;
}

static inline s32 CollisionCeiling(dBgCh_Lin &line, s32 floorHeight) {
    Vector3 pos = line.GetClsnPos();
    s32 height = pos.y - 0x80000;
    if (height < floorHeight)
        height = floorHeight;
    return height;
}

static inline void SetCollisionPosition(dCamera_c *cam, dBgCh_Lin &line) {
    Vector3 pos = line.GetClsnPos();
    cam->mWallPos.x = pos.x;
    cam->mWallPos.y = pos.y;
    cam->mWallPos.z = pos.z;
}

static inline void MoveCameraPosition(dCamera_c *cam, const Vector3 &offset) {
    Vector3 pos;
    Vec3_Add(&pos, &cam->lookAt, &offset);
    cam->pos.x = pos.x;
    cam->pos.y = pos.y;
    cam->pos.z = pos.z;
}

static inline s32 LimitCameraCeiling(dBgCh_Lin &line, Vector3 &endpoint, s32 ceiling) {
    Vector3 pos = line.GetClsnPos();
    endpoint.y = pos.y;
    if (ceiling > pos.y - 0x80000)
        ceiling = pos.y - 0x80000;
    return ceiling;
}

static inline s32 LimitMidpointCeiling(dBgCh_Lin &line, Vector3 &endpoint, s32 ceiling) {
    Vector3 pos = line.GetClsnPos();
    endpoint.y = pos.y - 0x80000;
    if (ceiling > endpoint.y)
        ceiling = endpoint.y;
    return ceiling;
}

/* The per-frame follow: track the target player, pick the mode record and
   view object for its area (func_0200c66c), hand the four special modes to
   their handlers, and otherwise orbit pos around lookAt at the mode's
   distances, turning from walls and keeping pos between the ground and the
   ceiling.
 *
 * Register-shaped locals keep their ROM register names. The compiler ranks a
 * reused local's later live ranges by the order the local was first assigned,
 * so the near limit reuses the mode-flags local and the far limit reuses the
 * rate/near-distance local; that reuse is what places them in fp and sl. */
s32 dCamera_c::func_02009e70() {
    /* Last frame's turn buttons (turnInput at the bottom): func_02012790(0xe) fires
       only on the frame they go from released to held. */
    static s32 sPrevTurnInput;
    s32 lookAtGroundY;
    s32 nearDist;
    s32 farDist;
    s32 minY;
    s32 prevDist;
    union {
        u16 pitch;
        s16 pitchS;
    };
    union {
        struct CamMode *mode;
        int modeAsInt;
    };
    union {
        CamViewObj *view;
        int viewAsInt;
    };
    s32 framing;
    s32 dist;
    s32 minRise;
    s32 maxRise;
    struct Vector3 targetMove;
    struct Vector3 offset;
    s32 r7;
    s32 r6;
    s32 sb;
    s32 r4;
    s16 sba;
    s32 r5;
    s32 fp;
    u32 goalY;
    struct CamMode *m;
    s32 t0;
    s32 turnInput;
    s16 diff;
    s32 ceil;

    if (mTargetPlayer == 0) {
        mAngleY = Vec3_HorzAngle(&lookAt, &pos);
    } else {
        mode = (mFlags & 0x100) ? &data_0208747c : mMode;
        Vec3_Sub(&targetMove, &mTargetPlayer->mPos, &mTargetPos);
        {
            struct Vector3 *q = &mTargetPlayer->mPos;
            mTargetPos.x = q->x;
            mTargetPos.y = q->y;
            mTargetPos.z = q->z;
        }
        r7 = Vec3_HorzLen(&targetMove);
        view = 0;
        framing = 0;
        if (mFocusActor != 0) {
            struct Vector3 *q;
            framing = 1;
            mFramedActor = 0;
            q = &mFocusActor->mPos;
            mLookTargetPos.x = q->x;
            mLookTargetPos.y = q->y;
            mLookTargetPos.z = q->z;
            mFocusDist = 0xdfe60;
        }
        r6 = func_0200c66c(&mTargetPos, &viewAsInt, &modeAsInt, &framing);
        r4 = 0;
        if (framing != 0) {
            r4 = 0x1000;
            VEC_Subtract(&mLookTargetPos, &mTargetPos, &mLookTargetPos);
        }
        Math_Function_0203b14c(&mFocusBlend, r4, 0x100, 0x80, 0x10);

        fp = mode->flags;
        r4 = mode->minDist;
        r5 = mode->posDist;
        if ((fp & 1) == 0 && (mFlags & 2) != 0) {
            r4 = FX_MUL(r4, 0x1800);
            r5 = FX_MUL(r5, 0x1800);
        }
        if (CheckMode() && (fp & 2) != 0 && IsTargetNearStarMarker() != 0) {
            r4 = FX_MUL(r4, 0x14cc);
            r5 = FX_MUL(r5, 0x14cc);
        }
        if (mFlags & 0x80) {
            r4 = FX_MUL(r4, 0x1800);
            r5 = FX_MUL(r5, 0x1800);
        }
        /* near limit -> fp (reuses the flags local), far limit -> r4 (reuses the
           near-distance local): both must be later live ranges of already-assigned
           locals for the allocator to colour them fp/sl. */
        t0 = mFocusDist - r4;
        fp = ScaleDistByFov(r4 + FX_MUL(t0, mFocusBlend));
        if (framing != 0) {
            r4 = fp;
        } else {
            if (r5 < 0)
                r5 = -r5;
            r4 = ScaleDistByFov(r5);
        }
        lookAtGroundY = func_0200bec4(&mTargetPos.x, r6, (char *)mode, r7);
        turnInput = data_0209f498[gActivePlayerSlot].pressed & 0x4300;
        if (mFocusActor != 0) {
            mAngleY = Vec3_HorzAngle(&lookAt, &pos);
        } else if ((u32)mode == (u32)&data_0208733c) {
            Mode8DirectionsCamera(view);
            turnInput = 0;
        } else if ((u32)mode == (u32)&data_020873dc) {
            UpdateFixedCamera(view, r6);
        } else if ((mFlags & 0x400) == 0) {
            if (mode == &data_02087404) {
                UpdateSpiralStairsCamera(view, r6);
            } else if ((u32)mode == (u32)&data_0208742c) {
                UpdateParallelTrackingCamera(view, r6);
            } else {
                mFlags |= 0x1000;
                nearDist = ScaleDistByFov(mode->nearDist);
                farDist = ScaleDistByFov(mode->farDist);
                if (view != 0 && view->type == 2) {
                    if (view->p1 == 0) {
                        s32 tx = view->x << 12;
                        s32 tz = view->z << 12;
                        s32 ty = view->y << 12;
                        pos.x = tx;
                        pos.y = ty;
                        pos.z = tz;
                    } else {
                        s16 a = (view->angleY + 0x8000);
                        if (mTargetAngleY != a) {
                            mTargetAngleY = a;
                            mTurningToTarget = 1;
                        }
                    }
                } else {
                    if (mTurningToTarget == 0) {
                        mTargetAngleY += 0x8000;
                    }
                }
                Vec3_Sub(&offset, &pos, &lookAt);
                dist = VEC_Mag(&offset);
                {
                    minY = 0x80000000;
                    Vector3 probe(mTargetPos.x, mTargetPos.y + 0x3e8000, mTargetPos.z);
                    Vector3 wallFrom;
                    Vector3 wallNormal;
                    ceil = 0x7fffffff;
                    sb = mTargetPos.y + 0x82500;
                    {
                        dBgCh_Lin line;
                        PrepareBgCh(line);
                        line.SetObjAndLine(mTargetPos, probe, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = CollisionCeiling(line, sb);
                        }
                        if ((mFlags & 0x100) == 0 && (mode->flags & 8) != 0) {
                            {
                                s32 tz = mTargetPos.z;
                                s32 tx = mTargetPos.x;
                                wallFrom.x = tx;
                                wallFrom.y = sb;
                                wallFrom.z = tz;
                            }
                            if (sb <= ceil) {
                                line.SetObjAndLine(wallFrom, pos, 0);
                                if (line.DetectClsn() != 0) {
                                    line.surface.CopyNormalTo(wallNormal);
                                    SetCollisionPosition(this, line);
                                    mWallNormal.x = wallNormal.x;
                                    mWallNormal.y = wallNormal.y;
                                    mWallNormal.z = wallNormal.z;
                                    t0 = wallNormal.y;
                                    if (t0 < 0)
                                        t0 = -t0;
                                    if (t0 > 0xb50) {
                                        mFlags |= 0x80200;
                                    } else {
                                        Vector3 wallHit = line.GetClsnPos();
                                        sba = cstd::atan2(wallNormal.x, wallNormal.z) + 0x8000;
                                        diff = sba - mAngleY;
                                        sba += (diff < 0) ? 0x4000 : -0x4000;
                                        if (mTurningToTarget != 0) {
                                            if (AngleDiff(sba, mTargetAngleY) >= 0x4000)
                                                sba = mTargetAngleY;
                                        }
                                        mTargetAngleY = sba;
                                        mTurningToTarget = 1;
                                        t0 = (s16)(mAngleY - mTargetAngleY);
                                        if (turnInput & 0x200)
                                            t0 = (s16)(-t0);
                                        if (t0 > 0)
                                            turnInput = 0;
                                        mFlags |= 0x40000;
                                    }
                                }
                            }
                        }
                        Vector3 mid;
                        Vector3 midAbove;
                        if (turnInput != 0) {
                            if (turnInput & 0x4000)
                                func_02012790(0x1a);
                            mTurningToTarget = 0;
                        } else {
                            if (mTurningToTarget == 0) {
                                turnInput = data_0209f498[gActivePlayerSlot].held & 0x4300;
                            }
                        }
                        if (mFlags & 0x100) {
                            if (dist > (s32)ScaleDistByFov(0xdfe60))
                                mFlags &= ~0x100;
                        }
                        /* sb = dist is assigned inside the test: hoisting it above the if
                           re-lays the function (290 words differ). */
                        if (turnInput != 0 || mRecentering != 0 || mTurningToTarget != 0 ||
                            r7 <= nearDist || (sb = dist) < fp || sb <= farDist) {
                            sb = mAngleY;
                            mAngleY = Vec3_HorzAngle(&lookAt, &pos);
                            if (turnInput != 0 || mRecentering != 0) {
                                if ((turnInput & 0x4000) || mRecentering != 0) {
                                    if (turnInput != 0) {
                                        mBehindAngleY = mTargetPlayer->mAngleY + 0x8000;
                                    }
                                    mRecentering = ApproachAngle(&mAngleY, mBehindAngleY, 4, 0x800,
                                                                 0x400) != 0;
                                    if (mRecentering != 0)
                                        mFlags |= 0x60;
                                } else {
                                    if (turnInput & 0x200) {
                                        mAngleY -= 0x400;
                                        mFlags |= 0x20;
                                    } else {
                                        mAngleY += 0x400;
                                        mFlags |= 0x40;
                                    }
                                }
                                turnInput = 0;
                            } else if (mTurningToTarget != 0) {
                                if (ApproachAngle(&mAngleY, mTargetAngleY, 0x10, 0x200, 0x100) == 0)
                                    mTurningToTarget = 0;
                            } else {
                                m = mode;
                                if (m == &data_020871ac || m == &data_020871d4) {
                                    UpdateRadialCamera(view, r7);
                                } else if (framing == 0) {
                                    if (m == &data_0208724c)
                                        t0 = mTargetPlayer->mPrevAngleY;
                                    else
                                        t0 = mTargetPlayer->mAngleY;
                                    t0 = (s16)(t0 + 0x8000);
                                    if (m == &data_020874cc) {
                                        mAngleY = t0 + mBaseAngleOffset;
                                    } else if (dist <= farDist) {
                                        ApproachAngle(&mAngleY, t0, 0x14, 0x600, 0x28);
                                    } else if (m->flags & 0x100) {
                                        mAngleY = sb;
                                    } else if (r7 <= nearDist) {
                                        ApproachAngle(&mAngleY, t0, 0x14, 0x64, 0x28);
                                    }
                                }
                            }

                            prevDist = dist;
                            if (dist > r4) {
                                if (r7 != 0 && mode == &data_02086fcc) {
                                    sb = cstd::atan2(targetMove.y, Vec3_HorzLen(&targetMove));
                                    if (sb < -0x3f80)
                                        sb = -0x3f80;
                                    else if (sb > 0x3f80)
                                        sb = 0x3f80;
                                    fp = Vec3_HorzLen(&offset);
                                    pitch = cstd::atan2(-offset.y, fp);
                                    ApproachAngle(&pitchS, sb, 0x20, 0x800, 0);
                                    offset.y = FX_MUL(dist, -FX_SinIdx(pitch >> 4));
                                    t0 = FX_Div(FX_MUL(dist, FX_CosIdx(pitch >> 4)), fp);
                                    offset.x = FX_MUL(offset.x, t0);
                                    offset.z = FX_MUL(offset.z, t0);
                                }
                                Math_Function_0203b14c(&dist, r4, 0x400, r7 + 0x20000, 0x100);
                                Vec3_MulScalarInPlace(&offset, FX_Div(dist, prevDist));
                            } else if (dist < fp) {
                                Math_Function_0203b14c(&dist, fp, 0x400, r7 + 0x20000, 0x100);
                                {
                                    s64 distanceSq = dist;
                                    distanceSq *= dist;
                                    distanceSq -= (s64)offset.y * offset.y;
                                    r7 = cstd::sqrt(distanceSq);
                                }
                                t0 = FX_Div(r7, Vec3_HorzLen(&offset));
                                offset.x = FX_MUL(offset.x, t0);
                                offset.z = FX_MUL(offset.z, t0);
                                mInputYawLag -= mAngleY - sb;
                            }
                            t0 = Vec3_HorzLen(&offset);
                            if (t0 < 0x1000)
                                t0 = 0x1000;
                            offset.z = t0;
                            offset.x = 0;
                            Vec3_RotateYAndTranslate(&pos, &lookAt, mAngleY, &offset);
                        } else {
                            if (sb > r4) {
                                Math_Function_0203b14c(&dist, r4, 0x400, r7 + 0x20000, 0x100);
                                Vec3_MulScalarInPlace(&offset, FX_Div(dist, sb));
                                MoveCameraPosition(this, offset);
                            }
                        }
                        if (KeepPosOutOfWalls() != 0) {
                            if (mFlags & 0x60) {
                                turnInput = 1;
                                mFlags &= ~0x60;
                            }
                        }
                        SetVec(probe, pos.x, pos.y + 0x100000, pos.z);
                        line.SetObjAndLine(pos, probe, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = LimitCameraCeiling(line, probe, ceil);
                        }
                        Vec3_Add(&mid, &pos, &lookAt);
                        Vec3_AsrInPlace(&mid, 1);
                        midAbove.x = mid.x;
                        midAbove.y = mid.y + 0x100000;
                        midAbove.z = mid.z;
                        line.SetObjAndLine(mid, midAbove, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = LimitMidpointCeiling(line, midAbove, ceil);
                        }
                        s32 y = mTargetPos.y;
                        dBgCh_Gnd ground;
                        ground.mProbeHeight = 0x5dc000;
                        PrepareBgCh(ground);
                        ground.SetObjAndPos(probe, 0);
                        if (mode > &data_02086fcc)
                            ground.StartDetectingWater();
                        sb = (ground.DetectClsn() != 0) ? ground.clsnY : y;
                        if (mode == &data_02086ff4) {
                            if (sb < data_0209f32c)
                                sb = data_0209f32c;
                            goalY = sb + 0x64000;
                        } else {
                            goalY = sb;
                            if (sb < r6)
                                goalY = r6;
                            goalY = goalY + ScaleHeightByFov((r6 - lookAtGroundY) +
                                                             ((mFlags & 2) ? 0x1e8000 : 0x138000));
                            if (mode->flags & 0x80) {
                                if (pos.y > (s32)goalY)
                                    goalY = pos.y;
                            }
                            minRise = mode->minRiseSlope;
                            maxRise = mode->maxRiseSlope;
                            func_0200c9e0(&minRise, &maxRise);
                            offset.y = goalY - lookAt.y;
                            if (mode == &data_0208706c && targetMove.y < 0)
                                t0 = 0xa49;
                            else
                                t0 = minRise;
                            r6 = FX_MUL(dist, t0);
                            t0 = FX_MUL(dist, maxRise);
                            if (offset.y < r6)
                                goalY = lookAt.y + r6;
                            else if (offset.y > t0)
                                goalY = lookAt.y + t0;
                            if (gCurrentSublevel == 0x26) {
                                if (pos.y < sb + 0x25bc8)
                                    pos.y = sb + 0x25bc8;
                            }
                            if (ceil < sb + 0x25bc8)
                                ceil = probe.y;
                            if (sb + 0x25bc8 > (s32)0x80000000)
                                minY = sb + 0x25bc8;
                        }
                        if (mode != &data_02087094 && mode != &data_020870e4 &&
                            mode != &data_0208710c) {
                            if ((s32)goalY < minY)
                                goalY = minY;
                            else if ((s32)goalY > ceil)
                                goalY = ceil;
                        }
                        Math_Function_0203b14c(&pos.y, goalY, 0x300, 0x14000, 0x100);
                        mAngleY = Vec3_HorzAngle(&lookAt, &pos);
                    }
                }
            }
        }
        if (turnInput != 0) {
            if (sPrevTurnInput == 0)
                func_02012790(0xe);
            sPrevTurnInput = turnInput;
        } else {
            sPrevTurnInput = 0;
            if (mFlags & 0x60) {
                mTurnSound = Sound_PlayIfNotActive(mTurnSound, 2, 0x43, 0);
            }
        }
    }
    mAngleX = Vec3_VertAngle(&lookAt, &pos);
    return 1;
}

/* The states' entry ops: each switches to mode record N of the table at
   0x02086fcc. */
int dCamera_c::func_02009e50() {
    SetMode(1);
    return 1;
}

int func_02009e30(dCamera_c *cam) {
    int dummy = 0;
    &dummy;
    cam->SetMode(0);
    return 1;
}

int func_02009e10(dCamera_c *cam) {
    cam->SetMode(6);
    return 1;
}

int func_02009df0(dCamera_c *cam) {
    cam->SetMode(2);
    return 1;
}

int func_02009dd0(dCamera_c *cam) {
    cam->SetMode(3);
    return 1;
}

int func_02009db0(dCamera_c *cam) {
    cam->SetMode(4);
    return 1;
}

int func_02009d90(dCamera_c *cam) {
    volatile int dummy __attribute__((unused));
    cam->SetMode(7);
    return 1;
}

int func_02009d70(dCamera_c *cam) {
    cam->SetMode(0x10);
    return 1;
}

/* Entry to the behind-the-player state: mode data_0208729c, turning to the
   yaw behind the target player. */
int dCamera_c::func_02009d30() {
    int result = 1;

    mMode = &data_0208729c;

    mTurningToTarget = result;

    {
        dActor_c *player = mTargetPlayer;
        short yaw = player->mAngleY;
        mTargetAngleY = (yaw + 0x8000);
        mFlags &= ~0x100u;
    }

    return result;
}

/* The behind-the-player state: while mTurningToTarget is set, carry lookAt
   and pos along with the player and turn to data_02086e90 behind it;
   afterwards follow its facing, with the stylus axes adding up to 0x2000 of
   yaw (the rest turns the player) and 0x1c00 of pitch. */
int func_02009aa8(dCamera_c *cam) {
    s32 orbitOffset[3];
    Vector3 delta;
    Vector3 src;

    src.x = data_02086e90[0];
    src.y = data_02086e90[1];
    src.z = data_02086e90[2];

    Vec3_RotateYAndTranslate(orbitOffset, &cam->mTargetPlayer->mPos, cam->mTargetPlayer->mAngleY,
                             &src);

    if (cam->mTurningToTarget != 0) {
        Vector3 *playerPos;
        int arrived;

        Vec3_Sub(&delta, &cam->mTargetPlayer->mPos, &cam->mTargetPos);
        VEC_Add(&cam->lookAt, &delta, &cam->lookAt);
        VEC_Add(&cam->pos, &delta, &cam->pos);

        playerPos = &cam->mTargetPlayer->mPos;
        cam->mTargetPos.x = playerPos->x;
        cam->mTargetPos.y = playerPos->y;
        cam->mTargetPos.z = playerPos->z;

        arrived = cam->ApproachLookAtAndAngles((Vector3 *)orbitOffset, cam->mTargetAngleY, 0);
        if (arrived != 0)
            cam->mTurningToTarget = 0;

        cam->mLookAtOffset.x = 0;
        cam->mLookAtOffset.z = 0;
    } else {
        s16 newAngle;
        s16 yawInput, pitchInput;
        s16 excess;
        int yawDelta;
        int clamped;
        s16 pitch;

        newAngle = cam->mTargetPlayer->mAngleY + 0x8000;
        cam->mAngleY += newAngle - cam->mTargetAngleY;
        cam->mTargetAngleY = newAngle;

        cam->lookAt.x = orbitOffset[0];
        cam->lookAt.y = orbitOffset[1];
        cam->lookAt.z = orbitOffset[2];

        yawInput = data_0209f498[gActivePlayerSlot].nx;
        cam->mAngleY -= FX_MUL(yawInput, 0x200);

        yawDelta = (s16)(cam->mAngleY - newAngle);
        if (yawDelta < -0x2000) {
            clamped = -0x2000;
        } else {
            if (yawDelta > 0x2000) {
                clamped = 0x2000;
            } else {
                clamped = yawDelta;
            }
        }
        excess = yawDelta - clamped;

        cam->mTargetPlayer->mAngleY += excess;
        cam->mTargetAngleY += excess;

        pitchInput = data_0209f498[gActivePlayerSlot].ny;
        cam->mAngleX += FX_MUL(pitchInput, 0x200);

        pitch = cam->mAngleX;
        if (pitch < -0x1c00) {
            pitch = -0x1c00;
        } else if (pitch > 0x1c00) {
            pitch = 0x1c00;
        }
        cam->mAngleX = pitch;
    }

    cam->ApproachDist(cam->ScaleDistByFov(0x48000));
    cam->mFlags |= 0x80000;
    cam->ClampPosAboveTarget();

    return 1;
}

int dCamera_c::func_02009a8c() {
    SaveCameraStateBeforeTalk();
    return func_02009d30();
}

/* The look-around state: turn to data_02086e9c first, then let the stylus
   axes swing mAngleY up to 0x5000 off mTargetAngleY and the pitch up to
   0x1c00. */
int dCamera_c::func_020098e0() {
    if (mTurningToTarget != 0) {
        int goal[3];
        int src[3];
        int result;
        src[0] = data_02086e9c[0];
        src[1] = data_02086e9c[1];
        src[2] = data_02086e9c[2];

        {
            dActor_c *player = mTargetPlayer;
            Vec3_RotateYAndTranslate(goal, &player->mPos, player->mAngleY, src);
        }

        result = ApproachLookAtAndAngles((Vector3 *)goal, mTargetAngleY, 0);
        if (result != 0) {
            mTurningToTarget = 0;
        }
    } else {
        s16 t1 = data_0209f498[gActivePlayerSlot].nx;
        int scaled1 = FX_MUL(t1, 0x200);

        mAngleY -= scaled1;

        {
            s16 targetYaw = mTargetAngleY;
            s16 diff = (mAngleY - targetYaw);
            if (diff < -0x5000)
                diff = -0x5000;
            else if (diff > 0x5000)
                diff = 0x5000;
            mAngleY = (targetYaw + diff);
        }

        {
            s16 t2 = data_0209f498[gActivePlayerSlot].ny;
            int scaled2 = FX_MUL(t2, 0x200);
            mAngleX += scaled2;
        }

        {
            s16 clamped = mAngleX;
            if (clamped < -0x1c00)
                clamped = -0x1c00;
            else if (clamped > 0x1c00)
                clamped = 0x1c00;
            mAngleX = clamped;
        }
    }

    {
        unsigned int dist = ScaleDistByFov(0x48000);
        ApproachDist(dist);
    }
    ClampPosAboveTarget();
    return 1;
}

/* Entry to state 0x13: arm mStateTimer, which func_0200987c counts down. */
int func_020098b8(dCamera_c *cam) {
    cam->SetMode(0x13);
    cam->mStateTimer = 0x1c;
    return 1;
}

/* State 0x13: follow until mStateTimer runs out, then switch to state
   0x14. */
void func_0200987c(dCamera_c *cam) {
    if (DecIfAbove0_Short(&cam->mStateTimer) == 0) {
        cam->SetMode(0x14);
    }
    cam->func_02009e70();
}

int func_0200985c(dCamera_c *cam) {
    cam->SetMode(0x17);
    return 1;
}

/* Entry to the first-person state: pos at the player, facing its way. */
int func_020097ec(dCamera_c *cam) {
    Vector3 *src;

    cam->SetMode(0x18);
    src = &cam->mTargetPlayer->mPos;
    cam->pos.x = src->x;
    cam->pos.y = src->y;
    cam->pos.z = src->z;
    cam->mBehindAngleY = cam->mTargetPlayer->mAngleY + 0x8000;
    cam->mAngleY = cam->mBehindAngleY;
    cam->mTargetAngleY = 0;
    cam->mAngleX = cam->mTargetPlayer->mAngleX;
    cam->mInputScale = 0;
    return 1;
}

/* The first-person state: the camera-input axes turn mTargetAngleY (held
   within 0x4000 of mBehindAngleY) and the pitch, with the turning sound, and
   lookAt is rebuilt 0x280000 in front of pos. The player faces away from the
   camera. */
int func_020095e4(dCamera_c *cam) {
    Vector3 v;
    int step;
    short prevTargetYaw, prevPitch;

    FUN_02029ab0();
    func_02008b08(cam);

    if (data_0209f498[gActivePlayerSlot].mag != 0) {
        step = (data_0209f498[gActivePlayerSlot].mode == 2) ? 0x240 : 0x200;
        prevTargetYaw = cam->mTargetAngleY;
        {
            int a = func_02008abc(cam);
            cam->mTargetAngleY -= FX_MUL(step, a);
        }
        {
            int angle = cam->mTargetAngleY;
            if (angle < -0x4000)
                angle = -0x4000;
            else if (angle > 0x4000)
                angle = 0x4000;
            cam->mTargetAngleY = angle;
        }
        cam->mAngleY = cam->mBehindAngleY + cam->mTargetAngleY;
        prevPitch = cam->mAngleX;
        cam->mAngleX += FX_MUL(step, func_02008a70(cam));
        {
            int angle = cam->mAngleX;
            if (angle < 0)
                angle = 0;
            else if (angle > 0x38e3)
                angle = 0x38e3;
            cam->mAngleX = angle;
        }
        if (cam->mTargetAngleY != prevTargetYaw || cam->mAngleX != prevPitch) {
            cam->mTurnSound = Sound_PlayIfNotActive(cam->mTurnSound, 3, 0x181, 0);
        }
    }

    v.x = 0;
    v.y = 0;
    v.z = -0x280000;
    Matrix4x3_FromTranslation(&data_020a0e68, cam->pos.x, cam->pos.y, cam->pos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, cam->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, cam->mAngleX);
    MTX_MultVec43(&v, &data_020a0e68, &cam->lookAt);
    cam->mTargetPlayer->mAngleY = cam->mAngleY - 0x8000;
    return 1;
}

int func_020095c4(dCamera_c *cam) {
    cam->SetMode(0x1d);
    return 1;
}

/* The exit-viewing state: turn toward mLookTargetPos, carry it toward
   data_02086ea8 turned by mTargetAngleY around mLookTarget, and orbit at
   0x300000. */
int func_02009540(dCamera_c *cam) {
    cam->ApproachLookAtAndAngles(&cam->mLookTargetPos, cam->mTargetAngleY, 0);

    /* stack layout: 12-byte temporary vector, then three ints */
    Vector3 goal;
    int offset[3];

    offset[0] = data_02086ea8[0];
    offset[1] = data_02086ea8[1];
    offset[2] = data_02086ea8[2];

    Vec3_RotateYAndTranslate(&goal, cam->mLookTarget, cam->mTargetAngleY, offset);

    ApproachLinear(cam->mLookTargetPos, goal, 0x8000);

    cam->ApproachDist(0x300000);

    return 1;
}

int func_020094e4(dCamera_c *cam) {
    int buf[3];
    cam->SetMode(0x1f);
    buf[0] = data_02086eb4[0];
    buf[1] = data_02086eb4[1];
    buf[2] = data_02086eb4[2];
    short angle = cam->mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&cam->pos, &cam->mTargetPos, angle, buf);
    return 1;
}

/* Frame the target player as the focus actor for this frame (Behavior
   clears it again) and follow. */
void func_020094c0(dCamera_c *cam) {
    cam->mFocusActor = cam->mTargetPlayer;
    cam->mFlags |= 4;
    cam->func_02009e70();
}

int func_020094a8(dCamera_c *cam) {
    cam->mPathIndex = 0;
    cam->mPathProgress = 0;
    return 1;
}

/* Walk pos along mPosPath and lookAt along mLookAtPath (MovePointAlongSpline,
   sharing one segment index and progress), then re-aim. Returns 0 once
   either path has run out. */
int func_02009414(dCamera_c *cam) {
    int ok = 1;
    CutsceneSpline *posPath = cam->mPosPath;

    if (posPath != 0) {
        ok = MovePointAlongSpline(&cam->pos, &cam->mPathIndex, &cam->mPathProgress, posPath);
    }

    CutsceneSpline *lookAtPath = cam->mLookAtPath;
    if (lookAtPath != 0) {
        if (MovePointAlongSpline(&cam->lookAt, &cam->mPathIndex, &cam->mPathProgress, lookAtPath) ==
            0) {
            ok = 0;
        }
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    cam->mAngleX = Vec3_VertAngle(&cam->lookAt, &cam->pos);

    cam->UpdateTargetPos();
    return ok;
}

/* Scale a horizontal distance by the field of view (mInvSinFov). */
unsigned int dCamera_c::ScaleDistByFov(int x) {
    long long prod = x * (long long)mInvSinFov;
    long long sum = prod + 0x800LL;
    unsigned int lo = sum;
    unsigned int hi = (sum >> 32);
    return (lo >> 12) | (hi << 20);
}

/* Scale a height by the field of view (mInvCosFov). */
unsigned int dCamera_c::ScaleHeightByFov(int a) {
    unsigned long long prod = a * (unsigned long long)mInvCosFov;
    unsigned long long sum = prod + 0x800u;
    unsigned int lo = sum;
    unsigned int hi = (sum >> 32);
    return (lo >> 12) | (hi << 20);
}

/* Is any of the twelve star markers within 0x8000 of the target? */
int dCamera_c::IsTargetNearStarMarker() {
    int i;
    for (i = 0; i < 12; i++) {
        dActor_c *marker = STAR_MARKERS[i];
        if (marker != 0) {
            if (func_0203cf94(&mTargetPos.x, &marker->mPos.x) < 0x40000000LL) {
                return 1;
            }
        }
    }
    return 0;
}

/* Ease out toward target: shrink the distance from target to out with
   Math_Function_0203b14c and rebuild out along the same line. Returns 1 on
   arrival. */
int func_020092c4(void *cam, Vector3 *out, const Vector3 *target) {
    struct Vector3 offset;
    int len;
    int arrived;
    int remaining;
    Vec3_Sub(&offset, out, target);
    len = VEC_Mag(&offset);
    if (len == 0)
        return 1;
    remaining = len;
    arrived = Math_Function_0203b14c(&remaining, 0, 0x200, 0x40000, 0x8000);
    Vec3_MulScalarInPlace(&offset, FX_Div(remaining, len));
    {
        struct Vector3 res;
        Vec3_Add(&res, target, &offset);
        out->x = res.x;
        out->y = res.y;
        out->z = res.z;
    }
    return arrived == 0 ? 1 : 0;
}

/* Recompute mAngleY and mAngleX from the lookAt -> pos line. */
void dCamera_c::UpdateAngles() {
    short h = Vec3_HorzAngle(&lookAt, &pos);
    mAngleY = h;

    short v = Vec3_VertAngle(&lookAt, &pos);
    mAngleX = v;
}

/* Ease lookAt toward the given point and the two angles toward the given
   yaw and pitch. Returns 1 once all three have arrived. */
int dCamera_c::ApproachLookAtAndAngles(Vector3 *target, int angleY, int angleX) {
    int moving;
    int angleMoving;
    moving = (func_020092c4(this, &lookAt, target) != 1);
    UpdateAngles();
    angleMoving = ApproachAngle(&mAngleY, angleY, 8, 0x4000, 0x80);
    if (angleMoving != 0)
        moving = 1;
    angleMoving = ApproachAngle(&mAngleX, angleX, 8, 0x4000, 0x80);
    return (moving | angleMoving) == 0;
}

/* Ease the lookAt-to-pos distance toward dist and rebuild pos from lookAt
   and the two angles. Returns 1 on arrival. */
int dCamera_c::ApproachDist(int dist) {
    struct Vector3 v;
    int len;
    int r;
    Vec3_Sub(&v, &pos, &lookAt);
    len = VEC_Mag(&v);
    r = Math_Function_0203b14c(&len, dist, 0x300, 0x60000, 0x200);
    v.x = 0;
    v.y = 0;
    v.z = len;
    Matrix4x3_FromTranslation(&data_020a0e68, lookAt.x, lookAt.y, lookAt.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    MTX_MultVec43(&v, &data_020a0e68, &pos);
    return (r == 0) ? 1 : 0;
}

/* Keep pos out of walls: probe from above the target (flags 0x40200) and
   from the previous pos, and pull pos in to the hit. A hit facing the same
   way as mWallNormal, or within 0x80000 of mWallPos, is ignored. */
int dCamera_c::KeepPosOutOfWalls() {
    Vector3 v0;
    Vector3 dir1;
    Vector3 end1;
    Vector3 n1;
    Vector3 dir2;
    Vector3 end2;
    Vector3 n2;
    Vector3 diff;
    int vy;
    int vz;
    int vx;
    int scale;
    int len;
    int d;

    {
        dActor_c *p = mTargetPlayer;
        int b = p->actorID;
        b = b == 0xbf;
        if (b != false) {
            if (((daPly_c *)p)->func_ov002_020ca270() != 0)
                return 0;
        }
    }

    scale = 0x200;
    if (mFlags & 0x40200) {
        vz = mTargetPos.z;
        vy = mTargetPos.y + 0x82500;
        vx = mTargetPos.x;
        v0.x = vx;
        v0.y = vy;
        v0.z = vz;
        Vec3_Sub(&dir1, &pos, &v0);
        len = VEC_Mag(&dir1);
        if (len != 0) {
            Vec3_MulScalarInPlace(&dir1, FX_Div(0x100000, len));
            Vec3_Add(&end1, &pos, &dir1);
            dBgCh_Lin rc1;
            PrepareBgCh(rc1);
            rc1.SetObjAndLine(end1, v0, 0);
            if (rc1.DetectClsn() != 0) {
                rc1.surface.CopyNormalTo(n1);
                if (VEC_DotProduct(&mWallNormal, &n1) > 0) {
                    mFlags &= ~0x80000;
                    return 0;
                }
                Vector3 cp1 = rc1.GetClsnPos();
                if (Vec3_Dist(&mWallPos, &cp1) < 0x80000) {
                    mFlags &= ~0x80000;
                    return 0;
                }
                {
                    u32 f = mFlags;
                    if ((f & 0x40000) && mTurningToTarget != 0 && !(f & 0x80100)) {
                        int a = cstd::atan2(n1.x, n1.z);
                        if (AngleDiff(a, cstd::atan2(mWallNormal.x, mWallNormal.z)) > 0x6000) {
                            mTargetAngleY += 0x8000;
                            mFlags |= 0x80000;
                        }
                    }
                }
            }
        }
        scale = 0x1000;
    }

    {
        u32 f = mFlags;
        if (f & 0x80100) {
            vz = mTargetPos.z;
            vy = (f & 0x100) ? pos.y : 0x82500 + mTargetPos.y;
            vx = mTargetPos.x;
            v0.x = vx;
            v0.y = vy;
            v0.z = vz;
        } else {
            scale = 0x1000;
            v0.x = mPrevPos.x;
            v0.y = mPrevPos.y;
            v0.z = mPrevPos.z;
        }
    }

    Vec3_Sub(&dir2, &pos, &v0);
    len = VEC_Mag(&dir2);
    if (len == 0) {
        mFlags &= ~0x80000;
        return 0;
    }
    Vec3_MulScalarInPlace(&dir2, FX_Div(0x60000, len));
    Vec3_Add(&end2, &pos, &dir2);
    dBgCh_Lin rc2;
    PrepareBgCh(rc2);
    rc2.SetObjAndLine(v0, end2, 0);
    if (rc2.DetectClsn() == 0) {
        mFlags &= ~0x80000;
        return 0;
    }
    if (!(mFlags & 0x80100)) {
        int t;
        rc2.surface.CopyNormalTo(n2);
        t = n2.y;
        if (t < 0)
            t = -t;
        if (t > 0xb50) {
            return 0;
        }
    }
    d = Vec3_Dist(&pos, &lookAt);
    Vec3_MulScalarInPlace(&dir2, scale);
    Vector3 cp2 = rc2.GetClsnPos();
    Vector3 np;
    Vec3_Sub(&np, &cp2, &dir2);
    pos.x = np.x;
    pos.y = np.y;
    pos.z = np.z;
    Vec3_Sub(&diff, &pos, &lookAt);
    if (VEC_Mag(&diff) >= d) {
        Vec3_MulScalarInPlace(&diff, FX_Div(d, VEC_Mag(&diff)));
        Vector3 cl;
        Vec3_Add(&cl, &lookAt, &diff);
        pos.x = cl.x;
        pos.y = cl.y;
        pos.z = cl.z;
    }
    if (mTurningToTarget != 0 || mRecentering != 0) {
        mTurningToTarget = 0;
        mRecentering = 0;
    }
    mFlags &= ~0x80000;
    return 1;
}

/* SM64's move_point_along_spline, in fixed point (CutsceneSpline.h). */
int MovePointAlongSpline(Vector3 *pos, s16 *segment, s32 *progress, CutsceneSpline *spline) {
    short *seg = segment;
    int *progPtr = progress;
    int idx;
    CutsceneSplinePoint *elem;
    CutsceneSpline *path = spline;
    int i;
    Vector3 v[4];
    int lo, hi;
    int prog;

    idx = *seg;
    if (idx + 3 >= path->numPoints) {
        return 1;
    }

    elem = path->points + idx;

    if (elem[0].index < 0 || elem[1].index < 0 || elem[2].index < 0) {
        return 1;
    }

    for (i = 0; i < 4; i++) {
        int x = elem[i].x << 12;
        int z = elem[i].z << 12;
        int y = elem[i].y << 12;
        v[i].x = x;
        v[i].y = y;
        v[i].z = z;
    }

    func_ov002_020effb8(pos, *progPtr, &v[0], &v[1], &v[2], &v[3]);

    lo = 0;
    hi = 0;
    if (elem[1].speed != 0) {
        lo = FX_Inv(elem[1].speed << 12);
    }
    if (elem[2].speed != 0) {
        hi = FX_Inv(elem[2].speed << 12);
    }

    {
        int speedGap = hi - lo;
        prog = *progPtr;
        *progPtr = lo + FX_MUL(speedGap, prog) + prog;
    }

    if (*progPtr >= 0x1000) {
        *seg = *seg + 1;
        *progPtr -= 0x1000;
    }

    return 0;
}

/* Ease mInputScale toward the active player's camera-input entry: slowly
   while it is nonzero, quickly back to 0. */
int func_02008b08(dCamera_c *cam) {
    s16 dest = data_0209f498[gActivePlayerSlot].mag;
    return ApproachLinear2(cam->mInputScale, dest, dest != 0 ? 0xa0 : 0x400);
}

/* The camera's horizontal input axis, scaled by mInputScale. */
short func_02008abc(dCamera_c *cam) {
    short axis = data_0209f498[gActivePlayerSlot].nx;
    short scale = cam->mInputScale;
    return FX_MUL(axis, scale);
}

/* The camera's vertical input axis, scaled by mInputScale. */
short func_02008a70(dCamera_c *cam) {
    short axis = data_0209f498[gActivePlayerSlot].ny;
    short scale = cam->mInputScale;
    return FX_MUL(axis, scale);
}

/* Copy the target player's position into mTargetPos and keep lookAt's
   offset from it in mLookAtOffset. */
void dCamera_c::UpdateTargetPos() {
    dActor_c *player;
    Vector3 *playerPos;
    int offset[3];

    player = mTargetPlayer;
    if (player == 0) {
        return;
    }
    playerPos = &player->mPos;
    mTargetPos.x = playerPos->x;
    mTargetPos.y = playerPos->y;
    mTargetPos.z = playerPos->z;
    Vec3_Sub((Vector3 *)offset, &lookAt, &mTargetPos);
    mLookAtOffset.x = offset[0];
    mLookAtOffset.y = offset[1];
    mLookAtOffset.z = offset[2];
    mFocusBlend = 0;
}

/* Keep pos at least 0x25bc8 above the target. */
void dCamera_c::ClampPosAboveTarget() {
    int limit = mTargetPos.y + 0x25bc8;
    if (pos.y < limit) {
        pos.y = limit;
    }
}

/* Prepare a collision query for the camera. It always stops ignoring
 * vanish surfaces, then starts again if the target is a vanished daPly_c,
 * and finally enables camera mode on the query.
 *
 * Codegen note: the (actorID == 0xbf) test is held in an enum-typed boolean.
 * Under -O4 -enum int this materializes the comparison as a full 32-bit 0/1
 * value (cmp; moveq #1; movne #0; cmp #0; beq) instead of folding it into a
 * single branch -- that is what the ROM does. A plain `int` bool folds to a
 * bare `bne`; a `u8` bool masks with `ands #0xff`; only the (int-width) enum
 * bool gives the exact `cmp r0, #0`. */
void dCamera_c::PrepareBgCh(dBgCh &bgCh) {
    daPly_c *owner;
    enum Bool isType;
    bgCh.StopIgnoringVanishSurfaces();
    owner = (daPly_c *)mTargetPlayer;
    isType = (enum Bool)(owner->actorID == 0xbf);
    if (isType && owner->mIsVanish != 0) {
        bgCh.StartIgnoringVanishSurfaces();
    }
    bgCh.EnableCameraMode();
}

/* The script ops, called through RunCameraCommand's member-function table
   with the command's operands. The class is opaque: only the pointer-to-
   member representation matters. */
typedef int (CamPmfHost::*CamScriptOp)(unsigned char *, int, int);

extern CamScriptOp data_02086c80, data_02086d80, data_02086c60, data_02086c90, data_02086bd8,
    data_02086d18, data_02086d98, data_02086c88, data_02086d00, data_02086bc0, data_02086e30,
    data_02086ce8, data_02086e38, data_02086e20, data_02086cd0, data_02086e08, data_02086df0,
    data_02086cb8, data_02086dd8, data_02086dc0, data_02086c20, data_02086da0, data_02086d60,
    data_02086d88, data_02086cf8, data_02086d68, data_02086c78, data_02086d40, data_02086d38,
    data_02086d20, data_02086c50, data_02086d08, data_02086cf0, data_02086c38, data_02086cd8,
    data_02086cc0, data_02086cb0, data_02086ca8, data_02086ca0;

/* Run one camera script command: byte 6 picks the op, and the operands
   follow from byte 7. The table is built on first use. */
int RunCameraCommand(dCamera_c *cam, u8 *command, int first, int last) {
    static CamScriptOp sScriptOps[39] = {
        data_02086c80, data_02086d80, data_02086c60, data_02086c90, data_02086bd8, data_02086d18,
        data_02086d98, data_02086c88, data_02086d00, data_02086bc0, data_02086e30, data_02086ce8,
        data_02086e38, data_02086e20, data_02086cd0, data_02086e08, data_02086df0, data_02086cb8,
        data_02086dd8, data_02086dc0, data_02086c20, data_02086da0, data_02086d60, data_02086d88,
        data_02086cf8, data_02086d68, data_02086c78, data_02086d40, data_02086d38, data_02086d20,
        data_02086c50, data_02086d08, data_02086cf0, data_02086c38, data_02086cd8, data_02086cc0,
        data_02086cb0, data_02086ca8, data_02086ca0};
    return (((CamPmfHost *)cam)->*sScriptOps[command[6]])(command + 7, first, last);
}

/* Script op: set lookAt from three whole-unit shorts. */
int func_02008500(dCamera_c *cam, u8 *data) {
    fx32 x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    cam->lookAt.x = x;
    cam->lookAt.y = y;
    cam->lookAt.z = z;
    return 1;
}

/* Script op: set pos from three whole-unit shorts. */
s32 func_020084b0(dCamera_c *cam, u8 *data, int a, int b) {
    s32 y, z, x;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    cam->pos.x = x;
    cam->pos.y = y;
    cam->pos.z = z;
    return 1;
}

/* Script op: set lookAt, then pos. */
int func_02008474(dCamera_c *cam, u8 *data, int a, int b) {
    func_02008500(cam, data);
    return func_020084b0(cam, data + 6, a, b);
}

/* Script op: set mFovAngle. */
int func_02008450(dCamera_c *cam, u8 *data) {
    cam->mFovAngle = ReadUnalignedUshort(data);
    return 1;
}

/* Script op: step mFovAngle toward a target by a rate. */
int func_0200840c(dCamera_c *cam, u8 *data) {
    short v1 = ReadUnalignedShort(data);
    short v2 = ReadUnalignedShort(data + 2);
    ApproachLinear2(cam->mFovAngle, v1, v2);
    return 1;
}

/* Script op: grow the viewport from the given rectangle to the full screen
   over frames lo..hi, keeping mAspect in step. */
int func_020082f8(dCamera_c *cam, u8 *src, int lo, int hi) {
    fx32 slope = FX_Div((gKuppaScriptFrame - lo) << 12, (hi - lo) << 12);
    cam->mViewport[0] = (FX_MUL((src[0] << 12), slope) >> 12);
    cam->mViewport[1] = (FX_MUL((src[1] << 12), slope) >> 12);
    cam->mViewport[2] = ((FX_MUL(((src[2] - 0xff) << 12), slope) >> 12) + 0xff);
    cam->mViewport[3] = ((FX_MUL(((src[3] - 0xbf) << 12), slope) >> 12) + 0xbf);
    cam->mAspect = FX_Div(((cam->mViewport[2] - cam->mViewport[0]) + 1) << 12,
                          ((cam->mViewport[3] - cam->mViewport[1]) + 1) << 12);
    return 1;
}

/* Script op: on its first frame, reset the path walk and take the lookAt
   and pos paths; every frame, walk them. */
int func_020082a0(dCamera_c *cam, u8 *src, int id) {
    if (id == gKuppaScriptFrame) {
        func_020094a8(cam);
        cam->mLookAtPath = (CutsceneSpline *)ReadUnalignedInt(src);
        cam->mPosPath = (CutsceneSpline *)ReadUnalignedInt(src + 4);
    }
    func_02009414(cam);
    return 1;
}

/* Script op: from the first frame, ease savedLookAt.y toward a height
   between the script player's ground and its position, and keep lookAt.y
   at least that high. */
int func_02008200(dCamera_c *cam, u8 *data, int first) {
    daPly_c *a;
    int diff, val, target;
    if (first == gKuppaScriptFrame)
        cam->savedLookAt.y = 0x190000;
    a = (daPly_c *)KuppaScript::FindScriptPlayer(0);
    diff = a->mPosY - a->mGroundY;
    val = FX_MUL(diff, 0x1199);
    target = a->mGroundY + 0x50000 + val;
    Math_Function_0203b0fc(&cam->savedLookAt.y, target, 0xa3, 0x7fffffff);
    if (cam->lookAt.y < cam->savedLookAt.y)
        cam->lookAt.y = cam->savedLookAt.y;
    return 1;
}

/* Script op: aim lookAt ahead of the script player, nearer the further it
   is below pos. */
int func_0200814c(dCamera_c *cam) {
    dActor_c *a;
    int diff, absdiff, val;
    Vector3 v;

    a = KuppaScript::FindScriptPlayer(0);
    diff = a->mPos.y - cam->pos.y;
    v.x = 0;
    v.y = 0x50000;
    v.z = diff;
    absdiff = diff < 0 ? -diff : diff;
    val = FX_MUL(absdiff, -0x199);
    v.z = val;
    if (val > -0x64000)
        v.z = -0x64000;
    if (diff > -0x300000)
        Vec3_RotateYAndTranslate(&cam->lookAt, &a->mPos, a->mAngleY, &v);
    return 1;
}

/* Script op: ease pos.y and lookAt.y toward heights above demo actor 0x13. */
int func_020080f0(dCamera_c *cam) {
    dActor_c *actor = KuppaScript::FindDemoActor(0x13);
    Vector3 *pos = &actor->mPos;
    int v1 = pos->y;
    Math_Function_0203b0fc(&cam->pos.y, v1 + 0x23000, 0x51, 0x7fffffff);
    int v2 = pos->y;
    Math_Function_0203b0fc(&cam->lookAt.y, v2 + 0x7d000, 0x266, 0x7fffffff);
    return 1;
}

/* Script op: lookAt at demo actor 0x13, mScriptParam above it. */
int func_020080b0(dCamera_c *cam) {
    dActor_c *obj = KuppaScript::FindDemoActor(0x13);
    Vector3 *base = &obj->mPos;

    int z = base->z;
    int y = base->y + cam->mScriptParam;
    int x = base->x;

    cam->lookAt.x = x;
    cam->lookAt.y = y;
    cam->lookAt.z = z;
    return 1;
}

/* Script op: raise pos by demo actor 0x13's height. */
int func_02008080(dCamera_c *cam) {
    dActor_c *a;
    int *dst;

    a = KuppaScript::FindDemoActor(0x13);
    dst = &cam->pos.y;
    *dst = *dst + a->mPosY;
    return 1;
}

/* Script op: walk the paths, then ease pos.y toward demo actor 0x13's
   height above where the walk left it. */
int func_02008028(dCamera_c *cam) {
    fx32 y;
    dActor_c *obj;

    y = cam->pos.y;
    func_02009414(cam);
    obj = KuppaScript::FindDemoActor(0x13);
    Math_Function_0203b0fc(&y, cam->pos.y + obj->mPosY, 0x11e, 0x7fffffff);
    cam->pos.y = y;
    return 1;
}

/* Script op: set mScriptParam. */
int func_02008008(dCamera_c *cam, u8 *data) {
    cam->mScriptParam = ReadUnalignedSignedInt(data);
    return 1;
}

/* Script op: step mScriptParam toward a target by a rate. */
int func_02007fcc(dCamera_c *cam, u8 *data) {
    int v1 = ReadUnalignedSignedInt(data);
    int v2 = ReadUnalignedSignedInt(data + 4);
    ApproachLinear(cam->mScriptParam, v1, v2);
    return 1;
}

/* Script op: step each lookAt axis toward a whole-unit target by its own
   rate. */
int func_02007f14(dCamera_c *cam, u8 *data) {
    int pos[3];
    int rot[3];
    int *posPtr, *rotPtr;
    fx32 px, py, pz;
    int rx, ry, rz;
    pz = ReadUnalignedShort(data + 4) << 12;
    py = ReadUnalignedShort(data + 2) << 12;
    px = ReadUnalignedShort(data) << 12;
    pos[0] = px;
    pos[1] = py;
    pos[2] = pz;
    rz = ReadUnalignedShort(data + 0xa);
    ry = ReadUnalignedShort(data + 8);
    rx = ReadUnalignedShort(data + 6);
    rot[0] = rx;
    rot[1] = ry;
    rot[2] = rz;
    posPtr = pos;
    rotPtr = rot;
    Math_Function_0203b0fc(&cam->lookAt.x, posPtr[0], rotPtr[0], 0x7fffffff);
    posPtr++;
    rotPtr++;
    Math_Function_0203b0fc(&cam->lookAt.y, posPtr[0], rotPtr[0], 0x7fffffff);
    posPtr++;
    rotPtr++;
    Math_Function_0203b0fc(&cam->lookAt.z, posPtr[0], rotPtr[0], 0x7fffffff);
    return 1;
}

/* Script op: the same for pos. */
int func_02007e5c(dCamera_c *cam, u8 *data) {
    int pos[3];
    int rot[3];
    int *posPtr, *rotPtr;
    fx32 px, py, pz;
    int rx, ry, rz;
    pz = ReadUnalignedShort(data + 4) << 12;
    py = ReadUnalignedShort(data + 2) << 12;
    px = ReadUnalignedShort(data) << 12;
    pos[0] = px;
    pos[1] = py;
    pos[2] = pz;
    rz = ReadUnalignedShort(data + 0xa);
    ry = ReadUnalignedShort(data + 8);
    rx = ReadUnalignedShort(data + 6);
    rot[0] = rx;
    rot[1] = ry;
    rot[2] = rz;
    posPtr = pos;
    rotPtr = rot;
    Math_Function_0203b0fc(&cam->pos.x, posPtr[0], rotPtr[0], 0x7fffffff);
    posPtr++;
    rotPtr++;
    Math_Function_0203b0fc(&cam->pos.y, posPtr[0], rotPtr[0], 0x7fffffff);
    posPtr++;
    rotPtr++;
    Math_Function_0203b0fc(&cam->pos.z, posPtr[0], rotPtr[0], 0x7fffffff);
    return 1;
}

/* Script op, Super Mario 64's cutscene_dance_fly_away_start: clear the orbit
   offsets and seed mOrbitTargetYaw from mFixedViewPos, or per level and area
   (the N64's four star_dance_bound_yaw cases). */
int CutsceneDanceFlyAwayStart(dCamera_c *cam) {
    int lvl;
    void *p;
    cam->mOrbitTargetYaw = 0;
    cam->mPitchOffset = 0;
    cam->mYawOffset = 0;
    p = cam->mFixedViewPos;
    if (p != 0) {
        cam->mOrbitTargetYaw = Vec3_HorzAngle((const Vector3 *)p, &cam->pos);
    }
    lvl = SublevelToLevel(gCurrentSublevel);
    if (lvl == 3 && data_0209f224 == 4) {
        StarDanceBoundYaw(cam, 0x5600, 0x800);
    } else if (lvl == 0xb && data_0209f224 == 2) {
        StarDanceBoundYaw(cam, 0, 0x800);
    } else if (lvl == 9 && data_0209f224 == 1) {
        StarDanceBoundYaw(cam, 0x2000, 0x800);
    } else if (lvl == 0xe && data_0209f224 == 3) {
        StarDanceBoundYaw(cam, 0, 0x800);
    }
    return 1;
}

/* Super Mario 64's approach_vec3f_asymptotic, in fixed point: step each axis
   of self toward src by its own fraction of the gap (0x1000 = all of it).
   self is a Vector3, not the camera: callers pass &camera->lookAt. */
void ApproachVec3fAsymptotic(Vector3 *self, const Vector3 *src, int stepX, int stepY, int stepZ) {
    Math_Function_0203b0fc(&self->x, src->x, stepX, 0x7fffffff);
    Math_Function_0203b0fc(&self->y, src->y, stepY, 0x7fffffff);
    Math_Function_0203b0fc(&self->z, src->z, stepZ, 0x7fffffff);
}

/* The same with one rate for all three axes. Its callers' declaration
   says int, and the ROM hands back whatever ApproachVec3fAsymptotic left in r0, so
   there is no return statement. */
extern "C" int func_02007cec(void *dest, const Vector3 *src, int step) {
    ApproachVec3fAsymptotic((Vector3 *)dest, src, step, step, step);
}

/* Super Mario 64's vec3f_get_dist_and_angle: the distance and the pitch and
   yaw from v0 to v1. */
extern "C" void Vec3fGetDistAndAngle(const Vector3 *v0, const Vector3 *v1, int *outDist,
                                     short *outVertAng, short *outHorzAng) {
    *outDist = Vec3_Dist(v0, v1);
    *outVertAng = Vec3_VertAngle(v1, v0);
    *outHorzAng = Vec3_HorzAngle(v0, v1);
}

/* Super Mario 64's vec3f_set_dist_and_angle, the inverse: res is trans plus
   mag along pitch ang, turned by yaw angY. The N64 takes the origin first;
   this takes the result first. */
extern "C" void Vec3fSetDistAndAngle(Vector3 *res, const Vector3 *trans, fx32 mag, short ang,
                                     short angY) {
    Vector3 v;
    unsigned idx = (unsigned short)ang >> 4;

    v.x = 0;
    v.y = FX_MUL(mag, FX_SinIdx(idx));
    v.z = FX_MUL(mag, FX_CosIdx(idx));
    Vec3_RotateYAndTranslate(res, trans, angY, &v);
}

/* Script op: step lookAt toward the target player's position plus a
   whole-unit offset. */
int func_02007b98(dCamera_c *cam, u8 *data) {
    struct Vector3 v;
    fx32 x, y, z;
    int b;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    v.x = x;
    v.y = y;
    v.z = z;
    b = data[6] << 4;
    VEC_Add(&v, &cam->mTargetPlayer->mPos, &v);
    func_02007cec(&cam->lookAt, &v, b);
    return 1;
}

/* Script op, Super Mario 64's focus_in_front_of_mario with the whole offset
   as operands (the N64 fixes x at 0 and y at 100): step lookAt toward a
   whole-unit offset turned to face the target player (yaw + 0x8000). */
int FocusInFrontOfMario(dCamera_c *cam, u8 *rec) {
    Vector3 v;
    Vector3 res;
    dActor_c *owner;
    int step;
    fx32 vx, vy, vz;
    vz = ReadUnalignedShort(rec + 4) << 12;
    vy = ReadUnalignedShort(rec + 2) << 12;
    vx = ReadUnalignedShort(rec + 0) << 12;
    v.x = vx;
    v.y = vy;
    v.z = vz;
    step = rec[6] << 4;
    owner = cam->mTargetPlayer;
    Vec3_RotateYAndTranslate(&res, &owner->mPos, owner->mAngleY + 0x8000, &v);
    func_02007cec(&cam->lookAt, &res, step);
    return 1;
}

/* Script op: orbit pos around the target player (func_020070e8); a yaw of
   -1 means mOrbitTargetYaw. */
int func_02007a8c(dCamera_c *cam, u8 *data) {
    fx32 distTarget = ReadUnalignedShort(data) << 12;
    int distStep = data[2] << 4;
    short vertTarget = ReadUnalignedShort(data + 3);
    short vertDiv = data[5];
    short horzTarget = ReadUnalignedShort(data + 6);
    if (horzTarget == -1)
        horzTarget = cam->mOrbitTargetYaw;
    func_020070e8(cam, distTarget, distStep, vertTarget, vertDiv, horzTarget, data[8]);
    return 1;
}

/* Script op, Super Mario 64's cutscene_pan_cvar9 with the pitch and yaw steps
   (-29 and 29 there) as operands: orbit lookAt around pos. On the first frame
   savedLookAt takes lookAt; every frame the two offsets accumulate and are
   added to the angles of the pos -> savedLookAt line before lookAt is rebuilt
   from it. */
int CutscenePanCvar9(dCamera_c *cam, u8 *data, int frame) {
    int dist;
    short vert, horz;
    if (frame == gKuppaScriptFrame) {
        cam->savedLookAt.x = cam->lookAt.x;
        cam->savedLookAt.y = cam->lookAt.y;
        cam->savedLookAt.z = cam->lookAt.z;
    }
    cam->mPitchOffset -= ReadUnalignedShort(data);
    cam->mYawOffset += ReadUnalignedShort(data + 2);
    Vec3fGetDistAndAngle(&cam->pos, &cam->savedLookAt, &dist, &vert, &horz);
    vert = (vert + cam->mPitchOffset);
    horz = (horz + cam->mYawOffset);
    Vec3fSetDistAndAngle(&cam->lookAt, &cam->pos, dist, vert, horz);
    return 1;
}

/* Script op: move pos by a distance and two angles relative to the target
   player, the steps of Super Mario 64's rotate_and_move_vec3f. The rotate
   and fly-away dances use it where the N64 calls that helper. */
int RotateAndMoveVec3f(dCamera_c *cam, u8 *p) {
    Vector3 *v = &cam->mTargetPlayer->mPos;
    int dist;
    short vert;
    short horz;

    Vec3fGetDistAndAngle(v, &cam->pos, &dist, &vert, &horz);
    dist = ReadUnalignedSignedInt(p) + dist;
    vert = (ReadUnalignedShort(p + 4) + vert);
    horz = (ReadUnalignedShort(p + 6) + horz);
    Vec3fSetDistAndAngle(&cam->pos, v, dist, vert, horz);
    return 1;
}

/* Script op: turn mFovAngle toward a target. */
int func_020078c4(dCamera_c *cam, u8 *data) {
    short v = ReadUnalignedShort(data);
    unsigned char b = data[2];
    ApproachAngle(&cam->mFovAngle, v, b, 0x4000, 0);
    return 1;
}

/* Script op, Super Mario 64's cutscene_dance_shake_fov with set_fov_shake's
   arguments as operands. Each dance script passes the values of its N64
   *_shake_fov: 0x200/0x28 for the default and rotate dances, 0x300/0x30
   closeup, 0x400/0x30 fly-away and 0x180/0x30 key dance, each with 0x8000. */
int CutsceneDanceShakeFov(dCamera_c *cam, u8 *data) {
    int v1 = ReadUnalignedShort(data);
    int v2 = ReadUnalignedShort(data + 2);
    int v3 = ReadUnalignedShort(data + 4);
    cam->SetFovShake(v1, v2, v3);
    return 1;
}

/* Script op, Super Mario 64's cutscene_dance_closeup_start: the per-level orbit
   yaws (StarDanceBoundYaw). */
int CutsceneDanceCloseupStart(dCamera_c *cam) {
    dCamera_c *camera = cam;
    int lvl = SublevelToLevel(gCurrentSublevel);

    if (lvl == 2 && data_0209f224 == 5) {
        StarDanceBoundYaw(camera, 0, 0x4000);
    } else if (lvl == 8 && data_0209f224 == 1) {
        StarDanceBoundYaw(camera, 0x8000, 0x5000);
    }

    return 1;
}

/* Script op, Super Mario 64's cutscene_dance_closeup_fly_above: orbit pos
   toward mOrbitTargetYaw, slower in two areas. */
int CutsceneDanceCloseupFlyAbove(dCamera_c *cam) {
    short amt = 0x1800;
    int level = SublevelToLevel(gCurrentSublevel);
    if ((level == 9 && data_0209f224 == 6) || (level == 0xd && data_0209f224 == 4))
        amt = 0x800;
    func_020070e8(cam, 0x320000, 0xcc, amt, 0x10, cam->mOrbitTargetYaw, 8);
    return 1;
}

/* Script op, Super Mario 64's cutscene_key_dance_jump_cvar with the cvar 7
   and 8 offsets as operands: place lookAt and pos at whole-unit offsets
   turned to face the target player. Only the key dance uses it, and its four
   records carry the N64's jump_closeup, jump_lower_left, jump_above and
   jump_last offsets exactly. */
int CutsceneKeyDanceJumpCvar(dCamera_c *cam, u8 *rec) {
    Vector3 v;
    Vector3 w;
    dActor_c *owner;
    short angle;
    Vector3 *pos;
    int vx, vy, vz;
    int wx, wy, wz;

    vz = ReadUnalignedShort(rec + 4) << 12;
    vy = ReadUnalignedShort(rec + 2) << 12;
    vx = ReadUnalignedShort(rec + 0) << 12;
    v.x = vx;
    v.y = vy;
    v.z = vz;

    wz = ReadUnalignedShort(rec + 0xa) << 12;
    wy = ReadUnalignedShort(rec + 8) << 12;
    wx = ReadUnalignedShort(rec + 6) << 12;
    w.x = wx;
    w.y = wy;
    w.z = wz;

    owner = cam->mTargetPlayer;
    pos = &owner->mPos;
    angle = owner->mAngleY + 0x8000;
    Vec3_RotateYAndTranslate(&cam->lookAt, pos, angle, &v);
    Vec3_RotateYAndTranslate(&cam->pos, pos, angle, &w);
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_start: savedLookAt at
   data_02086ec0 in front of the target player, at height 0x1c9000. */
int CutsceneGrandStarStart(dCamera_c *cam) {
    int offset[3];
    offset[0] = data_02086ec0[0];
    offset[1] = data_02086ec0[1];
    offset[2] = data_02086ec0[2];

    short angle = cam->mTargetPlayer->mAngleY;
    angle = (angle + 0x8000);

    Vec3_RotateYAndTranslate(&cam->savedLookAt, &cam->mTargetPlayer->mPos, angle, offset);

    cam->savedLookAt.y = 0x1c9000;
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_front_of_mario: orbit pos
   (func_020070e8) from the target player toward savedLookAt's distance and
   angles, swung 0x1200 to the side. */
int CutsceneGrandStarFrontOfMario(dCamera_c *cam) {
    int dist;
    short vertAng;
    short horzAng;

    Vec3fGetDistAndAngle(&cam->mTargetPlayer->mPos, &cam->savedLookAt, &dist, &vertAng, &horzAng);
    func_020070e8(cam, dist, 0x199, vertAng, 0x20, (horzAng + 0x1200), 0x14);
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_focus_mario: step lookAt
   toward a point above the target player. */
int CutsceneGrandStarFocusMario(dCamera_c *cam) {
    Vector3 *base = &cam->mTargetPlayer->mPos;
    int y0 = base->y + 0xffecd000;
    int tz = base->z;
    int tx = base->x;
    Vector3 src;
    src.y = 0x197000 + (y0 >> 1);
    src.x = tx;
    src.z = tz;
    ApproachVec3fAsymptotic(&cam->lookAt, &src, 0x800, 0xccc, 0x800);
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_mario_jump: turn to the yaw
   behind the target player and clear savedLookAt. */
int CutsceneGrandStarMarioJump(dCamera_c *cam) {
    int yaw = cam->mTargetPlayer->mAngleY;
    yaw += 0x8000;
    cam->mTargetAngleY = yaw;
    cam->savedLookAt.x = 0;
    cam->savedLookAt.y = 0;
    cam->savedLookAt.z = 0;
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_accel_cvar2: speed
   savedLookAt.z up toward -40 and set its x to -5 (the N64's 5, mirrored
   by the DS camera's yaw + 0x8000). */
int CutsceneGrandStarAccelCvar2(dCamera_c *cam) {
    ApproachLinear(cam->savedLookAt.z, -0x28000, 0x2000);
    cam->savedLookAt.x = -0x5000;
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_approach_mario: ease
   savedLookAt.z to 0 and pos's x and z toward the target player. */
int CutsceneGrandStarApproachMario(dCamera_c *cam) {
    ApproachLinear(cam->savedLookAt.z, 0, 0x2000);

    cam->savedLookAt.x = 0;

    Vector3 *target = &cam->mTargetPlayer->mPos;

    Math_Function_0203b0fc(&cam->pos.x, target->x, 0x28, 0x7fffffff);
    Math_Function_0203b0fc(&cam->pos.z, target->z, 0x28, 0x7fffffff);

    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_move_cvar2: pos = savedLookAt
   turned by mTargetAngleY, added to pos. */
int CutsceneGrandStarMoveCvar2(dCamera_c *cam) {
    short angle = cam->mTargetAngleY;
    Vec3_RotateYAndTranslate(&cam->pos, &cam->pos, angle, &cam->savedLookAt);
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_fly_start: clears
   savedLookAt, sets savedPos to (0, lookAt's height above the target player,
   0), and clears mScriptParam and mRampSpeed. */
int CutsceneGrandStarFlyStart(dCamera_c *cam) {
    cam->savedLookAt.x = 0;
    cam->savedLookAt.y = 0;
    cam->savedLookAt.z = 0;

    dActor_c *player = cam->mTargetPlayer;
    int lookAtY = cam->lookAt.y;
    int playerY = player->mPosY;

    cam->savedPos.x = 0;
    cam->savedPos.y = lookAtY - playerY;
    cam->savedPos.z = 0;
    cam->mScriptParam = 0;
    cam->mRampSpeed = 0;

    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_fly_move_to_mario: step
   savedLookAt toward data_02086ed8 and ease savedPos's y to 0 and its z to
   -0xc8000. Its member-function constant is data_02086cb0. */
int CutsceneGrandStarFlyMoveToMario(dCamera_c *cam) {
    int offset[3];
    offset[0] = data_02086ed8[0];
    offset[1] = data_02086ed8[1];
    offset[2] = data_02086ed8[2];

    func_02007cec(&cam->savedLookAt, (Vector3 *)offset, 0xcc);
    ApproachLinear(cam->savedPos.y, 0, 0x2000);
    ApproachLinear(cam->savedPos.z, -0xc8000, 0x6aaa);
    return 1;
}

/* Script op, Super Mario 64's cutscene_grand_star_fly_mario_offscreen: ramp
   mRampSpeed up to 0xf000 and ease savedLookAt and savedPos onto fixed targets,
   paced by it. Its member-function constant is data_02086ca8. Defined last, so
   it lands at the run's low edge. */
int CutsceneGrandStarFlyMarioOffscreen(dCamera_c *cam) {
    ApproachLinear(cam->mRampSpeed, 0xf000, 0x199);
    ApproachLinear(cam->savedLookAt.x, 0x7d0000, cam->mRampSpeed);
    ApproachLinear(cam->savedLookAt.y, 0x4b0000, cam->mRampSpeed / 10);
    ApproachLinear(cam->savedLookAt.z, 0x3e8000, cam->mRampSpeed / 10);
    ApproachLinear(cam->savedPos.x, 0, cam->mRampSpeed);
    ApproachLinear(cam->savedPos.y, 0x4b0000, cam->mRampSpeed >> 1);
    ApproachLinear(cam->savedPos.z, 0x3e8000, cam->mRampSpeed * 10 / 15);
    return 1;
}

/* Nineteen camera states, definition order is retail __sinit order.
   mwcc emits __sinit_d_camera.cpp from these, which was the handwritten
   __sinit_02073a24. */

extern CamPmf data_02086d28, data_02086c28, data_02086da8, data_02086c30, data_02086c48,
    data_02086c40, data_02086c98, data_02086c58, data_02086c68, data_02086bf8, data_02086d58,
    data_02086d48, data_02086c70, data_02086bf0, data_02086db8, data_02086d30, data_02086c10,
    data_02086d90, data_02086be8, data_02086be0, data_02086d10, data_02086c00, data_02086c08,
    data_02086bc8, data_02086c18, data_02086db0, data_02086bd0, data_02086e40, data_02086ce0,
    data_02086e28, data_02086e18, data_02086e10, data_02086e00, data_02086df8, data_02086de8,
    data_02086de0, data_02086dd0, data_02086dc8;

dCamera_c::State data_0209b008 = {data_02086d28, data_02086c28};
dCamera_c::State data_0209b018 = {data_02086da8, data_02086c30};
dCamera_c::State data_0209b028 = {data_02086c48, data_02086c40};
dCamera_c::State data_0209b038 = {data_02086c98, data_02086c58};
dCamera_c::State data_0209b048 = {data_02086c68, data_02086bf8};
dCamera_c::State data_0209b058 = {data_02086d58, data_02086d48};
dCamera_c::State data_0209b068 = {data_02086c70, data_02086bf0};
dCamera_c::State data_0209b078 = {data_02086db8, data_02086d30};
dCamera_c::State data_0209b088 = {data_02086c10, data_02086d90};
dCamera_c::State data_0209b098 = {data_02086be8, data_02086be0};
dCamera_c::State data_0209b0a8 = {data_02086d10, data_02086c00};
dCamera_c::State data_0209b0b8 = {data_02086c08, data_02086bc8};
dCamera_c::State data_0209b0c8 = {data_02086c18, data_02086db0};
dCamera_c::State data_0209b0d8 = {data_02086bd0, data_02086e40};
dCamera_c::State data_0209b0e8 = {data_02086ce0, data_02086e28};
dCamera_c::State data_0209b0f8 = {data_02086e18, data_02086e10};
dCamera_c::State data_0209b108 = {data_02086e00, data_02086df8};
dCamera_c::State data_0209b118 = {data_02086de8, data_02086de0};
dCamera_c::State data_0209b128 = {data_02086dd0, data_02086dc8};
