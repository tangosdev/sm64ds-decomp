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
 * bodies use it opaquely through pointers. No helper is promoted: none has
 * ROM method evidence, so all eleven stay extern "C" under their existing
 * names. func_0204531c's UpdateVertsUsingBones call is spelled as the member
 * it is (dExtModel_c.h); the mangled extern the shard used would be a new
 * local redeclaration of a header member.
 */
/* leftovers:
 * - Render is the member dExtModel_c.h declares; info/modules reach modelFile/
 *   materials through casts (the header models the BMD half, and Info/
 *   ModuleEntry are its render-half views at the same offsets). RenderNode is
 *   the shard's Node renamed: the bone-record Node already lives here.
 * - MeshPart/MeshEntry are func_02044534's Part/Entry renamed: func_0204488c's
 *   same-named records differ. Vec3w is the array view for the ROM's ldm-shaped
 *   3-word copy (the Player P2w precedent); the scalar spelling scalarizes.
 * - func_02044534/0204488c keep their local Mtx43/Vec3 spellings (common.h's
 *   Matrix4x3 is a flat m[12] with no r0..r3 rows), so Render's calls and the
 *   three MulMat4x3Mat4x3 sites carry reinterpret casts that emit no code.
 * - func_02044b30's buf[] is block-copied from data_02099f94 (the ROM's
 *   +0x284 load relocates against the table, so an init-list's anonymous
 *   .rodata would be a WRONG-DEST); the void*+int site takes the char* cast
 *   C++ requires.
 * - Render calls func_02044b30 through a one-word function-pointer cast: the
 *   definition takes (obj, idx) while this site passes only the object and the
 *   extra register rides along.
 * - func_02044534's z-prefixed zero temps and the two/three constants keep the
 *   ROM's register schedule; the compact human form does not emit them.
 */
#include "dExtModel_c.h"
#include "types.h"
#include "nitro/fx/fx.h"

namespace CP15 {
void FlushDataCache(void);
}

struct Sub {
    unsigned char *bytes;
    unsigned short *halfs;
    char pad8[4];
    char *blk;
};
struct Self {
    struct Inner *p0;
    char pad[8];
    char *m;
};
struct Inner {
    char pad[0x34];
    struct Sub *sub;
};

typedef struct {
    char pad0[4];
    u32 count;         /* +4 */
    u32 param8;        /* +8 */
    char padC[0x24];
    u32 unk30;         /* +0x30 */
    u32* matData;      /* +0x34 */
} Obj;

struct Node {
    s16 f0; s16 f2; s16 f4; s16 f6;
    s16 f8; s16 fa; s16 fc; s16 fe;
    int f10, f14, f18;
    u16 f1c, f1e, f20; s16 f22;
    int f24, f28, f2c;
};

struct Elem {
    int f0, f4, f8;
    int fc, f10, f14;
    u16 f18, f1a, f1c, f1e;
    int f20, f24, f28, f2c, f30;
};

struct G {
    int f0, f4;
    struct Elem* f8;
    int fc;
};

typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct Mtx43 { Vec3 r0, r1, r2, r3; } Mtx43;
typedef struct Vec3w { int w[3]; } Vec3w;
struct Buf8 { s16 v[8]; };
struct MeshPart {
    int count;
    unsigned char* ids;
    int f8;
    int fc;
};
struct MeshEntry {
    int count;
    struct MeshPart* part;
};
struct Matrix3x3_Vecs { struct Vector3 v0, v1, v2; };
struct RenderNode {
    u8 pad[0x30];
    u16 innerCount;
    u8 pad2[2];
    u8* arrA;
    u8* arrB;
    u16 flags;
    u8 pad3[2];
};
struct RenderInfo {
    u8 field0;
    u8 pad[3];
    u32 count;
    struct RenderNode* nodes;
};
struct RenderModule {
    u8 pad[0x24];
    u32 flags;
    u8 pad2[8];
};

