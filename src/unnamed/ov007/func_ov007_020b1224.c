#include "Ov007MenuState.h"
#include "Ov007Work.h"
extern void func_ov007_020b1390(int x);
extern char* func_ov007_020b2cdc(void);
extern void* data_ov007_0210342c;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020ad660, only the slot this file reads. */
typedef struct Ov007Item {
    char pad_00[4];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns; only its item here. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
} Ov007ItemView;

typedef char
    Ov007Work_chosenItemId_must_be_at_0x180[(int)&((Ov007Work *)0)->chosenItemId == 0x180 ? 1 : -1];

void func_ov007_020b1224(void)
{
    Ov007Work *r1 = (Ov007Work *)data_ov007_0210342c;
    if (r1->itemViews[8]->item->state->currentState != 0)
        return;
    switch (r1->chosenItemId) {
    case 9: case 10: case 11: func_ov007_020b1390(0); break;
    case 12: case 13: func_ov007_020b1390(1); break;
    case 8: func_ov007_020b1390(3); break;
    }
    {
        int val = ((Ov007Work *)data_ov007_0210342c)->chosenItemId;
        if (val == -1) return;
        /* func_ov007_020b2cdc returns the menu cursor's state
         * (func_ov007_020b2e64's Ov007Cursor::state) */
        if ((unsigned int)(val - 0xa) <= 1) {
            Ov007MenuState *p = (Ov007MenuState *)func_ov007_020b2cdc();
            p->requestedState = 9;
        } else {
            Ov007MenuState *p = (Ov007MenuState *)func_ov007_020b2cdc();
            p->requestedState = (short)val;
        }
    }
}
