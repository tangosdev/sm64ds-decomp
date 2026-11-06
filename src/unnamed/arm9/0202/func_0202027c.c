extern unsigned char func_02020254(void);
/* Bit `bit` of player 0's slot mask (func_02020254). */
unsigned char func_0202027c(int bit) {
    return (func_02020254() >> bit) & 1;
}
