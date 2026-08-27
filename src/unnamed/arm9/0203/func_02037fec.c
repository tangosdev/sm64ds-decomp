extern void func_02037f5c(void *p, int v);

void func_02037fec(char *c, int p1, int p2, int p3, int p4)
{
    func_02037f5c(c, p1);
    *(int*)(c + 0x1c) = p2;
    *(int*)(c + 0x20) = p3;
    *(int*)(c + 0x24) = p4;
}
