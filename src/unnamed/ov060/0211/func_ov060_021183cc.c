void func_ov060_021183cc(char *p, char *q)
{
    int b = (int)(*(unsigned short *)(q + 0xc) == 0x117);
    if (b != 0)
        *(unsigned char *)(p + 0x32b) = 1;
}
