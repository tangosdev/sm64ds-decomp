void func_ov006_0210c218(unsigned char *p, short a, short b)
{
    *(unsigned short *)(p + 0) = a;
    *(unsigned short *)(p + 2) = b;
    *(int *)(p + 4) = 0x48;
    *(unsigned char *)(p + 8) = 1;
}
