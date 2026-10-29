//cpp
/* unit020b413c -- ov007 unit 0x020b413c..0x020b58e4: 13 functions, .text only.
 *
 * The title menu's items. ov007 is the title and file-select menu program
 * that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or vtable, so no
 * class name is claimed and the file keeps the unit<addr> name.
 *
 * What the code does. Each menu item is an Ov007ItemView: the 0x2c-byte item
 * record func_ov007_020aea6c allocates and the sprite that places it.
 * func_ov007_020b4c04 builds a view for item id arg0 from the item tables
 * (layout row, hit box, texture cell) and func_ov007_020b4bd4 frees one.
 * func_ov007_020b44ec runs a view's frame: it ticks the item's state and
 * runs the state-0 handler (func_ov007_020b5068: the B button selects
 * items 8, 0x14 and 0x17) or the state-2 handler (func_ov007_020b50e8: the
 * pop / nudge / squash animation that then requests the next menu states).
 * func_ov007_020b45b0 selects an item once it is selectable, and
 * func_ov007_020b46b0 does the same for a stylus press or hold.
 * func_ov007_020b4b5c resets a view's sprite to its home point and
 * func_ov007_020b4464 re-places a sprite that moved.
 * func_ov007_020b42d8 builds the three number sprites (Ov007Work's
 * numberSprites) and func_ov007_020b413c draws a number with them on top
 * of a view's sprite. func_ov007_020b43dc and func_ov007_020b4438 set and
 * read the item-0x16/0x17 choice word, data_ov007_02103428.
 *
 * Edges. The .rodata tables the functions read interleave:
 * func_ov007_020b42d8 reads data_ov007_020d75e0, 020d75e4, 020d75f0,
 * 020d75f1, 020d761c and 020d761e, func_ov007_020b4c04 reads 020d75e8,
 * 020d75f8, 020d7600 and 020d7608, and func_ov007_020b413c reads 020d7610.
 * A TU's .rodata is contiguous, so those three functions are one TU. The
 * bss word data_ov007_02103428 is referenced by func_ov007_020b43dc,
 * func_ov007_020b4438 and func_ov007_020b50e8 and nothing else, so they are
 * one TU too. The two runs overlap and .text is contiguous per TU, so
 * 0x020b413c..0x020b58e4 is one unit. The previous function,
 * func_ov007_020b4100, ends unit020b3edc (bss 0x02103368..0x02103428). The
 * unit may run on past func_ov007_020b50e8 (func_ov007_020b44ec calls
 * func_ov007_020b58e4 and func_ov007_020b5bf0), but calls are not evidence,
 * so those stay one-function sources until a private data reference ties
 * them in.
 *
 * Code generation. The whole unit is compiled with opt_loop_invariants
 * off: func_ov007_020b413c's digit loop keeps &digit->screenX inside the
 * loop only under it (four bytes short without), and the other twelve
 * functions compile the same either way. Each place where a shape is
 * load-bearing says so.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "cstd.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"

#pragma opt_loop_invariants off

/* The 10-byte layout row func_ov007_020aea6c's item points at; names as in
 * func_ov007_020b5bf0. */
typedef struct Ov007ItemLayout {
    u16 kind;  /* 0x0: func_ov007_020b5bf0 switches on it; func_ov007_020b44ec skips 0xa */
    s16 homeX; /* 0x2: func_ov007_020ae810 seeds the item position from home */
    s16 homeY; /* 0x4 */
    char pad_06[4];
} Ov007ItemLayout;

typedef char Ov007ItemLayout_size_must_be_0xa[sizeof(Ov007ItemLayout) == 0xa ? 1 : -1];

/* The 0x14-byte touch rectangles func_ov007_020b4c04 points hitBox at
 * (data_ov007_020d78f8, data_ov007_020d76c0, ...): pixel offsets from the
 * sprite's screen position. */
typedef struct Ov007HitBox {
    char pad_00[4];
    s32 minX; /* 0x04 */
    s32 minY; /* 0x08 */
    s32 maxX; /* 0x0c */
    s32 maxY; /* 0x10 */
} Ov007HitBox;

typedef char Ov007HitBox_size_must_be_0x14[sizeof(Ov007HitBox) == 0x14 ? 1 : -1];

/* The 0x2c-byte record func_ov007_020aea6c allocates; only the slots this
 * unit touches. */
typedef struct Ov007Item {
    u16 id; /* 0x00: func_ov007_020b4c04's index */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1); ticked by func_ov007_020ae834 */
    s32 posX;              /* 0x08: Q12; func_ov007_020ae810 seeds it from the layout */
    s32 posY;              /* 0x0c */
    s32 fade;              /* 0x10: Q12; selectable once fully faded in (>= 0x1000) */
    char pad_14[0xc];
    s32 ready;               /* 0x20: func_ov007_020ae834 sets it once the state settles */
    Ov007ItemLayout *layout; /* 0x24 */
    Ov007HitBox *hitBox;     /* 0x28 */
} Ov007Item;

