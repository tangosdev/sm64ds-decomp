int func_ov006_020da88c(char *p, int v)
{
    int count = 0;
    int i;
    for (i = 0; i < 5; i++) {
        if (v == *(unsigned char *)(p + 0x2b))
            count++;
        p += 0x30;
    }
    return count;
}
