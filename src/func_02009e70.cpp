//cpp
// NONMATCHING (TERMINAL-FLOOR): functionally-verified C at the proven compiler floor.
// func_02009e70 @ 0x02009e70 (arm9, size 0x109c). 96 words diverge, ALL register-identity
// swaps (88) or same-multiset reorderings (8): zero wrong opcodes, immediates, or branches,
// semantics verified instruction-by-instruction 2026-08-01. Eight clusters, every one a
// coloring/scheduling build delta; every source axis is closed (notes 6ay, DB row has the
// full closed-axis list). Not byte-matchable without the NITRO V0.5-V0.6.1 compiler.
// For recomp/port purposes this file is complete: the compiled code is functionally
// identical to the ROM, differing only in register names and instruction order.
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"

extern "C" {
struct CamMode {
    s32 nearDist;   /* 0x00 */
    s32 farDist;    /* 0x04 */
    s32 f08;
    s32 f0c;
    s32 f10;
    s32 f14;
    s32 f18;
    s32 f1c;        /* 0x1c */
    s32 f20;        /* 0x20 */
    u16 f24;
    u16 flags;      /* 0x26 */
};

extern s16 Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern s16 Vec3_VertAngle(const struct Vector3 *a, const struct Vector3 *b);
extern void Vec3_Sub(struct Vector3 *out, const struct Vector3 *a, const struct Vector3 *b);
extern void Vec3_Add(struct Vector3 *out, const struct Vector3 *a, const struct Vector3 *b);
extern s32 Vec3_HorzLen(const struct Vector3 *v);
extern s32 LenVec3(const struct Vector3 *v);
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void Vec3_MulScalarInPlace(s32 *v, s32 s);
extern s32 *Vec3_AsrInPlace(s32 *v, s32 sh);
extern void Vec3_RotateYAndTranslate(s32 *out, s32 *in, s16 angle, s32 *src);
extern s32 Math_Function_0203b14c(s32 *p, s32 tgt, s32 rate, s32 lim, s32 step);
extern s32 ApproachAngle(s16 *cur, s32 target, s32 div, s32 band, s32 step);
extern s32 AngleDiff(s32 a, s32 b);
extern s32 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);
extern s32 _ZN4cstd4sqrtEy(u64 v);
extern u32 func_020093f4(void *p, s32 x);
extern u32 func_020093d4(void *p, s32 x);
extern s32 func_02009374(void *self);
extern s32 func_02008cb4(void *self);
extern void func_0200897c(void *self, void *arg);
extern s32 func_0200c66c(void *self, struct Vector3 *pos, void *out2, void *out1, void *out3);
extern s32 func_0200bec4(void *self, struct Vector3 *v, s32 a2, struct CamMode *mode, s32 a4);
extern void func_0200bb28(void *self, void *a);
extern void func_0200b798(void *self, void *a, s32 b);
extern void func_0200b590(void *self, void *a, s32 b);
extern void func_0200b0fc(void *self, void *a, s32 b);
extern void func_0200b990(void *self, void *a, s32 b);
extern void func_0200c9e0(void *self, s32 *a, s32 *b);
extern u32 func_02012790(u32 a);
extern s32 Sound_PlayIfNotActive(s32 a, s32 b, s32 c);

extern void _ZN9dBgCh_Lin10GetClsnPosEv(struct Vector3 *out, dBgCh_Lin *line);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *t, struct Vector3 *out);

extern s16 data_02082214[];
extern u8 data_020a0e40;
extern u16 data_0209f49c[];
extern u16 data_0209f49e[];
extern u8 data_0209f2d8;
extern signed char data_0209f2f8;
extern s32 data_0209f32c;
extern s32 data_0209b000;

extern struct CamMode data_02086fcc;
extern struct CamMode data_02086ff4;
extern struct CamMode data_0208706c;
extern struct CamMode data_02087094;
extern struct CamMode data_020870e4;
extern struct CamMode data_0208710c;
extern struct CamMode data_020871ac;
extern struct CamMode data_020871d4;
extern struct CamMode data_0208724c;
extern struct CamMode data_0208733c;
extern struct CamMode data_020873dc;
extern struct CamMode data_02087404;
extern struct CamMode data_0208742c;
extern struct CamMode data_0208747c;
extern struct CamMode data_020874cc;

