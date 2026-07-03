extern void Math_Function_0203b14c(int *p, int target, int scale, int max, int extra);
extern void func_ov015_02111fb8(void *self, int idx);

void func_ov015_02111df4(char *c)
{
    Math_Function_0203b14c((int *)(c + 0x5c), *(int *)(c + 0x320), 0x800, 0xb4000, 0x28000);
    *(int *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFF) =
        *(int *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFF) - 1;
    if (*(int *)(c + 0x334) > 0) {
        return;
    }
    *(int *)(c + 0x5c) = *(int *)(c + 0x320);
    func_ov015_02111fb8(c, 5);
}