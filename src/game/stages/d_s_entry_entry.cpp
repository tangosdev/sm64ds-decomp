//cpp
#include "dScEntry_c.h"
#include "private/ov075_entry_mesh.h"
#include "BMD_File.h"
#include "dExtSimpleModel_c.h"
#include "SharedFilePtr.h"
#include "Memory.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "cstd.h"
#include "nitro/fx/fx.h"

/* ROM order: the ENTRY factory, then EntryMesh (0x0211a948..0x0211b524). */
#pragma defer_codegen off

/* The array runtime passes each element address and ignores lifecycle results.
 * Explicit function-pointer casts mark that runtime ABI boundary. */
extern "C" {
/* local extern: the factory runs the base constructor over fresh operator new
   storage and stores the vptrs itself; C++ has no call spelling for a C2. */
extern void *_ZN7fBase_cC2Ev(void *);
extern void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                           void (*ctor)(void *), void (*dtor)(void *));

struct P2 {
    int a, b;
};
struct P2Copy {
    int words[2];
};
extern char data_0208e4b8[];
extern char _ZTV8dScene_c[];
extern char _ZTV10dScEntry_c[];
extern struct P2 data_02086b58;
/* local extern: __cxa_vec_ctor takes the element constructor and destructor
   by address, and C++ cannot take a constructor's or destructor's address. */
extern dScEntry_c::icon_c *_ZN10dScEntry_c6icon_cD1Ev(dScEntry_c::icon_c *object);
extern dScEntry_c::icon_c *_ZN10dScEntry_c6icon_cC1Ev(dScEntry_c::icon_c *object);
extern OamAnimation *_ZN12OamAnimationD1Ev(OamAnimation *object);
extern OamAnimation *_ZN12OamAnimationC1Ev(OamAnimation *object);

/* Reconstructed source-style name: SM64DS proves dScEntry_c through RTTI,
 * allocation size, vtable identity, and the ENTRY registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov075_0211a854. */
void *dScEntry_c_classInit_ENTRY(void) {
    /* The raw byte pointer preserves the ROM factory's address-expression
       tree; typed member expressions fold five instructions in mwccarm. */
    char *p = (char *)fBase_c::operator new(sizeof(dScEntry_c));
    if (p) {
        struct P2 v;
        _ZN7fBase_cC2Ev(p);
        *(char **)p = data_0208e4b8;
        *(char **)p = _ZTV8dScene_c;
        {
            unsigned char *flags = &((dScEntry_c *)p)->pauseFlags;
            *flags |= 1;
            *flags |= 4;
        }
        *(char **)p = _ZTV10dScEntry_c;
        /* Array-member copies preserve the original C aggregate-copy schedule
           in C++ without changing the two-word value's type or layout. */
        *(struct P2Copy *)&v = *(struct P2Copy *)&data_02086b58;
        *(struct P2Copy *)&((dScEntry_c *)p)->mEnterStep = *(struct P2Copy *)&v;
        *(struct P2Copy *)&((dScEntry_c *)p)->mBehaviorStep = *(struct P2Copy *)&v;
        *(struct P2Copy *)&((dScEntry_c *)p)->mRenderStep = *(struct P2Copy *)&v;
        __cxa_vec_ctor(((dScEntry_c *)p)->mIcons, 9, sizeof(dScEntry_c::icon_c),
                       (void (*)(void *))_ZN10dScEntry_c6icon_cC1Ev,
                       (void (*)(void *))_ZN10dScEntry_c6icon_cD1Ev);
        __cxa_vec_ctor(
            ((dScEntry_c *)p)->mOamAnimations, 4, sizeof(((dScEntry_c *)p)->mOamAnimations[0]),
            (void (*)(void *))_ZN12OamAnimationC1Ev, (void (*)(void *))_ZN12OamAnimationD1Ev);
    }
    return p;
}
}

/* EntryMesh: the entry-menu grid mesh dEntObj_c keeps at +0xe80
 * (include/private/ov075_entry_mesh.h). dEntObj_c::Init calls Init,
 * its Behavior and Render call Behavior and Render, which run the
 * {init, behavior, render} handler row __sinit_ov075_0211c51c builds:
 * InitGrid, UpdateRipple and DrawGrid. */

