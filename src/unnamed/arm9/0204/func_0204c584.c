#include "types.h"
#include "nitro/fx/fx.h"

/* File-local view of Particle::System (include/Particle__System.h is C++):
   the direction and the two axes perpendicular to it. */
typedef struct {
    u8 pad_000[0x3c];
    Vector3s direction; /* 0x3c */
    u8 pad_042[0x1a];
    Vector3s basisU; /* 0x5c */
    Vector3s basisV; /* 0x62 */
} ParticleSystemView;

typedef struct Node {
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct List {
    Node *head;
    int count;
} List;

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
    int rate;       /* 0x4 */
    u8 pad8[0xa];   /* 0x8 */
    u16 tex;        /* 0x12 */
    u8 pad14[0x10]; /* 0x14 */
    s16 lo;         /* 0x24 */
    s16 hi;         /* 0x26 */
    u8 pad28[4];    /* 0x28 */
    u8 b2c;         /* 0x2c */
    u8 b2d;         /* 0x2d */
    u8 b2e;         /* 0x2e */
    u8 pad2f[4];    /* 0x2f */
    u8 b33;         /* 0x33 */
    u8 b34;         /* 0x34 */
} Def;

typedef struct TexAnim {
    u16 a;        /* 0x0 */
    u16 b;        /* 0x2 */
    u8 pad[4];    /* 0x4 */
    u16 flag : 1; /* 0x8 bit 0 */
    u16 frest : 15;
} TexAnim;

typedef struct Pal {
    u8 colors[8]; /* 0x0 */
    u32 cnt : 8;  /* 0x8 bits 0-7 */
    u32 px : 8;   /* bits 8-15 */
    u32 loop : 1; /* bit 16 */
    u32 prest : 15;
} Pal;

/* Particle::SystemDefinition (include/Particle__Manager.h): `data` there.
 * texAnim and palette are read only when data's texanim / colorrand bits are
 * set, and supply the spawned particle's texture and colour. */
typedef struct SystemDefinition {
    Def *data; /* 0x00 */
    u8 pad_04[4];
    TexAnim *texAnim; /* 0x08 */
    u8 pad_0c[4];
    Pal *palette; /* 0x10 */
} SystemDefinition;

/* Particle::System (include/Particle__System.h), whose names this reuses.
 * Here callbackValue is the spawn-rate accumulator (data->rate is added each
 * call, whole units spawn, the 12-bit fraction stays). emitRadius scales the
 * spawn offset of every emitter shape. particleLifetime seeds each particle's
 * lifetime. The byte at 0x59 (into the low five bits of the particle's 0x40
 * halfword) has no established role and stays raw. */
typedef struct ParticleSystem {
    u8 pad_00[8];
    List particles; /* 0x08 */
    u8 pad_10[8];
    SystemDefinition *mDefinition; /* 0x18 */
    u8 pad_1c[4];
    Vector3 position; /* 0x20 */
    u8 pad_2c[0xe];
    s16 callbackValue; /* 0x3a */
    s16 direction[3];  /* 0x3c */
    u8 pad_42[2];
    int emitRadius;       /* 0x44 */
    int callbackParam;    /* 0x48 */
    int callbackVelocity; /* 0x4c */
    int callbackScale;    /* 0x50 */
    u16 particleLifetime; /* 0x54 */
} ParticleSystem;

/* Particle::Element (include/Particle__Element.h) and the fields past its
 * 0x30 bytes that this spawn fills in. scale is seeded from the system's
 * callbackScale, angle from the anglerand bit, texture and color as the
 * SystemDefinition note says, and invLifetime is 0xffff / lifetime.
 * The halfwords at 0x34, 0x38, 0x3c and 0x40 stay raw: this file only
 * seeds them, so their roles are not established here. */
typedef struct ParticleElement {
    Node link;            /* 0x00 */
    Vector3 basePosition; /* 0x08 */
    Vector3 offset;       /* 0x14 */
    Vector3 velocity;     /* 0x20 */
    u16 lifetime;         /* 0x2c */
    u16 age;              /* 0x2e */
    int scale;            /* 0x30 */
    u8 pad_34[2];
    u16 angle; /* 0x36 */
    u8 pad_38[2];
    u16 texture; /* 0x3a */
    u8 pad_3c[2];
    u16 invLifetime; /* 0x3e */
    u8 pad_40[2];
    u8 color; /* 0x42 */
} ParticleElement;

