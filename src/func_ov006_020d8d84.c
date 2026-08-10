#include "types.h"
extern void func_ov006_020d836c(char *c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int i);
extern void func_ov006_020d634c(char *c, int i);

void func_ov006_020d8d84(char *self)
{
    int flag;
    int count;
    int i;
    u8 (*arr)[0x40] = (u8 (*)[0x40])self;
    int (*iarr)[0x10] = (int (*)[0x10])self;

    func_ov006_020d836c(self);

    if (*(u16 *)(self + 0x62e8) != 0) {
        (*(u16 *)(self + 0x62e8))--;
        if (*(s16 *)(self + 0x62e8) < 0)
            *(s16 *)(self + 0x62e8) = 0;
        return;
    }

    if (*(u8 *)(self + 0x62f8) != 0) {
        flag = 0;
        count = 0;
        i = 0;
        do {
            if (arr[i][0x4698] != 0 && arr[i][0x4697] != 6) {
                arr[i][0x4697] = 6;
                arr[i][0x469b] = 0;
                iarr[i][0x119c] = 0x4000;
                if (arr[i][0x4696] != 0)
                    flag = 1;
                count++;
            }
            i++;
        } while (i < 0x70);
        if (count != 0) {
            _ZN5Sound12PlayBank2_2DEj(0x1e6);
            if (*(u8 *)(self + 0x62f8) == 1) {
                func_ov006_020d634c(self, 0);
                func_ov006_020d634c(self, 1);
            } else {
                func_ov006_020d634c(self, flag);
            }
        }
    } else {
        i = 0;
        do {
            if (arr[i][0x4698] != 0 && arr[i][0x4697] == 5) {
                if (*(u8 *)(self + 0x62f5) == arr[i][0x4696]) {
                    arr[i][0x4697] = 6;
                    arr[i][0x469b] = 0;
                    iarr[i][0x119c] = 0x4000;
                }
            }
            i++;
        } while (i < 0x70);
        _ZN5Sound12PlayBank2_2DEj(0x1e6);
        func_ov006_020d634c(self, *(u8 *)(self + 0x62f5));
    }
    *(int *)(self + 0x62d4) = 1;
}
