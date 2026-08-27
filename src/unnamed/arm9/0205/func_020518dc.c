extern void *func_02050c14(void *a);
extern int func_02051574(void *a, void *b);

int func_020518dc(void *a, void *b) {
    void *p = func_02050c14(a);
    if (p == 0) return 0;
    return func_02051574(*(void **)p, b) != 0;
}
