extern int func_0205f994(void *a, int *b);
extern void func_0205ff08(void);
extern void func_0205ff00(void);

int func_0205f958(void) {
    int local;
    int r = func_0205f994((void *)func_0205ff00, &local);
    if (r != 0) return r;
    func_0205ff08();
    return local;
}
