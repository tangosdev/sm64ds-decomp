extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(char *anim, void *file, int a, int b, int c, unsigned short u);
extern int func_02012694(int a, void *b);
extern void _ZN12WithMeshClsn15ClearGroundFlagEv(void *thiz);
struct G { int w[2]; };
extern struct G data_ov078_02126f10;

int func_ov078_02124b40(char *c)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x2cc, (void *)data_ov078_02126f10.w[1], 0, 0x40000000, 0x1000, 0);
    func_02012694(0x129, c + 0x74);
    _ZN12WithMeshClsn15ClearGroundFlagEv(c + 0x110);
    *(short *)(c + 0x92) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
    *(unsigned char *)(c + 0x499) = 0;
    *(int *)(c + 0x9c) = -0x6000;
    return 1;
}
