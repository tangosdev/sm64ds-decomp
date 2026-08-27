extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
typedef struct { int a, b; } P2;
extern P2 data_ov006_0213acb8;

void func_ov006_020c1164(int *t, int a1, int a2)
{
    *(short *)((char *)t + 0x1de) = a1;
    t[0x75] = a2;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x1c, t[0x87], 8, 0, 0x800, 0);
    *(P2 *)t = data_ov006_0213acb8;
}
