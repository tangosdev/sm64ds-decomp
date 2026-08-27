void func_ov006_020e3b9c(char *p)
{
    int i;
    for (i = 0; i < 0x3c; i++) {
        *(unsigned char *)(p + 0x4ff4) = 0;
        *(unsigned char *)(p + 0x4ff5) = 0;
        p += 0x18;
    }
}
