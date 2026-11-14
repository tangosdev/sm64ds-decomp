//cpp
#include "dScEntry_c.h"
#include "private/ov075_entry_mesh.h"
#include "BMD_File.h"
#include "dExtSimpleModel_c.h"
#include "SharedFilePtr.h"
#include "Memory.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/g3imm.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"

/* ROM order: the ENTRY factory, then EntryMesh (0x0211a948..0x0211b524). */
#pragma defer_codegen off

void *dScEntry_c_classInit_ENTRY(void) {
    return new dScEntry_c;
}

/* EntryMesh: the entry-menu grid mesh dEntObj_c keeps at +0xe80
 * (include/private/ov075_entry_mesh.h). dEntObj_c::Init calls Init,
 * its Behavior and Render call Behavior and Render, which run the
 * {init, behavior, render} handler row this file's static initializer builds:
 * InitGrid, UpdateRipple and DrawGrid. */

extern "C" {
/* local extern: no header declares these arm9 vector helpers.
   EntryMeshVec is the POD view used for the grid data; APIs taking Vector3
   receive explicit pointer views so the C++ Vector3 destructor is not emitted. */
void VEC_CrossProduct(const Vector3 *, const Vector3 *, Vector3 *);
void VEC_Normalize(const Vector3 *, Vector3 *);
void Vec3_Sub(Vector3 *, const Vector3 *, const Vector3 *);
/* local extern: the BMD texture and palette VRAM loaders. */
/* local extern: the geometry-engine matrix load and the
   BMD load through a file handle, declared only in decl_common.h, which
   clashes with dScEntry_c.h's vtable spellings. */
void G3_LoadMtx43(void *);
void *func_0201787c(SharedFilePtr *);
}
unsigned int func_02045ad8(const void *, unsigned int);
unsigned int func_02045a50(const void *, unsigned int);
/* local extern: the camera matrix, declared per consumer (d_ext_model.cpp,
   d_ent_obj.cpp). */
extern Matrix4x3 data_0209b3ec;
/* File 0x8418. The ctor and dtor are func_020178cc / func_020178b4; this
   TU does not define them. The handler row is filled by the static
   initializer those two objects emit. */
struct EntryMeshTexture : SharedFilePtr {
    EntryMeshTexture(unsigned int fileID);
    ~EntryMeshTexture();
};
extern EntryMeshTexture EntryMeshTextureFile;
extern EntryMeshHandler EntryMeshHandlers[3];

/* Push the cached material to the geometry engine: diffuse/ambient,
 * specular/emission, the texture image param and the palette base, then a
 * fixed polygon attribute, through NitroSDK's G3 immediate inlines. Same
 * shape as daPicGate_c::LoadMaterial. */
void EntryMesh::SendMaterial() {
    REG_DIF_AMB = mMaterial.difAmb;
    REG_SPE_EMI = mMaterial.speEmi;
    G3_TexImageParam((GXTexFmt)((mMaterial.param >> 0x1a) & 7), GX_TEXGEN_NONE,
                     (GXTexSizeS)((mMaterial.param >> 0x14) & 7),
                     (GXTexSizeT)((mMaterial.param >> 0x17) & 7), GX_TEXREPEAT_NONE,
                     GX_TEXFLIP_NONE, (GXTexPlttColor0)((mMaterial.param >> 0x1d) & 1),
                     mMaterial.texAddr);
    G3_TexPlttBase(mMaterial.pltAddr, (GXTexFmt)((mMaterial.param >> 0x1a) & 7));
    REG_POLYGON_ATTR = 0x11f0088;
}

/* Build mMtx from mPos (recentred on the 0xfa000 grid middle, in 1/8
 * units) and mAngleY, and keep its inverse in mInvMtx. */
void EntryMesh::CalcMatrix() {
    Matrix4x3_FromTranslation(&data_020a0e68, (mPos.x - 0xfa000) >> 3, (mPos.y - 0xfa000) >> 3,
                              mPos.z >> 3);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(Matrix4x3 *)&mMtx = data_020a0e68;
    MTX_Inverse43(&data_020a0e68, &data_020a0e68);
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
        mMaterial.pltAddr = func_02045ad8(pal->data, sz);
    else
        mMaterial.pltAddr = func_02045a50(pal->data, sz);
    /* d is the first BMD_Material. */
    mMaterial.difAmb = ((BMDMaterialColors *)d)->difAmb;
    mMaterial.speEmi = ((BMDMaterialColors *)d)->speEmi;
    mMaterial.param = tex->flags;
    if (mMaterial.texAddr == 0 || mMaterial.pltAddr == 0)
        return 0;
    return 1;
}

