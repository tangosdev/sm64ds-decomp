// NONMATCHING: geometry block register-coloring rotation, logic/control flow match (div=13).
extern int func_0203da9c(void);

extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];

int func_ov001_020ab450(char *self, int arg)
{
    int result;
    int ret;

    if (*(unsigned char *)(self + 0x11) == 0)
        return 0;
    if (arg < 0)
        arg = func_0203da9c();
    ret = 0;
    result = 0;
    if (data_020a0de8[arg][0] != 0) {
        int s8 = *(short *)(self + 8);
        int dx = data_020a0dea[arg][0] - *(short *)(self + 4);
        int dy = data_020a0deb[arg][0] - *(short *)(self + 6);

        if (dx >= -s8 && dx <= s8) {
            int sa = *(short *)(self + 0xa);

            if (dy >= -sa && dy <= sa)
                result = 1;
        }
    }
    if (result != 0) {
        int st = *(int *)(self + 0x14);

        if (st == 1) {
            if (data_020a0de9[arg][0] != 0)
                goto win;
        }
        if (st == 2) {
            if (*(unsigned char *)(self + 0x12) == 0)
                goto win;
        }
    }
    goto done;
win:
    ret = 1;
done:
    *(unsigned char *)(self + 0x12) = (unsigned char)result;
    return ret;
}
