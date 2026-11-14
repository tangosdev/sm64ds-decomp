//cpp
/* fBase_c::SceneNode -- the intrusive scene-graph node every actor carries
 * (arm9 0x0203b358..0x0203b4dc).
 *
 * Six functions, written in descending address order because mwccarm emits
 * .text in reverse source order: the two members, the tree's add and remove,
 * and its two pre-order walk steps. Every one works the node's
 * parent/child/prev/next links. Below the run, func_0203b2ec and its
 * neighbours are two-word list primitives, a different node shape. Reset
 * clears the four links and deliberately leaves the owner back-pointer at
 * +0x10 alone; fBase_c's own constructor writes that one (see the Manager
 * ctor in include/fBase_c.h).
 *
 * Names: fBase_c::SceneNode is a coined name. The ROM has RTTI for fBase_c
 * but none for the node, and no ROM string names it. The name came in with
 * the project's first import as ActorBase::SceneNode; #1576 renamed the
 * outer class ActorBase to fBase_c, the cartridge's name for it, and kept
 * the node's coined name. The lead for its real
 * name is cLib's cTreeNd_c: if this unit is c_tree.cpp, the add, remove
 * and walk steps below are cTreeMg_c's and cTreeNd_c's.
 *
 * deslop leftovers: the two members are real and fully typed, but the
 * tree's add/remove (func_0203b438, func_0203b3c0) still work through
 * shadow structs and all four helpers keep their address names.
 */
#include "fBase_c.h"

/* Defined last below (reverse emission); func_0203b394 calls it. */
fBase_c::SceneNode *func_0203b358(fBase_c::SceneNode *node);

#ifndef SM64DS_PLATFORM_PC
/* Five words: parent, child, prev, next, owner. fBase_c.h asserts the whole
   0x50 actor root; this pins the nested node itself. */
typedef char SceneNode_size_must_be_0x14[sizeof(fBase_c::SceneNode) == 0x14 ? 1 : -1];
#endif

fBase_c::SceneNode::SceneNode()
{
    Reset();
}

void fBase_c::SceneNode::Reset()
{
    parent = 0;
    child = 0;
    prev = 0;
    next = 0;
}

/* ROM ordinals 2..3 -- the members' backward neighbours, absorbed in descending
 * address order (reverse emission). Each keeps its own namespace so the
 * file-local struct tags do not collide; the functions themselves keep C
 * linkage under their existing names. Bodies as recovered. */

namespace Absorb0203b438 {
struct N {
    struct N *f0;   /* 0x0 */
    struct N *f4;   /* 0x4 */
    struct N *f8;   /* 0x8 */
    struct N *fc;   /* 0xc */
};

int func_0203b438(struct N *a, struct N *b, struct N *c) {
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

namespace Absorb0203b3c0 {
struct Node {
    struct Node *f0;
    struct Node *f4;
    struct Node *f8;
    struct Node *fc;
};

int func_0203b3c0(int *list, struct Node *n) {
    if (n != 0) {
        if (n->f4 != 0)
            return 0;
        if (n->f8 != 0) {
            n->f8->fc = n->fc;
        } else {
            if (n->f0 != 0) {
                n->f0->f4 = n->fc;
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

/* The two pre-order walk steps over the same links, at the bottom of the
 * run. fBase_c::checkChildProcessCreateState (ActorBase.cpp) and ProcessTree::Thing::func_020441cc
 * (fListNode.cpp) walk the scene tree with them. NSMBW's c_tree.cpp has
 * cTreeNd_c::getTreeNext and getTreeNextNotChild with the same two bodies,
 * last in a file whose six functions (ctor, forcedClear, addTreeNode,
 * removeTreeNode, getTreeNext, getTreeNextNotChild) are this run in reverse
 * emission order; nothing in this ROM names them, so they keep their
 * address names. */

/* The pre-order successor of `node`: its first child, else the first node
 * after its subtree (func_0203b358). */
fBase_c::SceneNode *func_0203b394(fBase_c::SceneNode *node) {
    fBase_c::SceneNode *p = node->child;
    if (p == 0)
        p = func_0203b358(node);
    return p;
}

/* The first node after `node`'s subtree: its next sibling, else the next
 * sibling of the nearest ancestor that has one; 0 past the root. */
fBase_c::SceneNode *func_0203b358(fBase_c::SceneNode *node) {
    fBase_c::SceneNode *n;
    if (node->next)
        return node->next;
    n = node->parent;
    while (n) {
        if (n->next)
            return n->next;
        n = n->parent;
    }
    return 0;
}
