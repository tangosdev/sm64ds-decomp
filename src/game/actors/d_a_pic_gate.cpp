//cpp
/* ov080/daPicGate_c -- picture-gate helpers: the mesh machinery under the
 * promoted TU (src/actors/daPicGate_c.cpp, .text 0x021264ec..0x02126fbc).
 * .text 0x02125404..0x021261f4: the destructor pair, then the fifteen
 * func_ov080_* helpers that fill the range up to the class TU.
 *
 *   ~daPicGate_c         D1 0x02125404, D0 0x02125428 -- the key function, so
 *                        the vtable and typeinfo come out of this file too.
 *   func_ov080_02125460  LoadMaterial -- texture/palette/light GX ports from
 *                        the material record kept in mTexRecord; states 2/3
 *                        (and a stopped wave) force ambient white.
 *   func_ov080_0212555c  BuildGateMatrix -- mMtx from position, rotation and
 *                        the param1 width, and mInvMtx as its inverse.
 *   func_ov080_02125630  LoadTexture -- decompress this picture's texture
 *                        record to VRAM. `this` is unused: it was a method
 *                        already.
 *   func_ov080_021256f8  HitTest -- map the closest player's current and
 *                        previous position into gate space; inside the mesh
 *                        begins a wave, beyond the far edge begins one and
 *                        plays a sound.
 *   func_ov080_02125940  BuildNormals -- per-cell normal from the neighbour
 *                        cross product, packed for G3_NORMAL.
 *   func_ov080_02125af0  FlattenFrame -- cells off the current width/height
 *                        get z = 0 and the flat packed normal.
 *   func_ov080_02125bb0  RippleHeight -- wave height at a cell's distance.
 *   func_ov080_02125cd4 / func_ov080_02125d08 -- wave-clock math on
 *                        mWaveParams.
 *   func_ov080_02125d64 -- distance of every cell from the wave origin.
 *   func_ov080_02125de0  BeginWave -- store the origin, refresh cell dists,
 *                        pick the WaveParams row, reload mWaveTimer.
 *   func_ov080_02125f00 / func_ov080_02126124 -- mCorners defaults for the
 *                        flat quad states; the texCoord pattern differs.
 *   func_ov080_02125fd0  DrawFlat -- emit the flat quad.
 *   func_ov080_02126120 -- the empty state slot.
 *
 * The wave-mesh renderer at 0x021261f4 is the one hole in the run: it sits
 * a div=5 mwccarm 2004/b56 scheduling tie away from the ROM (notes/
 * mwccarm-codegen.md 6cf/6cz) and is parked as a nonmatch draft in
 * src/_ZN11daPicGate_c19func_ov080_021261f4Ev.cpp.
 *
 * deslop leftovers:
 * - Vertex.texCoord/colour reach the GX ports through literal
 *   *(int *)0x0400xxxx stores: the ports stay unnamed so the stores keep
 *   the same non-volatile idiom the promoted TU uses.
 * - HitTest keeps the goto skeleton the ROM's branch shape needs (the
 *   positive/negative-z split), and BuildNormals keeps the triple-store
 *   zero-init of its Vec3 locals.
 */

#include "daPicGate_c.h"
#include "common.h"
#include "Player.h"
#include "Model.h"
#include "SharedFilePtr.h"
#include "private/Ov080Mat.h"

#pragma defer_codegen off

/* Geometry command ports. Plain stores, not volatile: a volatile port
 * reloads where the cartridge keeps the value in a register. */
#define G3_MTX_MODE     (*(int *)0x04000440)
#define G3_MTX_PUSH     (*(int *)0x04000444)
#define G3_MTX_POP      (*(int *)0x04000448)
#define G3_MTX_SCALE    (*(int *)0x0400046c)
#define G3_NORMAL       (*(int *)0x04000484)
#define G3_TEXCOORD     (*(int *)0x04000488)
#define G3_VTX_16       (*(int *)0x0400048c)
#define G3_LIGHT_VECTOR (*(int *)0x040004c8)
#define G3_LIGHT_COLOR  (*(int *)0x040004cc)
#define G3_BEGIN        (*(int *)0x04000500)
#define G3_END          (*(int *)0x04000504)

