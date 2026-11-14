//cpp
/* ModelComponents -- the bone/skeleton half of a loaded model (transforms at
 * +0x0c rebuilt from the BMD bone list), arm9 0x020443c8..0x0204547c,
 * 14 functions: Render, func_02044534/0204488c/02044b30, UpdateVertsUsingBones,
 * UpdateBones, func_020453c0 + 10 helpers.
 * The file takes the class name already used in the tree (include/dExtModel_c.h),
 * per Juan 2026-10-11.
 *
 * SCOPE: func_02044334 above Render stays out (a different object, PURE-C).
 * func_020453c0 is the bone-tree walk UpdateBones forwards to. Next above is
 * func_0204547c.c, a dExtSimpleModel_c leftover, and it stays out.
 *
 * mwccarm emits .text in reverse source order here, so definitions run from
 * the highest address (func_020453c0) down to the lowest (Render).
 * Descending order defines every in-span callee before its callers, so only
 * out-of-span callees need extern declarations.
 *
 * The two struct Node layouts the shards carried (func_02044f74's full
 * f0..f2c record, func_02045074's f0..fc prefix) are one shared definition:
 * the short form is an exact prefix of the long one and func_02045074 only
 * touches f0/f8/fa/fc, so every accessed offset is identical. The
 * func_02044efc Matrix4x3 typedef is dropped for include/dExtModel_c.h's; both
 * bodies use it opaquely through pointers. Five helpers are ModelComponents
 * members: under melonDS, r0 at func_02044534, func_0204488c and
 * func_02044b30 is always the `this` of the Render that called them, and r0
 * at func_02045074 and func_020453c0 is the `this` of UpdateVertsUsingBones,
 * UpdateBones or dExtSimpleModel_c::Virtual10's embedded components (or the
 * walk's own, recursing). The rest stay extern "C" under their existing
 * names: func_02045178 writes a stack matrix, and the others were not
 * observed. func_0204531c's UpdateVertsUsingBones call is spelled as the member
 * it is (dExtModel_c.h); the mangled extern the shard used would be a new
 * local redeclaration of a header member.
 */
/* leftovers:
 * - Render is the member dExtModel_c.h declares; it walks modelFile->bones as
 *   BMD_Bone and materials as BMD_Material (BMD_File.h).
 * - MeshPart/MeshEntry are func_02044534's Part/Entry renamed: func_0204488c's
 *   same-named records differ. Vec3w is the array view for the ROM's ldm-shaped
 *   3-word copy (the daPly_c P2w precedent); the scalar spelling scalarizes.
 * - func_02044534 walks include/private/mtx43.h's Mtx43 r[] rows. The
 *   Matrix4x3 this file sees through dExtModel_c.h is math/Matrix.h's
 *   rotation-plus-translation pair, with no per-axis rows, so the sites that
 *   view a transform, its local copies or a bone builder's int[12] as an
 *   Mtx43 or a Matrix4x3 carry reinterpret casts that emit no code. Its
 *   Vector3 argument is Render's scale (data_02099f88, {1.0, 1.0, 1.0}, when
 *   null), read through a VecFx32 view.
 * - func_02045074 and func_020453c0 take the BMD file bone (BMD_Bone) and
 *   read it through the local Node view (0x40 bytes, BMD_Bone's stride, so
 *   the child/sibling slots fa/fc step it as `n + n->fa`); BMD_Bone does not
 *   name the id and the parent/child/sibling offsets yet.
 * - func_020453c0 passes func_0204547c the bone's BCA_File track record; the
 *   int casts at that call fit the local extern's spelling, which is a banked
 *   declaration disagreement with the Ctx/Src definition.
 * - func_02044b30's halfSizes[] is block-copied from data_02099f94 (the
 *   ROM's +0x284 load relocates against the table, so an init-list's
 *   anonymous .rodata would be a WRONG-DEST), and it reads materials through
 *   the local MaterialState: dExtModel_c.h types the pointer as the file's
 *   BMD_Material, whose layout the runtime record does not share.
 * - Render passes func_02044b30 the material index it already holds in r1,
 *   so the call emits no argument move.
 * - func_02044534's z-prefixed zero temps and the two/three constants keep the
 *   ROM's register schedule; the compact human form does not emit them.
 */
#include "BCA_File.h"
#include "dExtModel_c.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/mtx43.h"
#include "math/MtxStack.h"
#include "nitro/gx/g3imm.h"
#include "nitro/gx/g3imm_cmd.h"

