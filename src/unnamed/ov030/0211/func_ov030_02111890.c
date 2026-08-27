extern int func_0201267c(int a, void *b);
extern int data_ov030_02115d18[];
void func_ov030_02111890(char *c)
{
    int b = (int)(*(int *)(c + 0x134) == data_ov030_02115d18[1]);
    if (b == 0)
        return;
    int v = (short)((unsigned int)*(int *)(c + 0x12c) << 4 >> 16);
    if (v == 0xa || v == 0xc)
        func_0201267c(0xea, c + 0x74);
}
