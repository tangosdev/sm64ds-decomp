typedef int Fix12i;
typedef unsigned short u16;

extern int data_ov066_0211ae54[];
extern int data_ov066_0211ae3c[];
extern int data_ov066_0211ae94[];
extern int data_ov066_0211aebc[];

extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    void* self, void* ref, int a, int b, Fix12i f, u16 t);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void* self, void* ref, int a, Fix12i f, unsigned d);

void func_ov066_021165cc(char* c)
{
    if (*(int*)(c + 0x49c) == 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 0x360, (void*)data_ov066_0211ae54[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            c + 0x448, (void*)data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 0x360, (void*)data_ov066_0211ae94[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            c + 0x448, (void*)data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
    }
    *(int*)(c + 0x3bc) = 0x1000;
    {
        int* p = (int*)(((int)c + 0x33c) & 0xFFFFFFFFFFFFFFFF);
        *p = *p & 0xfffbd82f;
        *(int*)(c + 0xb0) = 0;
    }
}
