void func_ov006_020e3c4c(char *c, int i)
{
    int idx = i * 0x24;
    if (*(unsigned short *)(c + 0x48d8 + idx) != 0) {
        short *p = (short *)(c + 0x48d8 + idx);
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (*(int *)(c + 0x48cc + idx) > 0x100) {
        int *q = (int *)(c + 0x48cc + idx);
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        *(unsigned char *)(c + 0x4000 + idx + 0x8df) = 0;
    }
}
