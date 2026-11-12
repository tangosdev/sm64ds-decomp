//cpp
/* Model4 -- VRAM upload for a BMD model, 0x020458e0..0x020468a0.
 *
 * Not part of src/engine/model/d_ext_model.cpp: func_020458a8 sits between that TU
 * and this run. dExtAnmTexPat_c::UpdateFileOffsets begins at 0x020468a0.
 * The name is the tree's dExtSimpleModel_c class plus the numeric suffix already used
 * for a second translation unit (Juan 2026-10-11).
 *
 * mwccarm's default deferred codegen emits .text in reverse source order, so
 * the definitions are highest address first. Do not reorder.
 */

#include "dExtSimpleModel_c.h"
#include "nitro/gx/g3x.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "msl/string.h"
#include "GX.h"
#include "nitro/os/tcm.h"
#include "nitro/os/system.h"

extern "C" {
extern u32 data_020a4bc8;
extern u32 data_020a4bcc;
extern u32 data_020a4bd8;
extern u32 data_020a4bdc;
extern u32 data_020a4be0;
extern u32 data_020a4be4;
extern u32 data_020a4be8;
}

#define REBASE(p) ((char *)(p) + base)
void dExtSimpleModel_c::UpdateFileOffsets(BMD_File &file) {
    u32 i;
    int k;
#ifndef SM64DS_HOST_64
    int base = (int)&file;
#else
    uintptr_t base = (uintptr_t)&file;
    if (base > 0xffffffffu)
        OS_Terminate();
#endif

    if (file.bones) file.bones = (BMD_Bone *)REBASE(file.bones);
    for (i = 0; i < file.numBones; i++) {
        BMD_Bone *b = file.bones + i;
        if (b->unk_04) b->unk_04 = REBASE(b->unk_04);
        if (b->materialIds) b->materialIds = (u8 *)REBASE(b->materialIds);
        if (b->displayListIds)
            b->displayListIds = (u8 *)REBASE(b->displayListIds);
    }

    if (file.displayListGroups)
        file.displayListGroups = (BMD_DisplayListGroup *)REBASE(file.displayListGroups);
    for (i = 0; i < file.numDisplayListGroups; i++) {
        BMD_DisplayListGroup *g = file.displayListGroups + i;
        if (g->lists) g->lists = (BMD_DisplayList *)(DS_IADDR(g->lists) + base);
        for (k = 0; k < (int)g->numLists; k++) {
            BMD_DisplayList *d = g->lists + k;
            if (d->unk_04) d->unk_04 = (void *)(DS_IADDR(d->unk_04) + base);
            if (d->data) d->data = (void *)(DS_IADDR(d->data) + base);
        }
    }

    if (file.textures) file.textures = (BMD_Texture *)REBASE(file.textures);
    for (i = 0; i < file.numTextures; i++) {
        BMD_Texture *e = file.textures + i;
        if (e->name) e->name = REBASE(e->name);
        if (e->data) e->data = REBASE(e->data);
    }

    if (file.palettes) file.palettes = (BMD_Palette *)REBASE(file.palettes);
    for (i = 0; i < file.numPalettes; i++) {
        BMD_Palette *e = file.palettes + i;
        if (e->name) e->name = REBASE(e->name);
        if (e->data) e->data = REBASE(e->data);
    }

    if (file.materials) file.materials = (BMD_Material *)REBASE(file.materials);
    for (i = 0; i < file.numMaterials; i++) {
        BMD_Material *e = file.materials + i;
        if (e->name) e->name = REBASE(e->name);
    }

    if (file.transformBoneMap)
        file.transformBoneMap = (u16 *)REBASE(file.transformBoneMap);
    if (file.numWeightedMatrices == 0)
        return;
    if (file.skin)
        file.skin = REBASE(file.skin);
    {
#ifndef SM64DS_HOST_64
        void **e = (void **)file.skin;
        if (e[0]) e[0] = REBASE(e[0]);
        if (e[1]) e[1] = REBASE(e[1]);
        if (e[2]) e[2] = REBASE(e[2]);
        if (e[3]) e[3] = REBASE(e[3]);
#else
        void * DS_P32 *e = (void * DS_P32 *)file.skin;
        if (e[0]) e[0] = REBASE(e[0]);
        if (e[1]) e[1] = REBASE(e[1]);
        if (e[2]) e[2] = REBASE(e[2]);
        if (e[3]) e[3] = REBASE(e[3]);
#endif
    }
}
#undef REBASE

