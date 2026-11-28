/* SPL's small shared utilities: the intrusive list, the random unit vectors and
 * the particle tracking helpers, folded from the per-function shards in reverse
 * ROM emission order. The names are pret's SPL's. */
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_vec.h"
#include "private/spl_particle_views.h"
#include "types.h"

/* The default projection for a tracked particle (func_0204dab4): the
   screen x and y are the position's whole parts, y negated, and the
   weight is 0xfff. */
void func_0204dcc0(int *src, short *o1, short *o2, int *o3) {
    *o1 = (short)(src[0] >> 12);
    *o2 = (short)((-src[1]) >> 12);
    *o3 = 0xfff;
}

/* Clear one 0xc-byte particle tracking record (dPa_c's
 * edStarKiraCallback_c::trackingRecords, and the ov007 sprite list).
 * func_0204dab4 fills them: it binds a record to a particle, has the
 * projection callback (func_0204dcc0 by default) write the screen x / y,
 * stores the callback's weight scaled by the particle's remaining life in
 * fade, keeps the caller's mask in flags bits 0-1 and marks bit 2 for a
 * record it updated this frame. */
typedef struct ParticleTrack {
    short x;             /* 0x00 */
    short y;             /* 0x02 */
    short fade;          /* 0x04 */
    unsigned char flags; /* 0x06 */
    unsigned char pad_07;
    void *particle; /* 0x08 */
} ParticleTrack;

void func_0204dc84(char *self) {
    ParticleTrack *rec = (ParticleTrack *)self;
    rec->flags &= ~3;
    rec->flags &= ~4;
    rec->fade = 0;
    rec->pad_07 = 0;
    rec->y = 0;
    rec->x = rec->y;
    rec->particle = 0;
}

//cpp
#include "nitro/fx/fx.h"
#include "private/spl_particle_views.h"
#include "nitro/fx/fx_cp.h"

/* spl_particle_views.h's record, spelled again so the declaration gate reads
 * TrackingRecord below as a struct tag */
typedef struct ParticleElement ParticleElement;
typedef struct TrackingRecord TrackingRecord;

/* One 0xc-byte tracking record (dPa_c's edStarKiraCallback_c::trackingRecords,
 * func_ov007_020be684's TrackingRecord): the callback writes the particle's
 * 2D point into screenX/screenY and a 20.12 scale this function multiplies
 * by the particle's remaining life into lifeScale. */
struct TrackingRecord {
    short screenX;   /* 0x00 */
    short screenY;   /* 0x02 */
    short lifeScale; /* 0x04 */
    /* 0x06: bits 0-1 the mask the record was claimed with, bit 2 set while
     * this pass sees the particle */
    unsigned char flags;
    unsigned char pad_07;
    ParticleElement *particle; /* 0x08 -- the tracked particle, 0 = free */
};

typedef char ParticleElement_size_must_be_0x44[sizeof(ParticleElement) == 0x44 ? 1 : -1];
typedef char TrackingRecord_size_must_be_0xc[sizeof(TrackingRecord) == 0xc ? 1 : -1];

extern void func_0204dcc0(int *src, short *o1, short *o2, int *o3);

void func_0204dab4(void *sys, TrackingRecord *buf, int count, unsigned int mask, void *cb) {
    int sp[4];
    int i;
    int j;
    ParticleElement *walk;
    TrackingRecord *slot;
    TrackingRecord *p;
    int empty;
    unsigned char *fp6;
    void (*fn)(int *, short *, short *, int *);

    if (cb == 0)
        fn = func_0204dcc0;
    else
        fn = (void (*)(int *, short *, short *, int *))cb;

    walk = ((ParticleSystem *)sys)->particles.head;
    while (walk != 0) {
        slot = 0;
        empty = -1;

        for (i = 0; i < count; i++) {
            ParticleElement *n = buf[i].particle;

            if (n != 0) {
                if (walk == n) {
                    slot = &buf[i];
                    break;
                }
            } else if (empty == -1) {
                empty = i;
            }
        }

        if (slot == 0 && empty != -1 && mask < 4) {
            slot = &buf[empty];
            slot->particle = walk;
            fp6 = &slot->flags;
            *fp6 = ((*fp6 & ~3u) | (mask & 3u));
        }

        if (slot != 0) {
            int scale;

            scale = 0x1000 - FX_DivS32(walk->age << 12, walk->lifetime);

            sp[1] = walk->offset.x + walk->basePosition.x;
            sp[2] = walk->offset.y + walk->basePosition.y;
            sp[3] = walk->offset.z + walk->basePosition.z;

            fn(&sp[1], &slot->screenX, &slot->screenY, &sp[0]);

            slot->lifeScale = FX_MUL(sp[0], scale);
            fp6 = &slot->flags;
            *fp6 = (*fp6 | 4u);
        }

        walk = walk->next;
    }

    j = 0;
    p = buf;

    if (count <= 0)
        return;

    do {
        if ((((unsigned int)p->flags << 29) >> 31) == 0) {
            if (p->particle != 0)
                p->particle = 0;
        }

        fp6 = &p->flags;
        *fp6 = (*fp6 & ~4u);
        p++;
    } while (++j < count);
}