extern "C" {
/* local extern: no header declares these arm9 vector and matrix helpers;
   they take the Vector3 layout, spelled EntryMeshVec here (see its header).
   SubVec3 is the exception: its definition takes Vector3, so the calls cast. */
void InvMat4x3(Matrix4x3 *, Matrix4x3 *);
void SubVec3(Vector3 *, Vector3 *, Vector3 *);
void CrossVec3(const EntryMeshVec *, const EntryMeshVec *, EntryMeshVec *);
void NormalizeVec3(const EntryMeshVec *, EntryMeshVec *);
void Vec3_Sub(EntryMeshVec *, const EntryMeshVec *, const Vector3 *);
/* local extern: the BMD texture and palette VRAM loaders. */
int func_02045ad8(const void *);
unsigned int func_02045a50(const void *, unsigned int);
/* local extern: the RTL divide, the geometry-engine matrix load and the
   BMD load through a file handle, declared only in decl_common.h, which
   clashes with dScEntry_c.h's vtable spellings. */
int __aeabi_idiv(int, int);
void func_020553a4(void *);
void *func_0201787c(SharedFilePtr *);
/* local extern: the veneer to operator delete (src/engine/heap/Heap.cpp). */
void func_0203cbc0(void *);
}
/* local extern: the camera matrix and the interleaved sine/cosine table,
   declared per consumer (d_ext_model.cpp, d_ent_obj.cpp). */
extern Matrix4x3 data_0209b3ec;
extern short data_02082214[];
/* The mesh's texture file handle (file 0x8418) and its handler row, both
   built by __sinit_ov075_0211c51c. */
extern SharedFilePtr EntryMeshTextureFile;
extern EntryMeshHandler EntryMeshHandlers[3];

#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)
#define reg_G3_TEXPLTT_BASE   (*(volatile u32 *)0x040004ac)
#define reg_G3_DIF_AMB        (*(volatile u32 *)0x040004c0)
#define reg_G3_SPE_EMI        (*(volatile u32 *)0x040004c4)
#define reg_G3_POLYGON_ATTR   (*(volatile u32 *)0x040004a4)

static inline void G3_TexImageParam(u32 addr, u32 texFmt, u32 texGen, u32 sizeS, u32 sizeT,
                                    u32 repeat, u32 flip, u32 pltt0) {
    reg_G3_TEXIMAGE_PARAM = (u32)((addr >> 3) | (texFmt << 26) | (texGen << 30) | (sizeS << 20) |
                                  (sizeT << 23) | (repeat << 16) | (flip << 18) | (pltt0 << 29));
}

#pragma push
/* 6f: keeps the format constant live so the palette shift colours as in ROM. */
#pragma opt_propagation off
// @symbol _ZN9EntryMesh12SendMaterialEv
/* Push the cached material to the geometry engine: diffuse/ambient,
 * specular/emission, the texture image param and the palette base, then a
 * fixed polygon attribute. The palette shift is 3 for GX_TEXFMT_PLTT4 (2)
 * and 4 otherwise; the format test has to complete before the palette
 * address is loaded, so it is its own statement. Same Nitro G3 inline shape
 * as func_ov080_02125460. */
void EntryMesh::SendMaterial() {
    reg_G3_DIF_AMB = mMaterial.difAmb;
    reg_G3_SPE_EMI = mMaterial.speEmi;
    G3_TexImageParam(mMaterial.texAddr, (mMaterial.param >> 0x1a) & 7, 0,
                     (mMaterial.param >> 0x14) & 7, (mMaterial.param >> 0x17) & 7, 0, 0,
                     (mMaterial.param >> 0x1d) & 1);
    {
        u32 fmt = (mMaterial.param >> 0x1a) & 7;
        u32 sub = (fmt == 2) ? 1 : 0;
        reg_G3_TEXPLTT_BASE = mMaterial.pltAddr >> (4 - sub);
    }
    reg_G3_POLYGON_ATTR = 0x11f0088;
}
#pragma pop

