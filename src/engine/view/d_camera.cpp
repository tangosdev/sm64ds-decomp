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
 *   addresses. Those that work on the camera they are passed are dCamera_c
 *   members; the rest keep C linkage. The three line-query handlers use C++
 *   for native collision-result initialization, and the state-change, shake
 *   and FOV handlers use C++ so they can call ChangeState and FX_Div
 *   directly; other handlers remain in cplusplus-off regions.
 * - Vec3 remains the POD scratch triple in the older handlers. Native
 *   collision results use Vector3 and its empty destructor.
 * - The 0x0209b008 state table is nineteen dCamera_c::State records
 *   defined at the bottom of this file. These bodies only take their
 *   address.
 * - CleanupResources keeps both null checks. The second is the
 *   inlined deleting destructor.
 * - The controller records at 0x0209f498 are read through the separate
 *   data_0209f49c..data_0209f4ae symbols with (char *) stride arithmetic.
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
#include "PathPtr.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "OAM.h"
#include "G2x.h"
#include "G3i.h"
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

extern s16 FX_SinCosTable_[];
extern void *data_02086efc;
extern void *data_02086f08;
extern s32 data_02086f14[3];
extern Vector3 data_02086f20;
extern Vector3 data_02086f2c;
extern int data_02086fcc;
extern char data_02086ff4;
extern char data_02087094;
extern int data_0208706c;
extern int data_0208715c;
extern int data_02087184;
extern int data_020871fc;
extern int data_02087224;
extern int data_02087274;
extern int data_02087314;
extern int data_0208747c;
extern char data_020871ac;
extern char data_020871d4;
extern char data_02087404;
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
extern void *data_ov002_0210c3b0;
extern void *data_ov002_0210c3e0[2];

extern "C" {
/* Siblings below 0x0200bec4 (defined at the end of the file). */
extern int func_020092c4(void *cam, Vector3 *out, const Vector3 *target);
extern s32 Math_Function_0203b14c(s32 *p, s32 tgt, s32 rate, s32 lim, s32 step);
extern int StartWithFarCamera();
extern struct ViewObj *GetViewObj(int idx);

extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern int Vec3_HorzLen(const void *v);
extern int LenVec3(void *v);
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern void AddVec3(void *a, void *b, void *c);
extern void SubVec3(void *a, void *b, void *c);
extern void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern int *Vec3_LslInPlace(int *v, int sh);
extern void Vec3_RotateYAndTranslate(void *out, const void *in, short angle, const void *src);
extern void MulVec3Mat4x3(void *dst, Matrix4x3 *src, void *out);
extern s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
/* local extern: including math/MtxStack.h renumbers this TU's function-local
   statics (sScriptOps, sPrevTurnInput), whose numbered names symbols.txt
   records, so its matrix routines stay declared here. */
extern void Matrix4x3_FromRotationZ(void *m, int angle);
extern void MulMat4x3Mat4x3(const Matrix4x3 *a, const Matrix4x3 *b, Matrix4x3 *out);
extern void Matrix4x3_LoadIdentity(Matrix4x3 *matrix);
}

/* Opaque camera-side records used by the older handlers. Ground and line
   queries use the real dBgCh_Gnd and dBgCh_Lin classes above. The remaining
   records are typedef'd for the cplusplus-off regions. */
typedef struct dBgCh_LinHit {
    char pad[0x14];
    char surf[0x64];
} dBgCh_LinHit; /* UpdateRadialCamera's variant */
typedef struct Entry3 { int a, b, c; } Entry3;                      /* data_02086f58 rows */
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
struct CamPmfHost {
    char pad138[0x138];
    CamPmf *pp;                     /* camera +0x138 == mState */
    char pad13c[0x154 - 0x13c];
    unsigned int flags;             /* camera +0x154 == mFlags */
};

enum Bool { FALSE, TRUE };

/* Allocation factory. new dCamera_c is the fBase constructor, the dBase and
 * dView vtables, the view-matrix clear, and the dCamera vtable. */
extern "C" dCamera_c *dCamera_c_classInit() {
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
    ChangeState((State *)&data_0209b008);
    func_0200cf40();
    if (StartWithFarCamera() != 0)
        func_0200d0ac(gLocalPlayerId);
    return this->Render();
}

int dCamera_c::Behavior()
{
    s32 spVec[3];
    s32 temp_r1;
    s32 r5;

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
    temp_r1 = mFlags;
    if (!(temp_r1 & 8)) {
        enum Bool b = (enum Bool)(gRunningKuppaScript != 0);
        if (b != FALSE) goto block_7;
    } else {
block_7:
        if (temp_r1 & 0xc000) {
            mFlags &= ~0xc000u;
            ChangeState((State *)&data_0209b008);
        }
        mAngleY = Vec3_HorzAngle(&lookAt, &pos);
        mAngleX = Vec3_VertAngle(&lookAt, &pos);
        UpdateTargetPos();
    }

    if ((u8)(gVsExitScreenActive | (gGamePaused | data_0209f20c)) == 0 &&
        !(gNextActorUpdateMask & ACTOR_UPDATE_MASK_STAGE_ENDING)) {
        enum Bool b2 = (enum Bool)(gRunningKuppaScript != 0);
        if (b2 == FALSE) {
            if (mFlags & 0x2000) {
                Math_Function_0203b0fc((int *)&mFovBlend, 0x1000, 0x88, 0x7fffffff);
            } else {
                Math_Function_0203b0fc((int *)&mFovBlend, 0, 0x88, 0x7fffffff);
            }
            UpdateState();
            if (mShakeY != 0) {
                ApproachLinear(*(int *)&mShakeY, 0, 0x1000);
                mShakeY = 0 - mShakeY;
            }
            if (mRollShake != 0) {
                if (ApproachLinear2(mRollShake, 0, 0x10) != 0) {
                    mRollShakePhase = 0;
                } else {
                    *(s16 *)((int)&mRollShakePhase) =
                        (s16)(*(s16 *)((int)&mRollShakePhase) + 0x3000);
                }
            }
        }
        if (mFovShake != 0) {
            *(s16 *)((int)&mFovShakePhase) =
                (s16)(*(s16 *)((int)&mFovShakePhase) + mFovShakePhaseStep);
            ApproachLinear2(mFovShake, 0, mFovShakeDecay);
        }
        {
            int condA = 1;
            if (gCurrentSublevel != 0xa && gCurrentSublevel != 0x13)
                condA = 0;
            if (condA != 0) {
                r5 = 0x112;
            } else {
                u8 t = (u8)gCurrentSublevel + 0xf8;
                enum Bool ip = FALSE;
                if ((u32)t <= 0x16) {
                    if ((1 << t) & 0x402401) {
                        ip = TRUE;
                    }
                }
                if (ip != FALSE) {
                    r5 = 0xbc;
                } else if (gCurrentSublevel == 0x31) {
                    r5 = 0x86;
                } else {
                    r5 = -1;
                }
            }
        }
        if (r5 >= 0) {
            MulVec3Mat4x3(&CAM_SPACE_CAM_POS_ASR_3, &data_0209b41c, spVec);
            Vec3_LslInPlace(spVec, 3);
            int flag;
            if (r5 != 0xbc) {
                flag = 1;
            } else {
                flag = IsUnderwater();
            }
            mWeather = Particle::System::NewWeather(mWeather, (u32)r5, spVec[0], spVec[1], spVec[2],
                                                    0, flag);
            if (r5 == 0x86) {
                mWeather2 = Particle::System::NewWeather(mWeather2, 0x87, spVec[0], spVec[1],
                                                         spVec[2], 0, 1);
            }
        }
    }

    {
        daPly_c *m = (daPly_c *)mTargetPlayer;
        enum Bool isBf = (enum Bool)(m->actorID == 0xbf);
        if (isBf != FALSE) {
            s32 t2 = data_0209f32c - (FX_SinCosTable_[((u16)mAngleX >> 4) * 2] * 0x10);
            if (!(mFlags & 1)) {
                if (m->mIsUnderwater != 0 || m->mIsInShallowWater != 0) {
                    if (pos.y <= t2) {
                        mFlags |= 1u;
                    }
                }
            } else if (pos.y > t2) {
                mFlags &= ~1u;
            }
        }
    }
    {
        u32 *pf = &mFlags;
        *pf &= 0xfff8fdfbu;
        if (mFocusActor != 0) {
            mFocusActor = 0;
            *pf |= 4u;
        }
    }
    mFramedActor = 0;
    func_0203dafc((short)(mAngleY + mInputYawLag));
    ApproachLinear2(mInputYawLag, 0, 0x400);
    return 1;
}

