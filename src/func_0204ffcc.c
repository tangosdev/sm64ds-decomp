extern void func_0205ac28(void *a, int b, int c, int d);

void func_0204ffcc(char *self)
{
    func_0205ac28(*(void **)(self + 0x2c), 0, 1 << *(int *)(self + 0x28), 0);
    *(int *)(((long long)(int)(self + 0xc)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
}
