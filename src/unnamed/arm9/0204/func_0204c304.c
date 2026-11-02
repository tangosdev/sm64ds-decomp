#include "types.h"
#include "private/spl_particle_views.h"

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

extern u32 data_020a4d30;

extern Node *func_0204d958(List *list);
extern void func_0204d9a0(List *list, Node *node);

/* Spawn child->count particles from the parent particle self into sys's
   child list. func_0204a730 calls it for a live particle at the emission
   interval (SPL's child emission). Each child takes the parent's position
   and offset, the parent velocity scaled by velocityRatio plus a random
   spread, and the parent's animated scale times scaleRatio. The other fields
   come from the child record or the parent. */
void func_0204c304(ParticleElement *self, SystemListView *sys, List *freelist) {
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

        p = (ParticleElement *)func_0204d958(freelist);
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
