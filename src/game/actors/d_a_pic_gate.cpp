//cpp
/* ov080/daPicGate_c -- picture-gate helpers: the mesh machinery under the
 * promoted TU (src/game/actors/d_a_pic_gate_methods.cpp, .text 0x021264ec..0x02126fbc).
 * .text 0x02125404..0x021264ec: the destructor pair, then the seventeen
 * helpers that fill the range up to the class TU.
 *
 *   ~daPicGate_c         D1 0x02125404, D0 0x02125428 -- the key function, so
 *                        the vtable and typeinfo come out of this file too.
 *   LoadMaterial         texture/palette/light GX ports from the material
 *                        record kept in mTexRecord; states 2/3 (and a
 *                        stopped wave) force ambient white.
 *   BuildGateMatrix      mMtx from position, rotation and the param1 width,
 *                        and mInvMtx as its inverse.
 *   LoadTexture          decompress this picture's texture record to VRAM.
 *                        `this` is unused: it was a method already.
 *   HitTest              map the closest player's current and previous
 *                        position into gate space; inside the mesh begins a
 *                        wave, beyond the far edge begins one and plays a
 *                        sound.
 *   BuildNormals         per-cell normal from the neighbour cross product,
 *                        packed for G3_NORMAL.
 *   FlattenFrame         cells on the border of the current width/height get
 *                        z = 0 and the flat packed normal.
 *   RippleHeight         wave height at a cell's distance.
 *   WaveAmplitude / WavePhase -- wave-clock math on mWaveParams.
 *   UpdateCellDistances  distance of every cell from the wave origin.
 *   BeginWave            store the origin, refresh cell dists, pick the
 *                        WaveParams row, reload mWaveTimer.
 *   PlaceCornersMirrored / PlaceCorners -- mCorners defaults for the flat
 *                        quad states; the texCoord pattern differs.
 *   DrawFlat             emit the flat quad.
 *   UpdateFlat           the empty state slot.
 *   DrawRippleEnvMapped  the environment-mapped wave-mesh renderer: one
 *                        triangle strip per pair of adjacent cell columns.
 *
 * deslop leftovers:
 * - BuildNormals keeps the triple-store zero-init of its Vec3 locals.
 */

#include "daPicGate_c.h"
#include "common.h"
#include "daPly_c.h"
#include "dExtSimpleModel_c.h"
#include "SharedFilePtr.h"
#include "private/Ov080Mat.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/g3_util.h"
#include "nitro/gx/g3imm.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/hw/registers.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx_cp.h"

#pragma defer_codegen off

/* Locals take the type of the call that fills them: the CalcLib Vec3_* helpers
   write Vector3, the NitroSDK VEC_* and MTX_MultVec43 write VecFx32. The SDK
   prototypes (math/CalcLib.h, math/MtxStack.h and the two below) still say
   Vector3 (tangosdev/sm64ds-decomp#5207), hence the casts on the VecFx32
   side. */

extern "C" void VEC_CrossProduct(const Vector3 *a, const Vector3 *b, Vector3 *out);
extern "C" void VEC_Normalize(const Vector3 *v, Vector3 *out);
extern "C" void G3_LoadMtx43(void *m);
void func_02012694(u32 id, const Vector3 *pos);
extern "C" void *func_0201787c(SharedFilePtr *sfp);
u32   func_02045ad8(const void *src, u32 size);
unsigned int func_02045a50(const void *src, unsigned int size);

extern Matrix4x3 data_0209b3ec;
extern char data_ov080_021277a8[];
extern SharedFilePtr *data_ov080_0212775c[];
extern unsigned char data_ov080_02128688[];
extern unsigned char data_ov080_0212869c[];

/* Members in reverse, then ~dActor_c. Written first: with codegen not
 * deferred it comes out as D1, D0, D2 in place, the cartridge's order. */
daPicGate_c::~daPicGate_c()
{
}

