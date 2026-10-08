extern unsigned short data_020a0f28;
extern unsigned char data_020a0f0c;

void func_0203e978(short *p)
{
    if (p[0] != 0)
        return;
    data_020a0f28 = *(unsigned short*)(p + 1);
    data_020a0f0c = 1;
}
