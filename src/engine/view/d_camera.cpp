//cpp
/* dCamera_c, the main camera, with its states and script ops.
 * arm9 .text 0x020073a0..0x0200e494.
 *
 * _ZTS9dCamera_c is at 0x02086e6c, _ZTI9dCamera_c at 0x02086ee4,
 * _ZTV9dCamera_c at 0x02086f84. ChangeState's mangling names the
 * nested State. The D1/D0 pair is its own TU at 0x0200705c
 * (src/engine/view/d_camera_dtor.cpp), outside this run.
 *
 * Below 0x0200bec4 sit the follow modes, states and script ops
 * (func_020073a0..func_0200bb28). They share this file's .data block
 * (0x02086bc0..0x0208747c: the script-op member-function constants, the
 * mode records and the offset triples) and the controller records at
 * 0x0209f498 with func_0200bec4 and up.
 *
 * leftovers:
 * - The func_0200* handlers retain their existing C linkage. The three
 *   line-query handlers use C++ for native collision-result initialization,
 *   and the state-change, shake and FOV handlers use C++ so they can call
 *   ChangeState and cstd::fdiv directly; other handlers remain in
 *   cplusplus-off regions.
 * - Vec3 remains the POD scratch triple in the older handlers. Native
 *   collision results use Vector3 and its empty destructor.
 * - The state records in the 0x0209b008 table stay int. These bodies
 *   only take their address.
 * - CleanupResources keeps both null checks. The second is the
 *   inlined deleting destructor.
 * - func_020072c4, the script op just below func_020073a0, stays a shard:
 *   it reads no data, so nothing but call order ties it to this run.
 * - The controller records at 0x0209f498 are read through the separate
 *   data_0209f49c..data_0209f4ae symbols with (char *) stride arithmetic.
 */
#include "dCamera_c.h"
#include "Player.h"
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
struct Vector3_16f;
namespace Particle {
struct System {
    static u32 NewWeather(u32, u32, Fix12i, Fix12i, Fix12i, const ::Vector3_16f *, u8);
};
}
#include "nitro/fx/fx.h"
#include "Sound.h"

typedef struct { int x, y, z; } Vec3;	/* POD triple for the C bodies */

/* dCamera_c's state objects are 0x10-byte records in the 0x0209b008 table; every
   use here is by address, so `int` gives the linker the symbol while the
   structures stay opaque. */
extern int data_0209b018;
extern int data_0209b028;
extern int data_0209b038;
extern int data_0209b048;
extern int data_0209b058;
extern int data_0209b068;
extern int data_0209b078;
extern int data_0209b088;
extern int data_0209b098;
extern int data_0209b0a8;
extern int data_0209b0b8;
extern int data_0209b0d8;
extern int data_0209b0f8;
extern int data_0209b108;
extern int data_0209b118;
extern int data_0209b128;
extern Matrix4x3 data_0209b41c;
extern unsigned int data_0209b454;

extern s16 data_02082214[];
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
extern s8 data_02092110;
extern s32 data_0209ee90[];
extern u8 data_0209f1f8;
extern u8 data_0209f20c;
extern u8 data_0209f250;
extern u8 data_0209f294;
extern u8 data_0209f2c4;
extern s8 data_0209f2f8;
extern s32 data_0209f32c;
extern void *data_0209f394[];
extern dClipper data_0209f43c;
extern s32 gRunningKuppaScript;
extern Matrix4x3 data_020a0e68;
extern void *data_ov002_0210c3b0;
extern void *data_ov002_0210c3e0[2];

extern "C" {
/* Siblings below 0x0200bec4 (defined at the end of the file). */
extern int func_020092c4(void *cam, Vector3 *out, const Vector3 *target);
extern void func_0200928c(dCamera_c *cam);
extern void RunStarCameraScript(int arg0);
extern s32 Math_Function_0203b14c(s32 *p, s32 tgt, s32 rate, s32 lim, s32 step);
extern int StartWithFarCamera();
extern void *GetViewObj(int idx);

extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
extern s16 Vec3_VertAngle(const void *v1, const void *v0);
extern int Vec3_HorzDist(const void *a, const void *b);
extern int Vec3_HorzLen(const void *v);
extern int LenVec3(void *v);
extern void Vec3_Add(void *out, const void *a, const void *b);
extern void Vec3_Sub(void *out, const void *a, const void *b);
extern void AddVec3(void *a, const void *b, void *c);
extern void SubVec3(void *a, const void *b, void *c);
extern void Vec3_MulScalar(void *out, const void *in, int scale);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern int *Vec3_LslInPlace(int *v, int sh);
extern void Vec3_RotateYAndTranslate(void *out, const void *in, short angle, const void *src);
extern void MulVec3Mat4x3(void *dst, const void *src, void *out);
extern s32 Vec3_Dist(const void *a, const void *b);
extern void Matrix4x3_FromRotationZ(void *m, int angle);
extern void MulMat4x3Mat4x3(void *dst, const void *a, void *b);
extern void Matrix4x3_LoadIdentity(Matrix4x3 *matrix);

/* local extern: the factory runs the base constructor over fresh operator new
   storage and stores the vptrs itself; C++ has no call spelling for a C2. */
extern void *_ZN7fBase_cC2Ev(void *self);
/* _ZTV7dBase_c/_ZTV7dView_c/_ZTV9dCamera_c come from decl_common.h (int[]/u32
   spellings); the stores below take their addresses explicitly. */

/* Siblings inside this TU whose file-local decl spellings decl_common.h does
   not already pin. */
extern void func_0200af20(dCamera_c *cam, Vector3 *v1, Vector3 *v2, short *out);
extern int func_0200c394(void *self, int a1, void *a2, void *a3, void **a5, int *a6);
extern void func_0200cbe0(void *self);
extern void func_0200cce4(char *cam);
extern void func_0200cf40(char *c);
extern int func_0200d0ac(int a, unsigned int b);
extern void func_0200d954(char *c, short arg);
}

/* Opaque camera-side records used by the older handlers. Ground and line
   queries use the real dBgCh_Gnd and dBgCh_Lin classes above. The remaining
   records are typedef'd for the cplusplus-off regions. */
typedef struct dBgCh_LinHit { char pad[0x14]; char surf[0x64]; } dBgCh_LinHit;   /* func_0200b990's variant */
typedef struct Entry3 { int a, b, c; } Entry3;                      /* data_02086f58 rows */
extern Entry3 data_02086f58[];
/* The 0x28-byte mode record dCamera_c::mMode and mAreaMode point at, entry N
   of the table at 0x02086fcc (func_0200cb58). func_02009e70 calls it CamMode.
   Only the fields this file reads are named. */
typedef struct CamMode {
    /* func_02009e70: below nearDist (func_020093f4-scaled) of horizontal
       travel the camera turns toward the player's facing; within farDist of
       lookAt it swings faster. */
    s32 nearDist; /* 0x00 */
    s32 farDist;  /* 0x04 */
    /* The least and greatest rise of pos over lookAt per unit of distance
       (func_02009e70, adjusted by func_0200c9e0). */
    s32 minRiseSlope; /* 0x08 */
    s32 maxRiseSlope; /* 0x0c */
    /* Scaled by func_020093d4 and added to the target's y to place lookAt;
       func_0200bec4 triples it while mFlags 0x20000 is set. */
    s32 lookAtHeight; /* 0x10 */
    /* Scaled by func_020093f4; the z of the offset that, turned by the
       player's yaw, places lookAt from the target. */
    s32 lookAtDist; /* 0x14 */
    /* func_0200bec4 moves lookAt.y linearly toward its goal while the rise
       is below this (func_020093d4-scaled), and lets it lag otherwise. */
    s32 followRise; /* 0x18 */
    /* The orbit distance func_02009e70 keeps pos beyond. */
    s32 minDist; /* 0x1c */
    /* The z of the offset that places pos from lookAt (func_0200d1e4,
       func_0200cce4). */
    s32 posDist; /* 0x20 */
    /* func_0200ca50 eases mFovAngle toward it. */
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
   +0 and +8; func_0200cae4/func_0200ca50 dispatch them through this shadow,
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

/* The G3x/OAM records below exist only so the Render body's member-call
   spellings mangle to the ROM's symbols; the by-value Fix12<int> parameters on
   the real signatures cannot be written (the Fix12 wall), so the actual calls
   go through the extern "C" spellings in the pool. */
struct OamAttr;
struct OAM {
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
};
struct G2x {
    static void SetBlendAlpha(volatile unsigned short *, int, int, int, int);
};
struct G3i {
    static void PerspectiveW_(Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, int, Matrix4x3 *);
    static void LookAt_(const Vector3 *, const Vector3 *, const Vector3 *, bool, Matrix4x3 *);
};

/* @symbol dCamera_c_classInit
 * Allocation factory historically labelled as the dCamera_c constructor. Its first action
 * overwrites r0 with fBase_c::operator new(sizeof(dCamera_c)); a real
 * constructor must consume the incoming storage pointer. The three vptr
 * transitions are dBase_c -> dView_c -> dCamera_c, and the Matrix4x3 at +0x50
 * is dView_c's view matrix. */
extern "C" dCamera_c *dCamera_c_classInit()
{
    dCamera_c *camera = (dCamera_c *)fBase_c::operator new(sizeof(dCamera_c));
    if (camera) {
        _ZN7fBase_cC2Ev(camera);
        *(void ***)camera = (void **)&_ZTV7dBase_c;
        *(void ***)camera = (void **)&_ZTV7dView_c;
        Matrix4x3_LoadIdentity(&camera->viewMat);
        *(void ***)camera = (void **)&_ZTV9dCamera_c;
    }
    return camera;
}

// @symbol _ZN9dCamera_c13InitResourcesEv
int dCamera_c::InitResources()
{
    dCamera_c *thiz = this;
    u32 i;
    for (i = 0; i < data_0209f1f8; i = (u8)(i + 1)) {
        CamViewObj *v = (CamViewObj *)GetViewObj(i);
        if (v->type == 7) {
            s32 *p;
            s16 x, y, z;
            thiz->mFixedViewPos = ::operator new(0xc);
            x = v->x; z = v->z; y = v->y;
            p = (s32 *)thiz->mFixedViewPos;
            p[0] = x << 0xc;
            p[1] = y << 0xc;
            p[2] = z << 0xc;
        }
    }
    thiz->mFovBlend = 0;
    thiz->mTargetPlayer = (dActor_c *)data_0209f394[data_0209f250];
    ChangeArea(thiz->mTargetPlayer->mAreaId);
    thiz->mFramedActor = 0;
    thiz->mFocusActor = 0;
    ChangeState((State *)&data_0209b008);
    func_0200cf40((char *)thiz);
    if (StartWithFarCamera() != 0)
        func_0200d0ac((int)thiz, data_0209f250);
    return this->Render();
}

// @symbol _ZN9dCamera_c8BehaviorEv
int dCamera_c::Behavior()
{
    char *c = (char *)((void *)this);
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
        func_020089f8(c);
    }

    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 && !(data_0209b454 & 0x40000000)) {
        enum Bool b2 = (enum Bool)(gRunningKuppaScript != 0);
        if (b2 == FALSE) {
            if (mFlags & 0x2000) {
                Math_Function_0203b0fc((int *)&mFovBlend, 0x1000, 0x88, 0x7fffffff);
            } else {
                Math_Function_0203b0fc((int *)&mFovBlend, 0, 0x88, 0x7fffffff);
            }
            func_0200ca50(c);
            if (mShakeY != 0) {
                ApproachLinear(*(int *)&mShakeY, 0, 0x1000);
                mShakeY = 0 - mShakeY;
            }
            if (mRollShake != 0) {
                if (ApproachLinear2(mRollShake, 0, 0x10) != 0) {
                    mRollShakePhase = 0;
                } else {
                    *(s16 *)((long long)(int)&mRollShakePhase) =
                        (s16)(*(s16 *)((long long)(int)&mRollShakePhase) + 0x3000);
                }
            }
        }
        if (mFovShake != 0) {
            *(s16 *)((long long)(int)&mFovShakePhase) =
                (s16)(*(s16 *)((long long)(int)&mFovShakePhase) + mFovShakePhaseStep);
            ApproachLinear2(mFovShake, 0, mFovShakeDecay);
        }
        {
            int condA = 1;
            if (data_0209f2f8 != 0xa && data_0209f2f8 != 0x13) condA = 0;
            if (condA != 0) {
                r5 = 0x112;
            } else {
                u8 t = (u8)data_0209f2f8 + 0xf8;
                enum Bool ip = FALSE;
                if ((u32)t <= 0x16) {
                    if ((1 << t) & 0x402401) {
                        ip = TRUE;
                    }
                }
                if (ip != FALSE) {
                    r5 = 0xbc;
                } else if (data_0209f2f8 == 0x31) {
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
        Player *m = (Player *)mTargetPlayer;
        enum Bool isBf = (enum Bool)(m->actorID == 0xbf);
        if (isBf != FALSE) {
            s32 t2 = data_0209f32c - (data_02082214[((u16)mAngleX >> 4) * 2] * 0x10);
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

// @symbol _ZN9dCamera_c6RenderEv
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
            if (data_0209f20c == 0 && data_0209f294 == 0 &&
                (u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) != 0) {
                func_0200af20(self, (Vector3 *)&sp[0], (Vector3 *)&sp[3], &sp18);
            }
        }
    }

    sp[1] += self->mShakeY;
    sp[4] += self->mShakeY;

    s16 tr3 = self->mRollShake;
    if (tr3 != 0) {
        s16 c = data_02082214[((s32)(u16)self->mRollShakePhase >> 4) * 2];
        var_r4 = (s16)(var_r4 + (s16)FX_MUL(tr3, c));
    }

    s16 tr0 = self->mFovShake;
    if (tr0 != 0) {
        s16 c = data_02082214[((s32)(u16)self->mFovShakePhase >> 4) * 2 + 1];
        s32 m = FX_MUL(c, 0x360);
        sp18 = (s16)(sp18 + ((tr0 * m) << 4 >> 16));
    }

    func_0200d954((char *)self, sp18);

    *(volatile s32 *)0x04000580 = self->mViewport[0] | (self->mViewport[1] << 8) |
                                  (self->mViewport[2] << 16) | (self->mViewport[3] << 24);

    s32 var_r6;
    if (data_0209f2f8 == 0x28) {
        var_r6 = 0x10;
    } else {
        var_r6 = data_0209ee90[0x44 / 4];
    }

    s32 idx = ((s32)(u16)sp18 >> 4) * 2;
    G3i::PerspectiveW_(data_02082214[idx], data_02082214[idx + 1], self->mAspect, self->mNear,
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

// @symbol _ZN9dCamera_c16OnPendingDestroyEv
/* No-op override; the camera does nothing extra when flagged for destruction. */
void dCamera_c::OnPendingDestroy()
{
    (void)this;
}

// @symbol _ZN9dCamera_c16CleanupResourcesEv
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
// @symbol func_0200d954
void func_0200d954(char *c, short arg) {
    int i = (int)(unsigned short)arg >> 4;
    int j = i * 2;
    struct dCamera_c *cam = (struct dCamera_c *)c;
    cam->mInvSinFov = func_02053200(data_02082214[j]);
    cam->mInvCosFov = func_02053200(data_02082214[j + 1]);
    data_0209f43c.Func_020156DC(cam->mAspect, arg, cam->mNear, cam->mFar);
}

// @symbol func_0200d8c8
/* Ground-pound jitter: if the target point is within range, raise mShakeY
 * to strength/distance scaled by 0x9000, clamped to 0xc000. */
void func_0200d8c8(struct dCamera_c *cam, const struct Vector3 *v, int strength) {
    Fix12i dist;
    Fix12i offset;

    dist = Vec3_Dist(&cam->pos, v);
    if (strength <= dist)
        return;

    offset = FX_MUL(cstd::fdiv(strength, dist), 0x9000);

    if (offset <= cam->mShakeY)
        return;

    if (offset > 0xc000)
        offset = 0xc000;

    cam->mShakeY = offset;
}

// @symbol func_0200d8ac
void func_0200d8ac(char *c, int r1, int r2, int r3)
{
    short ip;

    struct dCamera_c *cam = (struct dCamera_c *)c;
    ip = cam->mFovShake;
    if (r1 > ip) {
        cam->mFovShake = (short)r1;
        cam->mFovShakeDecay = (short)r2;
        cam->mFovShakePhaseStep = (short)r3;
    }
}

// @symbol func_0200d89c
void func_0200d89c(char *p)
{
    ((struct dCamera_c *)p)->mRollShake = 384;
}
}

// @symbol _ZNK9dCamera_c12IsUnderwaterEv
/* Returns the under-water bit of mFlags MASKED, not normalised to 0/1 -- the
 * ROM's `and r0,r0,#1` is the whole body. */
int dCamera_c::IsUnderwater() const
{
    return mFlags & 1;
}

extern "C" {
// @symbol func_0200d858
void func_0200d858(void *self, const struct Vector3 *offset)
{
    struct dCamera_c *cam = (struct dCamera_c *)self;
    AddVec3(&cam->lookAt, offset, &cam->lookAt);
    AddVec3(&cam->pos, offset, &cam->pos);
}

// @symbol func_0200d81c
void func_0200d81c(void *thiz, int playerID)
{
    if (playerID != (int)data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b008);
}

// @symbol func_0200d7e0
void func_0200d7e0(void *thiz, int playerID)
{
    if (playerID != (int)data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b098);
}

// @symbol func_0200d7a4
void func_0200d7a4(void *thiz, u8 playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b108);
}

// @symbol func_0200d768
void func_0200d768(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b018);
}

// @symbol func_0200d72c
void func_0200d72c(void *thiz, unsigned char playerID)
{
    if (playerID == *(unsigned char *)&data_0209f250)
        ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b028);
}

// @symbol func_0200d6f0
void func_0200d6f0(void *thiz, unsigned char playerID)
{
    if (playerID == *(unsigned char *)&data_0209f250)
    {
        ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b038);
    }
}

