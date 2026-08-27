extern int __aeabi_idiv(int a, int b);

int func_02052458(int *p) {
    int den = p[3];
    int num = p[2];
    if (num >= den) return p[1];
    {
        int lo = p[0];
        int d = p[1] - lo;
        return lo + num * d / den;
    }
}
