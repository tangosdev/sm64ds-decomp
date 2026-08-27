extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
typedef struct { int a, b; } P2;
extern P2 data_ov073_02123288;
int func_ov073_0212122c(int *t)
{
    *(short *)((char *)t + 0x100) = 0;
    *(char *)((char *)t + 0x4c8) = 0;
    t[0x134] = 0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, data_ov073_02123288.b, 4, 0, 0x1000, 0);
    t[0xda] = 0x2000;
    return 1;
}
