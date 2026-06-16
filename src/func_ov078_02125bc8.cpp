//cpp
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern int data_ov078_02126f30[];

extern "C" int func_ov078_02125bc8(char* c) {
    *(int*)(c+0x98) = 0;
    *(int*)(c+0x4fc) = 1;
    *(int*)(c+0x9c) = -0x2000;
    *(unsigned char*)(c+0x50a) = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x2cc, (void*)data_ov078_02126f30[1], 8, 0, 0x1000, 0);
    return 1;
}
