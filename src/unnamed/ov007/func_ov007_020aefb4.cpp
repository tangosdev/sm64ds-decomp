//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "cstd.h"

extern "C" {

extern char* data_ov007_0210342c;
extern int func_ov007_020b8fb4(void);
extern int func_ov007_020af260(int,int,int*,int*);
extern int func_ov007_020b8f64(void);
extern int func_ov007_020b8f50(void);
extern int func_ov007_020b9640(void);

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020ae2d0, only the slots this file writes. */
typedef struct Ov007Item {
    char pad_00[0x18];
    s32 wiggleTimer;  /* 0x18: func_ov007_020ae834 counts it down */
    s32 wiggleFrames; /* 0x1c: the wiggle's length */
} Ov007Item;

/* The 8-byte record func_ov007_020ade58 returns; names as in
 * func_ov007_020ade58, only its item here. */
typedef struct Ov007ItemNode {
    Ov007Item *item; /* 0x00: func_ov007_020aea6c */
} Ov007ItemNode;

typedef char
    Ov007Work_itemNodes_must_be_at_0xa4[(int)&((Ov007Work *)0)->itemNodes == 0xa4 ? 1 : -1];

/* The extern keeps the tree's char * spelling; the typed view is reached
 * through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* Every 0xf0 ticks of states[1], from tick 0x3c on, re-arms item node 1's
 * 0x3c-frame wiggle; func_ov007_020af260 picks the state requests below.
 * func_ov007_020b8f64 and func_ov007_020b8f50 return the picker scene's
 * states[2] and states[3] (data_ov007_02104ba0[3] and [4]); their externs
 * keep the tree's int return, so the results are cast here. */
void func_ov007_020aefb4(void) {
    int r6 = 0;
    int n = WORK->states[1]->stateTicks - 0x3c;
    if (n >= 0) {
        int rem = cstd::mod(n, 0xf0);
        int quo = cstd::div(n - rem, 0xf0);
        if (rem == 0) {
            Ov007Item *p = WORK->itemNodes[1]->item;
            p->wiggleFrames = 0x3c;
            p->wiggleTimer = p->wiggleFrames;
        }
        int r7 = (WORK->states[0]->previousState == 3) ? 1 : 0;
        if (func_ov007_020b8fb4() != 0 && quo >= r7) {
            int a = -1, b = -1;
            func_ov007_020af260(quo, rem, &a, &b);
            if (a != -1)
                ((Ov007MenuState *)func_ov007_020b8f64())->requestedState = (short)a;
            if (b != -1)
                ((Ov007MenuState *)func_ov007_020b8f50())->requestedState = (short)b;
        } else {
            r6 = 1;
        }
    } else {
        r6 = 1;
    }
    if (r6) {
        ((Ov007MenuState *)func_ov007_020b8f64())->requestedState = 0;
        ((Ov007MenuState *)func_ov007_020b8f50())->requestedState = 0;
    }
    func_ov007_020b9640();
}
}
