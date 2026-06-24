extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *anim, void *file, int a, int b, int c, unsigned short u);
extern int RandomIntInternal(int *seed);
struct G { int w[2]; };
extern struct G data_ov075_0211d384;
extern int data_0209e650;

void func_ov075_02114904(char *c, int a1)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, (void *)data_ov075_0211d384.w[1], 8, 0x40000000, 0x1000, 0);
    *(unsigned char *)(c + 0x156) = (((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) + 0x1e) & 0xf;
    *(int *)(c + 0x130) = a1;
    *(int *)(c + 0x134) = 0;
    *(int *)(c + 0x138) = -0x3e8000;
    *(int *)(c + 0x110) = 2;
    *(int *)(c + 0x114) = 0;
}
