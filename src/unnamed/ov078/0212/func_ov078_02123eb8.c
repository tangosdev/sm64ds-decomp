extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
typedef struct { int a, b; } P2;
extern P2 data_ov078_02126f28;

int func_ov078_02123eb8(int *t)
{
    t[0x27] = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x2cc, data_ov078_02126f28.b, 0, 0, 0x1000, 0);
    *(short *)((char *)t + 0x100) = 0x32;
    t[0x26] = 0xa000;
    return 1;
}
