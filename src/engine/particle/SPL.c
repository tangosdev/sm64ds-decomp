/* SPL: the particle library core -- emitter setup and update, the draw routines,
 * the animation callbacks and the texture/plane utilities, folded from the
 * per-function shards in reverse ROM emission order. The names are pret's SPL's. */
#include "common.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_mtx33.h"
#include "nitro/fx/fx_mtx43.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/fx/fx_vec.h"
#include "nitro/gx/g3imm.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/mi/memory.h"
#include "private/spl_particle_views.h"
#include "types.h"

struct ParticleAlphaView {
    char pad0[0x40];
    u16 visibility; /* 0x40 */
};

/* ---- shared types, hoisted from the merged sources ---- */
typedef struct Node {
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct List {
    Node *head;
    int count;
} List;

/* Particle::System (include/Particle__System.h) as the child spawn reads it:
   the second particle list holds the children. */
typedef struct SystemListView {
    u8 pad_00[0x10];
    List children;        /* 0x10 */
    PtclDef *mDefinition; /* 0x18 */
} SystemListView;

struct TexParam {
    u32 fmt : 4;
    u32 sizeS : 4;
    u32 sizeT : 4;
    u32 repeat : 2;
    u32 flip : 2;
    u32 color0 : 1;
};

struct Tex {
    int f0;
    u32 texAddr;
    u32 pltAddr;
    struct TexParam param;
    u16 width;
    u16 height;
};

extern u32 gSPLRandomState;
extern VecFx16 sUpVector;
extern void (*sRotationFunctions[])(int, int, void *);
extern void (*sPlaneDrawingFunctions[])(u8, u8);
extern Node *SPLList_PopFront(List *list);
extern void SPLList_PushFront(List *list, Node *node);
extern Node *SPLList_Erase(List *listHead, Node *node);
extern void SPLRandom_VecFx32(VecFx32 *out);
extern void SPLRandom_VecFx32_XY(VecFx32 *out);
extern void MTX_Scale43_(Matrix4x3 *m, int sx, int sy, int sz);
extern void G3_LoadMtx43(void *m);
extern void G3_MultMtx43(void *m);

void SPLAnim_Scale(ParticleElement *ptcl, PtclDef *def, int lifeRate);
void SPLAnim_Color(ParticleElement *ptcl, PtclDef *data, int age);
void SPLAnim_Alpha(struct ParticleAlphaView *ptcl, PtclDef *def, int lifeRate);
void SPLAnim_Texture(ParticleElement *ptcl, PtclDef *def, int lifeRate);
void SPLAnim_ChildScale(ParticleElement *ptcl, PtclDef *def, int lifeRate);
void SPLAnim_ChildAlpha(struct ParticleAlphaView *ptcl, void *def, int lifeRate);
void SPLEmitter_EmitParticles(ParticleSystem *sys, List *list);
void SPLEmitter_EmitChildren(ParticleElement *self, SystemListView *sys, List *freelist);
void SPLUtil_DrawXYPlane(int tileS, int tileT);
void SPLUtil_DrawXZPlane(int tileS, int tileT);
void SPLUtil_RotateXYZ(int sin, int cos, MtxFx43 *mat);
void SPLUtil_RotateY(int sin, int cos, MtxFx43 *mat);
void SPLDraw_Billboard(ManagerView *mgr, ParticleElement *ptcl);
void SPLDraw_Child_Billboard(ManagerView *mgr, ParticleElement *ptcl);
void SPLDraw_DirectionalBillboard(ManagerView *mgr, ParticleElement *ptcl);
void SPLDraw_Child_DirectionalBillboard(ManagerView *mgr, ParticleElement *ptcl);
void SPLDraw_Polygon(ManagerView *mgr, ParticleElement *ptcl);
void SPLDraw_Child_Polygon(ManagerView *mgr, ParticleElement *ptcl);
void SPLUtil_SetTexture(struct Tex *self);
void SPLUtil_SetTexture_Stub(void *texture);
void SPLEmitter_Init(ParticleSystem *o, void *p18, struct Vector3 *src3);
void SPLEmitter_Update(void *manager, void *self);
void SPLManager_DoDraw(char *self);
void SPLEmitter_Emit(void *self, void *list);

#include "private/spl_particle_views.h"

/* The particle definition's scale animation (SPL's SPLScaleAnim): the scale
 * ramps start -> mid over life rates [0, in), holds mid until out, then ramps
 * mid -> end over [out, 255]. */
struct ScaleAnim {
    short start;       /* 0x0 */
    short mid;         /* 0x2 */
    short end;         /* 0x4 */
    unsigned char in;  /* 0x6 */
    unsigned char out; /* 0x7 */
};

/* The scale animation callback. The name is pret's SPL coinage, not a ROM
 * or SDK symbol. lifeRate is the particle's animation time, 0..255. */
void SPLAnim_Scale(ParticleElement *ptcl, PtclDef *def, int lifeRate) {
    struct ScaleAnim *anim = def->scaleAnim;
    unsigned char in = anim->in;
    unsigned char out = anim->out;
    if (lifeRate < in) {
        short start = anim->start;
        ptcl->animScale = start + lifeRate * (anim->mid - start) / in;
        return;
    }
    if (lifeRate < out) {
        ptcl->animScale = anim->mid;
        return;
    }
    {
        short end = anim->end;
        ptcl->animScale = end + (lifeRate - 255) * (end - anim->mid) / (255 - out);
    }
}

#include "types.h"
#include "private/spl_particle_views.h"
/* Three-stop RGB555 colour ramp entry (SPL's SPLColorAnim). */
struct ColorRamp {
    u16 startColor;
    u16 endColor;
    u8 stop1;
    u8 stop2;
    u8 stop3;
    u8 pad7;
    u16 flags;
};

/* The colour animation callback: the start colour, then a ramp to the
 * definition's base colour and on to the end colour, stepped or interpolated
 * by flags bit 2. The name is pret's SPL coinage, not a ROM or SDK symbol.
 * age is the particle's animation time, 0..255 (SPLEmitter_Update passes its life
 * rate, or its loop time when the ramp loops). */
void SPLAnim_Color(ParticleElement *ptcl, PtclDef *data, int age) {
    PtclData *header = data->data;
    struct ColorRamp *ramp = data->colorAnim;
    u8 stop1 = ramp->stop1;
    u8 stop2 = ramp->stop2;
    u8 stop3 = ramp->stop3;

    if (age < stop1) {
        ptcl->color = ramp->startColor;
        return;
    }
    if (age < stop2) {
        u16 hiColor = header->color;
        u16 loColor = ramp->startColor;
        u16 flags = ramp->flags;
        int redHi = hiColor & 0x1f;
        int redLo = loColor & 0x1f;
        int greenHiRaw = hiColor >> 5;
        int greenHi = greenHiRaw & 0x1f;
        int greenLoRaw = loColor >> 5;
        int greenLo = greenLoRaw & 0x1f;
        int blueHiRaw = hiColor >> 10;
        int blueHi = blueHiRaw & 0x1f;
        int blueLoRaw = loColor >> 10;
        int blueLo = blueLoRaw & 0x1f;
        int lerp = (((unsigned int)(flags << 29)) >> 31);
        int num = age - stop1;
        int denom = stop2 - stop1;
        if (lerp == 0)
            num = denom;
        int blueDiff = blueHi - blueLo;
        int blueMul = num * blueDiff;
        int blueStep = blueMul / denom;
        int redDiff = redHi - redLo;
        int redMul = num * redDiff;
        int redStep = redMul / denom;
        int greenDiff = greenHi - greenLo;
        int greenMul = num * greenDiff;
        int greenStep = greenMul / denom;
        int green = greenLo + greenStep;
        int red = redLo + redStep;
        int blue = blueLo + blueStep;
        ptcl->color = (red | (green << 5) | (blue << 10));
        return;
    }
    if (age < stop3) {
        u16 loColor = header->color;
        u16 hiColor = ramp->endColor;
        u16 flags = ramp->flags;
        int redLo = loColor & 0x1f;
        int redHi = hiColor & 0x1f;
        int greenLoRaw = loColor >> 5;
        int greenLo = greenLoRaw & 0x1f;
        int greenHiRaw = hiColor >> 5;
        int greenHi = greenHiRaw & 0x1f;
        int blueLoRaw = loColor >> 10;
        int blueLo = blueLoRaw & 0x1f;
        int blueHiRaw = hiColor >> 10;
        int blueHi = blueHiRaw & 0x1f;
        int lerp = (((unsigned int)(flags << 29)) >> 31);
        int num = age - stop2;
        int denom = stop3 - stop2;
        if (lerp == 0)
            num = denom;
        int blueDiff = blueHi - blueLo;
        int blueMul = num * blueDiff;
        int blueStep = blueMul / denom;
        int redDiff = redHi - redLo;
        int redMul = num * redDiff;
        int redStep = redMul / denom;
        int greenDiff = greenHi - greenLo;
        int greenMul = num * greenDiff;
        int greenStep = greenMul / denom;
        int green = greenLo + greenStep;
        int red = redLo + redStep;
        int blue = blueLo + blueStep;
        ptcl->color = (red | (green << 5) | (blue << 10));
        return;
    }
    ptcl->color = ramp->endColor;
}

#include "types.h"
#include "private/spl_particle_views.h"
extern u32 gSPLRandomState;

/* The particle's 0x40 halfword (SPL's visibility): the base alpha in bits 0-4,
 * the animated alpha in bits 5-9. This
 * callback rewrites the halfword whole, so it keeps this view rather than
 * spl_particle_views.h's ParticleElement, which spells it as bitfields. */

/* Three 5-bit alpha stops in one halfword. */
struct AlphaStops {
    u16 start : 5;
    u16 mid : 5;
    u16 end : 5;
    u16 pad : 1;
};

/* The particle definition's alpha animation (SPL's SPLAlphaAnim): the alpha
 * ramps start -> mid over life rates [0, in), holds mid until out, then ramps
 * mid -> end, and each step is scaled down by up to randomRange / 256. */
struct AlphaAnim {
    struct AlphaStops alpha; /* 0x0 */
    u8 randomRange;          /* 0x2 */
    u8 flags;                /* 0x3 */
    u8 in;                   /* 0x4 */
    u8 out;                  /* 0x5 */
};

/* The alpha animation callback. The name is pret's SPL coinage, not a ROM
 * or SDK symbol. gSPLRandomState is the particle code's random seed (the same
 * LCG as SPLEmitter_EmitParticles). */
void SPLAnim_Alpha(struct ParticleAlphaView *ptcl, PtclDef *def, int lifeRate) {
    struct AlphaAnim *anim = def->alphaAnim;
    int in = anim->in;
    int out = anim->out;
    int value;
    if (lifeRate < in) {
        int start = anim->alpha.start;
        int mid = anim->alpha.mid;
        int diff = mid - start;
        int step = lifeRate * diff / in;
        value = step + start;
    } else if (lifeRate < out) {
        value = anim->alpha.mid;
    } else {
        int mid = anim->alpha.mid;
        int end = anim->alpha.end;
        int diff = end - mid;
        int step = (lifeRate - 255) * diff / (255 - out);
        value = step + end;
    }
    u16 *visibility = &ptcl->visibility;
    u32 s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
    gSPLRandomState = s;
    int range = anim->randomRange;
    u16 old = *visibility;
    *visibility =
        ((old & ~0x3e0) |
         ((((u32)((value * (255 - ((range * (int)(s >> 0x18)) >> 8))) << 8) >> 0x10) & 0x1f) << 5));
}

#include "types.h"
#include "private/spl_particle_views.h"

/* The particle definition's texture animation (SPL's SPLTexAnim): up to eight
 * texture indices, each shown for `step` life-rate units. */
struct TexAnim {
    u8 textures[8]; /* 0x0 */
    u8 frameCount;  /* 0x8 */
    u8 step;        /* 0x9 */
};

/* The texture animation callback: picks the frame whose span holds
 * lifeRate. The name is pret's SPL coinage, not a ROM or SDK symbol. */
void SPLAnim_Texture(ParticleElement *ptcl, PtclDef *def, int lifeRate) {
    struct TexAnim *anim = def->texAnim;
    int i;
    for (i = 0; i < anim->frameCount; i++) {
        if (lifeRate < anim->step * (i + 1)) {
            ptcl->texture = anim->textures[i];
            return;
        }
    }
}

#include "private/spl_particle_views.h"

/* The child particles' scale callback: the scale runs from 1.0 at birth to
 * child->endScale at lifeRate 255. The name is pret's SPL coinage, not a ROM
 * or SDK symbol. */
void SPLAnim_ChildScale(ParticleElement *ptcl, PtclDef *def, int lifeRate) {
    int endScale;
    int delta;
    endScale = def->child->endScale;
    delta = (endScale - 0x1000) * (lifeRate - 255) / 255;
    ptcl->animScale = endScale + delta;
}

#include "types.h"

/* The particle's 0x40 halfword (SPL's visibility): the base alpha in bits 0-4
 * (SPLEmitter_EmitParticles seeds it), the animated alpha in bits 5-9. This
 * callback rewrites the halfword whole, so it keeps this view rather than
 * spl_particle_views.h's ParticleElement, which spells it as bitfields. */

/* The child particles' alpha callback: the animated alpha fades from 31 at
 * birth to 0 at lifeRate 255. The name is pret's SPL coinage, not a ROM or
 * SDK symbol. */
void SPLAnim_ChildAlpha(struct ParticleAlphaView *ptcl, void *def, int lifeRate) {
    u16 *visibility = &ptcl->visibility;
    u16 alpha = ((255 - lifeRate) * 31) / 255;
    *visibility = (*visibility & ~0x3e0) | ((alpha & 0x1f) << 5);
}

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/fx/fx_vec.h"
#include "private/spl_particle_views.h"

/* The color animation (func_0204d294's ColorRampInit; SPL's SPLColorAnim). */
typedef struct ColorRampInit {
    u16 startColor;           /* 0x0 */
    u16 endColor;             /* 0x2 */
    u8 pad[4];                /* 0x4 */
    u16 randomStartColor : 1; /* 0x8 bit 0 */
    u16 frest : 15;
} ColorRampInit;

/* The texture animation (func_0204d150's TexAnimInit; SPL's SPLTexAnim). */
typedef struct TexAnimInit {
    u8 textures[8];        /* 0x0 */
    u32 frameCount : 8;    /* 0x8 bits 0-7 */
    u32 step : 8;          /* bits 8-15 */
    u32 randomizeInit : 1; /* bit 16 */
    u32 prest : 15;
} TexAnimInit;

extern VecFx16 sUpVector;
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
        if (def->hasColorAnim && ((ColorRampInit *)emitter->colorAnim)->randomStartColor) {
            u32 s;
            ColorRampInit *anim;
            s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
            gSPLRandomState = s;
            anim = (ColorRampInit *)emitter->colorAnim;
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
            if (hasTexAnim != 0 && ((TexAnimInit *)emitter->texAnim)->randomizeInit) {
                u32 s;
                TexAnimInit *anim;
                s = gSPLRandomState * 0x5eedf715u + 0x1b0cb173u;
                gSPLRandomState = s;
                anim = (TexAnimInit *)emitter->texAnim;
                p->texture = anim->textures[(s >> 0x14) % anim->frameCount];
            } else if (hasTexAnim != 0 && !((TexAnimInit *)emitter->texAnim)->randomizeInit) {
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

#include "types.h"
#include "private/spl_particle_views.h"

/* Particle::System (include/Particle__System.h) as the child spawn reads it:
   the second particle list holds the children. */

extern u32 gSPLRandomState;

/* Spawn child->count particles from the parent particle self into sys's
   child list. SPLEmitter_Update calls it for a live particle at the emission
   interval (SPL's child emission). Each child takes the parent's position
   and offset, the parent velocity scaled by velocityRatio plus a random
   spread, and the parent's animated scale times scaleRatio. The other fields
   come from the child record or the parent. */
void SPLEmitter_EmitChildren(ParticleElement *self, SystemListView *sys, List *freelist) {
    ParticleElement *p;
    PtclChild *def;
    int i;
    int eA, eB, eC, d1c;
    u32 rngA, rngC;

    def = sys->mDefinition->child;
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
        int ratio;

        p = (ParticleElement *)SPLList_PopFront(freelist);
        if (p == 0)
            return;

        SPLList_PushFront(&sys->children, (Node *)p);

        p->offset = self->offset;

        ratio = def->velocityRatio + 1;

        gSPLRandomState = gSPLRandomState * rngA + rngC;
        p->velocity.x = ((self->velocity.x * ratio) >> 8) +
                        ((def->spread * (int)(gSPLRandomState >> 0x17) - (def->spread << 8)) >> 8);

        gSPLRandomState = gSPLRandomState * rngA + rngC;
        p->velocity.y = ((self->velocity.y * ratio) >> 8) +
                        ((def->spread * (int)(gSPLRandomState >> 0x17) - (def->spread << 8)) >> 8);

        gSPLRandomState = gSPLRandomState * rngA + rngC;
        p->velocity.z = ((self->velocity.z * ratio) >> 8) +
                        ((def->spread * (int)(gSPLRandomState >> 0x17) - (def->spread << 8)) >> 8);

        p->basePosition = self->basePosition;

        {
            int c = self->scale;
            int m = (c * self->animScale) >> 12;
            p->scale = (m * (def->scaleRatio + 1)) >> 6;
        }

        p->animScale = d1c;

        if (def->flags.useChildColor)
            p->color = def->color;
        else
            p->color = self->color;

        {
            Visibility *q = &p->visibility;
            q->baseAlpha = ((self->visibility.baseAlpha * (self->visibility.animAlpha + 1)) >> 5);
            q->animAlpha = 0x1f;
        }

        {
            int mode = def->flags.rotationType;
            switch (mode) {
            case 0:
                p->angle = eB;
                p->angularSpeed = eB;
                break;
            case 1:
                p->angle = self->angle;
                p->angularSpeed = eA;
                break;
            case 2:
                p->angle = self->angle;
                p->angularSpeed = self->angularSpeed;
                break;
            }
        }

        p->lifetime = def->lifetime;
        p->age = eC;
        p->texture = def->texture;
        p->loopTimeFactor = 0xffff / (int)((u32)self->lifetime >> 1);
        p->lifeRateFactor = 0xffff / self->lifetime;

        i += 1;
    } while (i < def->count);
}

#include "nitro/gx/g3imm_cmd.h"

/* Draw the unit quad in the XY plane (z = 0), with the texture repeated
 * 2^tileS by 2^tileT times. The particle draws pick it or SPLUtil_DrawXZPlane out of
 * sPlaneDrawingFunctions by the definition's polygon reference plane. The name is pret's SPL's
 * (that revision adds an offset). */
void SPLUtil_DrawXYPlane(int tileS, int tileT) {
    fx32 s = 0x1000 << tileS;
    fx32 t = 0x1000 << tileT;

    G3_Begin(GX_BEGIN_QUADS);
    G3_TexCoord(0, 0);
    G3_Vtx10(-0x1000, 0x1000, 0);
    G3_TexCoord(s, 0);
    G3_Vtx10(0x1000, 0x1000, 0);
    G3_TexCoord(s, t);
    G3_Vtx10(0x1000, -0x1000, 0);
    G3_TexCoord(0, t);
    G3_Vtx10(-0x1000, -0x1000, 0);
    G3_End();
}

#include "nitro/gx/g3imm_cmd.h"

/* Draw the unit quad in the XZ plane (y = 0), with the texture repeated
 * 2^tileS by 2^tileT times. The particle draws pick it or SPLUtil_DrawXYPlane out of
 * sPlaneDrawingFunctions by the definition's polygon reference plane. The name is pret's SPL's
 * (that revision adds an offset). */
void SPLUtil_DrawXZPlane(int tileS, int tileT) {
    fx32 s = 0x1000 << tileS;
    fx32 t = 0x1000 << tileT;

    G3_Begin(GX_BEGIN_QUADS);
    G3_TexCoord(0, 0);
    G3_Vtx10(-0x1000, 0, 0x1000);
    G3_TexCoord(s, 0);
    G3_Vtx10(0x1000, 0, 0x1000);
    G3_TexCoord(s, t);
    G3_Vtx10(0x1000, 0, -0x1000);
    G3_TexCoord(0, t);
    G3_Vtx10(-0x1000, 0, -0x1000);
    G3_End();
}

#include "nitro/fx/fx.h"

/* Build a rotation about the (1, 1, 1) diagonal from its sine and cosine:
 * 0x555 is 1/3 and 0x93d is sqrt(1/3) in 20.12. The particle draws pick it or
 * SPLUtil_RotateY out of sRotationFunctions. The name is pret's SPL's. */
void SPLUtil_RotateXYZ(int sin, int cos, MtxFx43 *mat) {
    int c = FX_MUL((0x1000 - cos), 0x555);
    int s = FX_MUL(sin, 0x93d);
    int diag;
    int plus = c + s;
    int minus = c - s;
    diag = c + cos;
    mat->m[0][0] = diag;
    mat->m[1][0] = plus;
    mat->m[2][0] = minus;
    mat->m[3][0] = 0;
    mat->m[0][1] = minus;
    mat->m[1][1] = diag;
    mat->m[2][1] = plus;
    mat->m[3][1] = 0;
    mat->m[0][2] = plus;
    mat->m[1][2] = minus;
    mat->m[2][2] = diag;
    mat->m[3][2] = 0;
}

#include "types.h"
#include "nitro/fx/fx.h"

/* Build a rotation about Y from its sine and cosine. The particle draws pick
 * it or SPLUtil_RotateXYZ out of sRotationFunctions by the definition's polygon
 * rotation axis. The name is pret's SPL's. */
void SPLUtil_RotateY(int sin, int cos, MtxFx43 *mat) {
    mat->m[0][0] = cos;
    mat->m[1][0] = 0;
    mat->m[2][0] = sin;
    mat->m[3][0] = 0;
    mat->m[0][1] = 0;
    mat->m[1][1] = 0x1000;
    mat->m[2][1] = 0;
    mat->m[3][1] = 0;
    mat->m[0][2] = -sin;
    mat->m[1][2] = 0;
    mat->m[2][2] = cos;
    mat->m[3][2] = 0;
}

#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/fx/fx_mtx43.h"

/* Draw one particle as a camera-facing quad (draw type 0, SPLManager_DoDraw):
 * alpha and the next polygon ID into POLYGON_ATTR (nothing more when the
 * alpha is 0), move basePosition + offset into view space, then load a matrix
 * that rotates by the particle's angle about the view axis, scales by
 * scale * animScale (x by aspectRatio too) and translates there, set its
 * color and emit the XY-plane quad. The name is pret's SPL's;
 * SPLDraw_Child_Billboard is the child-particle twin. */
void SPLDraw_Billboard(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    const MtxFx43 *view = mgr->mDrawMatrix;
    VecFx32 trans;
    MtxFx43 mat;
    u32 v;
    int sx;
    int sy;

    v = sys->polygonIDs.current << 24;
    v |= mgr->mPolygonAttr | 0xc0;
    v |= alpha << 16;
    REG_POLYGON_ATTR = v;
    REG_POLYGON_ATTR;
    {
        PolygonIDs *ids = &sys->polygonIDs;
        ids->current++;
        if (sys->polygonIDs.current > sys->polygonIDs.max)
            ids->current = sys->polygonIDs.min;
    }

    if (alpha == 0)
        return;

    sy = FX_MUL(ptcl->scale, ptcl->animScale);
    trans.x = ptcl->offset.x + ptcl->basePosition.x;
    trans.y = ptcl->offset.y + ptcl->basePosition.y;
    trans.z = ptcl->offset.z + ptcl->basePosition.z;
    MTX_MultVec43(&trans, view, &trans);

    {
        int idx = ptcl->angle >> 4;
        int cos;
        int sin;

        sx = FX_MUL(sy, aspectRatio);
        cos = FX_CosIdx(idx);
        sin = FX_SinIdx(idx);

        /* the 4x3 rows: (cos, sin, 0) * sx, (-sin, cos, 0) * sy, (0, 0, 1),
         * then the view-space position */
        mat.m[0][0] = FX_MUL(cos, sx);
        mat.m[1][0] = FX_MUL(-sin, sy);
        mat.m[0][1] = FX_MUL(sin, sx);
        mat.m[3][0] = trans.x;
        mat.m[3][1] = trans.y;
        mat.m[2][2] = 0x1000;
        mat.m[3][2] = trans.z;
        mat.m[2][0] = 0;
        mat.m[1][1] = FX_MUL(cos, sy);
        mat.m[2][1] = 0;
        mat.m[0][2] = 0;
        mat.m[1][2] = 0;
        G3_Identity();
        G3_MultMtx43(&mat.m[0][0]);
    }

    G3_Color(ptcl->color);
    {
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        /* Keep the retail u8 narrowing; passing these bitfields as int changes MWCC output. */
        SPLUtil_DrawXYPlane((u8)data->textureTileCountS, (u8)data->textureTileCountT);
    }
}

#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/fx/fx_mtx43.h"

/* Draw one child particle as a camera-facing quad (child draw type 0,
 * SPLManager_DoDraw): SPLDraw_Billboard with the child record's tile counts. The
 * name is pret's SPL's. */
void SPLDraw_Child_Billboard(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    const MtxFx43 *view = mgr->mDrawMatrix;
    VecFx32 trans;
    MtxFx43 mat;
    u32 v;
    int sx;
    int sy;

    v = sys->polygonIDs.current << 24;
    v |= mgr->mPolygonAttr | 0xc0;
    v |= alpha << 16;
    REG_POLYGON_ATTR = v;
    REG_POLYGON_ATTR;
    {
        PolygonIDs *ids = &sys->polygonIDs;
        ids->current++;
        if (sys->polygonIDs.current > sys->polygonIDs.max)
            ids->current = sys->polygonIDs.min;
    }

    if (alpha == 0)
        return;

    sy = FX_MUL(ptcl->scale, ptcl->animScale);
    trans.x = ptcl->offset.x + ptcl->basePosition.x;
    trans.y = ptcl->offset.y + ptcl->basePosition.y;
    trans.z = ptcl->offset.z + ptcl->basePosition.z;
    MTX_MultVec43(&trans, view, &trans);

    {
        int idx = ptcl->angle >> 4;
        int cos;
        int sin;

        sx = FX_MUL(sy, aspectRatio);
        cos = FX_CosIdx(idx);
        sin = FX_SinIdx(idx);

        /* the 4x3 rows: (cos, sin, 0) * sx, (-sin, cos, 0) * sy, (0, 0, 1),
         * then the view-space position */
        mat.m[0][0] = FX_MUL(cos, sx);
        mat.m[1][0] = FX_MUL(-sin, sy);
        mat.m[0][1] = FX_MUL(sin, sx);
        mat.m[3][0] = trans.x;
        mat.m[3][1] = trans.y;
        mat.m[2][2] = 0x1000;
        mat.m[3][2] = trans.z;
        mat.m[2][0] = 0;
        mat.m[1][1] = FX_MUL(cos, sy);
        mat.m[2][1] = 0;
        mat.m[0][2] = 0;
        mat.m[1][2] = 0;
        G3_Identity();
        G3_MultMtx43(&mat.m[0][0]);
    }

    G3_Color(ptcl->color);
    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        /* Keep the retail u8 narrowing; passing these bitfields as int changes MWCC output. */
        SPLUtil_DrawXYPlane((u8)child->textureTileCountS, (u8)child->textureTileCountT);
    }
}

#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_vec.h"
#include "nitro/fx/fx_mtx33.h"
#include "nitro/fx/fx_mtx43.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/mi/memory.h"

/* No header declares these. */

/* Draw one particle as a quad stretched along its flight (draw type 1,
 * SPLManager_DoDraw): alpha and the next polygon ID into POLYGON_ATTR (nothing
 * more when the alpha is 0); the quad's x axis is velocity x the view's z
 * axis turned into view space (nothing is drawn when they are parallel), y
 * at right angles to it, scaled by scale * animScale (x by aspectRatio too,
 * y stretched by dbbScale the more the particle flies across the view),
 * translated to basePosition + offset in view space; then its color and the
 * XY-plane quad. The name is pret's SPL's (pret's revision adds the
 * scale-animation axis and view-space cases);
 * SPLDraw_Child_DirectionalBillboard is the child-particle twin. */
void SPLDraw_DirectionalBillboard(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    const MtxFx43 *view = mgr->mDrawMatrix;
    VecFx32 velDir;
    VecFx32 trans;
    VecFx32 dir;
    VecFx32 look;
    MtxFx33 rot;
    MtxFx43 mat;
    u32 v;
    int sx;
    s64 sy; /* s64, not int: int does not match */

    v = sys->polygonIDs.current << 24;
    v |= mgr->mPolygonAttr | 0xc0;
    v |= alpha << 16;
    REG_POLYGON_ATTR = v;
    REG_POLYGON_ATTR;
    {
        PolygonIDs *ids = &sys->polygonIDs;
        ids->current++;
        if (sys->polygonIDs.current > sys->polygonIDs.max)
            ids->current = sys->polygonIDs.min;
    }

    if (alpha == 0)
        return;

    sy = FX_MUL(ptcl->scale, ptcl->animScale);
    trans.x = ptcl->offset.x + ptcl->basePosition.x;
    trans.y = ptcl->offset.y + ptcl->basePosition.y;
    trans.z = ptcl->offset.z + ptcl->basePosition.z;
    sx = FX_MUL((int)sy, aspectRatio);

    /* the quad's x axis: velocity x the view's z column, in view space */
    dir = ptcl->velocity;
    look.x = view->m[0][2];
    look.y = view->m[1][2];
    look.z = view->m[2][2];
    VEC_CrossProduct(&dir, &look, &dir);

    if (dir.x == 0 && dir.y == 0 && dir.z == 0)
        return;

    VEC_Normalize(&dir, &dir);
    MI_Copy36B(view, &rot);
    MTX_MultVec33(&dir, &rot, &dir);
    MTX_MultVec43(&trans, view, &trans);
    velDir = ptcl->velocity;
    VEC_Normalize(&velDir, &velDir);

    {
        /* |velocity . -view z|: 1 when the particle flies straight at the
         * camera, 0 across it; y stretches by (1 - that) * dbbScale + 1 */
        int dot = FX_MUL(velDir.x, -view->m[0][2]) + FX_MUL(velDir.y, -view->m[1][2]) +
                  FX_MUL(velDir.z, -view->m[2][2]);
        int side;
        int stretch;
        int sy2;
        if (dot < 0)
            dot = -dot;
        side = 0x1000 - dot;
        stretch = FX_MUL(side, (int)mgr->mCurrentSystem->mDefinition->data->dbbScale);
        sy2 = FX_MUL(sy, (stretch + 0x1000));

        mat.m[0][0] = FX_MUL(dir.x, sx);
        mat.m[2][2] = 0x1000;
        mat.m[3][0] = trans.x;
        mat.m[3][1] = trans.y;
        mat.m[3][2] = trans.z;
        mat.m[1][0] = FX_MUL(-dir.y, sy2);
        mat.m[0][1] = FX_MUL(dir.y, sx);
        mat.m[2][0] = 0;
        mat.m[1][1] = FX_MUL(dir.x, sy2);
        mat.m[2][1] = 0;
        mat.m[0][2] = 0;
        mat.m[1][2] = 0;
        G3_Identity();
        G3_MultMtx43(&mat.m[0][0]);
    }

    G3_Color(ptcl->color);
    {
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        /* Keep the retail u8 narrowing; passing these bitfields as int changes MWCC output. */
        SPLUtil_DrawXYPlane((u8)data->textureTileCountS, (u8)data->textureTileCountT);
    }
}

#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_vec.h"
#include "nitro/fx/fx_mtx33.h"
#include "nitro/fx/fx_mtx43.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/mi/memory.h"

/* No header declares these. */

/* Draw one child particle as a quad stretched along its flight (child draw
 * type 1, SPLManager_DoDraw): SPLDraw_DirectionalBillboard, still with the parent
 * header's aspect ratio and dbbScale, but the child record's tile counts.
 * The name is pret's SPL's. */
void SPLDraw_Child_DirectionalBillboard(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    const MtxFx43 *view = mgr->mDrawMatrix;
    VecFx32 velDir;
    VecFx32 trans;
    VecFx32 dir;
    VecFx32 look;
    MtxFx33 rot;
    MtxFx43 mat;
    u32 v;
    int sx;
    s64 sy; /* s64, not int: int does not match */

    v = sys->polygonIDs.current << 24;
    v |= mgr->mPolygonAttr | 0xc0;
    v |= alpha << 16;
    REG_POLYGON_ATTR = v;
    REG_POLYGON_ATTR;
    {
        PolygonIDs *ids = &sys->polygonIDs;
        ids->current++;
        if (sys->polygonIDs.current > sys->polygonIDs.max)
            ids->current = sys->polygonIDs.min;
    }

    if (alpha == 0)
        return;

    sy = FX_MUL(ptcl->scale, ptcl->animScale);
    trans.x = ptcl->offset.x + ptcl->basePosition.x;
    trans.y = ptcl->offset.y + ptcl->basePosition.y;
    trans.z = ptcl->offset.z + ptcl->basePosition.z;
    sx = FX_MUL((int)sy, aspectRatio);

    /* the quad's x axis: velocity x the view's z column, in view space */
    dir = ptcl->velocity;
    look.x = view->m[0][2];
    look.y = view->m[1][2];
    look.z = view->m[2][2];
    VEC_CrossProduct(&dir, &look, &dir);

    if (dir.x == 0 && dir.y == 0 && dir.z == 0)
        return;

    VEC_Normalize(&dir, &dir);
    MI_Copy36B(view, &rot);
    MTX_MultVec33(&dir, &rot, &dir);
    MTX_MultVec43(&trans, view, &trans);
    velDir = ptcl->velocity;
    VEC_Normalize(&velDir, &velDir);

    {
        /* |velocity . -view z|: 1 when the particle flies straight at the
         * camera, 0 across it; y stretches by (1 - that) * dbbScale + 1 */
        int dot = FX_MUL(velDir.x, -view->m[0][2]) + FX_MUL(velDir.y, -view->m[1][2]) +
                  FX_MUL(velDir.z, -view->m[2][2]);
        int side;
        int stretch;
        int sy2;
        if (dot < 0)
            dot = -dot;
        side = 0x1000 - dot;
        stretch = FX_MUL(side, (int)mgr->mCurrentSystem->mDefinition->data->dbbScale);
        sy2 = FX_MUL(sy, (stretch + 0x1000));

        mat.m[0][0] = FX_MUL(dir.x, sx);
        mat.m[2][2] = 0x1000;
        mat.m[3][0] = trans.x;
        mat.m[3][1] = trans.y;
        mat.m[3][2] = trans.z;
        mat.m[1][0] = FX_MUL(-dir.y, sy2);
        mat.m[0][1] = FX_MUL(dir.y, sx);
        mat.m[2][0] = 0;
        mat.m[1][1] = FX_MUL(dir.x, sy2);
        mat.m[2][1] = 0;
        mat.m[0][2] = 0;
        mat.m[1][2] = 0;
        G3_Identity();
        G3_MultMtx43(&mat.m[0][0]);
    }

    G3_Color(ptcl->color);
    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        /* Keep the retail u8 narrowing; passing these bitfields as int changes MWCC output. */
        SPLUtil_DrawXYPlane((u8)child->textureTileCountS, (u8)child->textureTileCountT);
    }
}

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "math/MtxStack.h"

