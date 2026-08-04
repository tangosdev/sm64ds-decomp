void func_ov006_020fa9a0(char *p)
{
    int i;
    for (i = 0; i < 3; i++) {
        *(unsigned char *)(p + 0x4e6c) = 0;
        *(unsigned char *)(p + 0x4e6d) = 0;
        p += 0x18;
    }
}
