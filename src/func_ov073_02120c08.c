extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
typedef struct { int a, b; } P2;
extern P2 data_ov073_02123288;

int func_ov073_02120c08(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, ((int *)&data_ov073_02123288)[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x4000;
    t[0x26] = 0x14000;
    t[0x2a] = 0x1e000;
    *(short *)((char *)t + 0x8c) = 0;
    t[0x27] = -0x3000;
    t[0x12d] = 0;
    return 1;
}
