void func_ov064_0211a49c(char *c)
{
    if (*(unsigned char *)(c + 0x173) != 0) {
        *(unsigned short *)(c + 0x170) = 0x2d;
    }

    *(int *)(((int)c + 0xb0)) &= ~1;
}
