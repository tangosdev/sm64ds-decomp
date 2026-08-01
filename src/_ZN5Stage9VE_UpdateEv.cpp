//cpp
#include "types.h"
extern "C" {
extern u8 data_0209f244;
extern u8 data_0209f22c;
extern u8 data_0209f290;
extern u8 data_0209f2e0;
extern int data_0208ee44;
extern u8 data_ov002_02111150;
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
void _ZN5Stage17UpdateMenuButtonsEb(int b);
int IsButtonInputValid(void);
void func_02012790(unsigned int id);
void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);

void _ZN5Stage9VE_UpdateEv(void)
{
    u8 t = data_0209f244;
    u8 t2;
    u8 s;
    if (t != 0) {
        data_0209f244 = t - data_0208ee44;
        if (data_0209f244 == 0) {
            _ZN5Stage17UpdateMenuButtonsEb(0);
        }
    }
    t2 = data_0209f22c;
    if (t2 != 0) {
        data_0209f22c = t2 - data_0208ee44;
        return;
    }
    s = data_0209f290;
    switch (s) {
    case 0: {
        u8 idx = data_020a0e40[0];
        int ok = 0;
        int off = idx * 4;
        u8 idx2;
        int off2;
        u8 a;
        if (data_020a0de8[idx * 4]) {
            if (data_020a0de9[off]) ok = 1;
        }
        if (ok == 0) {
            if (IsButtonInputValid() == 0) return;
        }
        idx2 = data_020a0e40[0];
        a = data_020a0dea[idx2 * 4];
        off2 = idx2 * 4;
        if (((u8)(a - 8) < 0xf0 && (u8)(data_020a0deb[off2] - 0x38) < 0x20)
            || IsButtonInputValid() != 0) {
            if (data_0209f2e0 == 0) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 0;
            data_0209f22c = data_0208ee44 << 3;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9a);
            if ((u8)(data_020a0dea[data_020a0e40[0] * 4] - 0x28) < 0xb0) return;
            return;
        } else {
            u8 idx3 = data_020a0e40[0];
            int off3 = idx3 * 4;
            if ((u8)(data_020a0dea[idx3 * 4] - 8) >= 0xf0) return;
            if ((u8)(data_020a0deb[off3] - 0x68) >= 0x20) return;
            if (data_0209f2e0 == 1) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9b);
            if ((u8)(data_020a0dea[data_020a0e40[0] * 4] - 8) < 0xf0) return;
            return;
        }
    }
    case 1: {
        if (data_0209f2e0 == 0) {
            _ZN5Scene14StartSceneFadeEjjt(6, 1, 0);
        } else {
            _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
        }
        data_0209f290 = 2;
        data_0209d45c &= ~4;
        data_0209d454 &= ~3;
        *(u16 *)0x04000050 = 0;
        *(u16 *)0x04001050 = 0;
        break;
    }
    }
}
}
