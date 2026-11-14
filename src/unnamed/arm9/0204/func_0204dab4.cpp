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
extern "C" {

extern void func_0204dcc0(int *src, short *o1, short *o2, int *o3);
}

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

            scale = 0x1000 - FX_DivS32((int)walk->age << 12, walk->lifetime);

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
