extern int func_0205cdf4(void *self, int arg);

int func_0205d4cc(char *self)
{
    if (func_0205cdf4(self, 8) == 0)
        return 0;

    *(int *)(self + 8) = 0;
    *(int *)(self + 0x10) = 0xc;
    *(int *)(((long long)(int)(self + 0xc)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x30;
    return 1;
}
