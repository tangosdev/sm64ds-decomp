struct V2 { int x, y; };

extern void func_0205d874(int *s);
extern int func_02018dc4(int buf, struct V2 ab);
extern void Crash(void);
extern int func_0201834c(int *buf, int x);

int func_020182ec(int a, int b, int c, ...)
{
    int buf[0x11];
    int r = 0;
    func_0205d874(buf);
    if (func_02018dc4((int)buf, *(struct V2 *)&a) == 0)
        Crash();
    else
        r = func_0201834c(buf, c);
    return r;
}