#include "nitro/os/cache.h"
#include "nitro/mi/dma.h"

/* The four-pointer block at BMD_File::skin (UpdateFileOffsets rebases all
   four). */
struct Sub {
    unsigned char *bytes;
    unsigned short *halfs;
    unsigned short *ranges; /* +8: {count, first} pairs, one per BMD_File::numWeightedMatrices
                               matrix (func_02044dc4) */
    Matrix4x3 *blk;
};

struct Node {
    s16 f0; s16 f2; s16 f4; s16 f6;
    s16 f8; s16 fa; s16 fc; s16 fe;
    int f10, f14, f18;
    u16 f1c, f1e, f20; s16 f22;
    int f24, f28, f2c;
    u8 pad_30[0x10];
};


typedef struct Vec3w { int w[3]; } Vec3w;
struct Buf8 { s16 v[8]; };
struct MeshPart {
    int count;
    unsigned char *ids;
    u32 dlSize;
    const void *dl;
};
struct MeshEntry {
    int count;
    struct MeshPart* part;
};
struct Matrix3x3_Vecs { struct Vector3 v0, v1, v2; };

/* The 0x30-byte runtime material ModelComponents::materials points at.
   func_020462d0 (Model4.cpp) builds one from each file BMD_Material, so the
   layout is not BMD_Material's: dExtModel_c.h types the pointer as the file
   record, and func_02046bbc reads the same record as its MaterialState. The
   texture SRT words at 0x08..0x18 are the ones func_020469e8 (BTA) animates. */
struct MaterialState {
    u32 textureId;     /* 0x00 - index into BMD_File::textures */
    u32 paletteId;     /* 0x04 - index into BMD_File::palettes */
    fx32 texScaleS;    /* 0x08 */
    fx32 texScaleT;    /* 0x0c */
    u16 texRotation;   /* 0x10 - 4096 to the turn */
    u16 pad_12;        /* 0x12 */
    fx32 texTransS;    /* 0x14 */
    fx32 texTransT;    /* 0x18 */
    u32 texImageParam; /* 0x1c - the file's word ORed with the texture's flags */
    u32 texPlttBase;   /* 0x20 */
    u32 polygonAttr;   /* 0x24 - bit 31 hides the material (Render) */
    u32 difAmb;        /* 0x28 */
    u32 speEmi;        /* 0x2c */
};

extern "C" {
void MTX_Identity43_(Matrix4x3 *mat);
int MTX_Inverse43(const Matrix4x3 *a, Matrix4x3 *b);

void func_02048234(int *out, int sa, int sb, int sc, u16 ax, u16 ay, u16 az, int tx, int ty,
                   int tz);
void func_02047218(void *a, void *b, void *out, fx32 c);
void func_02047910(int *m0, int *m1, int *dst, int t);
void func_0204547c(int *a, int b, int c, void *elem);
void MTX_Scale43_(Mtx43 *m, int x, int y, int z);
void VEC_Normalize(const Vector3 *a, Vector3 *b);
void func_020458a8(void *src, void *dst);
void G3_MultMtx33(void *m);
extern VecFx32 data_02099f88;
extern u32 data_020a4bd4;
/* Identity, used when Render is given no matrix. */
extern const int data_02082190[12];
extern void *data_020a4bd0;
extern u8 data_020a4bbc;
extern struct Buf8 data_02099f94;
extern s16 data_020a4bc0;
extern s16 data_020a4bc4;
void G3_LoadMtx43(void *m);
void G3_MultMtx43(void *m);
void MTX_Concat33(void *a, void *b, void *dst);
void MTX_Copy33To44_(void *a, void *b);
void G3_LoadTexMtxEnv(int *n);
}

/* Bone-tree walk at 0x020453c0. UpdateBones forwards here. Samples each
 * bone's pose with func_0204547c (which stays out) from the model's
 * animation, or from the bone's own override once one is met, ORs in the
 * parent's scaled flag, and recurses on the node's child and sibling slots. */
void ModelComponents::func_020453c0(BMD_Bone *fileBone, BCA_File *file, int frame) {
    struct Node *node = (struct Node *)fileBone;
    int idx = node->f0;
    ModelBone *bone = &bones[idx];
    BCA_File *anim = file;
    if (bone->anim != 0) {
        anim = bone->anim;
        frame = bone->animFrame;
    }
    func_0204547c((int *)anim, (int)&anim->boneTracks[idx], frame, bone);
    bone->scaled |= bone[node->f8].scaled;
    if (node->fa != 0)
        func_020453c0(fileBone + node->fa, anim, frame);
    if (node->fc != 0)
        func_020453c0(fileBone + node->fc, anim, frame);
}

