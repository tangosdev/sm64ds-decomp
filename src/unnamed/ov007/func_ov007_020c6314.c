// @symbol func_ov007_020c6314
/* recovered: shared common types */
#include "common.h"

/* A point and a spring between two points, as func_ov007_020c5a6c spells them. */
typedef struct Ov007Point {
    struct Vector3 *pos; /* 0x0 */
} Ov007Point;

typedef struct Ov007Link {
    Ov007Point *from;   /* 0x0 */
    Ov007Point *to;     /* 0x4 */
    int length;         /* 0x8 */
    struct Vector3 dir; /* 0xc */
} Ov007Link;

/* A committed stroke; names as in include/Ov007StrokePath.h, only the count. */
typedef struct Ov007StrokePath {
    char pad_00[8];
    unsigned short pointCount; /* 0x08 */
} Ov007StrokePath;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c684c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[0x28];
    struct Vector3 **groupPoints;      /* 0x28 */
    struct Vector3 **groupPointCopies; /* 0x2c */
    char pad_30[4];
    Ov007Link ***groupLinks;  /* 0x34: per group, one link per point from posPoint */
    Ov007StrokePath **groups; /* 0x38 */
    char pad_3c[0x40];
    struct Vector3 restPos; /* 0x7c */
} Ov007Picker;

extern void SubVec3(struct Vector3* a, struct Vector3* b, struct Vector3* c);
extern int LenVec3(struct Vector3* v);

/* Re-measures group idx's links: from restPos to each rest point (flag 0),
 * between their own end points (flag 1), or from restPos to each live point
 * (flag 2). The parameter keeps the tree's char * spelling (the
 * declaration-agreement gate); the typed view is a local. */
void func_ov007_020c6314(char* t, int idx, int flag)
{
    Ov007Picker *self = (Ov007Picker *)t;
    int i;
    Ov007Link *e;

    switch (flag) {
    case 0:
        for (i = 0; i < self->groups[idx]->pointCount; i++) {
            e = self->groupLinks[idx][i];
            SubVec3(&self->restPos, &self->groupPointCopies[idx][i], &e->dir);
            e->length = LenVec3(&e->dir);
        }
        break;
    case 1:
        for (i = 0; i < self->groups[idx]->pointCount; i++) {
            e = self->groupLinks[idx][i];
            SubVec3(e->from->pos, e->to->pos, &e->dir);
            e->length = LenVec3(&e->dir);
        }
        break;
    case 2:
        for (i = 0; i < self->groups[idx]->pointCount; i++) {
            e = self->groupLinks[idx][i];
            SubVec3(&self->restPos, &self->groupPoints[idx][i], &e->dir);
            e->length = LenVec3(&e->dir);
        }
        break;
    }
}