extern void MTX_Scale43_(Matrix4x3 *m, int sx, int sy, int sz); /* scale matrix */
extern void G3_LoadMtx43(void *m);                              /* load the 4x3 matrix */
extern void G3_MultMtx43(void *m);                              /* multiply by the 4x3 matrix */

/* Draw one particle as a textured quad (draw type 2, SPLManager_DoDraw): alpha
 * and the next polygon ID into POLYGON_ATTR (nothing more when the alpha is
 * 0), rotate by its angle about
 * the definition's axis, scale by scale * animScale (x by aspectRatio too),
 * translate to basePosition + offset, set its color and emit the quad in the
 * definition's plane. The name is pret's SPL's; SPLDraw_Child_Polygon
 * is the child-particle twin. */
void SPLDraw_Polygon(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    Matrix4x3 out;
    Matrix4x3 rot;
    Matrix4x3 scl;
    u32 v;

    v = sys->polygonIDs.current << 24;
    v |= mgr->mPolygonAttr | 0xc0;
    v |= alpha << 16;
    REG_POLYGON_ATTR = v;
    REG_POLYGON_ATTR;
    {
        PolygonIDs *ids = &sys->polygonIDs;
        ids->current++;
        if (sys->polygonIDs.current > sys->polygonIDs.max)
            ids->current = sys->polygonIDs.min;
    }

    if (alpha == 0)
        return;

    {
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        int idx = ptcl->angle >> 4;
        sRotationFunctions[data->polygonRotAxis](FX_SinIdx(idx), FX_CosIdx(idx), &rot);
    }

    {
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        int sy = FX_MUL(ptcl->scale, ptcl->animScale);
        int sx = FX_MUL(sy, data->aspectRatio);
        MTX_Scale43_(&scl, sx, sy, sy);
    }

    MTX_Concat43(&scl, &rot, &out);

    out.t.x = ptcl->offset.x + ptcl->basePosition.x;
    out.t.y = ptcl->offset.y + ptcl->basePosition.y;
    out.t.z = ptcl->offset.z + ptcl->basePosition.z;

    G3_LoadMtx43(mgr->mDrawMatrix);
    G3_MultMtx43(&out);

    G3_Color(ptcl->color);
    {
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        sPlaneDrawingFunctions[data->polygonReferencePlane](data->textureTileCountS,
                                                            data->textureTileCountT);
    }
}

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "math/MtxStack.h"

