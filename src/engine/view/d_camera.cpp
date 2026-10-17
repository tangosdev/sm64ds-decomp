//cpp
/* dCamera_c, the main camera. arm9 .text 0x0200bec4..0x0200e494.
 *
 * _ZTS9dCamera_c is at 0x02086e6c, _ZTI9dCamera_c at 0x02086ee4,
 * _ZTV9dCamera_c at 0x02086f84. ChangeState's mangling names the
 * nested State. The D1/D0 pair is at 0x0200705c, outside this run.
 *
 * leftovers:
 * - The func_0200* handlers retain their existing C linkage. The three
 *   line-query handlers use C++ for native collision-result initialization;
 *   other handlers remain in cplusplus-off regions.
 * - Vec3 remains the POD scratch triple in the older handlers. Native
 *   collision results use Vector3 and its empty destructor.
 * - The state records in the 0x0209b008 table stay int. These bodies
 *   only take their address.
 * - CleanupResources keeps both null checks. The second is the
 *   inlined deleting destructor.
 * - The handlers below 0x0200bec4 stay out of this file.
 *   func_0200bb28 needs opt_common_subs off, and that pragma is
 *   TU-global, while Behavior needs it on.
 */
#include "dCamera_c.h"
#include "dBgCh_Lin.h"
#include "Memory.h"
#include "decl_common.h"
#include "decl_Particle.h"
#include "dClipper.h"
#include "nitro/fx/fx.h"

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
extern s32 data_0209fc48;
extern Matrix4x3 data_020a0e68;
extern void *data_ov002_0210c3b0;
extern void *data_ov002_0210c3e0[2];

extern "C" {
/* local extern: func_020092c4's callee owns a camera; the pointer is opaque to
   every caller here. */
extern int func_020092c4(void *self, Vec3 *out, Vec3 *target);
extern void func_0200928c(void *cam);
extern void func_0200ee8c(int arg0);
extern u32 func_02012790(u32 a);
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

extern int _ZN4cstd4fdivEii(int a, int b);
extern void _Z14ApproachLinearRiii(int *r, int b, int c);
extern int _Z15ApproachLinear2Rsss(short *dst, short target, short step);
/* local extern: dClipper::Func_020156DC takes a by-value Fix12<int> (the Fix12
   wall), so it stays spelled out with scalar arguments. */
extern void _ZN8dClipper13Func_020156DCEitii(void *self, int a, int b, int c, int d);
extern void *_ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN9dBgCh_GndD1Ev(void *self);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, const void *pos, void *actor);
/* The C bodies spell the member by its mangled name (C cannot write `this->ChangeState`). */
/* local extern: dCamera_c.h declares the same symbol as a member. */
extern int _ZN9dCamera_c11ChangeStateEPNS_5StateE(void *thiz, void *state);

extern void *_ZN7fBase_cC2Ev(void *self);
extern void *_Znwj(u32 sz);
/* _ZTV7dBase_c/_ZTV7dView_c/_ZTV9dCamera_c come from decl_common.h (int[]/u32
   spellings); the stores below take their addresses explicitly. */

/* Siblings inside this TU whose file-local decl spellings decl_common.h does
   not already pin. */
extern void func_0200af20(char *c, Vec3 *v1, Vec3 *v2, short *out);
extern int func_0200c394(void *self, int a1, void *a2, void *a3, void **a5, int *a6);
extern void func_0200cbe0(void *self);
extern void func_0200cce4(char *cam);
extern void func_0200cf40(char *c);
extern int func_0200d0ac(int a, unsigned int b);
extern void func_0200d954(char *c, short arg);
}

/* Opaque camera-side records used by the older handlers. The ground-query
   shadow retains its measured frame and flat reads (ground._[0x11] is
   clsnY at 0x44). Line queries use the real dBgCh_Lin class above. The
   remaining records are typedef'd for the cplusplus-off regions. */
typedef struct dBgCh_Gnd { int _[0x15]; } dBgCh_Gnd;
typedef struct dBgCh_LinHit { char pad[0x14]; char surf[0x64]; } dBgCh_LinHit;   /* func_0200b990's variant */
typedef struct Entry3 { int a, b, c; } Entry3;                      /* data_02086f58 rows */
extern Entry3 data_02086f58[];
typedef struct CamViewObj { u8 type; u8 p1; s16 x, y, z; } CamViewObj;  /* GetViewObj record */
typedef struct CamEntryE { s16 f0, f2, f4, f6, f8, fa; } CamEntryE;    /* func_0200cf40's spawn view */
typedef struct CamStateBase { char _pad[0x13c]; int *field_13c; } CamStateBase;   /* func_0200af0c */
typedef struct CamStateIdx { char _pad[0x13c]; int field_13c; } CamStateIdx;      /* func_0200cb58 */
typedef struct CamJitter {                      /* func_0200d8c8's slice */
    char _pad0[0x8c];
    Vec3 pos;
    char _pad1[0x134 - 0x98];
    Fix12i unk134;
} CamJitter;
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
    static void SetBlendAlpha(volatile unsigned short *, u16, u16, u16, u32);
};
struct G3i {
    static void PerspectiveW_(int, int, int, int, int, int, bool, Matrix4x3 *);
    static void LookAt_(const Vector3 *, const Vector3 *, const Vector3 *, bool, Matrix4x3 *);
};
extern "C" {
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(bool, OamAttr *, int, int, int, int, int, int, int, int);
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int, int, int, int, int, int, bool, Matrix4x3 *);
}

