#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Item.h"
#define PickerScene_GetMainState _Z24PickerScene_GetMainStatev
#define PickerScene_Update _Z18PickerScene_Updatev

/* Returns the picker scene's states[0] (unit020b8f50). */
extern Ov007MenuState *PickerScene_GetMainState(void);
extern void func_ov007_020c1d78(int i);
extern void PickerScene_Update(void);

/* Steps the picker alongside states[1]: in state 0 it starts the picker
 * (state 1) and leaves once the picker reaches state 2; in state 1 it moves
 * the picker to state 2 and, once the picker reaches state 3, sends item view
 * 3's item to state 2. Then PickerScene_Update. */
void func_ov007_020af12c(void) {
    Ov007MenuState *s1 = WORK->states[1];
    int state = s1->currentState;
    int ticks = s1->stateTicks;
    Ov007MenuState *picker;

    switch (state) {
    case 0:
        if (ticks == 0) {
            picker = PickerScene_GetMainState();
            picker->requestedState = 1;
        }
        picker = PickerScene_GetMainState();
        if (picker->currentState == 2)
            WORK->states[1]->requestedState = 1;
        break;
    case 1:
        if (ticks == 0) {
            picker = PickerScene_GetMainState();
            picker->requestedState = 2;
            func_ov007_020c1d78(0);
        }
        picker = PickerScene_GetMainState();
        if (picker->currentState == 3)
            WORK->itemViews[3]->item->state->requestedState = 2;
        break;
    }
    PickerScene_Update();
}
