extern void func_ov006_02126b4c(char *c, int a, int b);

void func_ov006_02126a98(char *c)
{
    int flag = 1;
    int val = (*(int*)(c + 0xa000 + 0xb6c) >> 12) / 16 - 2;
    int i;
    if (val < 0) val = 0;
    for (i = 0; i < 0x10; i++) {
        func_ov006_02126b4c(c, val, flag);
        val++;
    }
    {
        int flag2 = 0;
        int k = flag2;
        int r8 = ((*(int*)(c + 0xa000 + 0xb6c) >> 12) + 0x110) / 16 - 2;
        for (; flag2 < 0x10; flag2++) {
            if (r8 >= *(int*)(c + 0xb000 + 0xa08)) return;
            func_ov006_02126b4c(c, r8, k);
            r8++;
        }
    }
}
