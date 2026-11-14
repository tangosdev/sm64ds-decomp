#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Item.h"

extern void func_ov007_020c1d8c(int i);

void func_ov007_020b63e4(Ov007ItemView *view);

/* Chooses the item `view` shows (unit020b413c calls it on a tap): items 10
 * and 11 also send item view 9's item to state 2, item 9 sends item view
 * 11's. The item itself goes to state 2 and its id becomes chosenItemId. */
void func_ov007_020b63e4(Ov007ItemView *view) {
    int id = view->item->id;
    if (id == 10 || id == 11) {
        WORK->itemViews[9]->item->state->requestedState = 2;
    } else if (id == 9) {
        WORK->itemViews[11]->item->state->requestedState = 2;
    }
    func_ov007_020c1d8c(0);
    view->item->state->requestedState = 2;
    WORK->chosenItemId = view->item->id;
}
