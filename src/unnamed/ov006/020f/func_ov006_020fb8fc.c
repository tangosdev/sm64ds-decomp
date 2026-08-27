void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6)
{
    int i = 0;
    char *p = c;
    do {
        if (*(unsigned char *)(p + 0x5964) == 0) {
            char *e = c + i * 0x14;
            *(int *)(e + 0x5958) = a2;
            *(int *)(e + 0x595c) = a3;
            *(unsigned char *)(e + 0x5964) = 1;
            *(unsigned char *)(e + 0x5966) = 1;
            *(unsigned short *)(e + 0x5960) = 0;
            *(unsigned char *)(e + 0x5965) = 0;
            *(unsigned char *)(e + 0x5967) = a4;
            *(unsigned short *)(e + 0x5962) = a5;
            *(unsigned char *)(e + 0x5968) = a6;
            return;
        }
        i++;
        p += 0x14;
    } while (i < 0x1e);
}
