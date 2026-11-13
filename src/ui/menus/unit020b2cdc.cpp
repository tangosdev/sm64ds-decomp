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
 * START into confirm, ticks the cursor state, and runs Cursor_UpdateState,
 * which picks the next cursor state from the menu's own states[0] /
 * states[1] and the cursor's Cursor_NextState (the per-state move
 * table), or Cursor_UpdateParked while the cursor is parked in state 0xff.
 * Cursor_UpdateMode maps the cursor state to one of six sprite modes and
 * re-dresses the sprite through Cursor_SetSpriteMode;
 * func_ov007_020b2cf0 places the sprite over the current item with the
 * per-mode offset Cursor_GetModeOffset and eases it back from the last
 * move's nudge; Cursor_TouchItem turns a confirm into a stylus touch on
 * the current item.
 *
 * Edges: the bss words 0x02103360 (the cursor) and 0x02103364 (a confirm
 * toggle) are referenced only by these functions, and every function here
 * touches the cursor or is called only from one that does
 * (Cursor_GetModeOffset, Cursor_SetSpriteMode). The previous function,
 * func_ov007_020b2bd4 (bss 0x02103348 / 0x02103354), and the next one,
 * func_ov007_020b3edc (the file loader's release, bss 0x0210336c /
 * 0x02103370), touch none of it.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The Cursor_ names are coined from the
 * bodies, after this file's Ov007Cursor; the five functions other units
 * call (func_ov007_020b2cdc, 020b2cf0, 020b2e64, 020b3050 and 020b30b0)
 * keep their address names for now.
 */
#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Item.h"
#include "Ov007Pad.h"
#include "Ov007Touch.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

/* data_ov007_02103360: the 0x1c-byte cursor func_ov007_020b30b0 allocates. */
typedef struct Ov007Cursor {
    Ov007MenuState *state; /* 0x00: func_ov007_020c9388(0xff); func_ov007_020b2e64 ticks it */
    Ov007Sprite *sprite;   /* 0x04: func_ov007_020c44e4(1) */
    /* 0x08: Cursor_UpdateMode's sprite mode, 0..5; 2 picks the second
     * move sound in Cursor_NextState */
    int mode;
    /* 0x0c/0x0e: the last move's direction, -1/0/1 per axis;
     * func_ov007_020b2cf0 eases the sprite back from it */
    s16 nudgeX;
    s16 nudgeY;
    /* 0x10, medium confidence: func_ov007_020b2e64 sets it on a fresh A or
     * START press; states 9 and 0xc/0xd in Cursor_NextState take it as a
     * step and clear it */
    int confirm;
    /* 0x14: func_ov007_020b2e64's key-repeat direction this frame: -1 none,
     * 0 up, 1 down, 2 right, 3 left */
    int dirPressed;
    /* 0x18: the state Cursor_UpdateState returns to when the cursor leaves
     * states 0x16/0x17; -1 none */
    int returnState;
} Ov007Cursor;

typedef char Ov007Cursor_size_must_be_0x1c[sizeof(Ov007Cursor) == 0x1c ? 1 : -1];

extern Ov007Cursor *data_ov007_02103360;
extern int data_ov007_02103364;

extern unsigned char data_ov007_020d7dc0[];
extern unsigned char data_ov007_020d7dc8[];
extern unsigned char data_ov007_020d7dd0[];
extern unsigned char data_ov007_020d7dd1[];
extern unsigned char data_ov007_020d7ddc[];
extern unsigned char data_ov007_020d7ddd[];
extern short data_ov007_020d7de8[];
extern short data_ov007_020d7dea[];

int func_ov007_020b79e4(void);
int func_ov007_020bdeb0(int a);
extern int func_ov007_020c1da0(int i);
extern "C" void func_ov007_020c2090(Ov007Pad *pad);
extern "C" void func_ov007_020c3d1c(void *self);
extern "C" void *func_ov007_020c3df4(const char *name, int size);
void func_ov007_020c92d0(Ov007MenuState *state);
extern "C" void func_ov007_020c937c(void *state);
void *func_ov007_020c9388(int initialState);

