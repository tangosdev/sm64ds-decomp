#include "Ov007Transition.h"

#define func_ov007_020c86c4 _Z19func_ov007_020c86c4P15Ov007Transition
extern void func_ov007_020c86c4(Ov007Transition *tr);

/* Starts the transition forwards on screens v (Transition_StartBackward is the
 * backwards twin). */
void Transition_StartForward(Ov007Transition *tr, int v) {
    tr->reversed = 0;
    tr->screens = v;
    tr->t = 0;
    func_ov007_020c86c4(tr);
}
