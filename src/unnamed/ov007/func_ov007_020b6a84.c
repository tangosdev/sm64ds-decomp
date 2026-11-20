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

    sMenuWork->states[0] = func_ov007_020c9388(0);
    sMenuWork->states[1] = func_ov007_020c9388(-1);
    sMenuWork->states[2] = func_ov007_020c9388(1);
    sMenuWork->cameras[0] = func_ov007_020c023c();
    sMenuWork->cameras[1] = func_ov007_020c023c();
    sMenuWork->lights[0] = func_ov007_020c241c(0);
    sMenuWork->lights[1] = func_ov007_020c241c(1);
    sMenuWork->touch = func_ov007_020c2068();

    sMenuWork->pad = func_ov007_020c2304();
    sMenuWork->tweens[0] = func_ov007_020c908c(&sMenuWork->tweenOut[0]);
    sMenuWork->tweens[1] = func_ov007_020c908c(&sMenuWork->tweenOut[1]);

    sMenuWork->settings = func_ov007_020c3df4(0, sizeof(Ov007Settings));

    /* The owner-info language (0 Japanese ... 5 Spanish) to the menu's own
     * numbering. */
    OS_GetOwnerInfo(buf);
    v = buf[0];
    switch (v) {
    case 0:
    case 1:
        sMenuWork->settings->language = 1;
        break;
    case 2:
        sMenuWork->settings->language = 2;
        break;
    case 3:
        sMenuWork->settings->language = 3;
        break;
    case 4:
        sMenuWork->settings->language = 5;
        break;
    case 5:
        sMenuWork->settings->language = 4;
        break;
    default:
        sMenuWork->settings->language = 0;
        break;
    }
    sMenuWork->modelReady = 0;
}