// @symbol func_0200d6b4
void func_0200d6b4(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b048);
}

// @symbol func_0200d678
void func_0200d678(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b058);
}

// @symbol func_0200d63c
void func_0200d63c(void *thiz, unsigned char playerID)
{
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b068);
}

// @symbol func_0200d5fc
void func_0200d5fc(void *thiz, int playerID)
{
    if (playerID == data_0209f250) {
        ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b078);
        func_0200cb58(thiz, 5);
    }
}

// @symbol func_0200d5c0
void func_0200d5c0(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b078);
}

// @symbol func_0200d580
void func_0200d580(void *thiz, int playerID)
{
    if (playerID != data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b078);
    func_0200cb58(thiz, 8);
}

// @symbol func_0200d544
void func_0200d544(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b088);
}

// @symbol func_0200d508
void func_0200d508(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b0a8);
}

// @symbol func_0200d4b0
void func_0200d4b0(char* self, unsigned int playerID, int arg2)
{
    unsigned char tmp = data_0209f250;
    if (playerID == tmp)
    {
        volatile int dummy;
        struct dActor_c *other;
        ((struct dCamera_c *)self)->mOrbitTargetYaw = ((struct dCamera_c *)self)->mAngleY;
        other = ((struct dCamera_c *)self)->mTargetPlayer;
        other->mFlags |= 0x20000000;
        RunStarCameraScript(arg2);
    }
}

// @symbol func_0200d474
void func_0200d474(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b0b8);
}

// @symbol func_0200d438
void func_0200d438(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b0c8);
}

// @symbol func_0200d3f8
void func_0200d3f8(void *thiz, unsigned char playerID, void *ptr)
{
    unsigned char tmp = *(unsigned char *)&data_0209f250;
    if (playerID != tmp)
        return;
    ((struct dCamera_c *)thiz)->mLookTarget = (s32 *)ptr;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b0d8);
}
}

// @symbol _ZN9dCamera_c14GoBehindPlayerEj
void dCamera_c::GoBehindPlayer(unsigned int j)
{
    int slot4, slot8, slotc;

    if (j != data_0209f250)
        return;
    if (data_02092110 >= 0)
        return;

    mFlags &= 0xfffffaf7u;
    func_0200cb58(this, 0xa);
    mFlags |= 4u;

    slot4 = (int)mMode;
    slotc = 0;
    func_0200c66c(this, &mTargetPlayer->mPos, &slot8, &slot4, &slotc);
    if (slot4 == (int)&data_020873dc)
        return;
    if (slot4 == (int)&data_0208742c)
        return;
    ChangeState((State *)&data_0209b0e8);
}

#pragma cplusplus off
// @symbol func_0200d1e4
void func_0200d1e4(char *self)
{
    struct dCamera_c *cam = (struct dCamera_c *)self;
    struct dActor_c *info = cam->mTargetPlayer;
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

    cam->mTargetPos.x = base->x;
    cam->mTargetPos.y = base->y;
    cam->mTargetPos.z = base->z;

    v1 = cam->mMode->lookAtDist;
    r0 = func_020093f4(self, v1);

    temp1[0] = data_02086f2c.x;
    temp1[1] = data_02086f2c.y;
    temp1[2] = r0;
    data_02086f2c.z = r0;

    angle = cam->mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&cam->lookAt, &cam->mTargetPos, angle, temp1);

    v2 = cam->mMode->lookAtHeight;
    dResult = func_020093d4(self, v2);
    cam->lookAt.y += dResult;

    v3 = cam->mMode->posDist;
    r0b = func_020093f4(self, v3);

    data_02086f14[2] = r0b;
    temp2[0] = data_02086f14[0];
    temp2[1] = data_02086f14[1];
    temp2[2] = r0b;

    angle = cam->mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&cam->pos, &cam->lookAt, angle, temp2);

    cam->pos.y += (int)(dResult - 0x8b000);

    cam->mLookAtOffset.x = 0;
    cam->mLookAtOffset.y = 0xc3f9d;
    cam->mLookAtOffset.z = 0;

    ChangeArea(cam->mTargetPlayer->mAreaId);
}
#pragma cplusplus on

// @symbol _ZN9dCamera_c10LookAtExitER8dActor_c
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

extern "C" {
// @symbol func_0200d148
void func_0200d148(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b118);
}

// @symbol func_0200d10c
void func_0200d10c(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    ((dCamera_c *)thiz)->ChangeState((dCamera_c::State *)&data_0209b128);
}

// @symbol func_0200d0ac
int func_0200d0ac(int a, unsigned int b) {
    if (b == data_0209f250) {
        unsigned short v = ((struct dCamera_c *)a)->mAreaMode->flags;
        if (v & 1) {
            return 0;
        }
        ((struct dCamera_c *)a)->mFlags |= 2u;
        func_02012790(7);
    }
    return 1;
}

// @symbol func_0200d064
void func_0200d064(void *param_1, unsigned char param_2) {
    unsigned char tmp = data_0209f250;
    if (param_2 == tmp) {
        volatile int dummy;
        unsigned int *addr = &((struct dCamera_c *)param_1)->mFlags;
        unsigned int val = *addr;
        val &= ~2u;
        *addr = val;
        func_02012790(6);
    }
}
}

// @symbol _ZN9dCamera_c9SetFlag_3Ev
void dCamera_c::SetFlag_3()
{
    u32 *p = &mFlags;
    u32 v = *p | 8;
    *p = v;
    ((void (*)(u32))FUN_02029a68)(v);
}

#pragma cplusplus off
// @symbol func_0200cf40
void func_0200cf40(char *c) {
    struct dCamera_c *cam = (struct dCamera_c *)c;
    int new_var;
    cam->mViewport[0] = 0;
    if (c) {
    }
    cam->mViewport[1] = 0;
    cam->mViewport[2] = 0xff;
    cam->mViewport[3] = 0xbf;
    cam->mAspect = 0x1555;
    cam->mNear = 0x1000;
    cam->mFar = 0x1388000;
    cam->mAreaMode = (CamMode *)&data_0208715c;
    cam->mViewObj = 0;
    if ((int)cam->param1 != 0xf) {
        CamEntryE *v;
        int x;
        int y;
        int z;
        func_0200cb58(c, 0xa);
        cam->mFovAngle = cam->mMode->fovAngle;
        func_0200d954(c, cam->mFovAngle);
        func_0200cce4(c);
        v = (CamEntryE *)GetViewObj((int)cam->param1 & 0xff);
        x = v->f2;
        z = v->f6;
        new_var = z << 12;
        y = v->f4;
        cam->pos.x = x << 12;
        cam->pos.y = y << 12;
        cam->pos.z = new_var;
        cam->mBaseAngleY = v->fa + 0x8000;
        return;
    }
    func_0200cb58(c, 9);
    cam->mFovAngle = cam->mMode->fovAngle;
    func_0200d954(c, cam->mFovAngle);
    func_0200cce4(c);
}

// @symbol func_0200cce4
#pragma cplusplus on
extern "C" void func_0200cce4(char* cam)
{
    dCamera_c *self = (dCamera_c *)cam;
    Vec3 vec;
    Vec3 tmp1;
    Vec3 tmp2;
    u32 d;

    if (self->mTargetPlayer != 0) {
        Vec3 *p = (Vec3 *)&self->mTargetPlayer->mPos;
        self->mTargetPos.x = p->x;
        self->mTargetPos.y = p->y;
        self->mTargetPos.z = p->z;

        {
            u32 d1 = func_020093f4(cam, self->mMode->lookAtDist);
            tmp1.z = d1;
            tmp1.x = data_02086f2c.x;
            tmp1.y = data_02086f2c.y;
            data_02086f2c.z = d1;
        }
        Vec3_RotateYAndTranslate((Vec3 *)&self->lookAt, &self->mTargetPos,
                                 self->mTargetPlayer->mAngleY, &tmp1);

        d = func_020093d4(cam, self->mMode->lookAtHeight);
        self->lookAt.y += d;

        {
            u32 d2 = func_020093f4(cam, -self->mMode->posDist);
            tmp2.z = d2;
            data_02086f20.z = d2;
            tmp2.x = data_02086f20.x;
            tmp2.y = data_02086f20.y;
        }
        Vec3_RotateYAndTranslate((Vec3 *)&self->pos, (Vec3 *)&self->lookAt,
                                 self->mTargetPlayer->mAngleY, &tmp2);

        dBgCh_Lin line;
        func_0200897c(cam, &line);
        {
            int tx;
            int ty;
            int tz;
            tx = self->pos.x;
            tz = self->pos.z;
            ty = self->pos.y + 0x100000;
            vec.x = tx;
            vec.y = ty;
            vec.z = tz;
        }
        line.SetObjAndLine(self->pos, *(Vector3 *)&vec, 0);
        if (line.DetectClsn() != 0) {
            Vector3 cp = line.GetClsnPos();
            vec.x = cp.x;
            vec.y = cp.y;
            vec.z = cp.z;
        }
        dBgCh_Gnd ground;
        func_0200897c(cam, &ground);
        ground.SetObjAndPos(*(Vector3 *)&vec, 0);
        if (ground.DetectClsn() != 0) {
            self->pos.y = d + ground.clsnY;
        }
        self->pos.y += func_020093d4(cam, 0x3c286);
        {
            int diff;
            int base;
            int a;
            base = self->lookAt.y;
            diff = self->pos.y - base;
            a = (diff < 0) ? -diff : diff;
            if (a > 0x3e8000) {
                if (diff < -0x3e8000) diff = -0x3e8000;
                else if (diff > 0x3e8000) diff = 0x3e8000;
                self->pos.y = base + diff;
            }
        }
        {
            u32 t = func_020093d4(cam, self->mMode->lookAtHeight);
            self->mLookAtOffset.x = 0;
            self->mLookAtOffset.y = t;
            self->mLookAtOffset.z = 0;
        }
        func_0200928c((struct dCamera_c *)cam);
        ChangeArea(self->mTargetPlayer->mAreaId);
    }
    self->mFlags |= 4;
}
#pragma cplusplus on

// @symbol _ZN9dCamera_c9SetLookAtERK7Vector3
void dCamera_c::SetLookAt(const Vector3 & lookAt_)
{
    lookAt.x = lookAt_.x;
    lookAt.y = lookAt_.y;
    lookAt.z = lookAt_.z;
}

// @symbol _ZN9dCamera_c6SetPosERK7Vector3
void dCamera_c::SetPos(const Vector3 & pos_)
{
    pos.x = pos_.x;
    pos.y = pos_.y;
    pos.z = pos_.z;
}