static inline int CheckMode(void) { return data_0209f2d8 == 1; }

#define L0(p) ((s32)(((long long)(s32)(p)) & 0xFFFFFFFFFFFFFFFFLL))
#define FXMUL(a, b) ((s32)((((s64)(a)) * (b) + 0x800) >> 12))
#define FXMULC(a, b) ((s32)((((s64)(a)) * (s64)(b) + 0x800) >> 12))

#pragma opt_common_subs off
s32 func_02009e70(char *self)
{
    s32 sp04;
    s32 sp08;
    s32 sp0c;
    s32 sp10;
    s32 sp14;
    u16 sp18;
    struct CamMode *sp1c;
    u8 *sp20;
    s32 sp24;
    s32 sp28;
    s32 sp2c;
    s32 sp30;
    struct Vector3 sp34;
    struct Vector3 sp40;
    struct Vector3 sp4c;
    struct Vector3 sp58;
    struct Vector3 sp64;
    struct Vector3 sp70;
    struct Vector3 sp7c;
    struct Vector3 sp88;
    struct Vector3 sp94;
    struct Vector3 spa0;
    struct Vector3 spac;
    struct Vector3 spb8;
    struct Vector3 spc4;
    s32 r7;
    s32 r6;
    s32 fl;
    s32 sb;
    s32 r4;
    s32 t1;
    s32 sba;
    s32 sl;
    s32 r5;
    s32 fp;
    u32 r7h;
    struct CamMode *m;
    s32 t0;

    if (*(s32 *)(self + 0x110) == 0) {
        *(s16 *)(self + 0x17c) = Vec3_HorzAngle((struct Vector3 *)(self + 0x80), (struct Vector3 *)(self + 0x8c));
        goto L_TAIL;
    }

    sp1c = (*(s32 *)(self + 0x154) & 0x100) ? &data_0208747c : *(struct CamMode **)(self + 0x13c);
    Vec3_Sub(&sp34, (struct Vector3 *)(*(char **)(self + 0x110) + 0x5c), (struct Vector3 *)(self + 0x98));
    {
        struct Vector3 *q = (struct Vector3 *)L0(*(char **)(self + 0x110) + 0x5c);
        *(s32 *)(self + 0x98) = q->x;
        *(s32 *)(self + 0x9c) = q->y;
        *(s32 *)(self + 0xa0) = q->z;
    }
    r7 = Vec3_HorzLen(&sp34);
    sp20 = 0;
    sp24 = 0;
    if (*(s32 *)(self + 0x118) != 0) {
        struct Vector3 *q;
        sp24 = 1;
        *(s32 *)(self + 0x114) = 0;
        q = (struct Vector3 *)L0(*(char **)(self + 0x118) + 0x5c);
        *(s32 *)(self + 0x120) = q->x;
        *(s32 *)(self + 0x124) = q->y;
        *(s32 *)(self + 0x128) = q->z;
        *(s32 *)(self + 0x12c) = 0xdfe60;
    }
    r6 = func_0200c66c(self, (struct Vector3 *)(self + 0x98), &sp20, &sp1c, &sp24);
    r4 = 0;
    if (sp24 != 0) {
        r4 = 0x1000;
        SubVec3((struct Vector3 *)(self + 0x120), (struct Vector3 *)(self + 0x98), (struct Vector3 *)(self + 0x120));
    }
    Math_Function_0203b14c((s32 *)(self + 0x130), r4, 0x100, 0x80, 0x10);
    sb = 0;

    fl = sp1c->flags;
    r4 = sp1c->f1c;
    r5 = sp1c->f20;
    if ((fl & 1) == 0 && (*(s32 *)(self + 0x154) & 2) != 0) {
        r4 = FXMULC(r4, 0x1800);
        r5 = FXMULC(r5, 0x1800);
    }
    if (CheckMode() && (fl & 2) != 0 && func_02009374(self) != 0) {
        r4 = FXMULC(r4, 0x14cc);
        r5 = FXMULC(r5, 0x14cc);
    }
    if (*(s32 *)(self + 0x154) & 0x80) {
        r4 = FXMULC(r4, 0x1800);
        r5 = FXMULC(r5, 0x1800);
    }
    t0 = *(s32 *)(self + 0x12c) - r4;
    fp = func_020093f4(self, r4 + FXMUL(t0, *(s32 *)(self + 0x130)));
    if (sp24 != 0) {
        sl = fp;
    } else {
        if (r5 < 0) r5 = -r5;
        sl = func_020093f4(self, r5);
    }
    sp04 = func_0200bec4(self, (struct Vector3 *)(self + 0x98), r6, sp1c, r7);
    r5 = *(u16 *)((char *)data_0209f49e + data_020a0e40 * 0x18) & 0x4300;
    if (*(s32 *)(self + 0x118) != 0) {
        *(s16 *)(self + 0x17c) = Vec3_HorzAngle((struct Vector3 *)(self + 0x80), (struct Vector3 *)(self + 0x8c));
        goto L_AE14;
    }
    if (sp1c == &data_0208733c) {
        func_0200bb28(self, sp20);
        r5 = 0;
        goto L_AE14;
    }
    if (sp1c == &data_020873dc) {
        func_0200b798(self, sp20, r6);
        goto L_AE14;
    }
    if (*(s32 *)(self + 0x154) & 0x400) goto L_AE14;
    if (sp1c == &data_02087404) {
        func_0200b590(self, sp20, r6);
        goto L_AE14;
    }
    if (sp1c == &data_0208742c) {
        func_0200b0fc(self, sp20, r6);
        goto L_AE14;
    }
    *(u32 *)L0(self + 0x154) |= 0x1000;
    sp08 = func_020093f4(self, sp1c->nearDist);
    sp0c = func_020093f4(self, sp1c->farDist);
    if (sp20 != 0 && sp20[0] == 2) {
        if (sp20[1] == 0) {
            s32 tx = *(s16 *)(sp20 + 2) << 12;
            s32 tz = *(s16 *)(sp20 + 6) << 12;
            s32 ty = *(s16 *)(sp20 + 4) << 12;
            *(s32 *)(self + 0x8c) = tx;
            *(s32 *)(self + 0x90) = ty;
            *(s32 *)(self + 0x94) = tz;
        } else {
            s16 a = (s16)(*(s16 *)(sp20 + 0xa) + 0x8000);
            if (*(s16 *)(self + 0x186) != a) {
                *(s16 *)(self + 0x186) = a;
                *(u8 *)(self + 0x1a6) = 1;
            }
        }
    } else {
        if (*(u8 *)(self + 0x1a6) == 0) {
            *(s16 *)L0(self + 0x186) += 0x8000;
        }
    }
    Vec3_Sub(&sp40, (struct Vector3 *)(self + 0x8c), (struct Vector3 *)(self + 0x80));
    sp28 = LenVec3(&sp40);
    sp10 = 0x80000000;
    {
        s32 ty = *(s32 *)(self + 0x9c);
        s32 tz = *(s32 *)(self + 0xa0);
        s32 tx = *(s32 *)(self + 0x98);
        sp4c.x = tx;
        sp4c.y = ty + 0x3e8000;
        sp4c.z = tz;
    }
    r4 = 0x7fffffff;
    sb = *(s32 *)(self + 0x9c) + 0x82500;
    {
    dBgCh_Lin line;
    func_0200897c(self, &line);
    line.SetObjAndLine(*(struct Vector3 *)(self + 0x98), sp4c, 0);
    if (line.DetectClsn() != 0) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&sp94, &line);
        r4 = sp94.y - 0x80000;
        if (r4 < sb) r4 = sb;
    }
    if (*(s32 *)(self + 0x154) & 0x100) goto L_A534;
    if ((sp1c->flags & 8) == 0) goto L_A534;
    {
        s32 tz = *(s32 *)(self + 0xa0);
        s32 tx = *(s32 *)(self + 0x98);
        sp58.x = tx;
        sp58.y = sb;
        sp58.z = tz;
    }
    if (sb > r4) goto L_A534;
    line.SetObjAndLine(sp58, *(struct Vector3 *)(self + 0x8c), 0);
    if (line.DetectClsn() == 0) goto L_A534;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&line.surface, &sp64);
    _ZN9dBgCh_Lin10GetClsnPosEv(&spa0, &line);
    *(s32 *)(self + 0xe0) = spa0.x;
    *(s32 *)(self + 0xe4) = spa0.y;
    *(s32 *)(self + 0xe8) = spa0.z;
    *(s32 *)(self + 0xec) = sp64.x;
    *(s32 *)(self + 0xf0) = sp64.y;
    *(s32 *)(self + 0xf4) = sp64.z;
    t0 = sp64.y;
    if (t0 < 0) t0 = -t0;
    if (t0 > 0xb50) {
        *(u32 *)L0(self + 0x154) |= 0x80200;
        goto L_A534;
    }
    _ZN9dBgCh_Lin10GetClsnPosEv(&sp70, &line);
    sba = (s16)(_ZN4cstd5atan2E5Fix12IiES1_(sp64.x, sp64.z) + 0x8000);
    t1 = *(u8 *)(self + 0x1a6);
    t0 = (s16)(sba - *(s16 *)(self + 0x17c));
    if (t0 < 0) t0 = 0x4000; else t0 = -0x4000;
    sba = (s16)(sba + t0);
    if (t1 != 0) {
        if (AngleDiff(sba, *(s16 *)(self + 0x186)) >= 0x4000) sba = *(s16 *)(self + 0x186);
    }
    *(s16 *)(self + 0x186) = sba;
    *(u8 *)(self + 0x1a6) = 1;
    t0 = (s16)(*(s16 *)(self + 0x17c) - *(s16 *)(self + 0x186));
    if (r5 & 0x200) t0 = (s16)(-t0);
    if (t0 > 0) r5 = 0;
    *(u32 *)L0(self + 0x154) |= 0x40000;
