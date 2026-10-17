#include "types.h"
#include "nitro/fx/fx.h"
/* func_0204a730 @ 0x0204a730 (arm9, size 0x6fc)   [mwccarm 1.2/sp2p3]
 * Levers that matched (notes 6ap): group B = u32 byte-hoist at function scope with
 * s32 k demoted into the loop block after var_r5; group A = flags declared plain u32
 * but WRITTEN through a volatile cast - the volatile-touched object lands in the
 * stack hole at [sp,#0x2c] while the plain reads stay value-numberable, which blocks
 * the lsl/lsr re-fold. Do not "simplify" the volatile store or the shifts re-fold.
 */
typedef struct { s32 x, y, z; } Vec3;

extern void func_0204c584(void *self, void *arg);
extern void func_0204c304(void *node, void *self, void *arg);
extern void *func_0204d8e8(void *listHead, void *node);
extern void func_0204d9a0(void *list, void *node);

typedef void (*Cb1)(void *, void *, u32);
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
 * animations, child resource, behaviors), and the names follow SPL's. */
typedef struct PtclNode {
    struct PtclNode *next; /* 0x00 */
    void *prev;            /* 0x04 */
    Vec3 basePosition;     /* 0x08 */
    Vec3 offset;           /* 0x14 */
    Vec3 velocity;         /* 0x20 */
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

typedef struct {
    u32 flags; /* 0x00 */
    u8 pad_04[0x24];
    u16 emitterLifeTime; /* 0x28 -- emission stops once the system's age reaches it; 0 never stops */
    u8 pad_2a[0x8];
    u8 airResistance; /* 0x32 -- velocities scale by (airResistance + 0x180) / 0x200 each frame */
} PtclData;

typedef struct {
    u16 flags; /* 0x00 */
    u8 pad_02[0xb];
    u8 emissionDelay; /* 0x0d -- children start (emissionDelay + 1) / 256 into the parent's life */
    u8 emissionInterval; /* 0x0e -- then every emissionInterval frames */
} PtclChild;

typedef struct {
    PtclData *data;    /* 0x00 */
    char *scaleAnim;   /* 0x04 */
    char *colorAnim;   /* 0x08 */
    char *alphaAnim;   /* 0x0c */
    char *texAnim;     /* 0x10 */
    PtclChild *child;  /* 0x14 */
    Ent2 *behaviors;   /* 0x18 -- each adds its push into a particle's acceleration */
    u16 behaviorCount; /* 0x1c */
} PtclDef;

typedef struct {
    void *previous;      /* 0x00 */
    void *next;          /* 0x04 */
    PtclList particles;  /* 0x08 */
    PtclList particles2; /* 0x10 */
    PtclDef *def;        /* 0x18 */
    s32 callbackFlags;   /* 0x1c */
    Vec3 position;       /* 0x20 */
    Vec3 velocity;       /* 0x2c -- added to every particle's offset each frame */
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

extern void func_0204d440(void *, void *, u32);
extern void func_0204d294(void *, void *, u32);
extern void func_0204d1b4(void *, void *, u32);
extern void func_0204d150(void *, void *, u32);
extern void func_0204d104(void *, void *, u32);
extern void func_0204d0b8(void *, void *, u32);

#define LP(T, e) ((T *)(int)(e))

#pragma opt_common_subs off

void func_0204a730(void *arg0, void *self)
{

    PtclDef *sb;
    void *next;
    PtclChild *cfg;
    s32 spC;
    s32 sp10;
    s32 sp14;
    void *next2;
    s32 zero_home;
    u32 flags;
    u16 thresh;
    u16 count2;
    void *var_r4;
    u8 spArr[2];
    Ent1 table1[4];
    Ent1 tableB[4];
    s32 acc[3];
    PtclData *data;
    u32 bb;
    s32 i;
    s32 n;
    PtclSystem *sys = (PtclSystem *)self;
    PtclManager *mgr = (PtclManager *)arg0;

    sb = sys->def;
    cfg = sb->child;
    data = sb->data;
    *(volatile u32 *)&flags = data->flags;
    i = 0;
    n = 0;
    spC = (s32)data->airResistance + 0x180;
    thresh = data->emitterLifeTime;
    count2 = sb->behaviorCount;

    if (thresh == 0 || (u16)sys->age < thresh) {
        if ((u16)sys->age % (u8)sys->emitInterval == 0) {
            if (((u32)(sys->callbackFlags << 0x1f) >> 0x1f) == 0 &&
                ((u32)(sys->callbackFlags << 0x1e) >> 0x1f) == 0) {
                func_0204c584(self, &mgr->freeParticles);
            }
        }
    }

    if (((u32)(flags << 0x17) >> 0x1f) != 0) {
        table1[0].func = func_0204d440;
        n += 1;
        {
            char *t = sb->scaleAnim;
            table1[0].data = (u32)((u32)(*(u16 *)(t + 8) << 0x1f) >> 0x1f);
        }
    }
    if (((u32)(flags << 0x16) >> 0x1f) != 0) {
        char *t = sb->colorAnim;
        if (((u32)(*(u16 *)(t + 8) << 0x1f) >> 0x1f) == 0) {
            s32 idx = n;
            table1[idx].func = func_0204d294;
            {
                u32 d = (u32)((u32)(*(u16 *)(t + 8) << 0x1e) >> 0x1f);
                n = idx + 1;
                table1[idx].data = d;
            }
        }
    }
    if (((u32)(flags << 0x15) >> 0x1f) != 0) {
        s32 idx = n;
        table1[idx].func = func_0204d1b4;
        {
            char *t = sb->alphaAnim;
            u32 d = (u32)((u32)(*(u16 *)(t + 2) << 0x17) >> 0x1f);
            n = idx + 1;
            table1[idx].data = d;
        }
    }
    if (((u32)(flags << 0x14) >> 0x1f) != 0) {
        char *t = sb->texAnim;
        if (((u32)(*(s32 *)(t + 8) << 0xf) >> 0x1f) == 0) {
            table1[n].func = func_0204d150;
            table1[n].data = (u32)((u32)(*(s32 *)(t + 8) << 0xe) >> 0x1f);
            n += 1;
        }
    }

    var_r4 = sys->particles.head;
    if (var_r4 != 0) {
        u32 f = flags;
        sp10 = (s32)((u32)(f << 0x10) >> 0x1f);
        sp14 = (s32)((u32)(f << 0xf) >> 0x1f);
        zero_home = 0;
        do {
            PtclNode *nodeC = (PtclNode *)var_r4;
            u16 ratio;
            s32 j;

            next = nodeC->next;
            ratio = nodeC->age;

            spArr[0] = (u8)((s32)(nodeC->lifeRateFactor * ratio) >> 8);
            spArr[1] = (u8)((s32)(nodeC->loopTimeFactor * ratio) >> 8);

            i = zero_home;
            if (n > 0) {
                do {
                    table1[i].func(var_r4, sb, spArr[table1[i].data]);
                    i += 1;
                } while (i < n);
            }

            j = zero_home;
            acc[2] = zero_home;
            acc[1] = zero_home;
            acc[0] = zero_home;

            if (sp10 != 0) {
                nodeC->basePosition = sys->position;
            }

            if ((s32)count2 > 0) {
                do {
                    Ent2 *tbl2 = sb->behaviors;
                    tbl2[j].func(tbl2[j].data, var_r4, &acc[0]);
                    j += 1;
                } while (j < (s32)count2);
            }

            {
                u16 *pAng = LP(u16, &nodeC->angle);
                *pAng = (u16)(*pAng + nodeC->angularSpeed);
            }
            nodeC->velocity.x = (nodeC->velocity.x * spC) >> 9;
            nodeC->velocity.y = (nodeC->velocity.y * spC) >> 9;
            nodeC->velocity.z = (nodeC->velocity.z * spC) >> 9;
            {
                s32 *pvx = LP(s32, &nodeC->velocity.x);
                s32 *pvy = LP(s32, &nodeC->velocity.y);
                s32 *pvz = LP(s32, &nodeC->velocity.z);
                *pvx = *pvx + acc[0];
                *pvy = *pvy + acc[1];
                *pvz = *pvz + acc[2];
            }
            {
                s32 *ppx = LP(s32, &nodeC->offset.x);
                s32 *ppy = LP(s32, &nodeC->offset.y);
                s32 *ppz = LP(s32, &nodeC->offset.z);
                *ppx = *ppx + (nodeC->velocity.x + sys->velocity.x);
                *ppy = *ppy + (nodeC->velocity.y + sys->velocity.y);
                *ppz = *ppz + (nodeC->velocity.z + sys->velocity.z);
            }

            if (sp14 != 0) {
                u16 life = nodeC->age;
                s32 a = (s32)nodeC->lifetime << 0xc;
                s32 b = ((s32)cfg->emissionDelay + 1) << 0xc;
                s32 r1 = FX_MUL(a, b);
                s32 r0 = ((s32)life << 0xc) - (r1 >> 8);
                if (life != 0 && r0 >= 0 && ((r0 >> 0xc) % (u8)cfg->emissionInterval) == 0) {
                    func_0204c304(var_r4, self, &mgr->freeParticles);
                }
            }

            {
                u16 *pT = LP(u16, &nodeC->age);
                *pT = (u16)(*pT + 1);
            }
            if ((u16)nodeC->age > (u16)nodeC->lifetime) {
                func_0204d9a0(&mgr->freeParticles, func_0204d8e8(&sys->particles, var_r4));
            }

            var_r4 = next;
        } while (next != 0);
    }

    if (((u32)(flags << 0xf) >> 0x1f) != 0) {
        s32 n2 = 0;
        u16 cflags;
        void *var_r5;
        s32 k;

        cflags = cfg->flags;
        if (((u32)(cflags << 0x1e) >> 0x1f) != 0) {
            tableB[0].func = func_0204d104;
            tableB[0].data = 0;
            n2 += 1;
        }
        cflags = cfg->flags;
        if (((u32)(cflags << 0x1d) >> 0x1f) != 0) {
            tableB[n2].func = func_0204d0b8;
            tableB[n2].data = 0;
            n2 += 1;
        }
        cflags = cfg->flags;
        var_r5 = sys->particles2.head;
        if (((u32)(cflags << 0x1f) >> 0x1f) == 0) {
            count2 = 0;
        }

        if (var_r5 != 0) {
            zero_home = 0;
            do {
                PtclNode *nodeC = (PtclNode *)var_r5;
                u16 e2c;
                u16 e2e;

                e2e = nodeC->age;
                next2 = nodeC->next;
                e2c = nodeC->lifetime;
                spArr[0] = (u8)(((s32)(e2e << 8)) / (s32)e2c);

                k = zero_home;
                if (n2 > 0) {
                    bb = spArr[0];
                    do {
                        tableB[k].func(var_r5, sb, bb);
                        k += 1;
                    } while (k < n2);
                }

                k = zero_home;
                acc[2] = zero_home;
                acc[1] = zero_home;
                acc[0] = zero_home;

                if (((u32)(cfg->flags << 0x1a) >> 0x1f) != 0) {
                    nodeC->basePosition = sys->position;
                }

                if ((s32)count2 > 0) {
                    do {
                        Ent2 *tbl2 = sb->behaviors;
                        tbl2[k].func(tbl2[k].data, var_r5, &acc[0]);
                        k += 1;
                    } while (k < (s32)count2);
                }

                {
                    u16 *pAng = LP(u16, &nodeC->angle);
                    *pAng = (u16)(*pAng + nodeC->angularSpeed);
                }
                nodeC->velocity.x = (nodeC->velocity.x * spC) >> 9;
                nodeC->velocity.y = (nodeC->velocity.y * spC) >> 9;
                nodeC->velocity.z = (nodeC->velocity.z * spC) >> 9;
                {
                    s32 *pvx = LP(s32, &nodeC->velocity.x);
                    s32 *pvy = LP(s32, &nodeC->velocity.y);
                    s32 *pvz = LP(s32, &nodeC->velocity.z);
                    *pvx = *pvx + acc[0];
                    *pvy = *pvy + acc[1];
                    *pvz = *pvz + acc[2];
                }
                {
                    s32 *ppx = LP(s32, &nodeC->offset.x);
                    s32 *ppy = LP(s32, &nodeC->offset.y);
                    s32 *ppz = LP(s32, &nodeC->offset.z);
                    *ppx = *ppx + (nodeC->velocity.x + sys->velocity.x);
                    *ppy = *ppy + (nodeC->velocity.y + sys->velocity.y);
                    *ppz = *ppz + (nodeC->velocity.z + sys->velocity.z);
                }
                {
                    u16 *pT = LP(u16, &nodeC->age);
                    *pT = (u16)(*pT + 1);
                }

                if ((u16)nodeC->age > (u16)nodeC->lifetime) {
                    func_0204d9a0(&mgr->freeParticles, func_0204d8e8(&sys->particles2, var_r5));
                }

                var_r5 = next2;
            } while (next2 != 0);
        }
    }

    {
        u16 *pF = LP(u16, &sys->age);
        *pF = (u16)(*pF + 1);
    }

    {
        void (*cb)(void *) = sys->callback;
        if (cb == 0) {
            return;
        }
        cb(self);
    }
}
