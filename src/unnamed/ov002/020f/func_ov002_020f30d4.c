void func_ov002_020f30d4(char *p)
{
    int i;
    for (i = 0; i < 5; i++) {
        *(unsigned char *)(p + 0x170) = 0;
        *(unsigned char *)(p + 0x171) = 0;
        p += 0x14;
    }
}
