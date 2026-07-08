extern void func_ov002_020e63a4(void *self);

void func_ov002_020caf68(void *self) {
    *(unsigned int *)((char *)self + 0x37c) = 0;
    *(unsigned int *)(((long long)(int)((char *)self + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) &= ~4u;
    *(unsigned int *)(((long long)(int)((char *)self + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) |= 8u;
    func_ov002_020e63a4(self);
}
