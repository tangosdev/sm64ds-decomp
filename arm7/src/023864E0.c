int fn_023864E0(int a1, int a2, unsigned int a3) {
    int entry = *(int *)(a1 + 8 * a2 + 24);
    if (entry == 0)
        return 0;
    if (a3 >= *(unsigned int *)(entry + 56))
        return 0;
    return entry + *(int *)(entry + 4 * a3 + 60);
}
