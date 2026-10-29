#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
extern u8 data_ov007_0210342c[];
extern void Sprite_SetAnimation(char *c, int r1, int r2, int r3, int sp0);

/* The 10-byte layout row an item points at (names as in
 * func_ov007_020b5bf0); only its kind is read here. */
typedef struct Ov007ItemLayout {
    u16 kind; /* 0x0 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020b5bf0, only the slots this file reads. */
typedef struct Ov007Item {
    u16 id; /* 0x00: aea6c's first argument */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
    char pad_08[0x1c];
    Ov007ItemLayout *layout; /* 0x24 */
    char pad_28[4];
} Ov007Item;

/* The 0x40-byte OAM node func_ov007_020c11ac allocates; names as in
 * func_ov007_020c10ec, only the slot this file reads. */
typedef struct Ov007OamNode {
    char pad_00[0x1d];
    unsigned char animIndex; /* 0x1d: Sprite_SetAnimation's first argument */
} Ov007OamNode;

/* The 8-byte record func_ov007_020ade58 returns: an item and the OAM node
 * drawn for it. */
typedef struct Ov007ItemNode {
    Ov007Item *item;    /* 0x00: func_ov007_020aea6c */
    Ov007OamNode *node; /* 0x04: func_ov007_020c11ac */
} Ov007ItemNode;

typedef char Ov007Item_size_must_be_0x2c[sizeof(Ov007Item) == 0x2c ? 1 : -1];

/* Picks the next state for one item node from the menu state and the item's
 * own state. The parameter keeps the char * spelling of its caller
 * (func_ov007_020add3c); `self` is the typed view. */
int func_ov007_020ae558(char *node) {
    Ov007ItemNode *self = (Ov007ItemNode *)node;
    Ov007Work *g = *(Ov007Work **)data_ov007_0210342c;
    Ov007MenuState *b = g->states[2];
    int flag = b->stateTicks;
    Ov007Item *a = self->item;
    int sel = b->currentState;
    Ov007ItemLayout *r3ptr = a->layout;
    int r6 = (flag == 0);
    Ov007MenuState *c = a->state;
    int lr_v = r3ptr->kind;
    int ip_v = a->id;
    int r3_v = c->currentState;
    int ret = c->requestedState;

    switch (sel) {
    case 0:  if (r6) ret = 1; break;
    case 1:
        if (r6) {
            if (ip_v == 1)
                Sprite_SetAnimation((char *)self->node, 0, 0, 0, 0);
        }
        break;
    case 3:  if (r6) { if (lr_v == 1) ret = 7; else if (ip_v == 7) ret = 1; } break;
    case 2:  if (r6) { if (lr_v == 1) { if (ip_v != 2) ret = 4; } } break;
    case 4:  if (r6) { if (lr_v == 1) ret = 7; } break;
    case 5:  if (r6) { if (lr_v == 1) ret = 7; } break;
    case 7:  if (r6) { if (lr_v == 1) ret = 3; } break;
    case 6:  if (r6) { if (lr_v == 1) ret = 4; } break;
    case 8:
        if (r6) {
            short d = b->previousState;
            int t = (d == 1 || d == 3);
            if (lr_v == 1) {
                if (t) {
                    if (ip_v != 2) {
                        ret = 0;
                        if (ip_v == 1)
                            Sprite_SetAnimation((char *)self->node, 4, 0, 0, 0);
                    } else
                        ret = 1;
                } else ret = 7;
            } else if (lr_v == 5) {
                ret = 0;
                Sprite_SetAnimation((char *)self->node, 0, 1, 0, 0);
            } else ret = 1;
        }
        break;
    case 9: case 10: case 13: if (r6) { if (lr_v == 1) ret = 7; } break;
    case 11: if (lr_v == 1) { if (r6) ret = 4; } break;
    case 12:
        if (lr_v == 1) {
            if (r6) ret = 7;
            else {
                Ov007Work *g2 = *(Ov007Work *volatile *)data_ov007_0210342c;
                short v = g2->states[1]->currentState;
                u8 flag2 = g2->itemNodes[1]->node->animIndex;
                int x;
                if (v != 7) break;
                if (r3_v != 0) break;
                x = g->selectedSlot;
                if ((x != -1 && flag2 == 7) || (x == -1 && flag2 == 8))
                    ret = 7;
            }
        }
        break;
    case 14: if (r6) { if (lr_v == 1) ret = 7; else if (ip_v == 7) ret = 0; } break;
    }
    return ret;
}
