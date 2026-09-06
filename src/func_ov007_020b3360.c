#include "types.h"
extern void func_ov007_020b3c54(void);
extern int func_ov007_020b3708(void);
extern void func_ov007_020c2090(char* c);
extern void func_ov007_020bdeb0(int a);

extern char* data_ov007_02103360;
extern char* data_ov007_0210342c;

void func_ov007_020b3360(void)
{
    char* p0 = *(char**)data_ov007_02103360;

    if (*(int*)(p0 + 0xc) == 0) {
        if (*(s16*)(p0 + 0xa) == 0xff) {
            *(s16*)(data_ov007_02103360 + 0xc) = 0;
            *(s16*)(data_ov007_02103360 + 0xe) = 1;
        }
        func_ov007_020b3c54();
    }

    {
        int state1 = *(s16*)*(char**)(data_ov007_0210342c + 4);

        switch (state1) {
        case 0: {
            char* s2 = *(char**)(data_ov007_0210342c + 8);
            int state2 = *(s16*)s2;
            switch (state2) {
            case 0:
                if (*(s16*)(s2 + 2) == -1)
                    *(s16*)(*(char**)data_ov007_02103360 + 2) = func_ov007_020b3708();
                else
                    *(s16*)(*(char**)data_ov007_02103360 + 2) = 0xff;
                break;
            case 8:
                if (*(u32*)(s2 + 0xc) <= 1) {
                    {
                        char *o = *(char**)(data_ov007_0210342c + 0x54);
                        *(int*)(o + 0x64) = *(const int*)(o + 0x64) | 1;
                    }
                    func_ov007_020c2090(*(char**)(data_ov007_0210342c + 0x54));
                }
                if (*(s16*)(*(char**)(data_ov007_0210342c + 8) + 2) == -1)
                    *(s16*)(*(char**)data_ov007_02103360 + 2) = func_ov007_020b3708();
                else
                    *(s16*)(*(char**)data_ov007_02103360 + 2) = 0xff;
                break;
            default:
                *(s16*)(*(char**)data_ov007_02103360 + 2) = 0xff;
                break;
            }
            break;
        }
        case 5: {
            char* s2 = *(char**)(data_ov007_0210342c + 8);
            if (*(int*)(s2 + 0xc) == 0) {
                int state3 = *(s16*)s2;
                switch (state3) {
                case 0: {
                    char* p = *(char**)data_ov007_02103360;
                    int cur = *(s16*)p;
                    if ((u16)(s16)(cur - 0x11) <= 2)
                        *(s16*)(p + 2) = 0x15;
                    break;
                }
                case 1: {
                    char* p = *(char**)data_ov007_02103360;
                    if (*(s16*)p == 0x15)
                        *(s16*)(p + 2) = 0x11;
                    break;
                }
                case 3:
                case 7:
                case 9: {
                    char* p = *(char**)data_ov007_02103360;
                    int cur = *(s16*)p;
                    if ((u16)(s16)(cur - 0x11) <= 2)
                        *(s16*)(p + 2) = 0xe;
                    break;
                }
                case 0xb:
                case 0xc:
                    *(s16*)(*(char**)data_ov007_02103360 + 2) = 0xff;
                    return;
                default:
                    break;
                }
            }

            {
                int a;
                int b;
                char* p;
                int cur;
                *(s16*)(*(char**)data_ov007_02103360 + 2) = func_ov007_020b3708();
                p = *(char**)data_ov007_02103360;
                cur = *(s16*)p;
                a = 1;
                if (cur != 0x16 && cur != 0x17)
                    a = 0;
                b = (*(s16*)*(char**)(*(char**)*(char**)(data_ov007_0210342c + 0x16c) + 4) == 0);
                if (a == 0 && b != 0) {
                    *(s16*)(p + 2) = 0x17;
                    if (*(s16*)*(char**)data_ov007_02103360 == 0xff)
                        func_ov007_020bdeb0(3);
                    *(int*)(data_ov007_02103360 + 0x18) = *(s16*)*(char**)data_ov007_02103360;
                    *(s16*)(data_ov007_02103360 + 0xc) = 1;
                    *(s16*)(data_ov007_02103360 + 0xe) = 0;
                } else if (a != 0 && b == 0) {
                    int old = *(int*)(data_ov007_02103360 + 0x18);
                    *(int*)(data_ov007_02103360 + 0x18) = -1;
                    *(s16*)(*(char**)data_ov007_02103360 + 2) = old;
                    *(s16*)(data_ov007_02103360 + 0xc) = 0;
                    *(s16*)(data_ov007_02103360 + 0xe) = 1;
                }
            }
            break;
        }
        default:
            *(s16*)(*(char**)data_ov007_02103360 + 2) = 0xff;
            break;
        }
    }

    {
        char* p = *(char**)data_ov007_02103360;
        int cur = *(s16*)p;
        if ((u16)(s16)(cur - 0xe) <= 2)
            *(int*)(data_ov007_02103360 + 0x18) = cur;
    }
}
