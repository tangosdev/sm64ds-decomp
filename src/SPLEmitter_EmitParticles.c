#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/fx/fx_vec.h"
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

extern VecFx16 sUpVector;
extern Node *SPLList_PopFront(List *list);
extern void SPLList_PushFront(List *list, Node *node);
extern void SPLRandom_VecFx32(VecFx32 *out);
extern void SPLRandom_VecFx32_XY(VecFx32 *out);
extern u32 gSPLRandomState;

static inline u32 RandVal(void) {
    u32 s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
    gSPLRandomState = s;
    return s;
}

static inline int RandScale(void) {
    return (int)(((RandVal() >> 0x17) << 12) - 0x100000) >> 8;
}

static inline int FMul(int a, int b) {
    return FX_MUL(a, b);
}

void SPLEmitter_ComputeOrthogonalAxes(ParticleSystem *sys) {
    VecFx16 vecA;
    VecFx16 vecB;

    vecA = sUpVector;
    VEC_Fx16Normalize(&sys->direction, &vecB);

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

void SPLUtil_TiltCoordinates(VecFx32 *out, VecFx32 *in, int sel, ParticleSystem *sys) {
    switch (sel) {
    case 2:
        out->x = 0;
        out->y = in->x;
        out->z = in->y;
        break;
    case 1:
        out->x = in->y;
        out->y = 0;
        out->z = in->x;
        break;
    case 0:
        out->x = in->x;
        out->y = in->y;
        out->z = 0;
        break;
    case 3:
        out->x = FX_MUL(in->x, sys->basisU.x) + FX_MUL(in->y, sys->basisV.x);
        out->y = FX_MUL(in->x, sys->basisU.y) + FX_MUL(in->y, sys->basisV.y);
        out->z = FX_MUL(in->x, sys->basisU.z) + FX_MUL(in->y, sys->basisV.z);
        break;
    }
}

void SPLEmitter_EmitParticles(ParticleSystem *sys, List *list) {
    int i;
    int count;
    int mag1;
    int mag2;
    int ringAngle;
    u16 arr[3];
    VecFx32 vecB;
    VecFx32 vecC;
    VecFx32 vecE;
    VecFx32 nrm;
    PtclDef *emitter;
    PtclData *def;
    ParticleElement *p;
    int acc;

    emitter = sys->mDefinition;
    def = emitter->data;
    acc = def->rate + sys->callbackValue;
    sys->callbackValue = acc & 0xfff;
    count = acc >> 12;
    {
        u32 t = def->emissionType;
        if ((t == 2 || t == 3 || t == 5) && def->circleAxis == 3)
            SPLEmitter_ComputeOrthogonalAxes(sys);
    }
    i = 0;
    if (count <= 0)
        return;
    ringAngle = 0;
    do {
        p = (ParticleElement *)SPLList_PopFront(list);
        if (p == 0)
            return;
        SPLList_PushFront((List *)&sys->particles, (Node *)p);
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
            SPLUtil_TiltCoordinates(&p->offset, &vecB, def->circleAxis, sys);
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
            SPLUtil_TiltCoordinates(&p->offset, &vecC, def->circleAxis, sys);
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
            SPLUtil_TiltCoordinates(&p->offset, &vecE, def->circleAxis, sys);
            break;
        }
        {
            u32 s;
            int b;
            s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
            gSPLRandomState = s;
            b = def->velocityRandom;
            mag1 = (sys->callbackParam * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
            s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
            gSPLRandomState = s;
            b = def->velocityRandom;
            mag2 = (sys->callbackVelocity * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        if (p->offset.x == 0 && p->offset.y == 0 && p->offset.z == 0)
            SPLRandom_VecFx32(&nrm);
        else
            VEC_Normalize(&p->offset, &nrm);
        p->velocity.x = (nrm.x * mag1 + sys->direction.x * mag2) >> 12;
        p->velocity.y = (nrm.y * mag1 + sys->direction.y * mag2) >> 12;
        p->velocity.z = (nrm.z * mag1 + sys->direction.z * mag2) >> 12;
        p->basePosition = sys->position;
        {
            u32 s;
            int b;
            s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
            gSPLRandomState = s;
            b = def->scaleRandom;
            p->scale = (sys->callbackScale * ((b + 0xff) - ((b * (int)(s >> 0x18)) >> 7))) >> 8;
        }
        p->animScale = 0x1000;
        if (def->hasColorAnim && emitter->colorAnim->randomStartColor) {
            u32 s;
            ColorRamp *anim;
            s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
            gSPLRandomState = s;
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
            u32 s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
            gSPLRandomState = s;
            p->angle = (u16)s;
        } else {
            p->angle = 0;
        }
        if (def->hasRotation) {
            p->angularSpeed =
                (s16)((u32)((def->maxRotation - def->minRotation) * (int)(RandVal() >> 0x14) +
                            (def->minRotation << 12)) >>
                      12);
        } else {
            p->angularSpeed = 0;
        }
        {
            u32 s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
            gSPLRandomState = s;
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
                s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
                gSPLRandomState = s;
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
