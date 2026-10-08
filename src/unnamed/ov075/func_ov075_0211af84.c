extern void func_ov075_0211af0c(void *self);

void func_ov075_0211af84(char *self, int a, int b)
{
    *(int *)(self + 0x74) = a;
    *(int *)(self + 0x78) = b;
    *(int *)(self + 0x7c) = 0;
    func_ov075_0211af0c(self);
    *(short *)(self + 0xa2) = 0x64;
}
