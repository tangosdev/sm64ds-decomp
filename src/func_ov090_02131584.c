extern int func_ov090_021314a0(char *c);
extern void func_ov090_02131e00(char *c, void *p);
extern int data_ov090_02134514;
extern int data_ov090_02134504;

int func_ov090_02131584(char *c)
{
    unsigned int v;
    *(int *)(c + 0x98) = 0;
    v = ((unsigned int)*(int *)(c + 0x364) << 4) >> 0x10;
    if (v >= 0x3b)
        *(int *)(((int)c + 0x390) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (*(int *)(c + 0x390) > 2 || func_ov090_021314a0(c) == 1)
        func_ov090_02131e00(c, &data_ov090_02134514);
    if (*(unsigned char *)(c + 0x39c) == 1)
        func_ov090_02131e00(c, &data_ov090_02134504);
    return 1;
}
