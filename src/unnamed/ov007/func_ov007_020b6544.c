#include "Ov007Work.h"

extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b20e8(void);
extern void ItemView_ResetSprite(struct Ov007ItemView *view, struct Ov007Camera *camera);

/* Resets the manager's per-visit state, re-aims camera 1
 * (func_ov007_020b20e8) and puts every item view back at its home point
 * under it. */
void func_ov007_020b6544(void)
{
    func_ov007_020c1d8c(0);
    gMenuWork->pendingCapture = 0;
    gMenuWork->pendingBgMode = 1;
    gMenuWork->pendingChoice = 0;
    gMenuWork->choiceReady = 0;
    gMenuWork->frameCount = 0;
    gMenuWork->chosenItemId = -1;
    gMenuWork->cameraPreset = 0;
    gMenuWork->transitionMode = 1;
    func_ov007_020b20e8();
    int i;
    for (i = 0; i < 24; i++) {
        ItemView_ResetSprite(gMenuWork->itemViews[i], gMenuWork->cameras[1]);
    }
}
