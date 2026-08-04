int func_ov006_0210fa40(int (*c)[2])
{
    int i;
    for (i = 0; i < 3; i++) {
        if (c[i][0x14] > 0)
            return 0;
    }
    return 1;
}