extern "C" {
void Matrix4x3_LoadIdentity(Matrix4x3 *mat);
int InvMat4x3(int *a, int *b);

void func_02048234(int *out, int sa, int sb, int sc, int ax, int ay, int az, int tx, int ty, int tz);
void MulMat4x3Mat4x3(const int *A, const int *B, int *dst);
void func_02047218(void *a, void *b, void *out, fx32 c);
void func_02047910(int *m0, int *m1, int *dst, int t);
void func_0204547c(int *a, int b, int c, void *elem);
extern s16 data_02082214[];
void func_020527e8(Mtx43 *m, int x, int y, int z);
void NormalizeVec3(Vec3 *a, Vec3 *b);
void func_020458a8(void *src, void *dst);
void Geometry_MatrixMultiply3x3(void *m);
void func_0205a358(int a0, int a1, int a2, int a3, int a4);
void func_02059fa8(int ch);
extern Vec3 data_02099f88;
extern u32 data_020a4bd4;
extern int data_02082190;
extern void *data_020a4bd0;
extern u8 data_020a4bbc;
extern u16 data_02099f94[8];
extern s16 data_020a4bc0;
extern s16 data_020a4bc4;
void func_020553a4(void *m);
void func_02055388(void *m);
void MulMat3x3Mat3x3(void *a, void *b, void *dst);
void func_02052514(void *a, void *b);
void func_02055998(int *n);
void func_01ffde98(int a, int b, int c);
void func_02044b30(char *obj, int idx);
void func_02044534(char *self, int index, Mtx43 *mtx, Vec3 *light);
void func_0204488c(char *self, int index, int *color);
}

// @symbol func_020453c0
/* Bone-tree walk at 0x020453c0. UpdateBones forwards here. Recurses on the
 * node's child and sibling slots and calls func_0204547c, which stays out. */
extern "C" {
void func_020453c0(int *self, short *node, int *p2, int p3)
{
    int idx = node[0];
    char *elem = (char*)self[2] + idx * 0x34;
    int f2c = *(int*)(elem + 0x2c);
    int *anim = p2;
    int frame = p3;
    if (f2c != 0) {
        anim = (int*)f2c;
        frame = *(unsigned short*)(elem + 0x30);
    }
    int mla = idx * 0x24 + anim[5];
    unsigned short *dst;
    func_0204547c(anim, mla, frame, elem);
    dst = (unsigned short*)(int)(elem + 0x18);
    *dst = (unsigned short)(*dst | *(unsigned short*)(elem + node[4] * 0x34 + 0x18));
    if (node[5] != 0)
        func_020453c0(self, (short*)((char*)node + (node[5] << 6)), anim, frame);
    if (node[6] != 0)
        func_020453c0(self, (short*)((char*)node + (node[6] << 6)), anim, frame);
}
}

// @symbol _ZN15ModelComponents11UpdateBonesEP8BCA_Filei
/* ModelComponents::UpdateBones(BCA_File*, int) at 0x02045394 -- apply a bone
 * animation frame. Forwards to func_020453c0 with this, the BMD bone table
 * (modelFile->bones), the BCA file, and the frame index. */
void ModelComponents::UpdateBones(BCA_File *file, int frame)
{
    func_020453c0((int *)this, (short *)modelFile->bones, (int *)file, frame);
}

// @symbol func_0204531c
extern "C" {
void func_0204531c(char *self, int arg)
{
    unsigned int i;
    int a;
    int off;
    int save = *(int *)(self + 0xc);
    a = *(int *)(self + 0x10);

    *(int *)(self + 0xc) = a;
    ((ModelComponents *)self)->UpdateVertsUsingBones();
    *(int *)(self + 0xc) = save;

    i = 0;
    if (i >= *(unsigned int *)(*(int *)self + 4))
        return;

    off = i;
    do {
        func_02047910((int *)(*(int *)(self + 0xc) + off), (int *)a, (int *)(*(int *)(self + 0xc) + off), arg);
        i++;
        off += 0x30;
        a += 0x30;
    } while (i < *(unsigned int *)(*(int *)self + 4));
}
}

