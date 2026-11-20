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
    sMenuWork->pendingCapture = 0;
    sMenuWork->pendingBgMode = 1;
    sMenuWork->pendingChoice = 0;
    sMenuWork->choiceReady = 0;
    sMenuWork->frameCount = 0;
    sMenuWork->chosenItemId = -1;
    sMenuWork->cameraPreset = 0;
    sMenuWork->transitionMode = 1;
    func_ov007_020b20e8();
    int i;
    for (i = 0; i < 24; i++) {
        ItemView_ResetSprite(sMenuWork->itemViews[i], sMenuWork->cameras[1]);
    }
}
