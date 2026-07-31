extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
struct G { int w[2]; };
extern struct G data_ov084_02130ce8;

void func_ov084_0212a6f8(char *c)
{
    *(int *)(c + 0x98) = 0;
    if (*(unsigned short *)(c + 0x400 + 0x54)) {
        *(unsigned short *)(((int)c + 0x454)) -= 1;
    }
    if (*(unsigned short *)(c + 0x400 + 0x54))
        return;
    *(int *)(c + 0x434) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, (void *)data_ov084_02130ce8.w[1], 0, 0x1000, 0);
}
