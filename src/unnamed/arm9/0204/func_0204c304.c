#include "types.h"

typedef struct Node {
    struct Node *next;
    struct Node *prev;
} Node;
typedef struct List {
    Node *head;
    int count;
} List;

/* The particle's 0x40 halfword (SPL's visibility). */
typedef struct Visibility {
    u16 baseAlpha : 5;
    u16 animAlpha : 5;
    u16 rest : 6;
} Visibility;

/* Particle::Element (include/Particle__Element.h) and the fields past its
   0x30 bytes, named as in func_0204c584 (the parent spawn). */
typedef struct Particle {
    u8 pad0[8];            /* 0x0 */
    Vector3 basePosition;  /* 0x8 */
    Vector3 offset;        /* 0x14 */
    Vector3 velocity;      /* 0x20 */
    u16 lifetime;          /* 0x2c */
    u16 age;               /* 0x2e */
    int scale;             /* 0x30 */
    s16 animScale;         /* 0x34 */
    u16 angle;             /* 0x36 */
    s16 angularSpeed;      /* 0x38 */
    u16 color;             /* 0x3a */
    u16 loopTimeFactor;    /* 0x3c */
    u16 lifeRateFactor;    /* 0x3e */
    Visibility visibility; /* 0x40 */
    u8 texture;            /* 0x42 */
} Particle;

/* The child record's flags (SPL's SPLChildResourceFlags). rotationType 0
   starts a child unrotated, 1 copies the parent's angle, 2 also its angular
   speed. useChildColor takes the record's color over the parent's. */
typedef struct DefFlags {
    u16 pad0 : 3;          /* bits 0-2 */
    u16 rotationType : 2;  /* bits 3-4 */
    u16 pad5 : 1;          /* bit 5 */
    u16 useChildColor : 1; /* bit 6 */
    u16 pad7 : 9;          /* bits 7-15 */
} DefFlags;

/* The child record (func_0204a730's PtclChild; SPL's SPLChildResource).
   func_0204d104 reads endScale; func_0204a730 the emission delay and
   interval at 0xd/0xe. */
typedef struct Def {
    DefFlags flags;   /* 0x0 */
    s16 spread;       /* 0x2 -- random velocity spread */
    s16 endScale;     /* 0x4 */
    u16 lifetime;     /* 0x6 */
    u8 velocityRatio; /* 0x8 -- a child inherits (velocityRatio + 1) / 256 of the velocity */
    u8 scaleRatio;    /* 0x9 -- and (scaleRatio + 1) / 64 of the scale */
    u16 color;        /* 0xa */
    u8 count;         /* 0xc */
    u8 padd[2];       /* 0xd */
    u8 texture;       /* 0xf */
} Def;

/* Particle::SystemDefinition (include/Particle__Manager.h): child is the
   0x14-byte child record, present when data's flags bit 16 is set. */
typedef struct SystemDefinition {
    void *data;   /* 0x00 */
    void *opt[4]; /* 0x04 */
    Def *child;   /* 0x14 */
} SystemDefinition;

/* Particle::System (include/Particle__System.h): the second particle list
   holds the children. */
typedef struct ParticleSystem {
    u8 pad_00[0x10];
    List children;                 /* 0x10 */
    SystemDefinition *mDefinition; /* 0x18 */
} ParticleSystem;

extern u32 data_020a4d30;

extern Node *func_0204d958(List *list);
extern void func_0204d9a0(List *list, Node *node);

/* Spawn child->count particles from the parent particle self into sys's
   child list. func_0204a730 calls it for a live particle at the emission
   interval (SPL's child emission). Each child takes the parent's position
   and offset, the parent velocity scaled by velocityRatio plus a random
   spread, and the parent's animated scale times scaleRatio. The other fields
   come from the child record or the parent. */
void func_0204c304(Particle *self, ParticleSystem *sys, List *freelist) {
    Particle *p;
    Def *def;
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

        p = (Particle *)func_0204d958(freelist);
        if (p == 0)
            return;

        func_0204d9a0(&sys->children, (Node *)p);

        p->offset = self->offset;

        ratio = def->velocityRatio + 1;

        data_020a4d30 = data_020a4d30 * rngA + rngC;
        p->velocity.x = ((self->velocity.x * ratio) >> 8) +
                        ((def->spread * (int)(data_020a4d30 >> 0x17) - (def->spread << 8)) >> 8);

        data_020a4d30 = data_020a4d30 * rngA + rngC;
        p->velocity.y = ((self->velocity.y * ratio) >> 8) +
                        ((def->spread * (int)(data_020a4d30 >> 0x17) - (def->spread << 8)) >> 8);

        data_020a4d30 = data_020a4d30 * rngA + rngC;
        p->velocity.z = ((self->velocity.z * ratio) >> 8) +
                        ((def->spread * (int)(data_020a4d30 >> 0x17) - (def->spread << 8)) >> 8);

        p->basePosition = self->basePosition;

        {
            int c = self->scale;
            int m = (c * self->animScale) >> 12;
            p->scale = (m * (def->scaleRatio + 1)) >> 6;
        }

        p->animScale = (u16)d1c;

        if (def->flags.useChildColor)
            p->color = def->color;
        else
            p->color = self->color;

        {
            Visibility *q = &p->visibility;
            q->baseAlpha =
                (u16)((self->visibility.baseAlpha * (self->visibility.animAlpha + 1)) >> 5);
            q->animAlpha = 0x1f;
        }

        {
            int mode = def->flags.rotationType;
            switch (mode) {
            case 0:
                p->angle = (u16)eB;
                p->angularSpeed = (s16)eB;
                break;
            case 1:
                p->angle = self->angle;
                p->angularSpeed = (s16)eA;
                break;
            case 2:
                p->angle = self->angle;
                p->angularSpeed = self->angularSpeed;
                break;
            }
        }

        p->lifetime = def->lifetime;
        p->age = (u16)eC;
        p->texture = def->texture;
        p->loopTimeFactor = 0xffff / (int)((u32)self->lifetime >> 1);
        p->lifeRateFactor = 0xffff / self->lifetime;

        i += 1;
    } while (i < def->count);
}