// @symbol _ZN9dCamera_c25SaveCameraStateBeforeTalkEv
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

#pragma cplusplus off
// @symbol func_0200cbe0
void func_0200cbe0(void *self)
{
    struct dCamera_c *cam = (struct dCamera_c *)self;
    int r1 = func_020092c4(cam, &cam->lookAt, &cam->savedLookAt);
    int r2 = func_020092c4(cam, &cam->pos, &cam->savedPos);
    short h;
    short v;

    if (r2 != 0) {
        if (r1 != 0) {
            cam->mFlags &= ~0x8000;
        }
    }

    h = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    cam->mAngleY = h;

    v = Vec3_VertAngle(&cam->lookAt, &cam->pos);
    cam->mAngleX = v;
}
#pragma cplusplus on

// @symbol _ZN9dCamera_c11ChangeStateEPNS_5StateE
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
            func_020089f8(this);
        }
        mState = state;
        mTurningToTarget = 0;
    }
    return func_0200cae4(this);
}

#pragma cplusplus off
// @symbol func_0200cb58
void func_0200cb58(void *obj_, int index) {
    struct dCamera_c *obj = (struct dCamera_c *)obj_;
    obj->mMode = (CamMode *)(index * 0x28 + (int)&data_02086fcc);
}
#pragma cplusplus on

// @symbol func_0200cae4
/* State dispatch: honors the deferred-change flags, then calls the state's
 * +0 member-function pointer on the camera. */
