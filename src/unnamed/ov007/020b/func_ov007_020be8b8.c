extern int _ZN4cstd3divEii(int a, int b);
extern int func_02053200(int x);
extern char* data_ov007_02104bc0;

void func_ov007_020be8b8(void)
{
    int sl;
    for (sl = 0; sl < 0x10; sl++) {
        char* base = data_ov007_02104bc0 + 8;
        int* e = (int*)(base + sl * 0x10);
        int x = _ZN4cstd3divEii(sl * 0x1000, 0xf);
        int r3 = 0x1000 - x;
        long long m = (long long)r3 * 0x12c;
        int v = (int)((m + ((long long)x << 12)) >> 12);
        v = func_02053200(v);
        *(int*)(base + sl * 0x10) = v;
        e[1] = 0;
        e[2] = 0;
        e[3] = v;
    }
}
