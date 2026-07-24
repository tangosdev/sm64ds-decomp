// NONMATCHING: div=211 (was 277 banked; 214 recompiled tip). size=0x6e0 exact.
// Frame still sub sp,#0x84 (want 0x74): +0x10 spill before locals shifts all sp offs.
// Walls: (1) one spilled vs fp + c+0x702 hoist steals fp; (2) Fix12 cos/sin interleave;
// (3) loop setup reg birth (want r7/r5/r6/r4/r8/fp). angbase tip has frame 0x74+fp=one
// but size 0x6dc. mwccarm 1.2/sp2p3 --module ov034 @ 0x02112b5c size 0x6e0
#pragma opt_common_subs off
#pragma opt_strength_reduction off
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vec3 { s32 x, y, z; };

/* Force full 0x74 frame: intentional locals + spill pad to match ROM */
struct Frame {
    s32 sp00, sp04, sp08;
    void *sp0c;
    s32 sp10, sp14, sp18;
    struct Vec3 head;    /* +0x1c */
    struct Vec3 part;    /* +0x28 */
    struct Vec3 camA;    /* +0x34 */
    struct Vec3 camB;    /* +0x40 */
    struct Vec3 scratch; /* +0x4c */
    struct Vec3 t0;      /* +0x58 */
    struct Vec3 t1;      /* +0x64 */
};

extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void func_ov034_02112604(void *c);
extern void _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void *self);
extern void _ZN5Actor11LandingDustEb(void *self, int b);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void func_0201267c(int a, void *p);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern void _ZN9Animation7AdvanceEv(void *self);
extern int DecIfAbove0_Byte(u8 *p);
extern void _Z14ApproachLinearRiii(s32 *p, s32 a, s32 b);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cyl);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, u32 j);
extern void func_ov034_02112874(void *c);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, s32 x, s32 y, s32 z, void *v, void *cb);
extern void func_ov034_021129ec(void *c);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(void *in, void *mtx, void *out);
extern void Vec3_Add(void *out, void *a, void *b);
extern void func_020092c4(void *arg0, void *out, void *target);

extern void *data_ov034_02113874[];
extern void *data_ov034_02113888[];
extern void *data_ov034_0211389c[];
extern void *data_ov034_021138b0[];
extern u32 data_ov034_02113828[];
extern s16 data_02082214[];
extern void *data_0209f318;
extern s32 data_020a0e68[];

