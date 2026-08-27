extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
struct G { int w[2]; };
extern struct G data_ov085_021305c0;

int func_ov085_0212ad8c(char *c)
{
    int *a = (int *)(((int)c + 0x12c));
    int *b = (int *)(((int)c + 0x128));
    *(int *)(c + 0x9c) = -0x1000;
    *(unsigned char *)(c + 0x426) = 1;
    *a &= ~0x1000;
    *b |= 0x4000000;
    *(int *)(c + 0x114) = 0x78000;
    *a &= ~0x8000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)data_ov085_021305c0.w[1], 0, 0x1000, 0);
    return 1;
}