// @symbol _ZN9EntryMesh10CalcMatrixEv
/* Build mMtx from mPos (recentred on the 0xfa000 grid middle, in 1/8
 * units) and mAngleY, and keep its inverse in mInvMtx. */
void EntryMesh::CalcMatrix() {
    Matrix4x3_FromTranslation(&data_020a0e68, (mPos.x - 0xfa000) >> 3, (mPos.y - 0xfa000) >> 3,
                              mPos.z >> 3);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(Matrix4x3 *)&mMtx = data_020a0e68;
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    *(Matrix4x3 *)&mInvMtx = data_020a0e68;
}

/* A BMD_Material's two colour words, which BMD_File.h still keeps inside
 * pad_28 (a shared header with hundreds of consumers): SendMaterial pushes
 * the copies to DIF_AMB and SPE_EMI. Names as in EntryMesh::mMaterial. */
struct BMDMaterialColors {
    char pad_00[0x28];
    int difAmb; /* 0x28 */
    int speEmi; /* 0x2c */
};
typedef char BMDMaterialColors_size_must_be_0x30[sizeof(BMDMaterialColors) == 0x30 ? 1 : -1];

// @symbol _ZN9EntryMesh11LoadTextureEP8BMD_File
/* Load the mesh's texture and palette from its BMD file into VRAM and keep
 * the addresses with the first material's colours and the texture's
 * parameter word; 0 if either load failed. */
int EntryMesh::LoadTexture(BMD_File *file) {
    char *d;
    BMD_Texture *tex;
    BMD_Palette *pal;
    int sz;

    tex = file->textures;
    d = (char *)file->materials;
    pal = file->palettes;

    mMaterial.texAddr =
        dExtSimpleModel_c::LoadCompressedTextureToVram(tex->data, tex->size, tex->data + tex->size);
    sz = pal->size;
    if (sz <= 8)
        mMaterial.pltAddr = func_02045ad8(pal->data);
    else
        mMaterial.pltAddr = func_02045a50(pal->data, (unsigned int)sz);
    /* d is the first BMD_Material. */
    mMaterial.difAmb = ((BMDMaterialColors *)d)->difAmb;
    mMaterial.speEmi = ((BMDMaterialColors *)d)->speEmi;
    mMaterial.param = tex->flags;
    if (mMaterial.texAddr == 0 || mMaterial.pltAddr == 0)
        return 0;
    return 1;
}

// @symbol _ZN9EntryMesh7TouchAtEP7Vector3
/* Map a touched world point into grid space and start a ripple there. */
void EntryMesh::TouchAt(Vector3 *touch) {
    EntryMeshVec origin;
    EntryMeshVec r;
    EntryMeshVec out;
    origin.x = 0;
    origin.y = 0x15e000;
    origin.z = -0x320000;
    Vec3_Sub(&out, &origin, touch);
    Vec3_Asr((Vector3 *)&r, (Vector3 *)&out, 3);
    r.x = r.x + 0xfa000;
    r.y = r.y + 0xfa000;
    StartRipple(r.x, r.y);
}

// @symbol _ZN9EntryMesh11CalcNormalsEv
/* Pack each vertex's normal from the cross product of its row and column
 * neighbour differences (one-sided at the grid edges), z halved. */
