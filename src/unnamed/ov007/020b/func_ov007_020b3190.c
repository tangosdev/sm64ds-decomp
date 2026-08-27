#include "types.h"
extern int func_ov007_020c1da0(int i);
extern void func_ov007_020bdeb0(int i);

extern char* data_ov007_0210342c;
extern char* data_ov007_02103360;

void func_ov007_020b3190(void)
{
    char* g = data_ov007_0210342c;
    int state1 = *(s16*)*(char**)(g + 4);

    switch (state1) {
    case 0: {
        int state2 = *(s16*)*(char**)(g + 8);
        switch (state2) {
        case 0: {
            int r5 = 0;
            int r4 = 0;
            if (func_ov007_020c1da0(0) == 0) {
                char* p = *(char**)(data_ov007_0210342c + 0x54);
                if (*(u16*)p & ~*(u16*)(p + 2)) r4 = 1;
            }
            if (r4) {
                char* p2 = *(char**)(data_ov007_0210342c + 0x54);
                if ((*(u16*)p2 & 2) == 0) r5 = 1;
            }
            if (r5 == 0) return;
            *(s16*)(*(char**)data_ov007_02103360 + 2) = 0;
            func_ov007_020bdeb0(3);
            return;
        }
        case 8:
            *(s16*)(*(char**)data_ov007_02103360 + 2) = 9;
            return;
        }
        return;
    }
    case 5: {
        int r5 = 0;
        int r4 = 0;
        int st;
        if (func_ov007_020c1da0(0) == 0) {
            char* p = *(char**)(data_ov007_0210342c + 0x54);
            if (*(u16*)p & ~*(u16*)(p + 2)) r4 = 1;
        }
        if (r4) {
            char* p2 = *(char**)(data_ov007_0210342c + 0x54);
            if ((*(u16*)p2 & 2) == 0) r5 = 1;
        }
        if (r5 == 0) return;
        st = *(s16*)*(char**)(data_ov007_0210342c + 8);
        if (st == 0xc) return;
        if (st == 0xb) return;
        *(s16*)(*(char**)data_ov007_02103360 + 2) = 0xe;
        func_ov007_020bdeb0(3);
        return;
    }
    }
}
