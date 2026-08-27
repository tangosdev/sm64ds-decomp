int func_ov006_021147d0(char *p)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        if (*(unsigned char*)(p + 0x59bc) == 1)
            return 1;
        p += 0x24;
    }
    return 0;
}
