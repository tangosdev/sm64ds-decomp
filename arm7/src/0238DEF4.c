/* storage 0x0238DEF4 runtime 0x03805D8C linked 0x54 wram ARM.
 * Retry fn_02383024(10, a1, 0) until it is non-negative, then drop the lock. */

extern int fn_02383024(int a, int b, int c);
extern void fn_02388CD8(int arg);
extern int fn_0238176C(int arg);

int fn_0238DEF4(int a1) {
    while (fn_02383024(10, a1, 0) < 0)
        fn_02388CD8(0x100);
    return fn_0238176C(0x027D8074);
}
