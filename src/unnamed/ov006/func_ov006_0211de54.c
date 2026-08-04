void func_ov006_0211de54(char *p)
{
    int i;
    for (i = 0; i < 0x10; i++) {
        *(unsigned char *)(p + 0x4a70) = 0;
        *(unsigned char *)(p + 0x4a71) = 0;
        p += 0x14;
    }
}
