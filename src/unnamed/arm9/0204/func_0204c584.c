#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"

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
extern void VEC_Fx16Normalize(void *a, Vector3s *b);
extern Node *func_0204d958(List *list);
extern void func_0204d9a0(List *list, Node *node);
extern void SPLRandom_VecFx32(Vector3 *out);
extern void SPLRandom_VecFx32_XY(Vector3 *out);
struct Vector3;
extern void VEC_Normalize(const struct Vector3 *v, struct Vector3 *out);
extern u32 data_020a4d30;

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

void func_0204cebc(ParticleSystem *sys) {
    Vector3s vecA;
    Vector3s vecB;

    vecA = data_02099fc4;
    VEC_Fx16Normalize(sys->direction, &vecB);

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

void func_0204cd5c(int *out, int *in, int sel, ParticleSystem *sys) {
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

void func_0204c584(ParticleSystem *sys, List *list) {
    int i;
    int count;
    int mag1;
    int mag2;
    int ringAngle;
    u16 arr[3];
    Vector3 vecB;
    Vector3 vecC;
    Vector3 vecE;
    Vector3 nrm;
    PtclDef *emitter;
    PtclData *def;
    ParticleElement *p;
    int acc;

    emitter = sys->mDefinition;
    def = emitter->data;
    acc = def->rate + sys->callbackValue;
    sys->callbackValue = acc & 0xfff;
    count = acc >> 0xc;
    {
        u32 t = def->emissionType;
        if ((t == 2 || t == 3 || t == 5) && def->circleAxis == 3)
            func_0204cebc(sys);
    }
    i = 0;
    if (count <= 0)
        return;
    ringAngle = 0;
    do {
        p = (ParticleElement *)func_0204d958(list);
        if (p == 0)
            return;
        func_0204d9a0((List *)&sys->particles, (Node *)p);
        switch (def->emissionType) {
        case 0:
            p->offset.x = p->offset.y = p->offset.z = 0;
            break;
        case 1:
            SPLRandom_VecFx32(&p->offset);
            p->offset.x = FX_MUL(p->offset.x, sys->emitRadius);
            p->offset.y = FX_MUL(p->offset.y, sys->emitRadius);
            p->offset.z = FX_MUL(p->offset.z, sys->emitRadius);
            break;
        case 2:
            SPLRandom_VecFx32_XY(&vecB);
            vecB.x = FX_MUL(vecB.x, sys->emitRadius);
            vecB.y = FX_MUL(vecB.y, sys->emitRadius);
            func_0204cd5c((int *)&p->offset, (int *)&vecB, def->circleAxis, sys);
            break;
        case 3: {
            int q = ringAngle / count;
            fx16 s, c;
            ringAngle += 0x10000;
            s = FX_SinIdx(q >> 4);
            c = FX_CosIdx(q >> 4);
            vecC.x = FMul(s, sys->emitRadius);
            vecC.y = FMul(c, sys->emitRadius);
            vecC.z = 0;
            func_0204cd5c((int *)&p->offset, (int *)&vecC, def->circleAxis, sys);
            break;
        }
        case 4:
            SPLRandom_VecFx32(&p->offset);
            p->offset.x = FMul(FMul(p->offset.x, sys->emitRadius), RandScale());
            p->offset.y = FMul(FMul(p->offset.y, sys->emitRadius), RandScale());
            p->offset.z = FMul(FMul(p->offset.z, sys->emitRadius), RandScale());
            break;
        case 5:
            SPLRandom_VecFx32_XY(&vecE);
            vecE.x = FMul(FMul(vecE.x, sys->emitRadius), RandScale());
            vecE.y = FMul(FMul(vecE.y, sys->emitRadius), RandScale());
            func_0204cd5c((int *)&p->offset, (int *)&vecE, def->circleAxis, sys);
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
            SPLRandom_VecFx32(&nrm);
        else
            VEC_Normalize(&p->offset, &nrm);
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
        p->animScale = 0x1000;
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
            p->angle = 0;
        }
        if (def->hasRotation) {
            p->angularSpeed =
                (s16)((u32)((def->maxRotation - def->minRotation) * (int)(RandVal() >> 0x14) +
                            (def->minRotation << 0xc)) >>
                      0xc);
        } else {
            p->angularSpeed = 0;
        }
        {
            u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
            data_020a4d30 = s;
            p->lifetime = (u16)(((sys->particleLifetime *
                                  (0xff - ((def->lifetimeRandom * (int)(s >> 0x18)) >> 8))) >>
                                 8) +
                                1);
        }
        p->age = 0;
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
