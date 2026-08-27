//cpp
struct Vector3;
#include "Butterfly.h"
extern "C" {
int _ZN8dActor_c15IsPlayerInRangeEi(void* c, int n);
void* _ZN8dActor_c13ClosestPlayerEv(void* c);
short Vec3_HorzAngle(const void* a, const void* b);
}
void Butterfly::State1(){
    char* c = (char*)this;
    if(_ZN8dActor_c15IsPlayerInRangeEi(c, 0x3e8) == 0) return;
    *(int*)(c+0x3e4) = 2;
    void* p = _ZN8dActor_c13ClosestPlayerEv(c);
    *(short*)(c+0x94) = Vec3_HorzAngle(c+0x5c, (char*)p+0x5c);
}
