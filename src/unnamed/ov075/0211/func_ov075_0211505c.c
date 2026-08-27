extern int func_0203da9c(void);
extern void func_ov075_02114a6c(void *, int);
void func_ov075_0211505c(char *c)
{
    int r;
    if (*(unsigned char *)(c + 0xf40) == 0) return;
    r = func_0203da9c();
    func_ov075_02114a6c((char *)(c + 0x920) + 0x158 * r, 0x158);
    *(unsigned char *)(c + 0xf40) = 0;
}
