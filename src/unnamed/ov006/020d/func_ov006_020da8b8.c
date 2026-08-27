int func_ov006_020da8b8(char *p, int v)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (v == *(unsigned char *)(p + 0x2b))
            return i;
        p += 0x30;
    }
    return -1;
}
