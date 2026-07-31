extern int func_0203da9c(void);
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];

int func_ov001_020ab450(char *c, int a1)
{
    int ret;
    int flag;
    int dx, dy;
    int v;

    if (*(unsigned char *)(c + 0x11) == 0)
        return 0;
    if (a1 < 0)
        a1 = func_0203da9c();
    ret = 0;
    flag = 0;
    if (data_020a0de8[a1 * 4] != 0) {
        int i4 = a1 * 4;
        dx = data_020a0dea[i4] - *(short *)(c + 4);
        dy = data_020a0deb[i4] - *(short *)(c + 6);
        if (dx >= -*(short *)(c + 8) && dx <= *(short *)(c + 8)
            && dy >= -*(short *)(c + 0xa) && dy <= *(short *)(c + 0xa))
            flag = 1;
    }
    if (flag != 0) {
        v = *(int *)(c + 0x14);
        if ((v == 1 && data_020a0de9[(unsigned int)a1 * 4] != 0) || (v == 2 && *(unsigned char *)(c + 0x12) == 0))
            ret = 1;
    }
    *(unsigned char *)(c + 0x12) = (unsigned char)flag;
    return ret;
}
