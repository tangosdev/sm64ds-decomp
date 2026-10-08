extern int func_02065bd0(void);
extern int func_020659f8(int a);
extern void func_0205a588(int a, int b, int c);

void func_020647fc(int *out, int a, int b, int c)
{
    int d = func_02065bd0();
    int r;
    out[0] = a;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[1] = a / d + (a % d != 0);
    out[5] = b;
    out[6] = c;
    r = func_020659f8(a);
    func_0205a588(b, 0, r);
}
