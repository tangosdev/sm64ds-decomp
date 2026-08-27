extern int func_02020254(void);
unsigned char func_0202027c(int bit) {
    return (func_02020254() >> bit) & 1;
}
