int func_02065bd0(void);

int func_020659f8(int a) {
    int d = func_02065bd0();
    int n = (a % d != 0 ? 1 : 0) + a / d;
    int r = (n & 0x1f ? 1 : 0) + (n >> 5);
    return r << 2;
}
