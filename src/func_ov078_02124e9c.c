extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(char *anim, void *file, int a, int b, int c, unsigned short u);
extern void _ZN12WithMeshClsn15ClearGroundFlagEv(void *thiz);
struct G { int w[2]; };
extern struct G data_ov078_02126f20;

int func_ov078_02124e9c(char *c)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x2cc, (void *)data_ov078_02126f20.w[1], 0, 0, 0x1000, 0);
    if (*(int *)(c + 0x500) <= 0)
        *(int *)(c + 0xb0) = 0x10000000;
    _ZN12WithMeshClsn15ClearGroundFlagEv(c + 0x110);
    *(unsigned char *)(c + 0x499) = 0;
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa8) = 0x28000;
    *(int *)(c + 0x98) = 0x14000;
    *(int *)(c + 0xa0) = -0x3c000;
    return 1;
}
