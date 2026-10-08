void func_02071974(int arg);

int func_020719b4(int a, int base, int *pcount) {
    int count = *pcount;
    int i;
    for (i = 0; (unsigned)i < (unsigned)count; i++) {
        func_02071974(base + i);
    }
    return 0;
}
