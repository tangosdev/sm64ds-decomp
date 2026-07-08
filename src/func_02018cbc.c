extern void func_0205d874(void *s);
extern int func_02018e00(void *s, int b, int c, int d, int e);
extern int func_02018d48(void *s, void *b, int c);
extern int func_0205d4cc(void *self);

int func_02018cbc(char *self, int flag, int shift, int a3,
                  int a5, int a6, int a7, int a8) {
    char local[0x48];
    int off;
    int base;
    int hi;
    if (flag == 0) { hi = a6; base = a5; }
    else           { hi = a8; base = a7; }
    off = shift << 5;
    if ((unsigned int)off >= (unsigned int)hi) return 0;
    func_0205d874(local);
    func_02018e00(local, a3, base + off, base + hi, -1);
    func_02018d48(local, self, 0x20);
    func_0205d4cc(local);
    *(int *)(self + 0x20) = flag;
    return 1;
}
