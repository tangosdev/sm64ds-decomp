//cpp
/* Model4 -- VRAM upload for a BMD model, 0x020458e0..0x020468a0.
 *
 * Not part of src/engine/model/Model.cpp: func_020458a8 sits between that TU
 * and this run. TextureSequence::UpdateFileOffsets begins at 0x020468a0.
 * The name is the tree's Model class plus the numeric suffix already used
 * for a second translation unit (Juan 2026-10-11).
 *
 * mwccarm's default deferred codegen emits .text in reverse source order, so
 * the definitions are highest address first. Do not reorder.
 */

#include "Model.h"
#include "G3X.h"

extern "C" {
extern u32 data_020a4bc8;
extern u32 data_020a4bcc;
extern u32 data_020a4bd8;
extern u32 data_020a4bdc;
extern u32 data_020a4be0;
extern u32 data_020a4be4;
extern u32 data_020a4be8;
void Crash();
void _ZN2GX16BeginLoadTexPlttEv(void);
void _ZN2GX11LoadTexPlttEPKvjj(const void *src, u32 slot, u32 size);
void _ZN2GX14EndLoadTexPlttEv(void);
int func_02053e34(void);
int func_02053e1c(void);
}

// @symbol _ZN5Model17UpdateFileOffsetsER8BMD_File
#define REBASE(p) ((char *)(p) + base)
void Model::UpdateFileOffsets(BMD_File &file)
{
    u32 i;
    int k;
    int base = (int)&file;

    if (file.bones) file.bones = (BMD_Bone *)REBASE(file.bones);
    for (i = 0; i < file.numBones; i++) {
        BMD_Bone *b = file.bones + i;
        if (b->unk_04) b->unk_04 = REBASE(b->unk_04);
        if (b->materialIds) b->materialIds = (u8 *)REBASE(b->materialIds);
        if (b->unk_38) b->unk_38 = REBASE(b->unk_38);
    }

    if (file.displayListGroups)
        file.displayListGroups = (BMD_DisplayListGroup *)REBASE(file.displayListGroups);
    for (i = 0; i < file.numDisplayListGroups; i++) {
        BMD_DisplayListGroup *g = file.displayListGroups + i;
        if (g->lists) g->lists = (BMD_DisplayList *)((int)g->lists + base);
        for (k = 0; k < (int)g->numLists; k++) {
            BMD_DisplayList *d = g->lists + k;
            if (d->unk_04) d->unk_04 = (void *)((int)d->unk_04 + base);
            if (d->data) d->data = (void *)((int)d->data + base);
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

    if (file.unk_2c) file.unk_2c = REBASE(file.unk_2c);
    if (file.unk_30 == 0) return;
    if (file.unk_34) file.unk_34 = REBASE(file.unk_34);
    {
        void **e = (void **)file.unk_34;
        if (e[0]) e[0] = REBASE(e[0]);
        if (e[1]) e[1] = REBASE(e[1]);
        if (e[2]) e[2] = REBASE(e[2]);
        if (e[3]) e[3] = REBASE(e[3]);
    }
}
#undef REBASE

extern "C" int _ZN4cstd6strcmpEPKcS1_(const char *a, const char *b);

// @symbol func_02046590
extern "C" int func_02046590(const void *tbl, const char *name)
{
    struct Entry {
        const char *name;
        char pad[0x2c];
    };
    struct Table {
        char pad[0x24];
        u32 count;
        struct Entry *entries;
    };
    const struct Table *t = (const struct Table *)tbl;
    u32 count = t->count;
    u32 i = 0;
    if (i < count) {
        struct Entry *entry = t->entries;
        while (1)
        {
            if (_ZN4cstd6strcmpEPKcS1_(entry->name, name) == 0)
                return (int)i;
            i++;
            entry = (struct Entry *)((char *)entry + 0x30);
            if (i >= t->count)
                break;
        }
    }
    Crash();
    return -1;
}

// @symbol func_02046564
extern "C" int func_02046564(void *p)
{
    struct S { int pad0; int a; char p8[0x24-8]; int c; char p28[0x30-0x28]; int b; };
    struct A { char x[0x30]; };
    struct S *s = (struct S *)p;
    int a = s->a;
    int b = s->b;
    int c = s->c;
    int t = (int)&((struct A *)0)[a + b];
    t += c * 0x30;
    t += a * 0x34;
    return t;
}

// @symbol func_020462d0
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
    int t14;
    SrcB *s2;
    ElemB *e2;
    u32 o40;
    u32 o34;
    u32 j;

    p = arr;
    d->f0 = h;
    d->f10 = 0;
    d->fc = p;
    p += h->f4 + h->f30;
    d->f4 = p;
    p += h->f24;
    t14 = 0x14;
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
            u32 v = *(int *)((int)h->f18 + c1 * t14 + 0x10);
            shift = (v >> 0x1a) & 7;
            *(int *)(((int)e + 0x1c)) |= v;
        }
        idx2 = h->f28[i].f8;
        if (idx2 != -1) {
            u32 v2;
            if (h->f20 == 0)
                Crash();
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
    j = 0;
    if (j < h->f4) {
        o40 = 0;
        o34 = 0;
        do {
            s2 = (SrcB *)((int)h->f8 + o40);
            e2 = (ElemB *)((int)d->f8 + o34);
            e2->f18 = 0;
            if (s2->f10 != 0x1000)
                *(u16 *)(((int)e2 + 0x18)) |= 1;
            if (s2->f14 != 0x1000)
                *(u16 *)(((int)e2 + 0x18)) |= 1;
            if (s2->f18 != 0x1000)
                *(u16 *)(((int)e2 + 0x18)) |= 1;
            *(u16 *)(((int)e2 + 0x18)) |= e2[s2->f8].f18;
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
            o40 += 0x40;
            o34 += 0x34;
            j++;
        } while (j < h->f4);
    }
}
}

