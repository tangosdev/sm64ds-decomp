//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Item.h"
#include "nitro/fx/fx_cp.h"

extern int PickerScene_IsIdle(void);
extern void func_ov007_020af260(int, int, int *, int *);
extern Ov007MenuState *PickerScene_GetScaleAnimState(void);
extern Ov007MenuState *PickerScene_GetMoveAnimState(void);
extern "C" void PickerScene_Update(void);

typedef char
    Ov007Work_itemNodes_must_be_at_0xa4[(int)&((Ov007Work *)0)->itemNodes == 0xa4 ? 1 : -1];

/* Every 0xf0 ticks of states[1], from tick 0x3c on, re-arms item node 1's
 * 0x3c-frame wiggle; func_ov007_020af260 picks the state requests below.
 * PickerScene_GetScaleAnimState and PickerScene_GetMoveAnimState return the picker scene's
 * states[2] and states[3] (data_ov007_02104ba0[3] and [4]); their externs
 * keep the tree's int return, so the results are cast here. */
void func_ov007_020aefb4(void) {
    int r6 = 0;
    int n = WORK->states[1]->stateTicks - 0x3c;
    if (n >= 0) {
        int rem = FX_ModS32(n, 0xf0);
        int quo = FX_DivS32(n - rem, 0xf0);
        if (rem == 0) {
            Ov007Item *p = WORK->itemNodes[1]->item;
            p->wiggleFrames = 0x3c;
            p->wiggleTimer = p->wiggleFrames;
        }
        int r7 = (WORK->states[0]->previousState == 3) ? 1 : 0;
        if (PickerScene_IsIdle() != 0 && quo >= r7) {
            int a = -1, b = -1;
            func_ov007_020af260(quo, rem, &a, &b);
            if (a != -1)
                PickerScene_GetScaleAnimState()->requestedState = (short)a;
            if (b != -1)
                PickerScene_GetMoveAnimState()->requestedState = (short)b;
        } else {
            r6 = 1;
        }
    } else {
        r6 = 1;
    }
    if (r6) {
        PickerScene_GetScaleAnimState()->requestedState = 0;
        PickerScene_GetMoveAnimState()->requestedState = 0;
    }
    PickerScene_Update();
}
