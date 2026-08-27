void func_ov006_02100058(unsigned char (*c)[0x20])
{
    int i;
    for (i = 0; i < 2; i++) {
        c[i][0x5634] = 0;
        c[i][0x5639] = 0;
        c[i][0x563b] = 0;
    }
}