/* Map a touched world point into grid space and start a ripple there. */
void EntryMesh::TouchAt(Vector3 *touch) {
    EntryMeshVec origin;
    EntryMeshVec r;
    EntryMeshVec out;
    origin.x = 0;
    origin.y = 0x15e000;
    origin.z = -0x320000;
    Vec3_Sub((Vector3 *)&out, (const Vector3 *)&origin, touch);
    Vec3_Asr((Vector3 *)&r, (Vector3 *)&out, 3);
    r.x = r.x + 0xfa000;
    r.y = r.y + 0xfa000;
    StartRipple(r.x, r.y);
}

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
                VEC_Subtract((Vector3 *)&p->pos, (Vector3 *)&(p + 1)->pos, (Vector3 *)&ej);
            else if (j == mGridDim[0] - 1)
                VEC_Subtract((Vector3 *)&(p - 1)->pos, (Vector3 *)&p->pos, (Vector3 *)&ej);
            else
                VEC_Subtract((Vector3 *)&(p - 1)->pos, (Vector3 *)&(p + 1)->pos, (Vector3 *)&ej);

            if (i == 0)
                VEC_Subtract((Vector3 *)&p->pos, (Vector3 *)&(p + mGridDim[1])->pos,
                             (Vector3 *)&ei);
            else if (i == mGridDim[1] - 1)
                VEC_Subtract((Vector3 *)&(p - mGridDim[1])->pos, (Vector3 *)&p->pos,
                             (Vector3 *)&ei);
            else
                VEC_Subtract((Vector3 *)&(p - mGridDim[1])->pos, (Vector3 *)&(p + mGridDim[1])->pos,
                             (Vector3 *)&ei);

            VEC_CrossProduct((const Vector3 *)&ej, (const Vector3 *)&ei, (Vector3 *)&n);
            n.z >>= 1;
            VEC_Normalize((const Vector3 *)&n, (Vector3 *)&n);
            p->normal = (n.x >> 3 & mask) | ((n.y >> 3 & mask) << 10) | ((n.z >> 3 & mask) << 20);
            p++;
        }
    }
}

/* Hold the border vertices (x or y at 0 or the 0x1f4000 far edge) flat. */
void EntryMesh::PinBorder() {
    int i;
    for (i = 0; i < mNumVertices; i++) {
        EntryMeshVertex *v = &mVertices[i];
        if (v->pos.x == 0 || v->pos.x == 0x1f4000 || v->pos.y == 0 || v->pos.y == 0x1f4000)
            v->pos.z = 0;
    }
}

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
    slope = FX_Div(amp, 0x384000);
    v = FX_MUL((-slope), dist);
    r2 = amp + v;
    if (r2 < 0)
        r2 = 0;
    idx = (unsigned short)phase >> 4;
    t = FX_SinCosTable_[idx * 2];
    return FX_MUL(t, r2);
}

/* The ripple's amplitude after age frames. */
int EntryMesh::RippleAmplitude(int age) {
    return 0x3a000 - age * 0x947;
}

/* The sine phase at distance dist, behind the advancing mRipplePhase. */
short EntryMesh::RipplePhase(int dist) {
    int t = FX_Div(0xffff, 0x16c000);
    int v = FX_MUL(dist, t);
    return (v - mRipplePhase);
}

/* An array-member twin of EntryMeshVec. Copying through it keeps C's
   block-move schedule (ldm/stm); the C++ memberwise copy of the three named
   words is 0x10 bytes longer (a named-field copy does not block-move). */
struct EntryMeshVecCopy {
    s32 words[3];
};

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

/* Start a 100-frame ripple at grid point (x, y). */
void EntryMesh::StartRipple(int x, int y) {
    mRippleCenter.x = x;
    mRippleCenter.y = y;
    mRippleCenter.z = 0;
    CalcDistances();
    mRippleTimer = 100;
}

