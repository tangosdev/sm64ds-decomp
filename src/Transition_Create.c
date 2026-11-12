#include "Ov007Transition.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c8ac0(Ov007Transition *tr);

/* Allocates a transition and clears it (func_ov007_020c8ac0). */
Ov007Transition *Transition_Create(void) {
    Ov007Transition *tr = func_ov007_020c3df4(0, sizeof(Ov007Transition));
    func_ov007_020c8ac0(tr);
    return tr;
}
