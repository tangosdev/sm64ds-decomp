#include "types.h"
struct Entry {
    u16 id;     /* +0x00 */
    u16 p2;
    u16 p4;
    u16 p6;
    u16 flag1;  /* +0x08 */
    u16 idx1;   /* +0x0a */
    u16 flag2;  /* +0x0c */
    u16 idx2;   /* +0x0e */
    u16 flag3;  /* +0x10 */
    u16 idx3;   /* +0x12 */
    u16 flag4;  /* +0x14 */
    u16 idx4;   /* +0x16 */
    u16 flag5;  /* +0x18 */
    u16 idx5;   /* +0x1a */
};

struct Desc {
    int f0;             /* +0x00 */
    int *tableA;        /* +0x04 */
    u16 *tableB;        /* +0x08 */
    int *tableC;        /* +0x0c */
    u16 count;          /* +0x10 */
    u16 p12;
    struct Entry *entries; /* +0x14 */
};

struct Ctx {
    int f0;
    char *base;         /* +0x04 */
};

struct Dst {
    char pad[8];
    int f8;    /* +0x08 */
    int fc;    /* +0x0c */
    u16 f10;   /* +0x10 */
    u16 p12;
    int f14;   /* +0x14 */
    int f18;   /* +0x18 */
    char pad2[0x30 - 0x1c];
};

extern void Crash(void);
extern void func_02045e44(struct Ctx *self, unsigned int value, int index);

void func_020469e8(struct Ctx *ctx, struct Desc *desc, int sb) {
    int i;
    for (i = 0; i < desc->count; i++) {
        struct Entry *e = &desc->entries[i];
        struct Dst *dst;
        if (e->id == 0xffff) Crash();
        dst = (struct Dst *)(ctx->base + e->id * 0x30);
        func_02045e44(ctx, 1, e->id);
        if (e->flag1 == 1) dst->f8 = desc->tableA[e->idx1];
        else dst->f8 = desc->tableA[e->idx1 + sb];
        if (e->flag2 == 1) dst->fc = desc->tableA[e->idx2];
        else dst->fc = desc->tableA[e->idx2 + sb];
        if (e->flag3 == 1) dst->f10 = desc->tableB[e->idx3];
        else dst->f10 = desc->tableB[e->idx3 + sb];
        if (e->flag4 == 1) dst->f14 = desc->tableC[e->idx4];
        else dst->f14 = desc->tableC[e->idx4 + sb];
        if (e->flag5 == 1) dst->f18 = desc->tableC[e->idx5];
        else dst->f18 = desc->tableC[e->idx5 + sb];
    }
}
