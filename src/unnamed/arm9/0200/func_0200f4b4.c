extern void func_020557b4();

void func_0200f4b4(void)
{
    func_020557b4();
    *(volatile unsigned *)0x04000440 = 3;
    *(volatile unsigned *)0x04000454 = 0;
    *(volatile unsigned *)0x04000440 = 2;
}