extern Vector3s data_02099fc4;
extern void func_02053380(void *a, Vector3s *b);
extern Node *func_0204d958(List *list);
extern void func_0204d9a0(List *list, Node *node);
extern void func_0204da4c(Vector3 *out);
extern void func_0204d9f0(Vector3 *out);
extern void NormalizeVec3(int *v, int *out); /* as defined; the call casts */
extern u32 data_020a4d30;
extern s16 data_02082214[];

static inline u32 RandVal(void) {
    u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    return s;
}

static inline int RandScale(void) {
    return (int)(((RandVal() >> 0x17) << 0xc) - 0x100000) >> 8;
}

static inline int FMul(int a, int b) {
    return FX_MUL(a, b);
}

void func_0204cebc(void *c) {
    ParticleSystemView *sys = (ParticleSystemView *)c;
    Vector3s vecA;
    Vector3s vecB;

    vecA = data_02099fc4;
    func_02053380(&sys->direction, &vecB);

    if (vecA.x == vecB.x && vecA.y == vecB.y && vecA.z == vecB.z) {
        vecA.x = 0x1000;
        vecA.y = 0;
        vecA.z = 0;
    }

    sys->basisU.x = (s16)(FX_MUL(vecB.y, vecA.z) - FX_MUL(vecB.z, vecA.y));
    sys->basisU.y = (s16)(FX_MUL(vecB.z, vecA.x) - FX_MUL(vecB.x, vecA.z));
    sys->basisU.z = (s16)(FX_MUL(vecB.x, vecA.y) - FX_MUL(vecB.y, vecA.x));

    sys->basisV.x = (s16)(FX_MUL(vecB.y, sys->basisU.z) - FX_MUL(vecB.z, sys->basisU.y));
    sys->basisV.y = (s16)(FX_MUL(vecB.z, sys->basisU.x) - FX_MUL(vecB.x, sys->basisU.z));
    sys->basisV.z = (s16)(FX_MUL(vecB.x, sys->basisU.y) - FX_MUL(vecB.y, sys->basisU.x));
}

void func_0204cd5c(int *out, int *in, int sel, s16 *m) {
    switch (sel) {
    case 2:
        out[0] = 0;
        out[1] = in[0];
        out[2] = in[1];
        break;
    case 1:
        out[0] = in[1];
        out[1] = 0;
        out[2] = in[0];
        break;
    case 0:
        out[0] = in[0];
        out[1] = in[1];
        out[2] = 0;
        break;
    case 3:
        out[0] = FX_MUL(in[0], m[0x2e]) + FX_MUL(in[1], m[0x31]);
        out[1] = FX_MUL(in[0], m[0x2f]) + FX_MUL(in[1], m[0x32]);
        out[2] = FX_MUL(in[0], m[0x30]) + FX_MUL(in[1], m[0x33]);
        break;
    }
}