/* @symbol dCamera_c_classInit
 * Allocation factory historically labelled _ZN9dCamera_cC1Ev. Its first action
 * overwrites r0 with fBase_c::operator new(sizeof(dCamera_c)); a real
 * constructor must consume the incoming storage pointer. The three vptr
 * transitions are dBase_c -> dView_c -> dCamera_c, and the Matrix4x3 at +0x50
 * is dView_c's view matrix. */
extern "C" dCamera_c *dCamera_c_classInit()
{
    dCamera_c *camera = (dCamera_c *)_ZN7fBase_cnwEj(sizeof(dCamera_c));
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
            thiz->mFixedViewPos = _Znwj(0xc);
            x = v->x; z = v->z; y = v->y;
            p = (s32 *)thiz->mFixedViewPos;
            p[0] = x << 0xc;
            p[1] = y << 0xc;
            p[2] = z << 0xc;
        }
    }
    thiz->mFovBlend = 0;
    thiz->mTargetPlayer = (s32)data_0209f394[data_0209f250];
    ChangeArea(*(signed char *)((char *)thiz->mTargetPlayer + 0xcc));
    *(int*)((char*)thiz+0x114) = 0;
    *(int*)((char*)thiz+0x118) = 0;
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

    *(s32 *)(c + 0xc8) = *(s32 *)(c + 0x80);
    *(s32 *)(c + 0xcc) = *(s32 *)(c + 0x84);
    *(s32 *)(c + 0xd0) = *(s32 *)(c + 0x88);
    *(s32 *)(c + 0xd4) = *(s32 *)(c + 0x8c);
    *(s32 *)(c + 0xd8) = *(s32 *)(c + 0x90);
    *(s32 *)(c + 0xdc) = *(s32 *)(c + 0x94);
    *(u32 *)((int)c + 0x154) &= ~0x1000u;
    if (*(u32 *)(c + 0x140) != (u32)&data_0208733c) {
        *(u32 *)((int)c + 0x154) &= ~0x60u;
    }
    temp_r1 = *(s32 *)(c + 0x154);
    if (!(temp_r1 & 8)) {
        enum Bool b = (enum Bool)(data_0209fc48 != 0);
        if (b != FALSE) goto block_7;
    } else {
block_7:
        if (temp_r1 & 0xc000) {
            *(u32 *)(c + 0x154) &= ~0xc000u;
            ChangeState((State *)&data_0209b008);
        }
        *(s16 *)(c + 0x17c) = Vec3_HorzAngle(c + 0x80, c + 0x8c);
        *(s16 *)(c + 0x17e) = Vec3_VertAngle(c + 0x80, c + 0x8c);
        func_020089f8(c);
    }

    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 && !(data_0209b454 & 0x40000000)) {
        enum Bool b2 = (enum Bool)(data_0209fc48 != 0);
        if (b2 == FALSE) {
            if (*(s32 *)(c + 0x154) & 0x2000) {
                Math_Function_0203b0fc((int *)(c + 0x168), 0x1000, 0x88, 0x7fffffff);
            } else {
                Math_Function_0203b0fc((int *)(c + 0x168), 0, 0x88, 0x7fffffff);
            }
            func_0200ca50(c);
            if (*(s32 *)(c + 0x134) != 0) {
                _Z14ApproachLinearRiii((int *)(c + 0x134), 0, 0x1000);
                *(s32 *)(c + 0x134) = 0 - *(s32 *)(c + 0x134);
            }
            if (*(s16 *)(c + 0x18c) != 0) {
                if (_Z15ApproachLinear2Rsss((short *)(c + 0x18c), 0, 0x10) != 0) {
                    *(s16 *)(c + 0x18a) = 0;
                } else {
                    *(s16 *)((long long)(int)(c + 0x18a)) =
                        (s16)(*(s16 *)((long long)(int)(c + 0x18a)) + 0x3000);
                }
            }
        }
        if (*(s16 *)(c + 0x18e) != 0) {
            *(s16 *)((long long)(int)(c + 0x192)) =
                (s16)(*(s16 *)((long long)(int)(c + 0x192)) + *(s16 *)(c + 0x194));
            _Z15ApproachLinear2Rsss((short *)(c + 0x18e), 0, *(s16 *)(c + 0x190));
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
            *(u32 *)(c + 0x15c) = _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj(
                *(u32 *)(c + 0x15c), (u32)r5, spVec[0], spVec[1], spVec[2], 0, flag);
            if (r5 == 0x86) {
                *(u32 *)(c + 0x160) = _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj(
                    *(u32 *)(c + 0x160), 0x87, spVec[0], spVec[1], spVec[2], 0, 1);
            }
        }
    }

    {
        char *m = *(char **)(c + 0x110);
        enum Bool isBf = (enum Bool)(*(u16 *)(m + 0xc) == 0xbf);
        if (isBf != FALSE) {
            s32 t2 = data_0209f32c - (data_02082214[(*(u16 *)(c + 0x17e) >> 4) * 2] * 0x10);
            if (!(*(s32 *)(c + 0x154) & 1)) {
                if (*(u8 *)(m + 0x706) != 0 || *(u8 *)(m + 0x707) != 0) {
                    if (*(s32 *)(c + 0x90) <= t2) {
                        *(u32 *)(c + 0x154) |= 1u;
                    }
                }
            } else if (*(s32 *)(c + 0x90) > t2) {
                *(u32 *)(c + 0x154) &= ~1u;
            }
        }
    }
    {
        u32 *pf = (u32 *)(c + 0x154);
        *pf &= 0xfff8fdfbu;
        if (*(s32 *)(c + 0x118) != 0) {
            *(s32 *)(c + 0x118) = 0;
            *pf |= 4u;
        }
    }
    *(s32 *)(c + 0x114) = 0;
    func_0203dafc((short)(*(s16 *)(c + 0x17c) + *(s16 *)(c + 0x188)));
    _Z15ApproachLinear2Rsss((short *)(c + 0x188), 0, 0x400);
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

    if (!(data_0209fc48 != 0 ? 1 : 0)) {
        s32 f = self->mFlags;
        if (!(f & 8)) {
            if ((u8 *)self->mState_13c == &data_0208738c) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr *)&data_ov002_0210c3b0, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                G2x::SetBlendAlpha((volatile unsigned short *)0x04000050, 0x10, 0x2f, 0xc, 6);
            } else {
                if (f & 0x20) {
                    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr *)data_ov002_0210c3e0[0], 0x14, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                }
                if (self->mFlags & 0x40) {
                    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr *)data_ov002_0210c3e0[1], 0xec, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                }
            }
            s32 t = self->mFovBlend;
            if (t != 0x1000) {
                s32 d = 0xaaa - sp18;
                sp18 = (s16)(sp18 + (s16)FX_MUL(d, t));
            }
            if (data_0209f20c == 0 && data_0209f294 == 0 &&
                (u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) != 0) {
                func_0200af20((char *)self, (Vec3 *)&sp[0], (Vec3 *)&sp[3], &sp18);
            }
        }
    }

    sp[1] += self->mShakeY;
    sp[4] += self->mShakeY;

    s16 tr3 = self->mRollShake;
    if (tr3 != 0) {
        s16 c = data_02082214[((s32)*(u16 *)((char *)self + 0x18a) >> 4) * 2];
        var_r4 = (s16)(var_r4 + (s16)FX_MUL(tr3, c));
    }

    s16 tr0 = self->mFovShake;
    if (tr0 != 0) {
        s16 c = data_02082214[((s32)*(u16 *)((char *)self + 0x192) >> 4) * 2 + 1];
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
    _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
        data_02082214[idx], data_02082214[idx + 1], self->mAspect, self->mNear, self->mFar, var_r6,
        1, (Matrix4x3 *)0);

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

