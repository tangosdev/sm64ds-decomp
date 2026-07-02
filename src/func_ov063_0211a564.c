typedef unsigned short u16;
extern void func_ov063_0211adfc(char *p);
extern void func_ov063_0211a810(void *p, int x);
extern void func_ov063_0211a76c(void *p, int x, int y);
extern void func_ov063_0211a718(void *p);
extern void func_ov063_0211a6f0(void *p);
extern short data_ov063_0211e7e0[];

int func_ov063_0211a564(char *c, int arg1)
{
    func_ov063_0211adfc(c);
    if (*(u16 *)(c + 0x100) == 0)
        func_ov063_0211a810(c, 1);
    {
        u16 v = *(u16 *)(c + 0x100);
        if (v < 0x20) {
            int result = ((arg1 * data_ov063_0211e7e0[v]) << 12) / 5000;
            func_ov063_0211a76c(c, 1, result);
            goto ret0;
        }
        if (v < 0x30) {
            func_ov063_0211a718(c);
            goto ret0;
        }
    }
    (*(int *)(((int)c + 0x19c) & 0xFFFFFFFFFFFFFFFF)) &= ~1;
    func_ov063_0211a6f0(c);
    *(unsigned char *)(c + 0x5cc) = 1;
    return 1;
ret0:
    return 0;
}
