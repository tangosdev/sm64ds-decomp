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

/* Teardown of the gMenuWork manager. */
extern "C" void func_ov007_020b65e0(void) {
    int i;

    if (gMenuWork->states[0] != 0) {
        func_ov007_020c937c(gMenuWork->states[0]);
        gMenuWork->states[0] = 0;
    }
    if (gMenuWork->states[1] != 0) {
        func_ov007_020c937c(gMenuWork->states[1]);
        gMenuWork->states[1] = 0;
    }
    if (gMenuWork->states[2] != 0) {
        func_ov007_020c937c(gMenuWork->states[2]);
        gMenuWork->states[2] = 0;
    }
    if (gMenuWork->cameras[0] != 0) {
        func_ov007_020c020c(gMenuWork->cameras[0]);
        gMenuWork->cameras[0] = 0;
    }
    if (gMenuWork->cameras[1] != 0) {
        func_ov007_020c020c(gMenuWork->cameras[1]);
        gMenuWork->cameras[1] = 0;
    }
    if (gMenuWork->lights[0] != 0) {
        func_ov007_020c2410(gMenuWork->lights[0]);
        gMenuWork->lights[0] = 0;
    }
    if (gMenuWork->lights[1] != 0) {
        func_ov007_020c2410(gMenuWork->lights[1]);
        gMenuWork->lights[1] = 0;
    }
    if (gMenuWork->touch != 0) {
        func_ov007_020c2018(gMenuWork->touch);
        gMenuWork->touch = 0;
    }
    if (gMenuWork->pad != 0) {
        func_ov007_020c22f8(gMenuWork->pad);
        gMenuWork->pad = 0;
    }
    if (gMenuWork->tweens[0] != 0) {
        func_ov007_020c9080(gMenuWork->tweens[0]);
        gMenuWork->tweens[0] = 0;
    }
    if (gMenuWork->tweens[1] != 0) {
        func_ov007_020c9080(gMenuWork->tweens[1]);
        gMenuWork->tweens[1] = 0;
    }
    if (gMenuWork->settings != 0) {
        func_ov007_020c3d1c(gMenuWork->settings);
        gMenuWork->settings = 0;
    }

    for (i = 0; i < 11; i++) {
        if (gMenuWork->textures[i] != 0) {
            ((Ov007Texture *)gMenuWork->textures[i])->func_ov007_020c1620();
            gMenuWork->textures[i] = 0;
        }
    }

    func_ov007_020b3edc(0x16);

    if (gMenuWork->paletteState != 0) {
        func_ov007_020c937c(gMenuWork->paletteState);
        gMenuWork->paletteState = 0;
    }

    for (i = 0; i < 3; i++) {
        if (gMenuWork->paletteBufs[i] != 0) {
            func_ov007_020c9460(gMenuWork->paletteBufs[i]);
            gMenuWork->paletteBufs[i] = 0;
        }
    }

    for (i = 0; i < 24; i++) {
        if (gMenuWork->itemViews[i] != 0) {
            ItemView_Destroy(gMenuWork->itemViews[i]);
            gMenuWork->itemViews[i] = 0;
        }
    }

    func_ov007_020b6cd0();
    func_ov007_020b283c();
    PickerScene_Destroy();
    func_ov007_020bf304();

    if (gMenuWork == 0) {
        return;
    }

    func_ov007_020c3d1c(gMenuWork);
    gMenuWork = 0;
}
