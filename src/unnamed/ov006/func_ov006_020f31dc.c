extern void func_ov006_020f2790(int this);
extern int func_ov006_020f0d58(int this);
void func_ov006_020f31dc(int this) {
    int i;
    for (i = 0; i < 8; i++)
        func_ov006_020f2790(this);
    if (func_ov006_020f0d58(this) == 0)
        return;
    if (*(unsigned char*)(this + 0x5000 + 0x455) == 0)
        return;
    *(int*)(this + 0x4000 + 0xf78) = 2;
    *(unsigned char*)(this + 0x5000 + 0x458) = 0;
    *(short*)(this + 0x5100 + 0x6c) = 0xa;
    *(short*)(this + 0x5100 + 0x6e) = 0;
}
