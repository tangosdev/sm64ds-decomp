#include "types.h"
#include "nitro/fx/fx.h"
#include "private/spl_particle_views.h"

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

/* The color animation (func_0204d294's ColorRamp; SPL's SPLColorAnim). */
typedef struct ColorRamp {
    u16 startColor;           /* 0x0 */
    u16 endColor;             /* 0x2 */
    u8 pad[4];                /* 0x4 */
    u16 randomStartColor : 1; /* 0x8 bit 0 */
    u16 frest : 15;
} ColorRamp;

/* The texture animation (func_0204d150's TexAnim; SPL's SPLTexAnim). */
typedef struct TexAnim {
    u8 textures[8];        /* 0x0 */
    u32 frameCount : 8;    /* 0x8 bits 0-7 */
    u32 step : 8;          /* bits 8-15 */
    u32 randomizeInit : 1; /* bit 16 */
    u32 prest : 15;
} TexAnim;

extern Vector3s data_02099fc4;
extern void func_02053380(void *a, Vector3s *b);
extern Node *func_0204d958(List *list);
extern void func_0204d9a0(List *list, Node *node);
extern void func_0204da4c(Vector3 *out);
extern void func_0204d9f0(Vector3 *out);
struct Vector3;
extern void NormalizeVec3(const struct Vector3 *v, struct Vector3 *out);
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

void func_0204cd5c(int *out, int *in, int sel, ParticleSystemView *sys) {
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
        out[0] = FX_MUL(in[0], sys->basisU.x) + FX_MUL(in[1], sys->basisV.x);
        out[1] = FX_MUL(in[0], sys->basisU.y) + FX_MUL(in[1], sys->basisV.y);
        out[2] = FX_MUL(in[0], sys->basisU.z) + FX_MUL(in[1], sys->basisV.z);
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
    PtclDef *emitter;
    PtclData *def;
    ParticleElement *p;
    int acc;
    ParticleSystem *sys = (ParticleSystem *)self;

    emitter = sys->mDefinition;
    def = emitter->data;
    acc = def->rate + sys->callbackValue;
    sys->callbackValue = acc & 0xfff;
    count = acc >> 0xc;
    {
        u32 t = def->emissionType;
        if ((t == 2 || t == 3 || t == 5) && def->circleAxis == 3)
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
        func_0204d9a0((List *)&sys->particles, (Node *)p);
        switch (def->emissionType) {
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
            func_0204cd5c((int *)&p->offset, (int *)&vecB, def->circleAxis,
                          (ParticleSystemView *)self);
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
            func_0204cd5c((int *)&p->offset, (int *)&vecC, def->circleAxis,
                          (ParticleSystemView *)self);
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
            func_0204cd5c((int *)&p->offset, (int *)&vecE, def->circleAxis,
                          (ParticleSystemView *)self);
            break;
        }
        {
            u32 s;
            int b;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->velocityRandom;
            mag1 = (sys->callbackParam * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->velocityRandom;
            mag2 = (sys->callbackVelocity * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        if (p->offset.x == 0 && p->offset.y == 0 && p->offset.z == 0)
            func_0204da4c(&nrm);
        else
            NormalizeVec3((const struct Vector3 *)&p->offset, (struct Vector3 *)&nrm);
        p->velocity.x = (nrm.x * mag1 + sys->direction[0] * mag2) >> 0xc;
        p->velocity.y = (nrm.y * mag1 + sys->direction[1] * mag2) >> 0xc;
        p->velocity.z = (nrm.z * mag1 + sys->direction[2] * mag2) >> 0xc;
        p->basePosition = sys->position;
        {
            u32 s;
            int b;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            b = def->scaleRandom;
            p->scale = (sys->callbackScale * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        p->animScale = (s16)d1c;
        if (def->hasColorAnim && emitter->colorAnim->randomStartColor) {
            u32 s;
            ColorRamp *anim;
            s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            anim = emitter->colorAnim;
            arr[0] = anim->startColor;
            arr[1] = def->color;
            arr[2] = anim->endColor;
            p->color = arr[(s >> 0x14) % 3u];
        } else {
            p->color = def->color;
        }
        {
            u16 *q = (u16 *)&p->visibility;
            *q = (u16)((*q & ~0x1f) | (sys->baseAlpha & 0x1f));
            *q = (u16)((*q & ~0x3e0) | 0x3e0);
        }
        if (def->randomInitAngle) {
            u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            p->angle = (u16)s;
        } else {
            p->angle = (u16)d20;
        }
        if (def->hasRotation) {
            p->angularSpeed =
                (s16)((u32)((def->maxRotation - def->minRotation) * (int)(RandVal() >> 0x14) +
                            (def->minRotation << 0xc)) >>
                      0xc);
        } else {
            p->angularSpeed = (s16)d24;
        }
        {
            u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            p->lifetime = (u16)(((sys->particleLifetime *
                                  (0xff - ((def->lifetimeRandom * (int)(s >> 0x18)) >> 8))) >>
                                 8) +
                                1);
        }
        p->age = (u16)d28;
        {
            u32 hasTexAnim = def->hasTexAnim;
            if (hasTexAnim != 0 && emitter->texAnim->randomizeInit) {
                u32 s;
                TexAnim *anim;
                s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
                data_020a4d30 = s;
                anim = emitter->texAnim;
                p->texture = anim->textures[(s >> 0x14) % anim->frameCount];
            } else if (hasTexAnim != 0 && !emitter->texAnim->randomizeInit) {
                p->texture = emitter->texAnim->textures[0];
            } else {
                p->texture = def->texture;
            }
        }
        p->loopTimeFactor = (u16)(0xffff / emitter->data->loopFrames);
        p->lifeRateFactor = (u16)(0xffff / p->lifetime);
        i += 1;
    } while (i < count);
}
