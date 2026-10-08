extern int func_02065b94(void);
extern int func_02063d3c(int h, int z, int a, int b);
extern int func_02063e08(int h, int z, int a, int b);
extern int func_02063ae4(int h, int z, int a, int b);
extern int func_02063da0(int h, int z, int a, int b);
extern int func_02063bc4(int h, int z, int a, int b);
extern int func_02063980(int h, int a, int b);
extern int func_02063718(int h, int a, int b);
extern int func_02064fe8(int a, int x, int z);

int func_020653cc(int a, int b)
{
    int r;
    int h = func_02065b94();
    r = func_02063d3c(h, 0, a, b); if (r) return r;
    r = func_02063e08(h, 0, a, b); if (r) return r;
    r = func_02063ae4(h, 0, a, b); if (r) return r;
    r = func_02063da0(h, 0, a, b); if (r) return r;
    r = func_02063bc4(h, 0, a, b); if (r) return r;
    r = func_02063980(h, a, b); if (r) return r;
    r = func_02063718(h, a, b); if (r) return r;
    return func_02064fe8(a, 0xffff, 0);
}
