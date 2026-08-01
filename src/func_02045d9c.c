#include "types.h"
// func_02045d9c - Display/3D init: enable DTCM, configure DISPCNT, call G3X setup
extern void _ZN4CP1510EnableDTCMEv(void);
extern void _ZN3G3X13SetClearColorEtiiib(u16 color, int r, int g, int b, int alpha);
extern u16 func_02053ed0(void);
extern u16 func_02053ec0(void);
extern void Crash(void);

void func_02045d9c(void) {
    _ZN4CP1510EnableDTCMEv();
    u16 r3 = *(volatile u16*)0x4000060;
    u16 r1 = (u16)0x4000340;
    int r2 = 0x7fff;
    r3 = (r3 & ~0x3000u) | 4;
    *(volatile u16*)0x4000060 = r3;
    *(volatile u16*)0x4000340 = 0;
    u16 ip = *(volatile u16*)0x4000060;
    ip = (ip & ~0x3000u) | 8;
    *(volatile u16*)0x4000060 = ip;
    _ZN3G3X13SetClearColorEtiiib(0, 0, r2, 0, 0);
    if (func_02053ed0() == 0) {
        Crash();
    }
    if (func_02053ec0() != 0) {
        return;
    }
    Crash();
}
