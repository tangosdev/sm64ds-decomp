#include "Ov007Picker.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c684c(void *self);
extern Ov007Follower *func_ov007_020c844c(Vector3 *p, int q);
extern Ov007Link *func_ov007_020c80a4(void);
#define func_ov007_020c8010 _Z19func_ov007_020c8010P9Ov007LinkP13Ov007FollowerS2_
extern void func_ov007_020c8010(Ov007Link *link, Ov007Follower *from, Ov007Follower *to);
extern Ov007StrokePath *func_ov007_020c2c68(int a, int b, int c);

/* Allocates the 0xd0-byte picker and its per-group tables for maxGroups
 * strokes, resets it (func_ov007_020c684c), builds the pos and anchor
 * followers joined by `link`, and a stroke path of pathCapacity points;
 * func_ov007_020c6724 then clears the groups. */
Ov007Picker *func_ov007_020c690c(int maxGroups, int maxPoints, int pathCapacity,
                                 struct Ov007Camera *camera, void *params, void *springConstants) {
    Ov007Picker *p = func_ov007_020c3df4(0, 0xd0);
    int n4 = maxGroups << 2;
    int n2;
    int i;

    p->maxGroups = maxGroups;
    p->maxPoints = maxPoints;
    p->springConstants = springConstants;
    p->params = params;
    p->camera = camera;

    p->groupPoints = func_ov007_020c3df4(0, n4);
    p->groupPointCopies = func_ov007_020c3df4(0, n4);
    p->word30 = 0;
    p->groups = func_ov007_020c3df4(0, n4);
    p->pointLists = func_ov007_020c3df4(0, n4);

    n2 = maxGroups << 1;
    p->headLinkGroups = func_ov007_020c3df4(0, n2);
    p->tailLinkGroups = func_ov007_020c3df4(0, n2);
    p->headLinkPoints = func_ov007_020c3df4(0, n2);
    p->tailLinkPoints = func_ov007_020c3df4(0, n2);

    p->groupLinks = func_ov007_020c3df4(0, n4);
    p->headLinks = func_ov007_020c3df4(0, n4);
    p->tailLinks = func_ov007_020c3df4(0, n4);

    func_ov007_020c684c(p);

    for (i = 0; i < maxGroups; i++) {
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

    p->path = func_ov007_020c2c68(0, pathCapacity, 1);
    p->pathPoints = func_ov007_020c3df4(0, pathCapacity * 0xc);

    func_ov007_020c6724(p, 1);
    return p;
}
