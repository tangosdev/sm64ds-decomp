int func_ov096_02135838(char *c) {
    void *p = *(void**)(c + 0x390);
    int r0 = 0;
    if (p == 0) return r0;
    do {
        p = *(void**)((char*)p + 0x390);
        r0 = r0 + 1;
    } while (p != 0);
    return r0;
}
