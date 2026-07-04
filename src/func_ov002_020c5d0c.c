typedef unsigned short u16;

extern int func_ov002_020c5dec(char *c, int a);

int func_ov002_020c5d0c(char *c)
{
    u16 *p;
    if (*(int *)(c + 0x60) - *(int *)(c + 0x644) >= 0x100000)
        return 0;
    p = (u16 *)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFFLL);
    *p |= 0x400;
    return func_ov002_020c5dec(c, 1);
}
