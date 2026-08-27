extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
extern int data_ov085_021305b8[];

int func_ov085_0212b3fc(char *c) {
    *(int*)(c + 0x41c) = 0;
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0x114) = 0x28000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)data_ov085_021305b8[1], 0, 0x1000, 0);
    return 1;
}
