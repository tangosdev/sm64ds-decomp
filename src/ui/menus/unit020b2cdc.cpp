//cpp
/* unit020b2cdc -- ov007 unit 0x020b2cdc..0x020b3edc: 12 functions, .text only.
 *
 * The title menu's cursor. ov007 is the title and file-select menu program
 * that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or vtable, so
 * no class name is claimed and the file keeps the unit<addr> name.
 *
 * What the code does. The cursor is one 0x1c-byte record,
 * data_ov007_02103360, that func_ov007_020b30b0 (called from the init
 * dispatcher, func_ov007_020b7138) allocates with its own state machine and
 * sprite, and func_ov007_020b3050 (called from func_ov007_020b6f4c, which
 * tears every menu module down) frees. func_ov007_020b2e64 (called from
 * func_ov007_020b7090) runs it: it reads the d-pad with key repeat into dirPressed and a fresh A or
 * START into confirm, ticks the cursor state, and runs func_ov007_020b3360,
 * which picks the next cursor state from the menu's own states[0] /
 * states[1] and the cursor's func_ov007_020b3708 (the per-state move
 * table), or func_ov007_020b3190 while the cursor is parked in state 0xff.
 * func_ov007_020b3c54 maps the cursor state to one of six sprite modes and
 * re-dresses the sprite through func_ov007_020b3d30;
 * func_ov007_020b2cf0 places the sprite over the current item with the
 * per-mode offset func_ov007_020b3e04 and eases it back from the last
 * move's nudge; func_ov007_020b3e84 reports the current item's position.
 *
 * Edges: the bss words 0x02103360 (the cursor) and 0x02103364 (a confirm
 * toggle) are referenced only by these functions, and every function here
 * touches the cursor or is called only from one that does
 * (func_ov007_020b3e04, func_ov007_020b3d30). The previous function,
 * func_ov007_020b2bd4 (bss 0x02103348 / 0x02103354), and the next one,
 * func_ov007_020b3edc (the file loader's release, bss 0x0210336c /
 * 0x02103370), touch none of it.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "cstd.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"

extern "C" {

/* data_ov007_02103360: the 0x1c-byte cursor func_ov007_020b30b0 allocates. */
typedef struct Ov007Cursor {
    Ov007MenuState *state; /* 0x00: func_ov007_020c9388(0xff); func_ov007_020b2e64 ticks it */
    Ov007Sprite *sprite;   /* 0x04: func_ov007_020c44e4(1) */
    /* 0x08: func_ov007_020b3c54's sprite mode, 0..5; 2 picks the second
     * move sound in func_ov007_020b3708 */
    int mode;
    /* 0x0c/0x0e: the last move's direction, -1/0/1 per axis;
     * func_ov007_020b2cf0 eases the sprite back from it */
    s16 nudgeX;
    s16 nudgeY;
    /* 0x10, medium confidence: func_ov007_020b2e64 sets it on a fresh A or
     * START press; states 9 and 0xc/0xd in func_ov007_020b3708 take it as a
     * step and clear it */
    int confirm;
    /* 0x14: func_ov007_020b2e64's key-repeat direction this frame: -1 none,
     * 0 up, 1 down, 2 right, 3 left */
    int dirPressed;
    /* 0x18: the state func_ov007_020b3360 returns to when the cursor leaves
     * states 0x16/0x17; -1 none */
    int returnState;
} Ov007Cursor;

typedef char Ov007Cursor_size_must_be_0x1c[sizeof(Ov007Cursor) == 0x1c ? 1 : -1];

/* The 0x70-byte key record func_ov007_020c2304 allocates (names as in
 * func_ov007_020b1cf0). func_ov007_020c20b8 keeps one counter per button,
 * in the order up, down, right, left, A, B, X, Y, L, R, START, SELECT. */
