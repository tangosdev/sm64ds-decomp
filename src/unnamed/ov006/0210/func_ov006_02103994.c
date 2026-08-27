extern int func_ov004_020adbc0(void);
extern void func_ov006_02100278(char *c, int r1, int r2, int r3);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_02102d6c(void *self, int i);

#pragma opt_common_subs off
void func_ov006_02103994(char *c, int i)
{
    *(int*)(c + 0x4660 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x668);
    *(int*)(c + 0x4664 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x66c);
    *(int*)(c + 0x4684 + i * 0x40) += 0x10;
    *(int*)(c + 0x466c + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x684);

    if (*(int*)(c + i * 0x40 + 0x4000 + 0x66c) >= 0x8000)
        *(int*)(c + i * 0x40 + 0x4000 + 0x66c) = 0x8000;

    if (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x96) != 0)
    {
        *(unsigned short*)(c + 0x4696 + i * 0x40) -= 1;

        if (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x96) == 0x20)
        {
            int r3 = func_ov004_020adbc0();
            int f660 = *(int*)(c + i * 0x40 + 0x4000 + 0x660);
            int f664 = *(int*)(c + i * 0x40 + 0x4000 + 0x664);
            func_ov006_02100278(c, f660, f664, r3 + 1);
            func_ov004_020adb1c(func_ov004_020adbc0() + 1);
        }
    }

    if ((*(int*)(c + i * 0x40 + 0x4000 + 0x664) >> 12) < 0xa8) return;

    func_ov006_02102d6c(c, i);
}
