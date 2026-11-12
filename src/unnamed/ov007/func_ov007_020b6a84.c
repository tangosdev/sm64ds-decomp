#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007MenuMem.h"

extern void *func_ov007_020c9388(int initialState);
extern struct Ov007Camera *func_ov007_020c023c(void);
extern struct Ov007Light *func_ov007_020c241c(int id);
extern struct Ov007Touch *func_ov007_020c2068(void);
extern struct Ov007Pad *func_ov007_020c2304(void);
extern struct Ov007Tween *func_ov007_020c908c(s32 *out);
extern void OS_GetOwnerInfo(unsigned char *dst);

void func_ov007_020b6a84(void) {
    unsigned char buf[0x54];
    int v;

    WORK->states[0] = func_ov007_020c9388(0);
    WORK->states[1] = func_ov007_020c9388(-1);
    WORK->states[2] = func_ov007_020c9388(1);
    WORK->cameras[0] = func_ov007_020c023c();
    WORK->cameras[1] = func_ov007_020c023c();
    WORK->lights[0] = func_ov007_020c241c(0);
    WORK->lights[1] = func_ov007_020c241c(1);
    WORK->touch = func_ov007_020c2068();

    WORK->pad = func_ov007_020c2304();
    WORK->tweens[0] = func_ov007_020c908c(&WORK->tweenOut[0]);
    WORK->tweens[1] = func_ov007_020c908c(&WORK->tweenOut[1]);

    WORK->settings = func_ov007_020c3df4(0, sizeof(Ov007Settings));

    /* The owner-info language (0 Japanese ... 5 Spanish) to the menu's own
     * numbering. */
    OS_GetOwnerInfo(buf);
    v = buf[0];
    switch (v) {
    case 0:
    case 1:
        WORK->settings->language = 1;
        break;
    case 2:
        WORK->settings->language = 2;
        break;
    case 3:
        WORK->settings->language = 3;
        break;
    case 4:
        WORK->settings->language = 5;
        break;
    case 5:
        WORK->settings->language = 4;
        break;
    default:
        WORK->settings->language = 0;
        break;
    }
    WORK->modelReady = 0;
}