#pragma cplusplus off
// @symbol func_0200d954
void func_0200d954(char *c, short arg) {
    int i = (int)(unsigned short)arg >> 4;
    int j = i * 2;
    *(int *)(c + 0x104) = func_02053200(data_02082214[j]);
    *(int *)(c + 0x108) = func_02053200(data_02082214[j + 1]);
    _ZN8dClipper13Func_020156DCEitii(&data_0209f43c, *(int *)(c + 0xf8), arg, *(int *)(c + 0xfc),
                                     *(int *)(c + 0x100));
}

// @symbol func_0200d8c8
/* Ground-pound jitter: if the target point is within range, accumulate a
 * jitter offset (unk134) scaled by 0x9000/distance, clamped to 0xc000. */
void func_0200d8c8(struct dCamera_c *cam, const struct Vector3 *v, int strength) {
    CamJitter *j;
    Fix12i dist;
    Fix12i offset;

    dist = Vec3_Dist(&cam->pos, v);
    if (strength <= dist)
        return;

    offset = FX_MUL(_ZN4cstd4fdivEii(strength, dist), 0x9000);

    j = (CamJitter *)cam;
    if (offset <= j->unk134)
        return;

    if (offset > 0xc000)
        offset = 0xc000;

    j->unk134 = offset;
}

// @symbol func_0200d8ac
void func_0200d8ac(char *c, int r1, int r2, int r3)
{
    short ip;

    c = c + 0x100;
    ip = *(short *)(c + 0x8e);
    if (r1 > ip) {
        *(short *)(c + 0x8e) = (short)r1;
        *(short *)(c + 0x90) = (short)r2;
        *(short *)(c + 0x94) = (short)r3;
    }
}

// @symbol func_0200d89c
void func_0200d89c(char *p)
{
    *(short *)(p + 0x18c) = 384;
}
#pragma cplusplus on

// @symbol _ZNK9dCamera_c12IsUnderwaterEv
/* Returns the under-water bit of mFlags MASKED, not normalised to 0/1 -- the
 * ROM's `and r0,r0,#1` is the whole body. */
