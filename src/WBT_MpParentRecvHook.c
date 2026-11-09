extern void func_02065500(int a, int b, int c);

void WBT_MpParentRecvHook(int a, int b, int c) {
    if (a == 0) return;
    if (b == 0) return;
    if (b == 0xffff) return;
    func_02065500(c, a, b);
}
