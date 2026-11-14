#include "Ov007MenuState.h"
#include "Ov007Work.h"

#define func_ov007_020b1f2c _Z19func_ov007_020b1f2ciPi
extern int func_ov007_020b1f2c(int x, int *out);

/* Eases modelGrowth over states[1]'s ticks (func_ov007_020b1f2c). */
int func_ov007_020b1b4c(void) {
    Ov007Work *w = WORK;
    return func_ov007_020b1f2c(w->states[1]->stateTicks, &w->modelGrowth);
}
