//cpp
#include "CP15.h"

extern "C" {

extern volatile int data_020a648c;
extern volatile int data_020a649c;
extern int data_020a64a0;
extern int data_020a6760;
extern volatile int data_020a64a8[];
extern int data_020a64a4;
extern int data_020a6490;

extern int IPCSend(unsigned int a, unsigned int c, unsigned int b);
extern int func_0205b274(int a);

int func_0205b070(int x)
{
    if (data_020a648c == 0) return 1;
    if (data_020a64a0 >= 8) {
        if (x == 0) return 0;
        do {
            func_0205b274(1);
        } while (data_020a64a0 >= 8);
    }
    CP15::FlushAndInvalidateDataCache((unsigned int)&data_020a6760, 0x1800);
    if (IPCSend(7, data_020a648c, 0) < 0) {
        if (x == 0) return 0;
        while (IPCSend(7, data_020a648c, 0) < 0)
            ;
    }
    int temp = data_020a648c;
    int idx = data_020a649c;
    data_020a64a8[idx] = temp;
    idx++;
    data_020a649c = idx;
    if (idx > 8) data_020a649c = 0;

    data_020a648c = 0;
    data_020a6490 = 0;
    data_020a64a0 = data_020a64a0 + 1;
    data_020a64a4 = data_020a64a4 + 1;
    return 1;
}
}