typedef char Ov007Item_size_must_be_0x2c[sizeof(Ov007Item) == 0x2c ? 1 : -1];

/* The 0x1c-byte record func_ov007_020b4c04 returns (freed by
 * func_ov007_020b4bd4, stored in Ov007Work's itemViews by
 * func_ov007_020b693c). func_ov007_020b5f64 slides the item from slideFrom
 * to slideTo by slideT. */
typedef struct Ov007ItemView {
    Ov007Item *item;               /* 0x00 */
    Ov007Sprite *sprite;           /* 0x04: func_ov007_020c44e4 */
    s32 pad_08;                    /* 0x08: cleared by func_ov007_020b4c04; no reader found */
    s32 slideT;                    /* 0x0c: slideTween output, 0..0x1000 */
    struct Ov007Tween *slideTween; /* 0x10: func_ov007_020c908c, bound to slideT */
    s16 slideFromX;                /* 0x14 */
    s16 slideFromY;                /* 0x16 */
    s16 slideToX;                  /* 0x18 */
    s16 slideToY;                  /* 0x1a */
} Ov007ItemView;

typedef char Ov007ItemView_size_must_be_0x1c[sizeof(Ov007ItemView) == 0x1c ? 1 : -1];

/* The 0x3c-byte record at Ov007Work's settings; only the slots this unit
 * reads. */
typedef struct Ov007Settings {
    /* 0x00: func_ov007_020b7138's second argument; byte 9 picks which of
     * items 0xc and 0xd is live */
    u8 *bootArgs;
    char pad_04[0x36];
    u8 language; /* 0x3a: func_ov007_020b6a84 seeds it from the owner-info language */
} Ov007Settings;

/* The pad record at Ov007Work's pad; names as in unit020b8f50. */
typedef struct Ov007Pad {
    u16 keys;     /* 0x00: held this frame */
    u16 prevKeys; /* 0x02: held last frame */
} Ov007Pad;

/* The 0xc-byte item rows starting at data_ov007_020d77d8. Their columns
 * are separate ROM symbols (data_ov007_020d77d8 hit-box index,
 * data_ov007_020d77dc flags, data_ov007_020d77e0 sprite mode,
 * data_ov007_020d77e1 depth index), so each column is its own extern and
 * is indexed by id * 0xc. The flags: bit 0 never selectable, bit 1
 * selectable before the fade-in ends. */
typedef struct Ov007ItemFlags {
    u32 flags;
    char pad_04[8];
} Ov007ItemFlags;

/* The work block. The extern keeps the tree's char * spelling (the
 * declaration-agreement gate); the typed view is reached through WORK. */
extern char *data_ov007_0210342c;
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* Which of items 0x16 / 0x17 was picked: 1 while the choice is open,
 * 2 for 0x16, 3 for 0x17. */
extern int data_ov007_02103428;

extern Ov007ItemFlags data_ov007_020d77dc[];
extern s32 data_ov007_020d77d8[];
extern u8 data_ov007_020d77e0[];
extern u8 data_ov007_020d77e1[];
extern Ov007ItemLayout data_ov007_020d76e8[];
extern Ov007ItemLayout data_ov007_020d7680[];
extern Ov007HitBox data_ov007_020d78f8[];
extern Ov007HitBox data_ov007_020d76c0[];
extern Ov007HitBox data_ov007_020d7658;
extern Ov007HitBox data_ov007_020d766c;
extern u8 data_ov007_020d76a0[];
extern u8 data_ov007_020d76a1[];
extern s32 data_ov007_020d7648[];
extern u16 data_ov007_020d75e8[];
extern u16 data_ov007_020d75f8[];
extern u16 data_ov007_020d7608[];
extern u16 data_ov007_020d7600[];
extern u16 data_ov007_020d7628[];
extern u16 data_ov007_020d7638[];
extern s16 data_ov007_020d761c[];
extern s16 data_ov007_020d761e[];
extern u8 data_ov007_020d75f0[];
extern u8 data_ov007_020d75f1[];
extern u8 data_ov007_020d75e0[];
extern u8 data_ov007_020d75e4[];
/* each digit's width in pixels */
extern unsigned char data_ov007_020d7610[];