int dCamera_c::IsUnderwater() const
{
    return mFlags & 1;
}

#pragma cplusplus off
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
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, &data_0209b008);
}

// @symbol func_0200d7e0
void func_0200d7e0(void *thiz, int playerID)
{
    if (playerID != (int)data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, &data_0209b098);
}

// @symbol func_0200d7a4
void func_0200d7a4(void *thiz, u8 playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b108);
}

// @symbol func_0200d768
void func_0200d768(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b018);
}

// @symbol func_0200d72c
void func_0200d72c(void *thiz, unsigned char playerID)
{
    if (playerID == *(unsigned char *)&data_0209f250)
        _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz,
                                            (void *)&data_0209b028);
}

// @symbol func_0200d6f0
void func_0200d6f0(void *thiz, unsigned char playerID)
{
    if (playerID == *(unsigned char *)&data_0209f250)
    {
        _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b038);
    }
}

// @symbol func_0200d6b4
void func_0200d6b4(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b048);
}

// @symbol func_0200d678
void func_0200d678(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b058);
}

// @symbol func_0200d63c
void func_0200d63c(void *thiz, unsigned char playerID)
{
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b068);
}

// @symbol func_0200d5fc
void func_0200d5fc(void *thiz, int playerID)
{
    if (playerID == data_0209f250) {
        _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, &data_0209b078);
        func_0200cb58(thiz, 5);
    }
}

// @symbol func_0200d5c0
void func_0200d5c0(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b078);
}

// @symbol func_0200d580
void func_0200d580(void *thiz, int playerID)
{
    if (playerID != data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, &data_0209b078);
    func_0200cb58(thiz, 8);
}

// @symbol func_0200d544
void func_0200d544(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b088);
}

// @symbol func_0200d508
void func_0200d508(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b0a8);
}

// @symbol func_0200d4b0
void func_0200d4b0(char* self, unsigned int playerID, int arg2)
{
    unsigned char tmp = data_0209f250;
    if (playerID == tmp)
    {
        volatile int dummy;
        char* other;
        *(short*)(self + 0x19a) = *(short*)(self + 0x17c);
        other = *(char**)(self + 0x110);
        *(int*)(((int)other + 0xb0)) |= 0x20000000;
        func_0200ee8c(arg2);
    }
}

// @symbol func_0200d474
void func_0200d474(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b0b8);
}

// @symbol func_0200d438
void func_0200d438(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b0c8);
}

// @symbol func_0200d3f8
void func_0200d3f8(void *thiz, unsigned char playerID, void *ptr)
{
    unsigned char tmp = *(unsigned char *)&data_0209f250;
    if (playerID != tmp)
        return;
    ((struct dCamera_c *)thiz)->mLookTarget = (s32 *)ptr;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b0d8);
}
#pragma cplusplus on

// @symbol _ZN9dCamera_c14GoBehindPlayerEj
void dCamera_c::GoBehindPlayer(unsigned int j)
{
    int slot4, slot8, slotc;

    if (j != data_0209f250)
        return;
    if (data_02092110 >= 0)
        return;

    /* different launder spellings to defeat CSE across the call */
    *(unsigned int *)(((int)&mFlags)) &= 0xfffffaf7u;
    func_0200cb58((void *)((int)this), 0xa);
    *(unsigned int *)(((long long)(int)((int)&mFlags))) |= 4u;

    slot4 = mState_13c;
    slotc = 0;
    func_0200c66c((void *)((int)this), (void *)(mTargetPlayer + 0x5c), &slot8, &slot4, &slotc);
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
    char *info = *(char **)(self + 0x110);
    char *base = (char *)(info + 0x5c);
    int v1;
    unsigned int r0;
    s32 temp1[3];
    short angle;
    int v2;
    unsigned int dResult;
    int v3;
    unsigned int r0b;
    s32 temp2[3];

    *(s32 *)(self + 0x98) = *(s32 *)(base + 0);
    *(s32 *)(self + 0x9c) = *(s32 *)(base + 4);
    *(s32 *)(self + 0xa0) = *(s32 *)(base + 8);

    v1 = *(s32 *)(*(char **)(self + 0x13c) + 0x14);
    r0 = func_020093f4(self, v1);

    temp1[0] = data_02086f2c.x;
    temp1[1] = data_02086f2c.y;
    temp1[2] = r0;
    data_02086f2c.z = r0;

    angle = *(short *)(*(char **)(self + 0x110) + 0x8e);
    Vec3_RotateYAndTranslate(self + 0x80, self + 0x98, angle, temp1);

    v2 = *(s32 *)(*(char **)(self + 0x13c) + 0x10);
    dResult = func_020093d4(self, v2);
    *(s32 *)(self + 0x84) += dResult;

    v3 = *(s32 *)(*(char **)(self + 0x13c) + 0x20);
    r0b = func_020093f4(self, v3);

    data_02086f14[2] = r0b;
    temp2[0] = data_02086f14[0];
    temp2[1] = data_02086f14[1];
    temp2[2] = r0b;

    angle = *(short *)(*(char **)(self + 0x110) + 0x8e);
    Vec3_RotateYAndTranslate(self + 0x8c, self + 0x80, angle, temp2);

    *(s32 *)(self + 0x90) += (int)(dResult - 0x8b000);

    *(s32 *)(self + 0xa4) = 0;
    *(s32 *)(self + 0xa8) = 0xc3f9d;
    *(s32 *)(self + 0xac) = 0;

    ChangeArea(*(signed char *)(*(char **)(self + 0x110) + 0xcc));
}
#pragma cplusplus on