typedef struct Ov007Pad {
    u16 keys;     /* 0x00: held this frame */
    u16 prevKeys; /* 0x02: held last frame */
    /* 0x04: frames each button has been held, 1 on the press frame; 0 while
     * it is up (func_ov007_020c20b8) */
    u32 heldFrames[12];
    char pad_34[0x30];
    /* 0x64: bit 0 makes func_ov007_020c20b8 read no keys until every key is
     * released, then clears it */
    int inputLock;
    char pad_68[8];
} Ov007Pad;

typedef char Ov007Pad_size_must_be_0x70[sizeof(Ov007Pad) == 0x70 ? 1 : -1];

/* The 0x2c-byte item func_ov007_020aea6c allocates and the 0x1c-byte view
 * func_ov007_020b4c04 returns (names as in func_ov007_020beeb0). */
typedef struct Ov007Item {
    char pad_00[4];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
    int posX;              /* 0x08: Q12; func_ov007_020b3e84 reports it */
    int posY;              /* 0x0c */
} Ov007Item;

typedef struct Ov007ItemView {
    Ov007Item *item;     /* 0x00 */
    Ov007Sprite *sprite; /* 0x04: func_ov007_020b2cf0 places the cursor on it */
} Ov007ItemView;

extern Ov007Cursor *data_ov007_02103360;
extern int data_ov007_02103364;
extern char *data_ov007_0210342c;

extern unsigned char data_ov007_020d7dc0[];
extern unsigned char data_ov007_020d7dc8[];
extern unsigned char data_ov007_020d7dd0[];
extern unsigned char data_ov007_020d7dd1[];
extern unsigned char data_ov007_020d7ddc[];
extern unsigned char data_ov007_020d7ddd[];
extern short data_ov007_020d7de8[];
extern short data_ov007_020d7dea[];

extern int func_ov007_020b79e4(void);
extern void func_ov007_020bdeb0(int a);
extern int func_ov007_020c1da0(int i);
extern void func_ov007_020c2090(char *c);
extern void func_ov007_020c3d1c(void *self);
extern int func_ov007_020c3df4(int a, void *b);
extern void func_ov007_020c421c(char *r4);
extern void func_ov007_020c4388(char *r0, int r1);
extern void func_ov007_020c43bc(void *, int);
extern int func_ov007_020c44c4(char *a);
extern int func_ov007_020c44e4(int count);
extern void func_ov007_020c92d0(char *record);
extern void func_ov007_020c937c(void *state);
extern void *func_ov007_020c9388(int initialState);

/* The work-block extern keeps the tree's char * spelling; the typed view is
 * reached through a cast macro. */
#define CURSOR data_ov007_02103360
#define WORK ((Ov007Work *)data_ov007_0210342c)

// @symbol func_ov007_020b3e84
/* Writes the current item's position, in pixels, as a 1 x 0 rectangle. */
void func_ov007_020b3e84(short *r0) {
    Ov007Item *item = WORK->itemViews[CURSOR->state->currentState]->item;
    int x = item->posX;
    int y = item->posY;
    r0[0] = x >> 0xc;
    r0[1] = y >> 0xc;
    r0[2] = 1;
    r0[3] = 0;
}

// @symbol func_ov007_020b3e04
/* The sprite offset, in pixels, for each of func_ov007_020b3c54's modes. */
void func_ov007_020b3e04(int sel, s16 *p1, s16 *p2) {
    *p2 = 0;
    *p1 = *p2;
    switch (sel) {
    case 0:
        *p2 = *p2 - 0x16;
        break;
    case 1:
        *p2 = 0x10;
        break;
    case 2:
        *p1 = 0x20;
        break;
    case 3:
        *p2 = -0x16;
        break;
    case 4:
        *p2 = -0x10;
        break;
    case 5:
        *p1 = -0x19;
        break;
    }
}

// @symbol func_ov007_020b3d30
/* Sets up sprite `thiz` as entry idx of the data_ov007_020d7dc0..
 * data_ov007_020d7dea tables: its texture, texel origin, pixel size, shadow
 * polygon id and shadow offset, and its depth; then sizes it in world space
 * through camera 1 (func_ov007_020c43bc). */
