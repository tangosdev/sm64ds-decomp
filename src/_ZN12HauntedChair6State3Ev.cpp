//cpp
// @symbol _ZN12HauntedChair6State3Ev
#include "HauntedChair.h"

extern "C" {
extern short data_02082214[];
extern int AngleDiff(int a, int b);
}

void HauntedChair::State3()
{
    char* c = (char*)this;
    int* p3a4 = *(int**)(c + 0x3a4);
    ApproachStateValue((short *)p3a4, (short *)(c + 0x398),
                       *(short*)(c + 0x3a2), 0xfa0, 0x14, 2);
    {
        int a = AngleDiff(*(short*)*(int**)(c + 0x3a4), 0);
        int* ip = *(int**)(c + 0x3a4);
        short t = data_02082214[((unsigned short)*(unsigned short*)ip >> 4) * 2];
        short sa = (short)a;
        int m = (short)t * (short)0x50;
        if (ip != (int*)(c + 0x8c)) {
            *(int*)(c + 0x38c) = -m;
            *(int*)(c + 0x390) = 0;
            *(int*)(c + 0x394) = 0;
        } else {
            *(int*)(c + 0x38c) = 0;
            *(int*)(c + 0x390) = 0;
            *(int*)(c + 0x394) = m;
            goto cont;
        }
cont: ;
        if (sa >= 0x4000) {
            short d = (short)(sa - 0x4000);
            short t2 = data_02082214[((unsigned short)d >> 4) * 2];
            *(int*)(c + 0x60) = (short)t2 * (short)0x28 + (*(int*)(c + 0x384) + 0x28000);
        } else {
            short t3 = data_02082214[((unsigned short)sa >> 4) * 2];
            *(int*)(c + 0x60) = (short)t3 * (short)0x28 + *(int*)(c + 0x384);
        }
    }
}