/* The material block's ports keep the volatile spelling LoadMaterial was
 * matched under; see notes/mwccarm-codegen.md 6ba/6bb. */
#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)
#define reg_G3_TEXPLTT_BASE   (*(volatile u32 *)0x040004ac)
#define reg_G3_DIF_AMB        (*(volatile u32 *)0x040004c0)
#define reg_G3_SPE_EMI        (*(volatile u32 *)0x040004c4)
#define reg_G3_POLYGON_ATTR   (*(volatile u32 *)0x040004a4)

/* A plain {x, y, z} for stack locals: types.h Vector3 declares an empty
   destructor under C++, and a stack object would emit ~Vector3. */
struct Vec3 { int x, y, z; };
/* CrossVec3's own file spells the same triple Vec3_Fix12; the declaration
 * has to carry its definition's name to agree. */
typedef struct { int x, y, z; } Vec3_Fix12;

extern "C" {
void  Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
void  Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
int   InvMat4x3(int *src, int *dst);
void  MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void  SubVec3(Vec3 *a, Vec3 *b, Vec3 *out);
void  CrossVec3(const Vec3_Fix12 *a, const Vec3_Fix12 *b, Vec3_Fix12 *out);
void  NormalizeVec3(int *v, int *out);
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
void  MulMat4x3Mat4x3(const int *m1, const int *m0, int *mF);
void  func_020553a4(void *m);
void  func_02012694(u32 id, void *pos);
Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);
int   __aeabi_idiv(int a, int b);
void *func_0201787c(SharedFilePtr *sfp);
u32   func_02045ad8(const void *src, u32 size);
unsigned int func_02045a50(const void *src, unsigned int size);
}

extern Matrix4x3 data_020a0e68;
extern Matrix4x3 data_0209b3ec;
extern short data_02082214[];
extern char data_ov080_021277a8[];
extern SharedFilePtr *data_ov080_0212775c[];
extern unsigned char data_ov080_02128688[];
extern unsigned char data_ov080_0212869c[];

static inline void G3_TexImageParam(u32 addr, u32 texFmt, u32 texGen, u32 sizeS, u32 sizeT,
                                    u32 repeat, u32 flip, u32 pltt0)
{
    reg_G3_TEXIMAGE_PARAM = (u32)((addr >> 3) | (texFmt << 26) | (texGen << 30)
        | (sizeS << 20) | (sizeT << 23) | (repeat << 16) | (flip << 18) | (pltt0 << 29));
}

static inline void G3_TexPlttBase(u32 addr, u32 texFmt)
{
    reg_G3_TEXPLTT_BASE = addr >> (4 - (texFmt == 2 ? 1 : 0));
}

// @symbol _ZN11daPicGate_cD1Ev
// @symbol _ZN11daPicGate_cD0Ev
/* Members in reverse, then ~dActor_c. Written first: with codegen not
 * deferred it comes out as D1, D0, D2 in place, the cartridge's order. */
daPicGate_c::~daPicGate_c()
{
}

// @symbol _ZN11daPicGate_c19func_ov080_02125460Ev
#pragma opt_propagation off
void daPicGate_c::func_ov080_02125460()
{
    Ov080Mat *mat = (Ov080Mat *)mTexRecord;
    unsigned char mode = (param1 >> 0xd) & 3;
    u32 difAmb = mat->difAmb;
    if (mode >= 2 || mWaveTimer == 0)
        difAmb |= 0x7fff0000;
    reg_G3_DIF_AMB = difAmb;
    reg_G3_SPE_EMI = ((Ov080Mat *)mTexRecord)->speEmi;
    {
        Ov080Mat *m2 = (Ov080Mat *)mTexRecord;
        G3_TexImageParam(m2->texAddr, (m2->param >> 0x1a) & 7, 0, (m2->param >> 0x14) & 7,
                         (m2->param >> 0x17) & 7, 0, 0, (m2->param >> 0x1d) & 1);
    }
    {
        Ov080Mat *m3 = (Ov080Mat *)mTexRecord;
        G3_TexPlttBase(m3->pltAddr, (m3->param >> 0x1a) & 7);
    }
    reg_G3_POLYGON_ATTR = 0x11f0088;
}
#pragma opt_propagation on

