#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"

/* The 0x3c-byte record func_ov007_020b6a84 allocates; names as in
 * func_ov007_020aec94. +0x37 and +0x38 stay raw, as there. */
typedef struct Ov007Settings {
    char pad_00[0x39];
    /* 0x39: func_ov007_020b7d94's kind, the window menu state 9 opens before
     * the choice goes through; 0 = none (func_ov007_020b066c) */
    unsigned char dialogKind;
} Ov007Settings;

/* func_ov007_020adc08's 0xc-byte layer slot; func_ov007_020c0638 rebuilds its layer. */
typedef struct Ov007LayerSlot {
    int state;   /* 0x00: an Ov007MenuState (func_ov007_020c9388) */
    void *layer; /* 0x04 */
} Ov007LayerSlot;

/* The 0x70-byte key record func_ov007_020c2304 allocates; names as in
 * func_ov007_020c20b8. */
typedef struct Ov007Pad {
    u16 keys;     /* 0x00: held this frame */
    u16 prevKeys; /* 0x02: held last frame */
} Ov007Pad;

extern void func_ov007_020c0638(int a, int b, int c, int d);
extern void func_ov007_020b7d94(int kind);
extern void func_ov007_020b2614(int a, int b, int c);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020b79c8(void);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020b7a44(void);
extern int func_ov007_020b79e4(void);
extern void func_ov007_020aec94(int a);

extern char* data_ov007_0210342c;

void func_ov007_020b0834(void)
{
    char* G = data_ov007_0210342c;

    if (((Ov007Work *)G)->states[1]->stateTicks == 0) {
        func_ov007_020c0638((int)((Ov007Work *)G)->layerSlots[6]->layer, 0, 0, 0);
        G = data_ov007_0210342c;
        func_ov007_020b7d94(((Ov007Work *)G)->settings->dialogKind);
        func_ov007_020b2614(2, 0, 8);
        func_ov007_020c1d8c(0);
    }

    G = data_ov007_0210342c;
    switch (((Ov007Work *)G)->states[1]->stateWork) {
    case 0:
        if (func_ov007_020b79c8() == 0)
            return;
        G = data_ov007_0210342c;
        {
            Ov007Pad *p54 = ((Ov007Work *)G)->pad;
            if ((p54->keys & ~p54->prevKeys) == 0) {
                Ov007Touch *p50 = ((Ov007Work *)G)->touch;
                if (p50->cur.touch == 0)
                    return;
                if (p50->prev.touch != 0)
                    return;
                if (p50->releasedFrames < 1)
                    return;
            }
        }
        {
            char *pB = (char *)((Ov007Work *)G)->settings;
            int r5 = (*(unsigned char*)(pB + 0x37) == 1);
            int r4 = (*(unsigned char*)(pB + 0x38) == 1);
            func_ov007_020bdeb0(0xc);
            if (r5) {
                G = data_ov007_0210342c;
                if (((Ov007Work *)G)->pendingChoice == 9) {
                    int v = (*(int (**)(int))(G + 0x2c))(0x16);
                    G = data_ov007_0210342c;
                    *(unsigned char*)(*(int*)(G + 0x28) + 0x37) = v;
                }
            }
            if (r4) {
                G = data_ov007_0210342c;
                {
                    int v = (*(int (**)(int))(G + 0x2c))(0x17);
                    G = data_ov007_0210342c;
                    *(unsigned char*)(*(int*)(G + 0x28) + 0x38) = v;
                }
            }
            func_ov007_020b7a44();
            G = data_ov007_0210342c;
            ((Ov007Work *)G)->states[1]->stateWork = 1;
        }
        return;
    case 1:
        if (func_ov007_020b79e4() != 0)
            return;
        G = data_ov007_0210342c;
        func_ov007_020aec94(((Ov007Work *)G)->pendingChoice);
        return;
    }
}
