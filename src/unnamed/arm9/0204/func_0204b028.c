typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef long long s64;

typedef struct {
    u32 start : 6;
    u32 limit : 6;
    u32 count : 6;
    u32 : 14;
} Cnt74;

typedef struct {
    u16 : 9;
    u16 axis : 2;
    u16 flag : 1;
    u16 : 4;
    char _pad2[0x10 - 2];
    u32 tx : 2;
    u32 ty : 2;
    u32 : 28;
} Bdat;

typedef struct {
    char *p0;
    char _pad4[0x14 - 4];
    Bdat *p14;
} Adat;

typedef struct {
    char _pad0[0x18];
    Adat *p18;
    char _pad1c[0x74 - 0x1c];
    Cnt74 cnt;
} Ctx;

typedef struct {
    u16 lo : 5;
    u16 hi : 5;
    u16 : 6;
} M40;

typedef struct {
    char _pad0[8];
    int p8;
    int pc;
    int p10;
    int p14;
    int p18;
    int p1c;
    char _pad20[0x30 - 0x20];
    int p30;
    s16 p34;
    u16 p36;
    char _pad38[2];
    u16 p3a;
    char _pad3c[4];
    M40 p40;
} Node;

typedef struct {
    char _pad0[0x30];
    int p30;
    Ctx *p34;
    void *p38;
} Self;

extern s16 data_02082214[];
extern void (*data_02099fbc[])(int, int, void *);
extern void (*data_02099fb4[])(u8, u8);

extern void func_020527e8(int *m, int sx, int sy, int sz);
extern void MulMat4x3Mat4x3(void *a, void *b, void *out);
extern void func_020553a4(void *m);
extern void func_02055388(void *m);

void func_0204b028(Self *self, Node *node)
{
    Ctx *c = self->p34;
    int scale = (node->p40.lo * (node->p40.hi + 1)) >> 5;
    int out[12];
    int rot[12];
    int scl[12];
    u32 f2;
    u32 v;

    v = *(u32 *)((char *)c + 0x74) << 14;
    f2 = v >> 26;
    v = self->p30 | 0xc0;
    v |= f2 << 24;
    v |= scale << 16;
    *(volatile u32 *)0x40004a4 = v;
    *(volatile u32 *)0x40004a4;
    {
        Cnt74 *q = (Cnt74 *)(u32)((char *)c + 0x74);
        q->count++;
        if (c->cnt.count > c->cnt.limit)
            q->count = c->cnt.start;
    }

    if (scale == 0)
        return;

    {
        Bdat *B = self->p34->p18->p14;
        int idx = node->p36 >> 4;
        data_02099fbc[B->axis](data_02082214[idx * 2], data_02082214[idx * 2 + 1], rot);
    }

    {
        Adat *A = self->p34->p18;
        int v1 = (int)(((long long)node->p30 * node->p34 + 0x800) >> 12);
        s16 q = *(s16 *)(A->p0 + 0x20);
        int v2 = (int)(((long long)v1 * q + 0x800) >> 12);
        func_020527e8(scl, v2, v1, v1);
    }

    MulMat4x3Mat4x3(rot, scl, out);

    out[9] = node->p14 + node->p8;
    out[10] = node->p18 + node->pc;
    out[11] = node->p1c + node->p10;

    func_020553a4(self->p38);
    func_02055388(out);

    *(volatile u32 *)0x4000480 = node->p3a;
    {
        Bdat *B = self->p34->p18->p14;
        data_02099fb4[B->flag](B->tx, B->ty);
    }
}