extern "C" int func_02046590(const void *tbl, const char *name)
{
    const BMD_File *t = (const BMD_File *)tbl;
    u32 count = t->numMaterials;
    u32 i = 0;
    if (i < count) {
        BMD_Material *entry = t->materials;
        while (1)
        {
            if (strcmp(entry->name, name) == 0)
                return (int)i;
            i++;
            entry++;
            if (i >= t->numMaterials)
                break;
        }
    }
    OS_Terminate();
    return -1;
}

extern "C" int func_02046564(void *p)
{
    BMD_File *s = (BMD_File *)p;
    int a = s->numBones;
    int b = s->numWeightedMatrices;
    int c = s->numMaterials;
    int t = (int)&((Matrix4x3 *)0)[a + b];
    t += c * 0x30;
    t += a * 0x34;
    return t;
}

extern "C" {
typedef struct SrcA {
    int f0, f4, f8, fc, f10;
    u16 f14, f16;
    int f18, f1c, f20, f24, f28, f2c;
} SrcA;

typedef struct ElemA {
    int f0, f4, f8, fc;
    u16 f10, f12;
    int f14, f18, f1c, f20, f24, f28, f2c;
} ElemA;

typedef struct SrcB {
    int f0, f4;
    s16 f8, fa;
    int fc, f10, f14, f18;
    u16 f1c, f1e, f20, f22;
    int f24, f28, f2c, f30, f34, f38, f3c;
} SrcB;

typedef struct ElemB {
    int f0, f4, f8, fc, f10, f14;
    u16 f18, f1a, f1c, f1e;
    int f20, f24, f28, f2c;
    u16 f30, f32;
} ElemB;

typedef struct T14 {
    int f0, f4, f8, fc, f10;
} T14;

typedef struct T16 {
    int f0, f4, f8, fc;
} T16;

typedef struct Desc {
    int f0;
    u32 f4;
    SrcB *f8;
    int fc, f10, f14;
    T14 *f18;
    int f1c;
    T16 *f20;
    u32 f24;
    SrcA *f28;
    int f2c;
    int f30;
} Desc;

typedef struct Dst {
    Desc *f0;
    ElemA *f4;
    ElemB *f8;
    ElemA *fc;
    int f10;
} Dst;

void func_020462d0(Dst *d, Desc *h, ElemA *arr)
{
    u32 i;
    ElemA *p;
    SrcB *s2;
    ElemB *e2;

    p = arr;
    d->f0 = h;
    d->f10 = 0;
    d->fc = p;
    p += h->f4 + h->f30;
    d->f4 = p;
    p += h->f24;
    for (i = 0; i < h->f24; i++) {
        SrcA *s;
        ElemA *e;
        u32 shift;
        int idx2;
        int c1;
        s = &h->f28[i];
        e = &d->f4[i];
        e->f0 = s->f4;
        e->f4 = s->f8;
        c1 = h->f28[i].f4;
        e->f1c = s->f20;
        if (c1 != -1) {
            u32 v = h->f18[c1].f10;
            shift = (v >> 0x1a) & 7;
            e->f1c |= v;
        }
        idx2 = h->f28[i].f8;
        if (idx2 != -1) {
            u32 v2;
            if (h->f20 == 0)
                OS_Terminate();
            v2 = h->f20[idx2].fc;
            if (shift == 2)
                e->f20 = v2 >> 3;
            else
                e->f20 = v2 >> 4;
        }
        e->f24 = s->f24;
        e->f28 = s->f28;
        e->f2c = s->f2c;
        e->f8 = s->fc;
        e->fc = s->f10;
        e->f10 = s->f14;
        e->f14 = s->f18;
        e->f18 = s->f1c;
    }
    d->f8 = (ElemB *)p;
    for (i = 0; i < h->f4; i++) {
        s2 = &h->f8[i];
        e2 = &d->f8[i];
        e2->f18 = 0;
        if (s2->f10 != 0x1000)
            e2->f18 |= 1;
        if (s2->f14 != 0x1000)
            e2->f18 |= 1;
        if (s2->f18 != 0x1000)
            e2->f18 |= 1;
        e2->f18 |= e2[s2->f8].f18;
        e2->fc = s2->f10;
        e2->f10 = s2->f14;
        e2->f14 = s2->f18;
        e2->f1a = s2->f1c << 4;
        e2->f1c = s2->f1e << 4;
        e2->f1e = s2->f20 << 4;
        e2->f20 = s2->f24;
        e2->f24 = s2->f28;
        e2->f28 = s2->f2c;
        e2->f2c = 0;
        e2->f30 = 0;
    }
}
}

extern "C" void *func_020462bc(int *p)
{
    return &((Matrix4x3 *)0)[p[1]];
}

extern "C" void func_020462b4(int *p, int v)
{
    p[4] = v;
}

