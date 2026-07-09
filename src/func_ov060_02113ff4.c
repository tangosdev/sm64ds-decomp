extern void func_ov060_02111cc0(void *c, int a, int b);
extern int func_ov060_02115a30(void *c);

int func_ov060_02113ff4(char *c, unsigned short arg1, int arg2)
{
    int r;
    unsigned char f = *(unsigned char *)(c + 0x423);
    if (f == 0) {
        func_ov060_02111cc0(c, 0x13, 0x40000000);
        if (func_ov060_02115a30(c) != 0) {
            unsigned char *p = (unsigned char *)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFF);
            *p = *p + 1;
        }
    } else if (f == 1) {
        func_ov060_02111cc0(c, 0x12, 0x40000000);
        if (func_ov060_02115a30(c) != 0) {
            unsigned char *p = (unsigned char *)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFF);
            *p = *p + 1;
        }
    } else {
        func_ov060_02111cc0(c, 0x10, 0);
    }
    r = 0;
    *(int *)(c + 0x98) = 0;
    {
        short *q = (short *)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);
        *q += arg2;
    }
    if (*(unsigned short *)(c + 0x3fc) >= arg1)
        r = 1;
    return r;
}
