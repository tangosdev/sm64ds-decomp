extern void func_ov096_0213585c(void *t);

void _ZN5Pokey16OnPendingDestroyEv(void *c) {
    int r1 = *(unsigned short *)((char *)c + 0xc);
    r1 = (r1 == 0xf1);
    if (r1) return;
    void *p = *(void **)((char *)c + 0x394);
    if (!p) return;
    do {
        func_ov096_0213585c(p);
        p = *(void **)((char *)p + 0x394);
    } while (p);
}
