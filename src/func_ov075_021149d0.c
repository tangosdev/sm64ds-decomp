extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *anim, void *file, int a, int b, int cc, unsigned short u);
struct G { int w[2]; };
extern struct G data_ov075_0211d42c;

void func_ov075_021149d0(char *c, int a1)
{
    if (*(int *)(c + 0x124) == a1)
        return;
    *(int *)(c + 0x124) = a1;
    if (a1 >= *(int *)(c + 0x118))
        *(int *)(c + 0x13c) = 0x8000;
    else
        *(int *)(c + 0x13c) = -0x8000;
    *(int *)(c + 0x110) = 1;
    *(int *)(c + 0x114) = 1;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, (void *)data_ov075_0211d42c.w[1], 4, 0, 0x1000, 0);
    *(int *)(c + 0x5c) = 0x1000;
}
