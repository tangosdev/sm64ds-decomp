extern unsigned char data_ov002_020ff0f0[];
extern void func_02011f7c(int a);

void func_ov002_020e6330(unsigned char *self) {
    func_02011f7c(data_ov002_020ff0f0[self[0x6d9] & 3]);
}