/* ModelComponents::UpdateBones(BCA_File*, int) at 0x02045394 -- apply a bone
 * animation frame. Forwards to func_020453c0 with this, the BMD bone table
 * (modelFile->bones), the BCA file, and the frame index. */
void ModelComponents::UpdateBones(BCA_File *file, int frame)
{
    func_020453c0(modelFile->bones, file, frame);
}

extern "C" {
void func_0204531c(char *self, int arg)
{
    ModelComponents *mc = (ModelComponents *)self;
    unsigned int i;
    Matrix4x3 *save = mc->transforms;
    Matrix4x3 *blend = mc->blendTransforms;

    mc->transforms = blend;
    mc->UpdateVertsUsingBones();
    mc->transforms = save;

    for (i = 0; i < mc->modelFile->numBones; i++)
        func_02047910(&mc->transforms[i].r.m[0], &blend[i].r.m[0], &mc->transforms[i].r.m[0], arg);
}
}

extern "C" {
void func_02045178(int *out, int ax, int ay, int az, int tx, int ty, int tz) {
    int a0 = FX_SinIdx(ax >> 4);
    int a1 = FX_CosIdx(ax >> 4);
    int b0 = FX_SinIdx(ay >> 4);
    int b1 = FX_CosIdx(ay >> 4);
    int c0 = FX_SinIdx(az >> 4);
    int c1 = FX_CosIdx(az >> 4);
    int p00 = (int)(((s64)a0 * c0) >> 12);
    int p11 = (int)(((s64)a1 * c1) >> 12);
    int p01 = (int)(((s64)a0 * c1) >> 12);
    int p10 = (int)(((s64)a1 * c0) >> 12);

    out[0] = (int)(((s64)b1 * c1) >> 12);
    out[1] = (int)(((s64)b1 * c0) >> 12);
    out[2] = -b0;
    out[3] = FX_MUL(b0, p01) - p10;
    out[4] = p11 + FX_MUL(b0, p00);
    out[5] = (int)(((s64)b1 * a0) >> 12);
    out[6] = p00 + FX_MUL(b0, p11);
    out[7] = FX_MUL(b0, p10) - p01;
    out[8] = (int)(((s64)b1 * a1) >> 12);
    out[9] = tx;
    out[10] = ty;
    out[11] = tz;
}
}

void ModelComponents::func_02045074(BMD_Bone *fileBone) {
    struct Node *n = (struct Node *)fileBone;
    int m[12];
    int a = n->f0;
    ModelBone *bone = &bones[a];
    if (bone->scaled == 0) {
        func_02045178(m, bone->rotX, bone->rotY, bone->rotZ, bone->transX, bone->transY,
                      bone->transZ);
    } else {
        func_02048234(m, bone->scaleX, bone->scaleY, bone->scaleZ, bone->rotX, bone->rotY,
                      bone->rotZ, bone->transX, bone->transY, bone->transZ);
    }
    MTX_Concat43((Matrix4x3 *)m, &transforms[a + n->f8], &transforms[a]);
    if (n->fa != 0)
        func_02045074((BMD_Bone *)(n + n->fa));
    if (n->fc != 0)
        func_02045074((BMD_Bone *)(n + n->fc));
}

/* ModelComponents::UpdateVertsUsingBones() at 0x0204504c -- reset the per-bone
 * transform array to identity, then rebuild vertex positions from the BMD
 * bone list via func_02045074. The bones pointer passed to the helper is
 * modelFile->bones (BMD_File @0x08), not the ModelComponents::bones field. */
void ModelComponents::UpdateVertsUsingBones()
{
    MTX_Identity43_(transforms);
    func_02045074(modelFile->bones);
}

extern "C" {
void func_02044f74(void *g, struct Node *n)
{
    int m[12];
    int a = n->f0;
    func_02048234(m, n->f10, n->f14, n->f18, (u16)((unsigned)n->f1c << 4),
                  (u16)((unsigned)n->f1e << 4), (u16)((unsigned)n->f20 << 4), n->f24, n->f28,
                  n->f2c);
    {
        int *base = (int *)((BMD_File *)g)->skin;
        int t = base[3];
        MTX_Concat43((Matrix4x3 *)m, (const Matrix4x3 *)((a + n->f8) * 0x30 + t),
                     (Matrix4x3 *)((s16)a * 0x30 + t));
    }
    if (n->fa != 0)
        func_02044f74(g, n + n->fa);
    if (n->fc != 0)
        func_02044f74(g, n + n->fc);
}
}

