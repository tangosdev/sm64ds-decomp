#include "types.h"
extern void func_ov006_020c6f8c(int);
extern void func_ov006_020c8658(void *c);
extern void func_ov006_020c8a9c(int a0, int a1);

extern u32 data_ov006_02140414;
extern u32 data_ov006_02140410;
extern int data_ov006_02140418;
extern int data_ov006_02140428;
extern char *data_ov006_02140420;
extern int data_ov006_02140434;

void func_ov006_020c719c(u32 a0, u32 a1) {
    int i;
    int x;
    data_ov006_02140410 = a0;
    data_ov006_02140414 = a1;
    if (a0 < 5) {
        x = a0 * 0x140 + 0x1000;
    } else {
        x = (a0 % 5 + a0 / 5) * 0x280 + 0x1000;
    }
    func_ov006_020c6f8c(x);
    if (data_ov006_02140410 % 5 == 0) {
        data_ov006_02140428 = data_ov006_02140418;
    }
    for (i = 0; i < data_ov006_02140418; i++) {
        func_ov006_020c8658(data_ov006_02140420 + i * 0xb8);
    }
    func_ov006_020c8a9c(data_ov006_02140418 - data_ov006_02140428, 0);
    data_ov006_02140434 = 0;
}
