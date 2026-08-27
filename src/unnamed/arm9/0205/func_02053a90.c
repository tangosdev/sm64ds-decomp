void func_02053a90(unsigned short *out, int v) {
    if (v == 0) {
        *out = 0;
        return;
    }
    if (v > 0) {
        *out = (unsigned short)(v | 0x4000);
    } else {
        *out = (unsigned short)(-v | 0x8000);
    }
}
