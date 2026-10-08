extern int func_0205fb1c(int a, void *b);

int func_0205f8e0(int *a, int *b) {
    unsigned short flags;
    int r;
    r = func_0205fb1c(0, &flags);
    if (r) return r;
    if (a) *a = (flags & 8) ? 1 : 0;
    if (b) *b = (flags & 4) ? 1 : 0;
    return r;
}
