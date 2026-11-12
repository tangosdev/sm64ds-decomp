extern void func_020147bc(unsigned short *dst, int glyph);

void nds_print(unsigned short *dst, const char *str) {
    while (*str != 0) {
        func_020147bc(dst++, *str++);
    }
}
