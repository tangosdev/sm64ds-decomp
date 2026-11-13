//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Item.h"

extern void MessageWindow_Update(void);
extern "C" void func_ov007_020c92d0(Ov007MenuState *state);
extern void func_ov007_020c934c(Ov007MenuState *state, int initialState);
extern void func_ov007_020b066c(void);
extern "C" void func_ov007_020af0e4(void);
extern "C" void func_ov007_020af1f4(void);
extern void func_ov007_020aef84(void);
extern "C" void func_ov007_020af434(void);
extern "C" void func_ov007_020b0340(void);
extern "C" void func_ov007_020c2090(struct Ov007Pad *pad);
extern "C" void func_ov007_020b0548(void);
extern "C" void func_ov007_020b0358(void);
extern void func_ov007_020aefb4(void);
extern "C" void func_ov007_020af12c(void);
extern "C" void func_ov007_020aef78(void);
extern "C" void func_ov007_020af390(void);
extern "C" void func_ov007_020b0324(void);
extern void ItemView_Update(struct Ov007ItemView *view);
extern "C" void func_ov007_020add3c(struct Ov007ItemNode *self);
extern void func_ov007_020ad660(struct Ov007LayerSlot *slot);

void func_ov007_020aed98(void)
{
    Ov007Work *g = WORK;
    Ov007MenuState *r6 = g->states[0];
    Ov007MenuState *r5 = g->states[1];
    int i;

    for (i = 0; i < 0x18; i++) {
        WORK->itemViews[i]->ItemView_HandleTouch(WORK->touch);
    }

    MessageWindow_Update();
    func_ov007_020c92d0(r6);

    if (r6->stateTicks == 0) {
        func_ov007_020c934c(r5, 0);
        switch (r6->currentState) {
        case 0: func_ov007_020b066c(); break;
        case 2: func_ov007_020af0e4(); break;
        case 3: func_ov007_020af1f4(); break;
        case 4: func_ov007_020aef84(); break;
        case 5: func_ov007_020af434(); break;
        case 6: func_ov007_020b0340(); break;
        }
    }

    func_ov007_020c92d0(r5);

    if (r6->stateTicks == 0 || r5->stateTicks == 0) {
        func_ov007_020c2090(WORK->pad);
    }

    switch (r6->currentState) {
    case 0: func_ov007_020b0548(); break;
    case 1: func_ov007_020b0358(); break;
    case 2: func_ov007_020aefb4(); break;
    case 3: func_ov007_020af12c(); break;
    case 4: func_ov007_020aef78(); break;
    case 5: func_ov007_020af390(); break;
    case 6: func_ov007_020b0324(); break;
    }

    func_ov007_020c92d0(WORK->states[2]);

    WORK->chosenItemId = -1;
    for (i = 0; i < 0x18; i++) {
        ItemView_Update(WORK->itemViews[i]);
    }
    for (i = 0; i < 9; i++) {
        func_ov007_020add3c(WORK->itemNodes[i]);
    }
    for (i = 0; i < 8; i++) {
        func_ov007_020ad660(WORK->layerSlots[i]);
    }
}

