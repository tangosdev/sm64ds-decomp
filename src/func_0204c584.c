typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;

typedef struct Vector3 { int x, y, z; } Vector3;

typedef struct Node { struct Node *next; struct Node *prev; } Node;
typedef struct List { Node *head; int count; } List;

typedef struct Def {
    u32 mode : 4;      /* bits 0-3 */
    u32 f4 : 2;        /* 4-5 */
    u32 sel : 2;       /* 6-7 */
    u32 f8 : 1;        /* 8 */
    u32 texanim : 1;   /* 9 */
    u32 f10 : 1;       /* 10 */
    u32 colorrand : 1; /* 11 */
    u32 sizerand : 1;  /* 12 */
    u32 anglerand : 1; /* 13 */
    u32 rest : 18;
    int rate;          /* 0x4 */
    u8 pad8[0xa];      /* 0x8 */
    u16 tex;           /* 0x12 */
    u8 pad14[0x10];    /* 0x14 */
    s16 lo;            /* 0x24 */
    s16 hi;            /* 0x26 */
    u8 pad28[4];       /* 0x28 */
    u8 b2c;            /* 0x2c */
    u8 b2d;            /* 0x2d */
    u8 b2e;            /* 0x2e */
    u8 pad2f[4];       /* 0x2f */
    u8 b33;            /* 0x33 */
    u8 b34;            /* 0x34 */
} Def;

typedef struct TexAnim {
    u16 a;             /* 0x0 */
    u16 b;             /* 0x2 */
    u8 pad[4];         /* 0x4 */
    u16 flag : 1;      /* 0x8 bit 0 */
    u16 frest : 15;
} TexAnim;

typedef struct Pal {
    u8 colors[8];      /* 0x0 */
    u32 cnt : 8;       /* 0x8 bits 0-7 */
    u32 px : 8;        /* bits 8-15 */
    u32 loop : 1;      /* bit 16 */
    u32 prest : 15;
} Pal;

extern void func_0204cebc(void *c);
extern Node *func_0204d958(List *list);
extern void func_0204d9a0(List *list, Node *node);
extern void func_0204da4c(Vector3 *out);
extern void func_0204d9f0(Vector3 *out);
extern void func_0204cd5c(int *out, int *in, int sel, s16 *m);
extern void NormalizeVec3(Vector3 *src, Vector3 *dst);

extern u32 data_020a4d30;
extern s16 data_02082214[];

static inline u32 RandVal(void)
{
    u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    return s;
}

static inline int RandScale(void)
{
    return (int)(((RandVal() >> 0x17) << 0xc) - 0x100000) >> 8;
}

static inline int FMul(int a, int b)
{
    return (int)(((s64)a * b + 0x800) >> 12);
}

