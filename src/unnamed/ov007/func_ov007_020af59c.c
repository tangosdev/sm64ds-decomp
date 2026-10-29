#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern char *data_ov007_0210342c;
/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
extern void func_ov007_020b43dc(void *self);
extern int func_ov007_020b79e4(void);
extern int func_ov007_020b4438(void);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020b79c8(void);
extern void func_ov007_020aec00(void);
extern void Crash(void);

typedef int (*pfn)(int);

void func_ov007_020af59c(void)
{
    Ov007Work *w = WORK;
    Ov007MenuState *menu = w->states[1];
    int state = menu->currentState;
    int r2var = menu->stateTicks;
    unsigned char *base = *(unsigned char **)w->settings + 0xb;

    switch (state) {
    case 9:
        if (r2var == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            WORK->states[2]->requestedState = 0xd;
        }
        switch (menu->stateWork) {
        case 0: {
            int idx = WORK->selectedSlot;
            if (idx == -1)
                return;
            if (base[idx] == 0) {
                func_ov007_020b7d94(3);
                WORK->selectedSlot = -1;
                menu->stateWork = 1;
            } else {
                func_ov007_020b43dc((void *)0xc);
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
        if (r2var == 0) {
            func_ov007_020b7d94(0xa);
            func_ov007_020c1d8c(0);
        }
        switch (menu->stateWork) {
        case 0: {
            int r;
            if (func_ov007_020b79c8() == 0)
                return;
            r = (*(pfn *)(data_ov007_0210342c + 0x2c))(WORK->selectedSlot + 8);
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
                Crash();
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
