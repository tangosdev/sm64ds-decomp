#include "types.h"

extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c719c(u32 a0, u32 a1);
extern void func_ov006_020c44b4(void *arg0, int arg1);
extern void func_ov006_020c72b4(void);
extern void func_ov006_020c72c8(void);
extern void func_ov006_020ee658(char *c);
extern int RandomIntInternal(int *seed);

extern int data_0209e650;
extern char *data_ov004_020beb68;
extern int data_ov006_0213cb48;
extern int data_ov006_02140328;

void func_ov006_020ee994(char *c, int a1)
{
    u32 v;

    if (a1 == 0) {
        u32 *p = (u32 *)(c + 0xbc);
        *p = *p + 1;
        if (*(u32 *)(c + 0xbc) > 0x270e)
            *(u32 *)(c + 0xbc) = 0x270e;
    } else if (a1 == 0x12) {
        *(u32 *)(c + 0xbc) = 0;
        if (*(u32 *)(c + 0xbc) > 0x270e)
            *(u32 *)(c + 0xbc) = 0x270e;
        if (data_ov004_020beb68 != 0)
            *(int *)(data_ov004_020beb68 + 0xb4) = 0;
        func_ov004_020adb1c(*(int *)(c + 0xb4));
    } else {
        u32 n = *(u32 *)(c + 0xbc);
        if (data_ov004_020beb68 != 0)
            *(u32 *)(data_ov004_020beb68 + 0xb4) = n;
    }

    if (*(u32 *)(c + 0xbc) != 0)
        func_ov006_020c72b4();
    else
        func_ov006_020c72c8();

    v = *(u32 *)(c + 0xbc);
    if (v >= 0xf) {
        do {
            v = (u32)(((int)(((u32)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 10) >> 0xc);
        } while (v == (u32)data_ov006_0213cb48);
        data_ov006_0213cb48 = v;
    }
    func_ov006_020c719c(*(u32 *)(c + 0xbc), v);

    if (*(u32 *)(c + 0xbc) < 0xf)
        data_ov006_02140328 = 3;
    else if (*(u32 *)(c + 0xbc) < 0x14)
        data_ov006_02140328 = 4;
    else if (*(u32 *)(c + 0xbc) < 0x19)
        data_ov006_02140328 = 5;
    else
        data_ov006_02140328 = 6;

    func_ov006_020c44b4((void *)*(u32 *)(c + 0xbc), v);
    func_ov006_020ee658(c);
}
