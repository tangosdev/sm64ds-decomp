extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt();
extern int _ZN5Model12HideMaterialEii();
extern int _ZN5Model12ShowMaterialEii();
typedef struct { int a, b; } P2;
extern P2 data_ov006_0213aca0;

void func_ov006_020c0e8c(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x1c, t[0x8f], 0, 0x40000000, 0x800, 0);
    _ZN5Model12HideMaterialEii((char *)t + 0x1c, 2, 0);
    _ZN5Model12ShowMaterialEii((char *)t + 0x1c, 1, 0);
    *(P2 *)t = data_ov006_0213aca0;
}
