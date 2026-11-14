//cpp
#include "Ov007OamNode.h"
#include "Ov007MenuMem.h"

/* Frees an OAM node and, when it has any, its animation table. */
void func_ov007_020c1180(Ov007OamNode *node) {
    if (node->animCount) {
        func_ov007_020c3d1c(node->anims);
    }
    func_ov007_020c3d1c(node);
}