void ModelComponents::SetMaterialPolygonMode(int mode, int index) {
    BMD_Material *e = &materials[index];
    e->flags &= ~0x30;
    e->flags |= mode << 4;
}

void ModelComponents::SetPolygonMode(int mode) {
    int i;
    int count = modelFile->numMaterials;
    for (i = 0; i < count; i++)
        SetMaterialPolygonMode(mode, i);
}

extern "C" void func_02046208(void *self, unsigned int value, int index)
{
    ModelComponents *obj = (ModelComponents *)self;
    BMD_Material *e = &obj->materials[index];
    e->flags &= ~0x1f0000;
    e->flags |= value << 16;
}

void ModelComponents::SetAlpha(unsigned int alpha) {
    int i;
    int count = modelFile->numMaterials;
    for (i = 0; i < count; i++)
        func_02046208(this, alpha, i);
}

/* The two passes dExtShadowModel_c::RenderAll draws each shadow volume with.
   The mask pass draws back faces as shadow polygons with polygon ID 0, which
   marks the stencil; the draw pass draws front faces as shadow polygons
   with polygonID, which shades what the mask marked. */
void ModelComponents::SetShadowMask(int alpha) {
    int i;
    int n = modelFile->numMaterials;
    for (i = 0; i < n; i++) {
        BMD_Material *e = &materials[i];
        e->flags &= 0xc0ffff0f;
        e->flags |= 0x40;
        e->flags |= 0x30;
        e->flags &= ~0x1f0000;
        e->flags |= alpha << 16;
    }
}

void ModelComponents::SetShadowDraw(int alpha, int polygonID) {
    int i;
    int n = modelFile->numMaterials;
    for (i = 0; i < n; i++) {
        BMD_Material *e = &materials[i];
        e->flags &= 0xc0ffff0f;
        e->flags |= polygonID << 24;
        e->flags |= 0x80;
        e->flags |= 0x30;
        e->flags &= ~0x1f0000;
        e->flags |= alpha << 16;
    }
}

void ModelComponents::SetMaterialPolygonID(int id, int index) {
    BMD_Material *e = &materials[index];
    e->flags &= ~0x3f000000;
    e->flags |= id << 24;
}

void ModelComponents::SetPolygonID(int id) {
    int i;
    int count = modelFile->numMaterials;
    for (i = 0; i < count; i++)
        SetMaterialPolygonID(id, i);
}

void ModelComponents::SetMaterialDiffuse(unsigned int color, int index) {
    BMD_Material *e = &materials[index];
    e->diffuseAmbient &= 0xffff8000;
    e->diffuseAmbient |= color;
}

void ModelComponents::SetDiffuse(unsigned int color) {
    int i;
    int count = modelFile->numMaterials;
    for (i = 0; i < count; i++)
        SetMaterialDiffuse(color, i);
}

void ModelComponents::SetMaterialAmbient(unsigned int color, int index) {
    BMD_Material *e = &materials[index];
    e->diffuseAmbient &= 0xffff;
    e->diffuseAmbient |= color << 16;
}

void ModelComponents::SetAmbient(unsigned int color) {
    int i;
    int count = modelFile->numMaterials;
    for (i = 0; i < count; i++)
        SetMaterialAmbient(color, i);
}

void ModelComponents::SetMaterialEmission(unsigned int color, int index) {
    BMD_Material *e = &materials[index];
    e->specularEmission &= 0xffff;
    e->specularEmission |= color << 16;
}

void ModelComponents::SetEmission(unsigned int color) {
    int i;
    int count = modelFile->numMaterials;
    for (i = 0; i < count; i++)
        SetMaterialEmission(color, i);
}

extern "C" void func_02045e44(void *self, unsigned int value, int index)
{
    struct Element { char pad[0x1c]; unsigned int flags; char pad2[0x30 - 0x1c - 4]; };
    struct Obj { void *pad0; struct Element *arr; };
    struct Obj *obj = (struct Obj *)self;
    struct Element *e = &obj->arr[index];
    unsigned int *flags = (unsigned int *)(&e->flags);
    *flags &= ~0xc0000000;
    *flags |= value << 30;
}

extern "C" void func_02045e2c(int obj, int i, int a, short b)
{
    ModelBone *e = &((ModelComponents *)obj)->bones[i];
    e->anim = (BCA_File *)a;
    e->animFrame = b;
}

extern "C" void func_02045d9c(void)
{
    OS_EnableDTCM();
    G3X_AlphaTest(true, 0);
    G3X_AlphaBlend(true);
    G3X_SetClearColor(0, 0, 0x7fff, 0, false);
    if (GX_GetBankForTex() == 0) {
        OS_Terminate();
    }
    if (GX_GetBankForTexPltt() != 0) {
        return;
    }
    OS_Terminate();
}