void EntryMesh::CalcNormals() {
    EntryMeshVec ej;
    EntryMeshVec ei;
    EntryMeshVec n;
    EntryMeshVertex *p;
    int i;
    int j;
    int mask = 0x3ff;

    p = mVertices;

    for (i = 0; i < mGridDim[1]; i++) {
        for (j = 0; j < mGridDim[0]; j++) {
            int *q;
            int z = 0;
            q = (int *)&ej;
            q[0] = z;
            q[1] = z;
            q[2] = z;
            q = (int *)&ei;
            q[0] = z;
            q[1] = z;
            q[2] = z;
            q = (int *)&n;
            q[0] = z;
            q[1] = z;
            q[2] = z;

            if (j == 0)
                SubVec3((Vector3 *)&p->pos, (Vector3 *)&(p + 1)->pos, (Vector3 *)&ej);
            else if (j == mGridDim[0] - 1)
                SubVec3((Vector3 *)&(p - 1)->pos, (Vector3 *)&p->pos, (Vector3 *)&ej);
            else
                SubVec3((Vector3 *)&(p - 1)->pos, (Vector3 *)&(p + 1)->pos, (Vector3 *)&ej);

            if (i == 0)
                SubVec3((Vector3 *)&p->pos, (Vector3 *)&(p + mGridDim[1])->pos, (Vector3 *)&ei);
            else if (i == mGridDim[1] - 1)
                SubVec3((Vector3 *)&(p - mGridDim[1])->pos, (Vector3 *)&p->pos, (Vector3 *)&ei);
            else
                SubVec3((Vector3 *)&(p - mGridDim[1])->pos, (Vector3 *)&(p + mGridDim[1])->pos,
                        (Vector3 *)&ei);

            CrossVec3(&ej, &ei, &n);
            n.z >>= 1;
            NormalizeVec3(&n, &n);
            p->normal = (n.x >> 3 & mask) | ((n.y >> 3 & mask) << 10) | ((n.z >> 3 & mask) << 20);
            p++;
        }
    }
}

// @symbol _ZN9EntryMesh9PinBorderEv
/* Hold the border vertices (x or y at 0 or the 0x1f4000 far edge) flat. */
void EntryMesh::PinBorder() {
    int i;
    for (i = 0; i < mNumVertices; i++) {
        EntryMeshVertex *v = &mVertices[i];
        if (v->pos.x == 0 || v->pos.x == 0x1f4000 || v->pos.y == 0 || v->pos.y == 0x1f4000)
            v->pos.z = 0;
    }
}

// @symbol _ZN9EntryMesh12RippleHeightEi
/* The ripple's height at distance dist from mRippleCenter: a sine of the
 * phase, scaled by an amplitude that decays with the ripple's age and with
 * distance. 0 once the ripple is over or before its front reaches dist. */
int EntryMesh::RippleHeight(int dist) {
    int cnt = mRippleTimer;
    int amp, slope, v, r2, idx, t;
    short phase;
    if (cnt == 0)
        return 0;
    if (dist > (0x64 - cnt) * 0x18444)
        return 0;
    phase = RipplePhase(dist);
    amp = RippleAmplitude(0x64 - mRippleTimer);
    slope = cstd::fdiv(amp, 0x384000);
    v = FX_MUL((-slope), dist);
    r2 = amp + v;
    if (r2 < 0)
        r2 = 0;
    idx = (unsigned short)phase >> 4;
    t = *(short *)((char *)data_02082214 + idx * 4);
    return FX_MUL(t, r2);
}

// @symbol _ZN9EntryMesh15RippleAmplitudeEi
/* The ripple's amplitude after age frames. */
int EntryMesh::RippleAmplitude(int age) {
    return 0x3a000 - age * 0x947;
}

// @symbol _ZN9EntryMesh11RipplePhaseEi
/* The sine phase at distance dist, behind the advancing mRipplePhase. */
short EntryMesh::RipplePhase(int dist) {
    int t = cstd::fdiv(0xffff, 0x16c000);
    int v = FX_MUL(dist, t);
    return (short)(v - mRipplePhase);
}

/* An array-member twin of EntryMeshVec. Copying through it keeps C's
   block-move schedule (ldm/stm); the C++ memberwise copy of the three named
   words is 0x10 bytes longer (the same lever as the factory's P2Copy). */
struct EntryMeshVecCopy {
    s32 words[3];
};

// @symbol _ZN9EntryMesh13CalcDistancesEv
/* Store each vertex's flat (z = 0) distance from mRippleCenter. */
void EntryMesh::CalcDistances() {
    int i;
    EntryMeshVec tmp;
    for (i = 0; i < mNumVertices; i++) {
        EntryMeshVertex *v = &mVertices[i];
        *(EntryMeshVecCopy *)&tmp = *(EntryMeshVecCopy *)&v->pos;
        tmp.z = 0;
        v->dist = Vec3_Dist((Vector3 *)&mRippleCenter, (Vector3 *)&tmp);
    }
}

