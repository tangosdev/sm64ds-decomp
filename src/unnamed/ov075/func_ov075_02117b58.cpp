//cpp
#include "dScene_c.h"

extern "C" {
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int a);
}
extern int data_0208ee44;
extern unsigned char data_02092778;

extern "C" void func_ov075_02117b58(char* c)
{
    *(int*)(((int)c + 0x264)) -= data_0208ee44;
    if (*(int*)(c + 0x264) > 0) return;
    dScene_c::StartSceneFade(1, 0, 0);
    _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
    data_02092778 = 1;
}