// @symbol func_020462bc
extern "C" void *func_020462bc(int *p)
{
    struct E { char _pad[0x30]; };
    return &((struct E *)0)[p[1]];
}

// @symbol func_020462b4
extern "C" void func_020462b4(int *p, int v)
{
    p[4] = v;
}

// @symbol func_02046288
extern "C" void func_02046288(void *self, void *value, int index)
{
    struct Element { char pad[0x24]; unsigned int flags; char pad2[0x30 - 0x24 - 4]; };
    struct Obj { void *pad0; struct Element *arr; };
    struct Obj *obj = (struct Obj *)self;
    struct Element *e = &obj->arr[index];
    unsigned int bits = (unsigned int)value;
    unsigned int *flags = (unsigned int *)(&e->flags);
    *flags &= ~0x30;
    *flags |= bits << 4;
}

// @symbol func_02046234
extern "C" void func_02046234(void *a, void *b)
{
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++)
        func_02046288(a, b, i);
}

// @symbol func_02046208
extern "C" void func_02046208(void *self, unsigned int value, int index)
{
    struct Element { char pad[0x24]; unsigned int flags; char pad2[0x30 - 0x24 - 4]; };
    struct Obj { void *pad0; struct Element *arr; };
    struct Obj *obj = (struct Obj *)self;
    struct Element *e = &obj->arr[index];
    unsigned int *flags = (unsigned int *)(&e->flags);
    *flags &= ~0x1f0000;
    *flags |= value << 16;
}

// @symbol func_020461b4
extern "C" void func_020461b4(void *a, unsigned int b)
{
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++)
        func_02046208(a, b, i);
}

// @symbol func_02046120
extern "C" void func_02046120(void *self, int a)
{
    struct Sub { char _pad[0x24]; int count; };
    struct Obj { struct Sub *sub; char *data; };
    struct Obj *obj = (struct Obj *)self;
    int i;
    int n = obj->sub->count;
    for (i = 0; i < n; i++) {
        volatile u32 *p = (volatile u32 *)((int)obj->data + i * 0x30 + 0x24);
        *p &= 0xc0ffff0f;
        *p |= 0x40;
        *p |= 0x30;
        *p &= ~0x1f0000;
        *p |= a << 16;
    }
}

// @symbol func_02046088
extern "C" void func_02046088(void *self, int a, int b)
{
    struct Sub { char _pad[0x24]; int count; };
    struct Obj { struct Sub *sub; char *data; };
    struct Obj *obj = (struct Obj *)self;
    int i;
    int n = obj->sub->count;
    for (i = 0; i < n; i++) {
        volatile u32 *p = (volatile u32 *)((int)obj->data + i * 0x30 + 0x24);
        *p &= 0xc0ffff0f;
        *p |= b << 24;
        *p |= 0x80;
        *p |= 0x30;
        *p &= ~0x1f0000;
        *p |= a << 16;
    }
}

// @symbol func_0204605c
extern "C" void func_0204605c(void *self, void *value, int index)
{
    struct Element { char pad[0x24]; unsigned int flags; char pad2[0x30 - 0x24 - 4]; };
    struct Obj { void *pad0; struct Element *arr; };
    struct Obj *obj = (struct Obj *)self;
    struct Element *e = &obj->arr[index];
    unsigned int bits = (unsigned int)value;
    unsigned int *flags = (unsigned int *)(&e->flags);
    *flags &= ~0x3f000000;
    *flags |= bits << 24;
}

// @symbol func_02046008
extern "C" void func_02046008(void *a, void *b)
{
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++)
        func_0204605c(a, b, i);
}