extern void MTX_Scale43_(Matrix4x3 *m, int sx, int sy, int sz); /* scale matrix */
extern void G3_LoadMtx43(void *m);                              /* load the 4x3 matrix */
extern void G3_MultMtx43(void *m);                              /* multiply by the 4x3 matrix */

/* Draw one child particle as a textured quad (child draw type 2,
 * SPLManager_DoDraw): SPLDraw_Polygon with the child record's rotation axis, plane
 * and tile counts, and the rotation applied before the scale. The name is
 * pret's SPL's. */
void SPLDraw_Child_Polygon(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    Matrix4x3 out;
    Matrix4x3 rot;
    Matrix4x3 scl;
    u32 v;

    v = sys->polygonIDs.current << 24;
    v |= mgr->mPolygonAttr | 0xc0;
    v |= alpha << 16;
    REG_POLYGON_ATTR = v;
    REG_POLYGON_ATTR;
    {
        PolygonIDs *ids = &sys->polygonIDs;
        ids->current++;
        if (sys->polygonIDs.current > sys->polygonIDs.max)
            ids->current = sys->polygonIDs.min;
    }

    if (alpha == 0)
        return;

    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        int idx = ptcl->angle >> 4;
        sRotationFunctions[child->flags.polygonRotAxis](FX_SinIdx(idx), FX_CosIdx(idx), &rot);
    }

    {
        PtclDef *def = mgr->mCurrentSystem->mDefinition;
        int sy = FX_MUL(ptcl->scale, ptcl->animScale);
        s16 aspectRatio = def->data->aspectRatio;
        int sx = FX_MUL(sy, aspectRatio);
        MTX_Scale43_(&scl, sx, sy, sy);
    }

    MTX_Concat43(&rot, &scl, &out);

    out.t.x = ptcl->offset.x + ptcl->basePosition.x;
    out.t.y = ptcl->offset.y + ptcl->basePosition.y;
    out.t.z = ptcl->offset.z + ptcl->basePosition.z;

    G3_LoadMtx43(mgr->mDrawMatrix);
    G3_MultMtx43(&out);

    G3_Color(ptcl->color);
    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        sPlaneDrawingFunctions[child->flags.polygonReferencePlane](child->textureTileCountS,
                                                                   child->textureTileCountT);
    }
}

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/g3imm.h"
#include "nitro/gx/g3imm_cmd.h"

