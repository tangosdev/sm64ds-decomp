//cpp
// @symbol _ZN17dMg3DEspAnimSet_c8BehaviorEv
#include "dMg3DEspAnimSet_c.h"

extern "C" {
int _Z15ApproachLinear2Rsss(short* p, short a, short b);
void _ZN9Animation7AdvanceEv(void* a);
int _ZN9Animation8FinishedEv(void* a);
}

void dMg3DEspAnimSet_c::Behavior()
{
    char *self = (char *)this;
    int sb = 0;
    if (*(short*)(self + 0x178) != 0) {
        if (_Z15ApproachLinear2Rsss((short*)(self + 0x17a), 0, 1)) {
            *(short*)(self + 0x17a) = *(short*)(self + 0x17c);
            sb = 1;
            if (*(short*)(self + 0x178) > 0) {
                *(short*)(int)(self + 0x178) -= 1;
            }
        }
    }
    {
        int zero = 0;
        int i;
        char* a = self;
        char* b = self + 0x12c;
        char* cc = self;
        for (i = 0; i < 3; i++) {
            if (((int*)(self + 0x168))[i] != 0) {
                _ZN9Animation7AdvanceEv(a + 0x50);
                _ZN9Animation7AdvanceEv(b);
                if (_ZN9Animation8FinishedEv(a + 0x50)) {
                    ((int*)(self + 0x168))[i] = 0;
                }
            } else {
                if (sb == 1) {
                    ((int*)(self + 0x168))[i] = 1;
                    *(int*)(a + 0x58) = zero;
                    sb = zero;
                    *(int*)(cc + 0x134) = zero;
                }
            }
            a += 0x64;
            b += 0x14;
            cc += 0x14;
        }
    }
}