// @symbol _ZN9EntryMesh11StartRippleEii
/* Start a 100-frame ripple at grid point (x, y). */
void EntryMesh::StartRipple(int x, int y) {
    mRippleCenter.x = x;
    mRippleCenter.y = y;
    mRippleCenter.z = 0;
    CalcDistances();
    mRippleTimer = 100;
}

// @symbol _ZN9EntryMesh8DrawGridEv
/* The render handler. Concatenates mMtx with the camera matrix
 * (data_0209b3ec) and loads the result as both the position and the
 * directional matrix, sets a full-white diffuse/ambient and specular/emission
 * pair, sends the material, then draws the (mGridDim[0] - 1) x mGridDim[1]
 * grid as triangle strips: each inner iteration emits vertex (i, j) and
 * vertex (i + 1, j), converting each 1.19.12 coordinate to VTX_16 with
 * (v << 9) >> 16 and packing x and y into one command word.
 *
 * Two spellings in the vertex emission are load-bearing for the register
 * colouring of the second emission (notes/mwccarm-codegen.md 6ct): the z
 * coordinate is loaded straight into `sz` and shifted in place, and x << 9 is
 * parked in `cols`, the inner loop's bound, which the loop reloads from
 * mGridDim[1] before its next use. Both keep the shift out of the register of
 * the value it consumes, which is where the ROM's compiler puts it. */
void EntryMesh::DrawGrid() {
    int z0;
    int z1;
    int tmp[13];
    int i;
    int j;
    int cols;
    EntryMeshVertex *p0;
    EntryMeshVertex *p1;
    int vx;
    int vy;
    int sx;
    int sz;

    *(int *)0x4000444 = 0;
    MulMat4x3Mat4x3((const int *)&mMtx, (const int *)&data_0209b3ec, tmp);
    *(int *)0x4000440 = 2;
    func_020553a4(tmp);
    *(int *)0x4000440 = 1;
    func_020553a4(tmp);
    *(int *)0x40004c8 = 0xe0000000;
    *(int *)0x40004cc = 0xc0007fff;
    SendMaterial();

    *(int *)0x400046c = 0x10000;
    *(int *)0x400046c = 0x10000;
    *(int *)0x400046c = 0x10000;

    z0 = 0;
    z1 = 0;
    i = 0;
    if ((int)(mGridDim[0] - 1) > 0) {
        do {
            j = z0;
            *(int *)0x4000500 = 2;
            cols = mGridDim[1];
            if (cols > 0) {
                do {
                    p0 = &mVertices[i + j * cols];
                    p1 = &mVertices[i + 1 + j * cols];
                    *(int *)0x4000488 = p0->texCoord;
                    j = j + 1;
                    *(int *)0x4000484 = p0->normal;
                    vx = p0->pos.x;
                    sz = p0->pos.z;
                    cols = vx << 9;
                    sx = cols >> 16;
                    sz = (sz << 9) >> 16;
                    vy = p0->pos.y;
                    *(int *)0x400048c = (u16)sx | ((u16)((vy << 9) >> 16) << 16);
                    *(int *)0x400048c = (u16)sz;
                    *(int *)0x4000488 = p1->texCoord;
                    *(int *)0x4000484 = p1->normal;
                    vx = p1->pos.x;
                    sz = p1->pos.z;
                    cols = vx << 9;
                    sx = cols >> 16;
                    sz = (sz << 9) >> 16;
                    vy = p1->pos.y;
                    *(int *)0x400048c = (u16)sx | ((u16)((vy << 9) >> 16) << 16);
                    *(int *)0x400048c = (u16)sz;
                    cols = mGridDim[1];
                } while (j < cols);
            }
            i = i + 1;
            *(int *)0x4000504 = z1;
        } while (i < (int)(mGridDim[0] - 1));
    }
    *(int *)0x4000448 = 1;
}