void func_ov007_020b3d30(char *thiz, int idx) {
    Ov007Sprite *sprite = (Ov007Sprite *)thiz;
    sprite->frames[0] = WORK->textures[data_ov007_020d7dc8[idx]];
    sprite->texS = data_ov007_020d7dd0[idx * 2];
    sprite->texT = data_ov007_020d7dd1[idx * 2];
    sprite->width = data_ov007_020d7ddc[idx * 2];
    sprite->height = data_ov007_020d7ddd[idx * 2];
    sprite->shadowPolygonId = data_ov007_020d7dc0[idx];
    sprite->shadowOffset[0] = data_ov007_020d7de8[idx * 2];
    sprite->shadowOffset[1] = data_ov007_020d7dea[idx * 2];
    if (idx == 1)
        sprite->depth = -0x10c0;
    else
        sprite->depth = (int)0xffffef38;
    func_ov007_020c43bc(thiz, (int)WORK->cameras[1]);
}

// @symbol func_ov007_020b3c54
/* Maps the cursor state to a sprite mode (0..5) and re-dresses the sprite;
 * states 8..0xd also record the state as the chosen item. */
void func_ov007_020b3c54(void) {
    int mode;
    short st = CURSOR->state->currentState;
    switch (st) {
    case 0:
    case 1:
    case 2:
        mode = 0;
        break;
    case 7:
        mode = 1;
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        mode = 2;
        WORK->chosenItemId = st;
        break;
    case 14:
    case 15:
    case 16:
        mode = 3;
        break;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
        mode = 4;
        break;
    case 22:
    case 23:
        mode = 5;
        break;
    default:
        break;
    }
    CURSOR->mode = mode;
    func_ov007_020b3d30((char *)CURSOR->sprite, mode);
}

