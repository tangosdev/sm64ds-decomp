#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007LayerSlot.h"
#define MenuText_ScrollRecordBoard _Z26MenuText_ScrollRecordBoardi
#define MenuText_DrawRecordBoard _Z24MenuText_DrawRecordBoardiPci
#define RequestScreenFade _Z17RequestScreenFadeiiPFvvE

extern int func_ov007_020c8f98(void *p);
#define func_ov007_020c1d78 _Z19func_ov007_020c1d78i
#define func_ov007_020b7d94 _Z19func_ov007_020b7d94i
#define func_ov007_020c1d8c _Z19func_ov007_020c1d8ci
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
#define func_ov007_020b79e4 _Z19func_ov007_020b79e4v
extern int func_ov007_020b79e4(void);
extern void func_ov007_020c1d8c(int i);
extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void MenuText_DrawRecordBoard(int slot, void *records, int alt);
extern void func_ov007_020c0638(struct Ov007BgLayer *layer, int x, int y, u16 tile);
extern void MenuText_ScrollRecordBoard(int n);

/* states[1]'s save-file states 2..6 (func_ov007_020af390): 3 waits for a
 * slot to be picked (an unused one opens message 3), 4 draws the picked
 * file's record board (MenuText_DrawRecordBoard) and slides it in, 2 waits for
 * another slot, 5 slides over to it, 6 slides the board out. Tween 0 drives
 * the scroll of layers 6 and 7, tween 1 the board's (MenuText_ScrollRecordBoard). */
void func_ov007_020afc44(void) {
    Ov007MenuState *menu = WORK->states[1];
    unsigned char *fileUsed = WORK->settings->bootArgs->fileUsed;
    int ticks = menu->stateTicks;
    Ov007Tween *tween0 = WORK->tweens[0];
    Ov007Tween *tween1 = WORK->tweens[1];
    int arrived0 = func_ov007_020c8f98(tween0);
    int arrived1 = func_ov007_020c8f98(tween1);
    int scrollLayers = (arrived0 == 0);
    int scrollBoard = (arrived1 == 0);
    int st = menu->currentState;

    switch (st) {
    case 3:
        if (ticks == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            WORK->states[2]->requestedState = 0xa;
        }
        switch (menu->stateWork) {
        case 0: {
            int idx = WORK->selectedSlot;
            if (idx != -1) {
                if (fileUsed[idx] != 0) {
                    menu->requestedState = 4;
                } else {
                    WORK->selectedSlot = -1;
                    func_ov007_020b7d94(3);
                    menu->stateWork = 1;
                }
            }
            break;
        }
        case 1:
            if (func_ov007_020b79e4() == 0) {
                menu->stateWork = 0;
            }
            break;
        }
        break;
    case 4:
        if (ticks == 0) {
            func_ov007_020c1d8c(0);
            RequestScreenFade(8, 0xa, 0);
            WORK->states[2]->requestedState = 0xb;
            MenuText_DrawRecordBoard(WORK->selectedSlot, WORK->settings, WORK->recordBoardAlt);
            tween0->state = 1;
            tween0->mode = 1;
            tween0->t = 0;
            tween0->step = 0;
            tween0->from = 0;
            *tween0->out = tween0->from;
            tween0->to = 0;
            tween1->state = 1;
            tween1->mode = 1;
            tween1->t = 0;
            tween1->step = 0x2000;
            tween1->from = 0x30000;
            *tween1->out = tween1->from;
            tween1->to = 0x1000;
        } else if (arrived1 != 0) {
            menu->requestedState = 2;
        }
        break;
    case 2:
        if (ticks == 0) {
            func_ov007_020c1d78(0);
        }
        {
            int target = WORK->targetSlot;
            if (target != -1) {
                if (target != WORK->selectedSlot) {
                    menu->requestedState = 5;
                }
            }
        }
        break;
    case 5: {
        int fromX = 0x20000;
        int target = WORK->targetSlot;
        int toX = -0x20000;
        if (target < WORK->selectedSlot) {
            toX = 0x20000;
            fromX = -0x20000;
        }
        if (ticks == 0) {
            func_ov007_020c1d8c(0);
            tween0->state = 1;
            tween0->mode = 1;
            tween0->t = 0;
            tween0->step = 0x3000;
            tween0->from = 0;
            *tween0->out = tween0->from;
            tween0->to = toX;
        } else if (arrived0 != 0) {
            switch (menu->stateWork) {
            case 1:
                WORK->selectedSlot = target;
                WORK->targetSlot = -1;
                menu->requestedState = 2;
                break;
            case 0:
            case 2:
                if (fileUsed[target] != 0) {
                    if (func_ov007_020b79e4() == 0) {
                        func_ov007_020c1d8c(0);
                        if (menu->stateWork == 2) {
                            RequestScreenFade(8, 0xa, 0);
                            WORK->states[2]->requestedState = 0xb;
                        }
                        MenuText_DrawRecordBoard(WORK->targetSlot, WORK->settings,
                                                 WORK->recordBoardAlt);
                        scrollLayers = 1;
                        tween0->state = 1;
                        tween0->mode = 1;
                        tween0->t = 0;
                        tween0->step = 0x3000;
                        tween0->from = fromX;
                        *tween0->out = tween0->from;
                        tween0->to = 0;
                        tween1->state = 1;
                        tween1->mode = 1;
                        tween1->t = 0;
                        tween1->step = 0x1000;
                        tween1->from = 0x1000;
                        *tween1->out = tween1->from;
                        tween1->to = 0x1000;
                        menu->stateWork = 1;
                    }
                } else {
                    int t;
                    RequestScreenFade(8, 8, 0);
                    WORK->states[2]->requestedState = 0xa;
                    t = WORK->selectedSlot;
                    WORK->selectedSlot = WORK->targetSlot;
                    WORK->targetSlot = t;
                    func_ov007_020b7d94(3);
                    func_ov007_020c0638(WORK->layerSlots[6]->layer, 0, 0, 0);
                    menu->stateWork = 2;
                }
                break;
            }
        }
        break;
    }
    case 6:
        if (ticks == 0) {
            func_ov007_020c1d8c(0);
            if (menu->previousState == 3) {
                menu->requestedState = 0;
            } else {
                RequestScreenFade(8, 8, 0);
                WORK->states[2]->requestedState = 0xa;
                tween1->state = 1;
                tween1->mode = 1;
                tween1->t = 0;
                tween1->step = 0x2000;
                tween1->from = 0x1000;
                *tween1->out = tween1->from;
                tween1->to = 0x30000;
            }
        } else if (arrived1 != 0) {
            if (menu->previousState == 2) {
                menu->requestedState = 3;
            } else {
                menu->requestedState = 0;
            }
        }
        break;
    }

    if (menu->currentState == 3)
        return;
    if (func_ov007_020b79e4() != 0)
        return;
    if (scrollLayers != 0) {
        func_ov007_020c0638(WORK->layerSlots[6]->layer, (WORK->tweenOut[0] >> 0xc) * 8, 0, 0);
        func_ov007_020c0638(WORK->layerSlots[7]->layer, (WORK->tweenOut[0] >> 0xc) * 8, 0, 0);
    }
    if (scrollBoard == 0)
        return;
    MenuText_ScrollRecordBoard(WORK->tweenOut[1] >> 0xc);
}
