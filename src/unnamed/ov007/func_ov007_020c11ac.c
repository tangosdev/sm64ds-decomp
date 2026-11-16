#include "Ov007OamNode.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c10ec(Ov007OamNode *n);
extern void Sprite_SetAnimation(Ov007OamNode *node, int animIndex, int mode, int rev, int frame);

/* Allocates an OAM node with room for n animations, resets it
 * (func_ov007_020c10ec) and starts animation 0 looping. */
Ov007OamNode *func_ov007_020c11ac(int n) {
    Ov007OamNode *node = func_ov007_020c3df4(0, sizeof(Ov007OamNode));
    if (n == 0) {
        node->anims = 0;
    } else {
        node->anims = func_ov007_020c3df4(0, n * sizeof(Ov007CellAnim *));
    }
    func_ov007_020c10ec(node);
    node->animCount = n;
    Sprite_SetAnimation(node, 0, 1, 0, 0);
    return node;
}
