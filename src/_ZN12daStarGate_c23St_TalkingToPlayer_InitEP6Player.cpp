//cpp
// @symbol _ZN12daStarGate_c23St_TalkingToPlayer_InitEP6Player
#include "daStarGate_c.h"

extern "C" void func_02012694(int soundID, void *position);
extern "C" void func_ov100_02145070(int value);

bool daStarGate_c::St_TalkingToPlayer_Init(Player *)
{
    func_02012694(0xb8, &mCamSpacePosX);
    func_ov100_02145070(0x87);
    return true;
}