/* Bind a particle texture record: set its image and palette (texture
   coordinates from the texcoord command, texGen 1), then load a texture
   matrix that scales by its width and height. SPLManager_DoDraw binds the system's
   texture with it, and each particle's own when the texture animates. */
void SPLUtil_SetTexture(struct Tex *self) {
    struct TexParam p = self->param;
    G3_TexImageParam(p.fmt, GX_TEXGEN_TEXCOORD, p.sizeS, p.sizeT, p.repeat, p.flip, p.color0,
                     self->texAddr);
    G3_TexPlttBase(self->pltAddr, p.fmt);
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();
    G3_Scale(self->width << 12, self->height << 12, 0);
    G3_MtxMode(GX_MTXMODE_POSITION);
}

/* Empty: the per-particle texture bind SPLManager_DoDraw uses when the
   definition does not animate its texture. The name is pret's SPL's, for
   the same role (SPLManager_DrawParticles' `hasTexAnim ? SPLUtil_SetTexture
   : SPLUtil_SetTexture_Stub`). */
void SPLUtil_SetTexture_Stub(void *texture) {}

#include "types.h"
/* recovered: shared common types */
#include "common.h"
#include "private/spl_particle_views.h"
struct H2 {
    u16 a, b;
};

/* Initialise a Particle::System that Particle::Manager::AddSystem
 * (Particle_Manager.cpp) has just taken from the free list: clear its links
 * and both particle lists, store the definition and position, zero the
 * velocity, age and callback value, and seed the emitter from the
 * definition's header.
 *
 * The views are include/private/spl_particle_views.h's; the definition
 * header's fields are named for the System field this function seeds from
 * them. */
