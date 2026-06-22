extern int func_0205fbd0(int a, int b, int c);

int func_0205f9e8(int a, int b, int c, int d)
{
    int v = 0;
    if (a == 0) {
        if (b == 1) v = 6;
        if (b == 0) v = 7;
    } else if (a == 1) {
        if (b == 1) v = 4;
        if (b == 0) v = 5;
    } else if (a == 2) {
        if (b == 1) v = 8;
        if (b == 0) v = 9;
    }
    if (v == 0) return 0xffff;
    return func_0205fbd0(v, c, d);
}
