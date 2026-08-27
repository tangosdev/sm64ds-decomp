void func_0206a458(int *out)
{
    out[0] = (*(volatile unsigned short*)0x4000204 & 0xc) >> 2;
    out[1] = (*(volatile unsigned short*)0x4000204 & 0x10) >> 4;
    *(volatile unsigned short*)0x4000204 = (*(volatile unsigned short*)0x4000204 & ~0xc) | 0xc;
    *(volatile unsigned short*)0x4000204 = *(volatile unsigned short*)0x4000204 & ~0x10;
}
