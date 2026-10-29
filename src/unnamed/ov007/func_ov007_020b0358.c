#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020bca40(int i);
extern void func_ov007_020bcba8(int i);
extern void func_ov007_020b283c(void);
extern void func_ov007_020b76a0(void);
extern void func_ov007_020bd1c0(int i);
extern void *func_ov007_020b8f78(void);
extern char *data_ov007_0210342c;

typedef char Ov007Work_transitionMode_must_be_at_0x20[(int)&((Ov007Work *)0)->transitionMode == 0x20
                                                          ? 1
                                                          : -1];

/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

void func_ov007_020b0358(void)
{
    Ov007Work *b = WORK;
    Ov007MenuState *s2 = b->states[1];
    int state = s2->currentState;
    int r5 = s2->stateTicks;

    switch (state) {
    case 0:
        if (r5 == 0) {
            b->pendingCapture = 1;
            func_ov007_020c1d8c(0);
            return;
        }
        if (r5 != 1)
            return;
        b->pendingBgMode = 3;
        func_ov007_020bca40(1);
        WORK->states[1]->requestedState = 1;
        return;
    case 1:
        if (r5 == 0) {
            func_ov007_020bcba8(2);
            return;
        }
        if (r5 != 1)
            return;
        b->pendingBgMode = 4;
        func_ov007_020bca40(2);
        WORK->states[1]->requestedState = 2;
        return;
    case 2: {
        int hit = 0;
        int st2;
        if (r5 == 0) {
            func_ov007_020b283c();
            func_ov007_020b76a0();
            func_ov007_020bd1c0(1);
        }
        if (r5 == 0x28) {
            WORK->states[2]->requestedState = 7;
        }
        st2 = WORK->transitionMode;
        switch (st2) {
        case 1:
            if (*(short*)func_ov007_020b8f78() == 8) {
                WORK->states[0]->requestedState = 2;
                hit = 1;
            }
            break;
        case 0:
            if (*(short*)func_ov007_020b8f78() == 5) {
                WORK->states[0]->requestedState = 3;
                hit = 1;
            }
            break;
        }
        if (hit) {
            WORK->pendingBgMode = 5;
        }
        return;
    }
    }
}
