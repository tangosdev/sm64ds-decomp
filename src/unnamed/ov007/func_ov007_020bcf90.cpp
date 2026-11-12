//cpp
#include "nitro/gx/g3x.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Camera.h"

extern "C" {

extern void func_ov007_020c93b4(struct Ov007PaletteBuf *buf);
extern void func_ov007_020b2160(int a);
extern void func_ov007_020b8fd4(struct Ov007Camera *camera);
extern void func_ov007_020b413c(void *view, int number);
extern void func_ov007_020b2728(void);
extern int func_ov007_020b7a34(void);
extern void func_ov007_020b7a00(void);
extern void func_ov007_020b4464(void *view, struct Ov007Camera *camera);
extern void func_ov007_020b2cf0(void);
extern void func_ov007_020c2390(struct Ov007Light *light);
extern void func_ov007_020c232c(struct Ov007Light *light);
extern void func_ov007_020b91b4(void);
extern void func_ov007_020bee14(void);
extern void func_ov007_020b7658(Matrix4x3 *viewMatrix, int update);
extern void func_ov007_020b2370(void);
extern void ScreenFade_Draw(struct Ov007Camera *camera);

/* The menu's per-frame 3D draw. func_ov007_020b2728 and the model / figure
 * draw (func_ov007_020b2370) run only in main state 0, and in state 1 while
 * states[1] is in state 0. Per main state it also runs the picker's draws
 * (state 1 once states[1] reaches 2; states 2..4), or in state 5 draws the
 * three slot totals on item views 0xe..0x10; light 1 is set only in state
 * 0. */
void func_ov007_020bcf90(void) {
    Ov007MenuState *s0;
    Ov007MenuState *s1;
    int i;
    int j;
    int drawModel = 1;
    s0 = WORK->states[0];
    s1 = WORK->states[1];
    if (s0->currentState != 0 && (s0->currentState != 1 || s1->currentState != 0)) {
        drawModel = 0;
    }
    func_ov007_020c93b4(WORK->paletteBufs[0]);
    G3X_Reset();
    G3X_ResetMtxStack();
    G3X_SetClearColor(0x7fff, 0, 0x7fff, 0x3f, true);
    func_ov007_020b2160(1);
    switch (s0->currentState) {
    case 0:
        break;
    case 1:
        if (s1->currentState == 2) {
            func_ov007_020b8fd4(WORK->cameras[1]);
        }
        break;
    case 2:
    case 3:
    case 4:
        func_ov007_020b8fd4(WORK->cameras[1]);
        break;
    case 5:
        for (i = 0xe; i <= 0x10; i++) {
            s16 *total = &WORK->settings->starCounts[i - 0xe];
            func_ov007_020b413c(WORK->itemViews[i], *total);
        }
        break;
    case 6:
        break;
    }

    if (drawModel != 0) {
        func_ov007_020b2728();
    }
    if (func_ov007_020b7a34() != 0) {
        func_ov007_020b7a00();
    }
    for (j = 0; j < 0x18; j++) {
        func_ov007_020b4464(WORK->itemViews[j], WORK->cameras[1]);
    }

    func_ov007_020b2cf0();
    func_ov007_020b2160(0);
    func_ov007_020c2390(WORK->lights[0]);
    func_ov007_020c232c(WORK->lights[0]);
    switch (s0->currentState) {
    case 0:
        func_ov007_020c2390(WORK->lights[1]);
        func_ov007_020c232c(WORK->lights[1]);
        break;
    case 1:
        if (s1->currentState == 2) {
            func_ov007_020b91b4();
        }
        break;
    case 2:
    case 3:
    case 4:
        func_ov007_020b91b4();
        break;
    case 5:
        func_ov007_020bee14();
        break;
    case 6:
        break;
    }

    func_ov007_020b7658(&WORK->cameras[0]->viewMatrix, 1);
    if (drawModel != 0) {
        func_ov007_020b2370();
    }
    ScreenFade_Draw(WORK->cameras[0]);
}
}
