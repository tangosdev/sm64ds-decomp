void func_ov006_0210500c(char *c)
{
    *(int *)(((int)c + 0x4660)) += *(int *)(c + 0x4668);
    *(int *)(((int)c + 0x4668)) -= 0x400;
    if (*(int *)(c + 0x4660) >> 12 > 0x58) return;
    *(int *)(c + 0x4660) = 0x58000;
    *(unsigned char *)(c + 0x4675) = 2;
    *(unsigned short *)(c + 0x4670) = 0x80;
}
