extern unsigned short data_020a0f14;
extern void *data_020a0f74;
extern int func_020631dc(void *a, int b, int c, int d, int e);

int func_0203fa50(void) {
    int flag = (data_020a0f14 <= 1) ? 1 : 0;
    int r = func_020631dc(data_020a0f74, 0xc, 0xf, 0x20, flag);
    if (r) return 0;
    return 1;
}
