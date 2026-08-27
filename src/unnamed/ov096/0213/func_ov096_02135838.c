int func_ov096_02135838(char *c) {
    int *r1 = *(int **)(c + 0x390);
    int r0 = 0;
    if (r1 == 0) return r0;
    do {
        r1 = *(int **)((char *)r1 + 0x390);
        r0 = r0 + 1;
    } while (r1 != 0);
    return r0;
}
