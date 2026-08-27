extern int func_0205f5fc(int a, void *b, int *c);
extern void func_0205ff08(void);
extern void func_0205ff00(void);

int func_0205f5c0(int a) {
    int local;
    int r = func_0205f5fc(a, (void *)func_0205ff00, &local);
    if (r != 0) return r;
    func_0205ff08();
    return local;
}