// @symbol _ZN11daPicGate_c19func_ov080_0212555cEv
/* mMtx = translate(pos) * rotate(mAngle) * translate(-width/2): the gate's
 * mesh sits centred on its position, and mInvMtx maps back into it. */
void daPicGate_c::func_ov080_0212555c()
{
    Vec3 v;
    Vec3_Asr(&v, (Vec3 *)&mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68,
        -(((unsigned char)(param1 & 0xf) + 1) * 0x64000) / 2 >> 3, 0, 0);
    *(Matrix4x3 *)mMtx = data_020a0e68;
    InvMat4x3((int *)&data_020a0e68, (int *)&data_020a0e68);
    *(Matrix4x3 *)mInvMtx = data_020a0e68;
}

// @symbol _ZN11daPicGate_c19func_ov080_02125630Ei
/* LoadTexture: one 0x18-byte record per picture id, decompressed to VRAM on
 * first use (the parallel data_ov080_0212869c table is the loaded flag).
 * The record keeps the texture port address, the palette port address and
 * the packed parameter word LoadMaterial pushes. */
u8 *daPicGate_c::func_ov080_02125630(s32 i)
{
    int off = 0x18 * i;
    unsigned char *e = &data_ov080_02128688[off];
    unsigned char *fl = &data_ov080_0212869c[off];
    if (data_ov080_0212869c[off] == 0) {
        char *a;
        char *b;
        char *c;
        SharedFilePtr *sfp = data_ov080_0212775c[i];
        char *o = (char *)func_0201787c(sfp);
        a = *(char **)(o + 0x18);
        b = *(char **)(o + 0x28);
        c = *(char **)(o + 0x20);
        int sz;
        *(int *)(e + 0) = (int)Model::LoadCompressedTextureToVram(
            *(char **)(a + 4), *(unsigned int *)(a + 8),
            *(char **)(a + 4) + *(unsigned int *)(a + 8));
        sz = *(int *)(c + 8);
        if (sz <= 8)
            *(int *)(e + 4) = (int)func_02045ad8(*(const void **)(c + 4), (unsigned int)sz);
        else
            *(int *)(e + 4) = (int)func_02045a50(*(const void **)(c + 4), (unsigned int)sz);
        *(int *)(e + 8) = *(int *)(b + 0x28);
        *(int *)(e + 0xc) = *(int *)(b + 0x2c);
        *(int *)(e + 0x10) = *(int *)(a + 0x10);
        *fl = 1;
        sfp->Release();
    }
    return e;
}

// @symbol _ZN11daPicGate_c19func_ov080_021256f8Ev
/* HitTest: the closest player's position and previous position transformed
 * into gate space (mInvMtx). A z-straddle means the player crossed the gate
 * plane this frame: inside the cell bounds starts a wave toward the field,
 * beyond the far edge starts one from the other side and clicks. */
void daPicGate_c::func_ov080_021256f8()
{
    Vec3 a;
    Vec3 b;
    Vec3 ob;
    Vec3 oa;
    Player *r5;
    int za;
    int zb;
    int x, y;
    int lim;
    u32 raw;
    u8 nib;

    r5 = ClosestPlayer();
    if (r5 == 0)
        return;

    Vec3_Asr(&a, (Vec3 *)&r5->mPrevPosX, 3);
    Vec3_Asr(&b, (Vec3 *)&r5->mPosX, 3);
    data_020a0e68 = *(Matrix4x3 *)mInvMtx;
    MulVec3Mat4x3((Vector3 *)&a, &data_020a0e68, (Vector3 *)&oa);
    MulVec3Mat4x3((Vector3 *)&b, &data_020a0e68, (Vector3 *)&ob);

    za = oa.z;
    if (za <= 0)
        goto L9c;
    zb = ob.z;
    if (zb <= 0)
        goto Lb0;
L9c:
    if (za >= 0)
        goto neg;
    zb = ob.z;
    if (zb < 0)
        goto neg;
Lb0:
    x = ob.x << 3;
    y = ob.y << 3;
    ob.x = x;
    ob.y = y;
    if (x < 0)
        return;
    raw = param1;
    nib = raw & 0xf;
    lim = (nib + 1) * 0x64000;
    if (x > lim)
        return;
    if (y < 0)
        return;
    nib = (raw >> 4) & 0xf;
    lim = (nib + 1) * 0x64000;
    if (y > lim)
        return;
    func_ov080_02125de0(x, y, 1);
    return;

neg:
    if (za <= 0xa000)
        return;
    zb = ob.z;
    if (zb > 0xa000)
        return;
    if (mWaveTimer != 0)
        return;
    x = ob.x << 3;
    y = ob.y << 3;
    ob.x = x;
    ob.y = y;
    if (x < -0x46000)
        return;
    raw = param1;
    nib = raw & 0xf;
    lim = (nib + 1) * 0x64000 + 0x46000;
    if (x > lim)
        return;
    if (y < -0xc8000)
        return;
    nib = (raw >> 4) & 0xf;
    lim = (nib + 1) * 0x64000 + 0xc8000;
    if (y > lim)
        return;
    func_ov080_02125de0(x, y, 0);
    func_02012694(0x7b, &mCamSpacePosX);
}