// @symbol _ZN9dCamera_c10LookAtExitER8dActor_c
void dCamera_c::LookAtExit(dActor_c &actor)
{
    int *src;
    char *a = (char *)&actor;

    mLookTarget = (int *)(a + 0x5c);
    src = mLookTarget;
    mLookTargetPos.x = src[0];
    mLookTargetPos.y = src[1];
    mLookTargetPos.z = src[2];
    *(s16 *)((char *)this + 0x186) = -*(s16 *)(a + 0x8e);
    ChangeState((State *)&data_0209b0f8);
    mFlags |= 0x10;
}

#pragma cplusplus off
// @symbol func_0200d148
void func_0200d148(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b118);
}

// @symbol func_0200d10c
void func_0200d10c(void *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b128);
}

// @symbol func_0200d0ac
int func_0200d0ac(int a, unsigned int b)
{
  if (b == data_0209f250)
  {
    unsigned short v = *((unsigned short *) (((unsigned char *) (*((unsigned int **) (a + 0x140)))) + 0x26));
    if (v & 1)
    {
      return 0;
    }
    *((unsigned int *) (a + 0x154)) |= 2u;
    func_02012790(7);
  }
  return 1;
}

// @symbol func_0200d064
void func_0200d064(void *param_1, unsigned char param_2)
{
  unsigned char tmp = data_0209f250;
  if (param_2 == tmp)
  {
    volatile int dummy;
    unsigned int *addr = (unsigned int *) ((char *)param_1 + 0x154);
    unsigned int val = *addr;
    val &= ~2u;
    *addr = val;
    func_02012790(6);
  }
}
#pragma cplusplus on

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
void func_0200cf40(char *c)
{
  int new_var;
  *((u8 *) (c + 0x10c)) = 0;
  if (c)
  {
  }
  *((u8 *) (c + 0x10d)) = 0;
  *((u8 *) (c + 0x10e)) = 0xff;
  *((u8 *) (c + 0x10f)) = 0xbf;
  *((int *) (c + 0xf8)) = 0x1555;
  *((int *) (c + 0xfc)) = 0x1000;
  *((int *) (c + 0x100)) = 0x1388000;
  *((int *) (c + 0x140)) = (int) (&data_0208715c);
  *((int *) (c + 0x144)) = 0;
  if ((*((int *) (c + 8))) != 0xf)
  {
    CamEntryE *v;
    int x;
    int y;
    int z;
    func_0200cb58(c, 0xa);
    *((s16 *) (c + 0x17a)) = *((s16 *) ((*((char **) (c + 0x13c))) + 0x24));
    func_0200d954(c, *((s16 *) (c + 0x17a)));
    func_0200cce4(c);
    v = (CamEntryE *)GetViewObj((*((int *) (c + 8))) & 0xff);
    x = v->f2;
    z = v->f6;
    new_var = z << 12;
    y = v->f4;
    *((int *) (c + 0x8c)) = x << 12;
    *((int *) (c + 0x90)) = y << 12;
    *((int *) (c + 0x94)) = new_var;
    *((s16 *) (c + 0x182)) = v->fa + 0x8000;
    return;
  }
  func_0200cb58(c, 9);
  *((s16 *) (c + 0x17a)) = *((s16 *) ((*((char **) (c + 0x13c))) + 0x24));
  func_0200d954(c, *((s16 *) (c + 0x17a)));
  func_0200cce4(c);
}