// @symbol func_ov007_020b3708
int func_ov007_020b3708(void) {
    int ip;
    int r3 = 1;
    int r2;
    Ov007MenuState *p;
    int r4;

    p = CURSOR->state;
    r2 = CURSOR->dirPressed;
    ip = p->currentState;
    r4 = p->requestedState;

    switch (ip) {
    case 0xff:
        r3 = 0;
        break;
    case 0:
        if (r2 == 2)
            r4 = 2;
        else if (r2 == 3)
            r4 = r3;
        else if (r2 == 0)
            r4 = 7;
        break;
    case 1:
        if (r2 == 2)
            r4 = 0;
        else if (r2 == 3)
            r4 = 2;
        else if (r2 == 0)
            r4 = 7;
        break;
    case 2:
        if (r2 == 2)
            r4 = r3;
        else if (r2 == 3)
            r4 = 0;
        else if (r2 == 0)
            r4 = 7;
        break;
    case 7:
        if (r2 == 1) {
            r4 = p->previousState;
        }
        break;
    case 8:
        if (r2 == 0) {
            r4 = 0xfd;
        }
        break;
    case 9:
        if (r2 == 1) {
            r4 = 0xfd;
        } else if (r2 == 2 || CURSOR->confirm != 0) {
            r3 = 0;
            CURSOR->confirm = 0;
            r4 = 0xb;
        } else if (r2 == 3) {
            r3 = 0;
            CURSOR->confirm = 0;
            r4 = 0xa;
        }
        break;
    case 0xa:
    case 0xb:
        if (p->stateTicks == 0) {
            CURSOR->confirm = 1;
            r4 = 9;
        }
        break;
    case 0xc:
    case 0xd:
        if (r2 == 0) {
            r4 = 9;
        } else if (r2 == 1) {
            r4 = 8;
        } else if (p->stateTicks == 0 && (u16)(s16)(p->previousState - 0xc) <= 1) {
            CURSOR->confirm = 1;
        } else {
            int t;
            if (r2 == 2 || ((t = CURSOR->confirm) != 0 && ip == 0xc)) {
                p->currentState = 0xc;
                r3 = 0;
                CURSOR->confirm = 0;
                r4 = 0xd;
            } else if (r2 == 3 || (t != 0 && ip == 0xd)) {
                p->currentState = 0xd;
                r3 = 0;
                CURSOR->confirm = 0;
                r4 = 0xc;
            }
        }
        break;
    case 0xe:
        if (r2 == 2)
            r4 = 0xf;
        else if (r2 == 3)
            r4 = 0x10;
        else if (r2 == 1)
            r4 = 0xfe;
        break;
    case 0xf:
        if (r2 == 2)
            r4 = 0x10;
        else if (r2 == 3)
            r4 = 0xe;
        else if (r2 == 1)
            r4 = 0xfe;
        break;
    case 0x10:
        if (r2 == 2)
            r4 = 0xe;
        else if (r2 == 3)
            r4 = 0xf;
        else if (r2 == 1)
            r4 = 0xfe;
        break;
    case 0x11:
        if (r2 == 2)
            r4 = 0x12;
        else if (r2 == 3)
            r4 = 0x13;
        else if (r2 == 0)
            r4 = 0xe;
        else if (r2 == 1)
            r4 = 0x14;
        break;
    case 0x12:
        if (r2 == 2)
            r4 = 0x13;
        else if (r2 == 3)
            r4 = 0x11;
        else if (r2 == 0)
            r4 = 0xf;
        else if (r2 == 1)
            r4 = 0x14;
        break;
    case 0x13:
        if (r2 == 2)
            r4 = 0x11;
        else if (r2 == 3)
            r4 = 0x12;
        else if (r2 == 0)
            r4 = 0x10;
        else if (r2 == 1)
            r4 = 0x14;
        break;
    case 0x14:
        if (r2 == 3 && WORK->states[1]->currentState == 0) {
            r4 = 0x15;
        } else if (r2 == 0) {
            if (WORK->states[1]->currentState == 1) {
                r4 = 0x13;
            } else {
                r4 = p->previousState;
                if ((u16)(s16)(r4 - 0xe) > 2) {
                    r4 = 0x10;
                }
            }
        }
        break;
    case 0x15:
        if (r2 == 0) {
            r4 = p->previousState;
            if ((u16)(s16)(r4 - 0xe) > 2) {
                r4 = 0xf;
            }
        } else if (r2 == 2) {
            r4 = 0x14;
        }
        break;
    case 0x16:
        if (r2 == 2) {
            r4 = 0x17;
        }
        break;
    case 0x17:
        if (r2 == 3) {
            r4 = 0x16;
        }
        break;
    }

    if (r4 != 0xfd) {
        if (r4 == 0xfe) {
            int st = WORK->states[1]->currentState;
            switch (st) {
            case 0:
                r4 = (ip == 0x10) ? 0x14 : 0x15;
                break;
            case 1:
                switch (ip) {
                case 0xe:
                    r4 = 0x11;
                    break;
                case 0xf:
                    r4 = 0x12;
                    break;
                case 0x10:
                    r4 = 0x13;
                    break;
                }
                break;
            default:
                r4 = 0x14;
                break;
            }
        }
    } else {
        u8 b = *(u8 *)(*(char **)WORK->settings + 9);
        r4 = (b != 0) ? 0xc : 0xd;
    }

    if (CURSOR->state->requestedState != r4) {
        if (r2 == 2) {
            CURSOR->nudgeX = 1;
            CURSOR->nudgeY = 0;
        } else if (r2 == 3) {
            CURSOR->nudgeX = -1;
            CURSOR->nudgeY = 0;
        } else if (r2 == 0) {
            CURSOR->nudgeX = 0;
            CURSOR->nudgeY = -1;
        } else if (r2 == 1) {
            CURSOR->nudgeX = 0;
            CURSOR->nudgeY = 1;
        }
        if (r3 != 0) {
            func_ov007_020bdeb0((CURSOR->mode != 2) ? 4 : 5);
        }
    }
    return r4;
}

