extern void func_ov007_020bc02c(void *p, int c);
extern void func_ov007_020beb80(void *a);

/* The 0x1c-byte animation player func_ov007_020c92ac allocates; names as in
 * func_ov007_020bc02c, only the slot this file reads. */
typedef struct Ov007AnimPlayer {
    char pad_00[0xc];
    short anim; /* 0x0c: index into the owner's animation table; -1 none */
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc02c and func_ov007_020bfaf0, only the slots this file
 * reads. */
typedef struct Ov007Model {
    char pad_00[0x18];
    Ov007AnimPlayer *boneAnim; /* 0x18 */
    char pad_1c[0x78];
    int visible; /* 0x94: func_ov007_020bbff0 renders only while it is nonzero */
} Ov007Model;

/* The 0x1c-byte record func_ov007_020beaa0 builds around an Ov007Model;
 * names as in func_ov007_020b0da0 and func_ov007_020b2370, only the slots
 * this file reads. */
typedef struct Ov007Figure {
    /* 0x00: func_ov007_020beaa0's first argument; 0 for Ov007Work::figure,
     * 1 for altFigure (func_ov007_020b2998) */
    unsigned char index;
    /* 0x01: func_ov007_020beaa0's second argument, Ov007Work +0xec; no
     * reader names it */
    unsigned char b1;
    char pad_02[2];
    struct Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
    char pad_08[0x10];
    /* 0x18: the per-frame hook; func_ov007_020b0a20 installs one there */
    void (*update)(struct Ov007Figure *);
} Ov007Figure;

typedef char Ov007Model_visible_must_be_at_0x94[(int)&((Ov007Model *)0)->visible == 0x94 ? 1 : -1];
typedef char Ov007Figure_size_must_be_0x1c[sizeof(Ov007Figure) == 0x1c ? 1 : -1];

/* Poses a figure for the frame: hides its model while the manager model
 * (b) plays animation 1 or 2, and for the second figure while b1 is 0;
 * then poses the model under view matrix c, runs the figure's hook and
 * func_ov007_020beb80. b keeps the char * spelling of the definition;
 * `owner` is the typed view. */
void func_ov007_020be9ac(Ov007Figure *a, char *b, int c) {
    Ov007Model *owner = (Ov007Model *)b;
    Ov007AnimPlayer *p = owner->boneAnim;
    switch (p->anim) {
    case 0:
        break;
    case 1:
    case 2:
    {
        Ov007Model *pp = a->model;
        if (pp->visible != 0)
            pp->visible = 0;
        break;
    }
    }

    {
        Ov007Model *q = a->model;
        if (q->visible != 0 && a->index == 1 && a->b1 == 0) {
            q->visible = 0;
        }
    }

    func_ov007_020bc02c(a->model, c);
    if (a->update != 0)
        a->update(a);
    func_ov007_020beb80(a);
}