void daPicGate_c::LoadMaterial() {
    Ov080Mat *mat = (Ov080Mat *)mTexRecord;
    unsigned char mode = (param1 >> 0xd) & 3;
    u32 difAmb = mat->difAmb;
    if (mode >= 2 || mWaveTimer == 0)
        difAmb |= 0x7fff0000;
    REG_DIF_AMB = difAmb;
    REG_SPE_EMI = ((Ov080Mat *)mTexRecord)->speEmi;
    {
        Ov080Mat *m2 = (Ov080Mat *)mTexRecord;
        G3_TexImageParam((GXTexFmt)((m2->param >> 0x1a) & 7), GX_TEXGEN_NONE,
                         (GXTexSizeS)((m2->param >> 0x14) & 7),
                         (GXTexSizeT)((m2->param >> 0x17) & 7), GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
                         (GXTexPlttColor0)((m2->param >> 0x1d) & 1), m2->texAddr);
    }
    {
        Ov080Mat *m3 = (Ov080Mat *)mTexRecord;
        G3_TexPlttBase(m3->pltAddr, (GXTexFmt)((m3->param >> 0x1a) & 7));
    }
    REG_POLYGON_ATTR = 0x11f0088;
}

/* mMtx = translate(pos) * rotate(mAngle) * translate(-width/2): the gate's
 * mesh sits centred on its position, and mInvMtx maps back into it. */
void daPicGate_c::BuildGateMatrix() {
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68,
        -(((unsigned char)(param1 & 0xf) + 1) * 0x64000) / 2 >> 3, 0, 0);
    mMtx = data_020a0e68;
    MTX_Inverse43(&data_020a0e68, &data_020a0e68);
    mInvMtx = data_020a0e68;
}

/* LoadTexture: one 0x18-byte record per picture id, decompressed to VRAM on
 * first use (the parallel data_ov080_0212869c table is the loaded flag).
 * The record keeps the texture port address, the palette port address and
 * the packed parameter word LoadMaterial pushes. */
u8 *daPicGate_c::LoadTexture(s32 i) {
    int off = 0x18 * i;
    Ov080Mat *e = (Ov080Mat *)&data_ov080_02128688[off];
    unsigned char *fl = &data_ov080_0212869c[off];
    if (data_ov080_0212869c[off] == 0) {
        BMD_Texture *a;
        BMD_Material *b;
        BMD_Palette *c;
        SharedFilePtr *sfp = data_ov080_0212775c[i];
        BMD_File *o = (BMD_File *)func_0201787c(sfp);
        a = o->textures;
        b = o->materials;
        c = o->palettes;
        int sz;
        e->texAddr =
            dExtSimpleModel_c::LoadCompressedTextureToVram(a->data, a->size, a->data + a->size);
        sz = c->size;
        if (sz <= 8)
            e->pltAddr = func_02045ad8(c->data, sz);
        else
            e->pltAddr = func_02045a50(c->data, sz);
        e->difAmb = b->diffuseAmbient;
        e->speEmi = b->specularEmission;
        e->param = a->flags;
        *fl = 1;
        sfp->Release();
    }
    return (u8 *)e;
}

/* HitTest: the closest player's position and previous position transformed
 * into gate space (mInvMtx). A z-straddle means the player crossed the gate
 * plane this frame: inside the cell bounds starts a wave toward the field,
 * beyond the far edge starts one from the other side and clicks. */
void daPicGate_c::HitTest() {
    Vector3 a;
    Vector3 b;
    VecFx32 ob;
    VecFx32 oa;
    daPly_c *r5;
    int x, y;
    int lim;
    u32 raw;
    u8 nib;

    r5 = ClosestPlayer();
    if (r5 == 0)
        return;

    Vec3_Asr(&a, &r5->mPrevPos, 3);
    Vec3_Asr(&b, &r5->mPos, 3);
    data_020a0e68 = mInvMtx;
    MTX_MultVec43(&a, &data_020a0e68, (Vector3 *)&oa);
    MTX_MultVec43(&b, &data_020a0e68, (Vector3 *)&ob);

    if ((oa.z > 0 && ob.z <= 0) || (oa.z < 0 && ob.z >= 0)) {
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
        BeginWave(x, y, 1);
        return;
    }

    if (oa.z <= 0xa000)
        return;
    if (ob.z > 0xa000)
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
    BeginWave(x, y, 0);
    func_02012694(0x7b, (const Vector3 *)(&mCamSpacePosX));
}