L_A534:
    if (r5 != 0) {
        if (r5 & 0x4000) func_02012790(0x1a);
        *(u8 *)(self + 0x1a6) = 0;
    } else {
        if (*(u8 *)(self + 0x1a6) == 0) {
            r5 = *(u16 *)((char *)data_0209f49c + data_020a0e40 * 0x18) & 0x4300;
        }
    }
    if (*(s32 *)(self + 0x154) & 0x100) {
        if (sp28 > (s32)func_020093f4(self, 0xdfe60)) *(u32 *)L0(self + 0x154) &= ~0x100;
    }
    if (r5 != 0) goto L_A5F8;
    if (*(u16 *)(self + 0x1a0) != 0) goto L_A5F8;
    if (*(u8 *)(self + 0x1a6) != 0) goto L_A5F8;
    if (r7 <= sp08) goto L_A5F8;
    sb = sp28;
    if (sb < fp) goto L_A5F8;
    if (sb > sp0c) goto L_AA6C;

L_A5F8:
    sb = *(s16 *)(self + 0x17c);
    *(s16 *)(self + 0x17c) = Vec3_HorzAngle((struct Vector3 *)(self + 0x80), (struct Vector3 *)(self + 0x8c));
    if (r5 != 0) goto L_A628;
    if (*(u16 *)(self + 0x1a0) == 0) goto L_A6FC;