extern "C" int func_0200cae4(void *c_) {
    CamPmfHost *c = (CamPmfHost *)c_;
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

// @symbol func_0200ca50
/* Per-frame state tick: mFovAngle eases toward the mode record's +0x24
 * angle, then the state's +8 member-function pointer runs. */
extern "C" int func_0200ca50(void *self)
{
    CamPmfHost *ch = (CamPmfHost *)self;
    dCamera_c *cam = (dCamera_c *)self;
    int flags;
    int r5;
    {
        CamMode *p = cam->mMode;
        ApproachLinear2(cam->mFovAngle, p->fovAngle, 0x80);
    }
    r5 = 1;
    flags = cam->mFlags;
    if (flags & 8) {
        ;
    } else if (flags & 0x8000) {
        func_0200cbe0(self);
    } else {
        u8 *obj = (u8 *)cam->mState;
        if (*(int *)(obj + 8) != 0) {
            CamPmf *pp = (CamPmf *)(obj + 8);
            r5 = (ch->**pp)();
        }
    }
    return r5;
}

#pragma cplusplus off
// @symbol func_0200ca14
void func_0200ca14(void *r0, unsigned char r1, int r2)
{
    if (r1 != (unsigned int)data_0209f250)
        return;

    if (r2 == 0)
        ((struct dCamera_c *)r0)->mFlags |= 0x20000;
    else
        ((struct dCamera_c *)r0)->mFlags |= 0x10000;
}

// @symbol func_0200c9e0
void func_0200c9e0(void *c, s32 *r1, s32 *r2)
{
    unsigned int v = ((struct dCamera_c *)c)->mFlags;
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

// @symbol func_0200c66c
#pragma cplusplus on
extern "C" int func_0200c66c(void *self_, void *pos_, int *out2_, int *out1_, int *arg5_)
{
    char *self = (char *)self_;
    dCamera_c *cam = (dCamera_c *)self_;
    Vec3 *pos = (Vec3 *)pos_;
    void **out2 = (void **)out2_;
    void **out1 = (void **)out1_;
    Vec3 tmp;
    int hval;
    int col1;
    int col2;
    Player *p;
    Player *obj;
    int res;

    p = (Player *)cam->mTargetPlayer;
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

    if (obj != 0 && data_0209f2f8 != 0x1c) {
        col1 = obj->mClpsBits15_18 & 0xff;
        col2 = obj->mClpsBits6_11 & 0xff;
        hval = res;
    } else {
        dBgCh_Lin line;
        func_0200897c(self, &line);
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
        func_0200897c(self, &ground);
        ground.SetObjAndPos(*(Vector3 *)&tmp, 0);
        if (ground.DetectClsn() == 0) {
            hval = pos->y;
            col1 = 0;
            col2 = 0x3f;
        } else {
            hval = ground.clsnY;
            col1 = ((const CLPS *)&ground.surface)->func_02037e48() & 0xff;
            col2 = ((const CLPS *)&ground.surface)->func_02037e68() & 0xff;
        }

    }

    if (col1 == 0 || (cam->mFlags & 0x400) != 0) {
        if (cam->mMode->flags & 0x10) {
            if (cam->mAreaMode)
                *out1 = cam->mAreaMode;
            if (cam->mViewObj)
                *out2 = cam->mViewObj;
        }
    } else {
        int b0;
        if (col2 != 0x3f) *out2 = GetViewObj(col2);
        if (func_0200c394(self, col1, *out2, pos, out1, arg5_) == 0) {
            if ((u32)cam->mMode <= (u32)&data_02086ff4 || *out1 == (void *)&data_02087274) {
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
                        if (cam->mAreaMode != *out1)
                            cam->mFlags |= 4;
                    }
                    res = hval;
                } else if (b0 == 6) {
                    *out1 = (void*)&data_0208742c;
                } else {
                    if ((cam->mMode->flags & 4) == 0) {
                        if (b0 == 0) {
                            if (*out1 == (void*)&data_02087274 && data_0209f2f8 == 0x2d) {
                                cam->mLookTargetPos.x = cam->mTargetPos.x;
                                cam->mLookTargetPos.y = cam->mTargetPos.y;
                                cam->mLookTargetPos.z = cam->mTargetPos.z;
                            }
                            *out1 = (void*)&data_020871ac;
                        } else if (b0 == 1) {
                            *out1 = (void*)&data_020871d4;
                        } else if (b0 == 5) {
                            *out1 = (void*)&data_02087404;
                        }
                    }
                }
                cam->mFlags &= ~0x100;
            }
        }
        if (*out1 != (void *)&data_02087094)
            cam->mAreaMode = (CamMode *)*out1;
        if (*out1 != (void*)&data_0208733c) {
            if (*out2 != 0 && *(unsigned char*)(*out2) != 2)
                cam->mViewObj = *out2;
        }
    }

    return res;
}
extern "C" {

// @symbol func_0200c394
int func_0200c394(void *self_, int a1, void *a2_, void *a3, void **a5, int *a6)
{
    char *self = (char *)self_;
    struct dCamera_c *cam = (struct dCamera_c *)self_;
    unsigned char *a2 = (unsigned char *)a2_;
    CamMode *p = cam->mMode;
    Vec3 local, local2;
    signed char g;
    int flags = p->flags;
    int v;
    short sz, sy, sx;

    if ((flags & 0x20) == 0) {
        v = cam->mFlags & 0x100;
        if (v == 0) goto body;
    }
    return 0;
body:
    {
            if (a1 == 7) {
                struct dActor_c *o = cam->mFramedActor;
                if (o == 0) {
                    if (a2 == 0) goto other;
                    if (a2[0] != 3) goto other;
                }
                *a6 = 1;
                o = cam->mFramedActor;
                if (o) {
                    Vector3 *q = &o->mPos;
                    cam->mLookTargetPos.x = q->x;
                    cam->mLookTargetPos.y = q->y;
                    cam->mLookTargetPos.z = q->z;
                } else {
                    Vec3 t;
                    sx = *(short *)(a2 + 2);
                    sz = *(short *)(a2 + 6);
                    sy = *(short *)(a2 + 4);
                    t.x = sx << 12; t.z = sz << 12; t.y = sy << 12;
                    cam->mLookTargetPos.x = t.x;
                    cam->mLookTargetPos.y = t.y;
                    cam->mLookTargetPos.z = t.z;
                }
                Vec3_Sub(&local, (Vec3 *)&cam->mLookTargetPos, a3);
                cam->mFocusDist = LenVec3(&local) >> 1;
                g = data_0209f2f8;
                v = (g == 0x2d);
                if (v) {
                    if (cam->mFocusDist < 0x100000)
                        cam->mFocusDist = 0x100000;
                    else if (cam->mFocusDist > 0x180000)
                        cam->mFocusDist = 0x180000;
                } else {
                    if (cam->mFocusDist < 0x180000)
                        cam->mFocusDist = 0x180000;
                }
                if (cam->mFlags & 2) {
                    cam->mFocusDist = FX_MUL(cam->mFocusDist, 0x1800);
                }
                if (g == 0x2f && cam->mFocusDist > 0xc0000) {
                    int s = cstd::fdiv(0xc0000, cam->mFocusDist << 1);
                    Vec3_MulScalarInPlace(&local, s);
                    Vec3_Add(&local2, a3, &local);
                    cam->mLookTargetPos.x = local2.x;
                    cam->mLookTargetPos.y = local2.y;
                    cam->mLookTargetPos.z = local2.z;
                    cam->mFocusDist = 0xc0000;
                } else {
                    AddVec3((Vec3 *)&cam->mLookTargetPos, a3, (Vec3 *)&cam->mLookTargetPos);
                    Vec3_AsrInPlace((Vec3 *)&cam->mLookTargetPos, 1);
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

// @symbol func_0200bec4
}
extern "C" int func_0200bec4(char *self, int *arg1, int arg2, char *arg3, int arg4)
{
    dCamera_c *cam = (dCamera_c *)self;
    int r5 = arg4;
    Vec3 sp8;
    Vec3 sp14;
    Vec3 sp20;
    Vec3 sp2C;
    Vec3 sp38;
    Vec3 sp44;
    int r4;
    int t;
    int sl;

    sp44.z = func_020093f4(self, ((CamMode *)arg3)->lookAtDist);
    data_02086f2c.z = sp44.z;
    sp44.x = data_02086f2c.x;
    sp44.y = data_02086f2c.y;
    Vec3_RotateYAndTranslate(&sp8, arg1, cam->mTargetPlayer->mAngleY, (int *)&sp44);
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
    func_0200897c(self, &line);
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
    func_0200897c(self, &line);
    line.SetObjAndLine(*(Vector3 *)&sp8, *(Vector3 *)&sp2C, 0);
    if (line.DetectClsn() != 0) {
        Vector3 sp5C = line.GetClsnPos();
        sp2C.x = sp5C.x;
        sp2C.y = sp5C.y;
        sp2C.z = sp5C.z;
    }
    dBgCh_Gnd spEC;
    func_0200897c(self, &spEC);
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
    if (cam->mFlags & 0x20000) {
        t = FX_MUL(t, 0x3000);
    }
    sp8.y += func_020093d4(self, t);
    sl = Vec3_HorzLen(&sp8);
    Vec3 sp68;
    Vec3_MulScalar(&sp68, &cam->mLookTargetPos, cam->mFocusBlend);
    AddVec3(&sp8, &sp68, &sp8);
    if (cam->mFlags & 4) {
        cam->mLookAtOffset.x = sp8.x;
        cam->mLookAtOffset.y = sp8.y;
        cam->mLookAtOffset.z = sp8.z;
    } else {
        int hl = Vec3_HorzLen(&cam->mLookAtOffset) - sl;
        if (((CamMode *)arg3)->flags & 0x40) {
            r5 = 0x10000;
        } else if (hl > 0) {
            r5 += FX_MUL(hl, 0x400);
        }
        Vec3_Sub(&sp38, &sp8, &cam->mLookAtOffset);
        sl = Vec3_HorzLen(&sp38);
        {
            int f = sl;
            int fd;
            if (sl != 0) {
                Math_Function_0203b14c(&f, 0, 0x100, FX_MUL(r5, 0x600), 0x1000);
                fd = cstd::fdiv(f, sl);
                cam->mLookAtOffset.x = sp8.x - FX_MUL(sp38.x, fd);
                cam->mLookAtOffset.z = sp8.z - FX_MUL(sp38.z, fd);
            }
        }
        {
            int a8 = cam->mLookAtOffset.y;
            int diff = sp8.y - a8;
            cam->mLookAtOffset.y = sp8.y - FX_MUL(diff, 0xf80);
        }
        if (cam->mFlags & 0x400) {
            return r4;
        }
    }
    cam->lookAt.x = arg1[0] + cam->mLookAtOffset.x;
    cam->lookAt.z = arg1[2] + cam->mLookAtOffset.z;
    {
        int sl2 = arg1[1] + cam->mLookAtOffset.y;
        int r5b = sl2 - cam->lookAt.y;
        if (r5b < (int)func_020093d4(self, ((CamMode *)arg3)->followRise)) {
            ApproachLinear(*(int *)&cam->lookAt.y, sl2, 0x64000);
        } else {
            int r6 = arg2 - arg1[1];
            if (r6 >= 0) {
                r6 = 0;
            } else {
                int lim = func_020093d4(self, -0xf0a00);
                if (r6 < lim)
                    r6 = lim;
            }
            {
                int r2 = r6 + r5b;
                if (r2 > 0) {
                    cam->lookAt.y = sl2 - (r5b - FX_MUL(r2, 0x600));
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
 * script ops through the member-function table that func_02008550 builds from
 * 0x02086bc0. Their names are not recovered, so they keep C linkage, but every
 * one takes the camera as its first argument.
 * ------------------------------------------------------------------------- */

/* KuppaScript.cpp's readers of the script's unaligned operands and its
   lookups of the script's actors. */
extern "C" s16 ReadUnalignedShort(u8 *p);
extern "C" u16 ReadUnalignedUshort(u8 *p);
extern "C" s32 ReadUnalignedSignedInt(u8 *p);
extern "C" dActor_c *FindScriptPlayer(u32 character);
extern "C" dActor_c *FindDemoActor(u32 param1);

extern "C" {
/* local extern: math/CalcLib.h spells the Vec3 helpers with const Vector3 *,
   which this file's POD Vec3 callers cannot pass; only these two are taken
   from it. */
extern s32 ApproachAngle(s16 *cur, s32 target, s32 div, s32 band, s32 step);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
/* local extern: math/MtxStack.h's MulMat4x3Mat4x3 spelling conflicts with the
   one Render calls. */
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, short angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angle);

extern int NormalizeVec3IfNonZero(Vector3 *v);
extern Fix12i DotVec3(const Vector3 *a, const Vector3 *b);
/* The 64-bit squared distance between two points. */
extern long long func_0203cf94(int *a, int *b);
extern void func_ov002_020effb8(Vector3 *out, s32 t, Vector3 *p0, Vector3 *p1, Vector3 *p2,
                                Vector3 *p3);
extern int func_ov002_020ca270(char *player);
extern void func_02035468(char *query);
extern void FUN_02029ab0(void);
extern int Sound_PlayIfNotActive(int handle, int bank, int id, int arg);
extern void func_020070e8(dCamera_c *cam, int distTarget, int distStep, short vertTarget,
                          short vertDiv, short horzTarget, short horzDiv);
extern void func_020071a8(dCamera_c *cam, short ang, int threshold);

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
extern u8 data_0209f2d8;
extern s32 data_0209b000;
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
int func_02009374(dCamera_c *cam);
int func_02008cb4(dCamera_c *cam);
int func_020091f8(dCamera_c *cam, Vector3 *lookAt, int angleY, int angleX);
int func_02009138(dCamera_c *cam, int dist);
int func_02009414(dCamera_c *cam);
int func_020094a8(dCamera_c *cam);
int func_02008b08(dCamera_c *cam);
short func_02008abc(dCamera_c *cam);
short func_02008a70(dCamera_c *cam);
void func_020089d8(dCamera_c *cam);
void func_02007c14(Vector3 *res, const Vector3 *trans, int mag, short ang, short angY);
void func_02007c9c(const Vector3 *v0, const Vector3 *v1, int *outDist, short *outVertAng,
                   short *outHorzAng);
void func_02007d0c(Vector3 *self, const Vector3 *src, int stepX, int stepY, int stepZ);
}

extern "C" {

// @symbol func_0200bb28
/* The turn-by-buttons follow (mode data_0208733c): the 0x100/0x200 buttons
   step mBaseAngleOffset by 0x2000 and button 0x4000 swings the camera behind
   the player. A type-2 view object supplies the base yaw at +0xa; its byte 1
   zero means jump to it at once (snap mEasedAngleY and set mFlags bit 4, which
   places pos directly). */
void func_0200bb28(dCamera_c *cam, CamViewObj *view) {
    s32 flag4000;
    s32 sp_speed;
    struct Vector3 vsrc;
    struct Vector3 vin;
    struct Vector3 vout;
    s32 *tbl;
    s32 comp;
    s32 m;

    cam->mFlags |= 0x1000;

    m = gActivePlayerSlot * 0x18;
    flag4000 = *(u16 *)((char *)data_0209f49c + m) & 0x4000;

    if (flag4000 != 0 || cam->mRecentering != 0) {
        if (*(u16 *)((char *)data_0209f49e + m) & 0x4000) {
            func_02012790(0x1a);
        }
        if (flag4000 != 0) {
            cam->mBehindAngleY = (s16)(cam->mTargetPlayer->mAngleY + 0x8000);
        }
        cam->mRecentering =
            (ApproachAngle(&cam->mEasedAngleY, cam->mBehindAngleY, 4, 0x400, 0x100) != 0);
        cam->mBaseAngleOffset = (s16)(cam->mEasedAngleY - cam->mBaseAngleY);
        cam->mBaseAngleOffset = (s16)((cam->mBaseAngleOffset + 0x1000) & -0x2000);
        cam->mFlags |= 0x60;
    } else {
        if (view != 0) {
            if (view->type == 2 && view != cam->mViewObj) {
                cam->mBaseAngleY = (s16)(view->angleY + 0x8000);
                if (view->p1 == 0) {
                    cam->mEasedAngleY = cam->mBaseAngleY;
                    cam->mFlags |= 4;
                }
                cam->mBaseAngleOffset = 0;
                cam->mViewObj = view;
            }
        } else {
            cam->mViewObj = 0;
        }
        if (!(cam->mFlags & 0x60)) {
            s32 m2;
            s32 f1;
            /* Here mStateTimer is the delay before a held turn button
               repeats. */
            DecIfAbove0_Short(&cam->mStateTimer);
            m2 = gActivePlayerSlot * 0x18;
            f1 = *(u16 *)((char *)data_0209f49c + m2) & 0x300;
            if (f1 != 0) {
                s32 f2 = *(u16 *)((char *)data_0209f49e + m2) & 0x300;
                if (f2 != 0) {
                    cam->mTurnButtons = f2;
                } else if (!(cam->mTurnButtons & f1)) {
                    f2 = f1;
                    cam->mTurnButtons = f1;
                }
                if (f2 != 0 || cam->mStateTimer == 0) {
                    u32 dir = cam->mTurnButtons;
                    if (dir & 0x200) {
                        cam->mBaseAngleOffset -= 0x2000;
                        cam->mFlags |= 0x20;
                    } else if (dir & 0x100) {
                        cam->mBaseAngleOffset += 0x2000;
                        cam->mFlags |= 0x40;
                    }
                    cam->mStateTimer = 0x14;
                }
            }
        }
        ApproachLinear(cam->mEasedAngleY, (s16)(cam->mBaseAngleY + cam->mBaseAngleOffset), 0x800);
    }

    tbl = (cam->mFlags & 2) ? data_02086d50 : data_02086cc8;
    sp_speed = tbl[1];
    func_0200c9e0(cam, &sp_speed, &sp_speed);
    comp = FX_MUL(tbl[0], sp_speed);
    vsrc.x = 0;
    vsrc.z = 0;
    vsrc.y = comp;
    {
        s32 t0 = tbl[0];
        vsrc.z = cstd::sqrt((s64)t0 * t0 - (s64)comp * comp);
    }
    {
        s32 tz = cam->mTargetPos.z;
        s32 ty = cam->lookAt.y;
        s32 tx = cam->mTargetPos.x;
        vin.x = tx;
        vin.y = ty;
        vin.z = tz;
    }
    Vec3_RotateYAndTranslate(&vout.x, &vin.x, cam->mEasedAngleY, &vsrc.x);
    if (cam->mFlags & 4) {
        cam->pos.x = vout.x;
        cam->pos.y = vout.y;
        cam->pos.z = vout.z;
    }
    Math_Function_0203b14c(&cam->pos.y, vout.y, 0x300, 0x18000, 0x100);
    vout.y = cam->pos.y;
    if (ApproachLinear(cam->pos, vout, flag4000 != 0 ? 0xc0000 : 0x40000) != 0 && flag4000 == 0) {
        cam->mFlags &= ~0x60;
    }
    cam->mAngleY = cam->mEasedAngleY;
}

// @symbol func_0200b990
/* The orbit for modes data_020871ac and data_020871d4: ease mAngleY toward
   the yaw from the view object to lookAt, pushed a step to the side the
   target player faces away from. With byte 0 and byte 1 both clear, a wall
   between lookAt and the object can take over the yaw (except in three
   levels). */
void func_0200b990(dCamera_c *cam, CamViewObj *obj, int arg2) {
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
    angle = Vec3_HorzAngle(&v, &cam->lookAt);

    rate = 8;
    b0 = obj->type;
    if (b0 == 0) {
        b1 = obj->p1;
        if (b1 == 0xff) {
            signed char zone = data_0209f2f8;
            if (zone != 0x16 && zone != 0xa && zone != 0x10) {
                dBgCh_Lin rc;
                Vector3 lineB;
                func_0200897c(cam, &rc);
                lineB.y = cam->lookAt.y;
                lineB.x = v.x;
                lineB.z = v.z;
                rc.SetObjAndLine(cam->lookAt, lineB, 0);
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
            rate = (data_0209f2f8 == 0x2d) ? 0x12 : 0xf;
        }
    } else {
        step = 0x6800;
    }

    {
        dActor_c *ptr = cam->mTargetPlayer;
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
        ApproachLinear(cam->mAngleY, angle, (s16)fixmul);
    }
}

// @symbol func_0200b798
/* The fixed-point follow (mode data_020873dc): place pos on the line from the
   view object's ground point to the target, at the fraction and distance
   limits of row p1 of data_02086f58. */
void func_0200b798(dCamera_c *cam, CamViewObj *obj, int arg2) {
    Vector3 pos;
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
        pos.x = tx;
        pos.y = ty;
        pos.z = tz;
    }

    dBgCh_Gnd ray;
    ray.SetObjAndPos(pos, 0);
    if (ray.DetectClsn() != 0) {
        pos.y -= ray.clsnY;
    }

    sum = arg2 + pos.y;
    {
        int sz = cam->mTargetPos.z;
        int sx = cam->mTargetPos.x;
        int dz = sz - pos.z;
        int dx = sx - pos.x;
        int dy = sum - pos.y;
        v1.x = dx;
        v1.y = dy;
        v1.z = dz;
    }
    c = e->c;
    v2.y = sum;
    v2.x = pos.x + FX_MUL(v1.x, c);
    v2.z = pos.z + FX_MUL(v1.z, c);

    len = LenVec3(&v1);
    if (len < 0x220000) {
        v2.y += 0x220000 - len;
    }

    Vec3_Sub(&v3, &v2, &cam->lookAt);
    v1.x = v3.x;
    v1.y = v3.y;
    v1.z = v3.z;
    len = LenVec3(&v1);

    limit = e->a;
    if (cam->mFlags & 0xc00)
        limit = e->b;

    if (len > limit) {
        scale = cstd::fdiv(limit, len);
        Vec3_MulScalarInPlace(&v1, scale);
        Vec3_Add(&v4, &cam->lookAt, &v1);
        v2.x = v4.x;
        v2.y = v4.y;
        v2.z = v4.z;
    }

    if (cam->mFlags & 4) {
        cam->pos.x = v2.x;
        cam->pos.y = v2.y;
        cam->pos.z = v2.z;
    } else {
        func_020092c4(cam, &cam->pos, &v2);
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
}

// @symbol func_0200b590
/* The view-object orbit (mode data_02087404): turn data_02086e84 by the yaw
   from the object to the target, held within 0x4000 of the yaw from the
   object to pos, and lift the result off the ground below it. */
void func_0200b590(dCamera_c *cam, CamViewObj *obj, int arg2) {
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

    angle1 = Vec3_HorzAngle(&cam->mTargetPos, &v0);
    angle2 = Vec3_HorzAngle(&cam->pos, &v0);
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

    selfZ = cam->lookAt.z;
    dz = rotated.z - selfZ;
    selfX = cam->lookAt.x;
    dx = rotated.x - selfX;
    pos.y = arg2 + 0x100000;

    pos.x = selfX + FX_MUL(dx, 0xb33);
    pos.z = selfZ + FX_MUL(dz, 0xb33);

    dBgCh_Gnd rg;
    func_0200897c(cam, &rg);
    rg.SetObjAndPos(pos, 0);
    if (rg.DetectClsn()) {
        hitY = rg.clsnY;
        rotated.y = hitY;
        if (hitY < arg2)
            rotated.y = arg2;
    }
    rotated.y = rotated.y + 0x7d000;
    dist = Vec3_HorzDist(&cam->pos, &cam->mTargetPos);
    rem = 0x100000 - dist;
    if (rem > 0)
        rotated.y += rem;
    if (cam->mFlags & 4) {
        cam->pos.x = rotated.x;
        cam->pos.y = rotated.y;
        cam->pos.z = rotated.z;
    } else {
        func_020092c4(cam, &cam->pos, &rotated);
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
}

// @symbol func_0200b0fc
/* The path follow (mode data_0208742c): keep pos on the path the view
   object's byte 1 names, abreast of lookAt, stepping mPathNode forward or
   back as lookAt passes a node. */
void func_0200b0fc(dCamera_c *cam, CamViewObj *obj, int floorY) {
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

    if (cam->mFlags & 4) {
        cam->mPathNode = 0;
    }
    idx = cam->mPathNode + 1;

    path.GetNode(node0, cam->mPathNode);
    path.GetNode(node1, idx);
    Vec3_Sub(&dir, &node1, &node0);
    segLen = LenVec3(&dir);
    NormalizeVec3IfNonZero(&dir);

    Vec3_Sub(&toPos, &cam->lookAt, &node0);
    Vec3_Sub(&toTarget, &cam->pos, &node0);
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
        Vec3_MulScalarInPlace(&tmp, cstd::fdiv(0x190000, LenVec3(&tmp)));
        Vec3_Add(&farPoint, &node1, &tmp);
        Vec3_Sub(&diffFar, &farPoint, &cam->lookAt);
        tmp.x = diffFar.x;
        tmp.y = diffFar.y;
        tmp.z = diffFar.z;

        Vec3_MulScalar(&back, &dir, -0x190000);
        Vec3_Add(&farBack, &node1, &back);
        Vec3_Sub(&diffBack, &farBack, &cam->lookAt);

        lenBack = LenVec3(&diffBack);
        lenFwd = LenVec3(&tmp);
        if (lenFwd < lenBack) {
            cam->mPathNode++;
        }
    } else if (path.Loops() != 0) {
        if (projDir > segLen)
            projDir = segLen;
    }

    if (cam->mPathNode != 0 && cam->mPathNode != idx) {
        Vec3_MulScalar(&vecFwd2, &dir, 0x2bc000);
        Vec3_Add(&farFwd2, &node0, &vecFwd2);
        Vec3_Sub(&diffFwd2, &farFwd2, &cam->lookAt);

        path.GetNode(prevNode, cam->mPathNode - 1);
        Vec3_Sub(&diffPrev, &prevNode, &node0);
        Vec3_MulScalarInPlace(&diffPrev, cstd::fdiv(0x190000, LenVec3(&diffPrev)));
        Vec3_Add(&farPrev, &node0, &diffPrev);
        Vec3_Sub(&diffFarPrev, &farPrev, &cam->lookAt);
        diffPrev.x = diffFarPrev.x;
        diffPrev.y = diffFarPrev.y;
        diffPrev.z = diffFarPrev.z;

        lenA = LenVec3(&diffFwd2);
        lenB = LenVec3(&diffPrev);
        if (lenB < lenA) {
            cam->mPathNode--;
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

    if (data_0209f2f8 == 0x2f) {
        projSide = projSide - FX_MUL(cam->mFocusDist, 0x5000);
        if (projSide < 0) {
            targetPos.y -= FX_MUL(projSide, 0x400);
            projSide = 0;
        }
    } else {
        projSide = FX_MUL(projSide, v4);
    }

    Vec3_MulScalarInPlace(&side, projSide);
    AddVec3(&targetPos, &side, &targetPos);

    if (cam->mFlags & 4) {
        cam->pos.x = targetPos.x;
        cam->pos.y = targetPos.y;
        cam->pos.z = targetPos.z;
    } else {
        func_020092c4(cam, &cam->pos, &targetPos);
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
}

// @symbol func_0200af20
/* Render's view from mFixedViewPos: v1 halfway up from the fixed point to
   the target, v2 far out and above the target on the bearing from v1, boxed
   by data_02086f38 in level 6 and data_02086f48 in level 0x15. */
void func_0200af20(dCamera_c *cam, Vector3 *v1, Vector3 *v2, short *out) {
    Vector3 *f = (Vector3 *)cam->mFixedViewPos;
    int *b;
    unsigned short ang;
    int idx;
    int y;
    int z;

    if (f == 0)
        return;

    b = 0;
    {
        signed char g = data_0209f2f8;
        if (g == 6) {
            b = data_02086f38;
        } else if (g == 0x15) {
            if (cam->mTargetPos.y < 0)
                return;
            b = data_02086f48;
        }
    }

    z = f->z;
    y = f->y;
    y = y + cam->mTargetPos.y;
    v1->x = f->x;
    v1->y = y >> 1;
    v1->z = z;
    ang = Vec3_HorzAngle(v1, &cam->mTargetPos);
    idx = (ang >> 4) * 2;

    v2->y = cam->mTargetPos.y + 0x8f7690;
    v2->x = cam->mTargetPos.x + FX_MUL(data_02082214[idx], 0x15a6f80);
    v2->z = cam->mTargetPos.z + FX_MUL(data_02082214[idx + 1], 0x15a6f80);

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
    data_0209f43c.Func_020156DC(cam->mAspect, *out, cam->mNear, cam->mFar);
    cam->mFlags &= ~1;
}

// @symbol func_0200af0c
int func_0200af0c(dCamera_c *cam) {
    cam->mMode = (CamMode *)&data_0208715c;
    return 1;
}

/* func_02009e70's helpers. */
static inline void SetVec(struct Vector3 &v, s32 x, s32 y, s32 z) {
    v.x = x;
    v.y = y;
    v.z = z;
}

static inline int CheckMode(void) {
    return data_0209f2d8 == 1;
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

// @symbol func_02009e70
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
s32 func_02009e70(dCamera_c *cam) {
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

    if (cam->mTargetPlayer == 0) {
        cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    } else {
        sp1c = (cam->mFlags & 0x100) ? (CamMode *)&data_0208747c : cam->mMode;
        Vec3_Sub(&sp34, &cam->mTargetPlayer->mPos, &cam->mTargetPos);
        {
            struct Vector3 *q = &cam->mTargetPlayer->mPos;
            cam->mTargetPos.x = q->x;
            cam->mTargetPos.y = q->y;
            cam->mTargetPos.z = q->z;
        }
        r7 = Vec3_HorzLen(&sp34);
        sp20 = 0;
        sp24 = 0;
        if (cam->mFocusActor != 0) {
            struct Vector3 *q;
            sp24 = 1;
            cam->mFramedActor = 0;
            q = &cam->mFocusActor->mPos;
            cam->mLookTargetPos.x = q->x;
            cam->mLookTargetPos.y = q->y;
            cam->mLookTargetPos.z = q->z;
            cam->mFocusDist = 0xdfe60;
        }
        r6 = func_0200c66c(cam, &cam->mTargetPos, (int *)&sp20, (int *)&sp1c, &sp24);
        r4 = 0;
        if (sp24 != 0) {
            r4 = 0x1000;
            SubVec3(&cam->mLookTargetPos, &cam->mTargetPos, &cam->mLookTargetPos);
        }
        Math_Function_0203b14c(&cam->mFocusBlend, r4, 0x100, 0x80, 0x10);

        fp = sp1c->flags;
        r4 = sp1c->minDist;
        r5 = sp1c->posDist;
        if ((fp & 1) == 0 && (cam->mFlags & 2) != 0) {
            r4 = FX_MUL(r4, 0x1800);
            r5 = FX_MUL(r5, 0x1800);
        }
        if (CheckMode() && (fp & 2) != 0 && func_02009374(cam) != 0) {
            r4 = FX_MUL(r4, 0x14cc);
            r5 = FX_MUL(r5, 0x14cc);
        }
        if (cam->mFlags & 0x80) {
            r4 = FX_MUL(r4, 0x1800);
            r5 = FX_MUL(r5, 0x1800);
        }
        /* near limit -> fp (reuses the flags local), far limit -> r4 (reuses the
           near-distance local): both must be later live ranges of already-assigned
           locals for the allocator to colour them fp/sl. */
        t0 = cam->mFocusDist - r4;
        fp = func_020093f4(cam, r4 + FX_MUL(t0, cam->mFocusBlend));
        if (sp24 != 0) {
            r4 = fp;
        } else {
            if (r5 < 0)
                r5 = -r5;
            r4 = func_020093f4(cam, r5);
        }
        sp04 = func_0200bec4((char *)cam, &cam->mTargetPos.x, r6, (char *)sp1c, r7);
        kf = *(u16 *)((char *)data_0209f49e + gActivePlayerSlot * 0x18) & 0x4300;
        if (cam->mFocusActor != 0) {
            cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
        } else if (sp1c == (CamMode *)&data_0208733c) {
            func_0200bb28(cam, sp20);
            kf = 0;
        } else if (sp1c == (CamMode *)&data_020873dc) {
            func_0200b798(cam, sp20, r6);
        } else if ((cam->mFlags & 0x400) == 0) {
            if (sp1c == (CamMode *)&data_02087404) {
                func_0200b590(cam, sp20, r6);
            } else if (sp1c == (CamMode *)&data_0208742c) {
                func_0200b0fc(cam, sp20, r6);
            } else {
                cam->mFlags |= 0x1000;
                sp08 = func_020093f4(cam, sp1c->nearDist);
                sp0c = func_020093f4(cam, sp1c->farDist);
                if (sp20 != 0 && sp20->type == 2) {
                    if (sp20->p1 == 0) {
                        s32 tx = sp20->x << 12;
                        s32 tz = sp20->z << 12;
                        s32 ty = sp20->y << 12;
                        cam->pos.x = tx;
                        cam->pos.y = ty;
                        cam->pos.z = tz;
                    } else {
                        s16 a = (s16)(sp20->angleY + 0x8000);
                        if (cam->mTargetAngleY != a) {
                            cam->mTargetAngleY = a;
                            cam->mTurningToTarget = 1;
                        }
                    }
                } else {
                    if (cam->mTurningToTarget == 0) {
                        cam->mTargetAngleY += 0x8000;
                    }
                }
                Vec3_Sub(&sp40, &cam->pos, &cam->lookAt);
                sp28 = LenVec3(&sp40);
                {
                    sp10 = 0x80000000;
                    Vector3 sp4c(cam->mTargetPos.x, cam->mTargetPos.y + 0x3e8000,
                                 cam->mTargetPos.z);
                    Vector3 sp58;
                    Vector3 sp64;
                    ceil = 0x7fffffff;
                    sb = cam->mTargetPos.y + 0x82500;
                    {
                        dBgCh_Lin line;
                        func_0200897c(cam, &line);
                        line.SetObjAndLine(cam->mTargetPos, sp4c, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = CollisionCeiling(line, sb);
                        }
                        if ((cam->mFlags & 0x100) == 0 && (sp1c->flags & 8) != 0) {
                            {
                                s32 tz = cam->mTargetPos.z;
                                s32 tx = cam->mTargetPos.x;
                                sp58.x = tx;
                                sp58.y = sb;
                                sp58.z = tz;
                            }
                            if (sb <= ceil) {
                                line.SetObjAndLine(sp58, cam->pos, 0);
                                if (line.DetectClsn() != 0) {
                                    line.surface.CopyNormalTo(sp64);
                                    SetCollisionPosition(cam, line);
                                    cam->mWallNormal.x = sp64.x;
                                    cam->mWallNormal.y = sp64.y;
                                    cam->mWallNormal.z = sp64.z;
                                    t0 = sp64.y;
                                    if (t0 < 0)
                                        t0 = -t0;
                                    if (t0 > 0xb50) {
                                        cam->mFlags |= 0x80200;
                                    } else {
                                        Vector3 sp70 = line.GetClsnPos();
                                        sba = cstd::atan2(sp64.x, sp64.z) + 0x8000;
                                        diff = sba - cam->mAngleY;
                                        sba += (diff < 0) ? 0x4000 : -0x4000;
                                        if (cam->mTurningToTarget != 0) {
                                            if (AngleDiff(sba, cam->mTargetAngleY) >= 0x4000)
                                                sba = cam->mTargetAngleY;
                                        }
                                        cam->mTargetAngleY = sba;
                                        cam->mTurningToTarget = 1;
                                        t0 = (s16)(cam->mAngleY - cam->mTargetAngleY);
                                        if (kf & 0x200)
                                            t0 = (s16)(-t0);
                                        if (t0 > 0)
                                            kf = 0;
                                        cam->mFlags |= 0x40000;
                                    }
                                }
                            }
                        }
                        Vector3 sp7c;
                        Vector3 sp88;
                        if (kf != 0) {
                            if (kf & 0x4000)
                                func_02012790(0x1a);
                            cam->mTurningToTarget = 0;
                        } else {
                            if (cam->mTurningToTarget == 0) {
                                kf = *(u16 *)((char *)data_0209f49c + gActivePlayerSlot * 0x18) &
                                     0x4300;
                            }
                        }
                        if (cam->mFlags & 0x100) {
                            if (sp28 > (s32)func_020093f4(cam, 0xdfe60))
                                cam->mFlags &= ~0x100;
                        }
                        /* sb = sp28 is assigned inside the test: hoisting it above the if
                           re-lays the function (290 words differ). */
                        if (kf != 0 || cam->mRecentering != 0 || cam->mTurningToTarget != 0 ||
                            r7 <= sp08 || (sb = sp28) < fp || sb <= sp0c) {
                            sb = cam->mAngleY;
                            cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
                            if (kf != 0 || cam->mRecentering != 0) {
                                if ((kf & 0x4000) || cam->mRecentering != 0) {
                                    if (kf != 0) {
                                        cam->mBehindAngleY = cam->mTargetPlayer->mAngleY + 0x8000;
                                    }
                                    cam->mRecentering =
                                        ApproachAngle(&cam->mAngleY, cam->mBehindAngleY, 4, 0x800,
                                                      0x400) != 0;
                                    if (cam->mRecentering != 0)
                                        cam->mFlags |= 0x60;
                                } else {
                                    if (kf & 0x200) {
                                        cam->mAngleY -= 0x400;
                                        cam->mFlags |= 0x20;
                                    } else {
                                        cam->mAngleY += 0x400;
                                        cam->mFlags |= 0x40;
                                    }
                                }
                                kf = 0;
                            } else if (cam->mTurningToTarget != 0) {
                                if (ApproachAngle(&cam->mAngleY, cam->mTargetAngleY, 0x10, 0x200,
                                                  0x100) == 0)
                                    cam->mTurningToTarget = 0;
                            } else {
                                m = sp1c;
                                if (m == (CamMode *)&data_020871ac ||
                                    m == (CamMode *)&data_020871d4) {
                                    func_0200b990(cam, sp20, r7);
                                } else if (sp24 == 0) {
                                    if (m == (CamMode *)&data_0208724c)
                                        t0 = cam->mTargetPlayer->mPrevAngleY;
                                    else
                                        t0 = cam->mTargetPlayer->mAngleY;
                                    t0 = (s16)(t0 + 0x8000);
                                    if (m == (CamMode *)&data_020874cc) {
                                        cam->mAngleY = t0 + cam->mBaseAngleOffset;
                                    } else if (sp28 <= sp0c) {
                                        ApproachAngle(&cam->mAngleY, t0, 0x14, 0x600, 0x28);
                                    } else if (m->flags & 0x100) {
                                        cam->mAngleY = sb;
                                    } else if (r7 <= sp08) {
                                        ApproachAngle(&cam->mAngleY, t0, 0x14, 0x64, 0x28);
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
                                    sp40.y = FX_MUL(sp28, -data_02082214[t0]);
                                    t0 = cstd::fdiv(FX_MUL(sp28, data_02082214[t0 + 1]), fp);
                                    sp40.x = FX_MUL(sp40.x, t0);
                                    sp40.z = FX_MUL(sp40.z, t0);
                                }
                                Math_Function_0203b14c(&sp28, r4, 0x400, r7 + 0x20000, 0x100);
                                Vec3_MulScalarInPlace(&sp40, cstd::fdiv(sp28, sp14));
                            } else if (sp28 < fp) {
                                Math_Function_0203b14c(&sp28, fp, 0x400, r7 + 0x20000, 0x100);
                                {
                                    s64 distanceSq = sp28;
                                    distanceSq *= sp28;
                                    distanceSq -= (s64)sp40.y * sp40.y;
                                    r7 = cstd::sqrt((u64)distanceSq);
                                }
                                t0 = cstd::fdiv(r7, Vec3_HorzLen(&sp40));
                                sp40.x = FX_MUL(sp40.x, t0);
                                sp40.z = FX_MUL(sp40.z, t0);
                                cam->mInputYawLag -= cam->mAngleY - sb;
                            }
                            t0 = Vec3_HorzLen(&sp40);
                            if (t0 < 0x1000)
                                t0 = 0x1000;
                            sp40.z = t0;
                            sp40.x = 0;
                            Vec3_RotateYAndTranslate(&cam->pos, &cam->lookAt, cam->mAngleY, &sp40);
                        } else {
                            if (sb > r4) {
                                Math_Function_0203b14c(&sp28, r4, 0x400, r7 + 0x20000, 0x100);
                                Vec3_MulScalarInPlace(&sp40, cstd::fdiv(sp28, sb));
                                MoveCameraPosition(cam, sp40);
                            }
                        }
                        if (func_02008cb4(cam) != 0) {
                            if (cam->mFlags & 0x60) {
                                kf = 1;
                                cam->mFlags &= ~0x60;
                            }
                        }
                        SetVec(sp4c, cam->pos.x, cam->pos.y + 0x100000, cam->pos.z);
                        line.SetObjAndLine(cam->pos, sp4c, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = LimitCameraCeiling(line, sp4c, ceil);
                        }
                        Vec3_Add(&sp7c, &cam->pos, &cam->lookAt);
                        Vec3_AsrInPlace(&sp7c, 1);
                        sp88.x = sp7c.x;
                        sp88.y = sp7c.y + 0x100000;
                        sp88.z = sp7c.z;
                        line.SetObjAndLine(sp7c, sp88, 0);
                        if (line.DetectClsn() != 0) {
                            ceil = LimitMidpointCeiling(line, sp88, ceil);
                        }
                        s32 y = cam->mTargetPos.y;
                        dBgCh_Gnd ground;
                        ground.mProbeHeight = 0x5dc000;
                        func_0200897c(cam, &ground);
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
                            r7h = r7h +
                                  func_020093d4(cam, (r6 - sp04) +
                                                         ((cam->mFlags & 2) ? 0x1e8000 : 0x138000));
                            if (sp1c->flags & 0x80) {
                                if (cam->pos.y > (s32)r7h)
                                    r7h = cam->pos.y;
                            }
                            sp2c = sp1c->minRiseSlope;
                            sp30 = sp1c->maxRiseSlope;
                            func_0200c9e0(cam, &sp2c, &sp30);
                            sp40.y = r7h - cam->lookAt.y;
                            if (sp1c == (CamMode *)&data_0208706c && sp34.y < 0)
                                t0 = 0xa49;
                            else
                                t0 = sp2c;
                            r6 = FX_MUL(sp28, t0);
                            t0 = FX_MUL(sp28, sp30);
                            if (sp40.y < r6)
                                r7h = cam->lookAt.y + r6;
                            else if (sp40.y > t0)
                                r7h = cam->lookAt.y + t0;
                            if (data_0209f2f8 == 0x26) {
                                if (cam->pos.y < sb + 0x25bc8)
                                    cam->pos.y = sb + 0x25bc8;
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
                        Math_Function_0203b14c(&cam->pos.y, r7h, 0x300, 0x14000, 0x100);
                        cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
                    }
                }
            }
        }
        if (kf != 0) {
            if (data_0209b000 == 0)
                func_02012790(0xe);
            data_0209b000 = kf;
        } else {
            data_0209b000 = 0;
            if (cam->mFlags & 0x60) {
                cam->mTurnSound = Sound_PlayIfNotActive(cam->mTurnSound, 2, 0x43, 0);
            }
        }
    }
    cam->mAngleX = Vec3_VertAngle(&cam->lookAt, &cam->pos);
    return 1;
}

/* The states' entry ops: each switches to mode record N of the table at
   0x02086fcc. */
// @symbol func_02009e50
int func_02009e50(dCamera_c *cam) {
    func_0200cb58(cam, 1);
    return 1;
}

// @symbol func_02009e30
int func_02009e30(dCamera_c *cam) {
    int dummy = 0;
    (void)&dummy;
    func_0200cb58(cam, 0);
    return 1;
}

// @symbol func_02009e10
int func_02009e10(dCamera_c *cam) {
    func_0200cb58(cam, 6);
    return 1;
}

// @symbol func_02009df0
int func_02009df0(dCamera_c *cam) {
    func_0200cb58(cam, 2);
    return 1;
}

// @symbol func_02009dd0
int func_02009dd0(dCamera_c *cam) {
    func_0200cb58(cam, 3);
    return 1;
}

// @symbol func_02009db0
int func_02009db0(dCamera_c *cam) {
    func_0200cb58(cam, 4);
    return 1;
}

// @symbol func_02009d90
int func_02009d90(dCamera_c *cam) {
    volatile int dummy __attribute__((unused));
    func_0200cb58(cam, 7);
    return 1;
}

// @symbol func_02009d70
int func_02009d70(dCamera_c *cam) {
    func_0200cb58(cam, 0x10);
    return 1;
}

// @symbol func_02009d30
/* Entry to the behind-the-player state: mode data_0208729c, turning to the
   yaw behind the target player. */
int func_02009d30(dCamera_c *cam) {
    int one = 1;

    cam->mMode = (CamMode *)&data_0208729c;

    cam->mTurningToTarget = (unsigned char)one;

    {
        dActor_c *player = cam->mTargetPlayer;
        short h = player->mAngleY;
        cam->mTargetAngleY = (short)(h + 0x8000);
        cam->mFlags &= ~0x100u;
    }

    return one;
}

// @symbol func_02009aa8
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

        r = func_020091f8(cam, (Vector3 *)tmp, cam->mTargetAngleY, 0);
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

    func_02009138(cam, func_020093f4(cam, 0x48000));
    cam->mFlags |= 0x80000;
    func_020089d8(cam);

    return 1;
}

// @symbol func_02009a8c
int func_02009a8c(dCamera_c *cam) {
    cam->SaveCameraStateBeforeTalk();
    return func_02009d30(cam);
}

// @symbol func_020098e0
/* The look-around state: turn to data_02086e9c first, then let the stylus
   axes swing mAngleY up to 0x5000 off mTargetAngleY and the pitch up to
   0x1c00. */
int func_020098e0(dCamera_c *cam) {
    if (cam->mTurningToTarget != 0) {
        int pos[3];
        int src[3];
        int result;
        src[0] = data_02086e9c[0];
        src[1] = data_02086e9c[1];
        src[2] = data_02086e9c[2];

        {
            dActor_c *player = cam->mTargetPlayer;
            Vec3_RotateYAndTranslate(pos, &player->mPos, player->mAngleY, src);
        }

        result = func_020091f8(cam, (Vector3 *)pos, cam->mTargetAngleY, 0);
        if (result != 0) {
            cam->mTurningToTarget = 0;
        }
    } else {
        int idx = gActivePlayerSlot;
        int off = idx * 0x18;
        s16 t1 = *(short *)((char *)data_0209f4a2 + off);
        int scaled1 = FX_MUL(t1, 0x200);

        cam->mAngleY -= scaled1;

        {
            s16 a186 = cam->mTargetAngleY;
            s16 diff = (s16)(cam->mAngleY - a186);
            if (diff < -0x5000)
                diff = -0x5000;
            else if (diff > 0x5000)
                diff = 0x5000;
            cam->mAngleY = (s16)(a186 + diff);
        }

        {
            s16 t2 = *(short *)((char *)data_0209f4a4 + off);
            int scaled2 = FX_MUL(t2, 0x200);
            cam->mAngleX += scaled2;
        }

        {
            s16 clamped = cam->mAngleX;
            if (clamped < -0x1c00)
                clamped = -0x1c00;
            else if (clamped > 0x1c00)
                clamped = 0x1c00;
            cam->mAngleX = clamped;
        }
    }

    {
        unsigned int r = func_020093f4(cam, 0x48000);
        func_02009138(cam, (int)r);
    }
    func_020089d8(cam);
    return 1;
}

// @symbol func_020098b8
/* Entry to state 0x13: arm mStateTimer, which func_0200987c counts down. */
int func_020098b8(dCamera_c *cam) {
    func_0200cb58(cam, 0x13);
    cam->mStateTimer = 0x1c;
    return 1;
}

// @symbol func_0200987c
/* State 0x13: follow until mStateTimer runs out, then switch to state
   0x14. */
void func_0200987c(dCamera_c *cam) {
    if (DecIfAbove0_Short(&cam->mStateTimer) == 0) {
        func_0200cb58(cam, 0x14);
    }
    func_02009e70(cam);
}

// @symbol func_0200985c
int func_0200985c(dCamera_c *cam) {
    func_0200cb58(cam, 0x17);
    return 1;
}

// @symbol func_020097ec
/* Entry to the first-person state: pos at the player, facing its way. */
int func_020097ec(dCamera_c *cam) {
    Vector3 *src;

    func_0200cb58(cam, 0x18);
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

// @symbol func_020095e4
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

// @symbol func_020095c4
int func_020095c4(dCamera_c *cam) {
    func_0200cb58(cam, 0x1d);
    return 1;
}

// @symbol func_02009540
/* The exit-viewing state: turn toward mLookTargetPos, carry it toward
   data_02086ea8 turned by mTargetAngleY around mLookTarget, and orbit at
   0x300000. */
int func_02009540(dCamera_c *cam) {
    func_020091f8(cam, &cam->mLookTargetPos, cam->mTargetAngleY, 0);

    /* stack layout: 12-byte temporary vector, then three ints */
    Vector3 goal;
    int offset[3];

    offset[0] = data_02086ea8[0];
    offset[1] = data_02086ea8[1];
    offset[2] = data_02086ea8[2];

    Vec3_RotateYAndTranslate(&goal, cam->mLookTarget, cam->mTargetAngleY, offset);

    ApproachLinear(cam->mLookTargetPos, goal, 0x8000);

    func_02009138(cam, 0x300000);

    return 1;
}

// @symbol func_020094e4
int func_020094e4(dCamera_c *cam) {
    int buf[3];
    func_0200cb58(cam, 0x1f);
    buf[0] = data_02086eb4[0];
    buf[1] = data_02086eb4[1];
    buf[2] = data_02086eb4[2];
    short angle = cam->mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&cam->pos, &cam->mTargetPos, angle, buf);
    return 1;
}

// @symbol func_020094c0
/* Frame the target player as the focus actor for this frame (Behavior
   clears it again) and follow. */
void func_020094c0(dCamera_c *cam) {
    cam->mFocusActor = cam->mTargetPlayer;
    cam->mFlags |= 4;
    func_02009e70(cam);
}

// @symbol func_020094a8
int func_020094a8(dCamera_c *cam) {
    cam->mPathIndex = 0;
    cam->mPathProgress = 0;
    return 1;
}

// @symbol func_02009414
/* Walk pos along mPosPath and lookAt along mLookAtPath (func_02008b4c,
   sharing one segment index and progress), then re-aim. Returns 0 once
   either path has run out. */
int func_02009414(dCamera_c *cam) {
    int r4 = 1;
    void *tbl1 = cam->mPosPath;

    if (tbl1 != 0) {
        r4 = func_02008b4c(&cam->pos, &cam->mPathIndex, &cam->mPathProgress, tbl1);
    }

    void *tbl2 = cam->mLookAtPath;
    if (tbl2 != 0) {
        if (func_02008b4c(&cam->lookAt, &cam->mPathIndex, &cam->mPathProgress, tbl2) == 0) {
            r4 = 0;
        }
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    cam->mAngleX = Vec3_VertAngle(&cam->lookAt, &cam->pos);

    func_020089f8(cam);
    return r4;
}

// @symbol func_020093f4
/* Scale a horizontal distance by the field of view (mInvSinFov). */
unsigned int func_020093f4(void *self, int x) {
    dCamera_c *cam = (dCamera_c *)self;
    long long prod = (long long)x * (long long)cam->mInvSinFov;
    long long sum = prod + 0x800LL;
    unsigned int lo = (unsigned int)sum;
    unsigned int hi = (unsigned int)(sum >> 32);
    return (lo >> 12) | (hi << 20);
}

// @symbol func_020093d4
/* Scale a height by the field of view (mInvCosFov). */
unsigned int func_020093d4(void *self, int a) {
    dCamera_c *cam = (dCamera_c *)self;
    unsigned long long prod = (unsigned long long)a * (unsigned long long)cam->mInvCosFov;
    unsigned long long sum = prod + 0x800u;
    unsigned int lo = (unsigned int)sum;
    unsigned int hi = (unsigned int)(sum >> 32);
    return (lo >> 12) | (hi << 20);
}

// @symbol func_02009374
/* Is any of the twelve star markers within 0x8000 of the target? */
int func_02009374(dCamera_c *cam) {
    int i;
    for (i = 0; i < 12; i++) {
        dActor_c *marker = STAR_MARKERS[i];
        if (marker != 0) {
            if (func_0203cf94(&cam->mTargetPos.x, &marker->mPos.x) < 0x40000000LL) {
                return 1;
            }
        }
    }
    return 0;
}

// @symbol func_020092c4
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
    Vec3_MulScalarInPlace(&d, cstd::fdiv(scratch, len));
    {
        struct Vector3 res;
        Vec3_Add(&res, target, &d);
        out->x = res.x;
        out->y = res.y;
        out->z = res.z;
    }
    return r5 == 0 ? 1 : 0;
}

// @symbol func_0200928c
void func_0200928c(dCamera_c *cam) {
    short h = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    cam->mAngleY = h;

    short v = Vec3_VertAngle(&cam->lookAt, &cam->pos);
    cam->mAngleX = v;
}

// @symbol func_020091f8
/* Ease lookAt toward the given point and the two angles toward the given
   yaw and pitch. Returns 1 once all three have arrived. */
int func_020091f8(dCamera_c *cam, Vector3 *lookAt, int angleY, int angleX) {
    int r4;
    int t;
    r4 = (func_020092c4(cam, &cam->lookAt, lookAt) != 1);
    func_0200928c(cam);
    t = ApproachAngle(&cam->mAngleY, angleY, 8, 0x4000, 0x80);
    if (t != 0)
        r4 = 1;
    t = ApproachAngle(&cam->mAngleX, angleX, 8, 0x4000, 0x80);
    return (r4 | t) == 0;
}

// @symbol func_02009138
/* Ease the lookAt-to-pos distance toward dist and rebuild pos from lookAt
   and the two angles. Returns 1 on arrival. */
int func_02009138(dCamera_c *cam, int dist) {
    struct Vector3 v;
    int len;
    int r;
    Vec3_Sub(&v, &cam->pos, &cam->lookAt);
    len = LenVec3(&v);
    r = Math_Function_0203b14c(&len, dist, 0x300, 0x60000, 0x200);
    v.x = 0;
    v.y = 0;
    v.z = len;
    Matrix4x3_FromTranslation(&data_020a0e68, cam->lookAt.x, cam->lookAt.y, cam->lookAt.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, cam->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, cam->mAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &cam->pos);
    return (r == 0) ? 1 : 0;
}

// @symbol func_02008cb4
/* Keep pos out of walls: probe from above the target (flags 0x40200) and
   from the previous pos, and pull pos in to the hit. A hit facing the same
   way as mWallNormal, or within 0x80000 of mWallPos, is ignored. */
int func_02008cb4(dCamera_c *cam) {
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
        dActor_c *p = cam->mTargetPlayer;
        int b = p->actorID;
        b = b == 0xbf;
        if (b != false) {
            if (func_ov002_020ca270((char *)p) != 0)
                return 0;
        }
    }

    scale = 0x200;
    if (cam->mFlags & 0x40200) {
        vz = cam->mTargetPos.z;
        vy = cam->mTargetPos.y + 0x82500;
        vx = cam->mTargetPos.x;
        v0.x = vx;
        v0.y = vy;
        v0.z = vz;
        Vec3_Sub(&dir1, &cam->pos, &v0);
        len = LenVec3(&dir1);
        if (len != 0) {
            Vec3_MulScalarInPlace(&dir1, cstd::fdiv(0x100000, len));
            Vec3_Add(&end1, &cam->pos, &dir1);
            dBgCh_Lin rc1;
            func_0200897c(cam, &rc1);
            rc1.SetObjAndLine(end1, v0, 0);
            if (rc1.DetectClsn() != 0) {
                rc1.surface.CopyNormalTo(n1);
                if (DotVec3(&cam->mWallNormal, &n1) > 0) {
                    cam->mFlags &= ~0x80000;
                    return 0;
                }
                Vector3 cp1 = rc1.GetClsnPos();
                if (Vec3_Dist(&cam->mWallPos, &cp1) < 0x80000) {
                    cam->mFlags &= ~0x80000;
                    return 0;
                }
                {
                    u32 f = cam->mFlags;
                    if ((f & 0x40000) && cam->mTurningToTarget != 0 && !(f & 0x80100)) {
                        int a = cstd::atan2(n1.x, n1.z);
                        if (AngleDiff(a, cstd::atan2(cam->mWallNormal.x, cam->mWallNormal.z)) >
                            0x6000) {
                            cam->mTargetAngleY += 0x8000;
                            cam->mFlags |= 0x80000;
                        }
                    }
                }
            }
        }
        scale = 0x1000;
    }

    {
        u32 f = cam->mFlags;
        if (f & 0x80100) {
            vz = cam->mTargetPos.z;
            vy = (f & 0x100) ? cam->pos.y : 0x82500 + cam->mTargetPos.y;
            vx = cam->mTargetPos.x;
            v0.x = vx;
            v0.y = vy;
            v0.z = vz;
        } else {
            scale = 0x1000;
            v0.x = cam->mPrevPos.x;
            v0.y = cam->mPrevPos.y;
            v0.z = cam->mPrevPos.z;
        }
    }

    Vec3_Sub(&dir2, &cam->pos, &v0);
    len = LenVec3(&dir2);
    if (len == 0) {
        cam->mFlags &= ~0x80000;
        return 0;
    }
    Vec3_MulScalarInPlace(&dir2, cstd::fdiv(0x60000, len));
    Vec3_Add(&end2, &cam->pos, &dir2);
    dBgCh_Lin rc2;
    func_0200897c(cam, &rc2);
    rc2.SetObjAndLine(v0, end2, 0);
    if (rc2.DetectClsn() == 0) {
        cam->mFlags &= ~0x80000;
        return 0;
    }
    if (!(cam->mFlags & 0x80100)) {
        int t;
        rc2.surface.CopyNormalTo(n2);
        t = n2.y;
        if (t < 0)
            t = -t;
        if (t > 0xb50) {
            return 0;
        }
    }
    d = Vec3_Dist(&cam->pos, &cam->lookAt);
    Vec3_MulScalarInPlace(&dir2, scale);
    Vector3 cp2 = rc2.GetClsnPos();
    Vector3 np;
    Vec3_Sub(&np, &cp2, &dir2);
    cam->pos.x = np.x;
    cam->pos.y = np.y;
    cam->pos.z = np.z;
    Vec3_Sub(&diff, &cam->pos, &cam->lookAt);
    if (LenVec3(&diff) >= d) {
        Vec3_MulScalarInPlace(&diff, cstd::fdiv(d, LenVec3(&diff)));
        Vector3 cl;
        Vec3_Add(&cl, &cam->lookAt, &diff);
        cam->pos.x = cl.x;
        cam->pos.y = cl.y;
        cam->pos.z = cl.z;
    }
    if (cam->mTurningToTarget != 0 || cam->mRecentering != 0) {
        cam->mTurningToTarget = 0;
        cam->mRecentering = 0;
    }
    cam->mFlags &= ~0x80000;
    return 1;
}

/* A path point: a negative flag ends the path; amt (whole units) feeds the
   fixed-point reciprocal that scales progress through the segment. */
typedef struct {
    signed char flag;
    unsigned char amt;
    short x, y, z;
} CamPathPoint;

/* The path table the script ops hand in (camera and demo paths): a point
   count, then the point array. */
typedef struct {
    unsigned char numPoints; /* 0x00 */
    unsigned char pad_01[3];
    CamPathPoint *points; /* 0x04 */
} CamPathTable;

// @symbol func_02008b4c
/* Move out along a path: a spline through the four points from
   *segIndex, at *segProgress, which advances by the points' rates and
   carries into the next segment. Returns 1 once the path has run out. */
int func_02008b4c(void *out, void *segIndex, void *segProgress, void *table) {
    short *p1 = (short *)segIndex;
    int *p2 = (int *)segProgress;
    int idx;
    CamPathPoint *elem;
    CamPathTable *path = (CamPathTable *)table;
    int i;
    Vec3 v[4];
    int lo, hi;
    int m;

    idx = *p1;
    if (idx + 3 >= path->numPoints) {
        return 1;
    }

    elem = path->points + idx;

    if (elem[0].flag < 0 || elem[1].flag < 0 || elem[2].flag < 0) {
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

    func_ov002_020effb8((Vector3 *)out, *p2, (Vector3 *)&v[0], (Vector3 *)&v[1], (Vector3 *)&v[2],
                        (Vector3 *)&v[3]);

    lo = 0;
    hi = 0;
    if (elem[1].amt != 0) {
        lo = func_02053200(elem[1].amt << 12);
    }
    if (elem[2].amt != 0) {
        hi = func_02053200(elem[2].amt << 12);
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

// @symbol func_02008b08
/* Ease mInputScale toward the active player's camera-input entry: slowly
   while it is nonzero, quickly back to 0. */
int func_02008b08(dCamera_c *cam) {
    s16 dest = *(s16 *)((char *)data_0209f4a0 + gActivePlayerSlot * 0x18);
    return ApproachLinear2(cam->mInputScale, dest, dest != 0 ? 0xa0 : 0x400);
}

// @symbol func_02008abc
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

// @symbol func_02008a70
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

// @symbol func_020089f8
/* Copy the target player's position into mTargetPos and keep lookAt's
   offset from it in mLookAtOffset. */
void func_020089f8(void *self) {
    dCamera_c *cam = (dCamera_c *)self;
    dActor_c *player;
    Vector3 *p;
    int tmp[3];

    player = cam->mTargetPlayer;
    if (player == 0) {
        return;
    }
    p = &player->mPos;
    cam->mTargetPos.x = p->x;
    cam->mTargetPos.y = p->y;
    cam->mTargetPos.z = p->z;
    Vec3_Sub(tmp, &cam->lookAt.x, &cam->mTargetPos.x);
    cam->mLookAtOffset.x = tmp[0];
    cam->mLookAtOffset.y = tmp[1];
    cam->mLookAtOffset.z = tmp[2];
    cam->mFocusBlend = 0;
}

// @symbol func_020089d8
/* Keep pos at least 0x25bc8 above the target. */
void func_020089d8(dCamera_c *cam) {
    int limit = cam->mTargetPos.y + 0x25bc8;
    if (cam->pos.y < limit) {
        cam->pos.y = limit;
    }
}

// @symbol func_0200897c
/* Prepare a collision query for the camera. It always stops ignoring
 * vanish surfaces, then starts again if the target is a vanished Player,
 * and finally runs func_02035468 on the query.
 *
 * Codegen note: the (actorID == 0xbf) test is held in an enum-typed boolean.
 * Under -O4 -enum int this materializes the comparison as a full 32-bit 0/1
 * value (cmp; moveq #1; movne #0; cmp #0; beq) instead of folding it into a
 * single branch -- that is what the ROM does. A plain `int` bool folds to a
 * bare `bne`; a `u8` bool masks with `ands #0xff`; only the (int-width) enum
 * bool gives the exact `cmp r0, #0`. */
void func_0200897c(void *self, void *arg) {
    dCamera_c *cam = (dCamera_c *)self;
    Player *owner;
    enum Bool isType;
    ((dBgCh *)arg)->StopIgnoringVanishSurfaces();
    owner = (Player *)cam->mTargetPlayer;
    isType = (enum Bool)(owner->actorID == 0xbf);
    if (isType && owner->mIsVanish != 0) {
        ((dBgCh *)arg)->StartIgnoringVanishSurfaces();
    }
    func_02035468((char *)arg);
}
}

/* The script ops, called through the member-function table at 0x0209b138
   with the command's operands. The class is opaque: only the pointer-to-
   member representation matters. */
typedef int (CamPmfHost::*CamScriptOp)(unsigned char *, int, int);

extern "C" {
extern int data_0209b004;
extern CamScriptOp data_0209b138[39];
extern CamScriptOp data_02086c80, data_02086d80, data_02086c60, data_02086c90, data_02086bd8,
    data_02086d18, data_02086d98, data_02086c88, data_02086d00, data_02086bc0, data_02086e30,
    data_02086ce8, data_02086e38, data_02086e20, data_02086cd0, data_02086e08, data_02086df0,
    data_02086cb8, data_02086dd8, data_02086dc0, data_02086c20, data_02086da0, data_02086d60,
    data_02086d88, data_02086cf8, data_02086d68, data_02086c78, data_02086d40, data_02086d38,
    data_02086d20, data_02086c50, data_02086d08, data_02086cf0, data_02086c38, data_02086cd8,
    data_02086cc0, data_02086cb0, data_02086ca8, data_02086ca0;

// @symbol func_02008550
/* Run one camera script command: byte 6 picks the op, and the operands
   follow from byte 7. The table is built on first use. */
int func_02008550(dCamera_c *cam, u8 *command, int first, int last) {
    if ((data_0209b004 & 1) == 0) {
        data_0209b004 |= 1;
        data_0209b138[0] = data_02086c80;
        data_0209b138[1] = data_02086d80;
        data_0209b138[2] = data_02086c60;
        data_0209b138[3] = data_02086c90;
        data_0209b138[4] = data_02086bd8;
        data_0209b138[5] = data_02086d18;
        data_0209b138[6] = data_02086d98;
        data_0209b138[7] = data_02086c88;
        data_0209b138[8] = data_02086d00;
        data_0209b138[9] = data_02086bc0;
        data_0209b138[10] = data_02086e30;
        data_0209b138[11] = data_02086ce8;
        data_0209b138[12] = data_02086e38;
        data_0209b138[13] = data_02086e20;
        data_0209b138[14] = data_02086cd0;
        data_0209b138[15] = data_02086e08;
        data_0209b138[16] = data_02086df0;
        data_0209b138[17] = data_02086cb8;
        data_0209b138[18] = data_02086dd8;
        data_0209b138[19] = data_02086dc0;
        data_0209b138[20] = data_02086c20;
        data_0209b138[21] = data_02086da0;
        data_0209b138[22] = data_02086d60;
        data_0209b138[23] = data_02086d88;
        data_0209b138[24] = data_02086cf8;
        data_0209b138[25] = data_02086d68;
        data_0209b138[26] = data_02086c78;
        data_0209b138[27] = data_02086d40;
        data_0209b138[28] = data_02086d38;
        data_0209b138[29] = data_02086d20;
        data_0209b138[30] = data_02086c50;
        data_0209b138[31] = data_02086d08;
        data_0209b138[32] = data_02086cf0;
        data_0209b138[33] = data_02086c38;
        data_0209b138[34] = data_02086cd8;
        data_0209b138[35] = data_02086cc0;
        data_0209b138[36] = data_02086cb0;
        data_0209b138[37] = data_02086ca8;
        data_0209b138[38] = data_02086ca0;
    }
    return (((CamPmfHost *)cam)->*data_0209b138[command[6]])(command + 7, first, last);
}

// @symbol func_02008500
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

// @symbol func_020084b0
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

// @symbol func_02008474
/* Script op: set lookAt, then pos. */
int func_02008474(dCamera_c *cam, u8 *data, int a, int b) {
    func_02008500(cam, data);
    return func_020084b0(cam, data + 6, a, b);
}

// @symbol func_02008450
/* Script op: set mFovAngle. */
int func_02008450(dCamera_c *cam, u8 *data) {
    cam->mFovAngle = ReadUnalignedUshort(data);
    return 1;
}

// @symbol func_0200840c
/* Script op: step mFovAngle toward a target by a rate. */
int func_0200840c(dCamera_c *cam, u8 *data) {
    short v1 = ReadUnalignedShort(data);
    short v2 = ReadUnalignedShort(data + 2);
    ApproachLinear2(cam->mFovAngle, v1, v2);
    return 1;
}

// @symbol func_020082f8
/* Script op: grow the viewport from the given rectangle to the full screen
   over frames lo..hi, keeping mAspect in step. */
int func_020082f8(dCamera_c *cam, u8 *src, int lo, int hi) {
    Fix12i slope = cstd::fdiv((gKuppaScriptFrame - lo) << 12, (hi - lo) << 12);
    cam->mViewport[0] = (u8)(FX_MUL(((int)src[0] << 12), slope) >> 12);
    cam->mViewport[1] = (u8)(FX_MUL(((int)src[1] << 12), slope) >> 12);
    cam->mViewport[2] = (u8)((FX_MUL((((int)src[2] - 0xff) << 12), slope) >> 12) + 0xff);
    cam->mViewport[3] = (u8)((FX_MUL((((int)src[3] - 0xbf) << 12), slope) >> 12) + 0xbf);
    cam->mAspect = cstd::fdiv((((u8)cam->mViewport[2] - (u8)cam->mViewport[0]) + 1) << 12,
                              (((u8)cam->mViewport[3] - (u8)cam->mViewport[1]) + 1) << 12);
    return 1;
}

// @symbol func_020082a0
/* Script op: on its first frame, reset the path walk and take the lookAt
   and pos paths; every frame, walk them. */
int func_020082a0(dCamera_c *cam, u8 *src, int id) {
    if (id == gKuppaScriptFrame) {
        func_020094a8(cam);
        cam->mLookAtPath = (void *)ReadUnalignedInt(src);
        cam->mPosPath = (void *)ReadUnalignedInt(src + 4);
    }
    func_02009414(cam);
    return 1;
}

// @symbol func_02008200
/* Script op: from the first frame, ease savedLookAt.y toward a height
   between the script player's ground and its position, and keep lookAt.y
   at least that high. */
int func_02008200(dCamera_c *cam, u8 *data, int first) {
    Player *a;
    int diff, val, target;
    if (first == gKuppaScriptFrame)
        cam->savedLookAt.y = 0x190000;
    a = (Player *)FindScriptPlayer(0);
    diff = a->mPosY - a->mGroundY;
    val = FX_MUL(diff, 0x1199);
    target = a->mGroundY + 0x50000 + val;
    Math_Function_0203b0fc(&cam->savedLookAt.y, target, 0xa3, 0x7fffffff);
    if (cam->lookAt.y < cam->savedLookAt.y)
        cam->lookAt.y = cam->savedLookAt.y;
    return 1;
}

// @symbol func_0200814c
/* Script op: aim lookAt ahead of the script player, nearer the further it
   is below pos. */
int func_0200814c(dCamera_c *cam) {
    dActor_c *a;
    int diff, absdiff, val;
    Vector3 v;

    a = FindScriptPlayer(0);
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

// @symbol func_020080f0
/* Script op: ease pos.y and lookAt.y toward heights above demo actor 0x13. */
int func_020080f0(dCamera_c *cam) {
    dActor_c *tmp = FindDemoActor(0x13);
    Vector3 *r4 = &tmp->mPos;
    int v1 = r4->y;
    Math_Function_0203b0fc(&cam->pos.y, v1 + 0x23000, 0x51, 0x7fffffff);
    int v2 = r4->y;
    Math_Function_0203b0fc(&cam->lookAt.y, v2 + 0x7d000, 0x266, 0x7fffffff);
    return 1;
}

// @symbol func_020080b0
/* Script op: lookAt at demo actor 0x13, mScriptParam above it. */
int func_020080b0(dCamera_c *cam) {
    dActor_c *obj = FindDemoActor(0x13);
    Vector3 *base = &obj->mPos;

    int z = base->z;
    int y = base->y + cam->mScriptParam;
    int x = base->x;

    cam->lookAt.x = x;
    cam->lookAt.y = y;
    cam->lookAt.z = z;
    return 1;
}

// @symbol func_02008080
/* Script op: raise pos by demo actor 0x13's height. */
int func_02008080(dCamera_c *cam) {
    dActor_c *a;
    int *dst;

    a = FindDemoActor(0x13);
    dst = &cam->pos.y;
    *dst = *dst + a->mPosY;
    return 1;
}

// @symbol func_02008028
/* Script op: walk the paths, then ease pos.y toward demo actor 0x13's
   height above where the walk left it. */
int func_02008028(dCamera_c *cam) {
    Fix12i y;
    dActor_c *obj;

    y = cam->pos.y;
    func_02009414(cam);
    obj = FindDemoActor(0x13);
    Math_Function_0203b0fc(&y, cam->pos.y + obj->mPosY, 0x11e, 0x7fffffff);
    cam->pos.y = y;
    return 1;
}

// @symbol func_02008008
/* Script op: set mScriptParam. */
int func_02008008(dCamera_c *cam, u8 *data) {
    cam->mScriptParam = ReadUnalignedSignedInt(data);
    return 1;
}

// @symbol func_02007fcc
/* Script op: step mScriptParam toward a target by a rate. */
int func_02007fcc(dCamera_c *cam, u8 *data) {
    int v1 = ReadUnalignedSignedInt(data);
    int v2 = ReadUnalignedSignedInt(data + 4);
    ApproachLinear(cam->mScriptParam, v1, v2);
    return 1;
}

// @symbol func_02007f14
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

// @symbol func_02007e5c
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

// @symbol func_02007d58
/* Script op: clear the orbit offsets and seed mOrbitTargetYaw from
   mFixedViewPos, or per level and area. */
int func_02007d58(dCamera_c *cam) {
    int lvl;
    void *p;
    cam->mOrbitTargetYaw = 0;
    cam->mPitchOffset = 0;
    cam->mYawOffset = 0;
    p = cam->mFixedViewPos;
    if (p != 0) {
        cam->mOrbitTargetYaw = Vec3_HorzAngle(p, &cam->pos);
    }
    lvl = SublevelToLevel(data_0209f2f8);
    if (lvl == 3 && data_0209f224 == 4) {
        func_020071a8(cam, 0x5600, 0x800);
    } else if (lvl == 0xb && data_0209f224 == 2) {
        func_020071a8(cam, 0, 0x800);
    } else if (lvl == 9 && data_0209f224 == 1) {
        func_020071a8(cam, 0x2000, 0x800);
    } else if (lvl == 0xe && data_0209f224 == 3) {
        func_020071a8(cam, 0, 0x800);
    }
    return 1;
}

// @symbol func_02007d0c
/* Step each axis of self toward src by its own rate. self is a Vector3, not
   the camera: callers pass &camera->lookAt. */
void func_02007d0c(Vector3 *self, const Vector3 *src, int stepX, int stepY, int stepZ) {
    Math_Function_0203b0fc(&self->x, src->x, stepX, 0x7fffffff);
    Math_Function_0203b0fc(&self->y, src->y, stepY, 0x7fffffff);
    Math_Function_0203b0fc(&self->z, src->z, stepZ, 0x7fffffff);
}

// @symbol func_02007cec
/* The same with one rate for all three axes. Its callers' declaration
   says int, and the ROM hands back whatever func_02007d0c left in r0, so
   there is no return statement. */
int func_02007cec(void *dest, const Vector3 *src, int step) {
    func_02007d0c((Vector3 *)dest, src, step, step, step);
}

// @symbol func_02007c9c
/* Distance and the two angles from v0 to v1. */
void func_02007c9c(const Vector3 *v0, const Vector3 *v1, int *outDist, short *outVertAng,
                   short *outHorzAng) {
    *outDist = Vec3_Dist(v0, v1);
    *outVertAng = Vec3_VertAngle(v1, v0);
    *outHorzAng = Vec3_HorzAngle(v0, v1);
}

// @symbol func_02007c14
/* The inverse: res is trans plus mag along pitch ang, turned by angY. */
void func_02007c14(Vector3 *res, const Vector3 *trans, Fix12i mag, short ang, short angY) {
    Vector3 v;
    unsigned idx = (unsigned short)ang >> 4;

    v.x = 0;
    v.y = FX_MUL(mag, data_02082214[idx * 2]);
    v.z = FX_MUL(mag, data_02082214[idx * 2 + 1]);
    Vec3_RotateYAndTranslate(res, trans, angY, &v);
}

// @symbol func_02007b98
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

// @symbol func_02007b0c
/* Script op: step lookAt toward a whole-unit offset turned to face the
   target player (yaw + 0x8000). */
int func_02007b0c(dCamera_c *cam, u8 *rec) {
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

// @symbol func_02007a8c
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

// @symbol func_020079ac
/* Script op: orbit lookAt around pos. On the first frame savedLookAt takes
   lookAt; every frame the two offsets accumulate and are added to the
   angles of the pos -> savedLookAt line before lookAt is rebuilt from it. */
int func_020079ac(dCamera_c *cam, u8 *p, int tag) {
    int dist;
    short vert, horz;
    if (tag == gKuppaScriptFrame) {
        cam->savedLookAt.x = cam->lookAt.x;
        cam->savedLookAt.y = cam->lookAt.y;
        cam->savedLookAt.z = cam->lookAt.z;
    }
    cam->mPitchOffset -= ReadUnalignedShort(p);
    cam->mYawOffset += ReadUnalignedShort(p + 2);
    func_02007c9c(&cam->pos, &cam->savedLookAt, &dist, &vert, &horz);
    vert = (short)(vert + cam->mPitchOffset);
    horz = (short)(horz + cam->mYawOffset);
    func_02007c14(&cam->lookAt, &cam->pos, dist, vert, horz);
    return 1;
}

// @symbol func_02007910
/* Script op: move pos by a distance and two angles relative to the target
   player. */
int func_02007910(dCamera_c *cam, u8 *p) {
    Vector3 *v = &cam->mTargetPlayer->mPos;
    int dist;
    short vert;
    short horz;

    func_02007c9c(v, &cam->pos, &dist, &vert, &horz);
    dist = ReadUnalignedSignedInt(p) + dist;
    vert = (short)(ReadUnalignedShort(p + 4) + vert);
    horz = (short)(ReadUnalignedShort(p + 6) + horz);
    func_02007c14(&cam->pos, v, dist, vert, horz);
    return 1;
}

// @symbol func_020078c4
/* Script op: turn mFovAngle toward a target. */
int func_020078c4(dCamera_c *cam, u8 *data) {
    short v = ReadUnalignedShort(data);
    unsigned char b = data[2];
    ApproachAngle(&cam->mFovAngle, v, b, 0x4000, 0);
    return 1;
}

// @symbol func_02007870
/* Script op: the field-of-view shake (func_0200d8ac). */
int func_02007870(dCamera_c *cam, u8 *data) {
    int v1 = (int)ReadUnalignedShort(data);
    int v2 = (int)ReadUnalignedShort(data + 2);
    int v3 = (int)ReadUnalignedShort(data + 4);
    func_0200d8ac((char *)cam, v1, v2, v3);
    return 1;
}

// @symbol func_020077f0
/* Script op: the per-level orbit yaws (func_020071a8). */
int func_020077f0(dCamera_c *cam) {
    dCamera_c *r4 = cam;
    int lvl = SublevelToLevel(data_0209f2f8);

    if (lvl == 2 && data_0209f224 == 5) {
        func_020071a8(r4, 0, 0x4000);
    } else if (lvl == 8 && data_0209f224 == 1) {
        func_020071a8(r4, (short)0x8000, 0x5000);
    }

    return 1;
}

// @symbol func_02007758
/* Script op: orbit pos toward mOrbitTargetYaw, slower in two areas. */
int func_02007758(dCamera_c *cam) {
    short amt = 0x1800;
    int level = SublevelToLevel(data_0209f2f8);
    if ((level == 9 && data_0209f224 == 6) || (level == 0xd && data_0209f224 == 4))
        amt = 0x800;
    func_020070e8(cam, 0x320000, 0xcc, amt, 0x10, cam->mOrbitTargetYaw, 8);
    return 1;
}

// @symbol func_02007698
/* Script op: place lookAt and pos at whole-unit offsets turned to face the
   target player. */
int func_02007698(dCamera_c *cam, u8 *rec) {
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

// @symbol func_0200762c
/* Script op: savedLookAt at data_02086ec0 in front of the target player, at
   height 0x1c9000. */
int func_0200762c(dCamera_c *cam) {
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

// @symbol func_020075b4
/* Script op: orbit pos (func_020070e8) from the target player toward
   savedLookAt's distance and angles, swung 0x1200 to the side. */
int func_020075b4(dCamera_c *cam) {
    int dist;
    short vertAng;
    short horzAng;

    func_02007c9c(&cam->mTargetPlayer->mPos, &cam->savedLookAt, &dist, &vertAng, &horzAng);
    func_020070e8(cam, dist, 0x199, vertAng, 0x20, (short)(horzAng + 0x1200), 0x14);
    return 1;
}

// @symbol func_02007548
/* Script op: step lookAt toward a point above the target player. */
int func_02007548(dCamera_c *cam) {
    Vector3 *base = &cam->mTargetPlayer->mPos;
    int y0 = base->y + 0xffecd000;
    int tz = base->z;
    int tx = base->x;
    Vector3 src;
    src.y = 0x197000 + (y0 >> 1);
    src.x = tx;
    src.z = tz;
    func_02007d0c(&cam->lookAt, &src, 0x800, 0xccc, 0x800);
    return 1;
}

// @symbol func_0200751c
/* Script op: turn to the yaw behind the target player and clear
   savedLookAt. */
int func_0200751c(dCamera_c *cam) {
    int tmp = cam->mTargetPlayer->mAngleY;
    tmp += 0x8000;
    cam->mTargetAngleY = (short)tmp;
    cam->savedLookAt.x = 0;
    cam->savedLookAt.y = 0;
    cam->savedLookAt.z = 0;
    return 1;
}

// @symbol func_020074e8
int func_020074e8(dCamera_c *cam) {
    ApproachLinear(cam->savedLookAt.z, -0x28000, 0x2000);
    cam->savedLookAt.x = -0x5000;
    return 1;
}

// @symbol func_02007484
/* Script op: ease savedLookAt.z to 0 and pos's x and z toward the target
   player. */
int func_02007484(dCamera_c *cam) {
    ApproachLinear(cam->savedLookAt.z, 0, 0x2000);

    cam->savedLookAt.x = 0;

    Vector3 *target = &cam->mTargetPlayer->mPos;

    Math_Function_0203b0fc(&cam->pos.x, target->x, 0x28, 0x7fffffff);
    Math_Function_0203b0fc(&cam->pos.z, target->z, 0x28, 0x7fffffff);

    return 1;
}

// @symbol func_02007450
/* Script op: pos = savedLookAt turned by mTargetAngleY, added to pos. */
int func_02007450(dCamera_c *cam) {
    short angle = cam->mTargetAngleY;
    Vec3_RotateYAndTranslate(&cam->pos, &cam->pos, angle, &cam->savedLookAt);
    return 1;
}

// @symbol func_02007414
/* Script op: clears savedLookAt, sets savedPos to (0, lookAt's height above
   the target player, 0), and clears mScriptParam and mRampSpeed. */
int func_02007414(dCamera_c *cam) {
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

// @symbol func_020073a0
/* Script op: step savedLookAt toward data_02086ed8 and ease savedPos's y to
   0 and its z to -0xc8000. Its member-function constant is data_02086cb0. */
int func_020073a0(dCamera_c *cam) {
    int stackbuf[3];
    stackbuf[0] = data_02086ed8[0];
    stackbuf[1] = data_02086ed8[1];
    stackbuf[2] = data_02086ed8[2];

    func_02007cec(&cam->savedLookAt, (Vector3 *)stackbuf, 0xcc);
    ApproachLinear(cam->savedPos.y, 0, 0x2000);
    ApproachLinear(cam->savedPos.z, -0xc8000, 0x6aaa);
    return 1;
}
}
