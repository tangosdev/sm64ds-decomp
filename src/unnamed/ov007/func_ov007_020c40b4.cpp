//cpp
#include "Ov007SpriteGroup.h"
#include "Ov007MenuMem.h"

extern "C" {

extern void func_ov007_020c1180(Ov007OamNode *node);

/* Frees a func_ov007_020c40d4 record and its node. */
void func_ov007_020c40b4(Ov007SpriteItem *item) {
    func_ov007_020c1180(item->node);
    func_ov007_020c3d1c(item);
}
}
