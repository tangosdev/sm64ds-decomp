//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "cstd.h"

extern "C" {

#pragma opt_common_subs off
#pragma opt_strength_reduction off

/* The 0x24-byte follower func_ov007_020c844c allocates; names as in
 * func_ov007_020c83fc, only the slot this file adds to. */
typedef struct Ov007Follower {
    char pad_00[0x10];
    int force[3]; /* 0x10: accumulated per frame, cleared at the end of each step */
    char pad_1c[8];
} Ov007Follower;

/* A point list as func_ov007_020c798c builds it (names as in
 * func_ov007_020c9a84). */
typedef struct Ov007PointList {
    Ov007Follower **items; /* 0x0 */
    int count;             /* 0x4 */
} Ov007PointList;

/* The 0x10-byte grid func_ov007_020c73f0 builds: rows lists of columns
 * points, and the same points again as columns lists of rows. */
typedef struct Ov007Grid {
    int columns;                  /* 0x00 */
    int rows;                     /* 0x04 */
    Ov007PointList **rowLists;    /* 0x08: one per row, columns points each */
    Ov007PointList **columnLists; /* 0x0c: one per column, rows points each */
} Ov007Grid;

/* data_ov007_02104bd4[0], the 0x38-byte grid mesh func_ov007_020cbbb0
 * builds; names as in func_ov007_020ca5f0, only the slot this file reads. */
typedef struct Ov007Mesh {
    char pad_00[4];
    Ov007Grid *grid; /* 0x04: func_ov007_020c73f0 */
} Ov007Mesh;

/* data_ov007_020d7c9c, the wave template; chances are per mille. */
typedef struct Ov007MeshWaveParams {
    int speed;      /* 0x00: phase advance per tick of the wave's state */
    int spread;     /* 0x04: phase across one whole row or column */
    int amplitude;  /* 0x08: Q12, times the sine */
    u16 minIdle;    /* 0x0c: ticks a wave stays idle (-1) before it may re-roll */
    u16 keepChance; /* 0x0e: chance each tick a running wave keeps pushing */
    u16 idleChance; /* 0x10: chance a re-roll goes idle instead of picking a line */
} Ov007MeshWaveParams;

/* The 0x1c-byte record func_ov007_020ca2a8 builds and func_ov007_020ca1d4
 * ticks: waves[0] runs down a mesh column and waves[1] along a row (2 and 3
 * are ticked but drive nothing). Each wave's currentState is its column or
 * row, -1 when idle, and its stateTicks the sine's time. */
typedef struct Ov007MeshWave {
    int frame;                   /* 0x00: func_ov007_020ca1d4 counts it up every call */
    int delay;                   /* 0x04: no wave runs until it counts down to 0 */
    Ov007MenuState *waves[4];    /* 0x08: func_ov007_020c9388(-2) */
    Ov007MeshWaveParams *params; /* 0x18: func_ov007_020ca2a8's argument */
} Ov007MeshWave;

typedef char Ov007Follower_size_must_be_0x24[sizeof(Ov007Follower) == 0x24 ? 1 : -1];

typedef char Ov007MeshWave_size_must_be_0x1c[sizeof(Ov007MeshWave) == 0x1c ? 1 : -1];

extern short data_02082214[];
extern int func_ov007_020c3ba8(int);
extern void *func_ov007_020c3df4(int a, int b);
extern void *func_ov007_020c9388(int initialState);
extern void func_ov007_020c937c(void *state);
extern int func_ov007_020c3d1c(void *);
extern void func_ov007_020c92d0(int a);

/* Steps wave i over the mesh: while it holds a line and keepChance wins it
 * adds a travelling sine to the z force of every point on that line;
 * otherwise, once any idle time is served, it re-rolls to idle or to a
 * random line. The parameters keep their char * spelling; `wave` and `mesh`
 * are the typed views. */
void func_ov007_020ca308(char *self, char *t, int i) {
    Ov007MeshWave *wave = (Ov007MeshWave *)self;
    Ov007Mesh *mesh = (Ov007Mesh *)t;
    Ov007MenuState *e;
    Ov007MeshWaveParams *h;
    int cur;
    int amp;
    int cnt;
    int a;
    int b;
    int w;
    int timer;
    int j;
    int ph;

    e = wave->waves[i];
    h = wave->params;
    a = h->speed;
    cur = e->currentState;
    b = h->spread;
    amp = h->amplitude;

    if (e->stateTicks == 0)
        wave->waves[i]->stateTicks = func_ov007_020c3ba8(0x3c);
    timer = wave->waves[i]->stateTicks;

    if (i == 0 || i == 2) {
        Ov007Grid *q = mesh->grid;
        cnt = q->rows;
        w = q->columns;
    } else if (i == 1 || i == 3) {
        Ov007Grid *q = mesh->grid;
        cnt = q->columns;
        w = q->rows;
    }

    if (cur >= 0 && cur < w) {
        int c = h->keepChance;
        if (func_ov007_020c3ba8(10000) <= c * 10 - 1) {
            u16 base;
            u16 step = (u16)cstd::div(b, cnt);
            j = 0;
            base = (u16)(timer * a);
            if (cnt <= 0)
                return;
            ph = j;
            do {
                s16 s = data_02082214[((u16)(base + ph) >> 4) * 2];
                int val = FX_MUL(amp, s);
                if (i == 0) {
                    mesh->grid->rowLists[j]->items[cur]->force[2] += val;
                } else if (i == 1) {
                    mesh->grid->rowLists[cur]->items[j]->force[2] += val;
                }
                j++;
                ph += step;
            } while (j < cnt);
            return;
        }
    }

    if (cur == -1) {
        if (cur != -1)
            return;
        if (timer < h->minIdle)
            return;
    }
    {
        int pr = h->idleChance;
        if (func_ov007_020c3ba8(10000) <= pr * 10 - 1) {
            wave->waves[i]->requestedState = -1;
            return;
        }
    }
    wave->waves[i]->requestedState = func_ov007_020c3ba8(w);
}

void *func_ov007_020ca2a8(void *arg) {
    int *obj = (int *)func_ov007_020c3df4(0, 0x1c);
    int i;
    for (i = 0; i < 4; i++) {
        (obj + i)[2] = (int)func_ov007_020c9388(-2);
    }
    obj[6] = (int)arg;
    obj[1] = 0;
    obj[0] = 0;
    return obj;
}

int func_ov007_020ca26c(char *c) {
    int i;
    for (i = 0; i < 4; i++) {
        func_ov007_020c937c((void *)((int *)(c + 8))[i]);
    }
    return func_ov007_020c3d1c(c);
}

void func_ov007_020ca1d4(char *self, int arg1) {
    int i;
    if (*(int *)(self + 4) <= 0) {
        for (i = 0; i < 4; i++) {
            func_ov007_020c92d0(((int *)(self + 8))[i]);
            switch (i) {
            case 0:
            case 1:
                func_ov007_020ca308(self, (char *)arg1, i);
                break;
            case 2:
            case 3:
                break;
            }
        }
    } else {
        (*(int *)(((int)self + 4)))--;
        if (*(int *)(self + 4) <= 0)
            *(int *)(self + 4) = 0;
    }
    (*(int *)(self))++;
}
}
