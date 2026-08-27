#include "types.h"
extern void func_02031b84(int c);

struct S { u8 b[8]; };
extern struct S *data_0209fd0c;
extern u8 data_0209fcbc;
extern u8 data_0209fc80;
extern u8 data_0209fcd0;
extern u8 data_0209fcd8;
extern u8 data_0209fcf4[];
extern u8 data_0209fca4;

void func_02031154(void) {
    struct S *p = data_0209fd0c;
    u16 v = p->b[4] | (p->b[3] << 8);
    u8 idx;
    switch (v) {
    case 0:
        data_0209fc80 = data_0209fcbc;
        break;
    case 1:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcd8;
        func_02031b84(0x4d);
        data_0209fca4 = 1;
        break;
    case 2:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcbc;
        func_02031b84(0x4d);
        data_0209fca4 = 2;
        break;
    case 3:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcbc;
        func_02031b84(0x4d);
        data_0209fca4 = 3;
        break;
    }
}
