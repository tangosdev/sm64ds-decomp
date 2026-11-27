extern int fn_0238D594(int, int);

int fn_02383F1C(int a1)
{
    if (a1 == 0x7F)
        return 0xFFFF;
    if (a1 == 0x7E)
        return 0x3C00;
    if (a1 >= 0x32)
        return (unsigned short)fn_0238D594(0x1E00, 0x7E - a1);
    return (unsigned short)(2 * a1 + 1);
}