extern "C" void InitialiseVramGlobals(void)
{
    int r = GX_GetSizeOfTex();
    data_020a4be4 = 0;
    data_020a4bc8 = 0;
    data_020a4be0 = 0x20000;
    data_020a4be8 = 0x20000;
    data_020a4bdc = r;
    data_020a4bd8 = GX_GetSizeOfTexPltt();
    data_020a4bcc = 0;
}

extern "C" int func_02045d10(void) { return data_020a4bc8; }

extern "C" int func_02045d00(void) { return data_020a4bdc; }

extern "C" int func_02045cf0(void) { return data_020a4bcc; }

extern "C" int func_02045ce0(void) { return data_020a4bd8; }

u32 dExtSimpleModel_c::GetVramOffset(u32 size) {
    u32 *p;
    u32 b = data_020a4be8 - data_020a4bc8;
    u32 a = data_020a4bdc - data_020a4be0;
    if ((a << 1) > b && a > size) {
        p = &data_020a4bdc;
    } else if (b > size) {
        p = &data_020a4be8;
    } else {
        OS_Terminate();
    }
    *p -= size;
    return *p;
}

u32 dExtSimpleModel_c::LoadTextureToVram(char *texData, u32 size) {
    u32 vramOffset = GetVramOffset(size);
    GX_BeginLoadTex();
    GX_LoadTex(texData, vramOffset, size);
    data_020a4be4 += size;
    GX_EndLoadTex();
    return vramOffset;
}

u32 dExtSimpleModel_c::LoadCompressedTextureToVram(char *src, u32 size, char *dst) {
    u32 vramOffset;

    if ((data_020a4be8 - data_020a4bc8) < size)
        OS_Terminate();
    GX_BeginLoadTex();
    GX_LoadTex(src, data_020a4bc8, size);
    data_020a4be4 += size;
    GX_LoadTex(dst, data_020a4be0, size / 2);
    data_020a4be4 += size >> 1;
    GX_EndLoadTex();
    vramOffset = data_020a4bc8;
    data_020a4bc8 += size;
    data_020a4be0 += size >> 1;
    return vramOffset;
}

// 0x02045ad8 (0x80)
extern "C" u32 func_02045ad8(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = data_020a4bcc;
    end  = data_020a4bd8;

    if (slot + size > end)
        OS_Terminate();

    GX_BeginLoadTexPltt();
    GX_LoadTexPltt(src, data_020a4bcc, size);
    GX_EndLoadTexPltt();

    slot = data_020a4bcc;
    data_020a4bcc = slot + ((size + 0xf) & 0xfff0);

    return slot;
}

// 0x02045a50 (0x88)
extern "C" u32 func_02045a50(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = data_020a4bcc;
    end  = data_020a4bd8;

    if (slot + size > end)
        OS_Terminate();

    data_020a4bd8 -= (size + 0xf) & 0xfff0;

    GX_BeginLoadTexPltt();
    GX_LoadTexPltt(src, data_020a4bd8, size);
    GX_EndLoadTexPltt();

    return data_020a4bd8;
}

// 0x020458e0 (0x170)
void dExtSimpleModel_c::LoadTexAndPal(BMD_File &file) {
    u32 i;
    u32 ret;
    u32 sz;
    BMD_Palette *p;
    BMD_Texture *t;

    for (i = 0; i < file.numTextures; i++) {
        t = &file.textures[i];
        sz = t->size;
        if (((t->flags >> 26) & 7) == 5) {
            ret = LoadCompressedTextureToVram(t->data, sz, t->data + sz);
        } else {
            ret = LoadTextureToVram(t->data, sz);
        }
        t->flags = (t->flags & ~0xffff) | ((ret >> 3) & 0xffff);
    }

    GX_BeginLoadTexPltt();

    for (i = 0; i < file.numPalettes; i++) {
        p = &file.palettes[i];
        sz = p->size;
        if (data_020a4bcc + sz > data_020a4bd8)
            OS_Terminate();
        if (sz <= 8) {
            GX_LoadTexPltt(p->data, data_020a4bcc, sz);
            p->vramOffset = data_020a4bcc;
            data_020a4bcc = data_020a4bcc + ((sz + 7) & 0xfff8);
        } else {
            data_020a4bd8 = data_020a4bd8 - ((sz + 0xf) & 0xfff0);
            GX_LoadTexPltt(p->data, data_020a4bd8, sz);
            p->vramOffset = data_020a4bd8;
        }
    }

    GX_EndLoadTexPltt();
}
