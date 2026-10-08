extern int func_020587dc(void);
extern void *func_0205adf8(void);
extern int func_0205af00(void);
extern int func_0205b274(int);
extern void func_0205b070(int);
extern void func_0205ae30(void);

void *func_0205b1d8(int arg)
{
    void *p;
    if (func_020587dc()) return 0;
    p = func_0205adf8();
    if (p) return p;
    if (arg == 0) return 0;
    if (func_0205af00() > 0) {
        while (func_0205b274(0) != 0) ;
        p = func_0205adf8();
        if (p) return p;
    } else {
        func_0205b070(1);
    }
    func_0205ae30();
    do {
        func_0205b274(1);
        p = func_0205adf8();
    } while (p == 0);
    return p;
}
