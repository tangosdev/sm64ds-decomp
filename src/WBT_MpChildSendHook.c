int func_020653cc(int a, int b);

int WBT_MpChildSendHook(int a, int b) {
    if (b <= 0)
        return a;
    if (a == 0)
        return a;
    return func_020653cc(a, b);
}
