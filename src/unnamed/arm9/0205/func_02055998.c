void func_02055998(int *m)
{
    *(volatile int *)0x4000440 = 3;
    *(volatile int *)0x4000458 = m[0] << 4;
    *(volatile int *)0x4000458 = m[1] << 4;
    *(volatile int *)0x4000458 = m[2] << 4;
    *(volatile int *)0x4000458 = m[3] << 4;
    *(volatile int *)0x4000458 = m[4] << 4;
    *(volatile int *)0x4000458 = m[5] << 4;
    *(volatile int *)0x4000458 = m[6] << 4;
    *(volatile int *)0x4000458 = m[7] << 4;
    *(volatile int *)0x4000458 = m[8] << 4;
    *(volatile int *)0x4000458 = m[9] << 4;
    *(volatile int *)0x4000458 = m[10] << 4;
    *(volatile int *)0x4000458 = m[11] << 4;
    *(volatile int *)0x4000458 = m[12];
    *(volatile int *)0x4000458 = m[13];
    *(volatile int *)0x4000458 = m[14];
    *(volatile int *)0x4000458 = m[15];
}