/* The menu cursor (data_ov007_02103360). */
#define CURSOR data_ov007_02103360

/* Writes a stylus touch at the current item's position, in pixels: touch
 * down, both axes valid. */
void Cursor_TouchItem(TPData *sample) {
    Ov007Item *item = WORK->itemViews[CURSOR->state->currentState]->item;
    int x = item->posX;
    int y = item->posY;
    sample->x = x >> 0xc;
    sample->y = y >> 0xc;
    sample->touch = 1;
    sample->validity = 0;
}

/* The sprite offset, in pixels, for each of Cursor_UpdateMode's modes. */
void Cursor_GetModeOffset(int mode, s16 *dx, s16 *dy) {
    *dy = 0;
    *dx = *dy;
    switch (mode) {
    case 0:
        *dy = *dy - 0x16;
        break;
    case 1:
        *dy = 0x10;
        break;
    case 2:
        *dx = 0x20;
        break;
    case 3:
        *dy = -0x16;
        break;
    case 4:
        *dy = -0x10;
        break;
    case 5:
        *dx = -0x19;
        break;
    }
}

/* Sets up `sprite` as entry idx of the data_ov007_020d7dc0..
 * data_ov007_020d7dea tables: its texture, texel origin, pixel size, shadow
 * polygon id and shadow offset, and its depth; then sizes it in world space
 * through camera 1 (func_ov007_020c43bc). */
void Cursor_SetSpriteMode(Ov007Sprite *sprite, int idx) {
    sprite->frames[0] = WORK->textures[data_ov007_020d7dc8[idx]];
    sprite->texS = data_ov007_020d7dd0[idx * 2];
    sprite->texT = data_ov007_020d7dd1[idx * 2];
    sprite->width = data_ov007_020d7ddc[idx * 2];
    sprite->height = data_ov007_020d7ddd[idx * 2];
    sprite->shadowPolygonId = data_ov007_020d7dc0[idx];
    sprite->shadowOffset.x = data_ov007_020d7de8[idx * 2];
    sprite->shadowOffset.y = data_ov007_020d7dea[idx * 2];
    if (idx == 1)
        sprite->depth = -0x10c0;
    else
        sprite->depth = 0xffffef38;
    func_ov007_020c43bc(sprite, WORK->cameras[1]);
}

/* Maps the cursor state to a sprite mode (0..5) and re-dresses the sprite;
 * states 8..0xd also record the state as the chosen item. */
void Cursor_UpdateMode(void) {
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
    Cursor_SetSpriteMode(CURSOR->sprite, mode);
}

/* The per-state move table: returns the state dirPressed (or a confirm, in
 * states 9 and 0xc/0xd) moves the cursor to. 0xfd resolves by the boot
 * args' backlight flag (0xc on, 0xd off) and 0xfe by states[1]. A move
 * records its nudge and plays sound 4 (5 in mode 2) unless it came from a
 * confirm. */