#include "types.h"
#include "nitro/fx/fx_vec.h"
extern s32 PARTICLE_RNG_STATE;

/* A random unit vector: three draws from the SPL LCG, each shifted down 8,
 * for x, y and z, then normalized. The name is pret's SPL's
 * (its spl_random.c, pret-coined). */
void SPLRandom_VecFx32(VecFx32 *out) {
    s32 s = PARTICLE_RNG_STATE;
    s32 x = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    PARTICLE_RNG_STATE = x;
    out->x = x >> 8;
    s = PARTICLE_RNG_STATE;
    s32 y = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    PARTICLE_RNG_STATE = y;
    out->y = y >> 8;
    s = PARTICLE_RNG_STATE;
    s32 z = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    PARTICLE_RNG_STATE = z;
    out->z = z >> 8;
    VEC_Normalize(out, out);
}

#include "types.h"
#include "nitro/fx/fx_vec.h"
extern u32 gSPLRandomState;

/* A random unit vector in the XY plane: two draws from the SPL LCG, each
 * shifted down 8, for x and y, z = 0, then normalized. The name is pret's
 * SPL's (its spl_random.c, pret-coined); pret's file defines
 * SPLRandom_VecFx32 first, and mwccarm emits in reverse source order. */
void SPLRandom_VecFx32_XY(VecFx32 *out) {
    s32 s = gSPLRandomState;
    s32 x = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    gSPLRandomState = x;
    out->x = x >> 8;
    s = gSPLRandomState;
    s32 y = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    gSPLRandomState = y;
    out->y = y >> 8;
    out->z = 0;
    VEC_Normalize(out, out);
}

/* The particle engine's intrusive list; see SPLList_Erase. */
typedef struct Node {
    struct Node *next; /* 0x0 */
    struct Node *prev; /* 0x4 */
} Node;

typedef struct List {
    Node *head; /* 0x0 */
    int count;  /* 0x4 */
} List;

/* Push node onto the front of list. */
void SPLList_PushFront(List *list, Node *node) {
    Node *old = list->head;
    if (old == 0) {
        list->head = node;
        node->prev = node->next = 0;
    } else {
        node->next = old;
        node->prev = 0;
        list->head->prev = node;
        list->head = node;
    }
    list->count++;
}

/* The particle engine's intrusive list; see SPLList_Erase. */

/* Pop the head of list, or 0 when it is empty. */
Node *SPLList_PopFront(List *list) {
    Node *popped = 0;
    Node *head = list->head;
    if (head != 0) {
        popped = head;
        list->head = head->next;
        if (list->head != 0) {
            head->next->prev = 0;
        }
        list->count--;
    }
    return popped;
}

/* The particle engine's two-word intrusive list (SPLList_PopFront pops,
   SPLList_PushFront pushes): each element starts with next/prev links, whatever
   its type (Particle::System, ParticleNode, a free-pool node). */

/* Unlink n from l and return it. */
Node *SPLList_Erase(List *l, Node *n) {
    if (n->next == 0) {
        if (l->head == n)
            l->head = 0;
        else
            n->prev->next = 0;
    } else {
        if (l->head == n) {
            l->head = n->next;
            l->head->prev = 0;
        } else {
            n->next->prev = n->prev;
            n->prev->next = n->next;
        }
    }
    l->count--;
    return n;
}
