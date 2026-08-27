extern short data_020a0e58[];

void func_0203bc50(void)
{
    short *p = data_020a0e58;
    int i;
    for (i = 0; i < 4; i++) {
        p[0] = 0;
        p[1] = 0;
        p += 2;
    }
}