/* BuildNormals: each cell's normal is the cross product of its column and
 * row neighbours (edge cells take the one-sided edge), halved in z, then
 * normalized and packed 10 bits per axis into Vertex::normal. */
void daPicGate_c::BuildNormals() {
    VecFx32 ej;
    VecFx32 ei;
    VecFx32 n;
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
                VEC_Subtract((Vector3 *)p, (Vector3 *)(p + 1), (Vector3 *)&ej);
            else if (j == mCols - 1)
                VEC_Subtract((Vector3 *)(p - 1), (Vector3 *)p, (Vector3 *)&ej);
            else
                VEC_Subtract((Vector3 *)(p - 1), (Vector3 *)(p + 1), (Vector3 *)&ej);

            if (i == 0)
                VEC_Subtract((Vector3 *)p, (Vector3 *)(p + mRows), (Vector3 *)&ei);
            else if (i == mRows - 1)
                VEC_Subtract((Vector3 *)(p - mRows), (Vector3 *)p, (Vector3 *)&ei);
            else
                VEC_Subtract((Vector3 *)(p - mRows), (Vector3 *)(p + mRows), (Vector3 *)&ei);

            VEC_CrossProduct((const Vector3 *)&ej, (const Vector3 *)&ei, (Vector3 *)&n);
            n.z >>= 1;
            VEC_Normalize((const Vector3 *)&n, (Vector3 *)&n);
            p->normal = (n.x >> 3 & mask) | ((n.y >> 3 & mask) << 10) | ((n.z >> 3 & mask) << 20);
            p += 1;
        }
    }
}

/* FlattenFrame: every cell on the picture's border (x at 0 or the full
 * width, y at 0 or the full height) is reset to z = 0 and the flat packed
 * normal (0x1ff00000); interior cells are left as they are. */
void daPicGate_c::FlattenFrame() {
    for (int i = 0; i < mNumCells; i++) {
        Vertex *e = mCells + i;
        if (e->x == 0 || e->x == ((unsigned char)(param1 & 0xf) + 1) * 0x64000 || e->y == 0 ||
            e->y == ((unsigned char)((param1 >> 4) & 0xf) + 1) * 0x64000) {
            e->z = 0;
            mCells[i].normal = 0x1ff00000;
        }
    }
}

/* RippleHeight: the wave height at a cell, from the phase the wave clock
 * has reached (WavePhase) scaled down linearly over the wave's
 * run, through FX_SinIdx of the phase. */
s32 daPicGate_c::RippleHeight(s32 r6) {
    unsigned short r4 = mWaveTimer;
    const WaveParams *wave;
    int r5res, r4res, sum;
    Fix12i f;
    fx16 tv;

    if (r4 == 0)
        return 0;

    if (((unsigned char)((param1 >> 13) & 3)) == 0) {
        wave = mWaveParams;
        {
            /* Frames per cycle, then distance per frame: past the
               wavefront the picture is still flat. */
            int a = 0xffff / wave->phaseStep;
            int b = wave->wavelength / a;
            if (r6 > b * (wave->duration - r4))
                return 0;
        }
    }

    r5res = WavePhase(r6);
    r4res = WaveAmplitude(mWaveParams->duration - mWaveTimer);

    f = FX_Div(r4res, mWaveParams->radius);
    sum = r4res + FX_MUL(-f, r6);
    if (sum < 0)
        sum = 0;

    tv = FX_SinIdx((unsigned short)r5res >> 4);
    return FX_MUL(tv, sum);
}

