struct S { int a, b, w8; };
int func_ov084_0212ca60(struct S* p) {
    int r = 1;
    int b = r;
    int x = p->w8;
    if (x != 0xb26) {
        if (x != 0xb27)
            b = 0;
    }
    if (!b) {
        if (x != 0xb28)
            r = 0;
    }
    return r;
}
