#include "types.h"
extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, void* st);
extern int AngleDiff(int a, int b);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, void* st);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, u32 a, int b, Fix12i c, u32 d);
extern char data_ov002_02110334;
extern char data_ov002_0211031c;

void func_ov002_020c0364(char* c, u32 arg)
{
    int flag;

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110334)) return;
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211031c)) return;

    flag = 0;
    if (arg == 3) {
        if (AngleDiff(*(s16*)(c + 0x69a), *(s16*)(c + 0x8e)) < 0x4000)
            flag = 1;
    } else {
        if (arg & 1)
            flag = 1;
    }

    if (flag) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110334);
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211031c);
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x43, 0x40000000, 0x1000, 0);
    }
}
