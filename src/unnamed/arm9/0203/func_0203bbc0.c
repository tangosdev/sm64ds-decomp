extern void func_0205f270(void);
extern void func_0205f1e4(void *p);
extern void func_0205f0e0(void *p);
extern void func_0205edd8(int a, int b);
extern void func_0205ea10(int mask);
extern int func_0205e9fc(int mask);
extern void Crash(void);
extern void func_0205eeac(int a, int b, void *p, int d);
extern void *data_020a0de0;
extern void *data_020a0df8;

void func_0203bbc0(void)
{
    func_0205f270();
    func_0205f1e4(&data_020a0de0);
    func_0205f0e0(&data_020a0de0);
    func_0205edd8(3, 0x1e);
    func_0205ea10(8);
    if (func_0205e9fc(8) != 0)
        Crash();
    func_0205eeac(0, 4, &data_020a0df8, 9);
    func_0205ea10(2);
    if (func_0205e9fc(2) != 0)
        Crash();
}
