extern void func_ov004_020b1cf0(void *a, short b, short c, int d, int e);
extern void *data_ov006_021346bc;
void func_ov004_020b42c0(char *c) {
    func_ov004_020b1cf0(c + 0x34, *(short*)(c + 0x10), *(short*)(c + 0x12), *(int*)(c + 0x1c), *(int*)(c + 0x18));
    if (*(int*)(c + 0x20) != 0 && *(int*)(c + 0x20) != 1 && *(int*)(c + 0x20) != 2)
        return;
    func_ov004_020b1cf0(data_ov006_021346bc, *(short*)(c + 0x10), *(short*)(c + 0x12), *(int*)(c + 0x1c), *(int*)(c + 0x18));
}
