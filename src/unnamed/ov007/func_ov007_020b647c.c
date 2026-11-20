#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Item.h"

/* Sends item view `index`'s item to `state`, unless `index` is the item `id`
 * itself (unit020b413c passes the tapped item's id). */
void func_ov007_020b647c(int id, int index, short state) {
    if (id == index)
        return;
    gMenuWork->itemViews[index]->item->state->requestedState = state;
}
