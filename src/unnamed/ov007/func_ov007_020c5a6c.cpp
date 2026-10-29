//cpp
#include "types.h"
/* Array form on purpose: a three-int-member struct copies field by field,
   while the ROM copies these with one ldm/stm. */
struct Vec3 {
    int v[3];
};

extern "C" {
int func_ov007_020c30ac(void *arr, int count, void *target, int arg4, int *out);
void func_ov007_020c64c4(char *c, int b);
void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
int LenVec3(struct Vec3 *v);
}

struct Point {
    Vec3 *pos; /* 0x0 */
};

struct PointList {
    Point **items; /* 0x0 */
};

struct Link {
    Point *from; /* 0x0 */
    Point *to;   /* 0x4 */
    int length;  /* 0x8 */
    Vec3 dir;    /* 0xc */
};

/* A committed stroke; names as in include/Ov007StrokePath.h. */
struct Ov007StrokePath {
    char pad0[8];
    u16 pointCount; /* 0x8 */
};

/* The 0xd0-byte touch picker func_ov007_020c690c builds. */
struct Ov007Picker {
    char pad0[4];
    int mode;       /* 0x4 */
    int groupCount; /* 0x8 */
    char pad1[0x28 - 0xc];
    Vec3 **groupPoints; /* 0x28 */
    char pad2[0x38 - 0x2c];
    Ov007StrokePath **groups; /* 0x38 */
    PointList **pointLists; /* 0x3c */
    char pad3[0x58 - 0x40];
    int hitGroup;   /* 0x58: picked group, -1 = none */
    int hitIndex;   /* 0x5c: picked point in that group, -1 = none */
    Vec3 hitOffset; /* 0x60 */
    char pad4[0x70 - 0x6c];
    Vec3 pos; /* 0x70 */
    char pad5[0x98 - 0x7c];
    /* 0x98: the position of the Point at +0x94 that anchors link's from end
       (func_ov007_020c690c); mode 2 moves it to the hit point */
    Vec3 anchorPos;
    Link *link; /* 0xa4 */
    char pad6[0xac - 0xa8];
    int limitGroup; /* 0xac */
    int limitIndex; /* 0xb0 */
};

extern "C" void func_ov007_020c5a6c(Ov007Picker *self, void *target, int flag) {
    self->hitGroup = self->hitIndex = -1;

    int i;
    int carry = -1;
    int out;
    for (i = 0; i < self->groupCount; i++) {
        int best = func_ov007_020c30ac(self->groupPoints[i], self->groups[i]->pointCount, target,
                                       carry, &out);
        /* The ROM tests flag twice (cmp r7,#0 / beq twice), so the
           redundant `flag != 0` stays. */
        if (best != -1 &&
            (flag == 0 || (flag != 0 && (i < self->limitGroup ||
                                         (i == self->limitGroup && best < self->limitIndex))))) {
            self->hitGroup = i;
            self->hitIndex = best;
            carry = out;
        }
    }

    if (self->hitGroup == -1 || self->hitIndex == -1)
        return;

    Point *hit = self->pointLists[self->hitGroup]->items[self->hitIndex];

    switch (self->mode) {
    case 0:
        self->pos = *hit->pos;
        func_ov007_020c64c4((char *)self, 1);
        break;
    case 1:
        SubVec3(&self->pos, hit->pos, &self->hitOffset);
        break;
    case 2: {
        self->pos.v[2] = self->anchorPos.v[2];
        self->anchorPos = *hit->pos;
        Link *link = self->link;
        SubVec3(link->from->pos, link->to->pos, &link->dir);
        link->length = LenVec3(&link->dir);
        break;
    }
    }
}
