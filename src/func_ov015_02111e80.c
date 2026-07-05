extern void func_ov015_02111fb8(void *self, int idx);
void func_ov015_02111e80(int *c)
{
    int *p = (int *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFF);
    *p = *p - 1;
    if (*(int *)((int)c + 0x334) > 0)
        return;
    if (*(int *)((int)c + 0x32c) == 0)
        func_ov015_02111fb8(c, 3);
    else
        func_ov015_02111fb8(c, 4);
}
