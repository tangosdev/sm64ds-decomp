extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
struct G { int w[2]; };
extern struct G data_ov002_0210eb78;

void func_ov002_020ec978(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0x300, (void *)data_ov002_0210eb78.w[1], 0, 0x1000, 0);
    *(int *)(p + 0x98) = 0;
    *(int *)(p + 0x9c) = -0x2000;
}
