extern void func_ov070_02121c8c(void *t);
int func_ov070_02121eb0(void *c) {
    int r2 = *(int *)((char *)c + 0xb0);
    int r1 = (r2 & 0x20000) ? 1 : 0;
    if (r1 == 0) {
        r1 = (r2 & 0x40000) ? 1 : 0;
        if (r1 == 0)
            func_ov070_02121c8c(c);
    }
    return 1;
}
