#include "types.h"
extern void func_0203f650(int x);
extern void func_0203f8a4(void);
extern void func_0203f714(void);
extern void func_0203f6a4(int v);
extern s16 data_020a0f18;
extern int data_020a0f90;

void func_0203f920(u16 *o)
{
    switch (o[0]) {
    case 0:
        if (o[1] != 0)
            func_0203f650(-2);
        else
            func_0203f8a4();
        break;
    case 0x1e:
        func_0203f714();
        break;
    case 2:
        func_0203f6a4(3);
        func_0203f650(data_020a0f18);
        data_020a0f90 = 0;
        break;
    default:
        func_0203f650(data_020a0f18);
        break;
    }
}
