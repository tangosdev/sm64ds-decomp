extern int data_02099fd0;
void func_02053d9c(void);
void DMASyncFillTransfer(int a0, int a1, int a2, int a3);
void MultiStore_Int(int val, int *dst, int len);

void func_02053c40(void)
{
    *(volatile unsigned short *)0x4000304 |= 0x8000;
    *(volatile unsigned short *)0x4000304 =
        (*(volatile unsigned short *)0x4000304 & 0xfffffdf1) | 0x20e;
    *(volatile unsigned short *)0x4000304 |= 1;

    func_02053d9c();

    *(volatile unsigned short *)0x4000004 = 0;
    *(volatile int *)0x4000000 = 0;

    if ((&data_02099fd0)[0] != -1) {
        DMASyncFillTransfer((&data_02099fd0)[0], 0x4000008, 0, 0x60);
        *(volatile unsigned short *)0x400006c = 0;
        DMASyncFillTransfer((&data_02099fd0)[0], 0x4001000, 0, 0x70);
    } else {
        volatile int a = 0;
        volatile int b;
        MultiStore_Int(a, (int *)0x4000008, 0x60);
        *(volatile unsigned short *)0x400006c = 0;
        b = 0;
        MultiStore_Int(b, (int *)0x4001000, 0x70);
    }

    *(volatile unsigned short *)0x4000020 = 0x100;
    *(volatile unsigned short *)0x4000026 = 0x100;
    *(volatile unsigned short *)0x4000030 = 0x100;
    *(volatile unsigned short *)0x4000036 = 0x100;
    *(volatile unsigned short *)0x4001020 = 0x100;
    *(volatile unsigned short *)0x4001026 = 0x100;
    *(volatile unsigned short *)0x4001030 = 0x100;
    *(volatile unsigned short *)0x4001036 = 0x100;
}
