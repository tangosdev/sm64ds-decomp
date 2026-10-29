/* The 0x44-byte OAM sprite node func_ov007_020c11ac allocates and resets
 * here. Roles are from its drawer func_ov007_020c0b78, the animation step
 * func_ov007_020c0d70 (whose mode/rev/frame/tick/changed names are kept),
 * Sprite_SetAnimation and the child list func_ov007_020c10b0 builds
 * (owner/head/next as there). The word at +0x40 has no reader found yet and
 * stays raw on `c`. */
typedef struct Ov007OamNode {
    int x;       /* 0x00: Q12 screen x; func_ov007_020c0b78 adds it to the parent's */
    int y;       /* 0x04: Q12 screen y */
    int visible; /* 0x08: func_ov007_020c0b78 draws this node and its children only when set */
    int flip;    /* 0x0c: the OAM flip mode func_ov007_020c0eb0 applies */
    int mosaic;  /* 0x10: func_ov007_020c0eb0 sets the OAM mosaic bit when set */
    unsigned char priority;  /* 0x14: OAM priority, used when the caller passes none (> 3) */
    signed char affineIndex; /* 0x15: OAM affine matrix, -1 = none */
    char pad_16[2];
    void **anims;            /* 0x18: animCount animations (func_ov007_020c11ac) */
    unsigned char animCount; /* 0x1c: func_ov007_020c1180 frees anims only when nonzero */
    unsigned char animIndex; /* 0x1d: Sprite_SetAnimation's first argument */
    char pad_1e[2];
    int mode;    /* 0x20: 0 = stopped, 1 = looping, else once (func_ov007_020c0d70) */
    int rev;     /* 0x24: 1 = step backwards */
    int paused;  /* 0x28: func_ov007_020c0d70 does not step while set; no writer found */
    short frame; /* 0x2c */
    short tick;  /* 0x2e: ticks into the frame */
    int changed; /* 0x30: set when the frame wraps or clamps at an end */
    struct Ov007OamNode *owner; /* 0x34: func_ov007_020c10b0 */
    struct Ov007OamNode *head;  /* 0x38: first child */
    struct Ov007OamNode *next;  /* 0x3c: next sibling */
} Ov007OamNode;

typedef char Ov007OamNode_size_must_be_0x40[sizeof(Ov007OamNode) == 0x40 ? 1 : -1];

void func_ov007_020c10ec(char* c) {
    Ov007OamNode *n = (Ov007OamNode *)c;
    int i;
    n->y = 0;
    n->x = n->y;
    n->visible = 1;
    n->flip = 0;
    n->priority = 0;
    n->mosaic = 0;
    n->affineIndex = -1;
    n->animCount = 0;
    n->animIndex = 0;
    n->mode = 0;
    n->rev = 0;
    n->paused = 0;
    n->frame = 0;
    n->tick = 0;
    n->changed = 0;
    *(int*)(c + 0x40) = 0;
    n->next = (Ov007OamNode *)*(int *)(c + 0x40);
    n->head = n->next;
    n->owner = n->head;
    for (i = 0; i < n->animCount; i++) {
        n->anims[i] = 0;
    }
}
