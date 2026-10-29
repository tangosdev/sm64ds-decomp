//cpp
#include "types.h"
#include "cstd.h"

extern "C" {

/* SubVec3 still defines its vector as its own struct Vec3;
   declared as defined, the calls cast. */
struct Vec3;
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern int func_020538b8(int x, int y);

/* A point list as func_ov007_020c798c builds it; only the count is read. */
typedef struct Ov007PointList {
    void **items; /* 0x0 */
    int count;    /* 0x4 */
} Ov007PointList;

/* The template func_ov007_020ca010 builds every Ov007Swing from
 * (data_ov007_020d7cc8); names as in func_ov007_020c9c24, only the slots
 * this file reads. */
typedef struct Ov007SwingParams {
    char pad_00[0x28];
    /* 0x28/0x2a: the largest rotX/rotY this file hands the item sprite
     * (func_ov007_020beeb0 passes &rotX/&rotY); the side pairs' slopes are
     * eased up to them */
    u16 maxRotX;
    u16 maxRotY;
} Ov007SwingParams;

/* The 0x70-byte record func_ov007_020ca010 builds; names as in
 * func_ov007_020c9c24, only the slots this file reads. */
typedef struct Ov007Swing {
    char pad_00[4];
    Ov007SwingParams *params; /* 0x04 */
    char pad_08[0x10];
    /* 0x18: one position per chain point, z = index << 12 at build; each
     * chain Point's pos points into it (func_ov007_020ca010) */
    Vector3 *chainPositions;
    Ov007PointList *chain; /* 0x1c: func_ov007_020c798c point list */
    /* 0x20: xPoints[i]'s positions, seeded i << 12 apart along x
     * (func_ov007_020ca010) */
    Vector3 xPointPos[2];
    Vector3 yPointPos[2]; /* 0x38: yPoints[i]'s, i << 12 apart along y */
} Ov007Swing;

typedef char Ov007Swing_size_must_be_0x50[sizeof(Ov007Swing) == 0x50 ? 1 : -1];
typedef char Ov007SwingParams_size_must_be_0x2c[sizeof(Ov007SwingParams) == 0x2c ? 1 : -1];

/* Turns the swing into the item sprite's pose: w gets the chain's last
 * point (x, y), and *ox/*oy the x and y side pairs' slopes, eased and capped
 * at maxRotX/maxRotY. o keeps the char * spelling of the callers'
 * declarations; `self` is the typed view. */
void func_ov007_020c9a84(char *o, int *w, short *ox, short *oy)
{
    Ov007Swing *self = (Ov007Swing *)o;
    Vector3 d;
    int ra, rb;
    short a1, a2;
    int aa, ab, t1, t2, q1, q2;

    w[0] = self->chainPositions[self->chain->count - 1].x;
    {
        Ov007PointList *p = self->chain;
        Vector3 *base = self->chainPositions;
        int n = p->count - 1;
        w[1] = base[n].y;
    }
    SubVec3((struct Vec3 *)&self->xPointPos[0], (struct Vec3 *)&self->xPointPos[1],
            (struct Vec3 *)&d);
    ra = func_020538b8(d.x, d.z);
    SubVec3((struct Vec3 *)&self->yPointPos[0], (struct Vec3 *)&self->yPointPos[1],
            (struct Vec3 *)&d);
    rb = func_020538b8(d.z, d.y);
    a1 = (unsigned short)(ra + 0x3fff);
    a2 = (unsigned short)(rb + 0x7fff);
    aa = (a1 >= 0) ? a1 : -a1;
    {
        unsigned short lim = self->params->maxRotX;
        if (aa <= 0) t1 = 0;
        else if (aa >= lim) t1 = 0x1000;
        else
            t1 = cstd::div(aa << 12, lim);
    }
    ab = (a2 >= 0) ? a2 : -a2;
    {
        unsigned short lim = self->params->maxRotY;
        if (ab <= 0) t2 = 0;
        else if (ab >= lim) t2 = 0x1000;
        else
            t2 = cstd::div(ab << 12, lim);
    }
    q1 = (int)(((long long)t1 * (0x2000 - t1)) >> 12);
    q2 = (int)(((long long)t2 * (0x2000 - t2)) >> 12);
    *ox = (short)((self->params->maxRotX * (q1 * ((a1 >= 0) ? 1 : -1))) >> 12);
    *oy = (short)((self->params->maxRotY * (q2 * ((a2 >= 0) ? 1 : -1))) >> 12);
}
}
