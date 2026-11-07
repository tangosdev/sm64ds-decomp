int fn_023818C0(int a1, int a2) {
    int *p = (int *)(a1 << 2);
    int *q = p + 0x9C0000;
    q += 0x3FC00;
    q[0x368] = a2;
    return (int)q;
}
