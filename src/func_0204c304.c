typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;

typedef struct Vec3 { int x, y, z; } Vec3;

typedef struct Node { struct Node *next; struct Node *prev; } Node;
typedef struct List { Node *head; int count; } List;

typedef struct Bf40 {
    u16 f0 : 5;
    u16 f5 : 5;
    u16 rest : 6;
} Bf40;

typedef struct Particle {
    u8 pad0[8];   /* 0x0 */
    Vec3 v8;      /* 0x8 */
    Vec3 v14;     /* 0x14 */
    int i20;      /* 0x20 */
    int i24;      /* 0x24 */
    int i28;      /* 0x28 */
    u16 h2c;      /* 0x2c */
    u16 h2e;      /* 0x2e */
    int i30;      /* 0x30 */
    s16 sh34;     /* 0x34 */
    u16 h36;      /* 0x36 */
    s16 sh38;     /* 0x38 */
    u16 h3a;      /* 0x3a */
    u16 h3c;      /* 0x3c */
    u16 h3e;      /* 0x3e */
    Bf40 bf40;    /* 0x40 */
    u8 b42;       /* 0x42 */
} Particle;

typedef struct DefFlags {
    u16 pad0 : 3; /* bits 0-2 */
    u16 mode : 2; /* bits 3-4 */
    u16 pad5 : 1; /* bit 5 */
    u16 bit6 : 1; /* bit 6 */
    u16 pad7 : 9; /* bits 7-15 */
} DefFlags;

typedef struct Def {
    DefFlags flags; /* 0x0 */
    s16 spread;     /* 0x2 */
    u8 pad4[2];     /* 0x4 */
    u16 f6;         /* 0x6 */
    u8 level;       /* 0x8 */
    u8 f9;          /* 0x9 */
    u16 fa;         /* 0xa */
    u8 count;       /* 0xc */
    u8 padd[2];     /* 0xd */
    u8 ff;          /* 0xf */
} Def;

extern u32 data_020a4d30;

extern Node *func_0204d958(List *list);
extern void func_0204d9a0(List *list, Node *node);

void func_0204c304(Particle *self, u8 *mgr, List *freelist)
{
    Particle *p;
    Def *def;
    int i;
    int eA, eB, eC, d1c;
    u32 rngA, rngC;

    def = *(Def **)(*(u8 **)(mgr + 0x18) + 0x14);
    i = 0;
    if (i >= def->count)
        return;

    eA = 0;
    eB = 0;
    eC = 0;
    d1c = 0x1000;
    rngA = 0x5eedf715u;
    rngC = 0x1b0cb173u;

    do {
        int lvl;

        p = (Particle *)func_0204d958(freelist);
        if (p == 0)
            return;

        func_0204d9a0((List *)(mgr + 0x10), (Node *)p);

        p->v14 = self->v14;

        lvl = def->level + 1;

        data_020a4d30 = data_020a4d30 * rngA + rngC;
        p->i20 = ((self->i20 * lvl) >> 8) + ((def->spread * (int)(data_020a4d30 >> 0x17) - (def->spread << 8)) >> 8);

        data_020a4d30 = data_020a4d30 * rngA + rngC;
        p->i24 = ((self->i24 * lvl) >> 8) + ((def->spread * (int)(data_020a4d30 >> 0x17) - (def->spread << 8)) >> 8);

        data_020a4d30 = data_020a4d30 * rngA + rngC;
        p->i28 = ((self->i28 * lvl) >> 8) + ((def->spread * (int)(data_020a4d30 >> 0x17) - (def->spread << 8)) >> 8);

        p->v8 = self->v8;

        { int c = *(volatile int *)&self->i30; int m = (c * self->sh34) >> 12; p->i30 = (m * (def->f9 + 1)) >> 6; }

        p->sh34 = (u16)d1c;

        if (def->flags.bit6)
            p->h3a = def->fa;
        else
            p->h3a = self->h3a;

        {
            Bf40 *q = (Bf40 *)(((s64)(int)((u8 *)p + 0x40)) & 0xFFFFFFFFFFFFFFFFLL);
            q->f0 = (u16)((self->bf40.f0 * (self->bf40.f5 + 1)) >> 5);
            q->f5 = 0x1f;
        }

        {
        int mode = def->flags.mode;
        if (mode == 0)
            goto mode0;
        if (mode == 1)
            goto mode1;
        if (mode == 2) {
            p->h36 = self->h36;
            p->sh38 = self->sh38;
        }
        goto modedone;
    mode0:
        p->h36 = (u16)eB;
        p->sh38 = (s16)eB;
        goto modedone;
    mode1:
        p->h36 = self->h36;
        p->sh38 = (s16)eA;
    modedone:
        ;
        }

        p->h2c = def->f6;
        p->h2e = (u16)eC;
        p->b42 = def->ff;
        p->h3c = 0xffff / (int)((u32)self->h2c >> 1);
        p->h3e = 0xffff / self->h2c;

        i += 1;
    } while (i < def->count);
}
