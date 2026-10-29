/* func_ov007_020c0b78 at 0x020c0b78
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
struct E8 { int a, b; };

/* One animation frame: cellCount OAM attribute triples (6 bytes each), the
 * src func_ov007_020c0eb0 reads. */
typedef struct Ov007SpriteFrame {
    char *cells;   /* 0x00 */
    int cellCount; /* 0x04 */
} Ov007SpriteFrame;

/* One of a node's animations; durations and count (func_ov007_020c0d70)
 * are not read here. */
typedef struct Ov007SpriteAnim {
    Ov007SpriteFrame **frames; /* 0x00: indexed by the node's frame */
    char pad_04[4];
    /* 0x08, medium confidence: this file adds the pair to the node's screen
     * x/y before drawing the frame's cells; no other reader found */
    unsigned short offsetX;
    unsigned short offsetY; /* 0x0a */
} Ov007SpriteAnim;

/* The 0x44-byte OAM sprite node func_ov007_020c11ac allocates; names as in
 * func_ov007_020c10ec, only the slots this file reads. */
typedef struct Ov007OamNode {
    int x;                   /* 0x00: Q12 screen x; added to the parent's */
    int y;                   /* 0x04: Q12 screen y */
    int visible;             /* 0x08: draws this node and its children only when set */
    int flip;                /* 0x0c: the OAM flip mode func_ov007_020c0eb0 applies */
    int mosaic;              /* 0x10: func_ov007_020c0eb0 sets the OAM mosaic bit when set */
    unsigned char priority;  /* 0x14: OAM priority, used when the caller passes none (> 3) */
    signed char affineIndex; /* 0x15: OAM affine matrix, -1 = none */
    char pad_16[2];
    void **anims;            /* 0x18: animCount animations (func_ov007_020c11ac) */
    unsigned char animCount; /* 0x1c: func_ov007_020c1180 frees anims only when nonzero */
    unsigned char animIndex; /* 0x1d: Sprite_SetAnimation's first argument */
    char pad_1e[0xe];
    short frame; /* 0x2c */
    char pad_2e[0xa];
    struct Ov007OamNode *head; /* 0x38: first child */
    struct Ov007OamNode *next; /* 0x3c: next sibling */
} Ov007OamNode;

typedef char Ov007OamNode_size_must_be_0x40[sizeof(Ov007OamNode) == 0x40 ? 1 : -1];
typedef char Ov007SpriteAnim_size_must_be_0xc[sizeof(Ov007SpriteAnim) == 0xc ? 1 : -1];

extern int func_ov007_020c0eb0(struct E8*, int, int, int, int, int, int, int);
extern void func_ov007_020c0d70(void*, void*);
extern int func_ov007_020c0b78(struct E8* p, void* s, int x, int y, int a4, unsigned char a5, int a6, int sb);

/* Draws node `s` and its subtree into OAM buffer p from slot sb on: the node
 * at (x, y) plus its own offset, then its next sibling at the parent's
 * position and its children at its own. Returns the next free slot. The
 * parameter keeps the tree's void * spelling; `sp` is the typed local. */
int func_ov007_020c0b78(struct E8* p, void* s, int x, int y, int a4, unsigned char a5, int a6, int sb){
    Ov007OamNode *sp = (Ov007OamNode *)s;
    int u, t, sum24, sum28;
    unsigned char b5 = a5;

    sum24 = sp->x + x;
    sum28 = sp->y + y;

    if (b5 > 3)
        b5 = sp->priority;

    if (sp->mosaic != 0 || a6 != 0)
        t = 1;
    else
        t = 0;

    if (sp->visible != 0 && a4 != 0)
        u = 1;
    else
        u = 0;

    if (u) {
        if (sp->animCount) {
            Ov007SpriteAnim *r8 = (Ov007SpriteAnim *)sp->anims[sp->animIndex];
            if (r8) {
                int i;
                Ov007SpriteFrame *r6 = r8->frames[sp->frame];
                for (i = 0; i < r6->cellCount; i++) {
                    func_ov007_020c0eb0(&p[sb], (int)(r6->cells + i * 6),
                                        (sum24 >> 12) + r8->offsetX, (sum28 >> 12) + r8->offsetY,
                                        b5, sp->flip, t, sp->affineIndex);
                    sb++;
                }
                func_ov007_020c0d70(s, r8);
            }
        }
    }

    if (sp->next)
        sb = func_ov007_020c0b78(p, sp->next, x, y, a4, a5, a6, sb);
    if (sp->head)
        sb = func_ov007_020c0b78(p, sp->head, sum24, sum28, u, b5, t, sb);

    return sb;
}