void SPLEmitter_Init(ParticleSystem *o, void *p18, struct Vector3 *src3) {
    o->mDefinition = p18;
    o->callbackFlags = 0;
    o->position = *src3;
    o->velocity.z = 0;
    o->velocity.y = o->velocity.z;
    o->velocity.x = o->velocity.y;
    o->age = 0;
    o->callbackValue = 0;
    {
        PtclData *s = o->mDefinition->data;
        *(struct H2 *)&o->direction = *(struct H2 *)s->axis;
        o->direction.z = s->axis[2];
        o->emitRadius = o->mDefinition->data->emitRadius;
        o->callbackParam = o->mDefinition->data->callbackParam;
        o->callbackVelocity = o->mDefinition->data->callbackVelocity;
        o->callbackScale = o->mDefinition->data->callbackScale;
        o->particleLifetime = o->mDefinition->data->particleLifetime;
        o->emitInterval = o->mDefinition->data->emitInterval;
        o->baseAlpha = o->mDefinition->data->baseAlpha;
    }
    o->previous = 0;
    o->next = o->previous;
    o->particles2.head = 0;
    o->particles.head = o->particles2.head;
    o->particles2.count = 0;
    o->particles.count = o->particles2.count;
    o->callback = 0;
    o->m6c = 0;
    o->m70 = 0;
}

