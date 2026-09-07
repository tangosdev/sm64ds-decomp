//cpp
// @symbol _ZN11PyramidStep8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidStep.h"
extern "C" {
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* p, int a, int b);
}

int PyramidStep::Behavior()
{
    char* c = (char*)((void*)this);
    switch (*(unsigned char*)(c + 0x372)) {
    case 0:
        if (*(unsigned short*)(c + 0x370) == 0x64) {
            *(unsigned char*)(c + 0x372) = 1;
            *(int*)(c + 0xa8) = 0x5000;
            *(unsigned short*)(c + 0x370) = 0;
        }
        break;
    case 1:
        if (*(unsigned short*)(c + 0x370) == 0x64) {
            *(unsigned char*)(c + 0x372) = 0;
            *(int*)(c + 0xa8) = -0x5000;
            *(unsigned short*)(c + 0x370) = 0;
        }
        break;
    }
    {
        unsigned short* p370 = (unsigned short*)(((int)c + 0x370));
        int* p60 = (int*)(((int)c + 0x60));
        *p370 = (unsigned short)(*p370 + 1);
        *p60 = *p60 + *(int*)(c + 0xa8);
    }
    func_ov025_02111e30(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        func_ov025_02111dec(c);
    return 1;
}
