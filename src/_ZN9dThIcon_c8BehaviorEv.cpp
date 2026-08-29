//cpp
// @symbol _ZN9dThIcon_c8BehaviorEv
#include "dThIcon_c.h"

extern int data_0208ee44;

void dThIcon_c::Behavior()
{
    int v;

    v = unk_00c;
    if (v <= 0) {
        return;
    }
    unk_00c -= data_0208ee44;
    v = unk_00c;
    if (v > 0) {
        return;
    }
    if (unk_010 != 0) {
        unk_010 = 0;
    } else {
        unk_010 = 1;
    }
    if (unk_014 >= 1) {
        unk_011 = 1;
    }
}
