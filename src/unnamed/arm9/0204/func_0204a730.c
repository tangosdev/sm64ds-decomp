#include "types.h"
#include "nitro/fx/fx.h"
/* func_0204a730 @ 0x0204a730 (arm9, size 0x6fc)   [mwccarm 2004/b56]
 * Particle system update: emit, run the per-particle animation and behavior
 * tables, integrate, spawn children and retire expired particles, then the
 * same for the child particles. The child loop's time byte is staged in an int
 * local (the animation callbacks' lifeRate type) ahead of the call loop, as
 * the ROM keeps it in a register across it.
 */

extern void func_0204c584(void *self, void *arg);
extern void func_0204c304(void *node, void *self, void *arg);
extern void *func_0204d8e8(void *listHead, void *node);
extern void func_0204d9a0(void *list, void *node);

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
    UpdateChild *child; /* 0x14 */
    Ent2 *behaviors;    /* 0x18 -- each adds its push into a particle's acceleration */
    u16 behaviorCount;  /* 0x1c */
} UpdateDef;

typedef struct {
    void *next;          /* 0x00 */
    void *previous;      /* 0x04 */
    PtclList particles;  /* 0x08 */
    PtclList particles2; /* 0x10 */
    UpdateDef *def;      /* 0x18 */
    u32 terminate : 1;   /* 0x1c bit 0 */
    u32 stopGenerate : 1; /* bit 1 */
    u32 : 30;
    Vector3 position;    /* 0x20 */
    Vector3 velocity;    /* 0x2c -- added to every particle's offset each frame */
    u16 age;             /* 0x38 */
    u8 pad_3a[0x1e];
    u8 emitInterval; /* 0x58 */
    u8 pad_59[0xf];
    void (*callback)(void *); /* 0x68 */
} PtclSystem;

typedef struct {
    u8 pad_00[0x14];
    PtclList freeParticles; /* 0x14 -- expired particles go back here */
} PtclManager;

extern void SPLAnim_Scale(void *, void *, int);
extern void SPLAnim_Color(void *, void *, int);
extern void SPLAnim_Alpha(void *, void *, int);
extern void SPLAnim_Texture(void *, void *, int);
extern void SPLAnim_ChildScale(void *, void *, int);
extern void SPLAnim_ChildAlpha(void *, void *, int);

void func_0204a730(void *arg0, void *self) {
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
    PtclManager *mgr = (PtclManager *)arg0;

    sb = sys->def;
    cfg = sb->child;
    data = sb->data;
    flags = data->flags;
    i = 0;
    n = 0;
    drag = (s32)data->airResistance + 0x180;
    emitterLifeTime = data->emitterLifeTime;
    behaviorCount = sb->behaviorCount;

    if (emitterLifeTime == 0 || (u16)sys->age < emitterLifeTime) {
        if ((u16)sys->age % (u8)sys->emitInterval == 0) {
            if (!sys->terminate && !sys->stopGenerate) {
                func_0204c584(self, &mgr->freeParticles);
            }
        }
    }

    if (flags.hasScaleAnim) {
        table1[n].func = SPLAnim_Scale;
        table1[n++].data = sb->scaleAnim->loop;
    }
    if (flags.hasColorAnim) {
        UpdateColorAnim *anim = sb->colorAnim;
        if (!anim->randomStartColor) {
            table1[n].func = SPLAnim_Color;
            table1[n++].data = anim->loop;
        }
    }
    if (flags.hasAlphaAnim) {
        table1[n].func = SPLAnim_Alpha;
        table1[n++].data = sb->alphaAnim->loop;
    }
    if (flags.hasTexAnim) {
        UpdateTexAnim *anim = sb->texAnim;
        if (!anim->randomizeInit) {
            table1[n].func = SPLAnim_Texture;
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

            time[0] = (u8)((s32)(node->lifeRateFactor * ratio) >> 8);
            time[1] = (u8)((s32)(node->loopTimeFactor * ratio) >> 8);

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
                } while (j < (s32)behaviorCount);
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
                s32 a = (s32)node->lifetime << 0xc;
                s32 b = ((s32)cfg->emissionDelay + 1) << 0xc;
                s32 r1 = FX_MUL(a, b);
                s32 r0 = ((s32)life << 0xc) - (r1 >> 8);
                if (life != 0 && r0 >= 0 && ((r0 >> 0xc) % (u8)cfg->emissionInterval) == 0) {
                    func_0204c304(node, self, &mgr->freeParticles);
                }
            }

            node->age++;
            if (node->age > node->lifetime) {
                func_0204d9a0(&mgr->freeParticles, func_0204d8e8(&sys->particles, node));
            }

            node = next;
        } while (next != 0);
    }

    if (flags.hasChildResource) {
        s32 n2 = 0;
        PtclNode *child;
        s32 k;

        if (cfg->flags.hasScaleAnim) {
            tableB[0].func = SPLAnim_ChildScale;
            tableB[0].data = 0;
            n2 += 1;
        }
        if (cfg->flags.hasAlphaAnim) {
            tableB[n2].func = SPLAnim_ChildAlpha;
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
                time[0] = (u8)(((s32)(age << 8)) / (s32)lifetime);

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
                    } while (k < (s32)behaviorCount);
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
                    func_0204d9a0(&mgr->freeParticles, func_0204d8e8(&sys->particles2, child));
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
