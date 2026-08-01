int func_ov072_02120308(char *base)
{
    *(int *)(((int)base + 0xb0)) &= ~1;
    *(int *)(base + 0x9c) = -0x2000;
    *(int *)(base + 0xa0) = -0x3c000;
    *(int *)(base + 0xa8) = 0x2d000;
    *(unsigned char *)(base + 0x335) = 0x78;
    *(unsigned char *)(base + 0x334) = 0;
    *(int *)(base + 0x330) = 2;
    return 1;
}
