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
   They take Vector3. TouchAt's locals are Vector3; the grid data is the
   SDK's POD VecFx32 (private/ov075_entry_mesh.h), so CalcDistances hands
   it over through pointer views. */
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
extern Matrix4x3 gViewMtx;
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
    Matrix4x3_FromTranslation(&gScratchMtx, (mPos.x - 0xfa000) >> 3, (mPos.y - 0xfa000) >> 3,
                              mPos.z >> 3);
    Matrix4x3_ApplyInPlaceToRotationY(&gScratchMtx, mAngleY);
    mMtx = gScratchMtx;
    MTX_Inverse43(&gScratchMtx, &gScratchMtx);
    mInvMtx = gScratchMtx;
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
    Vector3 origin;
    Vector3 r;
    Vector3 out;
    origin.x = 0;
    origin.y = 0x15e000;
    origin.z = -0x320000;
    Vec3_Sub(&out, &origin, touch);
    Vec3_Asr(&r, &out, 3);
    r.x = r.x + 0xfa000;
    r.y = r.y + 0xfa000;
    StartRipple(r.x, r.y);
}

/* Pack each vertex's normal from the cross product of its row and column
 * neighbour differences (one-sided at the grid edges), z halved. */
void EntryMesh::CalcNormals() {
    VecFx32 ej;
    VecFx32 ei;
    VecFx32 n;
    EntryMeshVertex *p;
    int i;
    int j;

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
                VEC_Subtract(&p->pos, &(p + 1)->pos, &ej);
            else if (j == mGridDim[0] - 1)
                VEC_Subtract(&(p - 1)->pos, &p->pos, &ej);
            else
                VEC_Subtract(&(p - 1)->pos, &(p + 1)->pos, &ej);

            if (i == 0)
                VEC_Subtract(&p->pos, &(p + mGridDim[1])->pos, &ei);
            else if (i == mGridDim[1] - 1)
                VEC_Subtract(&(p - mGridDim[1])->pos, &p->pos, &ei);
            else
                VEC_Subtract(&(p - mGridDim[1])->pos, &(p + mGridDim[1])->pos, &ei);

            VEC_CrossProduct(&ej, &ei, &n);
            n.z >>= 1;
            VEC_Normalize(&n, &n);
            p->normal =
                (n.x >> 3 & 0x3ff) | ((n.y >> 3 & 0x3ff) << 10) | ((n.z >> 3 & 0x3ff) << 20);
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
    if (dist > (100 - cnt) * 0x18444)
        return 0;
    phase = RipplePhase(dist);
    amp = RippleAmplitude(100 - mRippleTimer);
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

/* Store each vertex's flat (z = 0) distance from mRippleCenter. flat is
   copy-initialized from the POD VecFx32, which mwcc block-moves (ldm/stm);
   assigning it after the declaration copies the three words one by one.
   Kept (#821 r77): both VecFx32s reach the game's Vec3_Dist through
   `(Vector3 *)` casts. A Vector3 flat built from v->pos grows this function
   0x78 -> 0x88, and mRippleCenter stays VecFx32 for the reason the header
   gives (re-measured #821 r111: a Vector3 mRippleCenter makes dEntObj_c's
   object emit an EntryMesh destructor the ROM does not have). */
void EntryMesh::CalcDistances() {
    int i;
    for (i = 0; i < mNumVertices; i++) {
        EntryMeshVertex *v = &mVertices[i];
        VecFx32 flat = v->pos;
        flat.z = 0;
        v->dist = Vec3_Dist((Vector3 *)&mRippleCenter, (Vector3 *)&flat);
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
 * (gViewMtx) and loads the result as both the directional and the
 * position matrix, shines white light 3 straight down -z, sends the material,
 * then draws the (mGridDim[0] - 1) x mGridDim[1] grid as triangle strips:
 * each inner iteration emits vertex (i, j) and vertex (i + 1, j), its 1.19.12
 * position at >>7. The same draw as daPicGate_c::DrawRippleImage. */
void EntryMesh::DrawGrid() {
    Matrix4x3 mtx;
    int i, j;

    G3_PushMtx();
    MTX_Concat43(&mMtx, &gViewMtx, &mtx);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(&mtx);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_LoadMtx43(&mtx);
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
    mGridDim[0] = 20;
    mGridDim[1] = 20;
    mNumVertices = mGridDim[0] * mGridDim[1];
    mVertices = (EntryMeshVertex *)::operator new[](mNumVertices * sizeof(EntryMeshVertex));
    LoadTexture((BMD_File *)EntryMeshTextureFile.func_0201787c());
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
