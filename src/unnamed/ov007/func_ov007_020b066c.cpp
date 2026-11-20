//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"

extern "C" {
extern void func_ov007_020b283c(void);
extern void PickerScene_Destroy(void);
extern void func_ov007_020bf304(void);
extern void func_ov007_020b6544(void);
extern void func_ov007_020bcba8(int i);
extern void func_ov007_020bca40(int i);
extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void func_ov007_020bd1c0(int i);
extern void func_ov007_020b131c(void);
}

void func_ov007_020b066c(void) {
    switch (gMenuWork->states[0]->previousState) {
    case 6:
        func_ov007_020b283c();
        break;
    case 4:
        PickerScene_Destroy();
        break;
    case 5:
        func_ov007_020bf304();
        gMenuWork->callback(0x11);
        break;
    default:
        gMenuWork->settings->minigamesFailed = gMenuWork->callback(0x15);
        gMenuWork->settings->filesFailed = gMenuWork->callback(0x14);
        gMenuWork->settings->dialogKind = 0;
        break;
    }

    func_ov007_020b6544();
    func_ov007_020bcba8(1);
    func_ov007_020bca40(0);

    {
        Ov007Work *bb = gMenuWork;
        if ((unsigned short)(short)(bb->states[0]->previousState - 4) <= 2) {
            bb->states[1]->requestedState = 5;
        } else {
            switch (bb->settings->bootArgs->status) {
            case 0:
            case 2:
                bb->states[1]->requestedState = 0xb;
                break;
            case 1:
                bb->states[1]->requestedState = 0xa;
                RequestScreenFade(3, 0xa, 0);
                break;
            }
        }
    }

    gMenuWork->states[2]->requestedState = 0;

    gMenuWork->figureIndex = gMenuWork->nextFigureIndex;

    if (gMenuWork->nextFigureIndex == 0)
        gMenuWork->nextFigureIndex = 1;
    else
        gMenuWork->nextFigureIndex = 0;

    func_ov007_020bd1c0(0);

    gMenuWork->model->visible = 0;
    gMenuWork->figure->model->visible = 0;
    gMenuWork->altFigure->model->visible = 0;
    func_ov007_020b131c();
}
