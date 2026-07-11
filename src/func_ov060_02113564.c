extern void func_ov060_02111cc0(char *c, int a, int b);
extern void func_02012694(int a, void *p, int b);

void func_ov060_02113564(char *c)
{
    func_ov060_02111cc0(c, 0, 0x40000000);
    if (*(unsigned char *)(c + 0x414) == 2)
        *(int *)(c + 0x98) = -0x1c000;
    else
        *(int *)(c + 0x98) = -0x19000;
    *(int *)(c + 0xa8) = 0x50000;
    *(int *)(c + 0x9c) = -0x2000;
    *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x408) + 0x8000);
    *(short *)(c + 0x3fe) = 0;
    *(unsigned char *)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFF) += 1;
    *(int *)(c + 0x364) = 0xb4000;
    func_02012694(0xb1, (void *)(c + 0x74), 0xb4000);
}