#include "types.h"
#include "nitro/fx/fx.h"
/* SPLEmitter_Update @ 0x0204a730 (arm9, size 0x6fc)   [mwccarm 2004/b56]
 * Particle system update: emit, run the per-particle animation and behavior
 * tables, integrate, spawn children and retire expired particles, then the
 * same for the child particles. The child loop's time byte is staged in an int
 * local (the animation callbacks' lifeRate type) ahead of the call loop, as
 * the ROM keeps it in a register across it.
 */

typedef void (*Cb1)(void *, void *, int);
typedef void (*Cb2)(u32, void *, void *);

typedef struct {
    Cb1 func;
    u32 data;
} Ent1;

typedef struct {
    Cb2 func;
    u32 data;
} Ent2;

/* File-local views of Particle::System, its particles and its definition
 * (include/Particle__System.h, Particle__Element.h, Particle__Manager.h are
 * C++). Only the fields this update loop touches are named. The definition
 * has NitroSDK SPL's SPLResource order (header, scale/color/alpha/texture
 * animations, child resource, behaviors), and the names follow SPL's.
 * UpdateData, UpdateChild and UpdateDef stay file-local, apart from
 * include/private/spl_particle_views.h's PtclData, PtclChild and PtclDef,
 * which leave what this loop reads as padding: the follow-emitter and
 * behavior bits, the emitter lifetime, air resistance, the child emission
 * timing and the behavior table. */
