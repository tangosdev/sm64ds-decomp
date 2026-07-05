extern int func_0205c5e4(void *self, int mode);

void func_0205c4e4(char *self, unsigned short value)
{
    *(int *)(((long long)(int)(self + 0xc)) & 0xFFFFFFFFFFFFFFFFLL) |= 4;
    *(int *)(self + 0x2c) = *(int *)(self + 8);
    *(int *)(self + 0x34) = 0;
    *(unsigned short *)(self + 0x32) = 0;
    *(unsigned short *)(self + 0x30) = value;
    func_0205c5e4(self, 2);
}
