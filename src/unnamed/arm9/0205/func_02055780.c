extern void func_020553c0(unsigned addr);
void func_02055780(void)
{
    func_020553c0(0x4000400);
    while (*(volatile unsigned *)0x4000600 & 0x8000000)
        ;
}