/* The render handler. Concatenates mMtx with the camera matrix
 * (data_0209b3ec) and loads the result as both the directional and the
 * position matrix, shines white light 3 straight down -z, sends the material,
 * then draws the (mGridDim[0] - 1) x mGridDim[1] grid as triangle strips:
 * each inner iteration emits vertex (i, j) and vertex (i + 1, j), its 1.19.12
 * position at >>7. The same draw as daPicGate_c::DrawRippleImage. */
void EntryMesh::DrawGrid() {
    Matrix4x3 tmp;
    int i, j;

    G3_PushMtx();
    MTX_Concat43((const Matrix4x3 *)&mMtx, &data_0209b3ec, &tmp);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(&tmp);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_LoadMtx43(&tmp);
    G3_LightVector(GX_LIGHTID_3, 0, 0, -0x1000);
    G3_LightColor(GX_LIGHTID_3, GX_RGB(31, 31, 31));
    SendMaterial();

    G3_Scale(0x10000, 0x10000, 0x10000);

    for (i = 0; i < mGridDim[0] - 1; i++) {
        G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
        for (j = 0; j < mGridDim[1]; j++) {
            EntryMeshVertex *p0 = &mVertices[i + j * mGridDim[1]];
            EntryMeshVertex *p1 = &mVertices[i + 1 + j * mGridDim[1]];

            REG_TEXCOORD = p0->texCoord;
            REG_NORMAL = p0->normal;
            G3_Vtx(p0->pos.x >> 7, p0->pos.y >> 7, p0->pos.z >> 7);
            REG_TEXCOORD = p1->texCoord;
            REG_NORMAL = p1->normal;
            G3_Vtx(p1->pos.x >> 7, p1->pos.y >> 7, p1->pos.z >> 7);
        }
        G3_End();
    }
    G3_PopMtx(1);
}

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

/* The init handler: lay the vertices out on a flat 0x1f4000-wide square,
 * the last row and column snapped to the far edge, with texture
 * coordinates across 0..0x80000 (v flipped). */
void EntryMesh::InitGrid() {
    int x = 0;
    int y = 0;
    int row, col;

    for (row = 0; row < mGridDim[1]; row++) {
        for (col = 0; col < mGridDim[0]; col++) {
            EntryMeshVertex *v = &mVertices[row * mGridDim[1] + col];
            v->pos.x = x;
            v->pos.y = y;
            v->pos.z = 0;
            v->normal = GX_VECFX10(0, 0, 0xfff);
            int du = 0x80000 / (mGridDim[0] - 1);
            int dv = 0x80000 / (mGridDim[1] - 1);
            int s = du * col;
            int t = 0x80000 - dv * row;
            v->texCoord = GX_ST(s, t);
            if (col == mGridDim[0] - 2)
                x = 0x1f4000;
            else
                x += 0x1f4000 / (mGridDim[0] - 1);
        }
        x = 0;
        if (row == mGridDim[1] - 2)
            y = 0x1f4000;
        else
            y += 0x1f4000 / (mGridDim[1] - 1);
    }
}

/* Free the vertex array. */
int EntryMesh::Destroy() {
    ::operator delete[](mVertices);
    return 1;
}

/* Run the row's render handler (DrawGrid). */
int EntryMesh::Render() {
    (this->*mState[2])();
    return 1;
}

/* Run the row's behavior handler (UpdateRipple). */
int EntryMesh::Behavior() {
    (this->*mState[1])();
    return 1;
}

/* Allocate a 20 x 20 grid, load its texture from the shared mesh file, run
 * the init handler (InitGrid) and place the mesh at pos, turned by angleY. */
int EntryMesh::Init(const Vector3 *pos, int angleY) {
    mGridDim[0] = 0x14;
    mGridDim[1] = 0x14;
    mNumVertices = mGridDim[0] * mGridDim[1];
    mVertices = (EntryMeshVertex *)::operator new[](mNumVertices * sizeof(EntryMeshVertex));
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

EntryMeshTexture EntryMeshTextureFile(0x8418);

extern "C" EntryMeshHandler EntryMeshHandlers[3] = {
    &EntryMesh::InitGrid,
    &EntryMesh::UpdateRipple,
    &EntryMesh::DrawGrid,
};
