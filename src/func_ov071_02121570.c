extern void func_ov071_021209c8(char* c);
int func_ov071_02121570(char* c) {
    short v = *(short*)(c+0x8e) - *(short*)(c+0x20c);
    if (v >= 0) *(int*)(c+0x98) = 0xc8;
    else *(int*)(c+0x98) = -0xc8;
    func_ov071_021209c8(c);
    *(int*)(c+0x1ec) = 0;
    *(int*)(c+0x1e8) = 0;
    return 1;
}