extern "C" {
void func_02044efc(BMD_File *self) {
    if (!self->numWeightedMatrices)
        return;
    Matrix4x3 *matrices = ((struct Sub *)self->skin)->blk;
    MTX_Identity43_(matrices);
    func_02044f74(self, (struct Node *)self->bones);
    for (u32 i = 0; i < self->numBones; i++) {
        Matrix4x3 *mat = &((struct Sub *)self->skin)->blk[i];
        MTX_Inverse43(mat, mat);
    }
    DC_StoreAll();
}
}

extern "C" {
void func_02044e50(ModelComponents *self, int count, int base, int *out) {
    int i;
    BMD_File *in = self->modelFile;
    out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0;
    out[4] = 0; out[5] = 0; out[6] = 0; out[7] = 0;
    out[8] = 0; out[9] = 0; out[10] = 0; out[11] = 0;
    for (i = 0; i < count; i++) {
        struct Sub *sub = (struct Sub *)in->skin;
        int idx = base + i;
        unsigned short lh = sub->halfs[idx];
        unsigned char b = sub->bytes[idx];
        func_02047218(&self->transforms[lh], &sub->blk[lh], out, b << 4);
    }
}
}

extern "C" {
void func_02044dc4(ModelComponents *self) {
    unsigned int i;
    Matrix4x3 *dst;
    BMD_File *tbl;
    tbl = self->modelFile;
    dst = &self->transforms[tbl->numBones];
    for (i = 0; i < tbl->numWeightedMatrices; i++) {
        unsigned short *arr = ((struct Sub *)tbl->skin)->ranges;
        func_02044e50(self, arr[2 * i], arr[2 * i + 1], (int *)dst);
        dst++;
    }
}
}

/* Material bind at 0x02044b30, called by Render before each display list:
 * sends material idx's polygon attributes, colours and texture parameters to
 * the geometry engine, keeps the texture-coordinate generation mode in
 * data_020a4bbc, and sets up that mode's texture transform. */
void ModelComponents::func_02044b30(int idx) {
    const MaterialState &mat = ((const MaterialState *)materials)[idx];

    REG_POLYGON_ATTR = mat.polygonAttr;
    REG_DIF_AMB = mat.difAmb;
    REG_SPE_EMI = mat.speEmi;
    reg_G3_TEXPLTT_BASE = mat.texPlttBase;
    u32 texImageParam = mat.texImageParam;
    reg_G3_TEXIMAGE_PARAM = texImageParam;

    data_020a4bbc = texImageParam >> 30;
    switch (data_020a4bbc) {
    case GX_TEXGEN_TEXCOORD: {
        /* Load the texture SRT as the texture matrix: the rotation is
           corrected for the texture's aspect ratio and the translation is
           scaled by its size. */
        G3_MtxMode(GX_MTXMODE_TEXTURE);

        /* The volatile read keeps textureId's load ahead of the textures
           pointer's. Read plainly -- in the subscript, through a named
           local, beside a named textures/file local, through an inline
           accessor, or at any statement position tried -- the ROM's
           `ldr r5,[r7]; ldr r4,[r4,#0x18]; mla r6,r5,r0,r4` at +0xa8 comes
           out with the two loads and their registers swapped (3 words). */
        u32 textureId = *(volatile const u32 *)&mat.textureId;
        const BMD_Texture &tex = modelFile->textures[textureId];
        s16 sin = FX_SinIdx((u16)(mat.texRotation << 4) >> 4);
        s16 cos = FX_CosIdx((u16)(mat.texRotation << 4) >> 4);
        int sSin = (int)(((s64)mat.texScaleS * sin) >> 12);
        int sCos = (int)(((s64)mat.texScaleS * cos) >> 12);
        int tSin = (int)(((s64)mat.texScaleT * sin) >> 12);
        int tCos = (int)(((s64)mat.texScaleT * cos) >> 12);
        int width = tex.width;
        int height = tex.height;

        Mtx43 mtx;
        mtx.r[0].x = sCos;
        mtx.r[0].y = tSin * -height / width;
        mtx.r[0].z = 0;
        mtx.r[1].x = sSin * width / height;
        mtx.r[1].y = tCos;
        mtx.r[1].z = 0;
        mtx.r[2].x = 0;
        mtx.r[2].y = 0;
        mtx.r[2].z = 0;
        mtx.r[3].x = width * (mat.texScaleS - (sSin + sCos)) * 8 -
                     width * (int)(((s64)mat.texScaleS * mat.texTransS) >> 8);
        mtx.r[3].y = height * (tSin - tCos - mat.texScaleT + 0x2000) * 8 +
                     height * (int)(((s64)mat.texScaleT * mat.texTransT) >> 8);
        mtx.r[3].z = 0;
        G3_LoadMtx43(&mtx);
        break;
    }

    case GX_TEXGEN_NORMAL: {
        /* Half the texture's S and T sizes, {4, 8, ..., 512}: block-copied
           from data_02099f94, since the ROM's load at +0x284 relocates
           against the table itself (an init-list's anonymous .rodata would
           be a WRONG-DEST). */
        struct Buf8 halfSizes;
        halfSizes = data_02099f94;
        int sizeS = (texImageParam >> 20) & 7;
        int sizeT = (texImageParam >> 23) & 7;
        data_020a4bc0 = halfSizes.v[sizeS];
        data_020a4bc4 = -halfSizes.v[sizeT];
        G3_TexCoord(mat.texTransS, mat.texTransT);
        break;
    }
    }
}