// @symbol _ZN9EntryMesh12UpdateRippleEv
/* The behavior handler: while the ripple runs, set every vertex's height,
 * repack the normals, hold the border flat and advance the phase. */
void EntryMesh::UpdateRipple() {
    int i;
    if (DecIfAbove0_Short(&mRippleTimer) == 0)
        return;
    for (i = 0; i < mNumVertices; i++) {
        EntryMeshVertex *v = &mVertices[i];
        v->pos.z = RippleHeight(v->dist);
    }
    CalcNormals();
    PinBorder();
    mRipplePhase = mRipplePhase + 0x1111;
}

// @symbol _ZN9EntryMesh8InitGridEv
/* The init handler: lay the vertices out on a flat 0x1f4000-wide square,
 * the last row and column snapped to the far edge, with texture
 * coordinates across 0..0x80000 (v flipped). */
void EntryMesh::InitGrid() {
    int q[4];
    int acc;
    int j;
    int i;
    int uu;
    int vv;
    int nb;

    acc = 0;
    q[0] = acc;
    j = 0;
    if (((int)mGridDim[1]) <= 0)
        return;

    q[2] = acc;
    q[3] = acc;
    q[1] = acc;

    do {
        i = q[1];
        if (((int)mGridDim[0]) > 0) {
            do {
                EntryMeshVertex *e;
                int b;
                int c;

                e = &mVertices[j * mGridDim[1] + i];
                e->pos.x = acc;
                e->pos.y = q[0];
                e->pos.z = q[2];
                e->normal = 0x1ff00000;

                uu = __aeabi_idiv(0x80000, ((int)mGridDim[0]) - 1);
                vv = __aeabi_idiv(0x80000, ((int)mGridDim[1]) - 1);
                c = ((0x80000 - vv * j) << 8) >> 16;
                b = ((uu * i) << 8) >> 16;
                e->texCoord = (unsigned short)b | ((unsigned short)c << 16);

                nb = (int)mGridDim[0];
                if (i == nb - 2)
                    acc = 0x1f4000;
                else
                    acc += __aeabi_idiv(0x1f4000, nb - 1);
                i++;
            } while (i < nb);
        }

        nb = (int)mGridDim[1];
        acc = q[3];
        if (j == nb - 2)
            q[0] = 0x1f4000;
        else
            q[0] += __aeabi_idiv(0x1f4000, nb - 1);

        j++;
    } while (j < nb);
}

// @symbol _ZN9EntryMesh7DestroyEv
/* Free the vertex array. */
int EntryMesh::Destroy() {
    func_0203cbc0(mVertices);
    return 1;
}

// @symbol _ZN9EntryMesh6RenderEv
/* Run the row's render handler (DrawGrid). */
int EntryMesh::Render() {
    (this->*mState[2])();
    return 1;
}

// @symbol _ZN9EntryMesh8BehaviorEv
/* Run the row's behavior handler (UpdateRipple). */
int EntryMesh::Behavior() {
    (this->*mState[1])();
    return 1;
}

// @symbol _ZN9EntryMesh4InitEPK7Vector3i
/* Allocate a 20 x 20 grid, load its texture from the shared mesh file, run
 * the init handler (InitGrid) and place the mesh at pos, turned by angleY. */
int EntryMesh::Init(const Vector3 *pos, int angleY) {
    mGridDim[0] = 0x14;
    mGridDim[1] = 0x14;
    mNumVertices = mGridDim[0] * mGridDim[1];
    mVertices = (EntryMeshVertex *)Memory::operator_new2(mNumVertices * sizeof(EntryMeshVertex));
    LoadTexture((BMD_File *)func_0201787c(&EntryMeshTextureFile));
    EntryMeshTextureFile.Release();
    mState = EntryMeshHandlers;
    (this->*(*mState))();
    mPos.x = pos->x;
    mPos.y = pos->y;
    mPos.z = pos->z;
    mAngleX = 0;
    mAngleY = angleY;
    mAngleZ = 0;
    CalcMatrix();
    return 1;
}
