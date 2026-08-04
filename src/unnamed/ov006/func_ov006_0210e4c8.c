int func_ov006_0210e4c8(int (*c)[1])
{
    int i;
    for (i = 0; i < 8; i++) {
        if (c[i][0x1b] > 0x168)
            return 1;
    }
    return 0;
}
