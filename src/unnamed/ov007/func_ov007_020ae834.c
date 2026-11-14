#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Item.h"
/* Per-frame state step for a menu item. Before ticking the item's state
 * (func_ov007_020c92d0) it retires requests: request 3 while idle or request
 * 4 while in state 1 becomes -1, request 7 in state 1 becomes 3. On the frame
 * a state is entered (stateTicks == 0) the wiggle timer is cleared and the
 * state drives the item: 0 shows it; 1 hides it, resets fade to 0x1000 and
 * slideOut to 0 and re-seeds the position through func_ov007_020ae810; 3/4/7
 * show it and ease slideOut toward the end chosen by the state (3 opens) over
 * 15 frames, feed its square to func_ov007_020aeb34, and request 0 when fully
 * open (slideOut 0), or 1 (3 for state 7) when fully closed (slideOut
 * 0x1000); 5/6 show it and ease fade
 * over 20 frames the same way and request 0 at the far end. The wiggle timer
 * counts down at the tail.
 *
 * Codegen notes: the request retirement is one OR-merged condition so both
 * arms share the -1 store; the switch covers 0..7 with an empty case 2; the
 * 3/4/7 body uses two sequential guarded ifs (flag && ..., !flag && ...) with
 * a break in the first, while the 5/6 body needs the two arms merged into
 * one OR so the request-0 store is emitted once; the function is void.
 */

#define func_ov007_020c92d0 _Z19func_ov007_020c92d0P14Ov007MenuState
#define func_ov007_020aeaec _Z19func_ov007_020aeaecP9Ov007Itemiii
extern void func_ov007_020c92d0(Ov007MenuState *state);
extern int func_ov007_020aeaec(Ov007Item *item, int flag, int lo, int hi);
extern void func_ov007_020ae810(Ov007Item *item);
extern void func_ov007_020aeb34(Ov007Item *item, int t);

void func_ov007_020ae834(Ov007Item *item) {
    Ov007MenuState *s = item->state;
    int state;
    int ticks;
    int st;
    int flag;

    state = s->currentState;
    if ((state == 0 && s->requestedState == 3) || (state == 1 && s->requestedState == 4)) {
        s->requestedState = -1;
    } else if (state == 1 && s->requestedState == 7) {
        s->requestedState = 3;
    }

    func_ov007_020c92d0(s);
    ticks = s->stateTicks;
    st = s->currentState;
    if (ticks == 0)
        item->wiggleTimer = 0;

    switch (st) {
    case 0:
        if (ticks == 0)
            item->visible = 1;
        break;
    case 1:
        if (ticks == 0) {
            item->fade = 0x1000;
            item->slideOut = 0;
            item->visible = 0;
            func_ov007_020ae810(item);
        }
        break;
    case 2:
        break;
    case 3:
    case 4:
    case 7:
        flag = (st == 3) ? 1 : 0;
        if (ticks == 0)
            item->visible = 1;
        item->slideOut = func_ov007_020aeaec(item, flag, 0, 0xf);
        {
            int t = item->slideOut;
            func_ov007_020aeb34(item, (int)(((long long)t * t) >> 12));
        }
        if (flag && item->slideOut == 0) {
            item->state->requestedState = 0;
            break;
        }
        if (!flag && item->slideOut == 0x1000) {
            if (st == 7)
                item->state->requestedState = 3;
            else
                item->state->requestedState = 1;
        }
        break;
    case 5:
    case 6:
        flag = (st == 5) ? 1 : 0;
        if (ticks == 0)
            item->visible = 1;
        if (item->fade == 0)
            item->wiggleTimer = 0;
        if ((flag && item->fade == 0x1000) || (!flag && item->fade == 0)) {
            item->state->requestedState = 0;
            break;
        }
        item->fade = func_ov007_020aeaec(item, (flag == 0) ? 1 : 0, 0, 0x14);
        break;
    }

    if (item->wiggleTimer > 0)
        item->wiggleTimer -= 1;
}
