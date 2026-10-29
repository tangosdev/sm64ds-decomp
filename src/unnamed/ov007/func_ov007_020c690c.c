#include "types.h"
#include "Ov007StrokePath.h"

/* The 0xd0-byte touch picker this function builds. Names follow
 * func_ov007_020c5a6c's own struct and the readers cited per slot; only the
 * slots this file touches are spelled.
 *
 * Left raw, no reader found to name it: +0x30 (cleared). It stays on the
 * char * alias `o` of the allocation; the named slots go through `p`. */
typedef struct Ov007Picker {
    char pad_00[0xc];
    int maxGroups; /* 0x0c: a0; func_ov007_020c6550 stops committing at it */
    int maxPoints; /* 0x10: a1; func_ov007_020c65bc stops appending at it */
    char pad_14[4];
    /* 0x18: a5; three (stiffness, damping) pairs that func_ov007_020c5188
     * hands to the spring steps: [0..1] func_ov007_020c7560 per point list,
     * [2..3] func_ov007_020c7b2c per groupLinks entry, [4..5] the same for
     * headLinks/tailLinks. func_ov007_020c7b2c scales the stretch by the
     * first and the velocity by the second. */
    int *springConstants;
    /* 0x1c: a4; func_ov007_020c65bc reads its +8/+0xc/+0x10 point thresholds,
     * func_ov007_020c6e68 its +0x1c factor */
    void *params;
    Ov007StrokePath *path; /* 0x20: the stroke being drawn (func_ov007_020c2c68) */
    /* 0x24: a2 points; func_ov007_020c65bc unprojects each appended point
     * into it through `camera` */
    Vector3 *pathPoints;
    Vector3 **groupPoints;      /* 0x28: func_ov007_020c6e68, per group */
    Vector3 **groupPointCopies; /* 0x2c: func_ov007_020c6e68 copies groupPoints */
    char pad_30[4];
    /* 0x34: per group, one link per point from posPoint (func_ov007_020c6e68) */
    void ***groupLinks;
    Ov007StrokePath **groups; /* 0x38: committed strokes (func_ov007_020c6d20) */
    void **pointLists;        /* 0x3c: func_ov007_020c798c, per group */
    /* 0x40: per group, the link func_ov007_020c6e68 appends after the
     * per-point ones; func_ov007_020c6174 measures it from the group's first
     * point to groupPointCopies[headLinkGroups[i]][headLinkPoints[i]] */
    void **headLinks;
    /* 0x44: likewise from the group's last point (pointCount - 1) to
     * groupPointCopies[tailLinkGroups[i]][tailLinkPoints[i]] */
    void **tailLinks;
    u16 *headLinkGroups; /* 0x48: per group, 0xffff = unset (this file) */
    u16 *tailLinkGroups; /* 0x4c */
    u16 *headLinkPoints; /* 0x50 */
    u16 *tailLinkPoints; /* 0x54 */
    char pad_58[0x14];
    void *posPoint; /* 0x6c: func_ov007_020c844c's Point over pos */
    Vector3 pos;    /* 0x70 */
    char pad_7c[0x18];
    /* 0x94: func_ov007_020c844c's Point over anchorPos, as posPoint is over
     * pos; `link`'s from end, freed by func_ov007_020c6684 */
    void *anchorPoint;
    Vector3 anchorPos; /* 0x98: func_ov007_020c5a6c mode 2 moves it to the hit point */
    void *link;        /* 0xa4: func_ov007_020c80a4; from anchorPoint to posPoint */
    void *camera; /* 0xa8: a3; func_ov007_020c65bc unprojects through it */
} Ov007Picker;

typedef char Ov007Picker_camera_must_be_at_0xa8[(int)&((Ov007Picker *)0)->camera == 0xa8 ? 1 : -1];

extern void* func_ov007_020c3df4(int a, int size);
extern void func_ov007_020c684c(void* self);
extern void* func_ov007_020c844c(void* p, int q);
extern void* func_ov007_020c80a4(void);
extern void func_ov007_020c8010(void* self, void* a, void* b);
extern int func_ov007_020c2c68(int a, int b, int c);
extern void func_ov007_020c6724(void* self, int flag);

void* func_ov007_020c690c(int a0, int a1, int a2, int a3, int a4, int a5)
{
    char* o = (char*)func_ov007_020c3df4(0, 0xd0);
    Ov007Picker *p = (Ov007Picker *)o;
    int n4 = a0 << 2;
    int n2;
    int i;

    p->maxGroups = a0;
    p->maxPoints = a1;
    p->springConstants = (int *)a5;
    p->params = (void *)a4;
    p->camera = (void *)a3;

    p->groupPoints = (Vector3 **)func_ov007_020c3df4(0, n4);
    p->groupPointCopies = (Vector3 **)func_ov007_020c3df4(0, n4);
    *(s32*)(o + 0x30) = 0;
    p->groups = (Ov007StrokePath **)func_ov007_020c3df4(0, n4);
    p->pointLists = (void **)func_ov007_020c3df4(0, n4);

    n2 = a0 << 1;
    p->headLinkGroups = (u16 *)func_ov007_020c3df4(0, n2);
    p->tailLinkGroups = (u16 *)func_ov007_020c3df4(0, n2);
    p->headLinkPoints = (u16 *)func_ov007_020c3df4(0, n2);
    p->tailLinkPoints = (u16 *)func_ov007_020c3df4(0, n2);

    p->groupLinks = (void ***)func_ov007_020c3df4(0, n4);
    p->headLinks = (void **)func_ov007_020c3df4(0, n4);
    p->tailLinks = (void **)func_ov007_020c3df4(0, n4);

    func_ov007_020c684c(p);

    for (i = 0; i < a0; i++) {
        p->groups[i] = 0;
        p->tailLinkGroups[i] = 0xffff;
        p->headLinkGroups[i] = p->tailLinkGroups[i];
        p->tailLinkPoints[i] = 0xffff;
        p->headLinkPoints[i] = p->tailLinkPoints[i];
    }

    p->posPoint = func_ov007_020c844c(&p->pos, 0xa000);
    p->anchorPoint = func_ov007_020c844c(&p->anchorPos, 0x1000);
    p->link = func_ov007_020c80a4();
    func_ov007_020c8010(p->link, p->anchorPoint, p->posPoint);

    p->path = (Ov007StrokePath *)func_ov007_020c2c68(0, a2, 1);
    p->pathPoints = (Vector3 *)func_ov007_020c3df4(0, a2 * 0xc);

    func_ov007_020c6724(p, 1);
    return p;
}
