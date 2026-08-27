extern void func_ov004_020b1de8(int a, int b, int c, int d);
void func_ov004_020b52b8(char *c)
{
    if (*(unsigned char *)(c + 0x22) == 0)
        return;
    func_ov004_020b1de8(*(int *)(c + 8) >> 12, *(int *)(c + 0xc) >> 12, -1, -1);
}
