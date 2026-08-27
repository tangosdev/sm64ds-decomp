extern int func_02065b94(void);
extern int func_02063d3c(int h, int z, int a, int b);
extern int func_02063e08(int h, int z, int a, int b);
extern int func_02063ae4(int h, int z, int a, int b);
extern int func_02063da0(int h, int z, int a, int b);
extern int func_02063bc4(int h, int z, int a, int b);
extern int func_02063980(int h, int a, int b);
extern int func_02063718(int h, int a, int b);
extern int func_02064fe8(int a, int x, int z);

extern int data_0209a070;

int func_02065538(int a, int b)
{
    int r;
    int i;
    int h = func_02065b94();

    for (i = 1; i < 0x10; i++) {
        r = func_02063d3c(h, i, a, b);
        if (r) return r;
    }

    for (i = 1; i < 0x10; i++) {
        r = func_02063e08(h, i, a, b);
        if (r) return r;
    }

    for (i = 1; i < 0x10; i++) {
        r = func_02063ae4(h, i, a, b);
        if (r) return r;
    }

    for (i = 1; i < 0x10; i++) {
        r = func_02063da0(h, i, a, b);
        if (r) return r;
    }

    r = func_02063980(h, a, b);
    if (r) return r;

    r = func_02063718(h, a, b);
    if (r) return r;

    i = data_0209a070;
    do {
        i = i + 1;
        if (i > 0xf) {
            i = 1;
        }
        r = func_02063bc4(h, i, a, b);
        if (r) {
            data_0209a070 = i;
            return r;
        }
    } while (i != data_0209a070);

    return func_02064fe8(a, 0xfffe, 0);
}