// @symbol _ZN11daPicGate_c19func_ov080_02125940Ev
/* BuildNormals: each cell's normal is the cross product of its column and
 * row neighbours (edge cells take the one-sided edge), halved in z, then
 * normalized and packed 10 bits per axis into Vertex::color. */
void daPicGate_c::func_ov080_02125940()
{
    Vec3 ej;
    Vec3 ei;
    Vec3 n;
    Vertex *p;
    int i;
    int j;
    int mask = 0x3ff;

    p = mCells;

    for (i = 0; i < mRows; i++) {
        for (j = 0; j < mCols; j++) {
            int *q;
            int z = 0;
            q = (int *)&ej; q[0] = z; q[1] = z; q[2] = z;
            q = (int *)&ei; q[0] = z; q[1] = z; q[2] = z;
            q = (int *)&n;  q[0] = z; q[1] = z; q[2] = z;

            if (j == 0)
                SubVec3((Vec3 *)p, (Vec3 *)(p + 1), &ej);
            else if (j == mCols - 1)
                SubVec3((Vec3 *)(p - 1), (Vec3 *)p, &ej);
            else
                SubVec3((Vec3 *)(p - 1), (Vec3 *)(p + 1), &ej);

            if (i == 0)
                SubVec3((Vec3 *)p, (Vec3 *)(p + mRows), &ei);
            else if (i == mRows - 1)
                SubVec3((Vec3 *)(p - mRows), (Vec3 *)p, &ei);
            else
                SubVec3((Vec3 *)(p - mRows), (Vec3 *)(p + mRows), &ei);

            CrossVec3((const Vec3_Fix12 *)&ej, (const Vec3_Fix12 *)&ei, (Vec3_Fix12 *)&n);
            n.z >>= 1;
            NormalizeVec3((int *)&n, (int *)&n);
            p->color = (n.x >> 3 & mask)
                | ((n.y >> 3 & mask) << 10)
                | ((n.z >> 3 & mask) << 20);
            p += 1;
        }
    }
}

// @symbol _ZN11daPicGate_c19func_ov080_02125af0Ev
/* FlattenFrame: every cell whose x or y is off the picture's width/height
 * keeps its edge; the interior is reset to z = 0 and the flat packed
 * normal (0x1ff00000). */
void daPicGate_c::func_ov080_02125af0()
{
    int i;
    int cnt = mNumCells;
    i = 0;
    if (cnt <= 0) return;
    do {
        Vertex *e = mCells + i;
        int v = e->x;
        if (v != 0) {
            unsigned int t = param1;
            int a = ((unsigned char)(t & 0xf) + 1) * 0x64000;
            if (v != a) {
                int v2 = e->y;
                if (v2 != 0) {
                    int b = ((unsigned char)((t >> 4) & 0xf) + 1) * 0x64000;
                    if (v2 != b) goto next;
                }
            }
        }
        e->z = 0;
        {
            Vertex *e2 = mCells + i;
            e2->color = 0x1ff00000;
        }
    next:
        i++;
    } while (i < mNumCells);
}

// @symbol _ZN11daPicGate_c19func_ov080_02125bb0Ei
/* RippleHeight: the wave height at a cell, from the phase the wave clock
 * has reached (func_ov080_02125d08) scaled down linearly over the wave's
 * run, looked up in the quarter-wave sine table data_02082214. */