// @symbol func_0200cce4
#pragma cplusplus on
extern "C" void func_0200cce4(char* cam)
{
    Vec3 vec;
    Vec3 tmp1;
    Vec3 tmp2;
    u32 d;

    if (*(char**)(cam + 0x110) != 0) {
        Vec3* p = (Vec3*)(((int)*(char**)(cam + 0x110) + 0x5c));
        *(int*)(cam + 0x98) = p->x;
        *(int*)(cam + 0x9c) = p->y;
        *(int*)(cam + 0xa0) = p->z;

        {
            u32 d1 = func_020093f4(cam, *(int*)(*(char**)(cam + 0x13c) + 0x14));
            tmp1.z = d1;
            tmp1.x = data_02086f2c.x;
            tmp1.y = data_02086f2c.y;
            data_02086f2c.z = d1;
        }
        Vec3_RotateYAndTranslate((Vec3*)(cam + 0x80), (Vec3*)(cam + 0x98),
            *(s16*)(*(char**)(cam + 0x110) + 0x8e), &tmp1);

        d = func_020093d4(cam, *(int*)(*(char**)(cam + 0x13c) + 0x10));
        *(int*)(((int)cam + 0x84)) += d;

        {
            u32 d2 = func_020093f4(cam, -*(int*)(*(char**)(cam + 0x13c) + 0x20));
            tmp2.z = d2;
            data_02086f20.z = d2;
            tmp2.x = data_02086f20.x;
            tmp2.y = data_02086f20.y;
        }
        Vec3_RotateYAndTranslate((Vec3*)(cam + 0x8c), (Vec3*)(cam + 0x80),
            *(s16*)(*(char**)(cam + 0x110) + 0x8e), &tmp2);

        dBgCh_Lin line;
        func_0200897c(cam, &line);
        {
            int tx;
            int ty;
            int tz;
            tx = *(int*)(cam + 0x8c);
            tz = *(int*)(cam + 0x94);
            ty = *(int*)(cam + 0x90) + 0x100000;
            vec.x = tx;
            vec.y = ty;
            vec.z = tz;
        }
        line.SetObjAndLine(((dCamera_c *)cam)->pos, *(Vector3 *)&vec, 0);
        if (line.DetectClsn() != 0) {
            Vector3 cp = line.GetClsnPos();
            vec.x = cp.x;
            vec.y = cp.y;
            vec.z = cp.z;
        }
        dBgCh_Gnd ground;
        _ZN9dBgCh_GndC1Ev(&ground);
        func_0200897c(cam, &ground);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&ground, &vec, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&ground) != 0) {
            *(int*)(cam + 0x90) = d + ground._[0x11];
        }
        *(int*)(((int)cam + 0x90)) += func_020093d4(cam, 0x3c286);
        {
            int diff;
            int base;
            int a;
            base = *(int*)(cam + 0x84);
            diff = *(int*)(cam + 0x90) - base;
            a = (diff < 0) ? -diff : diff;
            if (a > 0x3e8000) {
                if (diff < -0x3e8000) diff = -0x3e8000;
                else if (diff > 0x3e8000) diff = 0x3e8000;
                *(int*)(cam + 0x90) = base + diff;
            }
        }
        {
            u32 t = func_020093d4(cam, *(int*)(*(char**)(cam + 0x13c) + 0x10));
            *(int*)(cam + 0xa4) = 0;
            *(int*)(cam + 0xa8) = t;
            *(int*)(cam + 0xac) = 0;
        }
        func_0200928c(cam);
        ChangeArea(*(s8*)(*(char**)(cam + 0x110) + 0xcc));
        _ZN9dBgCh_GndD1Ev(&ground);

    }
    *(int*)(((int)cam + 0x154)) |= 4;
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
    int r1 = func_020092c4(cam, (Vec3 *)&cam->lookAt, (Vec3 *)&cam->savedLookAt);
    int r2 = func_020092c4(cam, (Vec3 *)&cam->pos, (Vec3 *)&cam->savedPos);
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
        unk_1a6 = 0;
    }
    return func_0200cae4(this);
}

#pragma cplusplus off
// @symbol func_0200cb58
void func_0200cb58(void *obj_, int index) {
    struct CamStateIdx *obj = (struct CamStateIdx *)obj_;
    obj->field_13c = index * 0x28 + (int)&data_02086fcc;
}
#pragma cplusplus on

// @symbol func_0200cae4
/* State dispatch: honors the deferred-change flags, then calls the state's
 * +0 member-function pointer on the camera. */
extern "C" int func_0200cae4(void *c_)
{
  CamPmfHost *c = (CamPmfHost *)c_;
  if(c->flags & 0x4000){
    volatile unsigned int* flags = (volatile unsigned int*)(((int)c + 0x154));
    *flags &= ~0x4000u;
    *flags |= 0x8000u;
  }
  CamPmf* p = c->pp;
  if(*(int*)p == 0) return 1;
  return (c->**p)();
}

// @symbol func_0200ca50
/* Per-frame state tick: the timer field at +0x17a eases toward the state
 * entry's target, then the state's +8 member-function pointer runs. */
