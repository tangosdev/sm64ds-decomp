//cpp
// @symbol _ZN17dMg3DEspAnimSet_c8BehaviorEv
#include "dMg3DEspAnimSet_c.h"

int ApproachLinear2(short& value, short target, short step);

void dMg3DEspAnimSet_c::Behavior()
{
    int activationPending = 0;
    if (unk_178 != 0) {
        if (ApproachLinear2(unk_17a, 0, 1)) {
            unk_17a = unk_17c;
            activationPending = 1;
            if (unk_178 > 0) {
                unk_178 -= 1;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        if (unk_168[i] != 0) {
            mModels[i].Advance();
            mMaterialChangers[i].Advance();
            if (mModels[i].Finished()) {
                unk_168[i] = 0;
            }
        } else if (activationPending == 1) {
            unk_168[i] = 1;
            mModels[i].currFrame = 0;
            activationPending = 0;
            mMaterialChangers[i].currFrame = 0;
        }
    }
}
