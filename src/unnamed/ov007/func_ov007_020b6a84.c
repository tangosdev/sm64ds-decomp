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

    gMenuWork->states[0] = func_ov007_020c9388(0);
    gMenuWork->states[1] = func_ov007_020c9388(-1);
    gMenuWork->states[2] = func_ov007_020c9388(1);
    gMenuWork->cameras[0] = func_ov007_020c023c();
    gMenuWork->cameras[1] = func_ov007_020c023c();
    gMenuWork->lights[0] = func_ov007_020c241c(0);
    gMenuWork->lights[1] = func_ov007_020c241c(1);
    gMenuWork->touch = func_ov007_020c2068();

    gMenuWork->pad = func_ov007_020c2304();
    gMenuWork->tweens[0] = func_ov007_020c908c(&gMenuWork->tweenOut[0]);
    gMenuWork->tweens[1] = func_ov007_020c908c(&gMenuWork->tweenOut[1]);

    gMenuWork->settings = func_ov007_020c3df4(0, sizeof(Ov007Settings));

    /* The owner-info language (0 Japanese ... 5 Spanish) to the menu's own
     * numbering. */
    OS_GetOwnerInfo(buf);
    v = buf[0];
    switch (v) {
    case 0:
    case 1:
        gMenuWork->settings->language = 1;
        break;
    case 2:
        gMenuWork->settings->language = 2;
        break;
    case 3:
        gMenuWork->settings->language = 3;
        break;
    case 4:
        gMenuWork->settings->language = 5;
        break;
    case 5:
        gMenuWork->settings->language = 4;
        break;
    default:
        gMenuWork->settings->language = 0;
        break;
    }
    gMenuWork->modelReady = 0;
}
