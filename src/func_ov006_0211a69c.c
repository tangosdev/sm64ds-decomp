void func_ov006_0211a69c(char *c, int i)
{
    int off = i * 0x24;
    char *pA = c + 0x51bc;
    char *pB = c + 0x51b4;
    int a = *(int *)(pA + off);
    int b = *(int *)(pB + off);
    *(int *)(pB + off) = b + a;
    a = *(int *)(pA + off);
    *(int *)(pA + off) = a + 0x400;
    {
        int d = *(int *)(c + off + 0x51c0) - *(int *)(pB + off);
        d >>= 12;
        if (d >= 0x20) {
            *(unsigned char *)(c + off + 0x51cc) = 0;
            *(unsigned char *)(c + off + 0x51cd) = 0;
        }
    }
}
