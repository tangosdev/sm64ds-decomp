extern int *func_02065b94(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);

int func_020659a0(int a, int b) {
    int x = *func_02065b94();
    if (x != 4 && x != 6) goto work;
    return 0;
work:
    func_02065ba0(a - 0xe);
    func_02065bb0(b - 0xe);
    return 1;
}