// @symbol func_ov007_020b3360
void func_ov007_020b3360(void) {
    Ov007MenuState *p0 = CURSOR->state;

    if (p0->stateTicks == 0) {
        if (p0->previousState == 0xff) {
            CURSOR->nudgeX = 0;
            CURSOR->nudgeY = 1;
        }
        func_ov007_020b3c54();
    }

    {
        int state1 = WORK->states[0]->currentState;

        switch (state1) {
        case 0: {
            Ov007MenuState *s2 = WORK->states[1];
            int state2 = s2->currentState;
            switch (state2) {
            case 0:
                if (s2->requestedState == -1)
                    CURSOR->state->requestedState = func_ov007_020b3708();
                else
                    CURSOR->state->requestedState = 0xff;
                break;
            case 8:
                if (s2->stateTicks <= 1) {
                    {
                        Ov007Pad *o = WORK->pad;
                        /* the const re-read is codegen: `|= 1` is 4 bytes longer */
                        o->inputLock = *(const int *)&o->inputLock | 1;
                    }
                    func_ov007_020c2090((char *)WORK->pad);
                }
                if (WORK->states[1]->requestedState == -1)
                    CURSOR->state->requestedState = func_ov007_020b3708();
                else
                    CURSOR->state->requestedState = 0xff;
                break;
            default:
                CURSOR->state->requestedState = 0xff;
                break;
            }
            break;
        }
        case 5: {
            Ov007MenuState *s2 = WORK->states[1];
            if (s2->stateTicks == 0) {
                int state3 = s2->currentState;
                switch (state3) {
                case 0: {
                    Ov007MenuState *p = CURSOR->state;
                    int cur = p->currentState;
                    if ((u16)(s16)(cur - 0x11) <= 2)
                        p->requestedState = 0x15;
                    break;
                }
                case 1: {
                    Ov007MenuState *p = CURSOR->state;
                    if (p->currentState == 0x15)
                        p->requestedState = 0x11;
                    break;
                }
                case 3:
                case 7:
                case 9: {
                    Ov007MenuState *p = CURSOR->state;
                    int cur = p->currentState;
                    if ((u16)(s16)(cur - 0x11) <= 2)
                        p->requestedState = 0xe;
                    break;
                }
                case 0xb:
                case 0xc:
                    CURSOR->state->requestedState = 0xff;
                    return;
                default:
                    break;
                }
            }

            {
                int a;
                int b;
                Ov007MenuState *p;
                int cur;
                CURSOR->state->requestedState = func_ov007_020b3708();
                p = CURSOR->state;
                cur = p->currentState;
                a = 1;
                if (cur != 0x16 && cur != 0x17)
                    a = 0;
                b = (WORK->itemViews[0x16]->item->state->currentState == 0);
                if (a == 0 && b != 0) {
                    p->requestedState = 0x17;
                    if (CURSOR->state->currentState == 0xff)
                        func_ov007_020bdeb0(3);
                    CURSOR->returnState = CURSOR->state->currentState;
                    CURSOR->nudgeX = 1;
                    CURSOR->nudgeY = 0;
                } else if (a != 0 && b == 0) {
                    int old = CURSOR->returnState;
                    CURSOR->returnState = -1;
                    CURSOR->state->requestedState = old;
                    CURSOR->nudgeX = 0;
                    CURSOR->nudgeY = 1;
                }
            }
            break;
        }
        default:
            CURSOR->state->requestedState = 0xff;
            break;
        }
    }

    {
        Ov007MenuState *p = CURSOR->state;
        int cur = p->currentState;
        if ((u16)(s16)(cur - 0xe) <= 2)
            CURSOR->returnState = cur;
    }
}

