int func_ov084_0212f298(char *c)
{
    int v;
    *(unsigned char *)(c + 0x45c) = 0;
    v = *(int *)(c + 0x80);
    if (*(unsigned short *)(c + 0x100) == 0)
        v = 0x4cc;
    if (v < 0x1000)
    {
        v += 0x52;
    }
    else
    {
        *(int *)(c + 0x458) = 0;
        v = 0x1000;
    }
    *(int *)(c + 0x80) = v;
    *(int *)(c + 0x84) = v;
    *(int *)(c + 0x88) = v;
}