int Cursor_NextState(void) {
    int cur;
    int playSound = 1;
    int dir;
    Ov007MenuState *p;
    int next;

    p = CURSOR->state;
    dir = CURSOR->dirPressed;
    cur = p->currentState;
    next = p->requestedState;

    switch (cur) {
    case 0xff:
        playSound = 0;
        break;
    case 0:
        if (dir == 2)
            next = 2;
        else if (dir == 3)
            next = 1;
        else if (dir == 0)
            next = 7;
        break;
    case 1:
        if (dir == 2)
            next = 0;
        else if (dir == 3)
            next = 2;
        else if (dir == 0)
            next = 7;
        break;
    case 2:
        if (dir == 2)
            next = 1;
        else if (dir == 3)
            next = 0;
        else if (dir == 0)
            next = 7;
        break;
    case 7:
        if (dir == 1) {
            next = p->previousState;
        }
        break;
    case 8:
        if (dir == 0) {
            next = 0xfd;
        }
        break;
    case 9:
        if (dir == 1) {
            next = 0xfd;
        } else if (dir == 2 || CURSOR->confirm != 0) {
            playSound = 0;
            CURSOR->confirm = 0;
            next = 0xb;
        } else if (dir == 3) {
            playSound = 0;
            CURSOR->confirm = 0;
            next = 0xa;
        }
        break;
    case 0xa:
    case 0xb:
        if (p->stateTicks == 0) {
            CURSOR->confirm = 1;
            next = 9;
        }
        break;
    case 0xc:
    case 0xd:
        if (dir == 0) {
            next = 9;
        } else if (dir == 1) {
            next = 8;
        } else if (p->stateTicks == 0 && (u16)(s16)(p->previousState - 0xc) <= 1) {
            CURSOR->confirm = 1;
        } else {
            int t;
            if (dir == 2 || ((t = CURSOR->confirm) != 0 && cur == 0xc)) {
                p->currentState = 0xc;
                playSound = 0;
                CURSOR->confirm = 0;
                next = 0xd;
            } else if (dir == 3 || (t != 0 && cur == 0xd)) {
                p->currentState = 0xd;
                playSound = 0;
                CURSOR->confirm = 0;
                next = 0xc;
            }
        }
        break;
    case 0xe:
        if (dir == 2)
            next = 0xf;
        else if (dir == 3)
            next = 0x10;
        else if (dir == 1)
            next = 0xfe;
        break;
    case 0xf:
        if (dir == 2)
            next = 0x10;
        else if (dir == 3)
            next = 0xe;
        else if (dir == 1)
            next = 0xfe;
        break;
    case 0x10:
        if (dir == 2)
            next = 0xe;
        else if (dir == 3)
            next = 0xf;
        else if (dir == 1)
            next = 0xfe;
        break;
    case 0x11:
        if (dir == 2)
            next = 0x12;
        else if (dir == 3)
            next = 0x13;
        else if (dir == 0)
            next = 0xe;
        else if (dir == 1)
            next = 0x14;
        break;
    case 0x12:
        if (dir == 2)
            next = 0x13;
        else if (dir == 3)
            next = 0x11;
        else if (dir == 0)
            next = 0xf;
        else if (dir == 1)
            next = 0x14;
        break;
    case 0x13:
        if (dir == 2)
            next = 0x11;
        else if (dir == 3)
            next = 0x12;
        else if (dir == 0)
            next = 0x10;
        else if (dir == 1)
            next = 0x14;
        break;
    case 0x14:
        if (dir == 3 && WORK->states[1]->currentState == 0) {
            next = 0x15;
        } else if (dir == 0) {
            if (WORK->states[1]->currentState == 1) {
                next = 0x13;
            } else {
                next = p->previousState;
                if ((u16)(s16)(next - 0xe) > 2) {
                    next = 0x10;
                }
            }
        }
        break;
    case 0x15:
        if (dir == 0) {
            next = p->previousState;
            if ((u16)(s16)(next - 0xe) > 2) {
                next = 0xf;
            }
        } else if (dir == 2) {
            next = 0x14;
        }
        break;
    case 0x16:
        if (dir == 2) {
            next = 0x17;
        }
        break;
    case 0x17:
        if (dir == 3) {
            next = 0x16;
        }
        break;
    }

    if (next != 0xfd) {
        if (next == 0xfe) {
            int st = WORK->states[1]->currentState;
            switch (st) {
            case 0:
                next = (cur == 0x10) ? 0x14 : 0x15;
                break;
            case 1:
                switch (cur) {
                case 0xe:
                    next = 0x11;
                    break;
                case 0xf:
                    next = 0x12;
                    break;
                case 0x10:
                    next = 0x13;
                    break;
                }
                break;
            default:
                next = 0x14;
                break;
            }
        }
    } else {
        u8 b = WORK->settings->bootArgs->backlightOn;
        next = (b != 0) ? 0xc : 0xd;
    }

    if (CURSOR->state->requestedState != next) {
        if (dir == 2) {
            CURSOR->nudgeX = 1;
            CURSOR->nudgeY = 0;
        } else if (dir == 3) {
            CURSOR->nudgeX = -1;
            CURSOR->nudgeY = 0;
        } else if (dir == 0) {
            CURSOR->nudgeX = 0;
            CURSOR->nudgeY = -1;
        } else if (dir == 1) {
            CURSOR->nudgeX = 0;
            CURSOR->nudgeY = 1;
        }
        if (playSound != 0) {
            func_ov007_020bdeb0((CURSOR->mode != 2) ? 4 : 5);
        }
    }
    return next;
}