int _ZN7Wiggler8BehaviorEv(void *arg0)
{
    s32 one;
    s32 i;
    char *c;
    char *r4;
    char *r6;
    char *r5;
    char *r7;
    s32 t;
    s32 t2;
    u32 u;
    s16 *tbl;
    void *vp;
    struct Frame fr;
    /* Extra 0x18 to absorb spills so total frame lands near 0x8c;
       try smaller pads to hit 0x74 exactly after spills. */

    c = (char *)arg0;

    if (*(u8 *)(c + 0x8df) != 0) {
        if (*(s32 *)(c + 0x8d0) + *(s32 *)(c + 0x60) < -0x1f4000)
            _ZN9ActorBase18MarkForDestructionEv(c);
    }

    func_ov034_02112604(c);

    if (*(s32 *)(c + 0xa8) <= 0) {
        _Z14ApproachLinearRsss(
            (s16 *)(c + 0x446),
            *(s16 *)(c + 0x8d8),
            (s16)((*(s32 *)(c + 0x98) / 0x1000) * 0x1e));
    }

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x708) != 0) {
        *(s32 *)(c + 0x9c) = -0x1000;
        _ZN5Actor11LandingDustEb(c, 1);
    }

    t = *(s32 *)(c + 0x170);
    if ((void *)t == *(void **)((char *)data_ov034_02113888[0] + 4) ||
        (void *)t == *(void **)((char *)data_ov034_02113874[0] + 4) ||
        (void *)t == *(void **)((char *)data_ov034_0211389c[0] + 4)) {
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x708) != 0) {
            if (((*(u32 *)(c + 0x168) << 4) >> 16) == 6) {
                if (*(u8 *)(c + 0x8db) == 2)
                    func_0201267c(0x10a, c + 0x74);
                else
                    func_0201267c(0x11d, c + 0x74);
            }
            if (*(u8 *)(c + 0x8db) == 2) {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                    *(s32 *)(c + 0x5c), *(s32 *)(c + 0x60), *(s32 *)(c + 0x64));
            }
        }
    }

    {
        void *cur = *(void **)(c + 0x170);
        vp = data_ov034_021138b0[0];
        fr.sp0c = vp;
        if (cur == *(void **)((char *)vp + 4) &&
            ((*(u32 *)(c + 0x168) << 4) >> 16) == 0xf) {
            func_0201267c(0x11e, c + 0x74);
        }
    }

    r7 = c;
    r5 = c;
    r6 = c + 0x160;
    r4 = c + 0x40c;
    i = 0;
    fr.sp10 = 0;
    fr.sp14 = 0;
    one = 1;
    fr.sp18 = 0x1000;

    do {
        u = (u16)(4 - *(u8 *)(c + 0x8db));
        t = fr.sp14;
        if (u >= 3)
            u = (u32)fr.sp10;
        if (u <= 2)
            t = one;
        if (t != 0)
            *(s32 *)(r7 + 0x370) = (s32)(u << 12);

        if (*(u8 *)(c + 0x8df) == 0)
            _ZN9Animation7AdvanceEv(r6);

        if (*(u8 *)(c + 0x8e0) == 0) {
            if (DecIfAbove0_Byte((u8 *)(c + 0x702 + i)) != 0) {
                s16 *pv = (s16 *)(c + 0x6f8 + (i << 1));
                s32 zero = 0;
                s32 round = 0x800;
                s32 mul = 0x3000;
                *pv = (s16)(*pv + 0x1200);
                t = data_02082214[((((u16)*pv) >> 4) << 1) + 1];
                t2 = (s32)(((long long)t * mul + round) >> 12);
                t2 = t2 + (zero & 0);
                *(s32 *)(r5 + 0x40c) = (t2 + 0xc000) / 10;
            } else {
                _Z14ApproachLinearRiii((s32 *)r4, fr.sp18, 0x199);
            }
        }

        r7 += 0x14;
        r6 += 0x64;
        r5 += 0xc;
        r4 += 0xc;
        i += 1;
    } while (i < 5);

    if (*(u8 *)(c + 0x8db) == 4) {
        if (*(u8 *)(c + 0x8de) != 0)
            *(s32 *)(c + 0x370) = 0x1000;
        else
            *(s32 *)(c + 0x370) = 0;
    }

    *(s16 *)(c + 0x94) = *(s16 *)(c + 0x446);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

    if (*(u8 *)(c + 0x8df) == 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x708, 0);

    *(s32 *)(c + 0x3cc) = *(s32 *)(c + 0x5c);
    *(s32 *)(c + 0x3d0) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x3d4) = *(s32 *)(c + 0x64);
    func_ov034_02112874(c);

    *(s32 *)(c + 0x3d0) = *(s32 *)(c + 0x3dc) + 0x1000;

    tbl = data_02082214;
    {
        s32 zero = 0;
        s32 round = 0x800;
        s32 scale = 0x64000;
        u16 ang;
        s32 cosv, sinv, fx;

        fr.head.x = *(s32 *)(c + 0x3cc);
        fr.head.y = *(s32 *)(c + 0x3d0);
        fr.head.z = *(s32 *)(c + 0x3d4);

        ang = *(u16 *)(c + 0x446);
        cosv = tbl[(ang >> 4) << 1];
        fx = (s32)(((long long)cosv * scale + round) >> 12);
        fx = fx + (zero & 0);
        fr.head.x = fr.head.x + fx;

        sinv = tbl[((ang >> 4) << 1) + 1];
        fx = (s32)(((long long)sinv * scale + round) >> 12);
        fx = fx + (zero & 0);
        fr.head.z = fr.head.z + fx;
    }

    *(s32 *)(c + 0x4ac) = fr.head.x;
    *(s32 *)(c + 0x4b0) = fr.head.y;
    *(s32 *)(c + 0x4b4) = fr.head.z;
    *(s32 *)(c + 0x5ec) = fr.head.x;
    *(s32 *)(c + 0x5f0) = fr.head.y;
    *(s32 *)(c + 0x5f4) = fr.head.z;

    if (*(void **)(c + 0x170) != *(void **)((char *)fr.sp0c + 4)) {
        u = (u32)((4 - *(u8 *)(c + 0x8db)) & 0xff);
        if (u < 3) {
            s32 zero = 0;
            s32 round = 0x800;
            s32 scale = 0x23000;
            u16 ang;
            s32 cosv, sinv, fx;

            fr.part.x = *(s32 *)(c + 0x3cc);
            fr.part.y = *(s32 *)(c + 0x3d0);
            fr.part.z = *(s32 *)(c + 0x3d4);
            ang = *(u16 *)(c + 0x446);
            cosv = tbl[(ang >> 4) << 1];
            fx = (s32)(((long long)cosv * scale + round) >> 12);
            fx = fx + (zero & 0);
            fr.part.x = fr.part.x + fx;
            sinv = tbl[((ang >> 4) << 1) + 1];
            fx = (s32)(((long long)sinv * scale + round) >> 12);
            fx = fx + (zero & 0);
            fr.part.z = fr.part.z + fx;
            if (u == 0)
                fr.part.y = *(s32 *)(c + 0x3d0) + 0x89000;
            else
                fr.part.y = *(s32 *)(c + 0x3d0) + 0xb9000;
            if (*(u8 *)(c + 0x8de) != 0) {
                vp = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(u32 *)(c + 0x8e4),
                    data_ov034_02113828[u],
                    fr.part.x, fr.part.y, fr.part.z, 0, 0);
                *(void **)(c + 0x8e4) = vp;
            }
        }
    }

    func_ov034_021129ec(c);

    r5 = c + 0x478;
    r4 = c + 0x5b8;
    i = 0;
    do {
        _ZN12CylinderClsn5ClearEv(r5);
        _ZN12CylinderClsn6UpdateEv(r5);
        _ZN12CylinderClsn5ClearEv(r4);
        _ZN12CylinderClsn6UpdateEv(r4);
        i += 1;
        r5 += 0x40;
        r4 += 0x40;
    } while (i < 5);

    if (*(u8 *)(c + 0x8e1) != 0) {
        fr.camB.z = 0x1f4000;
        fr.camA.y = 0x80000;
        fr.camB.y = 0x80000;
        fr.camA.x = 0;
        fr.camA.z = 0;
        fr.camB.x = 0;
        fr.scratch.x = 0;
        fr.scratch.y = 0;
        fr.scratch.z = 0;
        vp = data_0209f318;
        Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x446));
        MulVec3Mat4x3(&fr.camA, data_020a0e68, &fr.scratch);
        Vec3_Add(&fr.t0, c + 0x5c, &fr.scratch);
        fr.camA.x = fr.t0.x;
        fr.camA.y = fr.t0.y;
        fr.camA.z = fr.t0.z;
        fr.scratch.x = 0;
        fr.scratch.y = 0;
        fr.scratch.z = 0;
        MulVec3Mat4x3(&fr.camB, data_020a0e68, &fr.scratch);
        Vec3_Add(&fr.t1, c + 0x5c, &fr.scratch);
        fr.camB.x = fr.t1.x;
        fr.camB.y = fr.t1.y;
        fr.camB.z = fr.t1.z;
        func_020092c4(vp, (char *)vp + 0x80, &fr.camA);
        func_020092c4(vp, (char *)vp + 0x8c, &fr.camB);
    }

    return 1;
}