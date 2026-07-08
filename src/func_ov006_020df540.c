extern void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);

void func_ov006_020df540(char *c)
{
    *(int *)(((long long)(int)(c + 0x541c)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    if (*(int *)(c + 0x541c) > 0)
        return;
    func_ov004_020b0cac(0xf, 0x80, 0x38, 0, -1, 0xd);
    *(int *)(c + 0x5418) = 4;
}
