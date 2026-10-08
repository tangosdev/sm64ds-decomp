extern int func_0205fb58(int a, int b, void *c, int *d);
extern void func_0205ff08(void);
extern void func_0205ff00(void);

int func_0205fb1c(int a, int b) {
    int local;
    int r = func_0205fb58(a, b, (void *)func_0205ff00, &local);
    if (r != 0) return r;
    func_0205ff08();
    return local;
}
