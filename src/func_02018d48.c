extern int func_0205d3d4(int a, int b, int c);
extern void func_02018e68(void *c);
extern void func_0205d4a0(int a, void *buf, int n);
extern void Crash(void);
extern char data_0208eb9c;

int func_02018d48(int a, int b, int c)
{
    char buf[0x100];
    int r = func_0205d3d4(a, b, c);
    if (r != c) {
        func_02018e68(&data_0208eb9c);
        func_0205d4a0(a, buf, 0x100);
        Crash();
    }
    return r;
}