struct Part {
    int count;
    unsigned char *cmd;
    u32 dlSize;
    const void *dl;
};
struct Entry {
    int count;
    struct Part* parts;
};

void ModelComponents::func_0204488c(int index, Vector3 *scale) {
    int M[9];
    int N[16];
    int i;
    struct Part* part;
    struct Entry* e;
    int j;
    int fp_index;
    unsigned char* cmd;
    void* partMtx;
    void* lastMat;

    e = &((struct Entry *)modelFile->displayListGroups)[index];
    part = e->parts;

    for (i = 0; i < e->count; i++, part++) {
        cmd = part->cmd;
        for (j = 0; j < part->count; j++, cmd++) {
            unsigned char b = *cmd;
            int check;
            if (b == 0xff)
                continue;

            fp_index = modelFile->transformBoneMap[b];
            partMtx = &transforms[fp_index];

            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
            G3_Identity();
            G3_MultMtx33(data_020a4bd0);

            check = bones[fp_index].scaled;
            if (check) {
                func_020458a8(partMtx, (struct Matrix3x3_Vecs *)M);
                G3_MultMtx33(M);
                lastMat = M;
            } else {
                G3_MultMtx33(partMtx);
                lastMat = partMtx;
            }

            G3_MtxMode(GX_MTXMODE_POSITION);
            G3_LoadMtx43(data_020a4bd0);
            if (scale)
                G3_Scale(scale->x, scale->y, scale->z);
            G3_MultMtx43(partMtx);
            G3_Scale(data_020a4bd4, data_020a4bd4, data_020a4bd4);
            G3_StoreMtx(j);
        }

        if (data_020a4bbc == 2) {
            int sx;
            int sy;
            MTX_Concat33(lastMat, data_020a4bd0, M);
            MTX_Copy33To44_(M, N);
            sx = data_020a4bc0;
            N[0] = M[0] * sx;
            N[4] = M[3] * sx;
            N[8] = M[6] * sx;
            sy = data_020a4bc4;
            N[1] = M[1] * sy;
            N[5] = M[4] * sy;
            N[9] = M[7] * sy;
            G3_LoadTexMtxEnv(N);
            G3_StoreMtx(0);
        }

        MI_SendGXCommand(3, part->dl, part->dlSize);
    }
}