typedef struct PtclNode {
    struct PtclNode *next; /* 0x00 */
    void *prev;            /* 0x04 */
    Vector3 basePosition;  /* 0x08 */
    Vector3 offset;        /* 0x14 */
    Vector3 velocity;      /* 0x20 */
    u16 lifetime;          /* 0x2c */
    u16 age;               /* 0x2e */
    u8 pad_30[0x6];
    u16 angle;        /* 0x36 */
    s16 angularSpeed; /* 0x38 */
    u8 pad_3a[0x2];
    u16 loopTimeFactor; /* 0x3c -- age * it >> 8 drives the looping animations */
    u16 lifeRateFactor; /* 0x3e -- age * it >> 8 drives the one-shot animations */
} PtclNode;

typedef struct {
    PtclNode *head; /* 0x00 */
    s32 count;      /* 0x04 */
} PtclList;

/* SPL's SPLResourceFlags: the bits this loop tests. */
typedef struct {
    u32 emissionType : 4;
    u32 drawType : 2;
    u32 circleAxis : 2;
    u32 hasScaleAnim : 1; /* bit 8 */
    u32 hasColorAnim : 1; /* bit 9 */
    u32 hasAlphaAnim : 1; /* bit 10 */
    u32 hasTexAnim : 1;   /* bit 11 */
    u32 hasRotation : 1;
    u32 randomInitAngle : 1;
    u32 : 1;
    u32 followEmitter : 1;    /* bit 15 -- particles track the system's position */
    u32 hasChildResource : 1; /* bit 16 */
    u32 : 15;
} UpdateFlags;

typedef struct {
    UpdateFlags flags; /* 0x00 */
    u8 pad_04[0x24];
    u16 emitterLifeTime; /* 0x28 -- emission stops once the system's age reaches it; 0 never stops */
    u8 pad_2a[0x8];
    u8 airResistance; /* 0x32 -- velocities scale by (airResistance + 0x180) / 0x200 each frame */
} UpdateData;

/* SPL's SPLChildResourceFlags: the bits this loop tests. */
typedef struct {
    u16 usesBehaviors : 1; /* bit 0 -- run the parent's behaviors on the children too */
    u16 hasScaleAnim : 1;  /* bit 1 */
    u16 hasAlphaAnim : 1;  /* bit 2 */
    u16 rotationType : 2;
    u16 followEmitter : 1; /* bit 5 */
    u16 : 10;
} UpdateChildFlags;

typedef struct {
    UpdateChildFlags flags; /* 0x00 */
    u8 pad_02[0xb];
    u8 emissionDelay; /* 0x0d -- children start (emissionDelay + 1) / 256 into the parent's life */
    u8 emissionInterval; /* 0x0e -- then every emissionInterval frames */
} UpdateChild;

/* The animation records' flag words, with SPL's names. The loop bit picks
 * which of a particle's two time bytes drives the animation. */
typedef struct {
    u8 pad_00[0x8];
    u16 loop : 1; /* 0x08 */
    u16 : 15;
} UpdateScaleAnim;

typedef struct {
    u8 pad_00[0x8];
    u16 randomStartColor : 1; /* 0x08 -- the colour is picked once, not animated */
    u16 loop : 1;
    u16 : 14;
} UpdateColorAnim;

typedef struct {
    u8 pad_00[0x2];
    u16 randomRange : 8; /* 0x02 */
    u16 loop : 1;
    u16 : 7;
} UpdateAlphaAnim;

typedef struct {
    u8 pad_00[0x8];
    u32 frameCount : 8; /* 0x08 */
    u32 step : 8;
    u32 randomizeInit : 1; /* the texture is picked once, not animated */
    u32 loop : 1;
    u32 : 14;
} UpdateTexAnim;

typedef struct {
    UpdateData *data;           /* 0x00 */
    UpdateScaleAnim *scaleAnim; /* 0x04 */
    UpdateColorAnim *colorAnim; /* 0x08 */
    UpdateAlphaAnim *alphaAnim; /* 0x0c */
    UpdateTexAnim *texAnim;     /* 0x10 */
    UpdateChild *child;         /* 0x14 */
    Ent2 *behaviors;            /* 0x18 -- each adds its push into a particle's acceleration */
    u16 behaviorCount;          /* 0x1c */
} UpdateDef;

typedef struct {
    void *next;           /* 0x00 */
    void *previous;       /* 0x04 */
    PtclList particles;   /* 0x08 */
    PtclList particles2;  /* 0x10 */
    UpdateDef *def;       /* 0x18 */
    u32 terminate : 1;    /* 0x1c bit 0 */
    u32 stopGenerate : 1; /* bit 1 */
    u32 : 30;
    Vector3 position; /* 0x20 */
    Vector3 velocity; /* 0x2c -- added to every particle's offset each frame */
    u16 age;          /* 0x38 */
    u8 pad_3a[0x1e];
    u8 emitInterval; /* 0x58 */
    u8 pad_59[0xf];
    void (*callback)(void *); /* 0x68 */
} PtclSystem;

typedef struct {
    u8 pad_00[0x14];
    PtclList freeParticles; /* 0x14 -- expired particles go back here */
} PtclManager;

