extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
struct G { int w[2]; };
extern struct G data_ov102_0214e9c0;

void func_ov102_0214c0b8(char* c)
{
    *(int*)(c + 0x3dc) = 0;
    if (*(unsigned char*)(c + 0x3f5) == 1)
        *(int*)(c + 0x98) = 0;
    else
        *(int*)(c + 0x98) = 0x5000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)data_ov102_0214e9c0.w[1], 0, 0x1000, 0);
    *(short*)(c + 0x3e8) = 0x200;
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0x390) = 0;
}
