extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
extern int *data_ov075_0211c678[];
int func_ov075_02114a6c(char *c) {
    unsigned char idx = *(unsigned char*)(c+0x152);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, (void*)data_ov075_0211c678[idx][1], 4, 0, 0x1000, 0);
    *(int*)(c+0x5c) = 0x1000;
    *(int*)(c+0x110) = 0;
    *(int*)(c+0x114) = 0;
    return 0;
}
