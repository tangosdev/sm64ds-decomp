#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Pad.h"
#include "nitro/os/system.h"

extern int data_ov007_020ccc20[];

#define func_ov007_020c1d78 _Z19func_ov007_020c1d78i
#define func_ov007_020b7d94 _Z19func_ov007_020b7d94i
#define func_ov007_020b43dc _Z19func_ov007_020b43dci
#define func_ov007_020b79e4 _Z19func_ov007_020b79e4v
#define func_ov007_020c1d8c _Z19func_ov007_020c1d8ci
#define func_ov007_020b79c8 _Z19func_ov007_020b79c8v
#define func_ov007_020bdeb0 _Z19func_ov007_020bdeb0i
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
extern void func_ov007_020b43dc(int kind);
extern int func_ov007_020b79e4(void);
#define func_ov007_020b4438 _Z19func_ov007_020b4438v
extern int func_ov007_020b4438(void);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020b79c8(void);
extern void func_ov007_020aec00(void);
extern int func_ov007_020bdeb0(int idx);
extern void MessageWindow_Close(void);

void func_ov007_020af84c(void)
{
    Ov007Work *b = WORK;
    Ov007MenuState *s2 = b->states[1];
    int state = s2->currentState;
    int r2var = s2->stateTicks;
    unsigned char *base = b->settings->bootArgs->fileUsed;

    switch (state) {
    case 7:
        if (r2var == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            WORK->states[2]->requestedState = 0xc;
        }
        switch (s2->stateWork) {
        case 0: {
            int idx = WORK->selectedSlot;
            if (idx == -1)
                return;
            if (base[idx] != 0) {
                s2->stateWork = 1;
                return;
            }
            func_ov007_020b7d94(3);
            WORK->selectedSlot = -1;
            s2->stateWork = 2;
            return;
        }
        case 1: {
            int idx = WORK->selectedSlot;
            if (idx == -1) {
                s2->stateWork = 0;
                func_ov007_020c1d78(0);
                return;
            }
            {
                int idx2 = WORK->targetSlot;
                if (idx2 == -1)
                    return;
                if (base[idx2] == 0) {
                    s2->requestedState = 8;
                    return;
                }
                func_ov007_020b43dc(9);
                s2->stateWork = 3;
                return;
            }
        }
        case 2:
            if (func_ov007_020b79e4() == 0)
                s2->stateWork = 0;
            return;
        case 3:
            if (func_ov007_020b4438() == 2) {
                s2->requestedState = 8;
                func_ov007_020c1d8c(0);
                return;
            }
            if (func_ov007_020b4438() != 3)
                return;
            WORK->targetSlot = -1;
            s2->stateWork = 0;
            return;
        }
        break;

    case 8:
        if (r2var == 0) {
            func_ov007_020c1d8c(0);
            func_ov007_020b7d94(4);
        }
        switch (s2->stateWork) {
        case 0: {
            int r;
            if (func_ov007_020b79c8() == 0)
                return;
            r = WORK->callback(
                ((int(*)[3])data_ov007_020ccc20)[WORK->selectedSlot][WORK->targetSlot]);
            func_ov007_020aec00();
            switch (r) {
            case 0:
                s2->stateWork = 1;
                return;
            case 1:
                s2->stateWork = 2;
                return;
            case 2:
                _Z12OS_Terminatev();
                return;
            }
            return;
        }
        case 1:
        case 2: {
            int bb;
            if (func_ov007_020b79e4() != 0)
                return;
            bb = (s2->stateWork == 1);
            func_ov007_020b7d94(bb ? 5 : WORK->targetSlot + 6);
            WORK->targetSlot = -1;
            WORK->selectedSlot = WORK->targetSlot;
            s2->stateWork = bb ? 4 : 3;
            return;
        }
        case 3:
            if (func_ov007_020b79c8() == 0)
                return;
            {
                Ov007Pad *p54 = WORK->pad;
                unsigned short aa = p54->prevKeys;
                unsigned short bx = p54->keys;
                if ((bx & ~aa) == 0) {
                    Ov007Touch *p50 = WORK->touch;
                    if (p50->cur.touch == 0)
                        return;
                    if (p50->prev.touch != 0)
                        return;
                    if (p50->releasedFrames < 1)
                        return;
                }
                func_ov007_020bdeb0(0xc);
                MessageWindow_Close();
                s2->stateWork = 4;
                return;
            }
        case 4:
            if (func_ov007_020b79e4() == 0)
                s2->requestedState = 7;
            return;
        }
        break;
    }
}
