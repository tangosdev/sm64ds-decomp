extern void func_020147bc(unsigned short* r0, int r1);

void nds_print(unsigned short* dst, volatile signed char* str) {
    while (*str != 0) {
        func_020147bc(dst, *str);
        ++dst;
        ++str;
    }
}
