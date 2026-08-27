typedef void (*fn_t)(void);

void func_02072f94(void) {
    fn_t *p = (fn_t *)0x2086b60;
    fn_t f;
    while (p != 0 && (f = *p) != 0) {
        f();
        p++;
    }
}
