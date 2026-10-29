/* The OAM sprite node func_ov007_020c11ac allocates; names copied from
 * Ov007OamNode in src/unnamed/ov007/func_ov007_020c10ec.c, only the slots
 * this file writes. */
typedef struct Ov007OamNode {
    char pad_00[0x1d];
    unsigned char animIndex; /* 0x1d */
    char pad_1e[2];
    int mode;    /* 0x20: 0 = stopped, 1 = looping, else once */
    int rev;     /* 0x24: 1 = step backwards */
    int paused;  /* 0x28 */
    short frame; /* 0x2c */
    short tick;  /* 0x2e: ticks into the frame */
} Ov007OamNode;
typedef char Ov007OamNode_tick_must_be_at_0x2e[(int)&((Ov007OamNode *)0)->tick == 0x2e ? 1 : -1];

void Sprite_SetAnimation(char *c, int r1, int r2, int r3, int sp0) {
    Ov007OamNode *n = (Ov007OamNode *)c;
    n->animIndex = (char)r1;
    if (sp0 >= 0) {
        n->frame = (short)sp0;
        n->tick = 0;
    }
    n->mode = r2;
    n->rev = r3;
}
