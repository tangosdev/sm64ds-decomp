#include "types.h"
#include "Ov007SpriteGroup.h"
#include "Ov007MenuMem.h"

extern Ov007OamNode *func_ov007_020c11ac(int n);
extern Ov007SpriteItem *func_ov007_020c40d4(int animCount);
extern void func_ov007_020c10b0(Ov007OamNode *parent, Ov007OamNode *child);
extern void func_ov007_020c4090(Ov007SpriteItem *item);

/* Shows the item's node only while it lies within `margin` pixels of the
 * 256x192 screen, and hides it once it leaves. */
void func_ov007_020c3fe4(Ov007SpriteItem *item) {
    s32 y;
    s32 m;
    s32 x;
    s32 inside;
    s32 insideX;
    s32 negM;
    Ov007OamNode *node;
    s32 visible;

    node = item->node;
    m = item->margin;
    x = node->x >> 12;
    inside = 0;
    insideX = 0;
    negM = -m;
    y = node->y >> 12;

    if (negM <= x && x <= m + 255)
        insideX = 1;
    if (insideX && negM <= y && y <= m + 191)
        inside = 1;

    visible = node->visible;
    if ((visible == 0 && inside) || (visible != 0 && !inside))
        node->visible = (visible == 0);
}

/* Builds a sprite group for `layout` (whose count and animation count it
 * reads), an invisible root at priority 4 with an extra record of
 * `extraAnims` animations and one record per item under it. */
Ov007SpriteGroup *func_ov007_020c3f00(int *layout, int extraAnims) {
    Ov007SpriteGroup *grp = func_ov007_020c3df4(0, sizeof(Ov007SpriteGroup));
    grp->layout = layout;
    grp->root = func_ov007_020c11ac(0);
    grp->root->priority = 4;
    grp->root->visible = 0;
    grp->extra = func_ov007_020c40d4(extraAnims);
    func_ov007_020c10b0(grp->root, grp->extra->node);
    grp->items = func_ov007_020c3df4(0, *layout << 2);
    int i = 0;
    while (i < *layout) {
        grp->items[i] = func_ov007_020c40d4(layout[2]);
        Ov007SpriteItem *item = grp->items[i];
        func_ov007_020c4090(item);
        func_ov007_020c10b0(grp->root, item->node);
        i++;
    }
    grp->word14 = 0;
    grp->word10 = -1;
    return grp;
}
