void func_ov006_02104558(char *p)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        *(unsigned char *)(p + 0x46b8) = 0;
        *(unsigned char *)(p + 0x46bd) = 0;
        p += 0x18;
    }
}