void SPLEmitter_Update(void *manager, void *self) {
    UpdateDef *sb;
    PtclNode *next;
    UpdateChild *cfg;
    s32 drag;
    s32 followEmitter;
    s32 emitChildren;
    PtclNode *childNext;
    UpdateFlags flags;
    u16 emitterLifeTime;
    u16 behaviorCount;
    PtclNode *node;
    u8 time[2];
    Ent1 table1[4];
    Ent1 tableB[4];
    s32 acc[3];
    UpdateData *data;
    int childTime;
    s32 i;
    s32 n;
    PtclSystem *sys = (PtclSystem *)self;
    PtclManager *mgr = (PtclManager *)manager;

    sb = sys->def;
    cfg = sb->child;
    data = sb->data;
    flags = data->flags;
    i = 0;
    n = 0;
    drag = data->airResistance + 0x180;
    emitterLifeTime = data->emitterLifeTime;
    behaviorCount = sb->behaviorCount;

    if (emitterLifeTime == 0 || sys->age < emitterLifeTime) {
        if (sys->age % sys->emitInterval == 0) {
            if (!sys->terminate && !sys->stopGenerate) {
                SPLEmitter_EmitParticles((ParticleSystem *)self, (List *)&mgr->freeParticles);
            }
        }
    }

    if (flags.hasScaleAnim) {
        table1[n].func = (Cb1)SPLAnim_Scale;
        table1[n++].data = sb->scaleAnim->loop;
    }
    if (flags.hasColorAnim) {
        UpdateColorAnim *anim = sb->colorAnim;
        if (!anim->randomStartColor) {
            table1[n].func = (Cb1)SPLAnim_Color;
            table1[n++].data = anim->loop;
        }
    }
    if (flags.hasAlphaAnim) {
        table1[n].func = (Cb1)SPLAnim_Alpha;
        table1[n++].data = sb->alphaAnim->loop;
    }
    if (flags.hasTexAnim) {
        UpdateTexAnim *anim = sb->texAnim;
        if (!anim->randomizeInit) {
            table1[n].func = (Cb1)SPLAnim_Texture;
            table1[n].data = anim->loop;
            n += 1;
        }
    }

    node = sys->particles.head;
    if (node != 0) {
        followEmitter = flags.followEmitter;
        emitChildren = flags.hasChildResource;
        do {
            u16 ratio;
            s32 j;

            next = node->next;
            ratio = node->age;

            time[0] = (u8)((node->lifeRateFactor * ratio) >> 8);
            time[1] = (u8)((node->loopTimeFactor * ratio) >> 8);

            i = 0;
            if (n > 0) {
                do {
                    table1[i].func(node, sb, time[table1[i].data]);
                    i += 1;
                } while (i < n);
            }

            j = 0;
            acc[2] = 0;
            acc[1] = 0;
            acc[0] = 0;

            if (followEmitter != 0) {
                node->basePosition = sys->position;
            }

            if ((s32)behaviorCount > 0) {
                do {
                    Ent2 *tbl2 = sb->behaviors;
                    tbl2[j].func(tbl2[j].data, node, &acc[0]);
                    j += 1;
                } while (j < behaviorCount);
            }

            node->angle += node->angularSpeed;
            node->velocity.x = (node->velocity.x * drag) >> 9;
            node->velocity.y = (node->velocity.y * drag) >> 9;
            node->velocity.z = (node->velocity.z * drag) >> 9;
            node->velocity.x += acc[0];
            node->velocity.y += acc[1];
            node->velocity.z += acc[2];
            node->offset.x += node->velocity.x + sys->velocity.x;
            node->offset.y += node->velocity.y + sys->velocity.y;
            node->offset.z += node->velocity.z + sys->velocity.z;

            if (emitChildren != 0) {
                u16 life = node->age;
                s32 a = node->lifetime << 12;
                s32 b = (cfg->emissionDelay + 1) << 12;
                s32 r1 = FX_MUL(a, b);
                s32 r0 = (life << 12) - (r1 >> 8);
                if (life != 0 && r0 >= 0 && ((r0 >> 12) % cfg->emissionInterval) == 0) {
                    SPLEmitter_EmitChildren((ParticleElement *)node, (SystemListView *)self,
                                            (List *)&mgr->freeParticles);
                }
            }

            node->age++;
            if (node->age > node->lifetime) {
                SPLList_PushFront((List *)&mgr->freeParticles,
                                  (Node *)SPLList_Erase((List *)&sys->particles, (Node *)node));
            }

            node = next;
        } while (next != 0);
    }

    if (flags.hasChildResource) {
        s32 n2 = 0;
        PtclNode *child;
        s32 k;

        if (cfg->flags.hasScaleAnim) {
            tableB[0].func = (Cb1)SPLAnim_ChildScale;
            tableB[0].data = 0;
            n2 += 1;
        }
        if (cfg->flags.hasAlphaAnim) {
            tableB[n2].func = (Cb1)SPLAnim_ChildAlpha;
            tableB[n2].data = 0;
            n2 += 1;
        }
        child = sys->particles2.head;
        if (!cfg->flags.usesBehaviors) {
            behaviorCount = 0;
        }

        if (child != 0) {
            do {
                u16 lifetime;
                u16 age;

                age = child->age;
                childNext = child->next;
                lifetime = child->lifetime;
                time[0] = (u8)((age << 8) / lifetime);

                k = 0;
                if (n2 > 0) {
                    childTime = time[0];
                    do {
                        tableB[k].func(child, sb, childTime);
                        k += 1;
                    } while (k < n2);
                }

                k = 0;
                acc[2] = 0;
                acc[1] = 0;
                acc[0] = 0;

                if (cfg->flags.followEmitter) {
                    child->basePosition = sys->position;
                }

                if ((s32)behaviorCount > 0) {
                    do {
                        Ent2 *tbl2 = sb->behaviors;
                        tbl2[k].func(tbl2[k].data, child, &acc[0]);
                        k += 1;
                    } while (k < behaviorCount);
                }

                child->angle += child->angularSpeed;
                child->velocity.x = (child->velocity.x * drag) >> 9;
                child->velocity.y = (child->velocity.y * drag) >> 9;
                child->velocity.z = (child->velocity.z * drag) >> 9;
                child->velocity.x += acc[0];
                child->velocity.y += acc[1];
                child->velocity.z += acc[2];
                child->offset.x += child->velocity.x + sys->velocity.x;
                child->offset.y += child->velocity.y + sys->velocity.y;
                child->offset.z += child->velocity.z + sys->velocity.z;
                child->age++;

                if (child->age > child->lifetime) {
                    SPLList_PushFront(
                        (List *)&mgr->freeParticles,
                        (Node *)SPLList_Erase((List *)&sys->particles2, (Node *)child));
                }

                child = childNext;
            } while (childNext != 0);
        }
    }

    sys->age++;

    {
        void (*cb)(void *) = sys->callback;
        if (cb == 0) {
            return;
        }
        cb(self);
    }
}

#include "types.h"
#include "private/spl_particle_views.h"

/* Draw the current system's particles, then its child particles: bind the
 * definition's texture, pick the draw routine by draw type (billboard,
 * directional billboard, polygon), and when the definition animates its
 * texture rebind each particle's own before drawing it
 * (SPLUtil_SetTexture_Stub is the no-op). pret's SPL splits this into SPLManager_DrawParticles and
 * SPLManager_DrawChildParticles. */
void SPLManager_DoDraw(char *self) {
    ManagerView *mgr = (ManagerView *)self;
    ParticleSystem *system = mgr->mCurrentSystem;
    PtclDef *def = system->mDefinition;
    ParticleElement *n;
    PtclData *data = def->data;
    void (*draw)(void *, void *) = 0;
    void (*setTexture)(void *);

    SPLUtil_SetTexture((struct Tex *)&mgr->mTextures[data->texture]);

    switch (data->drawType) {
    case 0:
        draw = (void (*)(void *, void *))SPLDraw_Billboard;
        break;
    case 1:
        draw = (void (*)(void *, void *))SPLDraw_DirectionalBillboard;
        break;
    case 2:
        draw = (void (*)(void *, void *))SPLDraw_Polygon;
        break;
    }

    setTexture = data->hasTexAnim ? (void (*)(void *))SPLUtil_SetTexture : SPLUtil_SetTexture_Stub;

    n = system->particles.head;
    if (n != 0) {
        do {
            setTexture(&mgr->mTextures[n->texture]);
            draw(self, n);
            n = n->next;
        } while (n != 0);
    }

    if (!data->hasChildResource)
        return;

    SPLUtil_SetTexture((struct Tex *)&mgr->mTextures[def->child->texture]);

    switch (def->child->flags.drawType) {
    case 0:
        draw = (void (*)(void *, void *))SPLDraw_Child_Billboard;
        break;
    case 1:
        draw = (void (*)(void *, void *))SPLDraw_Child_DirectionalBillboard;
        break;
    case 2:
        draw = (void (*)(void *, void *))SPLDraw_Child_Polygon;
        break;
    }

    n = system->particles2.head;
    if (n == 0)
        return;
    do {
        draw(self, n);
        n = n->next;
    } while (n != 0);
}

/* SPLEmitter_Emit @ 0x204a5bc (arm9) -- tail-call veneer to SPLEmitter_EmitParticles (0x204c584).
 * ldr ip, [pc]; bx ip; .word 0x204c584
 *
 * Forwards both of its arguments in r0 and r1. A tail call leaves them alone,
 * so the old (void) spelling matched as well.
 */

void SPLEmitter_Emit(void *self, void *list) {
    SPLEmitter_EmitParticles(self, list);
}
