extern void func_0205f958(void);
extern void func_02019ff4(void);
extern void func_0205f5c0(int a);
extern void func_0205fa78(int a);
extern int func_02040714(void);
extern void func_02040724(void);
extern void func_0201a4d0(void);
extern unsigned char data_0209d4e4;

void func_02019f64(void)
{
    int r;
    if (data_0209d4e4 != 0) {
        func_0205f958();
        func_02019ff4();
        return;
    }
    func_0205f5c0(3);
    func_0205fa78(1);
    r = func_02040714();
    if (r != 0 && r != 1) {
        do {
            func_02040724();
            func_0201a4d0();
            r = func_02040714();
            if (r == 0)
                break;
        } while (r != 1);
    }
    func_0205f5c0(1);
    func_0205fa78(1);
    func_02019ff4();
}
