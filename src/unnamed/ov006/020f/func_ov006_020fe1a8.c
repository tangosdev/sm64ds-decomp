void func_ov006_020fe1a8(char *p)
{
    int i;
    for (i = 0; i < 0x30; i++) {
        *(unsigned char *)(p + 0x4f0c) = 0;
        *(unsigned char *)(p + 0x4f0e) = 0;
        p += 0x38;
    }
}