// @symbol func_02045fd4
extern "C" void func_02045fd4(void *self, void *value, int index)
{
    struct Element { char pad[0x28]; unsigned int flags; char pad2[0x30 - 0x28 - 4]; };
    struct Obj { void *pad0; struct Element *arr; };
    struct Obj *obj = (struct Obj *)self;
    struct Element *e = &obj->arr[index];
    unsigned int bits = (unsigned int)value;
    unsigned int *flags = (unsigned int *)(&e->flags);
    *flags &= 0xffff8000;
    *flags |= bits;
}

// @symbol func_02045f80
extern "C" void func_02045f80(void *a, void *b)
{
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++)
        func_02045fd4(a, b, i);
}

// @symbol func_02045f4c
extern "C" void func_02045f4c(void *self, void *value, int index)
{
    struct Element { char pad[0x28]; unsigned int flags; char pad2[0x30 - 0x28 - 4]; };
    struct Obj { void *pad0; struct Element *arr; };
    struct Obj *obj = (struct Obj *)self;
    struct Element *e = &obj->arr[index];
    unsigned int bits = (unsigned int)value;
    unsigned int *flags = (unsigned int *)(&e->flags);
    *flags &= 0xffff;
    *flags |= bits << 16;
}

// @symbol func_02045ef8
extern "C" void func_02045ef8(void *a, void *b)
{
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++)
        func_02045f4c(a, b, i);
}

// @symbol func_02045ec4
extern "C" void func_02045ec4(char *self, unsigned int value, int index)
{
    struct Element { char pad[0x2c]; unsigned int flags; char pad2[0x30 - 0x2c - 4]; };
    struct Obj { void *pad0; struct Element *arr; };
    struct Obj *obj = (struct Obj *)self;
    struct Element *e = &obj->arr[index];
    unsigned int bits = value;
    unsigned int *flags = (unsigned int *)(&e->flags);
    *flags &= 0xffff;
    *flags |= bits << 16;
}

// @symbol func_02045e70
extern "C" void func_02045e70(char *thiz, void *arg1)
{
    int i;
    int count = *(int *)(*(char **)thiz + 0x24);
    for (i = 0; i < count; i++)
        func_02045ec4(thiz, (unsigned int)arg1, i);
}

// @symbol func_02045e44
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

// @symbol func_02045e2c
extern "C" void func_02045e2c(int obj, int i, int a, short b)
{
    struct Elem {
        char pad[0x2c];
        int a;
        short b;
    };
    struct Elem *e = (struct Elem *)(*(int *)(obj + 8) + i * 0x34);
    e->a = a;
    e->b = b;
}

extern "C" void _ZN4CP1510EnableDTCMEv(void);
extern "C" u16 func_02053ed0(void);
extern "C" u16 func_02053ec0(void);

// @symbol func_02045d9c
extern "C" void func_02045d9c(void)
{
    _ZN4CP1510EnableDTCMEv();
    u16 r3 = *(volatile u16 *)0x4000060;
    u16 r1 = (u16)0x4000340;
    int r2 = 0x7fff;
    r3 = (r3 & ~0x3000u) | 4;
    *(volatile u16 *)0x4000060 = r3;
    *(volatile u16 *)0x4000340 = 0;
    u16 ip = *(volatile u16 *)0x4000060;
    ip = (ip & ~0x3000u) | 8;
    *(volatile u16 *)0x4000060 = ip;
    G3X::SetClearColor(0, 0, r2, 0, false);
    if (func_02053ed0() == 0) {
        Crash();
    }
    if (func_02053ec0() != 0) {
        return;
    }
    Crash();
}

// @symbol InitialiseVramGlobals
extern "C" void InitialiseVramGlobals(void)
{
    int r = func_02053e34();
    data_020a4be4 = 0;
    data_020a4bc8 = 0;
    data_020a4be0 = 0x20000;
    data_020a4be8 = 0x20000;
    data_020a4bdc = r;
    data_020a4bd8 = func_02053e1c();
    data_020a4bcc = 0;
}

// @symbol func_02045d10
extern "C" int func_02045d10(void) { return data_020a4bc8; }

// @symbol func_02045d00
extern "C" int func_02045d00(void) { return data_020a4bdc; }

// @symbol func_02045cf0
extern "C" int func_02045cf0(void) { return data_020a4bcc; }

// @symbol func_02045ce0
extern "C" int func_02045ce0(void) { return data_020a4bd8; }

struct GX {
    static void BeginLoadTexPltt();                          /* _ZN2GX16BeginLoadTexPlttEv */
    static void LoadTexPltt(const void *src, u32 addr, u32 size);
    static void EndLoadTexPltt();
    static void BeginLoadTex();                              /* _ZN2GX12BeginLoadTexEv */
    static void LoadTex(const void *src, u32 offset, u32 size);
    static void EndLoadTex();
};

