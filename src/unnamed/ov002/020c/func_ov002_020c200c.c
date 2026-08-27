#include "types.h"
extern int func_ov002_020c1e44(void* a, int x);
extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, void* st);
extern int _ZN6Player22IsBeingShotOutOfCannonEv(void* thiz);
extern void func_ov002_020eedc0(void* c, void* arg);
extern void func_ov002_020c1eb4(void* self, int arg);
extern char data_ov002_021105bc;
extern char data_ov002_0211031c;
extern char data_ov002_02110334;
extern char data_ov002_0211055c;
extern char data_ov002_02110214;

int func_ov002_020c200c(char* self, int arg)
{
    if (*(unsigned char*)(self + 0x70e) == 1)
        return func_ov002_020c1e44(self, arg);

    if (*(unsigned char*)(self + 0x709))
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021105bc) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211031c) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110334) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211055c) ||
        _ZN6Player22IsBeingShotOutOfCannonEv(self) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110214))
    {
        if (*(int*)(self + 0x98) <= 0x10000)
            return 0;

        if (_ZN6Player22IsBeingShotOutOfCannonEv(self))
            func_ov002_020eedc0(self + 0x380, self);
    }
    else
    {
        return 0;
    }

    func_ov002_020c1eb4(self, arg);
    return 1;
}