/* The wave's amplitude after n frames: it decays linearly from
 * amplitude to 0 over duration. */
s32 daPicGate_c::WaveAmplitude(s32 n) {
    const WaveParams *p = mWaveParams;
    int r4 = p->amplitude;
    int r1 = p->duration;
    int r = r4 / r1;
    return r4 - r * n;
}

/* The wave's phase at distance dist from the origin: dist * 0xffff /
 * wavelength, 4.12 fixed-point, minus the running phase mWavePhase. */
s32 daPicGate_c::WavePhase(s32 dist) {
    Fix12i speed = FX_Div(0xffff, mWaveParams->wavelength);
    int result = FX_MUL(dist, speed);
    s16 cur = mWavePhase;
    return (s16)(result - cur);
}

/* Refresh every cell's planar distance from the wave origin (mWaveOrigin);
 * Vertex::dist feeds RippleHeight. The plain vector is initialized inside
 * the loop: b56 emits the retail block copy for that form. */
void daPicGate_c::UpdateCellDistances() {
    int i;

    for (i = 0; i < mNumCells; i++) {
        Vertex *e = &mCells[i];
        VecFx32 tmp = *(VecFx32 *)&e->x;
        tmp.z = 0;
        e->dist = Vec3_Dist((Vector3 *)mWaveOrigin, (Vector3 *)&tmp);
    }
}

/* BeginWave: the origin in cell space goes to mWaveOrigin, every cell's dist
 * is refreshed, then a WaveParams row is picked for this param1 shape
 * (front/back rows differ) and mWaveTimer reloads from it. */