void func_0204c584(u8 *self, List *list)
{
    int i;
    int count;
    int mag1;
    int mag2;
    int d10;
    int d14;
    int d18;
    int d1c;
    int d20;
    int d24;
    int d28;
    u16 arr[3];
    Vector3 vecB;
    Vector3 vecC;
    Vector3 vecE;
    Vector3 nrm;
    u8 *emitter;
    Def *def;
    u8 *p;
    int acc;

    emitter = *(u8 **)(self + 0x18);
    def = *(Def **)emitter;
    acc = def->rate + *(s16 *)(self + 0x3a);
    *(s16 *)(self + 0x3a) = acc & 0xfff;
    count = acc >> 0xc;
    {
        u32 t = def->mode;
        if ((t == 2 || t == 3 || t == 5) && def->sel == 3)
            func_0204cebc(self);
    }
    i = 0;
    if (count <= 0)
        return;
    d10 = 0;
    d18 = 0;
    d14 = 0;
    d1c = 0x1000;
    d20 = i;
    d24 = i;
    d28 = i;
    do {
        p = (u8 *)func_0204d958(list);
        if (p == 0)
            return;
        func_0204d9a0((List *)(self + 8), (Node *)p);
        switch (def->mode) {
        case 0:
            *(int *)(p + 0x1c) = d14;
            *(int *)(p + 0x18) = *(int *)(p + 0x1c);
            *(int *)(p + 0x14) = *(int *)(p + 0x18);
            break;
        case 1:
            func_0204da4c((Vector3 *)(p + 0x14));
            *(int *)(p + 0x14) = (int)(((s64)*(int *)(p + 0x14) * *(int *)(self + 0x44) + 0x800) >> 12);
            *(int *)(p + 0x18) = (int)(((s64)*(int *)(p + 0x18) * *(int *)(self + 0x44) + 0x800) >> 12);
            *(int *)(p + 0x1c) = (int)(((s64)*(int *)(p + 0x1c) * *(int *)(self + 0x44) + 0x800) >> 12);
            break;
        case 2:
            func_0204d9f0(&vecB);
            vecB.x = (int)(((s64)vecB.x * *(int *)(self + 0x44) + 0x800) >> 12);
            vecB.y = (int)(((s64)vecB.y * *(int *)(self + 0x44) + 0x800) >> 12);
            func_0204cd5c((int *)(p + 0x14), (int *)&vecB, def->sel, (s16 *)self);
            break;
        case 3: {
            int q = d10 / count;
            int idx;
            s16 c, sn;
            d10 += 0x10000;
            idx = (q >> 4) << 1;
            c = data_02082214[idx];
            sn = data_02082214[idx + 1];
            vecC.x = FMul(c, *(int *)(self + 0x44));
            vecC.y = FMul(sn, *(int *)(self + 0x44));
            vecC.z = d18;
            func_0204cd5c((int *)(p + 0x14), (int *)&vecC, def->sel, (s16 *)self);
            break;
        }
        case 4:
            func_0204da4c((Vector3 *)(p + 0x14));
            *(int *)(p + 0x14) = FMul(FMul(*(int *)(p + 0x14), *(int *)(self + 0x44)), RandScale());
            *(int *)(p + 0x18) = FMul(FMul(*(int *)(p + 0x18), *(int *)(self + 0x44)), RandScale());
            *(int *)(p + 0x1c) = FMul(FMul(*(int *)(p + 0x1c), *(int *)(self + 0x44)), RandScale());
            break;
        case 5:
            func_0204d9f0(&vecE);
            vecE.x = FMul(FMul(vecE.x, *(int *)(self + 0x44)), RandScale());
            vecE.y = FMul(FMul(vecE.y, *(int *)(self + 0x44)), RandScale());
            func_0204cd5c((int *)(p + 0x14), (int *)&vecE, def->sel, (s16 *)self);
            break;
        }
        {
            u32 s;
            int b;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->b2e;
            mag1 = (*(int *)(self + 0x48) * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->b2e;
            mag2 = (*(int *)(self + 0x4c) * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        if (*(int *)(p + 0x14) == 0 && *(int *)(p + 0x18) == 0 && *(int *)(p + 0x1c) == 0)
            func_0204da4c(&nrm);
        else
            NormalizeVec3((Vector3 *)(p + 0x14), &nrm);
        *(int *)(p + 0x20) = (nrm.x * mag1 + *(s16 *)(self + 0x3c) * mag2) >> 0xc;
        *(int *)(p + 0x24) = (nrm.y * mag1 + *(s16 *)(self + 0x3e) * mag2) >> 0xc;
        *(int *)(p + 0x28) = (nrm.z * mag1 + *(s16 *)(self + 0x40) * mag2) >> 0xc;
        *(Vector3 *)(p + 8) = *(Vector3 *)(self + 0x20);
        {
            u32 s;
            int b;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->b2c;
            *(int *)(p + 0x30) = (*(int *)(self + 0x50) * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        *(u16 *)(p + 0x34) = (u16)d1c;
        if (def->texanim && (*(TexAnim **)(emitter + 8))->flag) {
            u32 s;
            u8 *tex;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            tex = *(u8 **)(emitter + 8);
            arr[0] = *(u16 *)tex;
            arr[1] = def->tex;
            arr[2] = *(u16 *)(tex + 2);
            *(u16 *)(p + 0x3a) = arr[(s >> 0x14) % 3u];
        } else {
            *(u16 *)(p + 0x3a) = def->tex;
        }
        {
            u16 *q = (u16 *)(((s64)(int)(p + 0x40)) & 0xFFFFFFFFFFFFFFFFLL);
            *q = (u16)((*q & ~0x1f) | (*(u8 *)(self + 0x59) & 0x1f));
            *q = (u16)((*q & ~0x3e0) | 0x3e0);
        }
        if (def->anglerand) {
            u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            *(u16 *)(p + 0x36) = (u16)s;
        } else {
            *(u16 *)(p + 0x36) = (u16)d20;
        }
        if (def->sizerand) {
            *(u16 *)(p + 0x38) = (u16)((u32)((def->hi - def->lo) * (int)(RandVal() >> 0x14) + (def->lo << 0xc)) >> 0xc);
        } else {
            *(u16 *)(p + 0x38) = (u16)d24;
        }
        {
            u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            *(u16 *)(p + 0x2c) = (u16)(((*(u16 *)(self + 0x54) * (0xff - ((def->b2d * (int)(s >> 0x18)) >> 8))) >> 8) + 1);
        }
        *(u16 *)(p + 0x2e) = (u16)d28;
        {
            u32 b11 = def->colorrand;
            if (b11 != 0 && ((Pal *)*(u8 **)(emitter + 0x10))->loop) {
                u32 s;
                u8 *pal;
                s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
                data_020a4d30 = s;
                pal = *(u8 **)(emitter + 0x10);
                *(p + 0x42) = pal[(s >> 0x14) % pal[8]];
            } else if (b11 != 0 && !((Pal *)*(u8 **)(emitter + 0x10))->loop) {
                *(p + 0x42) = **(u8 **)(emitter + 0x10);
            } else {
                *(p + 0x42) = def->b33;
            }
        }
        *(u16 *)(p + 0x3c) = (u16)(0xffff / (*(Def **)emitter)->b34);
        *(u16 *)(p + 0x3e) = (u16)(0xffff / *(u16 *)(p + 0x2c));
        i += 1;
    } while (i < count);
}