s32 daPicGate_c::func_ov080_02125bb0(s32 r6)
{
    unsigned short r4 = mWaveTimer;
    int *obj;
    int r5res, r4res, sum, idx;
    Fix12i f;
    short tv;
    long long prod;

    if (r4 == 0)
        return 0;

    if (((unsigned char)((param1 >> 13) & 3)) == 0) {
        obj = (int *)mWaveParams;
        {
            int a = __aeabi_idiv(0xffff, obj[1]);
            int b = __aeabi_idiv(obj[2], a);
            if (r6 > b * (obj[4] - r4))
                return 0;
        }
    }

    r5res = func_ov080_02125d08(r6);
    r4res = func_ov080_02125cd4(mWaveParams->duration - mWaveTimer);

    f = _ZN4cstd4fdivEii(r4res, mWaveParams->unk_0c);
    prod = (long long)(-f) * (long long)r6;
    sum = r4res + (int)((prod + 0x800) >> 12);
    if (sum < 0)
        sum = 0;

    idx = (unsigned short)r5res >> 4;
    tv = *(short *)((char *)data_02082214 + (idx << 2));
    prod = (long long)tv * (long long)sum;
    return (int)((prod + 0x800) >> 12);
}

// @symbol _ZN11daPicGate_c19func_ov080_02125cd4Ei
/* unk_00 / duration of the current WaveParams row, as a frame count. */
s32 daPicGate_c::func_ov080_02125cd4(s32 n)
{
    const WaveParams *p = mWaveParams;
    int r4 = p->unk_00;
    int r1 = p->duration;
    int r = __aeabi_idiv(r4, r1);
    return r4 - r * n;
}

// @symbol _ZN11daPicGate_c19func_ov080_02125d08Ei
/* frames * unk_08, 4.12 fixed-point, minus the running phase mWavePhase. */
s32 daPicGate_c::func_ov080_02125d08(s32 frames)
{
    int *obj = (int *)mWaveParams;
    Fix12i speed = _ZN4cstd4fdivEii(0xffff, obj[2]);
    long long prod = (long long)frames * (long long)speed;
    int result = (int)((prod + 0x800) >> 12);
    s16 cur = mWavePhase;
    return (s16)(result - cur);
}

/* func_ov080_02125d64 stays a C free function: the Vector3 copy it does is
 * the C front end's ldm/stm block move, and C++ scalarizes it on every
 * installed mwccarm (the ov062 wall). #pragma cplusplus off parses it as C.
 * Do not redeclare its callees. */
#pragma cplusplus off
struct Elem { struct Vector3 pos; int dist; int pad[2]; };  /* 0x18 */

// @symbol func_ov080_02125d64
/* Refresh every cell's planar distance from the wave origin (unk_134);
 * Elem::dist feeds RippleHeight. */
void func_ov080_02125d64(char *c)
{
    int i;
    struct Vec3 tmp;
    for (i = 0; i < *(unsigned short*)(c + 0x1b8); i++) {
        struct Elem *e = (struct Elem *)(*(char **)(c + 0x1a0) + i * 0x18);
        tmp = *(struct Vec3 *)&e->pos;
        tmp.z = 0;
        e->dist = Vec3_Dist((struct Vector3 *)(c + 0x134), (struct Vector3 *)&tmp);
    }
}
#pragma cplusplus on

// @symbol _ZN11daPicGate_c19func_ov080_02125de0Eiii
/* BeginWave: the origin in cell space goes to unk_134, every cell's dist
 * is refreshed, then a WaveParams row is picked for this param1 shape
 * (front/back rows differ) and mWaveTimer reloads from it. */
void daPicGate_c::func_ov080_02125de0(s32 a1, s32 a2, s32 a3)
{
    u32 m8;
    int r0;
    unsigned char f;
    unk_134[0] = a1;
    unk_134[1] = a2;
    unk_134[2] = 0;
    func_ov080_02125d64((char *)this);
    m8 = param1;
    r0 = 0;
    if ((unsigned char)((m8 >> 0xd) & 3) == 1) {
        f = (unsigned char)((m8 >> 8) & 0x1f);
        if (f == 4)
            r0 = 0xc;
        else if (f == 7)
            r0 = 0xd;
    } else {
        switch ((unsigned char)(m8 & 0xf) + 1) {
        case 3: r0 = 0; break;
        case 4: r0 = 1; break;
        case 5: r0 = 2; break;
        case 6: r0 = 3; break;
        case 7: r0 = 4; break;
        case 16: r0 = 5; break;
        }
        if (a3 != 0)
            r0 += 6;
    }
    mWaveParams = (const WaveParams *)(data_ov080_021277a8 + r0 * 0x14);
    mWaveTimer = mWaveParams->duration;
}