extern "C" int func_0200ca50(void *self)
{
    CamPmfHost *ch = (CamPmfHost *)self;
    u8 *f = (u8 *)self;
    int flags;
    int r5;
    {
        void *p = *(void **)(f + 0x13c);
        _Z15ApproachLinear2Rsss((short *)(f + 0x17a), *(s16 *)((u8 *)p + 0x24), 0x80);
    }
    r5 = 1;
    flags = *(int *)(f + 0x154);
    if (flags & 8) {
        ;
    } else if (flags & 0x8000) {
        func_0200cbe0(self);
    } else {
        u8 *obj = *(u8 **)(f + 0x138);
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
        *(unsigned *)((char *)r0 + 0x154) |= 0x20000;
    else
        *(unsigned *)((char *)r0 + 0x154) |= 0x10000;
}

// @symbol func_0200c9e0
void func_0200c9e0(void *c, s32 *r1, s32 *r2)
{
    unsigned int v = *(unsigned int *)((char *)c + 0x154);
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
    Vec3 *pos = (Vec3 *)pos_;
    void **out2 = (void **)out2_;
    void **out1 = (void **)out1_;
    Vec3 tmp;
    int hval;
    int col1;
    int col2;
    char* p;
    void* obj;
    int res;

    p = *(char**)(self + 0x110);
    obj = 0;
    {
        int b = *(u16*)(p + 0xc);
        b = b == 0xbf;
        if (b != 0) {
            res = *(int*)(p + 0x644);
            if ((u32)res == 0x80000000) res = *(int*)(p + 0x60);
            obj = p;
        }
    }

    if (obj != 0 && data_0209f2f8 != 0x1c) {
        col1 = *(int*)((char*)obj + 0x65c) & 0xff;
        col2 = *(int*)((char*)obj + 0x660) & 0xff;
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
        _ZN9dBgCh_GndC1Ev(&ground);
        func_0200897c(self, &ground);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&ground, &tmp, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&ground) == 0) {
            hval = pos->y;
            col1 = 0;
            col2 = 0x3f;
        } else {
            hval = ground._[0x11];
            col1 = func_02037e48((u32*)&ground._[5]) & 0xff;
            col2 = func_02037e68((u32*)&ground._[5]) & 0xff;
        }
        _ZN9dBgCh_GndD1Ev(&ground);

    }

    if (col1 == 0 || (*(u32*)(self + 0x154) & 0x400) != 0) {
        if (*(u16*)(*(char**)(self + 0x13c) + 0x26) & 0x10) {
            if (*(void**)(self + 0x140)) *out1 = *(void**)(self + 0x140);
            if (*(void**)(self + 0x144)) *out2 = *(void**)(self + 0x144);
        }
    } else {
        int b0;
        if (col2 != 0x3f) *out2 = GetViewObj(col2);
        if (func_0200c394(self, col1, *out2, pos, out1, arg5_) == 0) {
            if ((u32)*(char**)(self + 0x13c) <= (u32)&data_02086ff4 || *out1 == (void*)&data_02087274) {
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
                        if (*(void**)(self + 0x140) != *out1)
                            *(u32*)(((int)self + 0x154)) |= 4;
                    }
                    res = hval;
                } else if (b0 == 6) {
                    *out1 = (void*)&data_0208742c;
                } else {
                    if ((*(u16*)(*(char**)(self + 0x13c) + 0x26) & 4) == 0) {
                        if (b0 == 0) {
                            if (*out1 == (void*)&data_02087274 && data_0209f2f8 == 0x2d) {
                                *(int*)(self + 0x120) = *(int*)(self + 0x98);
                                *(int*)(self + 0x124) = *(int*)(self + 0x9c);
                                *(int*)(self + 0x128) = *(int*)(self + 0xa0);
                            }
                            *out1 = (void*)&data_020871ac;
                        } else if (b0 == 1) {
                            *out1 = (void*)&data_020871d4;
                        } else if (b0 == 5) {
                            *out1 = (void*)&data_02087404;
                        }
                    }
                }
                *(u32*)(((int)self + 0x154)) &= ~0x100;
            }
        }
        if (*out1 != (void*)&data_02087094) *(void**)(self + 0x140) = *out1;
        if (*out1 != (void*)&data_0208733c) {
            if (*out2 != 0 && *(unsigned char*)(*out2) != 2)
                *(void**)(self + 0x144) = *out2;
        }
    }

    return res;
}
#pragma cplusplus off