int dCamera_c::Render()
{
    dCamera_c *self = this;
    s32 sp[6];
    s16 sp18;
    s16 var_r4;

    sp[0] = self->lookAt.x;
    sp[1] = self->lookAt.y;
    sp[2] = self->lookAt.z;
    sp[3] = self->pos.x;
    sp[4] = self->pos.y;
    sp[5] = self->pos.z;
    sp18 = self->mFovAngle;
    var_r4 = self->mAngleZ;

    if (Vec3_Dist((Vector3 *)&sp[0], &self->mPrevLookAt) < 0x1000 &&
        Vec3_Dist((Vector3 *)&sp[3], &self->mPrevPos) < 0x1000) {
        sp[0] = self->mPrevLookAt.x;
        sp[1] = self->mPrevLookAt.y;
        sp[2] = self->mPrevLookAt.z;
        sp[3] = self->mPrevPos.x;
        sp[4] = self->mPrevPos.y;
        sp[5] = self->mPrevPos.z;
    }

    if (!(gRunningKuppaScript != 0 ? 1 : 0)) {
        s32 f = self->mFlags;
        if (!(f & 8)) {
            if ((u8 *)self->mMode == &data_0208738c) {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3b0, 0x80, 0x60, -1, -1, 0x1000, 0x1000,
                            0, -1);
                G2x::SetBlendAlpha((volatile unsigned short *)0x04000050, 0x10, 0x2f, 0xc, 6);
            } else {
                if (f & 0x20) {
                    OAM::Render(0, (OamAttr *)data_ov002_0210c3e0[0], 0x14, 0x60, -1, -1, 0x1000,
                                0x1000, 0, -1);
                }
                if (self->mFlags & 0x40) {
                    OAM::Render(0, (OamAttr *)data_ov002_0210c3e0[1], 0xec, 0x60, -1, -1, 0x1000,
                                0x1000, 0, -1);
                }
            }
            s32 t = self->mFovBlend;
            if (t != 0x1000) {
                s32 d = 0xaaa - sp18;
                sp18 = (s16)(sp18 + (s16)FX_MUL(d, t));
            }
            if (data_0209f20c == 0 && gVsExitScreenActive == 0 &&
                (u8)(gVsExitScreenActive | (gGamePaused | data_0209f20c)) != 0) {
                func_0200af20((Vector3 *)&sp[0], (Vector3 *)&sp[3], &sp18);
            }
        }
    }

    sp[1] += self->mShakeY;
    sp[4] += self->mShakeY;

    s16 tr3 = self->mRollShake;
    if (tr3 != 0) {
        s16 c = FX_SinCosTable_[((s32)(u16)self->mRollShakePhase >> 4) * 2];
        var_r4 = (s16)(var_r4 + (s16)FX_MUL(tr3, c));
    }

    s16 tr0 = self->mFovShake;
    if (tr0 != 0) {
        s16 c = FX_SinCosTable_[((s32)(u16)self->mFovShakePhase >> 4) * 2 + 1];
        s32 m = FX_MUL(c, 0x360);
        sp18 = (s16)(sp18 + ((tr0 * m) << 4 >> 16));
    }

    SetFovAngle(sp18);

    *(volatile s32 *)0x04000580 = self->mViewport[0] | (self->mViewport[1] << 8) |
                                  (self->mViewport[2] << 16) | (self->mViewport[3] << 24);

    s32 var_r6;
    if (gCurrentSublevel == 0x28) {
        var_r6 = 0x10;
    } else {
        var_r6 = data_0209ee90[0x44 / 4];
    }

    s32 idx = ((s32)(u16)sp18 >> 4) * 2;
    G3i::PerspectiveW_(FX_SinCosTable_[idx], FX_SinCosTable_[idx + 1], self->mAspect, self->mNear,
                       self->mFar, var_r6, 1, (Matrix4x3 *)0);

    {
        s32 s2 = (s32)(sp[2] + 4) >> 3;
        s32 s1 = (s32)(sp[1] + 4) >> 3;
        s32 s0 = (s32)(sp[0] + 4) >> 3;
        sp[0] = s0;
        sp[1] = s1;
        sp[2] = s2;
        sp[5] = (s32)(sp[5] + 4) >> 3;
        s32 s4 = (s32)(sp[4] + 4) >> 3;
        sp[3] = (s32)(sp[3] + 4) >> 3;
        sp[4] = s4;
    }

    void *lookdir = &data_02086efc;
    if (Vec3_HorzDist((Vector3 *)&sp[0], (Vector3 *)&sp[3]) == 0) {
        lookdir = &data_02086f08;
    }
    G3i::LookAt_((Vector3 *)&sp[3], (Vector3 *)lookdir, (Vector3 *)&sp[0], 1, &self->viewMat);

    if (var_r4 != 0) {
        Matrix4x3_FromRotationZ(&data_020a0e68, var_r4);
        MulMat4x3Mat4x3(&self->viewMat, &data_020a0e68, &self->viewMat);
    }

    dView_c::Render();
    return 1;
}

/* No-op override; the camera does nothing extra when flagged for destruction. */
void dCamera_c::OnPendingDestroy()
{
    (void)this;
}

/* Frees the owned fixed-view-position record at 0x148 and reports success.
 * The doubled null check is the ROM's own guard followed by the inlined
 * deleting-destructor's, giving two consecutive `cmp r0,#0; beq`. */
int dCamera_c::CleanupResources()
{
    void *p = mFixedViewPos;
    if (p) {
        if (p)
            Memory::operator_delete2(p);
    }
    return 1;
}

extern "C" {
void dCamera_c::SetFovAngle(short arg) {
    int i = (int)(unsigned short)arg >> 4;
    int j = i * 2;
    mInvSinFov = FX_Inv(FX_SinCosTable_[j]);
    mInvCosFov = FX_Inv(FX_SinCosTable_[j + 1]);
    data_0209f43c.Func_020156DC(mAspect, arg, mNear, mFar);
}
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
void dCamera_c::func_0200d8c8(const Vector3 *v, int strength) {
    Fix12i dist;
    Fix12i offset;

    dist = Vec3_Dist(&pos, v);
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
    short ip;

    ip = mFovShake;
    if (amplitude > ip) {
        mFovShake = (short)amplitude;
        mFovShakeDecay = (short)decay;
        mFovShakePhaseStep = (short)shakeSpeed;
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
    AddVec3(&lookAt, (void *)offset, &lookAt);
    AddVec3(&pos, (void *)offset, &pos);
}

void dCamera_c::func_0200d81c(int playerID) {
    if (playerID != (int)gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b008);
}

void dCamera_c::SetCameraZoomState(int playerID) {
    if (playerID != (int)gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b098);
}

void dCamera_c::func_0200d7a4(u8 playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b108);
}

void dCamera_c::func_0200d768(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b018);
}

void dCamera_c::func_0200d72c(unsigned char playerID) {
    if (playerID == *(unsigned char *)&gLocalPlayerId)
        ChangeState((dCamera_c::State *)&data_0209b028);
}

void dCamera_c::func_0200d6f0(unsigned char playerID) {
    if (playerID == *(unsigned char *)&gLocalPlayerId) {
        ChangeState((dCamera_c::State *)&data_0209b038);
    }
}

void dCamera_c::func_0200d6b4(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b048);
}

void dCamera_c::SetSpinState(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b058);
}

void dCamera_c::func_0200d63c(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b068);
}

void dCamera_c::func_0200d5fc(int playerID) {
    if (playerID == gLocalPlayerId) {
        ChangeState((dCamera_c::State *)&data_0209b078);
        SetMode(5);
    }
}

void dCamera_c::func_0200d5c0(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b078);
}

void dCamera_c::SetHeadstandState(int playerID) {
    if (playerID != gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b078);
    SetMode(8);
}

void dCamera_c::func_0200d544(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b088);
}

void dCamera_c::func_0200d508(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b0a8);
}

void dCamera_c::StartDanceCutscene(unsigned int playerID, int arg2) {
    unsigned char tmp = gLocalPlayerId;
    if (playerID == tmp)
    {
        volatile int dummy;
        struct dActor_c *other;
        mOrbitTargetYaw = mAngleY;
        other = mTargetPlayer;
        other->mFlags |= 0x20000000;
        KuppaScript::StartDanceCutscene(arg2);
    }
}

void dCamera_c::func_0200d474(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b0b8);
}

void dCamera_c::func_0200d438(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b0c8);
}

void dCamera_c::func_0200d3f8(unsigned char playerID, void *ptr) {
    unsigned char tmp = *(unsigned char *)&gLocalPlayerId;
    if (playerID != tmp)
        return;
    mLookTarget = (s32 *)ptr;
    ChangeState((dCamera_c::State *)&data_0209b0d8);
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
    ChangeState((State *)&data_0209b0e8);
}

void dCamera_c::func_0200d1e4() {
    struct dActor_c *info = mTargetPlayer;
    Vector3 *base = &info->mPos;
    int v1;
    unsigned int r0;
    s32 temp1[3];
    short angle;
    int v2;
    unsigned int dResult;
    int v3;
    unsigned int r0b;
    s32 temp2[3];

    mTargetPos.x = base->x;
    mTargetPos.y = base->y;
    mTargetPos.z = base->z;

    v1 = mMode->lookAtDist;
    r0 = ScaleDistByFov(v1);

    temp1[0] = data_02086f2c.x;
    temp1[1] = data_02086f2c.y;
    temp1[2] = r0;
    data_02086f2c.z = r0;

    angle = mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&lookAt, &mTargetPos, angle, temp1);

    v2 = mMode->lookAtHeight;
    dResult = ScaleHeightByFov(v2);
    lookAt.y += dResult;

    v3 = mMode->posDist;
    r0b = ScaleDistByFov(v3);

    data_02086f14[2] = r0b;
    temp2[0] = data_02086f14[0];
    temp2[1] = data_02086f14[1];
    temp2[2] = r0b;

    angle = mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&pos, &lookAt, angle, temp2);

    pos.y += (int)(dResult - 0x8b000);

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
    ChangeState((State *)&data_0209b0f8);
    mFlags |= 0x10;
}

void dCamera_c::func_0200d148(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b118);
}

void dCamera_c::SetGrabBowserTailState(unsigned char playerID) {
    if (playerID != *(unsigned char *)&gLocalPlayerId)
        return;
    ChangeState((dCamera_c::State *)&data_0209b128);
}

int dCamera_c::func_0200d0ac(unsigned int playerID) {
    if (playerID == gLocalPlayerId) {
        unsigned short v = mAreaMode->flags;
        if (v & 1) {
            return 0;
        }
        mFlags |= 2u;
        func_02012790(7);
    }
    return 1;
}

