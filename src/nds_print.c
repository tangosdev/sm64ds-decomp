extern void _Z13func_020147bcPti(unsigned short *dst, int glyph);

void nds_print(unsigned short *dst, const char *str) {
    while (*str != 0) {
        _Z13func_020147bcPti(dst++, *str++);
    }
}
