void func_ov006_020faeec(char *p)
{
    int i;
    for (i = 0; i < 4; i++) {
        *(unsigned char *)(p + 0x5bd4) = 0;
        *(unsigned char *)(p + 0x5bd5) = 0;
        p += 0xc;
    }
}
