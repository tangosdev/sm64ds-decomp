#include "types.h"
enum { false, true };

extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern u8 data_020a0e40;
extern u8 data_0209f49e[];
extern int data_0209f318;

extern char data_ov002_0211013c;
extern char data_ov002_0211043c;
extern char data_ov002_02110154;
extern char data_ov002_0211064c;
extern char data_ov002_02110664;

extern int func_0200d0ac(int a, unsigned int b);
extern int _ZN6Player7IsStateERNS_5StateE(char* c, char* s);
extern unsigned int func_02012790(unsigned int a);
extern void func_0200d064(int a, unsigned int b);

void func_ov002_020ca940(char* self)
{
    u8 old;
    int h;
    int b;

    if (*(u8*)(self + 0x709) != 0) return;

    b = data_0209f2d8;
    b = b == 1;
    if (b == false) {
        if (!(data_0209caa0[2] & 0x80)) return;
    }

    if (*(u16*)(data_0209f49e + (u8)data_020a0e40 * 0x18) & 0x8000) {
        old = *(u8*)(self + 0x715);
        h = data_0209f318;
        (*(u8*)((int)self + 0x715))++;

        if (*(u8*)(self + 0x715) > 2) {
            *(u8*)(self + 0x715) = 0;
        }

        switch (*(u8*)(self + 0x715)) {
        case 1:
            if (func_0200d0ac(h, *(u8*)(self + 0x6d8)) != 0) break;
            *(u8*)(self + 0x715) = 2;
        case 2:
            if (*(u8*)(self + 0x703) == 0) {
                b = data_0209f2d8;
                b = b == 1;
                if (b == false) {
                    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c) ||
                        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c) ||
                        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154)) {
                        *(u16*)((int)self + 0x6ce) |= 4;
                        break;
                    }
                }
            }
            if (old == 0) {
                func_02012790(0xe);
                *(u8*)(self + 0x715) = old;
                break;
            }
            *(u8*)(self + 0x715) = 0;
        case 0:
            func_0200d064(h, *(u8*)(self + 0x6d8));
            if ((u16)(*(u16*)(self + 0x6ce) & 4) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211064c)) {
                *(u8*)(self + 0x715) = 2;
            }
            break;
        default:
            *(u8*)(self + 0x715) = 0;
            func_0200d064(h, *(u8*)(self + 0x6d8));
            break;
        }
    }

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211064c)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110664)) return;

    if ((u16)(*(u16*)(self + 0x6ce) & 4)) {
        *(u16*)((int)self + 0x6ce) &= ~4;
    }
}
