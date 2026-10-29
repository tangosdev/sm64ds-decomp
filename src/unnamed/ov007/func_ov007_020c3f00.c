#include "types.h"

struct Inner {
    s32 x;    /* 0x00 */
    s32 y;    /* 0x04 */
    s32 flag; /* 0x08 */
};

struct Outer {
    struct Inner *inner; /* 0x00 */
    u8 unk04[6];         /* 0x04 */
    u8 margin;           /* 0x0a */
};

/* The 0x44-byte OAM sprite node func_ov007_020c11ac allocates; names as in
 * func_ov007_020c10ec, only the slots this file writes. */
typedef struct Ov007OamNode {
    char pad_00[8];
    int visible; /* 0x08: func_ov007_020c0b78 draws this node and its children only when set */
    char pad_0c[8];
    unsigned char priority; /* 0x14: OAM priority, used when the caller passes none (> 3) */
} Ov007OamNode;

/* The 0x18-byte sprite group this function builds and func_ov007_020c3e7c
 * frees: a root node with one child per item. Only the slots with a reader
 * are spelled; +0x0c (the extra node func_ov007_020c40d4 builds from r4),
 * +0x10 (-1) and +0x14 (0) stay raw on r5, no reader names them. */
typedef struct Ov007SpriteGroup {
    int *layout;        /* 0x00: r6; [0] is the item count, [2] each item's animation count */
    Ov007OamNode *root; /* 0x04: the items' parent (func_ov007_020c10b0) */
    /* 0x08: layout[0] func_ov007_020c40d4 records; each one's first word is
     * its node */
    void **items;
} Ov007SpriteGroup;

typedef char Ov007OamNode_size_must_be_0x18[sizeof(Ov007OamNode) == 0x18 ? 1 : -1];

typedef char Ov007SpriteGroup_size_must_be_0xc[sizeof(Ov007SpriteGroup) == 0xc ? 1 : -1];

extern int func_ov007_020c3df4(int a, void *b);
extern void *func_ov007_020c11ac(int n);
extern void *func_ov007_020c40d4(void *r5);
extern void func_ov007_020c10b0(void *a, int b);
extern void func_ov007_020c4090(char *c);

void func_ov007_020c3fe4(struct Outer *self) {
    s32 y;
    s32 m;
    s32 x;
    s32 vis;
    s32 onX;
    s32 negM;
    struct Inner *p;
    s32 f;

    p = self->inner;
    m = self->margin;
    x = p->x >> 12;
    vis = 0;
    onX = 0;
    negM = -m;
    y = p->y >> 12;

    if (negM <= x && x <= m + 255)
        onX = 1;
    if (onX && negM <= y && y <= m + 191)
        vis = 1;

    f = p->flag;
    if ((f == 0 && vis) || (f != 0 && !vis))
        p->flag = (f == 0);
}

void *func_ov007_020c3f00(int *r6, void *r4) {
    char *r5 = (char *)func_ov007_020c3df4(0, (void *)0x18);
    Ov007SpriteGroup *grp = (Ov007SpriteGroup *)r5;
    grp->layout = r6;
    grp->root = func_ov007_020c11ac(0);
    grp->root->priority = 4;
    grp->root->visible = 0;
    *(void **)(r5 + 0xc) = func_ov007_020c40d4(r4);
    func_ov007_020c10b0(grp->root, *(int *)*(void **)(r5 + 0xc));
    grp->items = (void **)func_ov007_020c3df4(0, (void *)(*r6 << 2));
    int i = 0;
    while (i < *r6) {
        grp->items[i] = func_ov007_020c40d4((void *)r6[2]);
        char *r7 = (char *)grp->items[i];
        func_ov007_020c4090(r7);
        func_ov007_020c10b0(grp->root, *(int *)r7);
        i++;
    }
    *(int *)(r5 + 0x14) = 0;
    *(int *)(r5 + 0x10) = -1;
    return r5;
}
