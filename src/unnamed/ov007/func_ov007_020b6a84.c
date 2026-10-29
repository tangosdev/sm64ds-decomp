#include "Ov007Work.h"

extern void *func_ov007_020c9388(int initialState);
extern char *func_ov007_020c023c(void);
extern int func_ov007_020c241c(int a);
extern char *func_ov007_020c2068(void);
extern char *func_ov007_020c2304(void);
extern void *func_ov007_020c908c(void *r5);
extern int func_ov007_020c3df4(int a, void *b);
extern void func_02059eb0(unsigned char *dst);
extern char *data_ov007_0210342c;

/* The 0x3c-byte record allocated below; only the slot this file seeds. */
typedef struct Ov007Settings {
    char pad_00[0x3a];
    unsigned char language; /* 0x3a: seeded below from the owner-info language */
} Ov007Settings;

/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

void func_ov007_020b6a84(void) {
    unsigned char buf[0x54];
    int v;

    WORK->states[0] = func_ov007_020c9388(0);
    WORK->states[1] = func_ov007_020c9388(-1);
    WORK->states[2] = func_ov007_020c9388(1);
    WORK->cameras[0] = (struct Ov007Camera *)func_ov007_020c023c();
    WORK->cameras[1] = (struct Ov007Camera *)func_ov007_020c023c();
    WORK->lights[0] = (struct Ov007Light *)func_ov007_020c241c(0);
    WORK->lights[1] = (struct Ov007Light *)func_ov007_020c241c(1);
    WORK->touch = (struct Ov007Touch *)func_ov007_020c2068();

    WORK->pad = (struct Ov007Pad *)func_ov007_020c2304();
    WORK->tweens[0] = func_ov007_020c908c((void *)&WORK->tweenOut[0]);
    WORK->tweens[1] = func_ov007_020c908c((void *)&WORK->tweenOut[1]);

    WORK->settings = (Ov007Settings *)func_ov007_020c3df4(0, (void *)0x3c);

    /* The owner-info language (0 Japanese ... 5 Spanish) to the menu's own
     * numbering. */
    func_02059eb0(buf);
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
    *(int *)(data_ov007_0210342c + 0xe8) = 0;
}