// @symbol func_ov007_020b3190
/* On a fresh key press with B not held (and no touch, func_ov007_020c1da0):
 * from states[0] 0 / states[1] 0 it requests cursor state 0 and plays sound
 * 3; from states[0] 5 (unless states[1] is 0xb or 0xc) cursor state 0xe and
 * sound 3. states[1] 8 requests cursor state 9 at once. */
void func_ov007_020b3190(void) {
    Ov007Work *g = WORK;
    int state1 = g->states[0]->currentState;

    switch (state1) {
    case 0: {
        int state2 = g->states[1]->currentState;
        switch (state2) {
        case 0: {
            int r5 = 0;
            int r4 = 0;
            if (func_ov007_020c1da0(0) == 0) {
                Ov007Pad *p = WORK->pad;
                if (p->keys & ~p->prevKeys)
                    r4 = 1;
            }
            if (r4) {
                Ov007Pad *p2 = WORK->pad;
                if ((p2->keys & 2) == 0)
                    r5 = 1;
            }
            if (r5 == 0)
                return;
            CURSOR->state->requestedState = 0;
            func_ov007_020bdeb0(3);
            return;
        }
        case 8:
            CURSOR->state->requestedState = 9;
            return;
        }
        return;
    }
    case 5: {
        int r5 = 0;
        int r4 = 0;
        int st;
        if (func_ov007_020c1da0(0) == 0) {
            Ov007Pad *p = WORK->pad;
            if (p->keys & ~p->prevKeys)
                r4 = 1;
        }
        if (r4) {
            Ov007Pad *p2 = WORK->pad;
            if ((p2->keys & 2) == 0)
                r5 = 1;
        }
        if (r5 == 0)
            return;
        st = WORK->states[1]->currentState;
        if (st == 0xc)
            return;
        if (st == 0xb)
            return;
        CURSOR->state->requestedState = 0xe;
        func_ov007_020bdeb0(3);
        return;
    }
    }
}

// @symbol func_ov007_020b30b0
/* Allocates the cursor: state machine in state 0xff, a one-frame sprite
 * with a drop shadow, mode 1, no direction and no return state. */
void func_ov007_020b30b0(void) {
    Ov007Sprite *sprite;
    if (data_ov007_02103360 != 0)
        return;
    data_ov007_02103360 = (Ov007Cursor *)func_ov007_020c3df4(0, (void *)0x1c);
    data_ov007_02103360->state = (Ov007MenuState *)func_ov007_020c9388(0xff);
    data_ov007_02103360->sprite = (Ov007Sprite *)func_ov007_020c44e4(1);
    data_ov007_02103360->mode = 1;
    data_ov007_02103360->dirPressed = -1;
    data_ov007_02103360->confirm = 0;
    data_ov007_02103360->returnState = -1;
    data_ov007_02103360->nudgeY = 0;
    data_ov007_02103360->nudgeX = data_ov007_02103360->nudgeY;
    sprite = data_ov007_02103360->sprite;
    sprite->polygonId = 0x21;
    sprite->texColor0 = 1;
    sprite->drawFlags |= 4;
    sprite->shadowAlphaDrop = 0x14;
    sprite->depth = (int)0xffffef01;
}

// @symbol func_ov007_020b3050
/* Frees the cursor's state machine, its sprite and the cursor. */
void func_ov007_020b3050(void) {
    if (data_ov007_02103360 == 0)
        return;
    func_ov007_020c937c(data_ov007_02103360->state);
    func_ov007_020c44c4((char *)data_ov007_02103360->sprite);
    func_ov007_020c3d1c(data_ov007_02103360);
    data_ov007_02103360 = 0;
}

// @symbol func_ov007_020b2e64
/* Reads the d-pad into dirPressed with key repeat (the press frame, then
 * every 10 frames from frame 20) and a fresh A or START press into confirm,
 * ticks the cursor state, and runs the cursor (func_ov007_020b3360) unless
 * it is in state 0xff; a confirm also flips data_ov007_02103364 and calls
 * func_ov007_020b3e84. */
