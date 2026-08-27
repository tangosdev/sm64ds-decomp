int func_02053274(int *a, int *b)
{
    volatile unsigned short *ime_reg = (volatile unsigned short *)0x4000208;
    volatile unsigned short *cnt = (volatile unsigned short *)0x40002b0;
    volatile unsigned int *param = (volatile unsigned int *)0x40002b8;
    int dx = a[0] - b[0];
    long long sq = (long long)dx * dx;
    int dy = a[1] - b[1];
    int dz;
    unsigned short ime;
    sq += (long long)dy * dy;
    dz = a[2] - b[2];
    sq += (long long)dz * dz;
    sq <<= 2;
    ime = *ime_reg;
    *ime_reg = 0;
    *cnt = 1;
    param[0] = (unsigned int)sq;
    param[1] = (unsigned int)(sq >> 32);
    *ime_reg;
    *ime_reg = ime;
    while (*cnt & 0x8000);
    return (*(volatile int *)0x40002b4 + 1) >> 1;
}
