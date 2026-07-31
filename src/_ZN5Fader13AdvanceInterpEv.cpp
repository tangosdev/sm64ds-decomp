//cpp
// @symbol _ZN5Fader13AdvanceInterpEv
/* recovered: named members + shared header, real C++ method */
#include "Fader.h"
extern "C" {
void func_0203ae58(int* value, int target, int step);
}

void Fader::AdvanceInterp()
{
    int speed = unk_008;
    int target = speed >= 0 ? 0x1000 : 0;
    if (speed < 0)
        speed = -speed;
    func_0203ae58((int*)((char*)&unk_004), target, speed);
}