void daPicGate_c::BeginWave(s32 a1, s32 a2, s32 a3) {
    u32 m8;
    int r0;
    unsigned char f;
    mWaveOrigin[0] = a1;
    mWaveOrigin[1] = a2;
    mWaveOrigin[2] = 0;
    UpdateCellDistances();
    m8 = param1;
    r0 = 0;
    if ((unsigned char)((m8 >> 0xd) & 3) == 1) {
        f = ((m8 >> 8) & 0x1f);
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

/* Init of state row 3: the picture's four border vertices, scaled by the
 * param1 width/height, with the s texture coordinate mirrored. */
void daPicGate_c::PlaceCornersMirrored() {
    mCorners[0].x = 0;
    mCorners[0].y = 0;
    mCorners[0].z = 0;
    mCorners[0].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[0].texCoord = GX_ST(0x80000, 0x80000);
    mCorners[1].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[1].y = 0;
    mCorners[1].z = 0;
    mCorners[1].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[1].texCoord = GX_ST(0, 0x80000);
    mCorners[2].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[2].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[2].z = 0;
    mCorners[2].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[2].texCoord = GX_ST(0, 0);
    mCorners[3].x = 0;
    mCorners[3].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[3].z = 0;
    mCorners[3].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[3].texCoord = GX_ST(0x80000, 0);
}

/* DrawFlat: the flat 2x2 quad for states 2/3. mMtx is composed onto the
 * camera matrix, scaled to 32, then each corner emits TEXCOORD, NORMAL and
 * one VTX_16 pair at >>8. */
void daPicGate_c::DrawFlat() {
    Matrix4x3 tmp;
    int i;
    Vertex *p;

    G3_PushMtx();
    MTX_Concat43(&mMtx, &data_0209b3ec, &tmp);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(&tmp);
    G3_LightVector(GX_LIGHTID_3, 0, 0, -0x1000);
    G3_LightColor(GX_LIGHTID_3, GX_RGB(31, 31, 31));
    LoadMaterial();

    G3_Scale(0x20000, 0x20000, 0x20000);
    G3_Begin(GX_BEGIN_QUADS);

    for (i = 0, p = mCorners; i < 4; i++, p++) {
        REG_TEXCOORD = p->texCoord;
        REG_NORMAL = p->normal;
        G3_Vtx(p->x >> 8, p->y >> 8, p->z >> 8);
    }

    G3_End();
    G3_PopMtx(1);
}

/* Empty. A no-op row for the state tables that need one. */
void daPicGate_c::UpdateFlat() {}

/* Init of state row 2, and the corners both mesh inits place: the same
 * vertices as PlaceCornersMirrored with s running the other way. */
void daPicGate_c::PlaceCorners() {
    mCorners[0].x = 0;
    mCorners[0].y = 0;
    mCorners[0].z = 0;
    mCorners[0].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[0].texCoord = GX_ST(0, 0x80000);
    mCorners[1].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[1].y = 0;
    mCorners[1].z = 0;
    mCorners[1].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[1].texCoord = GX_ST(0x80000, 0x80000);
    mCorners[2].x = (u8)(param1 & 0xf) * 0x64000 + 0x64000;
    mCorners[2].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[2].z = 0;
    mCorners[2].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[2].texCoord = GX_ST(0x80000, 0);
    mCorners[3].x = 0;
    mCorners[3].y = (u8)((param1 >> 4) & 0xf) * 0x64000 + 0x64000;
    mCorners[3].z = 0;
    mCorners[3].normal = GX_VECFX10(0, 0, -0x1000);
    mCorners[3].texCoord = GX_ST(0, 0);
}

extern "C" void G3_MultMtx43(void *m);
extern int data_ov080_0212771c[];

/* DrawWave. The texture binds with normal-based texture coordinates
 * (GX_TEXGEN_NORMAL) and no repeat or flip, so each vertex sends only
 * NORMAL and its position at >>7. */
void daPicGate_c::DrawRippleEnvMapped() {
    Matrix4x3 tmp;
    int i, j;

    MTX_Concat43(&mMtx, &data_0209b3ec, &tmp);
    G3_PushMtx();
    G3_LoadTexMtxEnv(data_ov080_0212771c);
    G3_MultMtx43(&tmp);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(&tmp);
    G3_TexCoord(0x40000, 0x40000);
    G3_LightVector(GX_LIGHTID_3, 0, 0, -0x1000);
    G3_LightColor(GX_LIGHTID_3, GX_RGB(31, 31, 31));
    {
        struct Ov080Mat *m2 = (struct Ov080Mat *)mTexRecord;
        REG_DIF_AMB = m2->difAmb | 0x7fff0000;
    }
    REG_SPE_EMI = ((struct Ov080Mat *)mTexRecord)->speEmi;
    {
        Ov080Mat *m2 = (Ov080Mat *)mTexRecord;
        G3_TexImageParam((GXTexFmt)((m2->param >> 26) & 7), GX_TEXGEN_NORMAL,
                         (GXTexSizeS)((m2->param >> 20) & 7), (GXTexSizeT)((m2->param >> 23) & 7),
                         GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
                         (GXTexPlttColor0)((m2->param >> 29) & 1), m2->texAddr);
    }
    {
        struct Ov080Mat *m3 = (struct Ov080Mat *)mTexRecord;
        G3_TexPlttBase(m3->pltAddr, (GXTexFmt)((m3->param >> 0x1a) & 7));
    }
    REG_POLYGON_ATTR = 0x01000088 | ((((u8)((param1 >> 8) & 0x1f)) == 7) ? 0x14 : 0x1f) << 16;
    G3_Scale(0x10000, 0x10000, 0x10000);

    for (i = 0; i < mCols - 1; i++) {
        G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
        for (j = 0; j < mRows; j++) {
            Vertex *v1 = &mCells[i + j * mRows];
            Vertex *v2 = &mCells[(i + 1) + j * mRows];

            REG_NORMAL = v1->normal;
            G3_Vtx(v1->x >> 7, v1->y >> 7, v1->z >> 7);
            REG_NORMAL = v2->normal;
            G3_Vtx(v2->x >> 7, v2->y >> 7, v2->z >> 7);
        }
        G3_End();
    }
    G3_PopMtx(1);
}
