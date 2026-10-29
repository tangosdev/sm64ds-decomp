#include "Ov007Work.h"

extern void func_ov007_020c937c(void *state);
extern void func_ov007_020c020c(void *a);
extern void func_ov007_020c2410(int a);
extern void func_ov007_020c2018(int a);
extern void func_ov007_020c22f8(int a);
extern void func_ov007_020c9080(int a);
extern void func_ov007_020c3d1c(void *p);
extern void func_ov007_020c1620(void *tex);
extern void func_ov007_020b3edc(int a);
extern void func_ov007_020c9460(int a);
extern void func_ov007_020b4bd4(struct Ov007ItemView *view);
extern void func_ov007_020b6cd0(void);
extern void func_ov007_020b283c(void);
extern void func_ov007_020b9770(void);
extern void func_ov007_020bf304(void);

extern char *data_ov007_0210342c;
/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* Teardown of the data_ov007_0210342c manager. */
#pragma opt_strength_reduction off
#pragma opt_common_subs off
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
        func_ov007_020c2410((int)WORK->lights[0]);
        WORK->lights[0] = 0;
    }
    if (WORK->lights[1] != 0) {
        func_ov007_020c2410((int)WORK->lights[1]);
        WORK->lights[1] = 0;
    }
    if (WORK->touch != 0) {
        func_ov007_020c2018((int)WORK->touch);
        WORK->touch = 0;
    }
    if (WORK->pad != 0) {
        func_ov007_020c22f8((int)WORK->pad);
        WORK->pad = 0;
    }
    if (WORK->tweens[0] != 0) {
        func_ov007_020c9080((int)WORK->tweens[0]);
        WORK->tweens[0] = 0;
    }
    if (WORK->tweens[1] != 0) {
        func_ov007_020c9080((int)WORK->tweens[1]);
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
        if (*(int *)(data_ov007_0210342c + i * 4 + 0x44) != 0) {
            func_ov007_020c9460(*(int *)(data_ov007_0210342c + i * 4 + 0x44));
            *(int *)(data_ov007_0210342c + i * 4 + 0x44) = 0;
        }
    }

    for (i = 0; i < 0x18; i++) {
        if (WORK->itemViews[i] != 0) {
            func_ov007_020b4bd4(WORK->itemViews[i]);
            WORK->itemViews[i] = 0;
        }
    }

    func_ov007_020b6cd0();
    func_ov007_020b283c();
    func_ov007_020b9770();
    func_ov007_020bf304();

    if (data_ov007_0210342c == 0) {
        return;
    }

    func_ov007_020c3d1c((void *)data_ov007_0210342c);
    data_ov007_0210342c = 0;
}
