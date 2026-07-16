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

/* G: first word packs axis (bits 17-18) and flag (bit 19); scale at +0x20; fa/fb at +0x34 */
typedef struct {
    u32 : 17;
    u32 axis : 2;
    u32 flag : 1;
    u32 : 12;
    char _pad4[0x20 - 4];
    s16 scale;
    char _pad22[0x34 - 0x22];
    u32 : 24;
    u32 fa : 2;
    u32 fb : 2;
    u32 : 4;
} G;

typedef struct {
    G *p0;
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

void func_0204b244(Self *self, Node *node)
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
        Cnt74 *q = (Cnt74 *)(u32)(((s64)(int)((char *)c + 0x74)) & 0xFFFFFFFFFFFFFFFFLL);
        q->count++;
        if (c->cnt.count > c->cnt.limit)
            q->count = c->cnt.start;
    }

    if (scale == 0)
        return;

    {
        G *g = self->p34->p18->p0;
        int idx = node->p36 >> 4;
        data_02099fbc[g->axis](data_02082214[idx * 2], data_02082214[idx * 2 + 1], rot);
    }

    {
        G *g = self->p34->p18->p0;
        int v1 = (int)(((long long)node->p30 * node->p34 + 0x800) >> 12);
        int v2 = (int)(((long long)v1 * g->scale + 0x800) >> 12);
        func_020527e8(scl, v2, v1, v1);
    }

    /* ROM: r0=scl@0x60, r1=rot@0x30, r2=out@0 */
    MulMat4x3Mat4x3(scl, rot, out);

    out[9] = node->p14 + node->p8;
    out[10] = node->p18 + node->pc;
    out[11] = node->p1c + node->p10;

    func_020553a4(self->p38);
    func_02055388(out);

    *(volatile u32 *)0x4000480 = node->p3a;
    {
        G *g = self->p34->p18->p0;
        data_02099fb4[g->flag]((u8)g->fa, (u8)g->fb);
    }
}
