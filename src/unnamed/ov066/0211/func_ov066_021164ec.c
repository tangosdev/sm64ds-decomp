extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
typedef struct { int a, b; } P2;
extern P2 data_ov066_0211ae64;
extern P2 data_ov066_0211ae44;
void func_ov066_021164ec(char *c)
{
    if (*(int*)(c + 0x498) != 0) return;
    if ((unsigned short)(*(int*)(c + 0x3b8) >> 0xc) != 0) return;
    *(int*)(((int)c + 0x33c)) |= 0x427f0;
    *(int*)(c + 0xb0) = 0x10000000;
    *(int*)(((int)c + 0x338)) |= 2;
    if (*(int*)(c + 0x49c) == 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char*)c + 0x360, data_ov066_0211ae64.b, 4, 0, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char*)c + 0x360, data_ov066_0211ae44.b, 4, 0, 0x1000, 0);
    }
    *(int*)(c + 0x498) = 1;
}
