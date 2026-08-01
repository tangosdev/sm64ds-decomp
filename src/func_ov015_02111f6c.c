extern void func_ov015_02111fb8(void *self, int idx);

void func_ov015_02111f6c(char *c)
{
    *(int *)(((int)c + 0x334)) -= 1;
    if (*(int *)(c + 0x334) > 0)
        return;

    func_ov015_02111fb8(c, 1);
}
