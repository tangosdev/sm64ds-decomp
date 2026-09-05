//cpp
// @symbol _ZN12daStarGate_c17St_OpenClose_InitEP6Player
#include "daStarGate_c.h"

extern "C" {
int ShowArea(int areaID);
extern int data_0209f318[];
int _ZN6Camera9SetFlag_3Ev(void *camera);
int func_02012694(int soundID, void *position);
}

bool daStarGate_c::St_OpenClose_Init(Player *)
{
    ShowArea((s8)mAngleX);
    ShowArea((s8)mAngleZ);
    _ZN6Camera9SetFlag_3Ev((void *)data_0209f318[0]);
    mTerminalVelocity = -0x8a000;
    func_02012694(0x4e, &mCamSpacePosX);
    return true;
}
