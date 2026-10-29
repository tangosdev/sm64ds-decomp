// @symbol func_ov007_020c6e68
#include "common.h"
#include "nitro/fx/fx.h"
#include "Ov007StrokePath.h"

/* Builds the per-group render state for path group `idx`: allocates the
 * group's node, three per-point arrays (position source, position copy, and
 * the per-point handles), then fills the copies point by point and appends two
 * trailing handles.
 *
 * The flag word starts at 1 and gains bit 2 when the group's own scale
 * (func_ov007_020c249c, times the Q12 factor at params+0x1c) is at
 * least the horizontal distance from the group's first point to its last one.
 * Byte-exact under 2004/b56. The projected scale has to be computed BEFORE
 * the dx/dz differences: with the multiply after them the compiler colours the
 * two pointer loads the other way round and the whole block moves. The
 * per-point addresses stay int base + byte offset: char * bases lengthen the
 * function by 12 bytes. */

extern int func_ov007_020c249c(void *grp);
extern int func_020531a4(int a);
extern void *func_ov007_020c798c(int a, int b, void *c, int d);
extern int func_ov007_020c3df4(int a, void *b);
extern void *func_ov007_020c80a4(void);
extern void func_ov007_020c8010(void *a, int *b, int *c);
extern void func_ov007_020bfe4c(char *thiz, int a, int b, int c, Vector3 *out);
extern void func_ov007_020c7804(void *self);

/* A point and a point list as func_ov007_020c5a6c spells them. */
typedef struct Ov007Point {
    Vector3 *pos; /* 0x0 */
} Ov007Point;

typedef struct Ov007PointList {
    Ov007Point **items; /* 0x0 */
} Ov007PointList;

/* The overlay's camera (func_ov007_020c023c); only the slot this file reads. */
typedef struct Ov007Camera {
    char pad_00[0x2c];
    int distance; /* 0x2c: func_ov007_020bffb8 places the eye this far from the target */
} Ov007Camera;

/* The 0xd0-byte touch picker func_ov007_020c690c builds (names as in that
 * file). params' first word (handed to func_ov007_020c798c) and its +0x1c
 * factor have no reader that names them and stay raw. */
typedef struct Ov007Picker {
    char pad_00[0x1c];
    char *params; /* 0x1c: a4 of func_ov007_020c690c */
    char pad_20[8];
    Vector3 **groupPoints;      /* 0x28: per group, the points unprojected through camera */
    Vector3 **groupPointCopies; /* 0x2c: per group, a copy of groupPoints */
    char pad_30[4];
    void ***groupLinks;          /* 0x34: per group, one link per point from posPoint */
    Ov007StrokePath **groups;    /* 0x38: committed strokes (func_ov007_020c6d20) */
    Ov007PointList **pointLists; /* 0x3c: func_ov007_020c798c, per group */
    /* 0x40: per group, the link appended here after the per-point ones;
     * func_ov007_020c6174 measures it from the group's first point */
    void **headLinks;
    void **tailLinks; /* 0x44: likewise from the group's last point */
    char pad_48[0x24];
    void *posPoint; /* 0x6c: func_ov007_020c844c's Point over pos */
    char pad_70[0x38];
    Ov007Camera *camera; /* 0xa8 */
} Ov007Picker;

typedef char Ov007Picker_camera_must_be_at_0xa8[(int)&((Ov007Picker *)0)->camera == 0xa8 ? 1 : -1];

/* The parameter keeps func_ov007_020c6d20's char * spelling (the
 * declaration-agreement gate); the typed view is a local. */
void func_ov007_020c6e68(char *picker, int idx) {
    Ov007Picker *self = (Ov007Picker *)picker;
    Ov007StrokePath *grp;
    char *fp;
    int flag;
    int count;
    int i;
    Ov007PointList *sb;
    int scale;
    int *px, *pz;
    int last;
    int dx, dz, proj, lensq;
    char *owner;

    grp = self->groups[idx];
    fp = self->params;
    count = grp->pointCount;
    flag = 1;
    scale = func_ov007_020c249c(grp);

    owner = self->params;
    px = grp->xCoords;
    pz = grp->yCoords;
    last = count - 1;
    proj = FX_MUL(scale, *(int *)(owner + 0x1c));
    dx = px[0] - px[last];
    dz = pz[0] - pz[last];
    lensq = FX_MUL(dx, dx) + FX_MUL(dz, dz);
    if (func_020531a4(lensq) <= proj)
        flag |= 2;

    {
        void *node = func_ov007_020c798c(2, grp->pointCount, *(void **)fp, flag);
        flag = count * 0xc;
        self->pointLists[idx] = node;
    }
    sb = self->pointLists[idx];

    self->groupPoints[idx] = (Vector3 *)func_ov007_020c3df4(0, (void *)flag);
    self->groupPointCopies[idx] = (Vector3 *)func_ov007_020c3df4(0, (void *)flag);
    self->groupLinks[idx] = (void **)func_ov007_020c3df4(0, (void *)(count << 2));

    i = 0;
    if (count > 0) {
        flag = 0;
        do {
            void *p = func_ov007_020c80a4();
            self->groupLinks[idx][i] = p;
            func_ov007_020c8010(p, (int *)self->posPoint, (int *)sb->items[i]);
            {
                int base = (int)self->groupPoints[idx];
                sb->items[i]->pos = (Vector3 *)(base + flag);
            }
            {
                Ov007Camera *a8 = self->camera;
                int base = (int)self->groupPoints[idx];
                Vector3 *out = (Vector3 *)(base + flag);
                func_ov007_020bfe4c((char *)a8, grp->xCoords[i], grp->yCoords[i], -a8->distance,
                                    out);
            }
            {
                int base_d = (int)self->groupPointCopies[idx];
                int base_s = (int)self->groupPoints[idx];
                Vector3 *d = (Vector3 *)(base_d + flag);
                Vector3 *s = (Vector3 *)(base_s + flag);
                flag += 0xc;
                i++;
                *d = *s;
            }
        } while (i < count);
    }

    self->headLinks[idx] = func_ov007_020c80a4();
    self->tailLinks[idx] = func_ov007_020c80a4();
    func_ov007_020c7804(sb);
}
