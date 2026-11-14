#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern int func_ov007_020b1f2c(int x, int *out);

/* Eases modelGrowth over states[1]'s ticks (func_ov007_020b1f2c). */
int func_ov007_020b1b4c(void) {
    Ov007Work *w = WORK;
    return func_ov007_020b1f2c(w->states[1]->stateTicks, &w->modelGrowth);
}
