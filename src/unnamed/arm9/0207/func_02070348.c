extern unsigned int data_0209a624;

unsigned int func_02070348(void)
{
    unsigned int s = data_0209a624 * 0x41c64e6d + 0x3039;
    data_0209a624 = s;
    return (s >> 16) & 0x7fff;
}
