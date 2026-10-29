//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern "C" {
extern void func_ov007_020b46b0(void* elem, void* tp);
extern void func_ov007_020b7a78(void);
extern void func_ov007_020c92d0(int o);
extern void func_ov007_020c934c(int o, int v);
extern void func_ov007_020b066c(void);
extern void func_ov007_020af0e4(void);
extern void func_ov007_020af1f4(void);
extern void func_ov007_020aef84(void);
extern void func_ov007_020af434(void);
extern void func_ov007_020b0340(void);
extern void func_ov007_020c2090(int c);
extern void func_ov007_020b0548(void);
extern void func_ov007_020b0358(void);
extern void func_ov007_020aefb4(void);
extern void func_ov007_020af12c(void);
extern void func_ov007_020aef78(void);
extern void func_ov007_020af390(void);
extern void func_ov007_020b0324(void);
extern void func_ov007_020b44ec(int a);
extern void func_ov007_020add3c(int c);
extern void func_ov007_020ad660(int a);
extern char *data_ov007_0210342c;

typedef char
    Ov007Work_chosenItemId_must_be_at_0x180[(int)&((Ov007Work *)0)->chosenItemId == 0x180 ? 1 : -1];

#pragma opt_strength_reduction off
void func_ov007_020aed98(void)
{
    Ov007Work *g = (Ov007Work *)data_ov007_0210342c;
    Ov007MenuState *r6 = g->states[0];
    Ov007MenuState *r5 = g->states[1];
    int i;

    for (i = 0; i < 0x18; i++) {
        Ov007Work *gg = (Ov007Work *)data_ov007_0210342c;
        func_ov007_020b46b0(gg->itemViews[i], gg->touch);
    }

    func_ov007_020b7a78();
    func_ov007_020c92d0((int)r6);

    if (r6->stateTicks == 0) {
        func_ov007_020c934c((int)r5, 0);
        switch (r6->currentState) {
        case 0: func_ov007_020b066c(); break;
        case 2: func_ov007_020af0e4(); break;
        case 3: func_ov007_020af1f4(); break;
        case 4: func_ov007_020aef84(); break;
        case 5: func_ov007_020af434(); break;
        case 6: func_ov007_020b0340(); break;
        }
    }

    func_ov007_020c92d0((int)r5);

    if (r6->stateTicks == 0 || r5->stateTicks == 0) {
        Ov007Work *gg = (Ov007Work *)data_ov007_0210342c;
        func_ov007_020c2090((int)gg->pad);
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

    {
        Ov007Work *gg = (Ov007Work *)data_ov007_0210342c;
        func_ov007_020c92d0((int)gg->states[2]);
    }

    {
        Ov007Work *gg = (Ov007Work *)data_ov007_0210342c;
        gg->chosenItemId = -1;
    }
    for (i = 0; i < 0x18; i++) {
        Ov007Work *gg = (Ov007Work *)data_ov007_0210342c;
        func_ov007_020b44ec((int)gg->itemViews[i]);
    }
    for (i = 0; i < 9; i++) {
        Ov007Work *gg = (Ov007Work *)data_ov007_0210342c;
        func_ov007_020add3c((int)gg->itemNodes[i]);
    }
    for (i = 0; i < 8; i++) {
        Ov007Work *gg = (Ov007Work *)data_ov007_0210342c;
        func_ov007_020ad660((int)gg->layerSlots[i]);
    }
}
}
