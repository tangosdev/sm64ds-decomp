extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* anim, void* file, int a, int b, unsigned int u);
extern void* data_ov034_021138b0[];
#pragma opt_strength_reduction off
void func_ov034_02111bb0(char *p) {
    int i;
    char *anim = p + 0x110;
    *(int*)(p+0x8e4) = 0;
    for (i = 0; i < 5; i++) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, ((void**)data_ov034_021138b0[i])[1], 0x40000000, 0x1000, 0);
        int *f = (int*)(((int)(p + (i<<6)) + 0x490));
        *f |= 4;
        anim += 0x64;
    }
    if (*(unsigned char*)(p+0x8db) > 1) {
        unsigned char *q = (unsigned char*)(((int)p + 0x8db));
        *q = *q - 1;
    }
}
