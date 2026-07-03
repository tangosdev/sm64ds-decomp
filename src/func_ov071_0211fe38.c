extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int a, int b, unsigned int u);
extern int func_0201267c(int a, void *b);
struct G { int w[2]; };
extern struct G data_ov071_02122f88;

int func_ov071_0211fe38(char *c)
{
    int *p3a0 = (int *)(((long long)(int)(c + 0x3a0)) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = 0xf000;
    *(int *)(c + 0xa8) = 0x12000;
    *(short *)(c + 0x8e) = *(short *)(c + 0x3a4);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *p3a0 += 0x12c000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov071_02122f88.w[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x2c00;
    *(int *)(c + 0x12c) = 0;
    func_0201267c(0xf1, c + 0x74);
    *(int *)(c + 0x39c) = 3;
    return 1;
}
