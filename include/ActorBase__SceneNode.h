/* fBase_c::SceneNode, as much of it as src/_ZN7fBase_c9SceneNode5ResetEv.c
 * needs: the four words Reset() clears.
 *
 * include/fBase_c.h has the fuller spelling, `ActorBase_SceneNode` (one
 * underscore), which adds the owner back-pointer at 0x10 that fBase_c's
 * constructor writes and Reset() deliberately leaves alone -- that fifth field
 * is what makes the real object 0x14 bytes rather than the 0x10 below. Prefer
 * that one in new code; see notes/actor-core-provenance.md.
 *
 * Field names are placeholders: nothing in the tree does anything with these
 * four words except zero them.
 *
 * Originally emitted by tools/gen_header.py from matched-function evidence.
 */
#ifndef ACTORBASE__SCENENODE_H
#define ACTORBASE__SCENENODE_H
#include "types.h"

struct ActorBase__SceneNode {
    s32 unk_000;            /* 0x000 */
    s32 unk_004;            /* 0x004 */
    s32 unk_008;            /* 0x008 */
    s32 unk_00c;            /* 0x00c */
};

#endif
