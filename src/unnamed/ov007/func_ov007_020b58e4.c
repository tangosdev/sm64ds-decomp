#include "types.h"
#include "ds_width.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"
#include "Ov007Item.h"

#ifndef SM64DS_HOST_64
typedef char Ov007Work_transitionMode_must_be_at_0x20[DS_IADDR(&((Ov007Work *)0)->transitionMode) == 0x20
                                                          ? 1
                                                          : -1];
#endif

/* Load-bearing: reading view->item in place instead of through this inline
 * costs 19 mismatches at 2004/b56. */
inline Ov007Item *view_item(Ov007ItemView *view) {
    return view->item;
}

/* The state the item view asks its item for. It is picked from the menu's
 * main state (states[2]) on that state's first tick (and on its second, in
 * state 9), the item's layout kind and its id. Otherwise the item keeps
 * its requested state. */
int func_ov007_020b58e4(Ov007ItemView *view) {
    Ov007Work *g = WORK;
    Ov007MenuState *b = WORK->states[2];
    int flag = b->stateTicks;
    Ov007Item *a = view_item(view);
    int sel = b->currentState;
    Ov007ItemLayout *r3ptr = a->layout;
    int r7 = flag == 0;
    Ov007MenuState *c = a->state;
    int lr_v = r3ptr->kind;
    int ip_v = a->id;
    int ret = c->requestedState;
    switch (sel) {
    case 0:
        if (r7) {
            ret = 1;
        }
        break;

    case 1:
        if (r7) {
            int d = b->previousState;
            if (d == 0xe) {
                if (lr_v == 0) {
                    ret = 3;
                } else if (lr_v == 7) {
                    ret = 4;
                } else if (lr_v == 6) {
                    ret = 5;
                }
            } else if ((lr_v == 0) || (lr_v == 6)) {
                ret = 0;
            } else {
                ret = 1;
            }
        }
        break;

    case 3:
        if (r7) {
            if (lr_v == 0) {
                ret = 3;
            } else if (lr_v == 6) {
                a->fade = 0;
                ret = 5;
            } else if (lr_v == 7) {
                ret = 4;
            }
        }
        break;

    case 2:
        if (r7) {
            if ((lr_v == 0) || (lr_v == 6)) {
                ret = 4;
            }
        }
        break;

    case 7:
        if (r7) {
            if (lr_v == 2) {
                int e = WORK->transitionMode;
                if (((e == 1) && (ip_v == 4)) || ((e == 0) && ((ip_v == 3) || (ip_v == 5)))) {
                    ret = 3;
                }
                if (ip_v == 6) {
                    int f = g->nextFigureIndex;
                    ret = 3;
                    if (f == 1) {
                        view->sprite->texS = 0;
                    } else {
                        view->sprite->texS = 0x28;
                    }
                }
            }
        }
        break;

    case 4:
        if (r7) {
            if (ip_v == 3) {
                ret = 0;
            } else if (ip_v == 5) {
                ret = 3;
            }
        }
        break;

    case 5:
        if (r7) {
            if (ip_v == 4) {
                ret = 0;
            } else if (ip_v == 5) {
                ret = 4;
            }
        }
        break;

    case 6:
        if (r7) {
            if (lr_v == 2) {
                ret = 4;
            }
        }
        break;

    case 14:
        if (r7) {
            if (lr_v == 0) {
                ret = 4;
            } else if (lr_v == 7) {
                ret = 3;
            }
        }
        break;

    case 8:
        if (r7) {
            int t = WORK->states[1]->previousState;
            if (t < 0) {
                if (lr_v == 3) {
                    ret = 0;
                } else {
                    ret = 1;
                }
            } else if (lr_v == 3) {
                ret = (ip_v != 0x15) ? (5) : (3);
            } else if (lr_v == 4) {
                ret = 4;
            }
        }
        break;

    case 9:
        if (r7) {
            if (((u32)(ip_v - 0xe)) <= 2) {
                ret = 6;
            } else if (ip_v == 0x15) {
                ret = 4;
            }
        } else if (flag == 1) {
            if (lr_v == 4) {
                int t = b->previousState;
                if (t == 8) {
                    ret = 3;
                } else {
                    a->fade = 0;
                    ret = 5;
                }
            }
        }
        break;

    case 10:

    case 12:

    case 13:
        if (r7) {
            if (lr_v == 4) {
                ret = 4;
            } else if (((u32)(ip_v - 0xe)) <= 2) {
                ret = 5;
            }
        }
        break;

    case 11:
        if (r7) {
            if (((u32)(ip_v - 0xe)) <= 2) {
                ret = 6;
            }
        }
        break;
    }

    return ret;
}
