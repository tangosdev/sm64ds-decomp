extern int func_ov080_0212513c(char *c);
int func_ov080_02124e60(char *c) {
    int v = *(int*)(c + 0xb0);
    int b1 = (v & 0x20000) ? 1 : 0;
    if (b1 != 0) goto done;
    int b2 = (v & 0x40000) ? 1 : 0;
    if (b2 != 0) goto done;
    *(int*)(c + 0xd0) = 0;
    func_ov080_0212513c(c);
done:
    return 1;
}
