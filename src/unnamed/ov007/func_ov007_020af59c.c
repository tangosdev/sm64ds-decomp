#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "nitro/os/system.h"

extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
extern void func_ov007_020b43dc(int kind);
extern int func_ov007_020b79e4(void);
extern int func_ov007_020b4438(void);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020b79c8(void);
extern void func_ov007_020aec00(void);

/* states[1]'s erase states (func_ov007_020af390): 9 waits for a slot to
 * be picked (an unused one opens message 3, a used one opens the two-way
 * choice with message 0xc, func_ov007_020b43dc; answer 2 goes on, 3
 * cancels), 10 erases it through the owner's command 8 + slot, re-sums the
 * star totals (func_ov007_020aec00) and shows the outcome. */
void func_ov007_020af59c(void) {
    Ov007Work *w = WORK;
    Ov007MenuState *menu = w->states[1];
    int state = menu->currentState;
    int ticks = menu->stateTicks;
    unsigned char *fileUsed = w->settings->bootArgs->fileUsed;

    switch (state) {
    case 9:
        if (ticks == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            WORK->states[2]->requestedState = 0xd;
        }
        switch (menu->stateWork) {
        case 0: {
            int idx = WORK->selectedSlot;
            if (idx == -1)
                return;
            if (fileUsed[idx] == 0) {
                func_ov007_020b7d94(3);
                WORK->selectedSlot = -1;
                menu->stateWork = 1;
            } else {
                func_ov007_020b43dc(0xc);
                menu->stateWork = 2;
            }
            return;
        }
        case 1:
            if (func_ov007_020b79e4() == 0)
                menu->stateWork = 0;
            return;
        case 2:
            if (func_ov007_020b4438() == 2) {
                menu->requestedState = 0xa;
                func_ov007_020c1d8c(0);
                return;
            }
            if (func_ov007_020b4438() != 3)
                return;
            WORK->selectedSlot = -1;
            menu->stateWork = 0;
            return;
        }
        return;

    case 10:
        if (ticks == 0) {
            func_ov007_020b7d94(0xa);
            func_ov007_020c1d8c(0);
        }
        switch (menu->stateWork) {
        case 0: {
            int r;
            if (func_ov007_020b79c8() == 0)
                return;
            /* ids 8..10: the owner erases save file 0..2 (DSMT_MenuCallback) */
            r = WORK->callback(WORK->selectedSlot + 8);
            WORK->selectedSlot = -1;
            switch (r) {
            case 0:
                func_ov007_020aec00();
                menu->stateWork = 1;
                return;
            case 1:
                menu->stateWork = 2;
                return;
            case 2:
                _Z12OS_Terminatev();
                return;
            }
            return;
        }
        case 1:
        case 2:
            if (func_ov007_020b79e4() != 0)
                return;
            func_ov007_020b7d94(0xb);
            menu->stateWork = 3;
            return;
        case 3:
            if (func_ov007_020b79e4() == 0) {
                menu->requestedState = 0;
                menu->requestedState = 9;
            }
            return;
        }
        return;
    }
}
