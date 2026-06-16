extern void func_ov063_0211adfc(char *p);
void func_ov063_02119b84(char *c)
{
    func_ov063_0211adfc(c);
    *(unsigned char*)(c + 0x5cc) = 1;
    *(int*)(c + 0x584) = 0x1000;
    *(unsigned char*)(c + 0x5c9) = 0xff;
}
