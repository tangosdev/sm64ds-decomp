//cpp
#include "Ov007Work.h"
#include "Ov007MenuMem.h"
#include "Ov007Texture.h"

extern "C" void func_ov007_020c937c(void *state);
extern "C" void func_ov007_020c020c(struct Ov007Camera *cam);
extern "C" void func_ov007_020c2410(void *p);
extern "C" void func_ov007_020c2018(void *p);
extern "C" void func_ov007_020c22f8(void *p);
extern "C" void func_ov007_020c9080(void *p);
extern "C" void func_ov007_020b3edc(int a);
extern "C" void func_ov007_020c9460(void *p);
extern "C" void ItemView_Destroy(struct Ov007ItemView *view);
extern "C" void func_ov007_020b6cd0(void);
extern "C" void func_ov007_020b283c(void);
extern "C" void PickerScene_Destroy(void);
extern "C" void func_ov007_020bf304(void);

/* Teardown of the sMenuWork manager. */
extern "C" void func_ov007_020b65e0(void) {
    int i;

    if (sMenuWork->states[0] != 0) {
        func_ov007_020c937c(sMenuWork->states[0]);
        sMenuWork->states[0] = 0;
    }
    if (sMenuWork->states[1] != 0) {
        func_ov007_020c937c(sMenuWork->states[1]);
        sMenuWork->states[1] = 0;
    }
    if (sMenuWork->states[2] != 0) {
        func_ov007_020c937c(sMenuWork->states[2]);
        sMenuWork->states[2] = 0;
    }
    if (sMenuWork->cameras[0] != 0) {
        func_ov007_020c020c(sMenuWork->cameras[0]);
        sMenuWork->cameras[0] = 0;
    }
    if (sMenuWork->cameras[1] != 0) {
        func_ov007_020c020c(sMenuWork->cameras[1]);
        sMenuWork->cameras[1] = 0;
    }
    if (sMenuWork->lights[0] != 0) {
        func_ov007_020c2410(sMenuWork->lights[0]);
        sMenuWork->lights[0] = 0;
    }
    if (sMenuWork->lights[1] != 0) {
        func_ov007_020c2410(sMenuWork->lights[1]);
        sMenuWork->lights[1] = 0;
    }
    if (sMenuWork->touch != 0) {
        func_ov007_020c2018(sMenuWork->touch);
        sMenuWork->touch = 0;
    }
    if (sMenuWork->pad != 0) {
        func_ov007_020c22f8(sMenuWork->pad);
        sMenuWork->pad = 0;
    }
    if (sMenuWork->tweens[0] != 0) {
        func_ov007_020c9080(sMenuWork->tweens[0]);
        sMenuWork->tweens[0] = 0;
    }
    if (sMenuWork->tweens[1] != 0) {
        func_ov007_020c9080(sMenuWork->tweens[1]);
        sMenuWork->tweens[1] = 0;
    }
    if (sMenuWork->settings != 0) {
        func_ov007_020c3d1c(sMenuWork->settings);
        sMenuWork->settings = 0;
    }

    for (i = 0; i < 11; i++) {
        if (sMenuWork->textures[i] != 0) {
            ((Ov007Texture *)sMenuWork->textures[i])->func_ov007_020c1620();
            sMenuWork->textures[i] = 0;
        }
    }

    func_ov007_020b3edc(0x16);

    if (sMenuWork->paletteState != 0) {
        func_ov007_020c937c(sMenuWork->paletteState);
        sMenuWork->paletteState = 0;
    }

    for (i = 0; i < 3; i++) {
        if (sMenuWork->paletteBufs[i] != 0) {
            func_ov007_020c9460(sMenuWork->paletteBufs[i]);
            sMenuWork->paletteBufs[i] = 0;
        }
    }

    for (i = 0; i < 24; i++) {
        if (sMenuWork->itemViews[i] != 0) {
            ItemView_Destroy(sMenuWork->itemViews[i]);
            sMenuWork->itemViews[i] = 0;
        }
    }

    func_ov007_020b6cd0();
    func_ov007_020b283c();
    PickerScene_Destroy();
    func_ov007_020bf304();

    if (sMenuWork == 0) {
        return;
    }

    func_ov007_020c3d1c(sMenuWork);
    sMenuWork = 0;
}
