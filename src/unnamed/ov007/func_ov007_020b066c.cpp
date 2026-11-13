//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"


extern "C" void func_ov007_020b283c(void);
extern "C" void PickerScene_Destroy(void);
extern "C" void func_ov007_020bf304(void);
extern "C" void func_ov007_020b6544(void);
extern "C" void func_ov007_020bcba8(int i);
extern "C" void func_ov007_020bca40(int i);
extern "C" void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern "C" void func_ov007_020bd1c0(int i);
extern "C" void func_ov007_020b131c(void);


void func_ov007_020b066c(void) {
    switch (WORK->states[0]->previousState) {
    case 6:
        func_ov007_020b283c();
        break;
    case 4:
        PickerScene_Destroy();
        break;
    case 5:
        func_ov007_020bf304();
        WORK->callback(0x11);
        break;
    default:
        WORK->settings->minigamesFailed = WORK->callback(0x15);
        WORK->settings->filesFailed = WORK->callback(0x14);
        WORK->settings->dialogKind = 0;
        break;
    }

    func_ov007_020b6544();
    func_ov007_020bcba8(1);
    func_ov007_020bca40(0);

    {
        Ov007Work *bb = WORK;
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

    WORK->states[2]->requestedState = 0;

    WORK->figureIndex = WORK->nextFigureIndex;

    if (WORK->nextFigureIndex == 0)
        WORK->nextFigureIndex = 1;
    else
        WORK->nextFigureIndex = 0;

    func_ov007_020bd1c0(0);

    WORK->model->visible = 0;
    WORK->figure->model->visible = 0;
    WORK->altFigure->model->visible = 0;
    func_ov007_020b131c();
}
