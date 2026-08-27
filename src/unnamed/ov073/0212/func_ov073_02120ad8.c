extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
extern int *data_ov073_02123288;

int func_ov073_02120ad8(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, (&data_ov073_02123288)[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x4000;
    *(short *)((char *)t + 0x8c) = 0;
    t[0x12d] = 0;
    t[0x27] = 0;
    t[0x26] = 0;
    t[0x29] = 0;
    t[0x2a] = 0;
    t[0x2b] = 0;
    t[0x134] = -0x1000;
    t[0x12a] = t[0x13b];
    t[0x12b] = t[0x13c];
    t[0x12c] = t[0x13d];
    t[0x140] = 0;
    *(short *)((char *)t + 0x100) = 0x5a;
    return 1;
}
