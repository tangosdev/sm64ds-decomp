//cpp
#include "Ov007SpriteGroup.h"
#include "Ov007MenuMem.h"



extern "C" Ov007OamNode *func_ov007_020c11ac(int n);
extern "C" void func_ov007_020c4090(Ov007SpriteItem *item);

/* Allocates a sprite item around a fresh OAM node with room for animCount
 * animations, then resets it (func_ov007_020c4090). */
Ov007SpriteItem *func_ov007_020c40d4(int animCount) {
    Ov007SpriteItem *item = (Ov007SpriteItem *)func_ov007_020c3df4(0, sizeof(Ov007SpriteItem));
    item->node = func_ov007_020c11ac(animCount);
    item->half6 = 0;
    item->half4 = item->half6;
    item->margin = 0;
    func_ov007_020c4090(item);
    return item;
}

