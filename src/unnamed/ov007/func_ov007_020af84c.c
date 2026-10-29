#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"

/* The 0x70-byte key record func_ov007_020c2304 allocates; names as in
 * func_ov007_020c20b8, only the slots read here. */
typedef struct Ov007Pad {
    u16 keys;     /* 0x00: held this frame */
    u16 prevKeys; /* 0x02: held last frame */
} Ov007Pad;

extern char *data_ov007_0210342c;
extern int data_ov007_020ccc20[];

/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
extern void func_ov007_020b43dc(void *self);
extern int func_ov007_020b79e4(void);
extern int func_ov007_020b4438(void);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020b79c8(void);
extern void func_ov007_020aec00(void);
extern void Crash(void);
extern void func_ov007_020bdeb0(int idx);
extern void func_ov007_020b7a44(void);

typedef int (*pfn)(int);

void func_ov007_020af84c(void)
{
    char *b = data_ov007_0210342c;
    Ov007MenuState *s2 = ((Ov007Work *)b)->states[1];
    int state = s2->currentState;
    int r2var = s2->stateTicks;
    unsigned char *base = *(unsigned char **)(*(char **)(b + 0x28)) + 0xb;

    switch (state) {
    case 7:
        if (r2var == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            WORK->states[2]->requestedState = 0xc;
        }
        switch (s2->stateWork) {
        case 0: {
            int idx = WORK->selectedSlot;
            if (idx == -1)
                return;
            if (base[idx] != 0) {
                s2->stateWork = 1;
                return;
            }
            func_ov007_020b7d94(3);
            WORK->selectedSlot = -1;
            s2->stateWork = 2;
            return;
        }
        case 1: {
            int idx = WORK->selectedSlot;
            if (idx == -1) {
                s2->stateWork = 0;
                func_ov007_020c1d78(0);
                return;
            }
            {
                int idx2 = WORK->targetSlot;
                if (idx2 == -1)
                    return;
                if (base[idx2] == 0) {
                    s2->requestedState = 8;
                    return;
                }
                func_ov007_020b43dc((void *)9);
                s2->stateWork = 3;
                return;
            }
        }
        case 2:
            if (func_ov007_020b79e4() == 0)
                s2->stateWork = 0;
            return;
        case 3:
            if (func_ov007_020b4438() == 2) {
                s2->requestedState = 8;
                func_ov007_020c1d8c(0);
                return;
            }
            if (func_ov007_020b4438() != 3)
                return;
            WORK->targetSlot = -1;
            s2->stateWork = 0;
            return;
        }
        break;

    case 8:
        if (r2var == 0) {
            func_ov007_020c1d8c(0);
            func_ov007_020b7d94(4);
        }
        switch (s2->stateWork) {
        case 0: {
            int r;
            if (func_ov007_020b79c8() == 0)
                return;
            r = (*(pfn *)(data_ov007_0210342c + 0x2c))(
                ((int(*)[3])data_ov007_020ccc20)[WORK->selectedSlot][WORK->targetSlot]);
            func_ov007_020aec00();
            switch (r) {
            case 0:
                s2->stateWork = 1;
                return;
            case 1:
                s2->stateWork = 2;
                return;
            case 2:
                Crash();
                return;
            }
            return;
        }
        case 1:
        case 2: {
            int bb;
            if (func_ov007_020b79e4() != 0)
                return;
            bb = (s2->stateWork == 1);
            func_ov007_020b7d94(bb ? 5 : WORK->targetSlot + 6);
            WORK->targetSlot = -1;
            WORK->selectedSlot = WORK->targetSlot;
            s2->stateWork = bb ? 4 : 3;
            return;
        }
        case 3:
            if (func_ov007_020b79c8() == 0)
                return;
            {
                Ov007Pad *p54 = WORK->pad;
                unsigned short aa = p54->prevKeys;
                unsigned short bx = p54->keys;
                if ((bx & ~aa) == 0) {
                    Ov007Touch *p50 = WORK->touch;
                    if (p50->cur.touch == 0)
                        return;
                    if (p50->prev.touch != 0)
                        return;
                    if (p50->releasedFrames < 1)
                        return;
                }
                func_ov007_020bdeb0(0xc);
                func_ov007_020b7a44();
                s2->stateWork = 4;
                return;
            }
        case 4:
            if (func_ov007_020b79e4() == 0)
                s2->requestedState = 7;
            return;
        }
        break;
    }
}
