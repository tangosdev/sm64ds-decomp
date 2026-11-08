#include "Ov007Transition.h"

extern void func_ov007_020c86c4(Ov007Transition *tr);
extern void func_ov007_020c897c(Ov007Transition *tr);
extern void func_ov007_020c8970(Ov007Transition *tr);
extern void Transition_StartBackward(Ov007Transition *tr, int v);
extern void Transition_StartForward(Ov007Transition *tr, int v);
extern void func_ov007_020c8c88(Ov007Transition *tr, int t);
extern void func_ov007_020c8bc8(Ov007Transition *tr, int t);
extern void func_ov007_020c8b34(Ov007Transition *tr, int t);

/* One frame of a transition: acts on a pending start or stop request, then,
 * while it runs, advances t by step. On reaching 1.0 it stops, holds or
 * restarts by endAction and calls onEnd; after a stop or hold it clears
 * onEnd unless the callback installed a new one. The eased value (curve,
 * flipped when reversed) goes to the effect's applier. */
void Transition_Update(Ov007Transition *tr) {
    int eased = 0;
    void (*onEnd)(void);

    if (tr->request == 1) {
        func_ov007_020c86c4(tr);
    } else if (tr->request == 2) {
        func_ov007_020c897c(tr);
    }

    if (tr->state == 0)
        return;

    if (tr->state == 1) {
        int *progress = &tr->t;
        *progress += tr->step;
        if (tr->t > 0x1000) {
            tr->t = 0x1000;
            switch (tr->endAction) {
            case 0:
                func_ov007_020c8970(tr);
                break;
            case 1:
                tr->state = 3;
                break;
            case 2:
                if (tr->reversed == 1)
                    Transition_StartBackward(tr, tr->screens);
                else
                    Transition_StartForward(tr, tr->screens);
                break;
            case 3:
                if (tr->reversed == 1)
                    Transition_StartForward(tr, tr->screens);
                else
                    Transition_StartBackward(tr, tr->screens);
                break;
            }

            onEnd = tr->onEnd;
            if (onEnd != 0) {
                onEnd();
            }
            if (onEnd == tr->onEnd && (u32)tr->endAction <= 1)
                tr->onEnd = 0;
        }
    }

    if (tr->state == 0)
        return;

    switch (tr->curve) {
    case 0:
        eased = tr->t;
        break;
    case 1:
        eased = tr->t << 1;
        if (eased > 0x1000)
            eased = -(eased - 0x2000);
        break;
    case 2:
        eased = (int)(((long long)tr->t * tr->t) >> 12);
        break;
    case 3:
        break;
    }

    if (tr->reversed == 1)
        eased = 0x1000 - eased;

    switch (tr->effect) {
    case 1:
        func_ov007_020c8c88(tr, eased);
        break;
    case 2:
    case 3:
        func_ov007_020c8bc8(tr, eased);
        break;
    case 4:
        func_ov007_020c8b34(tr, eased);
        break;
    default:
        break;
    }
}
