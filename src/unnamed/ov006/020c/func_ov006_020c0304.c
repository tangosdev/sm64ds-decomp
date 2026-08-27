extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
typedef struct { int a, b; } P2;
extern P2 data_ov006_0213ac70;

void func_ov006_020c0304(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x18, t[3], 8, 0, 0x800, 0);
    *(short *)((char *)t + 0xf2) = 0x78;
    t[0x3d] = 1;
    *(P2 *)((char *)t + 0xb4) = data_ov006_0213ac70;
}
