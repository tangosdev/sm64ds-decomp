void func_02060d98(unsigned int a, unsigned int b)
{
    while (*(volatile unsigned int*)0x40001a4 & 0x80000000)
        ;
    *(volatile unsigned char*)0x40001a1 = 0xc0;
    *(volatile unsigned char*)0x40001a8 = (unsigned char)(a >> 0x18);
    *(volatile unsigned char*)0x40001a9 = (unsigned char)(a >> 0x10);
    *(volatile unsigned char*)0x40001aa = (unsigned char)(a >> 8);
    *(volatile unsigned char*)0x40001ab = (unsigned char)a;
    *(volatile unsigned char*)0x40001ac = (unsigned char)(b >> 0x18);
    *(volatile unsigned char*)0x40001ad = (unsigned char)(b >> 0x10);
    *(volatile unsigned char*)0x40001ae = (unsigned char)(b >> 8);
    *(volatile unsigned char*)0x40001af = (unsigned char)b;
}
