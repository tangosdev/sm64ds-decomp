extern void *func_02050b4c(void *a);
extern int func_020514b4(void *a, void *b);

int func_020518a0(void *a, void *b) {
    void *p = func_02050b4c(a);
    if (p == 0) return 0;
    return func_020514b4(*(void **)p, b) != 0;
}