// @symbol _ZN11daPicGate_c19func_ov080_02125f00Ev
/* mCorners defaults, state row 0: the picture's four border vertices,
 * scaled by the param1 width/height. */
void daPicGate_c::func_ov080_02125f00()
{
    mCorners[0].x = 0;
    mCorners[0].y = 0;
    mCorners[0].z = 0;
    mCorners[0].color = 0x20000000;
    mCorners[0].texCoord = 0x8000800;
    mCorners[1].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[1].y = 0;
    mCorners[1].z = 0;
    mCorners[1].color = 0x20000000;
    mCorners[1].texCoord = 0x8000000;
    mCorners[2].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[2].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[2].z = 0;
    mCorners[2].color = 0x20000000;
    mCorners[2].texCoord = 0;
    mCorners[3].x = 0;
    mCorners[3].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[3].z = 0;
    mCorners[3].color = 0x20000000;
    mCorners[3].texCoord = 0x800;
}

// @symbol _ZN11daPicGate_c19func_ov080_02125fd0Ev
/* DrawFlat: the flat 2x2 quad for states 2/3. mMtx is composed onto the
 * camera matrix, scaled to 32, then each corner emits TEXCOORD, NORMAL and
 * one VTX_16 pair at >>8. */
void daPicGate_c::func_ov080_02125fd0()
{
    int tmp[13];
    int i;
    int *p;

    G3_MTX_PUSH = 0;
    MulMat4x3Mat4x3(mMtx, (const int *)&data_0209b3ec, tmp);
    G3_MTX_MODE = 2;
    func_020553a4(tmp);
    G3_LIGHT_VECTOR = 0xe0000000;
    G3_LIGHT_COLOR = 0xc0007fff;
    func_ov080_02125460();

    G3_MTX_SCALE = 0x20000;
    G3_MTX_SCALE = 0x20000;
    G3_MTX_SCALE = 0x20000;
    G3_BEGIN = 1;

    i = 0;
    p = (int *)mCorners;
    do {
        int v0, v1, v2;
        s16 s0, s1, s2;
        G3_TEXCOORD = p[5];
        G3_NORMAL = p[4];
        v0 = p[0];
        v1 = p[1];
        v2 = p[2];
        s0 = (s16)(v0 >> 8);
        s1 = (s16)(v1 >> 8);
        s2 = (s16)(v2 >> 8);
        G3_VTX_16 = (u16)s0 | ((u16)s1 << 16);
        G3_VTX_16 = (u16)s2;
        i++;
        p += 6;
    } while (i < 4);

    G3_END = 0;
    G3_MTX_POP = 1;
}

// @symbol _ZN11daPicGate_c19func_ov080_02126120Ev
/* Empty. A no-op row for the state tables that need one. */
void daPicGate_c::func_ov080_02126120()
{
}

// @symbol _ZN11daPicGate_c19func_ov080_02126124Ev
/* mCorners defaults, state row 1: same layout as func_ov080_02125f00 with
 * the texCoord pattern rotated. */
void daPicGate_c::func_ov080_02126124()
{
    mCorners[0].x = 0;
    mCorners[0].y = 0;
    mCorners[0].z = 0;
    mCorners[0].color = 0x20000000;
    mCorners[0].texCoord = 0x8000000;
    mCorners[1].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[1].y = 0;
    mCorners[1].z = 0;
    mCorners[1].color = 0x20000000;
    mCorners[1].texCoord = 0x8000800;
    mCorners[2].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[2].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[2].z = 0;
    mCorners[2].color = 0x20000000;
    mCorners[2].texCoord = 0x800;
    mCorners[3].x = 0;
    mCorners[3].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[3].z = 0;
    mCorners[3].color = 0x20000000;
    mCorners[3].texCoord = 0;
}
