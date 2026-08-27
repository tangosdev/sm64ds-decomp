extern void func_020541a4(void);
extern void func_02054190(void);
extern void func_0205417c(void);
extern void func_02054168(void);
extern void func_02054154(void);
extern void func_02054140(void);
extern void func_02054430(int x);
extern void func_02053f58(void);
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);

void func_ov007_020bd960(void)
{
    volatile int a;
    volatile int b;
    volatile int c;

    func_020541a4();
    func_02054190();
    func_0205417c();
    func_02054168();
    func_02054154();
    func_02054140();
    func_02054430(0x1ff);

    b = 0;
    MultiStore_Int(b, (int *)0x6800000, 0xa4000);

    func_02053f58();

    a = 0xc0;
    MultiStore32Bytes(a, (int *)0x7000000, 0x400);

    c = 0;
    MultiStore_Int(c, (int *)0x5000000, 0x400);
}
