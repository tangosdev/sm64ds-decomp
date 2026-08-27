#include "types.h"
extern void func_ov007_020c0638(int a, int b, int c, int d);
extern void func_ov007_020b7d94(int kind);
extern void func_ov007_020b2614(int a, int b, int c);
extern void func_ov007_020c1d8c(int i);
extern int func_ov007_020b79c8(void);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020b7a44(void);
extern int func_ov007_020b79e4(void);
extern void func_ov007_020aec94(int a);

extern char* data_ov007_0210342c;

void func_ov007_020b0834(void)
{
    char* G = data_ov007_0210342c;

    if (*(int*)(*(int*)(G + 8) + 0xc) == 0) {
        func_ov007_020c0638(*(int*)(*(int*)(G + 0xe0) + 4), 0, 0, 0);
        G = data_ov007_0210342c;
        func_ov007_020b7d94(*(unsigned char*)(*(int*)(G + 0x28) + 0x39));
        func_ov007_020b2614(2, 0, 8);
        func_ov007_020c1d8c(0);
    }

    G = data_ov007_0210342c;
    switch (*(int*)(*(int*)(G + 8) + 4)) {
    case 0:
        if (func_ov007_020b79c8() == 0)
            return;
        G = data_ov007_0210342c;
        {
            char* p54 = *(char**)(G + 0x54);
            if ((*(u16*)p54 & ~*(u16*)(p54 + 2)) == 0) {
                char* p50 = *(char**)(G + 0x50);
                if (*(u16*)(p50 + 0xc) == 0)
                    return;
                if (*(u16*)(p50 + 0x14) != 0)
                    return;
                if ((unsigned)*(int*)(p50 + 0x24) < 1)
                    return;
            }
        }
        {
            char* pB = *(char**)(G + 0x28);
            int r5 = (*(unsigned char*)(pB + 0x37) == 1);
            int r4 = (*(unsigned char*)(pB + 0x38) == 1);
            func_ov007_020bdeb0(0xc);
            if (r5) {
                G = data_ov007_0210342c;
                if (*(int*)(G + 0x10) == 9) {
                    int v = (*(int (**)(int))(G + 0x2c))(0x16);
                    G = data_ov007_0210342c;
                    *(unsigned char*)(*(int*)(G + 0x28) + 0x37) = v;
                }
            }
            if (r4) {
                G = data_ov007_0210342c;
                {
                    int v = (*(int (**)(int))(G + 0x2c))(0x17);
                    G = data_ov007_0210342c;
                    *(unsigned char*)(*(int*)(G + 0x28) + 0x38) = v;
                }
            }
            func_ov007_020b7a44();
            G = data_ov007_0210342c;
            *(int*)(*(int*)(G + 8) + 4) = 1;
        }
        return;
    case 1:
        if (func_ov007_020b79e4() != 0)
            return;
        G = data_ov007_0210342c;
        func_ov007_020aec94(*(int*)(G + 0x10));
        return;
    }
}
