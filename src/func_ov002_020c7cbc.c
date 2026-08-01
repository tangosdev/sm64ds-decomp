#include "types.h"
extern u8 data_0209f2fc;
extern signed char data_02092124;
extern void LoadKeyModels(int idx);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, u32 a, int b, Fix12i c, u32 d);

int func_ov002_020c7cbc(char* self)
{
    if (data_0209f2fc == 1) {
        switch (data_02092124) {
        case 0x24: *(u8*)(self + 0x719) = 0; break;
        case 0x26: *(u8*)(self + 0x719) = 1; break;
        case 0x2d: *(u8*)(self + 0x719) = 2; break;
        case 0x2f: *(u8*)(self + 0x719) = 3; break;
        case 0x31: *(u8*)(self + 0x719) = 4; break;
        }

        if (*(signed char*)(self + 0x719) >= 0) {
            LoadKeyModels(*(signed char*)(self + 0x719));
            *(u8*)(((long long)(int)(self + 0x718))) |= 0x10;
            _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x53, 0x40000000, 0x1000, 0);
            return 1;
        }
    }
    return 0;
}
