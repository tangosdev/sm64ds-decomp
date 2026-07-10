// Copies vector triple 0 from the source entry table (+0x24, stride 0x40)
// into the destination table (+0x20, stride 0x34), then copies triples
// 2..count-1 with a signed-short counter and moving element pointers.
typedef unsigned int u32;
typedef signed short s16;

typedef struct SrcTbl {
    char _pad0[0x24];
    int a, b, c;
} SrcTbl;

typedef struct DstTbl {
    char _pad0[0x20];
    int a, b, c;
} DstTbl;

typedef struct A {
    char _pad0[4];
    u32 count;
    SrcTbl *tbl;
} A;

typedef struct Inner {
    A *a;
    char _pad0[4];
    DstTbl *dst;
} Inner;

typedef struct Self {
    char _pad0[8];
    Inner inner;
} Self;

void func_ov075_02114b60(Self *self)
{
    Inner *q = (Inner *)((long long)(int)&self->inner & 0xFFFFFFFFFFFFFFFFLL);
    char *dp;
    A *a = q->a;
    DstTbl *d = q->dst;
    SrcTbl *s = a->tbl;
    char *sp;
    s16 i;
    d->a = s->a;
    d->b = s->b;
    d->c = s->c;
    dp = (char *)d + 2 * 0x34;
    sp = (char *)s + 2 * 0x40;
    for (i = 2; i < a->count; i++) {
        *(int *)(dp + 0x20) = *(int *)(sp + 0x24);
        *(int *)(dp + 0x24) = *(int *)(sp + 0x28);
        *(int *)(dp + 0x28) = *(int *)(sp + 0x2c);
        dp += 0x34;
        sp += 0x40;
    }
}
