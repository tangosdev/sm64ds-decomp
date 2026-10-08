void func_0204c24c(int a, int b)
{
    a = 0x1000 << a;
    b = 0x1000 << b;

    *(volatile int*)0x4000500 = 1;
    *(volatile int*)0x4000488 = 0;
    *(volatile int*)0x4000490 = 0x103c0;
    *(volatile int*)0x4000488 = (unsigned short)((int)(a << 8) >> 0x10);
    *(volatile int*)0x4000490 = 0x10040;
    *(volatile int*)0x4000488 = (unsigned short)((int)(a << 8) >> 0x10) | ((unsigned short)((int)(b << 8) >> 0x10) << 0x10);
    *(volatile int*)0x4000490 = 0xf0040;
    *(volatile int*)0x4000488 = (unsigned short)((int)(b << 8) >> 0x10) << 0x10;
    *(volatile int*)0x4000490 = 0xf03c0;
    *(volatile int*)0x4000504 = 0;
}
