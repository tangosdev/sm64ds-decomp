#include "types.h"
// @symbol func_ov007_020c5fc8
/* recovered: shared common types */
#include "common.h"
#include "Ov007StrokePath.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int LenVec3(struct Vector3 *v);

/* A spring between two points, as func_ov007_020c55bc spells it (0x18
 * bytes, func_ov007_020c80a4). */
typedef struct Ov007Link {
    int from;           /* 0x0 */
    int to;             /* 0x4 */
    int length;         /* 0x8 */
    struct Vector3 dir; /* 0xc */
} Ov007Link;

/* A point list as func_ov007_020c798c builds it. */
typedef struct Ov007PointList {
    void **items; /* 0x0 */
    int count;    /* 0x4 */
    /* 0x8: func_ov007_020c80a4 links between consecutive points (count - 1,
     * or count when the list is closed) */
    Ov007Link **links;
} Ov007PointList;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[0x28];
    struct Vector3 **groupPoints;      /* 0x28: func_ov007_020c6e68, per group */
    struct Vector3 **groupPointCopies; /* 0x2c: func_ov007_020c6e68 copies groupPoints */
    char pad_30[8];
    Ov007StrokePath **groups;    /* 0x38: committed strokes (func_ov007_020c6d20) */
    Ov007PointList **pointLists; /* 0x3c: func_ov007_020c798c, per group */
} Ov007Picker;

typedef char Ov007Link_size_must_be_0x18[sizeof(Ov007Link) == 0x18 ? 1 : -1];
typedef char Ov007Picker_size_must_be_0x40[sizeof(Ov007Picker) == 0x40 ? 1 : -1];

/* Re-measures group idx's links at a pose blended between groupPoints
 * (weight t, Q12) and groupPointCopies: x and y blended, z from the copies.
 * Each link gets the offset between consecutive blended points as its dir,
 * and that offset's length. sb keeps the char * spelling of the callers'
 * declarations; `self` is the typed view. */
void func_ov007_020c5fc8(char *sb, int idx, int t)
{
    Ov007Picker *self = (Ov007Picker *)sb;
    int i;
    int inv;
    struct Vector3 cur;
    struct Vector3 prev;

    i = 0;
    if ((int)self->groups[idx]->pointCount <= 0)
        return;

    inv = 0x1000 - t;
    do {
        struct Vector3 *a = self->groupPoints[idx];
        struct Vector3 *b = self->groupPointCopies[idx];
        int va = a[i].x;
        int vb = b[i].x;
        int r = va;
        if (t < 0x1000) {
            if (t <= 0) r = vb;
            else r = (int)(((s64)inv * vb + (s64)t * va) >> 12);
        }
        cur.x = r;

        a = self->groupPoints[idx];
        b = self->groupPointCopies[idx];
        va = a[i].y;
        vb = b[i].y;
        r = va;
        if (t < 0x1000) {
            if (t <= 0) r = vb;
            else r = (int)(((s64)inv * vb + (s64)t * va) >> 12);
        }
        cur.y = r;

        cur.z = self->groupPointCopies[idx][i].z;

        if (i > 0) {
            Ov007Link *link = self->pointLists[idx]->links[i - 1];
            SubVec3(&prev, &cur, &link->dir);
            link->length = LenVec3(&link->dir);
        }

        prev = cur;

        i++;
    } while (i < (int)self->groups[idx]->pointCount);
}
