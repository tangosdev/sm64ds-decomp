extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void* data_ov078_02126ee8[];

int func_ov078_02125734(char *c) {
    *(int*)(c + 0x4fc) = 1;
    *(int*)(c + 0x98) = 0x5000;
    *(int*)(c + 0x9c) = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x2cc, data_ov078_02126ee8[1], 8, 0, 0x1000, 0);
    return 1;
}
