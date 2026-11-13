#include "Ov007Work.h"
#include "Ov007MenuMem.h"
#define ItemView_Destroy _Z16ItemView_DestroyP13Ov007ItemView
#define PickerScene_Destroy _Z19PickerScene_Destroyv

extern void func_ov007_020c937c(void *state);
extern void func_ov007_020c020c(struct Ov007Camera *cam);
extern void func_ov007_020c2410(void *p);
extern void func_ov007_020c2018(void *p);
extern void func_ov007_020c22f8(void *p);
extern void func_ov007_020c9080(void *p);
extern void func_ov007_020c1620(void *tex);
extern void func_ov007_020b3edc(int a);
extern void func_ov007_020c9460(void *p);
extern void ItemView_Destroy(struct Ov007ItemView *view);
extern void func_ov007_020b6cd0(void);
extern void func_ov007_020b283c(void);
extern void PickerScene_Destroy(void);
extern void func_ov007_020bf304(void);

/* Teardown of the data_ov007_0210342c manager. */
void func_ov007_020b65e0(void)
{
    int i;

    if (WORK->states[0] != 0) {
        func_ov007_020c937c(WORK->states[0]);
        WORK->states[0] = 0;
    }
    if (WORK->states[1] != 0) {
        func_ov007_020c937c(WORK->states[1]);
        WORK->states[1] = 0;
    }
    if (WORK->states[2] != 0) {
        func_ov007_020c937c(WORK->states[2]);
        WORK->states[2] = 0;
    }
    if (WORK->cameras[0] != 0) {
        func_ov007_020c020c(WORK->cameras[0]);
        WORK->cameras[0] = 0;
    }
    if (WORK->cameras[1] != 0) {
        func_ov007_020c020c(WORK->cameras[1]);
        WORK->cameras[1] = 0;
    }
    if (WORK->lights[0] != 0) {
        func_ov007_020c2410(WORK->lights[0]);
        WORK->lights[0] = 0;
    }
    if (WORK->lights[1] != 0) {
        func_ov007_020c2410(WORK->lights[1]);
        WORK->lights[1] = 0;
    }
    if (WORK->touch != 0) {
        func_ov007_020c2018(WORK->touch);
        WORK->touch = 0;
    }
    if (WORK->pad != 0) {
        func_ov007_020c22f8(WORK->pad);
        WORK->pad = 0;
    }
    if (WORK->tweens[0] != 0) {
        func_ov007_020c9080(WORK->tweens[0]);
        WORK->tweens[0] = 0;
    }
    if (WORK->tweens[1] != 0) {
        func_ov007_020c9080(WORK->tweens[1]);
        WORK->tweens[1] = 0;
    }
    if (WORK->settings != 0) {
        func_ov007_020c3d1c(WORK->settings);
        WORK->settings = 0;
    }

    for (i = 0; i < 0xb; i++) {
        if (WORK->textures[i] != 0) {
            func_ov007_020c1620(WORK->textures[i]);
            WORK->textures[i] = 0;
        }
    }

    func_ov007_020b3edc(0x16);

    if (WORK->paletteState != 0) {
        func_ov007_020c937c(WORK->paletteState);
        WORK->paletteState = 0;
    }

    for (i = 0; i < 3; i++) {
        if (WORK->paletteBufs[i] != 0) {
            func_ov007_020c9460(WORK->paletteBufs[i]);
            WORK->paletteBufs[i] = 0;
        }
    }

    for (i = 0; i < 0x18; i++) {
        if (WORK->itemViews[i] != 0) {
            ItemView_Destroy(WORK->itemViews[i]);
            WORK->itemViews[i] = 0;
        }
    }

    func_ov007_020b6cd0();
    func_ov007_020b283c();
    PickerScene_Destroy();
    func_ov007_020bf304();

    if (WORK == 0) {
        return;
    }

    func_ov007_020c3d1c(WORK);
    WORK = 0;
}