/* Runs the cursor outside state 0xff: re-dresses the sprite on entering a
 * state, then requests Cursor_NextState while the menu (states[0] 0 or 5)
 * lets the cursor move and parks it in 0xff otherwise. Under states[0] 5
 * it also jumps to state 0x17 while item view 0x16 is in state 0 and back
 * to returnState after; it keeps the last of states 0xe..0x10 as
 * returnState. */
void Cursor_UpdateState(void) {
    Ov007MenuState *p0 = CURSOR->state;

    if (p0->stateTicks == 0) {
        if (p0->previousState == 0xff) {
            CURSOR->nudgeX = 0;
            CURSOR->nudgeY = 1;
        }
        Cursor_UpdateMode();
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
                    CURSOR->state->requestedState = Cursor_NextState();
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
                    func_ov007_020c2090(WORK->pad);
                }
                if (WORK->states[1]->requestedState == -1)
                    CURSOR->state->requestedState = Cursor_NextState();
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
                CURSOR->state->requestedState = Cursor_NextState();
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

/* On a fresh key press with B not held (and no touch, func_ov007_020c1da0):
 * from states[0] 0 / states[1] 0 it requests cursor state 0 and plays sound
 * 3; from states[0] 5 (unless states[1] is 0xb or 0xc) cursor state 0xe and
 * sound 3. states[1] 8 requests cursor state 9 at once. */
void Cursor_UpdateParked(void) {
    Ov007Work *g = WORK;
    int state1 = g->states[0]->currentState;

    switch (state1) {
    case 0: {
        int state2 = g->states[1]->currentState;
        switch (state2) {
        case 0: {
            int accept = 0;
            int pressed = 0;
            if (func_ov007_020c1da0(0) == 0) {
                Ov007Pad *p = WORK->pad;
                if (p->keys & ~p->prevKeys)
                    pressed = 1;
            }
            if (pressed) {
                Ov007Pad *p2 = WORK->pad;
                if ((p2->keys & 2) == 0)
                    accept = 1;
            }
            if (accept == 0)
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
        int accept = 0;
        int pressed = 0;
        int st;
        if (func_ov007_020c1da0(0) == 0) {
            Ov007Pad *p = WORK->pad;
            if (p->keys & ~p->prevKeys)
                pressed = 1;
        }
        if (pressed) {
            Ov007Pad *p2 = WORK->pad;
            if ((p2->keys & 2) == 0)
                accept = 1;
        }
        if (accept == 0)
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

/* Allocates the cursor: state machine in state 0xff, a one-frame sprite
 * with a drop shadow, mode 1, no direction and no return state. */
void func_ov007_020b30b0(void) {
    Ov007Sprite *sprite;
    if (data_ov007_02103360 != 0)
        return;
    data_ov007_02103360 = (Ov007Cursor *)func_ov007_020c3df4(0, 0x1c);
    data_ov007_02103360->state = (Ov007MenuState *)func_ov007_020c9388(0xff);
    data_ov007_02103360->sprite = func_ov007_020c44e4(1);
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
    sprite->depth = 0xffffef01;
}

/* Frees the cursor's state machine, its sprite and the cursor. */
void func_ov007_020b3050(void) {
    if (data_ov007_02103360 == 0)
        return;
    func_ov007_020c937c(data_ov007_02103360->state);
    func_ov007_020c44c4(data_ov007_02103360->sprite);
    func_ov007_020c3d1c(data_ov007_02103360);
    data_ov007_02103360 = 0;
}

/* Reads the d-pad into dirPressed with key repeat (the press frame, then
 * every 10 frames from frame 20) and a fresh A or START press into confirm,
 * ticks the cursor state, and runs the cursor (Cursor_UpdateState) unless
 * it is in state 0xff; a confirm also flips data_ov007_02103364 and
 * replaces this frame's touch sample with a touch on the current item
 * (Cursor_TouchItem). */
void func_ov007_020b2e64(TPData *sample) {
    unsigned int *held;
    int dir;
    int accept;

    CURSOR->confirm = 0;
    held = WORK->pad->heldFrames;

    if (func_ov007_020c1da0(0) != 0) {
        dir = -1;
    } else if (held[0] == 1 || (held[0] >= 0x14 && FX_ModS32(held[0] - 0x14, 0xa) == 0)) {
        dir = 0;
    } else if (held[1] == 1 || (held[1] >= 0x14 && FX_ModS32(held[1] - 0x14, 0xa) == 0)) {
        dir = 1;
    } else if (held[2] == 1 || (held[2] >= 0x14 && FX_ModS32(held[2] - 0x14, 0xa) == 0)) {
        dir = 2;
    } else if (held[3] == 1 || (held[3] >= 0x14 && FX_ModS32(held[3] - 0x14, 0xa) == 0)) {
        dir = 3;
    } else {
        dir = -1;
    }

    CURSOR->dirPressed = dir;

    accept = 0;
    if (func_ov007_020c1da0(0) == 0) {
        Ov007Pad *p = WORK->pad;
        int flag = 1;
        if (p->heldFrames[4] != 1) {
            if (p->heldFrames[10] != 1)
                flag = accept;
        }
        if (flag != 0)
            accept = 1;
    }
    CURSOR->confirm = accept;
    func_ov007_020c92d0(CURSOR->state);

    if (CURSOR->state->currentState != 0xff) {
        Cursor_UpdateState();
        if (CURSOR->confirm == 0) {
            return;
        }
        data_ov007_02103364 = (data_ov007_02103364 == 0) ? 1 : 0;
        Cursor_TouchItem(sample);
        return;
    }

    Cursor_UpdateParked();
}

/* Places the cursor sprite over the current item at the mode's offset
 * (Cursor_GetModeOffset), easing it back from the last move's nudge over the
 * first four frames of the state. Not in state 0xff or mode 2, and outside
 * mode 5 only while func_ov007_020b79e4 allows it. */
void func_ov007_020b2cf0(void) {
    int ticks;
    int factor;
    int easeX;
    int easeY;
    int x;
    int y;
    s16 dx;
    s16 dy;

    if (data_ov007_02103360->state->currentState == 0xff)
        return;
    if (data_ov007_02103360->mode == 2)
        return;
    if (data_ov007_02103360->mode != 5) {
        if (func_ov007_020b79e4() != 0)
            return;
    }
    easeX = 0;
    easeY = 0;
    ticks = data_ov007_02103360->state->stateTicks;
    Cursor_GetModeOffset(data_ov007_02103360->mode, &dx, &dy);

    {
        Ov007Sprite *q = WORK->itemViews[data_ov007_02103360->state->currentState]->sprite;
        x = dx + (q->screenX >> 12);
        y = dy + (q->screenY >> 12);
    }
    if (ticks <= 4) {
        factor = ProgressRatio(ticks, 4);
        factor = 0x1000 - factor;
        easeX = -data_ov007_02103360->nudgeX * factor * 6;
        easeY = -data_ov007_02103360->nudgeY * factor * 6;
    }
    data_ov007_02103360->sprite->screenX = easeX + (x << 12);
    data_ov007_02103360->sprite->screenY = easeY + (y << 12);
    func_ov007_020c4388(data_ov007_02103360->sprite, WORK->cameras[1]);
    func_ov007_020c421c(data_ov007_02103360->sprite);
}

/* The cursor's state machine. */
Ov007MenuState *func_ov007_020b2cdc(void) {
    return data_ov007_02103360->state;
}