L_A628:
    if (r5 & 0x4000) goto L_A640;
    if (*(u16 *)(self + 0x1a0) == 0) goto L_A6A8;
L_A640:
    if (r5 != 0) {
        *(s16 *)(self + 0x19e) = *(s16 *)(*(char **)(self + 0x110) + 0x8e) + 0x8000;
    }
    *(u16 *)(self + 0x1a0) = ApproachAngle((s16 *)(self + 0x17c), *(s16 *)(self + 0x19e), 4, 0x800, 0x400) != 0;
    if (*(u16 *)(self + 0x1a0) != 0) *(u32 *)L0(self + 0x154) |= 0x60;
    goto L_A6F4;
L_A6A8:
    if (r5 & 0x200) {
        *(s16 *)L0(self + 0x17c) -= 0x400;
        *(u32 *)L0(self + 0x154) |= 0x20;
    } else {
        *(s16 *)L0(self + 0x17c) += 0x400;
        *(u32 *)L0(self + 0x154) |= 0x40;
    }
L_A6F4:
    r5 = 0;
    goto L_A814;
L_A6FC:
    if (*(u8 *)(self + 0x1a6) != 0) {
        if (ApproachAngle((s16 *)(self + 0x17c), *(s16 *)(self + 0x186), 0x10, 0x200, 0x100) == 0)
            *(u8 *)(self + 0x1a6) = 0;
        goto L_A814;
    }
    m = sp1c;
    if (m == &data_020871ac || m == &data_020871d4) {
        func_0200b990(self, sp20, r7);
        goto L_A814;
    }
    if (sp24 != 0) goto L_A814;
    if (m == &data_0208724c)
        t0 = *(s16 *)(*(char **)(self + 0x110) + 0x94);
    else
        t0 = *(s16 *)(*(char **)(self + 0x110) + 0x8e);
    t0 = (s16)(t0 + 0x8000);
    if (m == &data_020874cc) {
        *(s16 *)(self + 0x17c) = t0 + *(s16 *)(self + 0x184);
        goto L_A814;
    }
    if (sp28 > sp0c) goto L_A7DC;
    ApproachAngle((s16 *)(self + 0x17c), t0, 0x14, 0x600, 0x28);
    goto L_A814;
