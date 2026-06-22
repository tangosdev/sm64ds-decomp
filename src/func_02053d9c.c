typedef unsigned short u16;
extern u16 data_020a6088[];

void func_02053d9c(void)
{
    data_020a6088[0] = 0;
    data_020a6088[1] = 0;
    data_020a6088[2] = 0;
    data_020a6088[3] = 0;
    data_020a6088[4] = 0;
    data_020a6088[5] = 0;
    data_020a6088[6] = 0;
    data_020a6088[7] = 0;
    data_020a6088[8] = 0;
    data_020a6088[9] = 0;
    data_020a6088[10] = 0;
    data_020a6088[11] = 0;
    data_020a6088[12] = 0;

    *(volatile unsigned int *)0x4000240 = 0;
    *(volatile unsigned char *)0x4000244 = 0;
    *(volatile unsigned char *)0x4000245 = 0;
    *(volatile unsigned char *)0x4000246 = 0;
    *(volatile unsigned short *)0x4000248 = 0;
}
