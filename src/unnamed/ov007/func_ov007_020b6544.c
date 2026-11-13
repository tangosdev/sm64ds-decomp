#include "Ov007Work.h"
#define ItemView_ResetSprite _Z20ItemView_ResetSpriteP13Ov007ItemViewP11Ov007Camera

extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b20e8(void);
extern void ItemView_ResetSprite(struct Ov007ItemView *view, struct Ov007Camera *camera);

/* Resets the manager's per-visit state, re-aims camera 1
 * (func_ov007_020b20e8) and puts every item view back at its home point
 * under it. */
void func_ov007_020b6544(void)
{
    func_ov007_020c1d8c(0);
    WORK->pendingCapture = 0;
    WORK->pendingBgMode = 1;
    WORK->pendingChoice = 0;
    WORK->choiceReady = 0;
    WORK->frameCount = 0;
    WORK->chosenItemId = -1;
    WORK->cameraPreset = 0;
    WORK->transitionMode = 1;
    func_ov007_020b20e8();
    int i;
    for (i = 0; i < 0x18; i++) {
        ItemView_ResetSprite(WORK->itemViews[i], WORK->cameras[1]);
    }
}
