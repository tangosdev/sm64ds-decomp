#include "types.h"
extern int _ZN6Player7IsStateERNS_5StateE(char* self, void* st);
extern int _ZN6Player7IsInAirEv(char* self);
extern int func_ov002_020e3078(char* self, void* s);
extern int func_ov002_020c5dec(char* c, int arg1);

extern char data_ov002_02110124;
extern char data_ov002_02110154;
extern char data_ov002_0211049c;
extern char data_ov002_02110424;
extern char data_ov002_021104cc;

int func_ov002_020bfec0(char* self)
{
    int state = *(int*)(self + 0x664);
    int inc;

    if (state == 1) {
        if (*(u8*)(self + 0x6f9) != 0) state = 8;
    }

    if (state < 6 || state > 9) {
        if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110124)) {
            *(int*)(self + 0x68c) = 0;
        }
        return 0;
    }

    if (*(u8*)(self + 0x6de) != 0 || _ZN6Player7IsInAirEv(self) ||
        *(int*)(self + 0x37c) != 0 || *(u8*)(self + 0x703) != 0 ||
        *(u8*)(self + 0x709) != 0) {
        return 0;
    }

    *(u8*)(self + 0x6eb) |= 1;

    {
        int flag = *(int*)(self + 0x354);
        flag = (flag != 0);
        if (flag) goto reset_timer;
        if (*(u16*)(self + 0x6bc) == 0) goto after_timer_check;
    reset_timer:
        *(int*)(self + 0x68c) = 0;
        return 0;
    after_timer_check:
        ;
    }

    inc = 0x400;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211049c)) {
        inc = 0x800;
    }

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110424)) {
        if (func_ov002_020e3078(self, &data_ov002_021104cc) == 0) {
            *(u16*)(self + 0x6c4) = 2;
        }
    }

    if (*(int*)(self + 0x68c) < 0x1100) *(int*)(self + 0x68c) = 0x1100;

    switch (state) {
    case 6:
        *(int*)(self + 0x68c) += inc;
        if (*(int*)(self + 0x68c) >= 0xa000) *(int*)(self + 0x68c) = 0xa000;
        goto ret0;
    case 7:
        *(int*)(self + 0x68c) += inc;
        if (*(int*)(self + 0x68c) >= 0x3c000) *(int*)(self + 0x68c) = 0x3c000;
        goto ret0;
    case 8:
        *(int*)(self + 0x68c) += inc;
        if (*(int*)(self + 0x68c) < 0xa0000) goto ret0;
        /* fallthrough */
    case 9:
        func_ov002_020c5dec(self, 3);
        return 1;
    default:
        *(int*)(self + 0x68c) = 0;
        goto ret0;
    }
ret0:
    return 0;
}
