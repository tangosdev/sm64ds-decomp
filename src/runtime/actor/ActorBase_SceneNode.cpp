//cpp
/* fBase_c::SceneNode -- the intrusive scene-graph node every actor carries
 * (arm9 0x0203b3c0..0x0203b4dc).
 *
 * Four functions, written first-to-last because mwccarm emits .text in reverse
 * source order. Reset clears the four links and deliberately leaves the owner
 * back-pointer at +0x10 alone; fBase_c's own constructor writes that one
 * (see the Manager ctor in include/fBase_c.h).
 *
 * deslop: no leftovers. Both members are real, fully typed, and named by the
 * ROM's own symbols.
 */
#include "fBase_c.h"

#ifndef SM64DS_PLATFORM_PC
/* Five words: parent, child, prev, next, owner. fBase_c.h asserts the whole
   0x50 actor root; this pins the nested node itself. */
typedef char SceneNode_size_must_be_0x14[sizeof(fBase_c::SceneNode) == 0x14 ? 1 : -1];
#endif

// @symbol _ZN7fBase_c9SceneNodeC1Ev
fBase_c::SceneNode::SceneNode()
{
    Reset();
}

// @symbol _ZN7fBase_c9SceneNode5ResetEv
void fBase_c::SceneNode::Reset()
{
    parent = 0;
    child = 0;
    prev = 0;
    next = 0;
}

/* ROM ordinals 0..1 -- the run's backward neighbours, absorbed in descending
 * address order (reverse emission). Each keeps its own namespace so the
 * file-local struct tags do not collide; the functions themselves keep C
 * linkage under their existing names. Bodies as recovered. */

// @symbol func_0203b438
namespace Absorb0203b438 {
struct N {
    struct N *f0;   /* 0x0 */
    struct N *f4;   /* 0x4 */
    struct N *f8;   /* 0x8 */
    struct N *fc;   /* 0xc */
};

extern "C" int func_0203b438(struct N *a, struct N *b, struct N *c)
{
    struct N *t;
    struct N *n;
    if (b != 0) {
        if (c != 0) {
            b->f0 = c;
            t = c->f4;
            if (t == 0) {
                c->f4 = b;
            } else {
                n = t->fc;
                while (n != 0) {
                    t = n;
                    n = n->fc;
                }
                t->fc = b;
                b->f8 = t;
            }
        } else {
            if (a->f0 != 0)
                return 0;
            a->f0 = b;
        }
    } else {
        /* The else arm places this return ahead of the shared return 1,
           as the ROM lays it out. */
        return 0;
    }
    return 1;
}
}

// @symbol func_0203b3c0
namespace Absorb0203b3c0 {
struct Node { int f0; int f4; struct Node* f8; struct Node* fc; };

extern "C" int func_0203b3c0(int* list, struct Node* n)
{
    if (n != 0) {
        if (n->f4 != 0)
            return 0;
        if (n->f8 != 0) {
            n->f8->fc = n->fc;
        } else {
            if (n->f0 != 0) {
                *(int *)(n->f0 + 4) = (int)n->fc;
            } else {
                *list = 0;
            }
        }
        if (n->fc != 0) {
            n->fc->f8 = n->f8;
        }
        n->f8 = 0;
        n->fc = 0;
        n->f0 = 0;
    } else {
        /* Same layout as func_0203b438: return 0 sits before return 1. */
        return 0;
    }
    return 1;
}
}