void func_ov007_020b2e64(char *self) {
    unsigned int *r4;
    int r2;
    int r4v;

    CURSOR->confirm = 0;
    r4 = WORK->pad->heldFrames;

    if (func_ov007_020c1da0(0) != 0) {
        r2 = -1;
    } else if (r4[0] == 1 || (r4[0] >= 0x14 && cstd::mod(r4[0] - 0x14, 0xa) == 0)) {
        r2 = 0;
    } else if (r4[1] == 1 || (r4[1] >= 0x14 && cstd::mod(r4[1] - 0x14, 0xa) == 0)) {
        r2 = 1;
    } else if (r4[2] == 1 || (r4[2] >= 0x14 && cstd::mod(r4[2] - 0x14, 0xa) == 0)) {
        r2 = 2;
    } else if (r4[3] == 1 || (r4[3] >= 0x14 && cstd::mod(r4[3] - 0x14, 0xa) == 0)) {
        r2 = 3;
    } else {
        r2 = -1;
    }

    CURSOR->dirPressed = r2;

    r4v = 0;
    if (func_ov007_020c1da0(0) == 0) {
        Ov007Pad *p = WORK->pad;
        int flag = 1;
        if (p->heldFrames[4] != 1) {
            if (p->heldFrames[10] != 1)
                flag = r4v;
        }
        if (flag != 0)
            r4v = 1;
    }
    CURSOR->confirm = r4v;
    func_ov007_020c92d0((char *)CURSOR->state);

    if (CURSOR->state->currentState != 0xff) {
        func_ov007_020b3360();
        if (CURSOR->confirm == 0) {
            return;
        }
        data_ov007_02103364 = (data_ov007_02103364 == 0) ? 1 : 0;
        func_ov007_020b3e84((short *)self);
        return;
    }

    func_ov007_020b3190();
}

// @symbol func_ov007_020b2cf0
/* Places the cursor sprite over the current item at the mode's offset
 * (func_ov007_020b3e04), easing it back from the last move's nudge over the
 * first four frames of the state. Not in state 0xff or mode 2, and outside
 * mode 5 only while func_ov007_020b79e4 allows it. */
void func_ov007_020b2cf0(void) {
    int sel;
    int factor;
    int prod_c;
    int prod_e;
    int x;
    int y;
    s16 loc0;
    s16 loc2;

    if (data_ov007_02103360->state->currentState == 0xff)
        return;
    if (data_ov007_02103360->mode == 2)
        return;
    if (data_ov007_02103360->mode != 5) {
        if (func_ov007_020b79e4() != 0)
            return;
    }
    prod_c = 0;
    prod_e = 0;
    sel = data_ov007_02103360->state->stateTicks;
    func_ov007_020b3e04(data_ov007_02103360->mode, &loc0, &loc2);

    {
        Ov007Sprite *q = WORK->itemViews[data_ov007_02103360->state->currentState]->sprite;
        x = loc0 + (q->screenX >> 12);
        y = loc2 + (q->screenY >> 12);
    }
    if (sel <= 4) {
        if (sel <= 0) {
            factor = 0;
        } else if (sel >= 4) {
            factor = 0x1000;
        } else {
            factor = cstd::div(sel << 12, 4);
        }
        factor = 0x1000 - factor;
        prod_c = -data_ov007_02103360->nudgeX * factor * 6;
        prod_e = -data_ov007_02103360->nudgeY * factor * 6;
    }
    data_ov007_02103360->sprite->screenX = prod_c + (x << 12);
    data_ov007_02103360->sprite->screenY = prod_e + (y << 12);
    func_ov007_020c4388((char *)data_ov007_02103360->sprite, (int)WORK->cameras[1]);
    func_ov007_020c421c((char *)data_ov007_02103360->sprite);
}

// @symbol func_ov007_020b2cdc
/* The cursor's state machine. */
int func_ov007_020b2cdc(void) {
    return (int)data_ov007_02103360->state;
}
}