extern "C" {

extern void func_ov007_020ae834(char *item);
extern char *func_ov007_020aea6c(int id, void *layout, void *hitBox);
extern int func_ov007_020aea4c(char *item);
extern void func_ov007_020aec94(int choice);
extern int func_ov007_020b58e4(char *view);
extern void func_ov007_020b5bf0(Ov007ItemView *view);
extern void func_ov007_020b6134(int a);
extern int func_ov007_020b6208(int id);
extern void func_ov007_020b63e4(char **view);
extern void func_ov007_020b647c(int r0, int r1, short r2);
extern void func_ov007_020b64a8(int dir);
extern int func_ov007_020b79e4(void);
extern void func_ov007_020b7a44(void);
extern void func_ov007_020b7d94(void *self);
extern void func_ov007_020b7eec(u8 *bootArgs);
extern int func_ov007_020b8f78(void);
extern int func_ov007_020b8fa0(void);
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020c1da0(int i);
extern int func_ov007_020c3598(int value, int count, char *digits, int base);
extern void func_ov007_020c3d1c(void *p);
extern void *func_ov007_020c3df4(int heap, int size);
extern void func_ov007_020c41dc(char *parent, char *sprite, int camera);
extern void func_ov007_020c4128(char *parent, char *sprite, int camera);
extern void func_ov007_020c421c(char *sprite);
extern void func_ov007_020c42f8(char *sprite);
extern void func_ov007_020c4388(char *sprite, int camera);
extern void func_ov007_020c43bc(void *sprite, void *camera);
extern int func_ov007_020c44c4(char *sprite);
extern int func_ov007_020c44e4(int mode);
extern int func_ov007_020c9080(int tween);
extern void *func_ov007_020c908c(void *out);

void func_ov007_020b45b0(void *t);

/* The item's state-2 step: on the state's first tick, sends the follow-up
 * state requests for this item, then over `thresh` ticks pops (ids 3/4),
 * nudges sideways (ids 10/11) or squashes its sprite; when the ticks run out
 * it resets the scale and requests the item's next state. arg keeps the
 * char * spelling of the definition; `view` is the typed view. */
// @symbol func_ov007_020b50e8
void func_ov007_020b50e8(char *arg) {
    Ov007ItemView *view = (Ov007ItemView *)arg;
    Ov007Item *st;
    u16 state;
    int t;
    int result;
    int is34;
    int thresh;
    Ov007MenuState *pv;
    u8 *bootArgs;
    int v4;
    int posX;
    Ov007ItemLayout *r3p;

    st = view->item;
    state = st->id;
    r3p = st->layout;
    bootArgs = WORK->settings->bootArgs;
    t = st->state->stateTicks;
    v4 = r3p->kind;
    is34 = (state == 3 || state == 4);
    thresh = is34 ? 0x18 : 8;
    pv = WORK->states[1];
    posX = r3p->homeX << 12;
    result = 0x1000;

    if (t == 0) {
        func_ov007_020c1d8c(0);
        if (v4 == 0) {
            func_ov007_020b647c(view->item->id, 0, 6);
            func_ov007_020b647c(view->item->id, 1, 6);
            func_ov007_020b647c(view->item->id, 2, 6);
            func_ov007_020b647c(view->item->id, 7, 6);
        } else if ((u32)(state - 0xe) <= 2 && pv->currentState == 0) {
            func_ov007_020b647c(view->item->id, 0xe, 4);
            func_ov007_020b647c(view->item->id, 0xf, 4);
            func_ov007_020b647c(view->item->id, 0x10, 4);
            func_ov007_020b647c(view->item->id, 0x15, 4);
            func_ov007_020b647c(view->item->id, 0x14, 4);
        }
        {
            int flag;
            flag = 1;
            func_ov007_020b6208(state);
            switch (st->id) {
            case 9:
                break;
            case 11:
                func_ov007_020b64a8(flag);
                break;
            case 10:
                func_ov007_020b64a8(-1);
                break;
            case 12:
                if (bootArgs[9] == 0) {
                    WORK->callback(4);
                }
                break;
            case 13:
                if (bootArgs[9] != 0) {
                    WORK->callback(5);
                }
                break;
            default:
                flag = 0;
                break;
            }
            if (flag != 0) {
                func_ov007_020b7eec(WORK->settings->bootArgs);
            }
        }
    }

    if (t < thresh) {
        int q;
        if (t <= 0) {
            q = 0;
        } else if (t >= thresh) {
            q = 0x1000;
        } else {
            q = cstd::div(t << 12, thresh);
        }
        if (is34 != 0) {
            if (q < 0x555) {
                int c;
                int w;
                w = q * 3;
                w <<= 2;
                if (w <= 0x1000) {
                    c = FX_MUL(w, 0x1000);
                } else if (w <= 0x3000) {
                    c = FX_MUL(w - 0x1000, 0x800) + 0x1000;
                } else {
                    c = FX_MUL(w - 0x3000, -0x2000) + 0x2000;
                }
                result = 0x1000 - FX_MUL(c, 0x4b0);
            } else {
                int c;
                int w;
                w = ((q * 3 - 0x1000) / 2) * 4;
                if (w <= 0x1000) {
                    c = FX_MUL(w, 0x800);
                } else if (w <= 0x3000) {
                    c = FX_MUL(w - 0x1000, 0x400) + 0x800;
                } else {
                    c = FX_MUL(w - 0x3000, -0x1000) + 0x1000;
                }
                result = FX_MUL(c, 0x640) + 0x1000;
            }
        } else {
            u16 s2;
            s2 = st->id;
            if ((u16)(s2 + 0xfff6) <= 1) {
                int dir;
                int w;
                int d8;
                dir = (s2 == 10) ? -1 : 1;
                w = q * 2;
                if (w > 0x1000) {
                    w = -(w - 0x2000);
                }
                d8 = dir * 8;
                posX += d8 * w;
            } else {
                int c;
                int w;
                w = q * 4;
                if (w <= 0x1000) {
                    c = FX_MUL(w, 0x1000);
                } else if (w <= 0x3000) {
                    c = FX_MUL(w - 0x1000, -0x2ec) + 0x1000;
                } else {
                    c = FX_MUL(w - 0x3000, -0xa28) + 0xa28;
                }
                result = 0x1000 - FX_MUL(c, 0x514);
            }
        }
    } else {
        Ov007Sprite *p;
        int next;
        u16 s;
        p = view->sprite;
        next = 0;
        p->scaleY = 0x1000;
        p->scaleX = p->scaleY;
        s = st->id;
        switch (s) {
        case 0:
            func_ov007_020aec94(9);
            break;
        case 1:
            func_ov007_020aec94(6);
            break;
        case 2:
            func_ov007_020aec94(7);
            break;
        case 7:
            pv->requestedState = 8;
            next = 6;
            break;
        case 3:
            next = 1;
            WORK->states[0]->requestedState = 2;
            break;
        case 4:
            next = 1;
            WORK->states[0]->requestedState = 3;
            break;
        case 6:
            WORK->states[0]->requestedState = 4;
            break;
        case 8:
            pv->requestedState = next;
            break;
        case 14:
        case 15:
        case 16:
            func_ov007_020b6134(s - 0xe);
            break;
        case 21:
            pv->requestedState = 1;
            break;
        case 17:
            pv->requestedState = 3;
            break;
        case 18:
            pv->requestedState = 7;
            break;
        case 19:
            pv->requestedState = 9;
            break;
        case 20:
            switch (pv->currentState) {
            case 0:
                pv->requestedState = 0xb;
                break;
            case 1:
                pv->requestedState = next;
                break;
            case 2:
                pv->requestedState = 6;
                break;
            case 3:
            case 9:
                pv->requestedState = 1;
                break;
            case 7: {
                Ov007Work *d;
                d = WORK;
                if (d->selectedSlot != -1) {
                    d->selectedSlot = -1;
                } else {
                    pv->requestedState = 1;
                }
                break;
            }
            case 12:
                break;
            }
            break;
        case 22:
        case 23:
            if (s == 0x16) {
                data_ov007_02103428 = 2;
                WORK->itemViews[0x17]->item->state->requestedState = 4;
            } else if (s == 0x17) {
                data_ov007_02103428 = 3;
                WORK->itemViews[0x16]->item->state->requestedState = 4;
            }
            func_ov007_020b7a44();
            next = 4;
            break;
        }
        st->state->requestedState = next;
    }

    view->sprite->scaleY = result;
    view->sprite->scaleX = view->sprite->scaleY;
    view->item->posX = posX;
}

/* The item's state-0 step: items 8, 0x14 and 0x17 are selected with the
 * B button (pressed this frame, not held) through func_ov007_020b45b0. */
// @symbol func_ov007_020b5068
void func_ov007_020b5068(void *t) {
    Ov007ItemView *view = (Ov007ItemView *)t;
    u16 id = view->item->id;
    Ov007Pad *pad;

    if (id != 8 && id != 0x14 && id != 0x17)
        return;

    pad = WORK->pad;
    if (!(pad->keys & 2))
        return;
    if (pad->prevKeys & 2)
        return;
    if (func_ov007_020c1da0(0))
        return;
    func_ov007_020b45b0(t);
}

// @symbol func_ov007_020b4c04
void *func_ov007_020b4c04(int arg0) {
    Ov007ItemView *view;
    Ov007Sprite *sprite;
    s32 r5;
    u8 t;

    view = (Ov007ItemView *)func_ov007_020c3df4(0, 0x1c);
    r5 = *(s32 *)((char *)data_ov007_020d77d8 + arg0 * 0xc);
    view->item = (Ov007Item *)func_ov007_020aea6c(arg0, &data_ov007_020d76e8[arg0],
                                                  &data_ov007_020d78f8[r5]);
    view->sprite = (Ov007Sprite *)func_ov007_020c44e4(data_ov007_020d77e0[view->item->id * 0xc]);
    sprite = view->sprite;
    if (arg0 == 0x15) {
        sprite->polygonId = 0x1c;
    } else {
        sprite->polygonId = 0x1b;
    }
    sprite->texRepeat = 3;
    sprite->texFlip = 0;
    sprite->texColor0 = 1;
    sprite->width =
        data_ov007_020d76a0[*(s32 *)((char *)data_ov007_020d77d8 + view->item->id * 0xc) * 2];
    sprite->height =
        data_ov007_020d76a1[*(s32 *)((char *)data_ov007_020d77d8 + view->item->id * 0xc) * 2];
    sprite->depth = data_ov007_020d7648[data_ov007_020d77e1[view->item->id * 0xc]];
    view->pad_08 = 0;
    view->slideTween = (struct Ov007Tween *)func_ov007_020c908c(&view->slideT);
    view->slideT = 0;
    view->slideToX = view->item->layout->homeX;
    view->slideFromX = view->slideToX;
    view->slideToY = view->item->layout->homeY;
    view->slideFromY = view->slideToY;

    if (arg0 >= 0 && arg0 <= 2) {
        sprite->frames[0] = WORK->textures[6];
        sprite->texS = data_ov007_020d75e8[arg0];
        sprite->texT = data_ov007_020d75f8[arg0];
        if (WORK->settings->language == 5) {
            view->item->hitBox = &data_ov007_020d76c0[r5 - 1];
            view->item->layout = &data_ov007_020d7680[arg0];
            if (arg0 != 0) {
                sprite->width = 0x48;
            } else {
                sprite->depth += 0xc;
            }
        }
    } else if (arg0 >= 3 && arg0 <= 6) {
        sprite->frames[0] = WORK->textures[1];
        sprite->texS = data_ov007_020d7608[arg0 - 3];
        sprite->texT = data_ov007_020d7600[arg0 - 3];
    } else if (arg0 >= 7 && arg0 <= 0xd) {
        if (arg0 == 7) {
            sprite->frames[0] = WORK->textures[6];
        } else {
            sprite->frames[0] = WORK->textures[8];
        }
        switch (arg0) {
        case 7:
            sprite->texS = 0x60;
            break;
        case 8:
            sprite->texS = 0xc0;
            sprite->texT = 0x20;
            t = WORK->settings->language;
            switch (t) {
            case 3:
                sprite->frames[0] = WORK->textures[7];
                sprite->texS = 0;
                sprite->texT = 0;
                sprite->width = 0x50;
                view->item->hitBox = &data_ov007_020d7658;
                break;
            case 4:
                sprite->frames[0] = WORK->textures[7];
                sprite->texS = 0x50;
                sprite->texT = 0;
                sprite->width = 0x60;
                view->item->hitBox = &data_ov007_020d766c;
                break;
            }
            break;
        case 9:
            break;
        case 10:
            sprite->texS = 0x90;
            sprite->texT = 0x20;
            break;
        case 11:
            sprite->texS = 0x90;
            sprite->texT = 0x30;
            break;
        default:
            sprite->texT = 0x20;
            break;
        }
    } else if (arg0 >= 0xe && arg0 <= 0x15) {
        sprite->frames[0] = WORK->textures[2];
        sprite->texS = data_ov007_020d7628[arg0 - 0xe];
        sprite->texT = data_ov007_020d7638[arg0 - 0xe];
    } else if (arg0 >= 0x16 && arg0 <= 0x17) {
        sprite->frames[0] = WORK->textures[2];
        sprite->texS = (u16)((arg0 - 0x16) * 0x48);
        sprite->texT = 0x80;
    }
    return view;
}

/* Frees a view built by func_ov007_020b4c04: its slide tween, its item,
 * its sprite, then the view itself. */
// @symbol func_ov007_020b4bd4
void func_ov007_020b4bd4(Ov007ItemView *view) {
    func_ov007_020c9080((int)view->slideTween);
    func_ov007_020aea4c((char *)view->item);
    func_ov007_020c44c4((char *)view->sprite);
    func_ov007_020c3d1c(view);
}

/* Resets the view's sprite to unit scale, full alpha and white, sizes it
 * for camera `arg` (func_ov007_020c43bc), puts it at the item's home point
 * and places it in the world (func_ov007_020c4388). c keeps the char *
 * spelling of the definition; `view` is the typed view. */
// @symbol func_ov007_020b4b5c
void func_ov007_020b4b5c(char *c, int arg) {
    Ov007ItemView *view = (Ov007ItemView *)c;
    Ov007Sprite *o = view->sprite;
    o->scaleY = 0x1000;
    o->scaleX = o->scaleY;
    func_ov007_020c43bc(o, (void *)arg);
    o->alpha = 0x1f000;
    o->color = 0x7fff;
    o->screenX = view->item->layout->homeX << 0xc;
    o->screenY = view->item->layout->homeY << 0xc;
    func_ov007_020c4388((char *)o, arg);
}

/* The ROM contains this block twice, verbatim, once per path of
 * func_ov007_020b46b0, and once more as func_ov007_020b45b0. `inline` is
 * required, not stylistic: plain `static` is not inlined at -O4 and emits a
 * separate symbol, which breaks the size. */
static inline int elem_selectable(Ov007ItemView *elem) {
    Ov007Item *obj = elem->item;
    int sel = 0;
    int gate2 = 0;
    int gate1 = 0;
    if (obj->state->currentState == 0) {
        if (obj->fade >= 0x1000 || (data_ov007_020d77dc[elem->item->id].flags & 2))
            gate1 = 1;
    }
    if (gate1) {
        if (!(data_ov007_020d77dc[elem->item->id].flags & 1))
            gate2 = 1;
    }
    if (gate2) {
        int ok = 1;
        if (func_ov007_020b79e4()) {
            int sub = 0;
            if (func_ov007_020b79e4()) {
                u16 id2 = elem->item->id;
                /* NOT a plain `if (id2 == 0x16 || id2 == 0x17) sub = 1;`. That
                 * compiles four bytes short. */
                int x = ok;
                if (id2 != 0x16 && id2 != 0x17)
                    x = sub;
                if (x)
                    sub = 1;
            }
            if (!sub)
                ok = 0;
        }
        if (ok)
            sel = 1;
    }
    return sel;
}

/* Title/menu per-element stylus hit test.
 *
 * func_ov007_020aed98 calls this once per element per frame, for all 24
 * elements: func_ov007_020b46b0(work->itemViews[i], work->touch). arg0 is
 * the element (an Ov007ItemView), arg1 is the Ov007Touch record that
 * func_ov007_020c1db0 maintains. A hit acts by calling
 * func_ov007_020b63e4, which parks the chosen item's id in chosenItemId.
 *
 * Two independent paths:
 *
 *   1. PRESS. Gated on down-now && !down-last-frame, i.e. the frame the
 *      stylus first touches, plus releasedFrames >= 1. Tests the stylus
 *      point against the item's hitBox, offset by the sprite's 20.12 screen
 *      position >> 12. A hit selects immediately. If that misses, three
 *      element types get a second chance against a hardcoded screen rect:
 *      x 8..0x66, y 0x50..0x60 for type 0xc (when bootArgs[9] is 0) or
 *      type 0xd (when it is not), and x 8..0x48, y 0x20..0x30 for type 9.
 *
 *   2. HOLD. Only while states[0] is in state 3, and only for element types
 *      3 and 6. Requires the stylus merely to be down, not freshly pressed,
 *      and counts frames in the item state's stateWork; past 0x3a it
 *      selects. Any frame the element is not held resets that counter to 0.
 *
 * Two things about the box shape below are load-bearing, not style. Each
 * axis is shifted once into its position local rather than at each of the
 * four edges, and the object and rect pointers are re-dereferenced in every
 * edge expression rather than bound to locals. Neither alone is enough.
 * Measured at 2004/b56 over all four combinations: bound pointers with
 * per-edge shifts scores 37 divergences, re-dereferencing with per-edge
 * shifts also 37, bound pointers with pre-shifted positions 51, and only the
 * two together 0. Binding each pointer on its own: binding the rect costs 49
 * divergences, binding the object costs 14, and binding the transform is
 * FREE at 0; the transform is written out longhand only to keep the four
 * edges reading alike.
 */
// @symbol func_ov007_020b46b0
void func_ov007_020b46b0(void *arg0, void *arg1) {
    Ov007ItemView *elem = (Ov007ItemView *)arg0;
    Ov007Touch *tp = (Ov007Touch *)arg1;
    int held = 0;

    {
        if (elem_selectable(elem) && func_ov007_020c1da0(0) == 0) {
            int armed = 0;
            int fresh = 0;
            int touching = tp->cur.touch;
            if (touching != 0) {
                if (tp->prev.touch == 0)
                    fresh = 1;
            }
            if (fresh) {
                if (tp->releasedFrames >= 1U)
                    armed = 1;
            }
            if (armed) {
                int pos_y = elem->sprite->screenY >> 12;
                int pos_x = elem->sprite->screenX >> 12;
                int max_y = elem->item->hitBox->maxY + pos_y;
                int min_y = elem->item->hitBox->minY + pos_y;
                int max_x = elem->item->hitBox->maxX + pos_x;
                int min_x = elem->item->hitBox->minX + pos_x;
                if (touching != 0 && (s32)tp->cur.x >= min_x && (s32)tp->cur.x <= max_x &&
                    (s32)tp->cur.y >= min_y && (s32)tp->cur.y <= max_y) {
                    func_ov007_020b63e4((char **)arg0);
                } else {
                    u16 t = elem->item->id;
                    if ((t == 0xc && WORK->settings->bootArgs[9] == 0) ||
                        (t == 0xd && WORK->settings->bootArgs[9] != 0)) {
                        if (touching != 0) {
                            s32 x = tp->cur.x;
                            if (x >= 8 && x <= 0x66) {
                                s32 y = tp->cur.y;
                                if (y >= 0x50 && y <= 0x60)
                                    func_ov007_020b63e4((char **)arg0);
                            }
                        }
                    } else if (t == 9) {
                        if (touching != 0) {
                            s32 x = tp->cur.x;
                            if (x >= 8 && x <= 0x48) {
                                s32 y = tp->cur.y;
                                if (y >= 0x20 && y <= 0x30)
                                    func_ov007_020b63e4((char **)arg0);
                            }
                        }
                    }
                }
                goto done; /* kept: an !armed test on HOLD (0x4a0) or an inline press helper re-lays it */
            }
        }
    }

    if (WORK->states[0]->currentState == 3) {
        /* Through arg0, not elem: reading it through elem lets the compiler
         * keep the item in a register for elem_selectable and drops 12 bytes. */
        u16 t0 = ((Ov007ItemView *)arg0)->item->id;
        if (t0 == 3 || t0 == 6) {
            if (elem_selectable(elem) && func_ov007_020c1da0(0) == 0 && tp->cur.touch != 0 &&
                *(s16 *)func_ov007_020b8f78() == 5 && func_ov007_020b8fa0() == 0) {
                int pos_y = elem->sprite->screenY >> 12;
                int pos_x = elem->sprite->screenX >> 12;
                int max_y = elem->item->hitBox->maxY + pos_y;
                int min_y = elem->item->hitBox->minY + pos_y;
                int max_x = elem->item->hitBox->maxX + pos_x;
                int min_x = elem->item->hitBox->minX + pos_x;
                if (tp->cur.touch != 0 && (s32)tp->cur.x >= min_x && (s32)tp->cur.x <= max_x &&
                    (s32)tp->cur.y >= min_y && (s32)tp->cur.y <= max_y) {
                    Ov007MenuState *state = elem->item->state;
                    held = 1;
                    state->stateWork += 1;
                    /* Re-dereferences the element rather than reusing `state`
                     * just above it: reusing the bound local compiles eight bytes
                     * short. */
                    if (elem->item->state->stateWork > 0x3a)
                        func_ov007_020b63e4((char **)arg0);
                }
            }
        }
    }

done:
    if (held == 0)
        elem->item->state->stateWork = 0;
    {
        Ov007MenuState *st = elem->item->state;
        if (st->currentState != 2)
            return;
        if (st->requestedState == -1)
            return;
    }
    func_ov007_020c1d78(0);
}

/* Selects the view's item (func_ov007_020b63e4) if it is selectable: the
 * same test as elem_selectable, written out. t keeps the void * spelling of
 * the callers' declarations; `view` is the typed view. */
// @symbol func_ov007_020b45b0
void func_ov007_020b45b0(void *t) {
    Ov007ItemView *view = (Ov007ItemView *)t;
    Ov007Item *p = view->item;
    int call_it = 0;
    int t0 = 0;
    int b6 = 0;

    if (p->state->currentState == 0) {
        if (p->fade >= 0x1000 || (data_ov007_020d77dc[view->item->id].flags & 2))
            b6 = 1;
    }
    if (b6) {
        if (!(data_ov007_020d77dc[view->item->id].flags & 1))
            t0 = 1;
    }
    if (t0) {
        int ok = 1;
        if (func_ov007_020b79e4()) {
            int special = 0;
            if (func_ov007_020b79e4()) {
                unsigned short id2 = view->item->id;
                int x = ok;
                if (id2 != 0x16 && id2 != 0x17)
                    x = special;
                if (x)
                    special = 1;
            }
            if (!special)
                ok = 0;
        }
        if (ok)
            call_it = 1;
    }
    if (!call_it)
        return;
    func_ov007_020b63e4((char **)t);
}

/* Runs one item view's frame unless its layout kind is 0xa or its state
 * work is 0xff: requests func_ov007_020b58e4's state, ticks the item, runs
 * the state-0 or state-2 handler, then func_ov007_020b5bf0 once the item is
 * ready. */
// @symbol func_ov007_020b44ec
void func_ov007_020b44ec(void *self) {
    Ov007Item *o = ((Ov007ItemView *)self)->item;
    Ov007ItemLayout *p24 = o->layout;
    if (p24->kind == 0xa)
        return;
    Ov007MenuState *menu = o->state;
    if (menu->stateWork == 0xff)
        return;
    short r = (short)func_ov007_020b58e4((char *)self);
    menu = o->state;
    menu->requestedState = r;
    func_ov007_020ae834((char *)o);
    menu = o->state;
    switch (menu->currentState) {
    case 0:
        func_ov007_020b5068(self);
        break;
    case 2:
        func_ov007_020b50e8((char *)self);
        break;
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;
    }
    if (o->ready == 0)
        return;
    func_ov007_020b5bf0((Ov007ItemView *)self);
}

/* Keeps the view's sprite on its item: when the item's position moved
 * since the last frame the sprite is re-placed for camera `arg`
 * (func_ov007_020c4388); while the item is ready the sprite is drawn
 * (func_ov007_020c421c). */
// @symbol func_ov007_020b4464
void func_ov007_020b4464(Ov007ItemView *view, int arg) {
    int changed;
    if (view->item->ready == 0) {
        return;
    }
    changed = 0;
    if (view->sprite->screenX != view->item->posX || view->sprite->screenY != view->item->posY) {
        changed = 1;
    }
    view->sprite->screenX = view->item->posX;
    view->sprite->screenY = view->item->posY;
    if (changed) {
        func_ov007_020c4388((char *)view->sprite, arg);
    }
    func_ov007_020c421c((char *)view->sprite);
}

/* The choice word data_ov007_02103428, or 0 while func_ov007_020b79e4
 * returns nonzero. */
// @symbol func_ov007_020b4438
int func_ov007_020b4438(void) {
    if (func_ov007_020b79e4())
        return 0;
    return data_ov007_02103428;
}

/* Opens the 0x16 / 0x17 choice: sets the choice word to 1, requests state 3
 * on both items' views, then hands `self` (9 or 0xc at the callers) to
 * func_ov007_020b7d94. */
// @symbol func_ov007_020b43dc
void func_ov007_020b43dc(void *self) {
    Ov007Work *work = WORK;
    data_ov007_02103428 = 1;
    Ov007ItemView *view1 = work->itemViews[0x16];
    Ov007Item *item1 = view1->item;
    Ov007MenuState *state1 = item1->state;
    state1->requestedState = 3;
    Ov007Work *work2 = WORK;
    Ov007ItemView *view2 = work2->itemViews[0x17];
    Ov007Item *item2 = view2->item;
    Ov007MenuState *state2 = item2->state;
    state2->requestedState = 3;
    func_ov007_020b7d94(self);
}

/* Builds number sprite `index` (Ov007Work's numberSprites): a frame of
 * texture 2 at the row's screen position, texel cell and size, parented to
 * item view 14's sprite for camera 1. */
// @symbol func_ov007_020b42d8
Ov007Sprite *func_ov007_020b42d8(int index) {
    Ov007Sprite *parent = WORK->itemViews[14]->sprite;
    Ov007Sprite *obj;
    WORK->numberSprites[index] = (Ov007Sprite *)func_ov007_020c44e4(1);
    obj = WORK->numberSprites[index];
    obj->screenX = *(s16 *)((char *)data_ov007_020d761c + (index << 2)) << 12;
    obj->screenY = *(s16 *)((char *)data_ov007_020d761e + (index << 2)) << 12;
    obj->depth = 0x14;
    obj->width = data_ov007_020d75f0[index * 2];
    obj->height = data_ov007_020d75f1[index * 2];
    obj->frames[0] = WORK->textures[2];
    obj->texRepeat = 3;
    obj->texFlip = 0;
    obj->texColor0 = 1;
    obj->texS = data_ov007_020d75e0[index];
    obj->texT = data_ov007_020d75e4[index];
    func_ov007_020c41dc((char *)parent, (char *)obj, (int)WORK->cameras[1]);
    return obj;
}

/* Draws `number` (three digits, func_ov007_020c3598) on top of the view's
 * sprite: numberSprites[2] is stepped across the digits, each placed by its
 * width from data_ov007_020d7610 and given the digit's texel cell, and
 * numberSprites[0] is drawn at the start of the run; a negative number draws
 * numberSprites[1] instead. Bracketed by a matrix push and pop. */
// @symbol func_ov007_020b413c
void func_ov007_020b413c(Ov007ItemView *view, int number) {
    Ov007Sprite *lead;
    Ov007Sprite *digit;
    s16 baseS;
    int width;
    int i;
    int x;
    char digits[4];

    *(volatile int *)0x4000444 = 0; /* MTX_PUSH */
    func_ov007_020c42f8((char *)view->sprite);
    if (number >= 0) {
        char *d;
        int j;
        int leadX;
        unsigned char *widths;
        lead = WORK->numberSprites[0];
        digit = WORK->numberSprites[2];
        x = lead->screenX;
        baseS = digit->texS;
        width = 0;
        func_ov007_020c3598(number, 3, digits, 0xa);
        for (i = 0; i < 3; i++) {
            if (digits[i] >= 0)
                width += data_ov007_020d7610[digits[i]];
        }
        /* through a local: indexing data_ov007_020d7610 directly in the
         * second loop re-lays it */
        widths = data_ov007_020d7610;
        x = x + ((-width / 2) << 12);
        digit->screenX = x + 0xa000;
        d = &digits[2];
        for (j = 2; j >= 0; j--) {
            if (*d >= 0) {
                digit->screenX += ((unsigned)widths[*d] >> 1) << 12;
                digit->texS = (s16)(*d * digit->width + baseS);
                func_ov007_020c4128((char *)view->sprite, (char *)digit, (int)WORK->cameras[1]);
                digit->screenX += (widths[*d] - ((unsigned)widths[*d] >> 1)) << 12;
            }
            d--;
        }
        digit->texS = baseS;
        leadX = lead->screenX;
        lead->screenX = x;
        func_ov007_020c4128((char *)view->sprite, (char *)lead, (int)WORK->cameras[1]);
        lead->screenX = leadX;
    } else {
        func_ov007_020c4128((char *)view->sprite, (char *)WORK->numberSprites[1],
                            (int)WORK->cameras[1]);
    }
    *(volatile int *)0x4000448 = 1; /* MTX_POP */
}
}