L_A7DC:
    if (m->flags & 0x100) {
        *(s16 *)(self + 0x17c) = sb;
        goto L_A814;
    }
    if (r7 > sp08) goto L_A814;
    ApproachAngle((s16 *)(self + 0x17c), t0, 0x14, 0x64, 0x28);

L_A814:
    sp14 = sp28;
    if (sp28 <= sl) goto L_A984;
    if (r7 == 0) goto L_A94C;
    if (sp1c != &data_02086fcc) goto L_A94C;
    sb = _ZN4cstd5atan2E5Fix12IiES1_(sp34.y, Vec3_HorzLen(&sp34));
    if (sb < -0x3f80) sb = -0x3f80;
    else if (sb > 0x3f80) sb = 0x3f80;
    fp = Vec3_HorzLen(&sp40);
    sp18 = _ZN4cstd5atan2E5Fix12IiES1_(-sp40.y, fp);
    ApproachAngle((s16 *)&sp18, sb, 0x20, 0x800, 0);
    t0 = (sp18 >> 4) * 2;
    sp40.y = FXMUL(sp28, -data_02082214[t0]);
    t0 = _ZN4cstd4fdivEii(FXMUL(sp28, data_02082214[t0 + 1]), fp);
    sp40.x = FXMUL(sp40.x, t0);
    sp40.z = FXMUL(sp40.z, t0);
L_A94C:
    Math_Function_0203b14c(&sp28, sl, 0x400, r7 + 0x20000, 0x100);
    Vec3_MulScalarInPlace((s32 *)&sp40, _ZN4cstd4fdivEii(sp28, sp14));
    goto L_AA34;
L_A984:
    if (sp28 >= fp) goto L_AA34;
    Math_Function_0203b14c(&sp28, fp, 0x400, r7 + 0x20000, 0x100);
    r7 = _ZN4cstd4sqrtEy((u64)((s64)sp28 * sp28 - (s64)sp40.y * sp40.y));
    t0 = _ZN4cstd4fdivEii(r7, Vec3_HorzLen(&sp40));
    sp40.x = FXMUL(sp40.x, t0);
    sp40.z = FXMUL(sp40.z, t0);
    *(s16 *)L0(self + 0x188) -= *(s16 *)(self + 0x17c) - sb;
L_AA34:
    t0 = Vec3_HorzLen(&sp40);
    if (t0 < 0x1000) t0 = 0x1000;
    sp40.z = t0;
    sp40.x = 0;
    Vec3_RotateYAndTranslate((s32 *)(self + 0x8c), (s32 *)(self + 0x80), *(s16 *)(self + 0x17c), (s32 *)&sp40);
    goto L_AAD0;
L_AA6C:
    if (sb > sl) {
        Math_Function_0203b14c(&sp28, sl, 0x400, r7 + 0x20000, 0x100);
        Vec3_MulScalarInPlace((s32 *)&sp40, _ZN4cstd4fdivEii(sp28, sb));
        Vec3_Add(&spac, (struct Vector3 *)(self + 0x80), &sp40);
        *(s32 *)(self + 0x8c) = spac.x;
        *(s32 *)(self + 0x90) = spac.y;
        *(s32 *)(self + 0x94) = spac.z;
    }
