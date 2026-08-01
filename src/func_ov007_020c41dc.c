extern void func_ov007_020c43bc(void *self, int arg);

void func_ov007_020c41dc(char *a0, char *a1, int a2)
{
    int *slot = (int *)(((int)a1 + 0x10));
    int addend = *(int *)(a0 + 0x10);
    int value = *slot;
    int old = *(int *)(a1 + 0x10);

    *slot = value + addend;
    func_ov007_020c43bc(a1, a2);
    *(int *)(a1 + 0x10) = old;
}