void func_0204c584(u8 *self, List *list) {
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
    SystemDefinition *emitter;
    Def *def;
    ParticleElement *p;
    int acc;
    ParticleSystem *sys = (ParticleSystem *)self;

    emitter = sys->mDefinition;
    def = emitter->data;
    acc = def->rate + sys->callbackValue;
    sys->callbackValue = acc & 0xfff;
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
        p = (ParticleElement *)func_0204d958(list);
        if (p == 0)
            return;
        func_0204d9a0(&sys->particles, (Node *)p);
        switch (def->mode) {
        case 0:
            p->offset.z = d14;
            p->offset.y = p->offset.z;
            p->offset.x = p->offset.y;
            break;
        case 1:
            func_0204da4c(&p->offset);
            p->offset.x = FX_MUL(p->offset.x, sys->emitRadius);
            p->offset.y = FX_MUL(p->offset.y, sys->emitRadius);
            p->offset.z = FX_MUL(p->offset.z, sys->emitRadius);
            break;
        case 2:
            func_0204d9f0(&vecB);
            vecB.x = FX_MUL(vecB.x, sys->emitRadius);
            vecB.y = FX_MUL(vecB.y, sys->emitRadius);
            func_0204cd5c((int *)&p->offset, (int *)&vecB, def->sel, (s16 *)self);
            break;
        case 3: {
            int q = d10 / count;
            int idx;
            s16 c, sn;
            d10 += 0x10000;
            idx = (q >> 4) << 1;
            c = data_02082214[idx];
            sn = data_02082214[idx + 1];
            vecC.x = FMul(c, sys->emitRadius);
            vecC.y = FMul(sn, sys->emitRadius);
            vecC.z = d18;
            func_0204cd5c((int *)&p->offset, (int *)&vecC, def->sel, (s16 *)self);
            break;
        }
        case 4:
            func_0204da4c(&p->offset);
            p->offset.x = FMul(FMul(p->offset.x, sys->emitRadius), RandScale());
            p->offset.y = FMul(FMul(p->offset.y, sys->emitRadius), RandScale());
            p->offset.z = FMul(FMul(p->offset.z, sys->emitRadius), RandScale());
            break;
        case 5:
            func_0204d9f0(&vecE);
            vecE.x = FMul(FMul(vecE.x, sys->emitRadius), RandScale());
            vecE.y = FMul(FMul(vecE.y, sys->emitRadius), RandScale());
            func_0204cd5c((int *)&p->offset, (int *)&vecE, def->sel, (s16 *)self);
            break;
        }
        {
            u32 s;
            int b;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->b2e;
            mag1 = (sys->callbackParam * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->b2e;
            mag2 = (sys->callbackVelocity * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        if (p->offset.x == 0 && p->offset.y == 0 && p->offset.z == 0)
            func_0204da4c(&nrm);
        else
            NormalizeVec3((int *)&p->offset, (int *)&nrm);
        p->velocity.x = (nrm.x * mag1 + sys->direction[0] * mag2) >> 0xc;
        p->velocity.y = (nrm.y * mag1 + sys->direction[1] * mag2) >> 0xc;
        p->velocity.z = (nrm.z * mag1 + sys->direction[2] * mag2) >> 0xc;
        p->basePosition = sys->position;
        {
            u32 s;
            int b;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->b2c;
            p->scale = (sys->callbackScale * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        *(u16 *)((u8 *)p + 0x34) = (u16)d1c;
        if (def->texanim && emitter->texAnim->flag) {
            u32 s;
            TexAnim *tex;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            tex = emitter->texAnim;
            arr[0] = tex->a;
            arr[1] = def->tex;
            arr[2] = tex->b;
            p->texture = arr[(s >> 0x14) % 3u];
        } else {
            p->texture = def->tex;
        }
        {
            u16 *q = (u16 *)((u8 *)p + 0x40);
            *q = (u16)((*q & ~0x1f) | (*(u8 *)(self + 0x59) & 0x1f));
            *q = (u16)((*q & ~0x3e0) | 0x3e0);
        }
        if (def->anglerand) {
            u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            p->angle = (u16)s;
        } else {
            p->angle = (u16)d20;
        }
        if (def->sizerand) {
            *(u16 *)((u8 *)p + 0x38) =
                (u16)((u32)((def->hi - def->lo) * (int)(RandVal() >> 0x14) + (def->lo << 0xc)) >>
                      0xc);
        } else {
            *(u16 *)((u8 *)p + 0x38) = (u16)d24;
        }
        {
            u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            p->lifetime =
                (u16)(((sys->particleLifetime * (0xff - ((def->b2d * (int)(s >> 0x18)) >> 8))) >>
                       8) +
                      1);
        }
        p->age = (u16)d28;
        {
            u32 b11 = def->colorrand;
            if (b11 != 0 && emitter->palette->loop) {
                u32 s;
                u8 *pal;
                s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
                data_020a4d30 = s;
                pal = (u8 *)emitter->palette;
                p->color = pal[(s >> 0x14) % pal[8]];
            } else if (b11 != 0 && !emitter->palette->loop) {
                p->color = emitter->palette->colors[0];
            } else {
                p->color = def->b33;
            }
        }
        *(u16 *)((u8 *)p + 0x3c) = (u16)(0xffff / emitter->data->b34);
        p->invLifetime = (u16)(0xffff / p->lifetime);
        i += 1;
    } while (i < count);
}