// @symbol _ZN5Model13GetVramOffsetEj
u32 Model::GetVramOffset(u32 size)
{
    u32 *p;
    u32 b = data_020a4be8 - data_020a4bc8;
    u32 a = data_020a4bdc - data_020a4be0;
    if ((a << 1) > b && a > size) {
        p = &data_020a4bdc;
    } else if (b > size) {
        p = &data_020a4be8;
    } else {
        Crash();
    }
    *p -= size;
    return *p;
}

// @symbol _ZN5Model17LoadTextureToVramEPcj
u32 Model::LoadTextureToVram(char *texData, u32 size)
{
    u32 vramOffset = GetVramOffset(size);
    GX::BeginLoadTex();
    GX::LoadTex(texData, vramOffset, size);
    data_020a4be4 += size;
    GX::EndLoadTex();
    return vramOffset;
}

// @symbol _ZN5Model27LoadCompressedTextureToVramEPcjS0_
/* Stays off through the rest of the file. Restoring it before deferred
   codegen emits this function reorders five words. */
#pragma opt_common_subs off
u32 Model::LoadCompressedTextureToVram(char *src, u32 size, char *dst)
{
    u32 vramOffset;

    if ((data_020a4be8 - data_020a4bc8) < size)
        Crash();
    GX::BeginLoadTex();
    GX::LoadTex(src, data_020a4bc8, size);
    data_020a4be4 += size;
    GX::LoadTex(dst, data_020a4be0, size >> 1);
    data_020a4be4 += size >> 1;
    GX::EndLoadTex();
    vramOffset = data_020a4bc8;
    data_020a4bc8 += size;
    data_020a4be0 += size >> 1;
    return vramOffset;
}

// 0x02045ad8 (0x80)
// @symbol func_02045ad8
extern "C" u32 func_02045ad8(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = data_020a4bcc;
    end  = data_020a4bd8;

    if (slot + size > end)
        Crash();

    _ZN2GX16BeginLoadTexPlttEv();
    _ZN2GX11LoadTexPlttEPKvjj(src, data_020a4bcc, size);
    _ZN2GX14EndLoadTexPlttEv();

    slot = data_020a4bcc;
    data_020a4bcc = slot + ((size + 0xf) & 0xfff0);

    return slot;
}

// 0x02045a50 (0x88)
// @symbol func_02045a50
extern "C" u32 func_02045a50(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = data_020a4bcc;
    end  = data_020a4bd8;

    if (slot + size > end)
        Crash();

    data_020a4bd8 -= (size + 0xf) & 0xfff0;

    _ZN2GX16BeginLoadTexPlttEv();
    _ZN2GX11LoadTexPlttEPKvjj(src, data_020a4bd8, size);
    _ZN2GX14EndLoadTexPlttEv();

    return data_020a4bd8;
}

// 0x020458e0 (0x170)
// @symbol _ZN5Model13LoadTexAndPalER8BMD_File
void Model::LoadTexAndPal(BMD_File &file)
{
    u32 i;
    u32 ret;
    u32 sz;
    BMD_Palette *p;
    u32 poff;
    u32 j;
    BMD_Texture *t;
    u32 off;

    i = 0;
    if (i < file.numTextures) {
        off = i;
        do {
            t = (BMD_Texture *)((char *)file.textures + off);
            sz = t->size;
            if (((t->flags >> 26) & 7) == 5) {
                ret = LoadCompressedTextureToVram(t->data, sz, t->data + sz);
            } else {
                ret = LoadTextureToVram(t->data, sz);
            }
            t->flags = (t->flags & ~0xffff) | ((ret >> 3) & 0xffff);
            i++;
            off += sizeof(BMD_Texture);
        } while (i < file.numTextures);
    }

    GX::BeginLoadTexPltt();

    j = 0;
    if (j < file.numPalettes) {
        poff = j;
        do {
            p = (BMD_Palette *)((char *)file.palettes + poff);
            sz = p->size;
            if (data_020a4bcc + sz > data_020a4bd8) Crash();
            if (sz <= 8) {
                GX::LoadTexPltt(p->data, data_020a4bcc, sz);
                p->vramOffset = data_020a4bcc;
                data_020a4bcc = data_020a4bcc + ((sz + 7) & 0xfff8);
            } else {
                data_020a4bd8 = data_020a4bd8 - ((sz + 0xf) & 0xfff0);
                GX::LoadTexPltt(p->data, data_020a4bd8, sz);
                p->vramOffset = data_020a4bd8;
            }
            j++;
            poff += sizeof(BMD_Palette);
        } while (j < file.numPalettes);
    }

    GX::EndLoadTexPltt();
}