// @symbol func_02045178
extern "C" {
void func_02045178(int *out, int ax, int ay, int az, int tx, int ty, int tz) {
    s16 *t = data_02082214;
    int ia = ax >> 4;
    int ib = ay >> 4;
    int ic = az >> 4;
    int a0 = t[ia * 2];
    int a1 = t[ia * 2 + 1];
    int b0 = t[ib * 2];
    int b1 = t[ib * 2 + 1];
    int c0 = t[ic * 2];
    int c1 = t[ic * 2 + 1];
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

// @symbol func_02045074
extern "C" {
void func_02045074(struct G *g, struct Node *n)
{
    int m[12];
    int a = n->f0;
    struct Elem *e = (struct Elem *)((char *)g->f8 + a * 0x34);
    if (e->f18 == 0) {
        func_02045178(m,
            e->f1a, e->f1c, e->f1e,
            e->f20, e->f24, e->f28);
    } else {
        func_02048234(m,
            e->fc, e->f10, e->f14,
            e->f1a, e->f1c, e->f1e,
            e->f20, e->f24, e->f28);
    }
    {
        int t = g->fc;
        MulMat4x3Mat4x3(&m[0], (const int *)((a + n->f8) * 0x30 + t), (int *)(a * 0x30 + t));
    }
    if (n->fa != 0) func_02045074(g, (struct Node *)((char *)n + n->fa * 0x40));
    if (n->fc != 0) func_02045074(g, (struct Node *)((char *)n + n->fc * 0x40));
}
}

// @symbol _ZN15ModelComponents21UpdateVertsUsingBonesEv
/* ModelComponents::UpdateVertsUsingBones() at 0x0204504c -- reset the per-bone
 * transform array to identity, then rebuild vertex positions from the BMD
 * bone list via func_02045074. The bones pointer passed to the helper is
 * modelFile->bones (BMD_File @0x08), not the ModelComponents::bones field. */
void ModelComponents::UpdateVertsUsingBones()
{
    Matrix4x3_LoadIdentity(transforms);
    func_02045074((struct G *)this, (struct Node *)modelFile->bones);
}

// @symbol func_02044f74
extern "C" {
void func_02044f74(void *g, struct Node *n)
{
    int m[12];
    int a = n->f0;
    func_02048234(m,
        n->f10, n->f14, n->f18,
        ((unsigned)n->f1c << 20) >> 16,
        ((unsigned)n->f1e << 20) >> 16,
        ((unsigned)n->f20 << 20) >> 16,
        n->f24, n->f28, n->f2c);
    {
        int *base = *(int **)((char *)g + 0x34);
        int t = base[3];
        MulMat4x3Mat4x3(&m[0], (const int *)((a + n->f8) * 0x30 + t), (int *)((s16)a * 0x30 + t));
    }
    if (n->fa != 0) func_02044f74(g, (struct Node *)((char *)n + n->fa * 0x40));
    if (n->fc != 0) func_02044f74(g, (struct Node *)((char *)n + n->fc * 0x40));
}
}

// @symbol func_02044efc
extern "C" {
void func_02044efc(Obj *self) {
    if (!self->unk30) return;
    Matrix4x3 *matrices = (Matrix4x3 *)self->matData[3];
    Matrix4x3_LoadIdentity(matrices);
    func_02044f74((void *)self, (struct Node *)self->param8);
    u32 count = self->count;
    u32 i = 0;
    if (i < count) {
        u32 byteOff = 0;
        do {
            Matrix4x3 *mat = (Matrix4x3 *)((char *)((Matrix4x3 *)self->matData[3]) + byteOff);
            InvMat4x3((int *)mat, (int *)mat);
            count = self->count;
            i++;
            byteOff += 0x30;
        } while (i < count);
    }
    CP15::FlushDataCache();
}
}

// @symbol func_02044e50
extern "C" {
void func_02044e50(struct Self *self, int count, int base, int *out)
{
    int i;
    struct Inner *in = self->p0;
    out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0;
    out[4] = 0; out[5] = 0; out[6] = 0; out[7] = 0;
    out[8] = 0; out[9] = 0; out[10] = 0; out[11] = 0;
    for (i = 0; i < count; i++) {
        struct Sub *sub = in->sub;
        int idx = base + i;
        unsigned short lh = sub->halfs[idx];
        unsigned char b = sub->bytes[idx];
        func_02047218(self->m + lh * 0x30, sub->blk + lh * 0x30, out, b << 4);
    }
}
}

// @symbol func_02044dc4
extern "C" {
void func_02044dc4(struct Self *self) {
    unsigned int i;
    int dst;
    char *tbl;
    tbl = *(char **)self;
    dst = *(int *)(tbl + 4) * 0x30 + *(int *)((char *)self + 0xc);
    for (i = 0; i < *(unsigned int *)(tbl + 0x30); i++) {
        unsigned short *arr = *(unsigned short **)(*(char **)(tbl + 0x34) + 8);
        func_02044e50(self, arr[2 * i], arr[2 * i + 1], (int *)dst);
        dst += 0x30;
    }
}
}

// @symbol func_02044b30
extern "C" {
void func_02044b30(char *obj, int idx)
{
  char *m = (*((char **) (obj + 4))) + (idx * 0x30);
  int mtx3;
  int texParam;
  *((int *) 0x40004a4) = *((int *) (m + 0x24));
  *((int *) 0x40004c0) = *((int *) (m + 0x28));
  *((int *) 0x40004c4) = *((int *) (m + 0x2c));
  *((int *) 0x40004ac) = *((int *) (m + 0x20));
  texParam = *((int *) (m + 0x1c));
  *((int *) 0x40004a8) = texParam;
  data_020a4bbc = (u8) (((u32) texParam) >> 0x1e);
  switch (data_020a4bbc)
  {
    case 1:
    {
      int mtx[12];
      int A;
      int B;
      int C;
      int D;
      u16 elemFieldE;
      u16 elemFieldC;
      char *elem;
      int m0;
      u16 rawAngle;
      int angle;
      int k;
      s16 sinVal;
      s16 cosVal;
      int D0;
      int D1;
      *((int *) 0x4000440) = 3;
      rawAngle = *((u16 *) (m + 0x10));
      m0 = *((volatile int *) m);
      elem = (*((char **) ((char *)(*((void **) obj)) + 0x18))) + (m0 * 0x14);
      angle = ((u16) (rawAngle << 4)) >> 4;
      k = angle << 1;
      sinVal = data_02082214[k];
      cosVal = data_02082214[k + 1];
      C = (int) ((((long long) (*((int *) (m + 0x8)))) * sinVal) >> 12);
      B = (int) ((((long long) (*((int *) (m + 0x8)))) * cosVal) >> 12);
      A = (int) ((((long long) (*((int *) (m + 0xc)))) * sinVal) >> 12);
      D = (int) ((((long long) (*((int *) (m + 0xc)))) * cosVal) >> 12);
      elemFieldE = *((u16 *) (elem + 0xe));
      elemFieldC = *((u16 *) (elem + 0xc));
      D0 = (A * (-((int) elemFieldE))) / elemFieldC;
      mtx3 = (C * elemFieldC) / elemFieldE;
      mtx[0] = B;
      mtx[1] = D0;
      mtx[2] = 0;
      D1 = mtx3;
      mtx[3] = D1;
      mtx[4] = D;
      mtx[5] = 0;
      mtx[6] = 0;
      mtx[7] = 0;
      mtx[8] = 0;
      mtx[9] = ((elemFieldC * ((*((int *) (m + 0x8))) - (C + B))) * 8) - (elemFieldC * ((int) ((((long long) (*((int *) (m + 0x8)))) * (*((int *) (m + 0x14)))) >> 8)));
      mtx[10] = ((elemFieldE * (((A - D) - (*((int *) (m + 0xc)))) + 0x2000)) * 8) + (elemFieldE * ((int) ((((long long) (*((int *) (m + 0xc)))) * (*((int *) (m + 0x18)))) >> 8)));
      mtx[11] = 0;
      func_020553a4(mtx);
    }
      break;

    case 2:
    {
      /* The texture-size template {4,8,16,32,64,128,256,512} (4 << i) is
         block-copied from data_02099f94: the ROM's load at +0x284 relocates
         against the table itself, so an init-list (anonymous .rodata) would be
         a WRONG-DEST. The struct assignment keeps the ROM's block-copy shape. */
      s16 buf[8];
      *(struct Buf8 *)buf = *(const struct Buf8 *)data_02099f94;
      int idx1;
      int idx2;
      s16 sa;
      s16 sb;
      idx1 = (((u32) texParam) >> 0x17) & 7;
      idx2 = (((u32) texParam) >> 0x14) & 7;
      data_020a4bc0 = ((s16 *) buf)[idx2];
      data_020a4bc4 = (s16) (-((s16 *) buf)[idx1]);
      sa = (s16) ((*((int *) (m + 0x14))) >> 8);
      sb = (s16) ((*((int *) (m + 0x18))) >> 8);
      *((int *) 0x4000488) = ((u16) sa) | (((u16) sb) << 16);
    }
      break;

  }

}
}

// @symbol func_0204488c

struct Part {
    int count;
    unsigned char* cmd;
    int f8;
    int fc;
};
struct Entry {
    int count;
    struct Part* parts;
};

extern "C" {
void func_0204488c(char* self, int index, int* color)
{
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
    volatile int* mtxScale;

    e = &(*(struct Entry**)(*(char**)self + 0x10))[index];
    part = e->parts;

    mtxScale = (volatile int*)0x400046c;

    for (i = 0; i < e->count; i++, part++) {
        cmd = part->cmd;
        for (j = 0; j < part->count; j++, cmd++) {
            unsigned char b = *cmd;
            int check;
            if (b == 0xff)
                continue;

            fp_index = (*(unsigned short**)(*(char**)self + 0x2c))[b];
            partMtx = (char*)*(void**)(self + 0xc) + fp_index * 0x30;

            *(volatile int*)0x4000440 = 2;
            *(volatile int*)0x4000454 = 0;
            Geometry_MatrixMultiply3x3(data_020a4bd0);

            check = *(unsigned short*)((char*)*(void**)(self + 8) + fp_index * 0x34 + 0x18);
            if (check) {
                func_020458a8((struct Matrix3x3_Vecs*)partMtx, (struct Matrix3x3_Vecs*)M);
                Geometry_MatrixMultiply3x3(M);
                lastMat = M;
            } else {
                Geometry_MatrixMultiply3x3(partMtx);
                lastMat = partMtx;
            }

            *(volatile int*)0x4000440 = 1;
            func_020553a4(data_020a4bd0);
            if (color) {
                int c2 = color[2];
                int c1 = color[1];
                int c0 = color[0];
                *mtxScale = c0;
                *mtxScale = c1;
                *mtxScale = c2;
            }
            func_02055388(partMtx);
            {
                int white = data_020a4bd4;
                *mtxScale = white;
                *mtxScale = white;
                *mtxScale = white;
            }
            *(volatile int*)0x400044c = j;
        }

        if (data_020a4bbc == 2) {
            int sx;
            int sy;
            MulMat3x3Mat3x3(lastMat, data_020a4bd0, M);
            func_02052514(M, N);
            sx = data_020a4bc0;
            N[0] = M[0] * sx;
            N[4] = M[3] * sx;
            N[8] = M[6] * sx;
            sy = data_020a4bc4;
            N[1] = M[1] * sy;
            N[5] = M[4] * sy;
            N[9] = M[7] * sy;
            func_02055998(N);
            *(volatile int*)0x400044c = 0;
        }

        func_01ffde98(3, part->fc, part->f8);
    }
}
}


// @symbol func_02044534
extern "C" {
void func_02044534(char* self, int index, Mtx43* mtx, Vec3* light)
{
    int i;
    struct MeshEntry* e;
    int off;
    int j0;
    int z14;
    int z18;
    int z1c;
    int z20;
    int z24;
    int two;
    int z2c;
    int z30;
    int three;
    Vec3 dirv;
    Mtx43 mw;
    Mtx43 m;
    int out[9];
    Vec3* dir;
    struct MeshPart* part;
    int j;
    Mtx43* mp;
    struct MeshEntry* base;
    int cnt;
    int t0;
    char* obj;

    dir = &dirv;
    *(struct Vec3w *)&dirv = *(struct Vec3w *)&data_02099f88;
    if (light)
        dir = light;
    obj = *(char**)self;
    i = 0;
    base = *(struct MeshEntry **)(obj + 0x10);
    cnt = base[index].count;
    e = base + index;
    if (cnt <= 0)
        return;

    t0 = i;
    z14 = t0;
    z20 = t0;
    z24 = t0;
    z18 = t0;
    z1c = t0;
    two = 2;
    t0 = i;
    z2c = t0;
    z30 = t0;
    three = 3;
    j0 = i;

    do {
        part = e->part;
        j = j0;
        if (part->count > 0) {
            do {
                unsigned char b = *(volatile unsigned char*)(part->ids + j);
                int t;
                int ix = *(volatile unsigned short*)((*(unsigned short**)(*(char**)self + 0x2c)) + b);
                mp = *(Mtx43* volatile*)(self + 0xc) + ix;
                off = ix * 0x30;
                if (mtx) {
                    func_020527e8(&mw, dir->x, dir->y, dir->z);
                    MulMat4x3Mat4x3((const int *)&mw, (const int *)mtx, (int *)&mw);
                    MulMat4x3Mat4x3((const int *)mp, (const int *)&mw, (int *)&mw);
                    mp = &mw;
                }
                m.r2.y = m.r2.x = z14;
                m.r2.z = dir->z;
                m.r3.x = mp->r3.x;
                m.r3.y = mp->r3.y;
                m.r3.z = mp->r3.z;
                if (mtx) {
                    MulMat4x3Mat4x3((const int *)((*(char **)(self + 0xc)) + off), (const int *)mtx, (int *)&mw);
                    mp = &mw;
                }
                if (mp->r1.x * mp->r1.x + mp->r1.y * mp->r1.y > 0x4000) {
                    m.r1.x = mp->r1.x;
                    m.r1.y = mp->r1.y;
                    m.r1.z = z18;
                    NormalizeVec3(&m.r1, &m.r1);
                    t = FX_MUL(m.r1.x, dir->x);
                    m.r1.x = t;
                    { int dy = dir->y; int ry = m.r1.y;
                    m.r0.y = -t;
                    t = FX_MUL(ry, dy);
                    }
                    { int zz = z1c;
                    m.r1.y = t;
                    m.r0.x = t;
                    m.r0.z = zz; }
                } else {
                    m.r0.x = mp->r0.x;
                    m.r0.y = mp->r0.y;
                    m.r0.z = z20;
                    NormalizeVec3(&m.r0, &m.r0);
                    m.r0.x = FX_MUL(m.r0.x, dir->x);
                    t = FX_MUL(m.r0.y, dir->y);
                    m.r1.y = m.r0.x;
                    m.r1.x = -t;
                    { int zz = z24;
                    m.r0.y = t;
                    m.r1.z = zz; }
                }
                func_020458a8(&m, out);
                *(int*)0x4000440 = two;
                *(int*)0x4000454 = z2c;
                Geometry_MatrixMultiply3x3(out);
                *(int*)0x4000440 = 1;
                func_020553a4(&m);
                {
                    int v = 1 << (*(unsigned char*)*(char**)self + 12);
                    *(int*)0x400046c = v;
                    *(int*)0x400046c = v;
                    *(int*)0x400046c = v;
                }
                *(int*)0x400044c = j;
                j++;
            } while (j < part->count);
        }
        { int a5 = z30; int a4 = z30;
        func_0205a358(three, part->fc, part->f8, a4, a5); }
        func_02059fa8(three);
        i = i + 1;
        cnt = e->count;
    } while (i < cnt);
}
}

// @symbol _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3
void ModelComponents::Render(Matrix4x3 *mat, Vector3 *vec) {
    data_020a4bd4 = 1u << (((RenderInfo *)modelFile)->field0 + 12);
    if (!mat) {
        data_020a4bd0 = &data_02082190;
    } else {
        data_020a4bd0 = mat;
    }

    u32 i = 0;
    RenderNode* node = ((RenderInfo *)modelFile)->nodes;
    if (i < ((RenderInfo *)modelFile)->count) {
        do {
            if (node->flags & 1) {
                for (int j = 0; j < node->innerCount; j++) {
                    u8 idx = node->arrA[j];
                    u32 hidden = ((((RenderModule *)materials)[idx].flags >> 31)) & 1;
                    if (!hidden) {
                        ((void (*)(void *))func_02044b30)(this);
                        func_02044534((char *)this, node->arrB[j], (Mtx43 *)mat, (Vec3 *)vec);
                    }
                }
            } else {
                for (int j = 0; j < node->innerCount; j++) {
                    u8 idx = *(volatile u8*)&node->arrA[j];
                    RenderModule* m = (RenderModule *)materials;
                    m += idx;
                    if (!(m->flags & 0x80000000)) {
                        ((void (*)(void *))func_02044b30)(this);
                        func_0204488c((char *)this, node->arrB[j], (int *)vec);
                    }
                }
            }
            i++;
            node++;
        } while (i < ((RenderInfo *)modelFile)->count);
    }
}