void dCamera_c::func_0200d064(unsigned char playerID) {
    unsigned char tmp = gLocalPlayerId;
    if (playerID == tmp) {
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
    int new_var;
    mViewport[0] = 0;
    mViewport[1] = 0;
    mViewport[2] = 0xff;
    mViewport[3] = 0xbf;
    mAspect = 0x1555;
    mNear = 0x1000;
    mFar = 0x1388000;
    mAreaMode = (CamMode *)&data_0208715c;
    mViewObj = 0;
    if ((int)param1 != 0xf) {
        CamEntryE *v;
        int x;
        int y;
        int z;
        SetMode(0xa);
        mFovAngle = mMode->fovAngle;
        SetFovAngle(mFovAngle);
        func_0200cce4();
        v = (CamEntryE *)GetViewObj((int)param1 & 0xff);
        x = v->f2;
        z = v->f6;
        new_var = z << 12;
        y = v->f4;
        pos.x = x << 12;
        pos.y = y << 12;
        pos.z = new_var;
        mBaseAngleY = v->fa + 0x8000;
        return;
    }
    SetMode(9);
    mFovAngle = mMode->fovAngle;
    SetFovAngle(mFovAngle);
    func_0200cce4();
}

void dCamera_c::func_0200cce4() {
    VecFx32 vec;
    VecFx32 tmp1;
    VecFx32 tmp2;
    u32 d;

    if (mTargetPlayer != 0) {
        VecFx32 *p = (VecFx32 *)&mTargetPlayer->mPos;
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
        Vec3_RotateYAndTranslate((VecFx32 *)&lookAt, &mTargetPos, mTargetPlayer->mAngleY, &tmp1);

        d = ScaleHeightByFov(mMode->lookAtHeight);
        lookAt.y += d;

        {
            u32 d2 = ScaleDistByFov(-mMode->posDist);
            tmp2.z = d2;
            data_02086f20.z = d2;
            tmp2.x = data_02086f20.x;
            tmp2.y = data_02086f20.y;
        }
        Vec3_RotateYAndTranslate((VecFx32 *)&pos, (VecFx32 *)&lookAt, mTargetPlayer->mAngleY,
                                 &tmp2);

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
        line.SetObjAndLine(pos, *(Vector3 *)&vec, 0);
        if (line.DetectClsn() != 0) {
            Vector3 cp = line.GetClsnPos();
            vec.x = cp.x;
            vec.y = cp.y;
            vec.z = cp.z;
        }
        dBgCh_Gnd ground;
        PrepareBgCh(ground);
        ground.SetObjAndPos(*(Vector3 *)&vec, 0);
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
    int r1 = func_020092c4(this, &lookAt, &savedLookAt);
    int r2 = func_020092c4(this, &pos, &savedPos);
    short h;
    short v;

    if (r2 != 0) {
        if (r1 != 0) {
            mFlags &= ~0x8000;
        }
    }

    h = Vec3_HorzAngle(&lookAt, &pos);
    mAngleY = h;

    v = Vec3_VertAngle(&lookAt, &pos);
    mAngleX = v;
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
        if (mState == (State *)&data_0209b0c8) {
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
    CamPmfHost *c = (CamPmfHost *)this;
    if (c->flags & 0x4000) {
        volatile unsigned int *flags = (volatile unsigned int *)&c->flags;
        *flags &= ~0x4000u;
        *flags |= 0x8000u;
    }
    CamPmf *p = c->pp;
    if (*(int *)p == 0)
        return 1;
    return (c->**p)();
}

/* Per-frame state tick: mFovAngle eases toward the mode record's +0x24
 * angle, then the state's +8 member-function pointer runs. */
int dCamera_c::UpdateState() {
    CamPmfHost *ch = (CamPmfHost *)this;
    int flags;
    int r5;
    {
        CamMode *p = mMode;
        ApproachLinear2(mFovAngle, p->fovAngle, 0x80);
    }
    r5 = 1;
    flags = mFlags;
    if (flags & 8) {
        ;
    } else if (flags & 0x8000) {
        RestoreCameraStateAfterTalk();
    } else {
        u8 *obj = (u8 *)mState;
        if (*(int *)(obj + 8) != 0) {
            CamPmf *pp = (CamPmf *)(obj + 8);
            r5 = (ch->**pp)();
        }
    }
    return r5;
}

void dCamera_c::func_0200ca14(unsigned char playerID, int r2) {
    if (playerID != (unsigned int)gLocalPlayerId)
        return;

    if (r2 == 0)
        mFlags |= 0x20000;
    else
        mFlags |= 0x10000;
}

void dCamera_c::func_0200c9e0(s32 *r1, s32 *r2) {
    unsigned int v = mFlags;
    if (v & 0x10000u) {
        *r1 = 0xfffffd39;
        *r2 = 0xfffffd39;
        return;
    }
    if (v & 0x20000u) {
        *r1 = 0xd1b;
        *r2 = 0xd1b;
    }
}

int dCamera_c::func_0200c66c(void *pos_, int *out2_, int *out1_, int *arg5_) {
    VecFx32 *pos = (VecFx32 *)pos_;
    void **out2 = (void **)out2_;
    void **out1 = (void **)out1_;
    VecFx32 tmp;
    int hval;
    int col1;
    int col2;
    daPly_c *p;
    daPly_c *obj;
    int res;

    p = (daPly_c *)mTargetPlayer;
    obj = 0;
    {
        int b = p->actorID;
        b = b == 0xbf;
        if (b != 0) {
            res = p->mGroundY;
            if ((u32)res == 0x80000000)
                res = p->mPosY;
            obj = p;
        }
    }

    if (obj != 0 && gCurrentSublevel != 0x1c) {
        col1 = obj->mFloorCameraBehavior & 0xff;
        col2 = obj->mFloorViewID & 0xff;
        hval = res;
    } else {
        dBgCh_Lin line;
        PrepareBgCh(line);
        {
            int tx, tz, ty;
            tx = pos->x;
            tz = pos->z;
            ty = pos->y + 0x100000;
            tmp.x = tx;
            tmp.y = ty;
            tmp.z = tz;
        }
        line.SetObjAndLine(*(Vector3 *)pos, *(Vector3 *)&tmp, 0);
        if (line.DetectClsn() != 0) {
            Vector3 clsn = line.GetClsnPos();
            tmp.x = clsn.x;
            tmp.y = clsn.y;
            tmp.z = clsn.z;
        }
        dBgCh_Gnd ground;
        PrepareBgCh(ground);
        ground.SetObjAndPos(*(Vector3 *)&tmp, 0);
        if (ground.DetectClsn() == 0) {
            hval = pos->y;
            col1 = 0;
            col2 = 0x3f;
        } else {
            hval = ground.clsnY;
            col1 = ((const CLPS *)&ground.surface)->GetCameraBehavior() & 0xff;
            col2 = ((const CLPS *)&ground.surface)->GetViewID() & 0xff;
        }
    }

    if (col1 == 0 || (mFlags & 0x400) != 0) {
        if (mMode->flags & 0x10) {
            if (mAreaMode)
                *out1 = mAreaMode;
            if (mViewObj)
                *out2 = mViewObj;
        }
    } else {
        int b0;
        if (col2 != 0x3f) *out2 = GetViewObj(col2);
        if (func_0200c394(col1, *out2, pos, out1, arg5_) == 0) {
            if ((u32)mMode <= (u32)&data_02086ff4 || *out1 == (void *)&data_02087274) {
                if (*out2 != 0) {
                    if (*(unsigned char*)(*out2) == 6) *out1 = (void*)&data_0208742c;
                }
            }
        } else {
            if (col2 != 0x3f) {
                b0 = *(unsigned char*)(*out2);
                if (b0 == 4) {
                    *out1 = (void*)&data_020873dc;
                    if (*(unsigned char*)((char*)*out2 + 1) == 2) {
                        if (mAreaMode != *out1)
                            mFlags |= 4;
                    }
                    res = hval;
                } else if (b0 == 6) {
                    *out1 = (void*)&data_0208742c;
                } else {
                    if ((mMode->flags & 4) == 0) {
                        if (b0 == 0) {
                            if (*out1 == (void *)&data_02087274 && gCurrentSublevel == 0x2d) {
                                mLookTargetPos.x = mTargetPos.x;
                                mLookTargetPos.y = mTargetPos.y;
                                mLookTargetPos.z = mTargetPos.z;
                            }
                            *out1 = (void*)&data_020871ac;
                        } else if (b0 == 1) {
                            *out1 = (void*)&data_020871d4;
                        } else if (b0 == 5) {
                            *out1 = (void*)&data_02087404;
                        }
                    }
                }
                mFlags &= ~0x100;
            }
        }
        if (*out1 != (void *)&data_02087094)
            mAreaMode = (CamMode *)*out1;
        if (*out1 != (void*)&data_0208733c) {
            if (*out2 != 0 && *(unsigned char*)(*out2) != 2)
                mViewObj = *out2;
        }
    }

    return res;
}
extern "C" {

int dCamera_c::func_0200c394(int a1, void *a2_, void *a3, void **a5, int *a6) {
    unsigned char *a2 = (unsigned char *)a2_;
    CamMode *p = mMode;
    VecFx32 local, local2;
    signed char g;
    int flags = p->flags;
    int v;
    short sz, sy, sx;

    if ((flags & 0x20) == 0) {
        v = mFlags & 0x100;
        if (v == 0) goto body;
    }
    return 0;
body:
    {
            if (a1 == 7) {
                struct dActor_c *o = mFramedActor;
                if (o == 0) {
                    if (a2 == 0) goto other;
                    if (a2[0] != 3) goto other;
                }
                *a6 = 1;
                o = mFramedActor;
                if (o) {
                    Vector3 *q = &o->mPos;
                    mLookTargetPos.x = q->x;
                    mLookTargetPos.y = q->y;
                    mLookTargetPos.z = q->z;
                } else {
                    VecFx32 t;
                    sx = *(short *)(a2 + 2);
                    sz = *(short *)(a2 + 6);
                    sy = *(short *)(a2 + 4);
                    t.x = sx << 12; t.z = sz << 12; t.y = sy << 12;
                    mLookTargetPos.x = t.x;
                    mLookTargetPos.y = t.y;
                    mLookTargetPos.z = t.z;
                }
                Vec3_Sub((Vector3 *)&local, &mLookTargetPos, (const Vector3 *)a3);
                mFocusDist = LenVec3(&local) >> 1;
                g = gCurrentSublevel;
                v = (g == 0x2d);
                if (v) {
                    if (mFocusDist < 0x100000)
                        mFocusDist = 0x100000;
                    else if (mFocusDist > 0x180000)
                        mFocusDist = 0x180000;
                } else {
                    if (mFocusDist < 0x180000)
                        mFocusDist = 0x180000;
                }
                if (mFlags & 2) {
                    mFocusDist = FX_MUL(mFocusDist, 0x1800);
                }
                if (g == 0x2f && mFocusDist > 0xc0000) {
                    int s = FX_Div(0xc0000, mFocusDist << 1);
                    Vec3_MulScalarInPlace(&local, s);
                    Vec3_Add((Vector3 *)&local2, (const Vector3 *)a3, (const Vector3 *)&local);
                    mLookTargetPos.x = local2.x;
                    mLookTargetPos.y = local2.y;
                    mLookTargetPos.z = local2.z;
                    mFocusDist = 0xc0000;
                } else {
                    AddVec3((VecFx32 *)&mLookTargetPos, a3, (VecFx32 *)&mLookTargetPos);
                    Vec3_AsrInPlace((VecFx32 *)&mLookTargetPos, 1);
                }
                *a5 = &data_02087274;
                return v;
            }

        other:
            if (flags & 4) {
                return (p == (CamMode *)&data_0208706c) ? 0 : 1;
            }
            if (p == (CamMode *)&data_0208715c) {
                if (v) { *a5 = &data_0208747c; }
                else if (a1 == 8) { *a5 = &data_0208715c; }
                else if (a1 == 6) { *a5 = &data_02087184; }
                else if (a1 == 0xa) { *a5 = &data_020871fc; }
                else if (a1 == 0xb) { *a5 = &data_02087224; }
                else if (a1 == 9) { *a5 = &data_0208733c; }
                else if (a1 == 2) { *a5 = &data_02087314; }
            }
            return 1;
    }
    }

}
int dCamera_c::func_0200bec4(int *arg1, int arg2, char *arg3, int arg4) {
    int r5 = arg4;
    VecFx32 sp8;
    VecFx32 sp14;
    VecFx32 sp20;
    VecFx32 sp2C;
    VecFx32 sp38;
    VecFx32 sp44;
    int r4;
    int t;
    int sl;

    sp44.z = ScaleDistByFov(((CamMode *)arg3)->lookAtDist);
    data_02086f2c.z = sp44.z;
    sp44.x = data_02086f2c.x;
    sp44.y = data_02086f2c.y;
    Vec3_RotateYAndTranslate(&sp8, arg1, mTargetPlayer->mAngleY, (int *)&sp44);
    dBgCh_Lin line;
    sp14.x = sp8.x;
    sp14.y = sp8.y + 0x32000;
    sp14.z = sp8.z;
    {
        int tz = arg1[2];
        int ty = arg1[1] + 0x32000;
        int tx = arg1[0];
        sp20.x = tx;
        sp20.y = ty;
        sp20.z = tz;
    }
    PrepareBgCh(line);
    line.SetObjAndLine(*(Vector3 *)&sp20, *(Vector3 *)&sp14, 0);
    if (line.DetectClsn() != 0) {
        Vector3 sp50 = line.GetClsnPos();
        sp8.x = sp50.x;
        sp8.y = sp50.y;
        sp8.z = sp50.z;
    }
    sp2C.x = sp8.x;
    sp2C.y = sp8.y + 0x100000;
    sp2C.z = sp8.z;
    PrepareBgCh(line);
    line.SetObjAndLine(*(Vector3 *)&sp8, *(Vector3 *)&sp2C, 0);
    if (line.DetectClsn() != 0) {
        Vector3 sp5C = line.GetClsnPos();
        sp2C.x = sp5C.x;
        sp2C.y = sp5C.y;
        sp2C.z = sp5C.z;
    }
    dBgCh_Gnd spEC;
    PrepareBgCh(spEC);
    spEC.SetObjAndPos(*(Vector3 *)&sp2C, 0);
    r4 = sp8.y;
    if (spEC.DetectClsn() != 0) {
        int d;
        r4 = spEC.clsnY;
        d = r4 - sp8.y;
        if (d > 0) {
            if (d > 0x30000)
                d = 0x30000;
            sp8.y += d;
        }
    }
    SubVec3(&sp8, arg1, &sp8);
    t = ((CamMode *)arg3)->lookAtHeight;
    if (mFlags & 0x20000) {
        t = FX_MUL(t, 0x3000);
    }
    sp8.y += ScaleHeightByFov(t);
    sl = Vec3_HorzLen(&sp8);
    VecFx32 sp68;
    Vec3_MulScalar((Vector3 *)&sp68, &mLookTargetPos, mFocusBlend);
    AddVec3(&sp8, &sp68, &sp8);
    if (mFlags & 4) {
        mLookAtOffset.x = sp8.x;
        mLookAtOffset.y = sp8.y;
        mLookAtOffset.z = sp8.z;
    } else {
        int hl = Vec3_HorzLen(&mLookAtOffset) - sl;
        if (((CamMode *)arg3)->flags & 0x40) {
            r5 = 0x10000;
        } else if (hl > 0) {
            r5 += FX_MUL(hl, 0x400);
        }
        Vec3_Sub((Vector3 *)&sp38, (const Vector3 *)&sp8, &mLookAtOffset);
        sl = Vec3_HorzLen(&sp38);
        {
            int f = sl;
            int fd;
            if (sl != 0) {
                Math_Function_0203b14c(&f, 0, 0x100, FX_MUL(r5, 0x600), 0x1000);
                fd = FX_Div(f, sl);
                mLookAtOffset.x = sp8.x - FX_MUL(sp38.x, fd);
                mLookAtOffset.z = sp8.z - FX_MUL(sp38.z, fd);
            }
        }
        {
            int a8 = mLookAtOffset.y;
            int diff = sp8.y - a8;
            mLookAtOffset.y = sp8.y - FX_MUL(diff, 0xf80);
        }
        if (mFlags & 0x400) {
            return r4;
        }
    }
    lookAt.x = arg1[0] + mLookAtOffset.x;
    lookAt.z = arg1[2] + mLookAtOffset.z;
    {
        int sl2 = arg1[1] + mLookAtOffset.y;
        int r5b = sl2 - lookAt.y;
        if (r5b < (int)ScaleHeightByFov(((CamMode *)arg3)->followRise)) {
            ApproachLinear(*(int *)&lookAt.y, sl2, 0x64000);
        } else {
            int r6 = arg2 - arg1[1];
            if (r6 >= 0) {
                r6 = 0;
            } else {
                int lim = ScaleHeightByFov(-0xf0a00);
                if (r6 < lim)
                    r6 = lim;
            }
            {
                int r2 = r6 + r5b;
                if (r2 > 0) {
                    lookAt.y = sl2 - (r5b - FX_MUL(r2, 0x600));
                }
            }
        }
    }
    return r4;
}
#pragma cplusplus on

/* ---------------------------------------------------------------------------
 * The camera's states and script ops, 0x020073a0..0x0200bec4.
 *
 * The states run through the State records in the 0x0209b008 table, and the
 * script ops through the member-function table that RunCameraCommand builds from
 * 0x02086bc0. Their names are not recovered. The follow, placement and state
 * helpers among them are dCamera_c members; the rest keep C linkage, but every
 * one takes the camera as its first argument.
 * ------------------------------------------------------------------------- */

/* KuppaScript.cpp's readers of the script's unaligned operands and its
   lookups of the script's actors. */
extern "C" dActor_c *KuppaScript::FindScriptPlayer(u32 character);
extern "C" dActor_c *KuppaScript::FindDemoActor(u32 param1);

extern "C" {
/* local extern: math/CalcLib.h spells the Vec3 helpers with const Vector3 *,
   which this file's VecFx32 callers cannot pass; only these two are taken
   from it. */
extern s32 ApproachAngle(s16 *cur, s32 target, s32 div, s32 band, s32 step);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
/* local extern: math/MtxStack.h declares these; see the matrix helpers above. */
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, short angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angle);

extern int NormalizeVec3IfNonZero(Vector3 *v);
extern Fix12i DotVec3(const Vector3 *a, const Vector3 *b);
/* The 64-bit squared distance between two points. */
extern long long func_0203cf94(int *a, int *b);
extern void func_ov002_020effb8(Vector3 *out, s32 t, Vector3 *p0, Vector3 *p1, Vector3 *p2,
                                Vector3 *p3);
extern void FUN_02029ab0(void);
extern int Sound_PlayIfNotActive(int handle, int bank, int id, int arg);
extern void func_020070e8(dCamera_c *cam, int distTarget, int distStep, short vertTarget,
                          short vertDiv, short horzTarget, short horzDiv);
extern void StarDanceBoundYaw(dCamera_c *cam, short ang, int threshold);

extern dActor_c *STAR_MARKERS[12];
extern Matrix4x3 data_020a0e68;
extern u8 data_0209f224;
/* The stride-0x18 controller records at 0x0209f498, indexed by
   gActivePlayerSlot: +4 and +6 are button words, +8 the camera-input scale,
   +0xa and +0xc the camera axes, +0x16 a mode byte. */
extern u16 data_0209f49c[];
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];
extern s16 data_0209f4a2[];
extern s16 data_0209f4a4[];
extern u8 data_0209f4ae[];
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
extern int data_020870e4;
extern int data_0208710c;
extern int data_0208724c;
extern int data_0208729c;
extern struct CamMode data_020874cc;

/* Siblings defined below. */
int func_02009414(dCamera_c *cam);
int func_020094a8(dCamera_c *cam);
int func_02008b08(dCamera_c *cam);
short func_02008abc(dCamera_c *cam);
short func_02008a70(dCamera_c *cam);
void Vec3fSetDistAndAngle(Vector3 *res, const Vector3 *trans, int mag, short ang, short angY);
void Vec3fGetDistAndAngle(const Vector3 *v0, const Vector3 *v1, int *outDist, short *outVertAng,
                          short *outHorzAng);
void ApproachVec3fAsymptotic(Vector3 *self, const Vector3 *src, int stepX, int stepY, int stepZ);
}

extern "C" {

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
    s32 m;

    mFlags |= 0x1000;

    m = gActivePlayerSlot * 0x18;
    flag4000 = *(u16 *)((char *)data_0209f49c + m) & 0x4000;

    if (flag4000 != 0 || mRecentering != 0) {
        if (*(u16 *)((char *)data_0209f49e + m) & 0x4000) {
            func_02012790(0x1a);
        }
        if (flag4000 != 0) {
            mBehindAngleY = (s16)(mTargetPlayer->mAngleY + 0x8000);
        }
        mRecentering = (ApproachAngle(&mEasedAngleY, mBehindAngleY, 4, 0x400, 0x100) != 0);
        mBaseAngleOffset = (s16)(mEasedAngleY - mBaseAngleY);
        mBaseAngleOffset = (s16)((mBaseAngleOffset + 0x1000) & -0x2000);
        mFlags |= 0x60;
    } else {
        if (view != 0) {
            if (view->type == 2 && view != mViewObj) {
                mBaseAngleY = (s16)(view->angleY + 0x8000);
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
            s32 m2;
            s32 f1;
            /* Here mStateTimer is the delay before a held turn button
               repeats. */
            DecIfAbove0_Short(&mStateTimer);
            m2 = gActivePlayerSlot * 0x18;
            f1 = *(u16 *)((char *)data_0209f49c + m2) & 0x300;
            if (f1 != 0) {
                s32 f2 = *(u16 *)((char *)data_0209f49e + m2) & 0x300;
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
        ApproachLinear(mEasedAngleY, (s16)(mBaseAngleY + mBaseAngleOffset), 0x800);
    }

    tbl = (mFlags & 2) ? data_02086d50 : data_02086cc8;
    sp_speed = tbl[1];
    func_0200c9e0(&sp_speed, &sp_speed);
    comp = FX_MUL(tbl[0], sp_speed);
    vsrc.x = 0;
    vsrc.z = 0;
    vsrc.y = comp;
    {
        s32 t0 = tbl[0];
        vsrc.z = cstd::sqrt((s64)t0 * t0 - (s64)comp * comp);
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
        int sz = (int)obj->z << 12;
        int sy = (int)obj->y << 12;
        int sx = (int)obj->x << 12;
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
        s16 diff = (s16)(angle - target);
        if (diff < 0) {
            angle -= step;
        } else {
            angle += step;
        }
    }

    {
        int fixmul = FX_MUL(arg2, rate);
        ApproachLinear(mAngleY, angle, (s16)fixmul);
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

    len = LenVec3(&v1);
    if (len < 0x220000) {
        v2.y += 0x220000 - len;
    }

    Vec3_Sub(&v3, &v2, &lookAt);
    v1.x = v3.x;
    v1.y = v3.y;
    v1.z = v3.z;
    len = LenVec3(&v1);

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
    newAngle = (short)(angle1 + diff);

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
    struct Vector3 tmp;
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
    int v4;
    int sideScale;

    path.FromID(obj->p1);

    if (mFlags & 4) {
        mPathNode = 0;
    }
    idx = mPathNode + 1;

    path.GetNode(node0, mPathNode);
    path.GetNode(node1, idx);
    Vec3_Sub(&dir, &node1, &node0);
    segLen = LenVec3(&dir);
    NormalizeVec3IfNonZero(&dir);

    Vec3_Sub(&toPos, &lookAt, &node0);
    Vec3_Sub(&toTarget, &pos, &node0);
    func_0203cf00(&cross1, &toPos, &dir);
    func_0203cf00(&side, &dir, &cross1);
    NormalizeVec3IfNonZero(&side);

    projSide = DotVec3(&toPos, &side);
    projDir = DotVec3(&toPos, &dir);
    diff = projDir - DotVec3(&toTarget, &dir);

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
        Vec3_Sub(&tmp, &node2, &node1);
        Vec3_MulScalarInPlace(&tmp, FX_Div(0x190000, LenVec3(&tmp)));
        Vec3_Add(&farPoint, &node1, &tmp);
        Vec3_Sub(&diffFar, &farPoint, &lookAt);
        tmp.x = diffFar.x;
        tmp.y = diffFar.y;
        tmp.z = diffFar.z;

        Vec3_MulScalar(&back, &dir, -0x190000);
        Vec3_Add(&farBack, &node1, &back);
        Vec3_Sub(&diffBack, &farBack, &lookAt);

        lenBack = LenVec3(&diffBack);
        lenFwd = LenVec3(&tmp);
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
        Vec3_MulScalarInPlace(&diffPrev, FX_Div(0x190000, LenVec3(&diffPrev)));
        Vec3_Add(&farPrev, &node0, &diffPrev);
        Vec3_Sub(&diffFarPrev, &farPrev, &lookAt);
        diffPrev.x = diffFarPrev.x;
        diffPrev.y = diffFarPrev.y;
        diffPrev.z = diffFarPrev.z;

        lenA = LenVec3(&diffFwd2);
        lenB = LenVec3(&diffPrev);
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
        v4 = 0;
    } else {
        v4 = path.GetPullFactor() << 4;
    }

    if (gCurrentSublevel == 0x2f) {
        projSide = projSide - FX_MUL(mFocusDist, 0x5000);
        if (projSide < 0) {
            targetPos.y -= FX_MUL(projSide, 0x400);
            projSide = 0;
        }
    } else {
        projSide = FX_MUL(projSide, v4);
    }

    Vec3_MulScalarInPlace(&side, projSide);
    AddVec3(&targetPos, &side, &targetPos);

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
    int idx;
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
    idx = (ang >> 4) * 2;

    v2->y = mTargetPos.y + 0x8f7690;
    v2->x = mTargetPos.x + FX_MUL(FX_SinCosTable_[idx], 0x15a6f80);
    v2->z = mTargetPos.z + FX_MUL(FX_SinCosTable_[idx + 1], 0x15a6f80);

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
    mMode = (CamMode *)&data_0208715c;
    return 1;
}

/* func_02009e70's helpers. */
static inline void SetVec(struct Vector3 &v, s32 x, s32 y, s32 z) {
    v.x = x;
    v.y = y;
    v.z = z;
}

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
    /* Last frame's turn buttons (kf at the bottom): func_02012790(0xe) fires
       only on the frame they go from released to held. */
    static s32 sPrevTurnInput;
    s32 sp04;
    s32 sp08;
    s32 sp0c;
    s32 sp10;
    s32 sp14;
    u16 sp18;
    struct CamMode *sp1c;
    CamViewObj *sp20;
    s32 sp24;
    s32 sp28;
    s32 sp2c;
    s32 sp30;
    struct Vector3 sp34;
    struct Vector3 sp40;
    s32 r7;
    s32 r6;
    s32 sb;
    s32 r4;
    s16 sba;
    s32 r5;
    s32 fp;
    u32 r7h;
    struct CamMode *m;
    s32 t0;
    s32 kf;
    s16 diff;
    s32 ceil;

    if (mTargetPlayer == 0) {
        mAngleY = Vec3_HorzAngle(&lookAt, &pos);
    } else {
        sp1c = (mFlags & 0x100) ? (CamMode *)&data_0208747c : mMode;
        Vec3_Sub(&sp34, &mTargetPlayer->mPos, &mTargetPos);
        {
            struct Vector3 *q = &mTargetPlayer->mPos;
            mTargetPos.x = q->x;
            mTargetPos.y = q->y;
            mTargetPos.z = q->z;
        }
        r7 = Vec3_HorzLen(&sp34);
        sp20 = 0;
        sp24 = 0;
        if (mFocusActor != 0) {
            struct Vector3 *q;
            sp24 = 1;
            mFramedActor = 0;
            q = &mFocusActor->mPos;
            mLookTargetPos.x = q->x;
            mLookTargetPos.y = q->y;
            mLookTargetPos.z = q->z;
            mFocusDist = 0xdfe60;
        }
        r6 = func_0200c66c(&mTargetPos, (int *)&sp20, (int *)&sp1c, &sp24);
        r4 = 0;
        if (sp24 != 0) {
            r4 = 0x1000;
            SubVec3(&mLookTargetPos, &mTargetPos, &mLookTargetPos);
        }
        Math_Function_0203b14c(&mFocusBlend, r4, 0x100, 0x80, 0x10);

        fp = sp1c->flags;
        r4 = sp1c->minDist;
        r5 = sp1c->posDist;
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
        if (sp24 != 0) {
            r4 = fp;
        } else {
            if (r5 < 0)
                r5 = -r5;
            r4 = ScaleDistByFov(r5);
        }
        sp04 = func_0200bec4(&mTargetPos.x, r6, (char *)sp1c, r7);
        kf = *(u16 *)((char *)data_0209f49e + gActivePlayerSlot * 0x18) & 0x4300;
        if (mFocusActor != 0) {
            mAngleY = Vec3_HorzAngle(&lookAt, &pos);
        } else if (sp1c == (CamMode *)&data_0208733c) {
            Mode8DirectionsCamera(sp20);
            kf = 0;
        } else if (sp1c == (CamMode *)&data_020873dc) {
            UpdateFixedCamera(sp20, r6);
        } else if ((mFlags & 0x400) == 0) {
            if (sp1c == (CamMode *)&data_02087404) {
                UpdateSpiralStairsCamera(sp20, r6);
            } else if (sp1c == (CamMode *)&data_0208742c) {
                UpdateParallelTrackingCamera(sp20, r6);
            } else {
                mFlags |= 0x1000;
                sp08 = ScaleDistByFov(sp1c->nearDist);
                sp0c = ScaleDistByFov(sp1c->farDist);
                if (sp20 != 0 && sp20->type == 2) {
                    if (sp20->p1 == 0) {
                        s32 tx = sp20->x << 12;
                        s32 tz = sp20->z << 12;
                        s32 ty = sp20->y << 12;
                        pos.x = tx;
                        pos.y = ty;
                        pos.z = tz;
                    } else {
                        s16 a = (s16)(sp20->angleY + 0x8000);
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
                Vec3_Sub(&sp40, &pos, &lookAt);
                sp28 = LenVec3(&sp40);
                {
                    sp10 = 0x80000000;
                    Vector3 sp4c(mTargetPos.x, mTargetPos.y + 0x3e8000, mTargetPos.z);
                    Vector3 sp58;
                    Vector3 sp64;
                    ceil = 0x7fffffff;
                    sb = mTargetPos.y + 0x82500;
                    {
                        dBgCh_Lin line;
                        PrepareBgCh(line);
                        line.SetObjAndLine(mTargetPos, sp4c, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = CollisionCeiling(line, sb);
                        }
                        if ((mFlags & 0x100) == 0 && (sp1c->flags & 8) != 0) {
                            {
                                s32 tz = mTargetPos.z;
                                s32 tx = mTargetPos.x;
                                sp58.x = tx;
                                sp58.y = sb;
                                sp58.z = tz;
                            }
                            if (sb <= ceil) {
                                line.SetObjAndLine(sp58, pos, 0);
                                if (line.DetectClsn() != 0) {
                                    line.surface.CopyNormalTo(sp64);
                                    SetCollisionPosition(this, line);
                                    mWallNormal.x = sp64.x;
                                    mWallNormal.y = sp64.y;
                                    mWallNormal.z = sp64.z;
                                    t0 = sp64.y;
                                    if (t0 < 0)
                                        t0 = -t0;
                                    if (t0 > 0xb50) {
                                        mFlags |= 0x80200;
                                    } else {
                                        Vector3 sp70 = line.GetClsnPos();
                                        sba = cstd::atan2(sp64.x, sp64.z) + 0x8000;
                                        diff = sba - mAngleY;
                                        sba += (diff < 0) ? 0x4000 : -0x4000;
                                        if (mTurningToTarget != 0) {
                                            if (AngleDiff(sba, mTargetAngleY) >= 0x4000)
                                                sba = mTargetAngleY;
                                        }
                                        mTargetAngleY = sba;
                                        mTurningToTarget = 1;
                                        t0 = (s16)(mAngleY - mTargetAngleY);
                                        if (kf & 0x200)
                                            t0 = (s16)(-t0);
                                        if (t0 > 0)
                                            kf = 0;
                                        mFlags |= 0x40000;
                                    }
                                }
                            }
                        }
                        Vector3 sp7c;
                        Vector3 sp88;
                        if (kf != 0) {
                            if (kf & 0x4000)
                                func_02012790(0x1a);
                            mTurningToTarget = 0;
                        } else {
                            if (mTurningToTarget == 0) {
                                kf = *(u16 *)((char *)data_0209f49c + gActivePlayerSlot * 0x18) &
                                     0x4300;
                            }
                        }
                        if (mFlags & 0x100) {
                            if (sp28 > (s32)ScaleDistByFov(0xdfe60))
                                mFlags &= ~0x100;
                        }
                        /* sb = sp28 is assigned inside the test: hoisting it above the if
                           re-lays the function (290 words differ). */
                        if (kf != 0 || mRecentering != 0 || mTurningToTarget != 0 || r7 <= sp08 ||
                            (sb = sp28) < fp || sb <= sp0c) {
                            sb = mAngleY;
                            mAngleY = Vec3_HorzAngle(&lookAt, &pos);
                            if (kf != 0 || mRecentering != 0) {
                                if ((kf & 0x4000) || mRecentering != 0) {
                                    if (kf != 0) {
                                        mBehindAngleY = mTargetPlayer->mAngleY + 0x8000;
                                    }
                                    mRecentering = ApproachAngle(&mAngleY, mBehindAngleY, 4, 0x800,
                                                                 0x400) != 0;
                                    if (mRecentering != 0)
                                        mFlags |= 0x60;
                                } else {
                                    if (kf & 0x200) {
                                        mAngleY -= 0x400;
                                        mFlags |= 0x20;
                                    } else {
                                        mAngleY += 0x400;
                                        mFlags |= 0x40;
                                    }
                                }
                                kf = 0;
                            } else if (mTurningToTarget != 0) {
                                if (ApproachAngle(&mAngleY, mTargetAngleY, 0x10, 0x200, 0x100) == 0)
                                    mTurningToTarget = 0;
                            } else {
                                m = sp1c;
                                if (m == (CamMode *)&data_020871ac ||
                                    m == (CamMode *)&data_020871d4) {
                                    UpdateRadialCamera(sp20, r7);
                                } else if (sp24 == 0) {
                                    if (m == (CamMode *)&data_0208724c)
                                        t0 = mTargetPlayer->mPrevAngleY;
                                    else
                                        t0 = mTargetPlayer->mAngleY;
                                    t0 = (s16)(t0 + 0x8000);
                                    if (m == (CamMode *)&data_020874cc) {
                                        mAngleY = t0 + mBaseAngleOffset;
                                    } else if (sp28 <= sp0c) {
                                        ApproachAngle(&mAngleY, t0, 0x14, 0x600, 0x28);
                                    } else if (m->flags & 0x100) {
                                        mAngleY = sb;
                                    } else if (r7 <= sp08) {
                                        ApproachAngle(&mAngleY, t0, 0x14, 0x64, 0x28);
                                    }
                                }
                            }

                            sp14 = sp28;
                            if (sp28 > r4) {
                                if (r7 != 0 && sp1c == (CamMode *)&data_02086fcc) {
                                    sb = cstd::atan2(sp34.y, Vec3_HorzLen(&sp34));
                                    if (sb < -0x3f80)
                                        sb = -0x3f80;
                                    else if (sb > 0x3f80)
                                        sb = 0x3f80;
                                    fp = Vec3_HorzLen(&sp40);
                                    sp18 = cstd::atan2(-sp40.y, fp);
                                    ApproachAngle((s16 *)&sp18, sb, 0x20, 0x800, 0);
                                    t0 = (sp18 >> 4) * 2;
                                    sp40.y = FX_MUL(sp28, -FX_SinCosTable_[t0]);
                                    t0 = FX_Div(FX_MUL(sp28, FX_SinCosTable_[t0 + 1]), fp);
                                    sp40.x = FX_MUL(sp40.x, t0);
                                    sp40.z = FX_MUL(sp40.z, t0);
                                }
                                Math_Function_0203b14c(&sp28, r4, 0x400, r7 + 0x20000, 0x100);
                                Vec3_MulScalarInPlace(&sp40, FX_Div(sp28, sp14));
                            } else if (sp28 < fp) {
                                Math_Function_0203b14c(&sp28, fp, 0x400, r7 + 0x20000, 0x100);
                                {
                                    s64 distanceSq = sp28;
                                    distanceSq *= sp28;
                                    distanceSq -= (s64)sp40.y * sp40.y;
                                    r7 = cstd::sqrt((u64)distanceSq);
                                }
                                t0 = FX_Div(r7, Vec3_HorzLen(&sp40));
                                sp40.x = FX_MUL(sp40.x, t0);
                                sp40.z = FX_MUL(sp40.z, t0);
                                mInputYawLag -= mAngleY - sb;
                            }
                            t0 = Vec3_HorzLen(&sp40);
                            if (t0 < 0x1000)
                                t0 = 0x1000;
                            sp40.z = t0;
                            sp40.x = 0;
                            Vec3_RotateYAndTranslate(&pos, &lookAt, mAngleY, &sp40);
                        } else {
                            if (sb > r4) {
                                Math_Function_0203b14c(&sp28, r4, 0x400, r7 + 0x20000, 0x100);
                                Vec3_MulScalarInPlace(&sp40, FX_Div(sp28, sb));
                                MoveCameraPosition(this, sp40);
                            }
                        }
                        if (KeepPosOutOfWalls() != 0) {
                            if (mFlags & 0x60) {
                                kf = 1;
                                mFlags &= ~0x60;
                            }
                        }
                        SetVec(sp4c, pos.x, pos.y + 0x100000, pos.z);
                        line.SetObjAndLine(pos, sp4c, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = LimitCameraCeiling(line, sp4c, ceil);
                        }
                        Vec3_Add(&sp7c, &pos, &lookAt);
                        Vec3_AsrInPlace(&sp7c, 1);
                        sp88.x = sp7c.x;
                        sp88.y = sp7c.y + 0x100000;
                        sp88.z = sp7c.z;
                        line.SetObjAndLine(sp7c, sp88, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = LimitMidpointCeiling(line, sp88, ceil);
                        }
                        s32 y = mTargetPos.y;
                        dBgCh_Gnd ground;
                        ground.mProbeHeight = 0x5dc000;
                        PrepareBgCh(ground);
                        ground.SetObjAndPos(sp4c, 0);
                        if (sp1c > (CamMode *)&data_02086fcc)
                            ground.StartDetectingWater();
                        sb = (ground.DetectClsn() != 0) ? ground.clsnY : y;
                        if (sp1c == (CamMode *)&data_02086ff4) {
                            if (sb < data_0209f32c)
                                sb = data_0209f32c;
                            r7h = sb + 0x64000;
                        } else {
                            r7h = sb;
                            if (sb < r6)
                                r7h = r6;
                            r7h = r7h + ScaleHeightByFov((r6 - sp04) +
                                                         ((mFlags & 2) ? 0x1e8000 : 0x138000));
                            if (sp1c->flags & 0x80) {
                                if (pos.y > (s32)r7h)
                                    r7h = pos.y;
                            }
                            sp2c = sp1c->minRiseSlope;
                            sp30 = sp1c->maxRiseSlope;
                            func_0200c9e0(&sp2c, &sp30);
                            sp40.y = r7h - lookAt.y;
                            if (sp1c == (CamMode *)&data_0208706c && sp34.y < 0)
                                t0 = 0xa49;
                            else
                                t0 = sp2c;
                            r6 = FX_MUL(sp28, t0);
                            t0 = FX_MUL(sp28, sp30);
                            if (sp40.y < r6)
                                r7h = lookAt.y + r6;
                            else if (sp40.y > t0)
                                r7h = lookAt.y + t0;
                            if (gCurrentSublevel == 0x26) {
                                if (pos.y < sb + 0x25bc8)
                                    pos.y = sb + 0x25bc8;
                            }
                            if (ceil < sb + 0x25bc8)
                                ceil = sp4c.y;
                            if (sb + 0x25bc8 > (s32)0x80000000)
                                sp10 = sb + 0x25bc8;
                        }
                        if (sp1c != (CamMode *)&data_02087094 &&
                            sp1c != (CamMode *)&data_020870e4 &&
                            sp1c != (CamMode *)&data_0208710c) {
                            if ((s32)r7h < sp10)
                                r7h = sp10;
                            else if ((s32)r7h > ceil)
                                r7h = ceil;
                        }
                        Math_Function_0203b14c(&pos.y, r7h, 0x300, 0x14000, 0x100);
                        mAngleY = Vec3_HorzAngle(&lookAt, &pos);
                    }
                }
            }
        }
        if (kf != 0) {
            if (sPrevTurnInput == 0)
                func_02012790(0xe);
            sPrevTurnInput = kf;
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
    (void)&dummy;
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
    int one = 1;

    mMode = (CamMode *)&data_0208729c;

    mTurningToTarget = (unsigned char)one;

    {
        dActor_c *player = mTargetPlayer;
        short h = player->mAngleY;
        mTargetAngleY = (short)(h + 0x8000);
        mFlags &= ~0x100u;
    }

    return one;
}

/* The behind-the-player state: while mTurningToTarget is set, carry lookAt
   and pos along with the player and turn to data_02086e90 behind it;
   afterwards follow its facing, with the stylus axes adding up to 0x2000 of
   yaw (the rest turns the player) and 0x1c00 of pitch. */
int func_02009aa8(dCamera_c *cam) {
    s32 tmp[3];
    Vector3 delta;
    Vector3 src;

    src.x = data_02086e90[0];
    src.y = data_02086e90[1];
    src.z = data_02086e90[2];

    Vec3_RotateYAndTranslate(tmp, &cam->mTargetPlayer->mPos, cam->mTargetPlayer->mAngleY, &src);

    if (cam->mTurningToTarget != 0) {
        Vector3 *base;
        int r;

        Vec3_Sub(&delta, &cam->mTargetPlayer->mPos, &cam->mTargetPos);
        AddVec3(&cam->lookAt, &delta, &cam->lookAt);
        AddVec3(&cam->pos, &delta, &cam->pos);

        base = &cam->mTargetPlayer->mPos;
        cam->mTargetPos.x = base->x;
        cam->mTargetPos.y = base->y;
        cam->mTargetPos.z = base->z;

        r = cam->ApproachLookAtAndAngles((Vector3 *)tmp, cam->mTargetAngleY, 0);
        if (r != 0)
            cam->mTurningToTarget = 0;

        cam->mLookAtOffset.x = 0;
        cam->mLookAtOffset.z = 0;
    } else {
        s16 newAngle;
        int idx;
        s16 t1, t2;
        s16 rem;
        int diff2;
        int clamped;
        s16 v17e;

        newAngle = cam->mTargetPlayer->mAngleY + 0x8000;
        cam->mAngleY += newAngle - cam->mTargetAngleY;
        cam->mTargetAngleY = newAngle;

        cam->lookAt.x = tmp[0];
        cam->lookAt.y = tmp[1];
        cam->lookAt.z = tmp[2];

        idx = gActivePlayerSlot * 0x18;
        t1 = *(s16 *)((char *)data_0209f4a2 + idx);
        cam->mAngleY -= FX_MUL(t1, 0x200);

        diff2 = (s16)(cam->mAngleY - newAngle);
        if (diff2 < -0x2000) {
            clamped = -0x2000;
        } else {
            if (diff2 > 0x2000) {
                clamped = 0x2000;
            } else {
                clamped = diff2;
            }
        }
        rem = diff2 - clamped;

        cam->mTargetPlayer->mAngleY += rem;
        cam->mTargetAngleY += rem;

        t2 = *(s16 *)((char *)data_0209f4a4 + idx);
        cam->mAngleX += FX_MUL(t2, 0x200);

        v17e = cam->mAngleX;
        if (v17e < -0x1c00) {
            v17e = -0x1c00;
        } else if (v17e > 0x1c00) {
            v17e = 0x1c00;
        }
        cam->mAngleX = v17e;
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
        int idx = gActivePlayerSlot;
        int off = idx * 0x18;
        s16 t1 = *(short *)((char *)data_0209f4a2 + off);
        int scaled1 = FX_MUL(t1, 0x200);

        mAngleY -= scaled1;

        {
            s16 a186 = mTargetAngleY;
            s16 diff = (s16)(mAngleY - a186);
            if (diff < -0x5000)
                diff = -0x5000;
            else if (diff > 0x5000)
                diff = 0x5000;
            mAngleY = (s16)(a186 + diff);
        }

        {
            s16 t2 = *(short *)((char *)data_0209f4a4 + off);
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
        unsigned int r = ScaleDistByFov(0x48000);
        ApproachDist((int)r);
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
    int off;
    int r7;
    short old186, old17e;

    FUN_02029ab0();
    func_02008b08(cam);

    off = gActivePlayerSlot * 0x18;
    if (*(short *)((char *)data_0209f4a0 + off) != 0) {
        r7 = (data_0209f4ae[off] == 2) ? 0x240 : 0x200;
        old186 = cam->mTargetAngleY;
        {
            int a = func_02008abc(cam);
            cam->mTargetAngleY -= FX_MUL(r7, a);
        }
        {
            int t = cam->mTargetAngleY;
            if (t < -0x4000)
                t = -0x4000;
            else if (t > 0x4000)
                t = 0x4000;
            cam->mTargetAngleY = t;
        }
        cam->mAngleY = cam->mBehindAngleY + cam->mTargetAngleY;
        old17e = cam->mAngleX;
        cam->mAngleX += FX_MUL(r7, (long long)func_02008a70(cam));
        {
            int t = cam->mAngleX;
            if (t < 0)
                t = 0;
            else if (t > 0x38e3)
                t = 0x38e3;
            cam->mAngleX = t;
        }
        if (cam->mTargetAngleY != old186 || cam->mAngleX != old17e) {
            cam->mTurnSound = Sound_PlayIfNotActive(cam->mTurnSound, 3, 0x181, 0);
        }
    }

    v.x = 0;
    v.y = 0;
    v.z = -0x280000;
    Matrix4x3_FromTranslation(&data_020a0e68, cam->pos.x, cam->pos.y, cam->pos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, cam->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, cam->mAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &cam->lookAt);
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
    int r4 = 1;
    CutsceneSpline *tbl1 = cam->mPosPath;

    if (tbl1 != 0) {
        r4 = MovePointAlongSpline(&cam->pos, &cam->mPathIndex, &cam->mPathProgress, tbl1);
    }

    CutsceneSpline *tbl2 = cam->mLookAtPath;
    if (tbl2 != 0) {
        if (MovePointAlongSpline(&cam->lookAt, &cam->mPathIndex, &cam->mPathProgress, tbl2) == 0) {
            r4 = 0;
        }
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    cam->mAngleX = Vec3_VertAngle(&cam->lookAt, &cam->pos);

    cam->UpdateTargetPos();
    return r4;
}

/* Scale a horizontal distance by the field of view (mInvSinFov). */
unsigned int dCamera_c::ScaleDistByFov(int x) {
    long long prod = (long long)x * (long long)mInvSinFov;
    long long sum = prod + 0x800LL;
    unsigned int lo = (unsigned int)sum;
    unsigned int hi = (unsigned int)(sum >> 32);
    return (lo >> 12) | (hi << 20);
}

/* Scale a height by the field of view (mInvCosFov). */
unsigned int dCamera_c::ScaleHeightByFov(int a) {
    unsigned long long prod = (unsigned long long)a * (unsigned long long)mInvCosFov;
    unsigned long long sum = prod + 0x800u;
    unsigned int lo = (unsigned int)sum;
    unsigned int hi = (unsigned int)(sum >> 32);
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
    struct Vector3 d;
    int len;
    int r5;
    int scratch;
    Vec3_Sub(&d, out, target);
    len = LenVec3(&d);
    if (len == 0)
        return 1;
    scratch = len;
    r5 = Math_Function_0203b14c(&scratch, 0, 0x200, 0x40000, 0x8000);
    Vec3_MulScalarInPlace(&d, FX_Div(scratch, len));
    {
        struct Vector3 res;
        Vec3_Add(&res, target, &d);
        out->x = res.x;
        out->y = res.y;
        out->z = res.z;
    }
    return r5 == 0 ? 1 : 0;
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
    int r4;
    int t;
    r4 = (func_020092c4(this, &lookAt, target) != 1);
    UpdateAngles();
    t = ApproachAngle(&mAngleY, angleY, 8, 0x4000, 0x80);
    if (t != 0)
        r4 = 1;
    t = ApproachAngle(&mAngleX, angleX, 8, 0x4000, 0x80);
    return (r4 | t) == 0;
}

/* Ease the lookAt-to-pos distance toward dist and rebuild pos from lookAt
   and the two angles. Returns 1 on arrival. */
int dCamera_c::ApproachDist(int dist) {
    struct Vector3 v;
    int len;
    int r;
    Vec3_Sub(&v, &pos, &lookAt);
    len = LenVec3(&v);
    r = Math_Function_0203b14c(&len, dist, 0x300, 0x60000, 0x200);
    v.x = 0;
    v.y = 0;
    v.z = len;
    Matrix4x3_FromTranslation(&data_020a0e68, lookAt.x, lookAt.y, lookAt.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &pos);
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
        len = LenVec3(&dir1);
        if (len != 0) {
            Vec3_MulScalarInPlace(&dir1, FX_Div(0x100000, len));
            Vec3_Add(&end1, &pos, &dir1);
            dBgCh_Lin rc1;
            PrepareBgCh(rc1);
            rc1.SetObjAndLine(end1, v0, 0);
            if (rc1.DetectClsn() != 0) {
                rc1.surface.CopyNormalTo(n1);
                if (DotVec3(&mWallNormal, &n1) > 0) {
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
    len = LenVec3(&dir2);
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
    if (LenVec3(&diff) >= d) {
        Vec3_MulScalarInPlace(&diff, FX_Div(d, LenVec3(&diff)));
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
    short *p1 = segment;
    int *p2 = progress;
    int idx;
    CutsceneSplinePoint *elem;
    CutsceneSpline *path = spline;
    int i;
    VecFx32 v[4];
    int lo, hi;
    int m;

    idx = *p1;
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

    func_ov002_020effb8(pos, *p2, (Vector3 *)&v[0], (Vector3 *)&v[1], (Vector3 *)&v[2],
                        (Vector3 *)&v[3]);

    lo = 0;
    hi = 0;
    if (elem[1].speed != 0) {
        lo = FX_Inv(elem[1].speed << 12);
    }
    if (elem[2].speed != 0) {
        hi = FX_Inv(elem[2].speed << 12);
    }

    {
        int d = hi - lo;
        m = *p2;
        *p2 = lo + FX_MUL(d, m) + m;
    }

    if (*p2 >= 0x1000) {
        *p1 = *p1 + 1;
        *p2 -= 0x1000;
    }

    return 0;
}

/* Ease mInputScale toward the active player's camera-input entry: slowly
   while it is nonzero, quickly back to 0. */
int func_02008b08(dCamera_c *cam) {
    s16 dest = *(s16 *)((char *)data_0209f4a0 + gActivePlayerSlot * 0x18);
    return ApproachLinear2(cam->mInputScale, dest, dest != 0 ? 0xa0 : 0x400);
}

/* The camera's horizontal input axis, scaled by mInputScale. */
short func_02008abc(dCamera_c *cam) {
    unsigned char idx = *(unsigned char *)&gActivePlayerSlot;
    short tbl = *(short *)((char *)data_0209f4a2 + idx * 0x18);
    short val = cam->mInputScale;
    long long prod = (long long)tbl * (long long)val;
    prod += 0x800LL;
    int tmp = (int)(prod >> 12);
    return (short)tmp;
}

/* The camera's vertical input axis, scaled by mInputScale. */
short func_02008a70(dCamera_c *cam) {
    unsigned char idx = *(unsigned char *)&gActivePlayerSlot;
    short tbl = *(short *)((char *)data_0209f4a4 + idx * 0x18);
    short val = cam->mInputScale;
    long long prod = (long long)tbl * (long long)val;
    prod += 0x800LL;
    int tmp = (int)(prod >> 12);
    return (short)tmp;
}

/* Copy the target player's position into mTargetPos and keep lookAt's
   offset from it in mLookAtOffset. */
void dCamera_c::UpdateTargetPos() {
    dActor_c *player;
    Vector3 *p;
    int tmp[3];

    player = mTargetPlayer;
    if (player == 0) {
        return;
    }
    p = &player->mPos;
    mTargetPos.x = p->x;
    mTargetPos.y = p->y;
    mTargetPos.z = p->z;
    Vec3_Sub((Vector3 *)tmp, (const Vector3 *)&lookAt.x, (const Vector3 *)&mTargetPos.x);
    mLookAtOffset.x = tmp[0];
    mLookAtOffset.y = tmp[1];
    mLookAtOffset.z = tmp[2];
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
}

/* The script ops, called through RunCameraCommand's member-function table
   with the command's operands. The class is opaque: only the pointer-to-
   member representation matters. */
typedef int (CamPmfHost::*CamScriptOp)(unsigned char *, int, int);

extern "C" {
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
    Fix12i x, y, z;
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
    Fix12i slope = FX_Div((gKuppaScriptFrame - lo) << 12, (hi - lo) << 12);
    cam->mViewport[0] = (u8)(FX_MUL(((int)src[0] << 12), slope) >> 12);
    cam->mViewport[1] = (u8)(FX_MUL(((int)src[1] << 12), slope) >> 12);
    cam->mViewport[2] = (u8)((FX_MUL((((int)src[2] - 0xff) << 12), slope) >> 12) + 0xff);
    cam->mViewport[3] = (u8)((FX_MUL((((int)src[3] - 0xbf) << 12), slope) >> 12) + 0xbf);
    cam->mAspect = FX_Div((((u8)cam->mViewport[2] - (u8)cam->mViewport[0]) + 1) << 12,
                          (((u8)cam->mViewport[3] - (u8)cam->mViewport[1]) + 1) << 12);
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
    dActor_c *tmp = KuppaScript::FindDemoActor(0x13);
    Vector3 *r4 = &tmp->mPos;
    int v1 = r4->y;
    Math_Function_0203b0fc(&cam->pos.y, v1 + 0x23000, 0x51, 0x7fffffff);
    int v2 = r4->y;
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
    Fix12i y;
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
    int *pp, *rp;
    Fix12i px, py, pz;
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
    pp = pos;
    rp = rot;
    Math_Function_0203b0fc(&cam->lookAt.x, pp[0], rp[0], 0x7fffffff);
    pp++;
    rp++;
    Math_Function_0203b0fc(&cam->lookAt.y, pp[0], rp[0], 0x7fffffff);
    pp++;
    rp++;
    Math_Function_0203b0fc(&cam->lookAt.z, pp[0], rp[0], 0x7fffffff);
    return 1;
}

/* Script op: the same for pos. */
int func_02007e5c(dCamera_c *cam, u8 *data) {
    int pos[3];
    int rot[3];
    int *pp, *rp;
    Fix12i px, py, pz;
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
    pp = pos;
    rp = rot;
    Math_Function_0203b0fc(&cam->pos.x, pp[0], rp[0], 0x7fffffff);
    pp++;
    rp++;
    Math_Function_0203b0fc(&cam->pos.y, pp[0], rp[0], 0x7fffffff);
    pp++;
    rp++;
    Math_Function_0203b0fc(&cam->pos.z, pp[0], rp[0], 0x7fffffff);
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
        cam->mOrbitTargetYaw = Vec3_HorzAngle(p, &cam->pos);
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
int func_02007cec(void *dest, const Vector3 *src, int step) {
    ApproachVec3fAsymptotic((Vector3 *)dest, src, step, step, step);
}

/* Super Mario 64's vec3f_get_dist_and_angle: the distance and the pitch and
   yaw from v0 to v1. */
void Vec3fGetDistAndAngle(const Vector3 *v0, const Vector3 *v1, int *outDist, short *outVertAng,
                          short *outHorzAng) {
    *outDist = Vec3_Dist(v0, v1);
    *outVertAng = Vec3_VertAngle(v1, v0);
    *outHorzAng = Vec3_HorzAngle(v0, v1);
}

/* Super Mario 64's vec3f_set_dist_and_angle, the inverse: res is trans plus
   mag along pitch ang, turned by yaw angY. The N64 takes the origin first;
   this takes the result first. */
void Vec3fSetDistAndAngle(Vector3 *res, const Vector3 *trans, Fix12i mag, short ang, short angY) {
    Vector3 v;
    unsigned idx = (unsigned short)ang >> 4;

    v.x = 0;
    v.y = FX_MUL(mag, FX_SinCosTable_[idx * 2]);
    v.z = FX_MUL(mag, FX_SinCosTable_[idx * 2 + 1]);
    Vec3_RotateYAndTranslate(res, trans, angY, &v);
}

/* Script op: step lookAt toward the target player's position plus a
   whole-unit offset. */
int func_02007b98(dCamera_c *cam, u8 *data) {
    struct Vector3 v;
    Fix12i x, y, z;
    int b;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    v.x = x;
    v.y = y;
    v.z = z;
    b = data[6] << 4;
    AddVec3(&v, &cam->mTargetPlayer->mPos, &v);
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
    Fix12i vx, vy, vz;
    vz = (Fix12i)ReadUnalignedShort(rec + 4) << 12;
    vy = (Fix12i)ReadUnalignedShort(rec + 2) << 12;
    vx = (Fix12i)ReadUnalignedShort(rec + 0) << 12;
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
    Fix12i a = ReadUnalignedShort(data) << 12;
    int b = data[2] << 4;
    short c = ReadUnalignedShort(data + 3);
    short d = data[5];
    short e = ReadUnalignedShort(data + 6);
    if (e == -1)
        e = cam->mOrbitTargetYaw;
    func_020070e8(cam, a, b, c, d, e, data[8]);
    return 1;
}

/* Script op, Super Mario 64's cutscene_pan_cvar9 with the pitch and yaw steps
   (-29 and 29 there) as operands: orbit lookAt around pos. On the first frame
   savedLookAt takes lookAt; every frame the two offsets accumulate and are
   added to the angles of the pos -> savedLookAt line before lookAt is rebuilt
   from it. */
int CutscenePanCvar9(dCamera_c *cam, u8 *p, int tag) {
    int dist;
    short vert, horz;
    if (tag == gKuppaScriptFrame) {
        cam->savedLookAt.x = cam->lookAt.x;
        cam->savedLookAt.y = cam->lookAt.y;
        cam->savedLookAt.z = cam->lookAt.z;
    }
    cam->mPitchOffset -= ReadUnalignedShort(p);
    cam->mYawOffset += ReadUnalignedShort(p + 2);
    Vec3fGetDistAndAngle(&cam->pos, &cam->savedLookAt, &dist, &vert, &horz);
    vert = (short)(vert + cam->mPitchOffset);
    horz = (short)(horz + cam->mYawOffset);
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
    vert = (short)(ReadUnalignedShort(p + 4) + vert);
    horz = (short)(ReadUnalignedShort(p + 6) + horz);
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
    int v1 = (int)ReadUnalignedShort(data);
    int v2 = (int)ReadUnalignedShort(data + 2);
    int v3 = (int)ReadUnalignedShort(data + 4);
    cam->SetFovShake(v1, v2, v3);
    return 1;
}

/* Script op, Super Mario 64's cutscene_dance_closeup_start: the per-level orbit
   yaws (StarDanceBoundYaw). */
int CutsceneDanceCloseupStart(dCamera_c *cam) {
    dCamera_c *r4 = cam;
    int lvl = SublevelToLevel(gCurrentSublevel);

    if (lvl == 2 && data_0209f224 == 5) {
        StarDanceBoundYaw(r4, 0, 0x4000);
    } else if (lvl == 8 && data_0209f224 == 1) {
        StarDanceBoundYaw(r4, (short)0x8000, 0x5000);
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

    vz = (Fix12i)ReadUnalignedShort(rec + 4) << 12;
    vy = (Fix12i)ReadUnalignedShort(rec + 2) << 12;
    vx = (Fix12i)ReadUnalignedShort(rec + 0) << 12;
    v.x = vx;
    v.y = vy;
    v.z = vz;

    wz = (Fix12i)ReadUnalignedShort(rec + 0xa) << 12;
    wy = (Fix12i)ReadUnalignedShort(rec + 8) << 12;
    wx = (Fix12i)ReadUnalignedShort(rec + 6) << 12;
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
    int stack[3];
    stack[0] = data_02086ec0[0];
    stack[1] = data_02086ec0[1];
    stack[2] = data_02086ec0[2];

    short angle = cam->mTargetPlayer->mAngleY;
    angle = (short)(angle + 0x8000);

    Vec3_RotateYAndTranslate(&cam->savedLookAt, &cam->mTargetPlayer->mPos, angle, stack);

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
    func_020070e8(cam, dist, 0x199, vertAng, 0x20, (short)(horzAng + 0x1200), 0x14);
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
    int tmp = cam->mTargetPlayer->mAngleY;
    tmp += 0x8000;
    cam->mTargetAngleY = (short)tmp;
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
    int stackbuf[3];
    stackbuf[0] = data_02086ed8[0];
    stackbuf[1] = data_02086ed8[1];
    stackbuf[2] = data_02086ed8[2];

    func_02007cec(&cam->savedLookAt, (Vector3 *)stackbuf, 0xcc);
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
}

/* Nineteen camera states, definition order is retail __sinit order. Each
   record is two CamPmf (enter at +0, execute at +8), plain-copied from the
   .data member-function constants. mwcc emits __sinit_d_camera.cpp from
   these, which was the handwritten __sinit_02073a24. */
struct dCamera_c::State {
    CamPmf enter;
    CamPmf exec;
};

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
