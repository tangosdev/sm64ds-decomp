void func_020647a4(char *self, int value)
{
    *(int *)(((long long)(int)(self + 0x10)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    *(int *)(self + 0xc) = value;

    int *arr = *(int **)(self + 0x14);
    int idx = value >> 5;
    int bit = value & 0x1f;
    arr[idx] |= (1 << bit);
}
