int func_0206e254(unsigned short *dst, unsigned char *src, int flag) {
    if (src == 0) return 0;
    if (flag == 0) return -1;
    if (dst != 0) *dst = *src;
    if (*src == 0) return 0;
    return 1;
}
