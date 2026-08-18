//cpp
// @symbol _ZN3Amp8BehaviorEv
#include "Amp.h"

extern "C" {
extern void func_ov070_02120d34(void *c);
extern void func_ov070_02120724(void *c);
extern int data_ov070_0212365c[];
}

int Amp::Behavior()
{
    func_ov070_02120d34(this);
    mCylinderOffset.y += data_ov070_0212365c[1];
    mMovingCylinderClsnWithPos.SetPosRelativeToActor(mCylinderOffset);
    mMovingCylinderClsnWithPos.Clear();
    mMovingCylinderClsnWithPos.Update();
    func_ov070_02120724(this);
    return 1;
}
