#include "Ov007Transition.h"

extern void func_ov007_020c86c4(Ov007Transition *tr);

/* Starts the transition backwards on screens v (Transition_StartForward is the
 * forwards twin). */
void Transition_StartBackward(Ov007Transition *tr, int v) {
    tr->reversed = 1;
    tr->screens = v;
    tr->t = 0;
    func_ov007_020c86c4(tr);
}