void ModelComponents::func_02044534(int index, Matrix4x3 *mtx, Vector3 *scale) {
    int i;
    struct MeshEntry* e;
    int off;
    VecFx32 dirv;
    Mtx43 mw;
    Mtx43 m;
    int out[9];
    VecFx32 *dir;
    struct MeshPart* part;
    int j;
    Mtx43* mp;
    struct MeshEntry* base;
    int cnt;

    dir = &dirv;
    *(struct Vec3w *)&dirv = *(struct Vec3w *)&data_02099f88;
    if (scale)
        dir = (VecFx32 *)scale;
    i = 0;
    base = (struct MeshEntry *)modelFile->displayListGroups;
    cnt = base[index].count;
    e = base + index;
    if (cnt <= 0)
        return;

    do {
        part = e->part;
        j = 0;
        if (part->count > 0) {
            do {
                unsigned char b = *(volatile unsigned char*)(part->ids + j);
                int t;
                int ix = *(volatile u16 *)&modelFile->transformBoneMap[b];
                mp = (Mtx43 *)*(Matrix4x3 *volatile *)&transforms + ix;
                off = ix * 0x30;
                if (mtx) {
                    MTX_Scale43_(&mw, dir->x, dir->y, dir->z);
                    MTX_Concat43((const Matrix4x3 *)&mw, mtx, (Matrix4x3 *)&mw);
                    MTX_Concat43((const Matrix4x3 *)mp, (const Matrix4x3 *)&mw, (Matrix4x3 *)&mw);
                    mp = &mw;
                }
                m.r[2].y = m.r[2].x = 0;
                m.r[2].z = dir->z;
                m.r[3].x = mp->r[3].x;
                m.r[3].y = mp->r[3].y;
                m.r[3].z = mp->r[3].z;
                if (mtx) {
                    MTX_Concat43((const Matrix4x3 *)((char *)transforms + off), mtx,
                                 (Matrix4x3 *)&mw);
                    mp = &mw;
                }
                if (mp->r[1].x * mp->r[1].x + mp->r[1].y * mp->r[1].y > 0x4000) {
                    m.r[1].x = mp->r[1].x;
                    m.r[1].y = mp->r[1].y;
                    m.r[1].z = 0;
                    VEC_Normalize((const Vector3 *)&m.r[1], (Vector3 *)&m.r[1]);
                    t = FX_MUL(m.r[1].x, dir->x);
                    m.r[1].x = t;
                    {
                        int dy = dir->y;
                        int ry = m.r[1].y;
                        m.r[0].y = -t;
                        t = FX_MUL(ry, dy);
                    }
                    m.r[1].y = t;
                    m.r[0].x = t;
                    m.r[0].z = 0;
                } else {
                    m.r[0].x = mp->r[0].x;
                    m.r[0].y = mp->r[0].y;
                    m.r[0].z = 0;
                    VEC_Normalize((const Vector3 *)&m.r[0], (Vector3 *)&m.r[0]);
                    m.r[0].x = FX_MUL(m.r[0].x, dir->x);
                    t = FX_MUL(m.r[0].y, dir->y);
                    m.r[1].y = m.r[0].x;
                    m.r[1].x = -t;
                    m.r[0].y = t;
                    m.r[1].z = 0;
                }
                func_020458a8(&m, out);
                G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
                G3_Identity();
                G3_MultMtx33(out);
                G3_MtxMode(GX_MTXMODE_POSITION);
                G3_LoadMtx43(&m);
                {
                    fx32 v = 1 << (modelFile->scaleShift + 12);
                    G3_Scale(v, v, v);
                }
                G3_StoreMtx(j);
                j++;
            } while (j < part->count);
        }
        MI_SendGXCommandAsync(3, part->dl, part->dlSize, 0, 0);
        MI_WaitDma(3);
        i = i + 1;
        cnt = e->count;
    } while (i < cnt);
}

void ModelComponents::Render(Matrix4x3 *mat, Vector3 *vec) {
    data_020a4bd4 = 1u << (modelFile->scaleShift + 12);
    if (!mat) {
        data_020a4bd0 = (void *)data_02082190;
    } else {
        data_020a4bd0 = mat;
    }

    u32 i = 0;
    BMD_Bone *node = modelFile->bones;
    if (i < modelFile->numBones) {
        do {
            if (node->flags & 1) {
                for (int j = 0; j < node->numMaterials; j++) {
                    u8 idx = node->materialIds[j];
                    u32 hidden = ((materials[idx].flags >> 31)) & 1;
                    if (!hidden) {
                        func_02044b30(idx);
                        func_02044534(node->displayListIds[j], mat, vec);
                    }
                }
            } else {
                for (int j = 0; j < node->numMaterials; j++) {
                    u8 idx = node->materialIds[j];
                    if (!(materials[idx].flags & 0x80000000)) {
                        func_02044b30(idx);
                        func_0204488c(node->displayListIds[j], vec);
                    }
                }
            }
            i++;
            node++;
        } while (i < modelFile->numBones);
    }
}
