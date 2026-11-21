#ifndef OV007_OAM_NODE_H
#define OV007_OAM_NODE_H

#include "types.h"
#include "Ov007SpriteBank.h"

/* Reconstructed layouts and names, not recovered class identity: the ov007
 * menu's OBJ sprite tree. func_ov007_020c11ac allocates a node and
 * func_ov007_020c10ec resets it; func_ov007_020c0b78 draws a node and its
 * subtree into OAM. Roles are from that drawer, the animation step
 * func_ov007_020c0d70 (whose mode/rev/frame/tick/changed names are kept),
 * Sprite_SetAnimation and the child list func_ov007_020c10b0 builds
 * (owner/head/next as there); prev is the OAM node list's back link
 * (func_ov007_020c0af4). */
typedef struct Ov007OamNode {
    int x;          /* 0x00: Q12 screen x; func_ov007_020c0b78 adds it to the parent's */
    int y;          /* 0x04: Q12 screen y */
    int visible;    /* 0x08: func_ov007_020c0b78 draws this node and its children only when set */
    int flip;       /* 0x0c: the OAM flip mode func_ov007_020c0eb0 applies */
    int mosaic;     /* 0x10: func_ov007_020c0eb0 sets the OAM mosaic bit when set */
    u8 priority;    /* 0x14: OAM priority, used when the caller passes none (> 3) */
    s8 affineIndex; /* 0x15: OAM affine matrix, -1 = none */
    char pad_16[2];
    /* 0x18: animCount animations (func_ov007_020c11ac); func_ov007_020c0b78
     * draws the current frame of anims[animIndex] */
    Ov007CellAnim **anims;
    u8 animCount; /* 0x1c: func_ov007_020c1180 frees anims only when nonzero */
    u8 animIndex; /* 0x1d: Sprite_SetAnimation's second argument */
    char pad_1e[2];
    int mode; /* 0x20: 0 = stopped, 1 = looping, else once (func_ov007_020c0d70) */
    int rev;  /* 0x24: 1 = step backwards */
    /* 0x28: func_ov007_020c0d70 does not step while it is set. Nothing sets
     * it nonzero: func_ov007_020c10ec and func_ov007_020c0d70 only write 0 */
    int paused;
    short frame;                /* 0x2c */
    short tick;                 /* 0x2e: ticks into the frame */
    int changed;                /* 0x30: set when the frame wraps or clamps at an end */
    struct Ov007OamNode *owner; /* 0x34: func_ov007_020c10b0 */
    struct Ov007OamNode *head;  /* 0x38: first child */
    struct Ov007OamNode *next;  /* 0x3c: next sibling */
    struct Ov007OamNode *prev;  /* 0x40: previous node in its OAM list */
} Ov007OamNode;

/* The 0xc-byte OAM node list func_ov007_020c1068 builds (the display
 * record's oamListMain/oamListSub); func_ov007_020c0af4 appends to it.
 * Nothing in ov007 touches its third word. */
typedef struct Ov007OamList {
    Ov007OamNode *head; /* 0x00 */
    Ov007OamNode *tail; /* 0x04 */
    char pad_08[4];
} Ov007OamList;

typedef char Ov007OamNode_size_must_be_0x44[sizeof(Ov007OamNode) == 0x44 ? 1 : -1];
typedef char Ov007OamList_size_must_be_0xc[sizeof(Ov007OamList) == 0xc ? 1 : -1];

#endif