L_AAD0:
    if (func_02008cb4(self) != 0) {
        if (*(s32 *)(self + 0x154) & 0x60) {
            r5 = 1;
            *(u32 *)L0(self + 0x154) &= ~0x60;
        }
    }
    {
        s32 ty = *(s32 *)(self + 0x90) + 0x100000;
        s32 tz = *(s32 *)(self + 0x94);
        s32 tx = *(s32 *)(self + 0x8c);
        sp4c.x = tx;
        sp4c.y = ty;
        sp4c.z = tz;
    }
    line.SetObjAndLine(*(struct Vector3 *)(self + 0x8c), sp4c, 0);
    if (line.DetectClsn() != 0) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&spb8, &line);
        sp4c.y = spb8.y;
        if (r4 > spb8.y - 0x80000) r4 = spb8.y - 0x80000;
    }
    Vec3_Add(&sp7c, (struct Vector3 *)(self + 0x8c), (struct Vector3 *)(self + 0x80));
    Vec3_AsrInPlace((s32 *)&sp7c, 1);
    sp88.x = sp7c.x;
    sp88.y = sp7c.y + 0x100000;
    sp88.z = sp7c.z;
    line.SetObjAndLine(sp7c, sp88, 0);
    if (line.DetectClsn() != 0) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&spc4, &line);
        sp88.y = spc4.y - 0x80000;
        if (r4 > sp88.y) r4 = sp88.y;
    }
    sb = *(s32 *)(self + 0x9c);
    dBgCh_Gnd ground;
    ground.mProbeHeight = 0x5dc000;
    func_0200897c(self, &ground);
    ground.SetObjAndPos(sp4c, 0);
    if (sp1c > &data_02086fcc) ground.StartDetectingWater();
    if (ground.DetectClsn() != 0) sb = ground.clsnY;
    if (sp1c == &data_02086ff4) {
        if (sb < data_0209f32c) sb = data_0209f32c;
        r7h = sb + 0x64000;
        goto L_AD94;
    }
    r7h = sb;
    if (sb < r6) r7h = r6;
    r7h = r7h + func_020093d4(self, (r6 - sp04) + ((*(s32 *)(self + 0x154) & 2) ? 0x1e8000 : 0x138000));
    if (sp1c->flags & 0x80) {
        if (*(s32 *)(self + 0x90) > (s32)r7h) r7h = *(s32 *)(self + 0x90);
    }
    sp2c = sp1c->f08;
    sp30 = sp1c->f0c;
    func_0200c9e0(self, &sp2c, &sp30);
    sp40.y = r7h - *(s32 *)(self + 0x84);
    if (sp1c == &data_0208706c && sp34.y < 0) t0 = 0xa49;
    else t0 = sp2c;
    r6 = FXMUL(sp28, t0);
    t0 = FXMUL(sp30, sp28);
    if (sp40.y < r6) r7h = *(s32 *)(self + 0x84) + r6;
    else if (sp40.y > t0) r7h = *(s32 *)(self + 0x84) + t0;
    if (data_0209f2f8 == 0x26) {
        if (*(s32 *)(self + 0x90) < sb + 0x25bc8) *(s32 *)(self + 0x90) = sb + 0x25bc8;
    }
    if (r4 < sb + 0x25bc8) r4 = sp4c.y;
    if (sb + 0x25bc8 > (s32)0x80000000) sp10 = sb + 0x25bc8;
L_AD94:
    if (sp1c == &data_02087094) goto L_ADD4;
    if (sp1c == &data_020870e4) goto L_ADD4;
    if (sp1c == &data_0208710c) goto L_ADD4;
    if ((s32)r7h < sp10) r7h = sp10;
    else if ((s32)r7h > r4) r7h = r4;
L_ADD4:
    Math_Function_0203b14c((s32 *)(self + 0x90), r7h, 0x300, 0x14000, 0x100);
    *(s16 *)(self + 0x17c) = Vec3_HorzAngle((struct Vector3 *)(self + 0x80), (struct Vector3 *)(self + 0x8c));
    }
L_AE14:
    if (r5 != 0) {
        if (data_0209b000 == 0) func_02012790(0xe);
        data_0209b000 = r5;
    } else {
        data_0209b000 = 0;
        if (*(s32 *)(self + 0x154) & 0x60) {
            *(s32 *)(self + 0x158) = Sound_PlayIfNotActive(*(s32 *)(self + 0x158), 2, 0x43);
        }
    }
L_TAIL:
    *(s16 *)(self + 0x17e) = Vec3_VertAngle((struct Vector3 *)(self + 0x80), (struct Vector3 *)(self + 0x8c));
    return 1;
}
}
