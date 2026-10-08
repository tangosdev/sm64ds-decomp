extern int _ZN4cstd3divEii(int a, int b);

int func_ov007_020c2dfc(char* self, int idx, int v) {
    int q;
    unsigned short* s;
    int d;
    if (v >= 0 && idx >= 2 && idx < *(unsigned short*)(self + 8)) {
        q = (unsigned short)_ZN4cstd3divEii(v << 0x10, 0x168);
        s = *(unsigned short**)(self + 0x1c);
        d = (short)(s[idx] - s[idx - 1]);
        return d * d < q * q;
    }
    return 0;
}
