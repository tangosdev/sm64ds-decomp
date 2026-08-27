extern int func_0200bec4(int a0, int a1, int a2, int a3, int a4);
extern int func_02009138(int a0, int a1);

int func_02005098(int param_1) {
    int ip = *(int *)(param_1 + 0x110);
    func_0200bec4(param_1,
                  ip + 0x5c,
                  *(int *)(ip + 0x644),
                  *(int *)(param_1 + 0x13c),
                  0x20000);
    func_02009138(param_1, 0x80000);
    return 1;
}