// @symbol func_0200c394
int func_0200c394(void *self_, int a1, void *a2_, void *a3, void **a5, int *a6)
{
    char *self = (char *)self_;
    unsigned char *a2 = (unsigned char *)a2_;
    void *p = *(void **)(self + 0x13c);
    Vec3 local, local2;
    signed char g;
    int flags = *(unsigned short *)((char *)p + 0x26);
    int v;
    short sz, sy, sx;

    if ((flags & 0x20) == 0) {
        v = *(int *)(self + 0x154) & 0x100;
        if (v == 0) goto body;
    }
    return 0;
body:
    {
            if (a1 == 7) {
                void *o = *(void **)(self + 0x114);
                if (o == 0) {
                    if (a2 == 0) goto other;
                    if (a2[0] != 3) goto other;
                }
                *a6 = 1;
                o = *(void **)(self + 0x114);
                if (o) {
                    int qb = (int)o + 0x5c;
                    *(int *)(self + 0x120) = *(int *)(qb);
                    *(int *)(self + 0x124) = *(int *)(qb + 4);
                    *(int *)(self + 0x128) = *(int *)(qb + 8);
                } else {
                    Vec3 t;
                    sx = *(short *)(a2 + 2);
                    sz = *(short *)(a2 + 6);
                    sy = *(short *)(a2 + 4);
                    t.x = sx << 12; t.z = sz << 12; t.y = sy << 12;
                    *(int *)(self + 0x120) = t.x;
                    *(int *)(self + 0x124) = t.y;
                    *(int *)(self + 0x128) = t.z;
                }
                Vec3_Sub(&local, (Vec3 *)(self + 0x120), a3);
                *(int *)(self + 0x12c) = LenVec3(&local) >> 1;
                g = data_0209f2f8;
                v = (g == 0x2d);
                if (v) {
                    if (*(int *)(self + 0x12c) < 0x100000) *(int *)(self + 0x12c) = 0x100000;
                    else if (*(int *)(self + 0x12c) > 0x180000) *(int *)(self + 0x12c) = 0x180000;
                } else {
                    if (*(int *)(self + 0x12c) < 0x180000) *(int *)(self + 0x12c) = 0x180000;
                }
                if (*(int *)(self + 0x154) & 2) {
                    *(int *)(self + 0x12c) = FX_MUL(*(int *)(self + 0x12c), 0x1800);
                }
                if (g == 0x2f && *(int *)(self + 0x12c) > 0xc0000) {
                    int s = _ZN4cstd4fdivEii(0xc0000, *(int *)(self + 0x12c) << 1);
                    Vec3_MulScalarInPlace(&local, s);
                    Vec3_Add(&local2, a3, &local);
                    *(int *)(self + 0x120) = local2.x;
                    *(int *)(self + 0x124) = local2.y;
                    *(int *)(self + 0x128) = local2.z;
                    *(int *)(self + 0x12c) = 0xc0000;
                } else {
                    AddVec3((Vec3 *)(self + 0x120), a3, (Vec3 *)(self + 0x120));
                    Vec3_AsrInPlace((Vec3 *)(self + 0x120), 1);
                }
                *a5 = &data_02087274;
                return v;
            }

        other:
            if (flags & 4) {
                return (p == &data_0208706c) ? 0 : 1;
            }
            if (p == &data_0208715c) {
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
#pragma cplusplus on
extern "C" int func_0200bec4(char *self, int *arg1, int arg2, char *arg3, int arg4)
{
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

    sp44.z = func_020093f4(self, *(int*)(arg3 + 0x14));
    data_02086f2c.z = sp44.z;
    sp44.x = data_02086f2c.x;
    sp44.y = data_02086f2c.y;
    Vec3_RotateYAndTranslate(&sp8, arg1, *(short*)(*(char**)(self + 0x110) + 0x8e), (int*)&sp44);
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
    char spEC[0x54];
    _ZN9dBgCh_GndC1Ev(spEC);
    func_0200897c(self, spEC);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(spEC, &sp2C, 0);
    r4 = sp8.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(spEC) != 0) {
        int d;
        r4 = *(int*)(spEC + 0x44);
        d = r4 - sp8.y;
        if (d > 0) {
            if (d > 0x30000)
                d = 0x30000;
            sp8.y += d;
        }
    }
    SubVec3(&sp8, arg1, &sp8);
    t = *(int*)(arg3 + 0x10);
    if (*(int*)(self + 0x154) & 0x20000) {
        t = FX_MUL(t, 0x3000);
    }
    sp8.y += func_020093d4(self, t);
    sl = Vec3_HorzLen(&sp8);
    Vec3 sp68;
    Vec3_MulScalar(&sp68, self + 0x120, *(int*)(self + 0x130));
    AddVec3(&sp8, &sp68, &sp8);
    if (*(int*)(self + 0x154) & 4) {
        *(int*)(self + 0xa4) = sp8.x;
        *(int*)(self + 0xa8) = sp8.y;
        *(int*)(self + 0xac) = sp8.z;
    } else {
        int hl = Vec3_HorzLen(self + 0xa4) - sl;
        if (*(unsigned short*)(arg3 + 0x26) & 0x40) {
            r5 = 0x10000;
        } else if (hl > 0) {
            r5 += FX_MUL(hl, 0x400);
        }
        Vec3_Sub(&sp38, &sp8, self + 0xa4);
        sl = Vec3_HorzLen(&sp38);
        {
            int f = sl;
            int fd;
            if (sl != 0) {
                Math_Function_0203b14c(&f, 0, 0x100, FX_MUL(r5, 0x600), 0x1000);
                fd = _ZN4cstd4fdivEii(f, sl);
                *(int *)(self + 0xa4) = sp8.x - FX_MUL(sp38.x, fd);
                *(int *)(self + 0xac) = sp8.z - FX_MUL(sp38.z, fd);
            }
        }
        {
            int a8 = *(int*)(self + 0xa8);
            int diff = sp8.y - a8;
            *(int *)(self + 0xa8) = sp8.y - FX_MUL(diff, 0xf80);
        }
        if (*(int*)(self + 0x154) & 0x400) {
            _ZN9dBgCh_GndD1Ev(spEC);

            return r4;
        }
    }
    *(int*)(self + 0x80) = arg1[0] + *(int*)(self + 0xa4);
    *(int*)(self + 0x88) = arg1[2] + *(int*)(self + 0xac);
    {
        int sl2 = arg1[1] + *(int*)(self + 0xa8);
        int r5b = sl2 - *(int*)(self + 0x84);
        if (r5b < (int)func_020093d4(self, *(int*)(arg3 + 0x18))) {
            _Z14ApproachLinearRiii((int*)(self + 0x84), sl2, 0x64000);
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
                    *(int *)(self + 0x84) = sl2 - (r5b - FX_MUL(r2, 0x600));
                }
            }
        }
    }
    _ZN9dBgCh_GndD1Ev(spEC);

    return r4;
}
#pragma cplusplus on
