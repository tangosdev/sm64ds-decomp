#include "types.h"
// @symbol func_ov007_020ae834
/* recovered: per-frame state step for a menu panel whose controller record
 * (0x10 bytes, at *(c + 4)) carries a state (+0) and a pending request (+2).
 * Before ticking the controller (func_ov007_020c92d0) it retires requests:
 * request 3 while idle or request 4 while in state 1 becomes -1, request 7
 * in state 1 becomes 3. After the tick, a finished controller (+0xc == 0)
 * clears the hold counter (+0x18) and the state drives the panel: 0 flags
 * ready (+0x20 = 1); 1 resets the two eased values (+0x10 = 0x1000, +0x14 =
 * 0) and re-arms through func_ov007_020ae810; 3/4/7 ease +0x14 toward the
 * end chosen by the state (3 opens) over 15 frames, feed its square to
 * func_ov007_020aeb34, and post request 0 when fully closed, or 1 (3 for
 * state 7) when fully open; 5/6 ease +0x10 over 20 frames the same way and
 * post request 0 at the far end. The hold counter counts down at the tail.
 *
 * Codegen notes: the request retirement is one OR-merged condition so both
 * arms share the -1 store; the switch covers 0..7 with an empty case 2; the
 * 3/4/7 body uses two sequential guarded ifs (flag && ..., !flag && ...) with
 * a break in the first, while the 5/6 body needs the two arms merged into
 * one OR so the request-0 store is emitted once; the function is void.
 */
extern void func_ov007_020c92d0(void *s);
extern int func_ov007_020aeaec(char *c, int flag, int lo, int hi);
extern void func_ov007_020ae810(char *c);
extern void func_ov007_020aeb34(char *c, int t);

struct Sub {
    short f0;
    short f2;
    int f4;
    int f8;
    int fc;
};

void func_ov007_020ae834(char *c)
{
    struct Sub *s = *(struct Sub **)(c + 4);
    int state;
    int done;
    int st;
    int flag;

    state = s->f0;
    if ((state == 0 && s->f2 == 3) || (state == 1 && s->f2 == 4)) {
        s->f2 = -1;
    } else if (state == 1 && s->f2 == 7) {
        s->f2 = 3;
    }

    func_ov007_020c92d0(s);
    done = s->fc;
    st = s->f0;
    if (done == 0)
        *(int *)(c + 0x18) = 0;

    switch (st) {
    case 0:
        if (done == 0)
            *(int *)(c + 0x20) = 1;
        break;
    case 1:
        if (done == 0) {
            *(int *)(c + 0x10) = 0x1000;
            *(int *)(c + 0x14) = 0;
            *(int *)(c + 0x20) = 0;
            func_ov007_020ae810(c);
        }
        break;
    case 2:
        break;
    case 3:
    case 4:
    case 7:
        flag = (st == 3) ? 1 : 0;
        if (done == 0)
            *(int *)(c + 0x20) = 1;
        *(int *)(c + 0x14) = func_ov007_020aeaec(c, flag, 0, 0xf);
        {
            int t = *(int *)(c + 0x14);
            func_ov007_020aeb34(c, (int)(((long long)t * t) >> 12));
        }
        if (flag && *(int *)(c + 0x14) == 0) {
            (*(struct Sub **)(c + 4))->f2 = 0;
            break;
        }
        if (!flag && *(int *)(c + 0x14) == 0x1000) {
            if (st == 7)
                (*(struct Sub **)(c + 4))->f2 = 3;
            else
                (*(struct Sub **)(c + 4))->f2 = 1;
        }
        break;
    case 5:
    case 6:
        flag = (st == 5) ? 1 : 0;
        if (done == 0)
            *(int *)(c + 0x20) = 1;
        if (*(int *)(c + 0x10) == 0)
            *(int *)(c + 0x18) = 0;
        if ((flag && *(int *)(c + 0x10) == 0x1000) || (!flag && *(int *)(c + 0x10) == 0)) {
            (*(struct Sub **)(c + 4))->f2 = 0;
            break;
        }
        *(int *)(c + 0x10) = func_ov007_020aeaec(c, (flag == 0) ? 1 : 0, 0, 0x14);
        break;
    }

    if (*(int *)(c + 0x18) > 0)
        *(int *)(c + 0x18) -= 1;
}
