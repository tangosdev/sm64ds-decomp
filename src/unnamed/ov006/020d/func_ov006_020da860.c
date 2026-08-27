int func_ov006_020da860(char *p, int v)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (v != *(unsigned char *)(p + 0x2b))
            return 0;
        p += 0x30;
    }
    return 1;
}
