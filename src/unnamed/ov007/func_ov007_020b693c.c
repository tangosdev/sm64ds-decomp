#include "Ov007Work.h"

extern struct Ov007ItemView *ItemView_Create(int i);

/* Builds the 0x18 item views. */
void func_ov007_020b693c(void) {
    int i;
    for (i = 0; i < 0x18; i++) {
        WORK->itemViews[i] = ItemView_Create(i);
    }
}
